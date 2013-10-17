#include "StdAfx.h"
#include "ximage.h"
#include "IGLibPicture.h"
#include "IGFrame.h"
#include "IGLayer.h"
#include "IGIPMergeLayers.h"
#include "IGImageProcMessages.h"
#include "IGIPFilter.h"
#include "ximaig.h"
#include "IGInternalProcessing.h"
#include "ximaDZ.h"
#include "IGBrush.h"
#include <typeinfo>
#include <atlstr.h>

using namespace IGLibrary;

#define IGFRAME_RENDERING_MININTERVAL			333	// 3 fps max to prevent DC crash (issue on Windows/graphic driver/GPU)

wstring					IGFrame::g_wsOutputPath;

IGFrame::IGFrame (HRSRC hRes, DWORD dwType, HINSTANCE hInstance, HWND hOwner, bool bStart)	: m_hOwner (hOwner)
																							, m_pFrame (NULL)
																							, IGView (hOwner)
																							, m_spProgress (new IGSplashWindow (hOwner))
{
	// Start image processing thread
	if (bStart)
		Start ();
	info.hProgress = m_spProgress->GetHWND();
	AddCalc (new IGLayer (this), -1);
	Initialize();
	GetLayer (0)->LoadResource (hRes, dwType, hInstance);
	head.biWidth = GetLayer (0)->GetWidth();
	head.biHeight = GetLayer (0)->GetHeight();
	setGuid();
}

// Asynchronous method
IGFrame::IGFrame (HWND hOwner, LPCWSTR pwPath, bool bAutoRotate, bool bStart)	: IGView (hOwner)
															, m_hOwner (hOwner)
															, m_pFrame (NULL)
															, m_spProgress (new IGSplashWindow (hOwner))
															, m_wsFilePath (pwPath)															
{
	// Start image processing thread
	if (bStart)
		Start ();
	info.hProgress = m_spProgress->GetHWND();
	if (GetImageType (pwPath) == CXIMAGE_FORMAT_IG)
	{
		Load (pwPath, bAutoRotate);
	}
	else
	{
		AddCalc (pwPath, -1, bAutoRotate);
		head.biWidth = GetLayer (0)->GetWidth();
		head.biHeight = GetLayer (0)->GetHeight();	
	}
	Initialize();
	setGuid();
}

// Asynchronous method
IGFrame::IGFrame (IGSmartPtr <IGSplashWindow> spProgress, LPCWSTR pwPath, bool bAutoRotate, bool bStart)	: IGView (NULL)
																						, m_hOwner (NULL)
																						, m_pFrame (NULL)
																						, m_spProgress (spProgress)
																						, m_wsFilePath (pwPath)
{
	// Start image processing thread
	if (bStart)
		Start ();
	info.hProgress = m_spProgress->GetHWND ();
	if (GetImageType (pwPath) == CXIMAGE_FORMAT_IG)
	{
		Load (pwPath, bAutoRotate);
	}
	else
	{				
		AddCalc (pwPath, -1, bAutoRotate);
		head.biWidth = GetLayer (0)->GetWidth();
		head.biHeight = GetLayer (0)->GetHeight();	
	}
	Initialize();
	setGuid();
}

IGFrame::IGFrame (HWND hOwner, HANDLE hMem, bool bStart)	: IGView (hOwner)
															, m_hOwner (hOwner)
															, m_pFrame (NULL)
															, m_spProgress (new IGSplashWindow (hOwner))
{
	// Start image processing thread
	if (bStart)
		Start ();
	info.hProgress = m_spProgress->GetHWND();
	AddCalc (new IGLayer (hMem, this), -1);
	Initialize();
	head.biWidth = GetLayer (0)->GetWidth();
	head.biHeight = GetLayer (0)->GetHeight();
	setGuid();
}

IGFrame::IGFrame (HWND hOwner, int nWidth, int nHeight,
				  int nColorMode, int nBackgroundMode, bool bStart)	: IGView (hOwner)
																	, m_hOwner (hOwner)
																	, m_pFrame (NULL)
																	, m_spProgress (new IGSplashWindow (hOwner))
{
	// Start image processing thread
	if (bStart)
		Start ();
	info.hProgress = m_spProgress->GetHWND();
	head.biWidth = nWidth;
	head.biHeight = nHeight;
	AddCalc (new IGLayer (this), -1);
	Initialize();
	setGuid();
	static int numNew = 1;
	wstring wsNewPath;
	m_spConfigMgr->GetUserPicturesPath (wsNewPath);
	wsNewPath += L"New";
	if (numNew++ > 1)
	{
		wchar_t numNewBuf [256];
		::_itow_s (numNew, numNewBuf, 256, 10);
		wsNewPath += numNewBuf;
	}
	wsNewPath += L".ig";
	SetPath (wsNewPath.c_str());
}

// IG Format loader
IGFrame::IGFrame (DWORD dwType)	: CxImage (dwType)
								, IGView (NULL)
{
	setGuid();
}

IGFrame::IGFrame (const IGFrame& frameCopy, bool bStart)	: IGView (frameCopy.m_hOwner)
															, m_spProgress (new IGSplashWindow (frameCopy.m_hOwner))
{
	Copy (*((CxImage*)&frameCopy));
	Initialize();
	m_bSelected = frameCopy.m_bSelected;
	m_bRedraw = frameCopy.m_bRedraw;
	if ((GetNbLayers() > 0) && bStart)
	{
		// Start image processing thread
		Start ();	
	}	
	setGuid();
}

IGFrame::~IGFrame(void)
{	
	if (IsStarted ())
	{
		// Exit image processing thread
		Exit ();
	}
}

bool IGFrame::Start()
{
	return IGLibrary::IGThread::Start();
}

bool IGFrame::Exit()
{
	return IGLibrary::IGThread::Exit();
}

void IGFrame::setGuid()
{
	GUID guidReq;
	::CoCreateGuid (&guidReq);
	LPOLESTR pwGuid;
	::StringFromCLSID (guidReq, &pwGuid);
	m_wsGuid = pwGuid;
	m_wsGuid = m_wsGuid.substr (1, m_wsGuid.length() - 2);
	::CoTaskMemFree (pwGuid);
}

