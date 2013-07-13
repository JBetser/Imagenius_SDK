#include "stdafx.h"
#include <atlbase.h>
#include <ShlObj.h>
#include "ximaDZthread.h"
#include "ximage.h"
#include "ximaDZ.h"

CxImageDZThreadSampler::CxImageDZThreadSampler () : m_nProgress (100)
{
}

CxImageDZThreadSampler::~CxImageDZThreadSampler(void)
{
}

////////////////////////////////////////////////////////////////////////////////
/**
 * DeepZoom config - To be called before Thread::Start
	@nNbCroppingAndSavingSubThreads : nb sub threads by sampling threads
 */
void CxImageDZThreadSampler::SetNbDZThreads (int nNbSamplingThreads, int nNbCroppingAndSavingSubThreads)
{
	_ASSERTE ((nNbSamplingThreads == 0) && (nNbCroppingAndSavingSubThreads > 0) && (nNbCroppingAndSavingSubThreads <= CXIMAGE_MAX_DZTHREADS) && L"CxImageDZThreadSampler::SetNbDZThreads error: invalid number of threads");
	if (!((nNbSamplingThreads == 0) && (nNbCroppingAndSavingSubThreads > 0) && (nNbCroppingAndSavingSubThreads <= CXIMAGE_MAX_DZTHREADS)))
		return;
	info.nNbDZThreads = nNbSamplingThreads;
	info.nNbDZSubThreads = nNbCroppingAndSavingSubThreads;
}

bool CxImageDZThreadSampler::Start()
{
	// Start all DeepZoom threads
	int nNbThreads = info.nNbDZSubThreads;
	_ASSERTE (((nNbThreads > 0) && (nNbThreads <= CXIMAGE_MAX_DZTHREADS)) && L"CxImageDZThreadSampler::Start error: invalid number of threads");
	if ((nNbThreads <= 0) || (nNbThreads > CXIMAGE_MAX_DZTHREADS))
		return false;
	while (nNbThreads--)
	{
		CxImageDZCropAndSaveThread *pDZThread = new CxImageDZCropAndSaveThread();
		if (!pDZThread->Start())
			return false;
		vDZThreads.push_back (pDZThread);
	}
	// Start the sampling thread
	return IGLibrary::IGThread::Start();
}

bool CxImageDZThreadSampler::Exit()
{
	// Kill the sampling thread
	bool bSuccess = IGLibrary::IGThread::Exit();
	// Kill all DeepZoom threads
	while (!vDZThreads.empty())
	{
		CxImageThread *pDZThread = vDZThreads[vDZThreads.size()-1];
		bSuccess = bSuccess && pDZThread->Exit();
		delete pDZThread;
		vDZThreads.pop_back();
	}
	return bSuccess;
}

