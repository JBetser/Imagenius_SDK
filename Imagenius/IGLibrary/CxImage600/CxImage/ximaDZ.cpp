/*
 * File:	ximaDZ.cpp
 * Purpose:	multithreaded generation of microsoft DeepZoom file format
 * 07/Oct/2010 Jonathan Betser
 */
#include "stdafx.h"
#include <atlbase.h>
#include "ximaDZ.h"
#include "ximaDZthread.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <windowsx.h>
#include <commctrl.h>

#if CXIMAGE_SUPPORT_DZ
#include "../../IGFrameHistory.h"
#include "../../IGImageProcessing.h"
#include "ximaiter.h"

#define CXIMAGEZ_ROOTFOLDER_POSTFIX		L"_files"

////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
bool CxImageDZ::Encode(CxFile *hFile)
{
	if ((pDib==NULL)||(info.hProgress==NULL)){
		strcpy(info.tcLastError,CXIMAGE_ERR_NOIMAGE);
		return false;
	}
	if (vDZThreads.size() == 0){
		strcpy(info.tcLastError,CXIMAGE_ERR_NODZTHREAD);
		return false;
	}	

	// create root folder
	int nLastOuputSize = ::wcslen (info.twLastOutputPath);
	std::wstring wsDZRootFolder;

	if (info.bFullDZUpdate){
		int nRes = ERROR_SUCCESS;
		// Create the output DZ root folder 
		wsDZRootFolder = info.twOutputPath;
		wsDZRootFolder += CXIMAGEZ_ROOTFOLDER_POSTFIX;
		nRes = ::SHCreateDirectory (NULL, wsDZRootFolder.c_str());
		if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
			return false;
	}
	else
	{
		_ASSERTE ((nLastOuputSize > 0) && L"CxImageDZ::Encode error: cannot make a partial update on a new DZ path");
		if (nLastOuputSize <= 0)
		{
			strcpy(info.tcLastError,CXIMAGE_ERR_BADPATH);
			return false;
		}
		wsDZRootFolder = info.twLastOutputPath;
		wsDZRootFolder += CXIMAGEZ_ROOTFOLDER_POSTFIX;	
	}
	wsDZRootFolder += L"/";

	// Create monitor
	double dRatio = (double)head.biWidth / (double)head.biHeight;
	int nTileSize = dRatio > 1 ? head.biWidth : head.biHeight;
	double dCurSz = (double)nTileSize;
	int nLevel = 0;
	while (dCurSz > 1.0)
	{
		dCurSz /= 2.0;
		nLevel++;
	}
	if (nLevel == 0)
		return false;
	IGDZMonitor monitor (vDZThreads, GetBits(), head.biWidth, head.biHeight, ::GetCurrentThreadId(), GetProgressBar(), wsDZRootFolder);

	// Create and post the first sampling task (original image)
	RECT rcFirst = {0, 0, head.biWidth, head.biHeight};
	RECT rcUpdate = info.rcUpdateBox;
	if (info.bFullDZUpdate)
	{
		rcUpdate.right = -1;
		rcUpdate.bottom = -1;
	}
	monitor.PostTask (new IGDZSampleMessage (&monitor, rcFirst, rcFirst, rcUpdate, info.rcLastUpdateBox, nLevel));

	// DZ message loop:
	MSG msg;
	while (::GetMessageW (&msg, NULL, 0, 0))
	{
		if (msg.message == UM_DEEPZOOM_ENDED)
			break;	// end of DZ generation
		else if (msg.hwnd)
			::DispatchMessage(&msg);
	}

	if (!info.bFullDZUpdate)
	{
		// rename the previously created output folder
		std::wstring wsNewRootFolder (info.twOutputPath);
		wsNewRootFolder += CXIMAGEZ_ROOTFOLDER_POSTFIX;
		if (_wrename (wsDZRootFolder.c_str(), wsNewRootFolder.c_str()) != 0)
			return false;
	}
	return TRUE;
}