LPCWSTR IGFrame::GetUserOutputPath()
{
	if (g_wsOutputPath.empty())
		return NULL;
	return g_wsOutputPath.c_str();
}

void IGFrame::SetOutputPath (LPCWSTR pcwPath)
{
	g_wsOutputPath = pcwPath;
}


void IGFrame::Initialize()
{
	if (GetNbLayers() > 0)
	{
		if (head.biWidth < 1)
			head.biWidth = GetLayer (0)->GetWidth();
		if (head.biHeight < 1)
			head.biHeight = GetLayer (0)->GetHeight();
		GetLayer (0)->SetId (0);
		SelectCalc (0);
		SetWorkingLayer (0);
	}
	SetProperty (IGFRAMEPROPERTY_WIDTH, head.biWidth);
	SetProperty (IGFRAMEPROPERTY_HEIGHT, head.biHeight);
	m_bSelected = false;
	m_bRedraw = true;
	m_bNewDragAndDrop = true;
}

void IGFrame::SetSize (int nWidth, int nHeight)
{
	head.biWidth = nWidth;
	head.biHeight = nHeight;
	SetProperty (IGFRAMEPROPERTY_WIDTH, nWidth);
	SetProperty (IGFRAMEPROPERTY_HEIGHT, nHeight);
}

void IGFrame::Ghost(const CxImage *src)
{
	CxImage::Ghost (src);
}

void IGFrame::Ghost (const IGLibrary::IGFrame *src)
{
	CxImage::Ghost ((const CxImage *)src);
}

void IGFrame::CopyInfo(const CxImage &src)
{
	CxImage::CopyInfo (src);
}

bool IGFrame::Transfer(CxImage &from, bool bTransferFrames /*=true*/)
{
	return CxImage::Transfer (from, bTransferFrames);
}

HRESULT IGFrame::AddCalc (LPCWSTR pwPath, long nPos, bool bAutoRotate)
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	if (nPos >= GetNbLayers())
		return E_FAIL;	
	DWORD dwImageType = GetImageType (pwPath);
	if (dwImageType == CXIMAGE_FORMAT_UNKNOWN)
		return E_FAIL;
	if (!LayerCreate (nPos))
		return E_FAIL;
	IGLayer *pLayer = GetLayer(nPos);
	if (!pLayer)
		return E_FAIL;
	pLayer->setFrameOwner (this);
	pLayer->setProgressOwner (info.hProgress);
	if (!pLayer->Load (pwPath, bAutoRotate))
		return E_FAIL;	
	return S_OK;
}

HRESULT IGFrame::AddCalc (IGLayer *pLayer, long nPos, bool bHistory, int x, int y)
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	if ((int)pLayer->GetId() < 0)
		pLayer->SetId (GetMaxLayerId() + 1);
	int nNbLayers = GetNbLayers();
	if (nPos >= nNbLayers)
		nPos = -1;
	std::list<CxImage*>::const_iterator itInsert = lpLayers.begin();
	while (itInsert != lpLayers.end()){
		if ((*itInsert)->GetId() == pLayer->GetId())
			return E_FAIL;
		++itInsert;
	}
	if (nPos >= 0)
	{
		itInsert = lpLayers.begin();		
		while (nPos--)
			++itInsert;
		lpLayers.insert (itInsert, pLayer);
	}
	else
	{
		lpLayers.push_back (pLayer);
	}
	info.nNumLayers = lpLayers.size();
	pLayer->setFrameOwner (this);
	pLayer->setProgressOwner (info.hProgress);
	pLayer->info.xOffset = x;
	pLayer->info.yOffset = y;

	if (bHistory)
		AddNewStep (IGFRAMEHISTORY_STEP_LAYERADD, L"Add layer", GetNbLayers() - 1);	
	return S_OK;	
}

HRESULT IGFrame::RemoveCalc (long nPos, IGLayer **ppDeletedLayer, bool bHistory)
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	if (bHistory) // do not record that step just initialize the layer id. saving the layer is done after adding it.
		AddNewStep (IGFRAMEHISTORY_STEP_LAYERREMOVE, L"Remove layer", nPos, -1, -1, false);
	int nNbLayers = GetNbLayers();
	if ((nNbLayers <= 1) || (nPos >= nNbLayers))
		return E_FAIL;
	bool bSelectLast = false;	
	if (GetLayer (nPos)->GetSelected())
		bSelectLast = true;	
	bool bWorking = false;
	int nCurWorkingLayerPos = GetLayerPos(GetWorkingLayer());
	if (nCurWorkingLayerPos > nPos)
		SetWorkingLayer (nCurWorkingLayerPos - 1);
	else if (nCurWorkingLayerPos == nPos)
		bWorking = true;
	bool bRes = false;
	if (ppDeletedLayer)
	{
		CxImage *pDeletedLayer;
		bRes = CxImage::LayerDelete (nPos, &pDeletedLayer);
		if (ppDeletedLayer)
			*ppDeletedLayer = dynamic_cast <IGLayer*> (pDeletedLayer);
	}
	else
	{
		bRes = CxImage::LayerDelete (nPos);
	}
	if (bSelectLast)
	{
		UnselectCalcs();		
		SelectCalc (nNbLayers - 1);		
	}
	if (bWorking)
		SetWorkingLayer (GetNbLayers() - 1);
	if (bHistory)
		SaveStep();
	return S_OK ;
}

HRESULT IGFrame::MoveCalc (long nPosFrom, long nPosDest)
{
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	IGLayer *pLayer = NULL;
	if (FAILED (RemoveCalc (nPosFrom, &pLayer, false)))
		return E_FAIL;
	if (FAILED (AddCalc (pLayer, nPosDest, false)))
		return E_FAIL;
	// add a new history step
	AddNewStep (IGFRAMEHISTORY_STEP_LAYERMOVE, L"Move layer", nPosFrom, nPosDest);
	return S_OK;
}

HRESULT IGFrame::RemoveAllCalcs()
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	CxImage::LayerDeleteAll();
	return S_OK;
}

void IGFrame::UnselectCalcs()
{
	// This method is safe for multithreading => no request access on beginning
	for (int nIdxItem = 0; nIdxItem < GetNbLayers(); nIdxItem++)
	{
		IGLayer *pLayer = GetLayer (nIdxItem);
		if (pLayer)
			pLayer->SetSelected (false);
	}
	m_lpSelectedLayers.clear();
}