void CxImageDZThreadSampler::ThreadProc (IGLibrary::IGThreadMessage *pMessage)
{
	IGDZSampleMessage *pIGThreadMessage = dynamic_cast <IGDZSampleMessage *> (pMessage);
	_ASSERTE (pIGThreadMessage && L"CxImageDZThreadSampler error: Wrong IGThread message, image processing aborted");
	if (!pIGThreadMessage)
		return;
	IGDZMonitor *pMonitor = pIGThreadMessage->GetMonitor();
	_ASSERTE (pMonitor && L"CxImageDZThreadSampler error: No monitor");
	if (!pMonitor)
		return;
	_ASSERTE (!pMonitor->IsWorkFinished() && L"DZ Work should be finished");

	int nCurLevel = pIGThreadMessage->GetLevel();
	
	// Create the sampling level folder
	std::wstring wsLevelFolder (pMonitor->GetRootFolder());
	wchar_t wFolder[8];
	int nRes = ::_itow_s (nCurLevel, wFolder, 10);
	_ASSERTE ((nRes == 0) && L"CxImageDZThreadSampler error during folder creation");
	if (nRes != 0)
		return;
	wsLevelFolder += wFolder;
	nRes = ::SHCreateDirectory (NULL, wsLevelFolder.c_str());
	_ASSERTE (((nRes == ERROR_SUCCESS) || (nRes == ERROR_FILE_EXISTS) || (nRes == ERROR_ALREADY_EXISTS)) && L"CxImageDZThreadSampler error during folder creation");
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return;

	// Create a new tile, to be managed by DZ monitor
	IGDZTile *pTile = new IGDZTile (pIGThreadMessage->GetWidth(), pIGThreadMessage->GetHeight(), pIGThreadMessage->GetLevel());
	
	//Preparing Bitmap Info Src And Dst
	const BYTE *pSrcBits;
	int nSrcWidth, nSrcHeight;
	pIGThreadMessage->GetMonitor()->GetSampledImageBits (nCurLevel + 1, pSrcBits, nSrcWidth, nSrcHeight);
		
	// no stretch needed in case of the first picture
	if (pIGThreadMessage->GetMonitor()->IsEmpty())
		pTile->Copy (pSrcBits);
	else
		pTile->Resample (pSrcBits, nSrcWidth, nSrcHeight);
	
	// notify end of processing to DZ monitor
	pMonitor->NotifySamplingTaskEnded (pTile);

	// post next DZ sampling task
	RECT rcUpdate = pIGThreadMessage->GetUpdateRect();
	RECT rcLastUpdate = pIGThreadMessage->GetLastUpdateRect();
	if (nCurLevel > 0)
	{
		RECT rcSrc = {0, 0, pIGThreadMessage->GetWidth(), pIGThreadMessage->GetHeight()};
		RECT rcNext = rcSrc;
		rcNext.right >>= 1;
		if (rcNext.right == 0)
			rcNext.right = 1;
		rcNext.bottom >>= 1;
		if (rcNext.bottom == 0)
			rcNext.bottom = 1;
		RECT rcNextUpdate = rcUpdate;
		rcNextUpdate.left >>= 1; rcNextUpdate.right >>= 1;
		rcNextUpdate.top >>= 1; rcNextUpdate.bottom >>= 1;
		RECT rcNextLastUpdate = rcLastUpdate;
		rcNextLastUpdate.left >>= 1; rcNextLastUpdate.right >>= 1;
		rcNextLastUpdate.top >>= 1; rcNextLastUpdate.bottom >>= 1;
		pMonitor->PostTask (new IGDZSampleMessage (pMonitor, rcNext, rcSrc, rcNextUpdate, rcNextLastUpdate, nCurLevel - 1));
	}

	// post all DZ cropping&saving tasks for current level
	RECT rcCurrentTile = {0, 0, CXIMAGE_DZ_DEFAULT_TILESIZE, CXIMAGE_DZ_DEFAULT_TILESIZE};
	RECT rcCurrentSrc = {0, 0, pIGThreadMessage->GetWidth(), pIGThreadMessage->GetHeight()};
	int nNumTileCol = 0;
	int nNumTileRow = 0;
	double dRatio = (double)pIGThreadMessage->GetWidth() / (double)pIGThreadMessage->GetHeight();
	int nTileSize = dRatio > 1 ? pIGThreadMessage->GetWidth() : pIGThreadMessage->GetHeight();
	std::vector <CxImageDZThreadSampler*>::iterator it = vDZThreads.begin();
	do
	{
		if (isUpdateNeeded (rcCurrentTile, rcUpdate, rcLastUpdate))
		{
			// construct the current tile path
			std::wstring wsCurTilePath (pMonitor->GetRootFolder());
			wchar_t twNumLevel[4];
			::_itow_s (nCurLevel, twNumLevel, 10);
			wsCurTilePath += twNumLevel;
			wsCurTilePath += L"/";
			wchar_t twNumTileRow[4], twNumTileCol[4];
			::_itow_s (nNumTileCol, twNumTileCol, 10);
			wsCurTilePath += twNumTileCol;
			wsCurTilePath += L"_";
			::_itow_s (nNumTileRow, twNumTileRow, 10);
			wsCurTilePath += twNumTileRow;
			wsCurTilePath += L".jpg";

			// post next DZ cropping&saving task to next DZ cropping&saving thread
			(*it)->PostMsg (new IGDZCropAndSaveMessage (pMonitor, rcCurrentTile, rcCurrentSrc, nCurLevel, wsCurTilePath.c_str()));
			if (++it == vDZThreads.end())
				it = vDZThreads.begin();
		}
		else
		{
			// cancel the crop task if it is out of bounds
			pMonitor->NotifyCroppingTaskEnded();
		}

		// create next tile info
		nNumTileCol++;
		rcCurrentTile.left += CXIMAGE_DZ_DEFAULT_TILESIZE;
		rcCurrentTile.right += CXIMAGE_DZ_DEFAULT_TILESIZE;
		if (rcCurrentTile.left >= rcCurrentSrc.right)
		{
			// go to next row
			nNumTileCol = 0;
			nNumTileRow++;
			rcCurrentTile.left = 0;
			rcCurrentTile.right = CXIMAGE_DZ_DEFAULT_TILESIZE;
			rcCurrentTile.top += CXIMAGE_DZ_DEFAULT_TILESIZE;
			rcCurrentTile.bottom += CXIMAGE_DZ_DEFAULT_TILESIZE;
		}
	}
	while (rcCurrentTile.top < rcCurrentSrc.bottom);
}

bool CxImageDZThreadSampler::isUpdateNeeded (const RECT& rcTile, const RECT& rcUpdate, const RECT& rcLastUpdate) const
{
	return ((rcUpdate.right == -1) || (rcUpdate.bottom == -1) || 
			((rcUpdate.left <= rcTile.right) &&
			(rcUpdate.right >= rcTile.left) &&
			(rcUpdate.top <= rcTile.bottom) &&
			(rcUpdate.bottom >= rcTile.top)) || 
			((rcLastUpdate.right != -1) &&
			(rcLastUpdate.bottom != -1) &&
			(rcLastUpdate.left <= rcTile.right) &&
			(rcLastUpdate.right >= rcTile.left) &&
			(rcLastUpdate.top <= rcTile.bottom) &&
			(rcLastUpdate.bottom >= rcTile.top)));
}