IGDZMonitor::IGDZMonitor (std::vector <CxImageDZThreadSampler*> vSamplingThreads, void *pBits, int nWidth, int nHeight, int nParentThreadId, HWND hProgress, const std::wstring& wsDZRootFolder)	
																																			: m_nNbSteps (0)
																																			, m_pOriginalBits (pBits)
																																			, m_nParentThreadId (nParentThreadId)
																																			, m_hProgressBar (hProgress)
																																			, m_wsDZRootFolder (wsDZRootFolder)
																																			, m_vpSamplingThreads (vSamplingThreads)
{
	double dRatio = (double)nWidth / (double)nHeight;
	double dTileMaxSize = (dRatio > 1.0) ? (double)nWidth : (double)nHeight;
	double dTileMinSize = (dRatio <= 1.0) ? (double)nWidth : (double)nHeight;
	int nNbSampleSteps = 1;
	int nNbCropSteps = 1;
	while (dTileMaxSize > 1.0)
	{
		double dCurNbTilesMax = dTileMaxSize / (double)CXIMAGE_DZ_DEFAULT_TILESIZE;
		int nCurNbTilesMax = (int)dCurNbTilesMax;
		if (dCurNbTilesMax > (double)nCurNbTilesMax)
			nCurNbTilesMax++;
		double dCurNbTilesMin = dTileMinSize / (double)CXIMAGE_DZ_DEFAULT_TILESIZE;
		int nCurNbTilesMin = (int)dCurNbTilesMin;
		if (dCurNbTilesMin > (double)nCurNbTilesMin)
			nCurNbTilesMin++;
		nNbCropSteps += nCurNbTilesMin * nCurNbTilesMax;
		dTileMaxSize /= 2.0;
		dTileMinSize /= 2.0;
		nNbSampleSteps++;
	}	
	m_nNbSteps = nNbSampleSteps + nNbCropSteps;
	::SendMessageW (hProgress, UM_PROGRESS_SETRANGE, 0, nNbSampleSteps + nNbCropSteps);
}

IGDZMonitor::~IGDZMonitor ()
{
	std::vector <IGDZTile*>& lpImageDZSampled = m_vpFullImages.Lock();
	while (!lpImageDZSampled.empty())
	{
		IGDZTile *pImageDZSampled = lpImageDZSampled.back();
		lpImageDZSampled.pop_back();
		delete pImageDZSampled;
	}
	m_vpFullImages.UnLock ();
}

void IGDZMonitor::NotifySamplingTaskEnded (IGDZTile *pImageDZSampled)
{
	std::vector <IGDZTile*>& lpImageDZSampled = m_vpFullImages.Lock();
	lpImageDZSampled.push_back (pImageDZSampled);
	m_vpFullImages.UnLock ();
	--m_nNbSteps;
	::SendMessageW (m_hProgressBar, UM_PROGRESS_STEPIT, 0, 0);
	if (IsWorkFinished())
	{
		::PostThreadMessageW (m_nParentThreadId, UM_DEEPZOOM_ENDED, NULL, IGREQUEST_SUCCEEDED);
	}
}

void IGDZMonitor::NotifyCroppingTaskEnded()
{
	--m_nNbSteps;
	::SendMessageW (m_hProgressBar, UM_PROGRESS_STEPIT, 0, 0);
	if (IsWorkFinished())
	{
		::PostThreadMessageW (m_nParentThreadId, UM_DEEPZOOM_ENDED, NULL, IGREQUEST_SUCCEEDED);
	}
}

bool IGDZMonitor::PostTask (IGDZSampleMessage *pMessage)
{
	bool bRet = false;
	std::vector <CxImageDZThreadSampler*>& lpUnlockedImages = m_vpSamplingThreads.Lock();
	// find the most advanced thread
	int nMaxProgress = 0;
	CxImageDZThreadSampler *pMaxProgThread = NULL;
	for (std::vector <CxImageDZThreadSampler*>::iterator it = lpUnlockedImages.begin();
			it != lpUnlockedImages.end(); ++it)
	{
		if (nMaxProgress <= (*it)->GetProgress())
		{
			pMaxProgThread = *it;
			nMaxProgress = pMaxProgThread->GetProgress();			
		}
	}
	m_vpSamplingThreads.UnLock ();
	// post task to the most advanced thread
	if (pMaxProgThread)
		bRet = pMaxProgThread->PostMsg (pMessage);
	_ASSERTE (bRet && L"IGDZMonitor::PostTask Error: failed to post the task");
	return bRet;
}

int IGDZMonitor::GetParentThreadId()
{
	return m_nParentThreadId;
}

HWND IGDZMonitor::GetProgressBar()
{
	return m_hProgressBar;
}

std::wstring IGDZMonitor::GetRootFolder()
{
	return m_wsDZRootFolder;
}

bool IGDZMonitor::IsWorkFinished()
{
	return (m_nNbSteps == 0);
}

void IGDZMonitor::GetSampledImageBits(int nLevel, const BYTE*& pBits, int& nWidth, int& nHeight)
{
	std::vector <IGDZTile*>& lpImageDZSampled = m_vpFullImages.Lock();
	bool bFound = false;
	if (lpImageDZSampled.size() > 0)
	{
		std::vector <IGDZTile*>::iterator it = lpImageDZSampled.begin();
		do
		{
			if ((*it)->GetLevel() == nLevel)
			{
				bFound = true;
				break;
			}
		}
		while (++it != lpImageDZSampled.end());
		_ASSERTE (bFound && L"IGDZMonitor error: Cannot find the requested sampled image");
		if (bFound)
			(*it)->GetBits (pBits, nWidth, nHeight);
	}
	else
	{
		// Get the original image bits
		pBits = (const BYTE*)m_pOriginalBits;
		nWidth = -1;
		nHeight = -1;
	}
	m_vpFullImages.UnLock();
}