int IGFrame::GetNbLayers () const
{
	// This method is safe for multithreading => no request access on beginning
	return GetNumLayers();
}

bool IGFrame::SelectCalc (int nPos)
{
	// This method is safe for multithreading => no request access on beginning
	if (nPos >= GetNumLayers() && nPos != -1)
		return false;
	// select layer nPos
	IGLayer *pLayer = GetLayer (nPos);
	if (!pLayer)
		return false;
	pLayer->SetSelected();
	m_lpSelectedLayers.push_back (pLayer);
	return true;
}

std::list <const IGLayer*> IGFrame::GetSelectedCalcs () const
{
	// This method is safe for multithreading => no request access on beginning
	std::list <const IGLayer*> lcpLayers;
	for (list <IGLayer*>::const_iterator itItem = m_lpSelectedLayers.begin();
		 itItem != m_lpSelectedLayers.end(); ++itItem)
	{
		lcpLayers.push_back (const_cast <const IGLayer*> (*itItem));
	}
	return lcpLayers;
}

bool IGFrame::Render (HDC hdc)
{
	// This method is safe for multithreading => no request access on beginning
	// only redraw mode is forbidden for multithreading
	int nZoomPosX = 0;
	int nZoomPosY = 0;
	int nZoomWidth = 0;
	int nZoomHeight = 0;
	GetFramePos (nZoomPosX, nZoomPosY, nZoomWidth, nZoomHeight);	
	if (!nZoomWidth ||
		!nZoomHeight)
		return false;
	if ((m_nZoomWidth != nZoomWidth) || (m_nZoomHeight != nZoomHeight) ||
		(nZoomPosX != m_nZoomPosX) || (nZoomPosY != m_nZoomPosY))
		m_bRedraw = true;

	// redraw mode is forbidden for multithreading
	if (!RequestAccess () && m_bRedraw)
	{
		m_bRedraw = false;
		IGView <IGFrame>::ThrowDelayedRendering();
	}

	m_nZoomWidth = nZoomWidth;
	m_nZoomHeight = nZoomHeight;
	m_nZoomPosX = nZoomPosX;
	m_nZoomPosY = nZoomPosY;
	RECT rcFrame;
	ZeroMemory (&rcFrame, sizeof (RECT));
	rcFrame.left = 0;
	rcFrame.top = 0;
	if (nZoomPosX > 0)
	{
		rcFrame.right = (nZoomWidth > m_nWidth) ? m_nWidth : nZoomWidth;
	}
	else
	{
		rcFrame.right = ((nZoomPosX + nZoomWidth) > m_nWidth) ? m_nWidth : nZoomPosX + nZoomWidth;
	}
	if (nZoomPosY > 0)
	{
		rcFrame.bottom = (nZoomHeight > m_nHeight) ? m_nHeight : nZoomHeight;
	}
	else
	{
		rcFrame.bottom = ((nZoomPosY + nZoomHeight) > m_nHeight) ? m_nHeight : nZoomPosY + nZoomHeight;
	}
	if (m_bRedraw)
	{
		// check if rendering has to be delayed
		if (m_dwRenderTickCount)
		{
			DWORD dwTickCount = ::GetTickCount ();
			if (dwTickCount - m_dwRenderTickCount < IGFRAME_RENDERING_MININTERVAL)
			{
				// rendering interval is too short, delay !
				m_bRedraw = false;
				IGView <IGFrame>::ThrowDelayedRendering();
			}
		}
		if (m_bRedraw)
		{
			LayerDrawAllInCache (hdc, nZoomPosX < 0 ? nZoomPosX : 0, nZoomPosY < 0 ? nZoomPosY : 0, nZoomWidth, nZoomHeight, &rcFrame, false, true);
			m_dwRenderTickCount = ::GetTickCount ();
			m_bRedraw = false;
		}
	}
	return (LayerDrawAllFromCache (hdc, nZoomPosX > 0 ? nZoomPosX : 2,
										nZoomPosY > 0 ? nZoomPosY : 2) == 1);
}

bool IGFrame::RenderSecondary (HDC hdc, int nLeft, int nTop,
								int nWidth, int nHeight, bool bChessBackground)
{
	// This method is safe for multithreading => no request access on beginning
	// only draw-in-cache mode is forbidden for multithreading
	if (!SelectLayerCache (1) || (head.biWidth == 0) || (head.biHeight == 0))	// switch to second channel cache
		return false;
	long nRes = 1;
	float fOldZoom = GetCurrentZoom();
	SetCurrentZoom (min ((float)nWidth / (float)head.biWidth, (float)nHeight / (float)head.biHeight));
	if (RequestAccess ())
	{
		nRes = LayerDrawAllInClearedCache (hdc, 0, 0,
										nWidth, nHeight, NULL,
										false, bChessBackground);	
	}
	SetCurrentZoom (fOldZoom);
	nRes = (LayerDrawAllFromCache (hdc, nLeft, nTop) == 1) ? nRes : 0;
	SelectLayerCache (0);		// switch back to first channel cache
	return (nRes == 1);
}

long IGFrame::LayerDrawAllFromCache (HDC hdc, int nDestX, int nDestY, unsigned char ucAlpha, COLORREF cTranspMask)
{
	// This method is safe for multithreading => no request access on beginning
	return CxImage::LayerDrawAllFromCache (hdc, nDestX, nDestY, ucAlpha, cTranspMask);
}

long IGFrame::LayerDrawAllInCache(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bShowCurrentBackground, COLORREF cBackgroundColor, COLORREF cTranspMask)
{
	// Request thread access
	if (!RequestAccess ())
		return false;

	return CxImage::LayerDrawAllInCache (hdc, x, y, cx, cy, pClipRect, bSmooth, bShowChessBackground, bShowCurrentBackground, cBackgroundColor, cTranspMask);
}

long IGFrame::LayerDrawAllInClearedCache(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bShowCurrentBackground, COLORREF cBackgroundColor, COLORREF cTranspMask)
{
	// Request thread access
	if (!RequestAccess ())
		return false;

	return CxImage::LayerDrawAllInClearedCache (hdc, x, y, cx, cy, pClipRect, bSmooth, bShowChessBackground, bShowCurrentBackground, cBackgroundColor, cTranspMask);
}