CxImageDZCropAndSaveThread::CxImageDZCropAndSaveThread ()
{
	info.nNbDZThreads = 0;	// this a leaf in the tree structure of DZ thread objects
}

CxImageDZCropAndSaveThread::~CxImageDZCropAndSaveThread()
{
}

bool CxImageDZCropAndSaveThread::Start()
{
	// Start the cropping thread
	return IGLibrary::IGThread::Start();
}

bool CxImageDZCropAndSaveThread::Exit()
{
	// Kill the cropping thread
	return IGLibrary::IGThread::Exit();
}

void CxImageDZCropAndSaveThread::ThreadProc (IGLibrary::IGThreadMessage *pMessage)
{
	IGDZCropAndSaveMessage *pIGThreadMessage = dynamic_cast <IGDZCropAndSaveMessage *> (pMessage);
	_ASSERTE (pIGThreadMessage && L"Wrong IGThread message, image processing aborted");
	if (!pIGThreadMessage)
		return;
	_ASSERTE (!pIGThreadMessage->GetMonitor()->IsWorkFinished() && L"DZ Work should be finished");

	//Preparing Bitmap Info Src And Dst
	const BYTE *pSrcBits;
	int nSrcWidth, nSrcHeight;
	pIGThreadMessage->GetMonitor()->GetSampledImageBits (pIGThreadMessage->GetLevel(), pSrcBits, nSrcWidth, nSrcHeight);
	RECT rcCrop = pIGThreadMessage->GetCropRect();
	BITMAPINFO bmInfoSrc, bmInfoDst;
	memset(&bmInfoSrc.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	memset(&bmInfoDst.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	bmInfoSrc.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoSrc.bmiHeader.biWidth = nSrcWidth;
	bmInfoSrc.bmiHeader.biHeight = nSrcHeight;
	bmInfoSrc.bmiHeader.biPlanes = 1;
	bmInfoSrc.bmiHeader.biBitCount = 24;
	bmInfoSrc.bmiHeader.biCompression = BI_RGB;
	bmInfoDst.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfoDst.bmiHeader.biPlanes = 1;
	bmInfoDst.bmiHeader.biBitCount = 24;
	bmInfoDst.bmiHeader.biCompression = BI_RGB;
	bmInfoDst.bmiHeader.biWidth = rcCrop.right - rcCrop.left;
	bmInfoDst.bmiHeader.biHeight = rcCrop.bottom - rcCrop.top;
	if (rcCrop.left + bmInfoDst.bmiHeader.biWidth > nSrcWidth)
		bmInfoDst.bmiHeader.biWidth = nSrcWidth - rcCrop.left;
	if (rcCrop.top + bmInfoDst.bmiHeader.biHeight > nSrcHeight)
		bmInfoDst.bmiHeader.biHeight = nSrcHeight - rcCrop.top;

	//WIN32 cropping
	HDC hDstDC = ::CreateCompatibleDC (::GetDC (NULL));
	HDC hSrcDC = ::CreateCompatibleDC (::GetDC (NULL));
	void *pSrcDib;
	HBITMAP hBmpSrc = ::CreateDIBSection (hSrcDC, &bmInfoSrc, DIB_RGB_COLORS, (void**)&pSrcDib, 0, 0);
	HGDIOBJ hObjSrc = ::SelectObject (hSrcDC, hBmpSrc);
	::SetDIBits (hSrcDC, hBmpSrc, 0, bmInfoSrc.bmiHeader.biHeight, pSrcBits, &bmInfoSrc, DIB_RGB_COLORS);
	void *pCropDib;
	HBITMAP hBmpDst = ::CreateDIBSection (hDstDC, &bmInfoDst, DIB_RGB_COLORS, (void**)&pCropDib, 0, 0);
	HGDIOBJ hObjDst = ::SelectObject (hDstDC, hBmpDst);
	::BitBlt (hDstDC, 0, 0, bmInfoDst.bmiHeader.biWidth, bmInfoDst.bmiHeader.biHeight, hSrcDC, rcCrop.left, rcCrop.top, SRCCOPY);

	CxImage outputTile;
	outputTile.CreateFromHBITMAP (hBmpDst, NULL, false);
	outputTile.Save (pIGThreadMessage->GetPath(), CXIMAGE_FORMAT_JPG);

	::SelectObject (hDstDC, hObjDst);
	::SelectObject (hSrcDC, hObjSrc);
	::DeleteObject (hBmpSrc);
	::DeleteObject (hBmpDst);
	::DeleteDC (hSrcDC);
	::DeleteDC (hDstDC);

	// notify end of processing to DZ monitor
	pIGThreadMessage->GetMonitor()->NotifyCroppingTaskEnded();
}