IGDZTile::IGDZTile (int nWidth, int nHeight, int nLevel, int nBitCount) : m_nWidth (nWidth)
															, m_nHeight (nHeight)
															, m_nLevel (nLevel)
															, m_nBitCount (nBitCount)
{
	::memset (&m_bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	m_bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmInfo.bmiHeader.biWidth = nWidth;
	m_bmInfo.bmiHeader.biHeight = nHeight;
	m_bmInfo.bmiHeader.biPlanes=1;
	m_bmInfo.bmiHeader.biBitCount=nBitCount;

	m_hTileDC = ::CreateCompatibleDC (::GetDC (NULL));
	::SetStretchBltMode (m_hTileDC, COLORONCOLOR);
	m_hTileBmp = ::CreateDIBSection (m_hTileDC, &m_bmInfo, DIB_RGB_COLORS, (void**)&m_pDib, 0, 0);
}

IGDZTile::~IGDZTile(void)
{
	::DeleteObject (m_hTileBmp);
	::DeleteDC (m_hTileDC);
}

void IGDZTile::Copy (const BYTE *pSrcBits)
{
	memset(&m_bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	m_bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmInfo.bmiHeader.biWidth = m_nWidth;
	m_bmInfo.bmiHeader.biHeight = m_nHeight;
	m_bmInfo.bmiHeader.biPlanes=1;
	m_bmInfo.bmiHeader.biBitCount=m_nBitCount;

	//WIN32 resampling
	HGDIOBJ hTmpObj = SelectObject (m_hTileDC, m_hTileBmp);
	::SetDIBits (m_hTileDC, m_hTileBmp, 0, m_nHeight, pSrcBits, &m_bmInfo, DIB_RGB_COLORS);
	::SelectObject (m_hTileDC, hTmpObj);
}

void IGDZTile::Resample (const BYTE *pSrcBits, int nSrcWidth, int nSrcHeight)
{
	memset(&m_bmInfo.bmiHeader,0,sizeof(BITMAPINFOHEADER));
	m_bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bmInfo.bmiHeader.biWidth = nSrcWidth;
	m_bmInfo.bmiHeader.biHeight = nSrcHeight;
	m_bmInfo.bmiHeader.biPlanes=1;
	m_bmInfo.bmiHeader.biBitCount=m_nBitCount;

	//WIN32 resampling
	HGDIOBJ hTmpObj = SelectObject (m_hTileDC, m_hTileBmp);
	::StretchDIBits (m_hTileDC, 0, 0, m_nWidth, m_nHeight, 
		0, 0, nSrcWidth, nSrcHeight, pSrcBits, &m_bmInfo, DIB_RGB_COLORS, SRCCOPY);
	::SelectObject (m_hTileDC, hTmpObj);
}

void IGDZTile::BW()
{
	for (int idxRow = 0; idxRow < m_nHeight; idxRow++){
		BYTE *pCurPixel = GetBitsRW (idxRow);
		for (int idxCol = 0; idxCol < m_nWidth; idxCol++){
			if (*pCurPixel > 128){
				*pCurPixel++ = 255;
				*pCurPixel++ = 255;
				*pCurPixel++ = 255;
			}
			else{
				*pCurPixel++ = 0;
				*pCurPixel++ = 0;
				*pCurPixel++ = 0;
			}
		}
	}
}

void IGDZTile::Save (const std::wstring& wsPath)
{
	CxImage tmp;
	tmp.CreateFromHBITMAP (m_hTileBmp);
	tmp.Save (wsPath.c_str(), CXIMAGE_FORMAT_JPG);
}

void IGDZTile::CopyTo (CxImage& copyDest)
{
	copyDest.CreateFromHBITMAP (m_hTileBmp);
}

IGDZSampleMessage::IGDZSampleMessage(IGDZMonitor *pMonitor, 
						const RECT& rcDst,
						const RECT& rcSrc,
						const RECT& rcUpdate,
						const RECT& rcLastUpdate,						
						int nLevel)	: IGLibrary::IGThreadMessage (pMonitor->GetParentThreadId(), pMonitor->GetProgressBar())
									, m_pMonitor (pMonitor)
									, m_rcDst (rcDst)
									, m_rcSrc (rcSrc) 
									, m_rcUpdate (rcUpdate)
									, m_rcLastUpdate (rcLastUpdate)
									, m_nLevel (nLevel)

{
}

IGDZSampleMessage::IGDZSampleMessage(IGDZMonitor *pMonitor, 
	const RECT& rcDst,
	const RECT& rcSrc,
	int nLevel)	: IGLibrary::IGThreadMessage (pMonitor->GetParentThreadId(), pMonitor->GetProgressBar())
	, m_pMonitor (pMonitor)
	, m_rcDst (rcDst)
	, m_rcSrc (rcSrc) 
	, m_nLevel (nLevel)

{
}

////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ENCODE

#endif // CXIMAGE_SUPPORT_DZ