long IGFrame::LayerDrawAllClearCache()
{
	// Request thread access
	if (!RequestAccess ())
		return 0;

	return CxImage::LayerDrawAllClearCache ();
}

long IGFrame::GetLayerPos (const CxImage *pLayer) const
{
	// This method is safe for multithreading => no request access on beginning
	return CxImage::GetLayerPos (pLayer);
}

void IGFrame::Redraw (bool bForceRedraw, bool bAllFrame)
{
	// This method is safe for multithreading => no request access on beginning
	m_bRedraw = bForceRedraw;
	if (bAllFrame)
	{
		::InvalidateRect (m_hOwner, NULL, FALSE);
		return;
	}
	int nFramePosX = 0;
	int nFramePosY = 0;
	int nFramePosWidth = 0;
	int nFramePosHeight = 0;
	GetFramePos (nFramePosX, nFramePosY, nFramePosWidth, nFramePosHeight);
	RECT rcFrame = {nFramePosX, nFramePosY, nFramePosX + nFramePosWidth, nFramePosY + nFramePosHeight};
	::InvalidateRect (m_hOwner, &rcFrame, FALSE);	
}

void IGFrame::RedrawSecondary ()
{
	// Request thread access
	if (!RequestAccess ())
		return;

	if (SelectLayerCache (1))	// switch to second channel cache
	{
		LayerDrawAllClearCache();
	}
	SelectLayerCache (0);
}

bool IGFrame::SetSelected (bool bSelected)
{
	// This method is safe for multithreading => no request access on beginning
	bool bChanged = (bSelected != m_bSelected);
	m_bSelected = bSelected;
	return bChanged;
}

IGLayer* IGFrame::GetLayer (int nPos) const
{
	// This method is safe for multithreading => no request access on beginning
	CxImage *pLayer = CxImage::GetLayer(nPos);
	return dynamic_cast <IGLayer*> (pLayer);
}

IGLayer* IGFrame::GetLayerFromId (int nId) const
{
	IGLayer *pCurrentLayer;
	for (int idxLayer = 0; idxLayer < GetNbLayers(); idxLayer++)
	{
		pCurrentLayer = GetLayer (idxLayer);
		if ((int)pCurrentLayer->GetId() == nId || pCurrentLayer->HasBeen(nId))
			return pCurrentLayer;
	}
	return NULL;
}

bool IGFrame::CreateNewCalc (long nPos)
{
	// Request thread access
	if (!RequestAccess ())
		return false;	
	if (!LayerCreate (nPos, true, false))
		return false;
	return AddNewStep (IGFRAMEHISTORY_STEP_LAYERADD, L"Add layer", GetNbLayers() - 1);
}

bool IGFrame::LayerCreate (long nPos, bool bAlpha, bool bSelection, int x, int y)
{	
	if (nPos > GetNbLayers())
		return false;	
	if (info.nNumLayers >= IGFRAME_MAX_NBLAYERS)
	{
		::MessageBoxW (m_hOwner, L"The maximum number of layers (10) has been reached", L"Layer creation", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	IGLayer *pNewLayer = new IGLayer (this, bAlpha, bSelection);
	pNewLayer->setProgressOwner (info.hProgress);
	if ((int)pNewLayer->GetId() < 0)
		pNewLayer->SetId (GetMaxLayerId() + 1);
	if (!CxImage::LayerCreate (nPos, pNewLayer, x, y))
		return false;
	if (nPos < 0)
		SetWorkingLayer (GetNbLayers() - 1);
	else
		SetWorkingLayer (nPos);
	return true;
}

void IGFrame::UpdatePosition()
{
	if (m_spProgress)
		m_spProgress->UpdatePosition();
}

void IGFrame::OnImageProcessingEnded (IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcId)
{	
	_ASSERTE ((GetNbLayers() >= 1) && L"No layer loaded");
	if (GetNbLayers() < 1)
		return;
	switch (nImageProcId)
	{
		case IGREQUEST_FRAME_MERGELAYER:
			for (int idxLayer = 0; idxLayer < GetNbLayers(); idxLayer++)
			{
				GetLayer (idxLayer)->SetVisible();
				GetLayer (idxLayer)->LayerDrawAllClearCache();
			}
			SelectCalc (0);
			SetWorkingLayer (0);
			break;
	}	
	ZoomToFrame();	
	Redraw (true);
}

HANDLE IGFrame::CopyToHandle (bool bCopyAlpha)
{
	// Request thread access
	if (!RequestAccess ())
		return NULL;

	int nNbLayers = GetNbLayers();
	if (nNbLayers <= 0)
		return false;
	std::list <CxImage*> lpSrcLayers;
	lpSrcLayers.insert (lpSrcLayers.begin(), lpLayers.begin(), lpLayers.end());
	CxImage *pDstLayer = lpSrcLayers.front();
	lpSrcLayers.pop_front();
	IGLayer *pNewLayer = new IGLayer (this);
	pNewLayer->Copy (*pDstLayer, true, false, true, false);
	pNewLayer->setFrameOwner (this, false);
	if (m_bNewDragAndDrop)
	{
		m_spProgress->ProgressSetMode (SPLASHSCREEN_PROGRESSBAR_PROGRESSION);
		m_spProgress->ProgressSetRange (IGIPMergeLayersMessage::ComputeNbProgressSteps (lpLayers));
		m_spProgress->Show (true);
	}
	for (int idxLayer = 1; idxLayer < nNbLayers; idxLayer++)
	{
		pNewLayer->Mix (*GetLayer (idxLayer));
	}
	m_spProgress->Hide();

	HANDLE hCopy = pNewLayer->CopyToHandle (bCopyAlpha);
	delete pNewLayer;

	if (m_bNewDragAndDrop)
	{
		m_bNewDragAndDrop = false;
	}
	else
	{
		m_spProgress->Hide();
		m_spProgress->ProgressSetRange (100);
		m_bNewDragAndDrop = true;
	}
	return hCopy;
}
	
bool IGFrame::PasteFromHandle (HANDLE hMem, long x, long y)
{
	// Request thread access
	if (!RequestAccess ())
		return false;

	IGLayer *pNewLayer = new IGLayer (hMem, this);
	int nPosX, nPosY, nWidth, nHeight;
	GetFramePos (nPosX, nPosY, nWidth, nHeight);
	float fCurrentZoom = CxImage::GetCurrentZoom();
	x = (long)(((float)(x - nPosX)) / fCurrentZoom) - pNewLayer->GetWidth() / 2;
	y = (long)(((float)(y - nPosY)) / fCurrentZoom) - pNewLayer->GetHeight() / 2;
	if (SUCCEEDED (AddCalc (pNewLayer, -1, true, x, y)))
	{
		SetWorkingLayer (GetNbLayers() - 1);
		return true;
	}
	return false;
}

HBITMAP IGFrame::MakeBitmap (HDC hdc, int nImageWidth, int nImageHeight, bool bDrawChessBackground, bool bDrawWhiteBackground)
{
	// Request thread access
	if (!RequestAccess ())
		return false;

	return CxImage::MakeBitmap (hdc, nImageWidth, nImageHeight, bDrawChessBackground, bDrawWhiteBackground);
}

HRESULT IGFrame::Load (const TCHAR* filename, bool bAutoRotate)
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	if (IGThread::IsStarted())
	{
		IGImageProcessing *pLoadImageProcessing = new IGImageProcessing (this, new IGImageLoadMessage (filename, bAutoRotate));
		return pLoadImageProcessing->Start() ? S_OK : E_FAIL;
	}
	return CxImage::Load (filename, GetImageType (filename)) ? S_OK : E_FAIL;
}

HRESULT IGFrame::Save (const TCHAR* filename, DWORD imagetype, bool bCloseAfterSave, bool bAsync)
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;

	return InternalSave (filename, imagetype, bCloseAfterSave, bAsync);
}

HRESULT IGFrame::GetPath (std::wstring& wsPath) const
{
	// Request thread access
	wsPath = m_safewsPath;
	return S_OK;
}

HRESULT IGFrame::GetName (std::wstring& wsName) const
{
	std::wstring wsPicPath;
	HRESULT hr = GetPath (wsPicPath);
	if (SUCCEEDED(hr) && !wsPicPath.empty())
	{
		std::wstring wsUserPicturesPath;
		IGSingleton <IGConfigManager> spConfigManager;
		spConfigManager->GetUserPicturesPath (wsUserPicturesPath);
		if ((wsPicPath[wsUserPicturesPath.length() - 1] == L'\\') ||
			(wsPicPath[wsUserPicturesPath.length() - 1] == L'/'))
			wsName = wsPicPath.substr (wsUserPicturesPath.length());
		else
			wsName = wsPicPath.substr (wsUserPicturesPath.length() + 1);
	}
	return hr;
}

HRESULT IGFrame::SetName (const std::wstring& wsName)
{
	std::wstring wsUserPicturesPath;
	IGSingleton <IGConfigManager> spConfigManager;
	spConfigManager->GetUserPicturesPath (wsUserPicturesPath);
	m_safewsPath = wsUserPicturesPath + wsName;
	return S_OK;
}

HRESULT IGFrame::InternalSave (const TCHAR* filename, DWORD imagetype, bool bCloseAfterSave, bool bAsync)
{
	if (bAsync)
	{
		IGImageProcessing *pSaveImageProcessing = new IGImageProcessing (this, new IGImageSaveMessage (filename, imagetype, bCloseAfterSave));
		return pSaveImageProcessing->Start() ? S_OK : E_FAIL;
	}
	return (Save (filename, imagetype) == 1) ? S_OK : E_FAIL;
}

HRESULT IGFrame::Resample (long newx, long newy, IGFRAMEPROPERTY_STRETCHING_ENUM eStretchMode)
{
	// Request thread access
	if (!RequestAccess ())
		return E_ACCESSDENIED;
	HRESULT hRes = E_NOTIMPL;
	CxImage *pCurrentLayer = GetWorkingLayer();
	if (!pCurrentLayer)
		return false;
	pCurrentLayer->SelectionDelete();
	switch (eStretchMode)
	{
	case IGFRAMEPROPERTY_STRETCHING_NO:
		hRes = resampleNoStretch (newx, newy);
		break;
	case IGFRAMEPROPERTY_STRETCHING_BICUBIC:
		hRes = resampleBicubic (newx, newy);
		break;
	case IGFRAMEPROPERTY_STRETCHING_FAST:
		hRes = resampleFast (newx, newy);	
		break;
	}
	if (SUCCEEDED (hRes))
		SetSize (newx, newy);
	return hRes;
}

HRESULT IGFrame::resampleNoStretch (long newx, long newy)
{
	IGLayer *pBackgnd = GetLayer(0);
	if (!pBackgnd)
		return E_FAIL;
	int nOldEndx = pBackgnd->info.xOffset + pBackgnd->GetWidth();
	int nOldEndy = pBackgnd->info.yOffset + pBackgnd->GetHeight();
	int nEndx = max (nOldEndx, newx);
	int nEndy = max (nOldEndy, newy);
	IGSplashWindow::SendStepIt (info.hProgress);
	if ((nEndx != nOldEndx) || (nEndy != nOldEndy))
	{
		int nWidth = nEndx - pBackgnd->info.xOffset;
		int nHeight = nEndy - pBackgnd->info.yOffset;
		if (!pBackgnd->Expand (nWidth, nHeight, IGColor(IGColor::white, 0).GetRGBQUAD()))
			return E_FAIL;
		pBackgnd->UpdateSize (nWidth, nHeight);
	}
	IGSplashWindow::SendStepIt (info.hProgress);
	return S_OK;
}

HRESULT IGFrame::resampleFast (long newx, long newy)
{
	float fRatioX = (float)newx / (float)head.biWidth;
	float fRatioY = (float)newy / (float)head.biHeight;
	IGLayer *pCurrentLayer;
	std::list <IGLayer *> lRemoveLayers;
	for (int idxLayer = 0; idxLayer < GetNbLayers(); idxLayer++)
	{
		pCurrentLayer = GetLayer (idxLayer);
		int nCurLayerWidth = (int)((float)pCurrentLayer->head.biWidth * fRatioX);
		int nCurLayerHeight = (int)((float)pCurrentLayer->head.biHeight * fRatioY);
		IGDZTile dzTile (nCurLayerWidth, nCurLayerHeight);
		BYTE *pBits = pCurrentLayer->GetBits();
		dzTile.Resample (pBits, pCurrentLayer->head.biWidth, pCurrentLayer->head.biHeight);
		dzTile.CopyTo (*pCurrentLayer);
		pCurrentLayer->info.xOffset = (int)(fRatioX * (float)pCurrentLayer->info.xOffset);
		pCurrentLayer->info.yOffset = (int)(fRatioY * (float)pCurrentLayer->info.yOffset);
		pCurrentLayer->UpdateSize (nCurLayerWidth, nCurLayerHeight);
		IGSplashWindow::SendStepIt (info.hProgress);
	}
	return S_OK;
}

HRESULT IGFrame::resampleBicubic (long newx, long newy)
{
	float fRatioX = (float)newx / (float)head.biWidth;
	float fRatioY = (float)newy / (float)head.biHeight;
	IGLayer *pCurrentLayer;
	std::list <IGLayer *> lRemoveLayers;
	for (int idxLayer = 0; idxLayer < GetNbLayers(); idxLayer++)
	{
		pCurrentLayer = GetLayer (idxLayer);
		int nCurLayerWidth = (int)((float)pCurrentLayer->head.biWidth * fRatioX);
		int nCurLayerHeight = (int)((float)pCurrentLayer->head.biHeight * fRatioY);
		if (pCurrentLayer->Resample2 (nCurLayerWidth, nCurLayerHeight))
		{
			pCurrentLayer->info.xOffset = (int)(fRatioX * (float)pCurrentLayer->info.xOffset);
			pCurrentLayer->info.yOffset = (int)(fRatioY * (float)pCurrentLayer->info.yOffset);
		}
		else
		{
			// current layer could not be resampled, remove it !
			lRemoveLayers.push_back (pCurrentLayer);
		}
		pCurrentLayer->UpdateSize (nCurLayerWidth, nCurLayerHeight);
	}
	while (!lRemoveLayers.empty())
	{
		RemoveCalc (GetLayerPos (lRemoveLayers.front()));
		lRemoveLayers.pop_front();
	}
	return S_OK;
}

float IGFrame::GetCurrentZoom() const
{
	return CxImage::GetCurrentZoom();
}

void IGFrame::SetCurrentZoom (float fCurrentZoom)
{
	CxImage::SetCurrentZoom (fCurrentZoom);
}

DWORD IGFrame::GetImageType (LPCWSTR lpPicturePath)
{
	CAtlStringW cstrExtension (lpPicturePath);
	if (cstrExtension.GetLength() < 5)
		return CXIMAGE_FORMAT_UNKNOWN;	
	cstrExtension = cstrExtension.Right (4);
	// supported file extenstion length may be 2, 3 or 4
	if (cstrExtension.GetAt (0) == L'.')
		cstrExtension = cstrExtension.Right (3);
	else if (cstrExtension.GetAt (1) == L'.')
		cstrExtension = cstrExtension.Right (2);		
	cstrExtension.MakeUpper();
	if (cstrExtension == L"BMP")
		return CXIMAGE_FORMAT_BMP;
	else if (cstrExtension == L"JPG")
		return CXIMAGE_FORMAT_JPG;
	else if (cstrExtension == L"PNG")
		return CXIMAGE_FORMAT_PNG;
	else if (cstrExtension == L"ICO")
		return CXIMAGE_FORMAT_ICO;
	else if (cstrExtension == L"GIF")
		return CXIMAGE_FORMAT_GIF;
	else if (cstrExtension == L"TIF")
		return CXIMAGE_FORMAT_TIF;
	else if (cstrExtension == L"IG")
		return CXIMAGE_FORMAT_IG;
	return CXIMAGE_FORMAT_UNKNOWN;
}

void IGFrame::setFramePtr (OLE_HANDLE pFrame)
{
	m_pFrame = pFrame;
}

OLE_HANDLE IGFrame::getFramePtr()
{
	return m_pFrame;
}

bool IGFrame::StartDrawing (std::list<POINT>& lPts, IGBrush *pBrush)
{
	IGIPDraw *pDrawImageProcessing = new IGIPDraw (this, 
		new IGIPDrawMessage (lPts, pBrush));
	return pDrawImageProcessing->Start();
}

bool IGFrame::DrawLines (std::list<POINT>& lPts, const IGBrush& brush)
{
	// Request thread access
	if (!RequestAccess ())
		return false;
	CxImage *pCurrentLayer = GetWorkingLayer();
	if (!pCurrentLayer)
		return false;
	if (lPts.size() < 2)
		return false;
	IGConvertible::FromIGtoCxCoords (lPts, pCurrentLayer->GetHeight());
	POINT ptFrom;
	std::list<POINT>::iterator itCurrent = lPts.begin();
	ptFrom = *itCurrent;
	while (++itCurrent != lPts.end())
	{
		pCurrentLayer->DrawLine (ptFrom.x, (*itCurrent).x, ptFrom.y, (*itCurrent).y, brush);
		ptFrom = *itCurrent;
	}
	return true;
}

bool IGFrame::ManageSelection (const IGLibrary::SELECTIONPARAMS& selParams, const std::list<POINT>& lPts)
{
	// Request thread access
	if (!RequestAccess ())
		return false;
	CxImage *pCurrentLayer = GetWorkingLayer();
	if (!pCurrentLayer)
		return false;
	int nCurLayerId = (int)pCurrentLayer->GetId();
	int nCurLayerWidth = (int)pCurrentLayer->GetWidth();
	int nCurLayerHeight = (int)pCurrentLayer->GetHeight();
	_ASSERTE ((nCurLayerId >= 0) && L"Current layer is not identified");
	if (nCurLayerId < 0)
		return false;
	bool bRes = false;
	POINT ptTopLeft = {0, 0};
	POINT ptBottomRight = {-1, -1};
	IGSELECTIONENUM eSelectionType = selParams.eSelectionType;
	if (((eSelectionType & IGSELECTION_SQUARE) == IGSELECTION_SQUARE) ||
		((eSelectionType & IGSELECTION_LASSO) == IGSELECTION_LASSO))
	{
		if (lPts.size() > 0)
		{
			bool bAllEqual = true;
			for (list <POINT>::const_iterator itPt = lPts.cbegin(); itPt != lPts.cend(); ++itPt)
			{
				if (((*itPt).x != lPts.front().x) || ((*itPt).y != lPts.front().y))
					bAllEqual = false;
			}
			if (bAllEqual)
				eSelectionType = IGSELECTION_CLEAR;
		}
	}
	if ((eSelectionType & IGSELECTION_CLEAR) == IGSELECTION_CLEAR)
	{
		if ((eSelectionType & IGSELECTION_INVERT) == IGSELECTION_INVERT)
			bRes = pCurrentLayer->SelectionInvert();
		else
			bRes = pCurrentLayer->SelectionDelete();
	}
	else
	{
		if ((eSelectionType & IGSELECTION_REPLACE) == IGSELECTION_REPLACE)
			pCurrentLayer->SelectionClear();	
		BYTE level = ((eSelectionType & IGSELECTION_REMOVE) == IGSELECTION_REMOVE) ? 0 : 255;

		std::list<POINT> lConvertedPts (lPts); 
		// convert IG coordinates to Cx coordinates
		IGConvertible::FromIGtoCxCoords(lConvertedPts, pCurrentLayer->GetHeight());
	
		// apply selection
		if ((eSelectionType & IGSELECTION_SQUARE) == IGSELECTION_SQUARE)
		{
			if (lPts.size() != 2)
				return false;	
			// read rectangle coordinates
			ptTopLeft = lConvertedPts.front();
			ptBottomRight = lConvertedPts.back();
			int nPosX = ptTopLeft.x;
			int nPosY = ptBottomRight.y;
			RECT rcSel;
			rcSel.left = nPosX; rcSel.top = nPosY;
			nPosX = ptBottomRight.x;
			nPosY = ptTopLeft.y;
			rcSel.right = nPosX; rcSel.bottom = nPosY;
			// adjust rectangle orientation
			int nWidth = rcSel.right - rcSel.left;
			int nHeight = rcSel.top - rcSel.bottom;
			nWidth = (nWidth < 0) ? -1 * nWidth : nWidth;
			nHeight = (nHeight < 0) ? -1 * nHeight : nHeight;
			rcSel.left = (rcSel.left < rcSel.right) ? rcSel.left : rcSel.right;
			rcSel.bottom = (rcSel.bottom < rcSel.top) ? rcSel.bottom : rcSel.top;
			rcSel.right = rcSel.left + nWidth;
			rcSel.top = rcSel.bottom + nHeight;
			// test if rectangle is inside the frame
			if ((rcSel.right < 0) || (rcSel.left >= nCurLayerWidth)
				|| (rcSel.top < 0) || (rcSel.bottom >= nCurLayerHeight))
				return false;	// selection out of bounds
			// adjust bounds
			rcSel.left = (rcSel.left < 0) ? 0 : rcSel.left;
			rcSel.right = (rcSel.right >= nCurLayerWidth) ? nCurLayerWidth - 1 : rcSel.right;
			rcSel.bottom = (rcSel.bottom < 0) ? 0 : rcSel.bottom;
			rcSel.top = (rcSel.top >= nCurLayerHeight) ? nCurLayerHeight - 1 : rcSel.top;
			// add the selection
			bRes = pCurrentLayer->SelectionAddRect (rcSel, level);
		}
		else if ((eSelectionType & IGSELECTION_LASSO) == IGSELECTION_LASSO)
		{
			bRes = pCurrentLayer->SelectionAddPolygon (lConvertedPts, level);
		}
		else if ((eSelectionType & IGSELECTION_MAGIC) == IGSELECTION_MAGIC)
		{
			bRes = pCurrentLayer->SelectionAddMagic (lConvertedPts.front(), level, selParams.nTolerance);
		}
		else if ((eSelectionType & IGSELECTION_FACES) == IGSELECTION_FACES)
		{
			bRes = pCurrentLayer->SelectionAddFaces (level);
		}
		else if ((eSelectionType & IGSELECTION_EYES) == IGSELECTION_EYES)
		{
			bRes = pCurrentLayer->SelectionAddEyes (level);
		}
		else if ((eSelectionType & IGSELECTION_MOUTH) == IGSELECTION_MOUTH)
		{
			bRes = pCurrentLayer->SelectionAddMouth (level);
		}
		else if ((eSelectionType & IGSELECTION_NOZE) == IGSELECTION_NOZE)
		{
			bRes = pCurrentLayer->SelectionAddNoze (level);
		}
		else if ((eSelectionType & IGSELECTION_LPE) == IGSELECTION_LPE)
		{
			bRes = pCurrentLayer->SelectionAddLPE (lConvertedPts, level);
		}
		if (ptBottomRight.x < ptTopLeft.x)
		{
			int nSwap = ptTopLeft.x;
			ptTopLeft.x = ptBottomRight.x;
			ptBottomRight.x = nSwap;
		}
		if (ptBottomRight.y < ptTopLeft.y)
		{
			int nSwap = ptTopLeft.y;
			ptTopLeft.y = ptBottomRight.y;
			ptBottomRight.y = nSwap;
		}
		if (ptTopLeft.x < 0)
			ptTopLeft.x = 0;
		if (ptTopLeft.y < 0)
			ptTopLeft.y = 0;
		if (ptBottomRight.x >= nCurLayerWidth)
			ptBottomRight.x = nCurLayerWidth - 1;
		if (ptBottomRight.y >= nCurLayerHeight)
			ptBottomRight.y = nCurLayerHeight - 1;
	}
	if (!pCurrentLayer->SelectionIsValid())
		pCurrentLayer->SelectionDelete();
	AddNewStep (IGFRAMEHISTORY_STEP_SELECTION, L"Change selection", (int)&selParams, (int)&lPts);
	Redraw (true);
	return bRes;
}

void IGFrame::SetCurrentCursor()
{
	::SendMessageW (m_hOwner, UM_IGFRAME_LAYERCHANGED, 0, 0);
}

LPCWSTR IGFrame::GetId() const
{
	return CxImage::GetId(); 
}

bool IGFrame::StartFiltering (int nFilterId)
{
	IGIPFilter *pFilterImageProcessing = new IGIPFilter (this, 
														  new IGIPFilterMessage ((IGIPFILTER)nFilterId));
	return pFilterImageProcessing->Start();
}

bool IGFrame::StartIndexing (int nFilterId, IGMarker *pMarkerObject, IGMarker *pMarkerBackground)
{
	IGIPIndex *pIndexImageProcessing = new IGIPIndex (this, 
		new IGIPIndexMessage ((IGIPFILTER)nFilterId, pMarkerObject, pMarkerBackground));
	return pIndexImageProcessing->Start();
}

bool IGFrame::StartFaceEffect (int nFaceEffectId, COLORREF col1, COLORREF col2, double dParam1, double dParam2, double dParam3, const TCHAR* imageFile1, const TCHAR* imageFile2)
{
	CxImage *pImage1 = NULL;
	CxImage *pImage2 = NULL;
	if (imageFile1 != NULL)
		pImage1 = new CxImage(imageFile1, GetImageType(imageFile1));
	if (imageFile2 != NULL)
		pImage2 = new CxImage(imageFile1, GetImageType(imageFile2));
	IGIPFilter *pFilterImageProcessing = new IGIPFilter (this, 
		new IGIPFaceEffectMessage ((IGIPFACE_EFFECT)nFaceEffectId, col1, col2, dParam1, dParam2, dParam3, pImage1, pImage2));
	return pFilterImageProcessing->Start();
}

bool IGFrame::Load (const TCHAR* filename, DWORD imagetype)
{
	if (!filename)
		return false;
	SetPath (filename);
	bool bRes = true;
	try{
		bRes = CxImage::Load (filename, imagetype);
		int nNbLayers = GetNumLayers();
		if (nNbLayers <= 0)
			return false;
	}
	catch(...)
	{
		return false;
	}		
	return bRes;
}

bool IGFrame::Save(const TCHAR* filename, DWORD imagetype)
{
	if (!filename)
		return false;
	bool bRes = true;
	try{		
		bRes = CxImage::Save (filename, imagetype);
	}
	catch(...)
	{
		return false;
	}
	if (bRes)
	{
		wstring wsHistoryPath;
		m_spConfigMgr->GetHistoryPath (wsHistoryPath);
		wstring wsTestHistory (filename);
		if (wsTestHistory.find (wsHistoryPath) == wstring::npos)
			SetPath (filename);
	}
	return bRes;
}	

void IGFrame::SetPath(const TCHAR* filename)
{
	m_safewsPath = filename;
	wstring wsUserTempPath;
	m_spConfigMgr->GetUserTempPath (wsUserTempPath);	
	wstring wsPath = m_safewsPath;
	if (wsPath.find (wsUserTempPath.c_str()) != wstring::npos)
	{
		wstring wsUserPicturesPath;
		m_spConfigMgr->GetUserPicturesPath (wsUserPicturesPath);
		m_safewsPath = wsPath.replace (0, wsUserTempPath.length(), wsUserPicturesPath.c_str());
	}
}

const TCHAR* IGFrame::GetPath()
{
	return ((wstring)m_safewsPath).c_str();
}

CxImage* IGFrame::GetSelectionLayer()
{
	for (int nIdxItem = 0; nIdxItem < GetNbLayers(); nIdxItem++)
	{
		CxImage *pLayer = GetLayer(nIdxItem);
		if (pLayer->SelectionIsValid())
			return pLayer;
	}
	return NULL;
}

bool IGFrame::IsSelection()
{
	return GetSelectionLayer() != NULL;
}

bool IGFrame::Update()
{
	if (!UpdateSize (head.biWidth, head.biHeight))
		return false;
	// update selection rect
	RECT rcSel = {0, 0, -1, -1};
	CxImage *pLayer = GetWorkingLayer();
	if (pLayer->SelectionIsValid())
		pLayer->SelectionGetBox (rcSel);
	if (rcSel.right != -1 && rcSel.bottom != -1){
		long nOffsetX, nOffsetY;
		pLayer->GetOffset (&nOffsetX, &nOffsetY);
		rcSel.left += nOffsetX; rcSel.right += nOffsetX;
		rcSel.top -= nOffsetY; rcSel.bottom -= nOffsetY;
	}
	wchar_t twLeft [32], twRight [32], twTop [32], twBottom [32];
	::_itow (rcSel.left, twLeft, 10);
	::_itow (rcSel.right, twRight, 10);
	::_itow (rcSel.top, twTop, 10);
	::_itow (rcSel.bottom, twBottom, 10);		
	wstring wsSelRect (twLeft);
	wsSelRect += L"#"; wsSelRect+= twRight;
	wsSelRect += L"#"; wsSelRect+= twTop;
	wsSelRect += L"#"; wsSelRect+= twBottom;
	SetProperty (IGFRAMEPROPERTY_SELECTIONRECT, wsSelRect);
	// update current history step id
	SetProperty (IGFRAMEPROPERTY_HISTORYSTEP, GetCurrentStepId());
	return true;
}

bool IGFrame::OnFrameSizeChanged (int nNewWidth, int nNewHeight, IGFRAMEPROPERTY_STRETCHING_ENUM stretchMode)
{
	// Request thread access
	if (!RequestAccess ())
		return false;
	IGInternalProcessing *pResampleImageProcessing = new IGInternalProcessing(this, new IGInternalMessageResize());
	return pResampleImageProcessing->Start (false);	// synchronous
}

bool IGFrame::PickColor (POINT ptCoords, RGBQUAD& rgbColor)
{
	// Request thread access
	if (!RequestAccess ())
		return false;
	CxImage *pCurrentLayer = GetWorkingLayer();
	if (!pCurrentLayer)
		return false;
	// convert IG coordinates to Cx coordinates
	IGConvertible::FromIGtoCxCoords (ptCoords, pCurrentLayer->GetHeight());
	rgbColor = pCurrentLayer->GetPixelColor (ptCoords.x, ptCoords.y);
	return true;
}