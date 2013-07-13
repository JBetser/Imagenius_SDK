#include "StdAfx.h"
#include "ximathread.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGImageProcMessages.h"
#include "IGProperties.h"
#include "IGFaceDescriptor.h"

using namespace IGLibrary;

IGLayer::IGLayer (LPWSTR pwPath, IGFrame *pFrame)	: m_bSelected (false)
													, m_bVisible (true)
													, m_nId (-1)
{	
	Load (pwPath);
	SelectionCreate();
	info.pParent = (CxImageThread*)pFrame;
}

IGLayer::IGLayer (IGFrame *pFrame, bool bAlpha, bool bSelection)	: m_bSelected (false)
																	, m_bVisible (true)
																	, m_nId (-1)
{
	if (pFrame)
	{
		Create (pFrame->GetWidth(), pFrame->GetHeight(), 24, CXIMAGE_FORMAT_BMP);
		info.pParent = (CxImageThread*)pFrame;
	}
	else
	{
		Startup();
	}
	if (bAlpha)
		AlphaCreate(0);
	if (bSelection)
		SelectionCreate();
}

IGLayer::IGLayer (HANDLE hMem, IGFrame *pFrame)	: m_bSelected (false)
												, m_bVisible (true)
												, m_nId (-1)
{
	CreateFromHANDLE (hMem);
	SelectionCreate();
	info.pParent = (CxImageThread*)pFrame;
}

IGLayer::IGLayer (HBITMAP hBmp, IGFrame *pFrame)	: m_bSelected (false)
													, m_bVisible (true)
													, m_nId (-1)
{
	CreateFromHBITMAP (hBmp);
	SelectionCreate();
	info.pParent = (CxImageThread*)pFrame;
}

IGLayer::~IGLayer(void)
{
}

bool IGLayer::RequestThreadAccess ()
{
	if (!info.pParent)
		return true;
	return info.pParent->RequestAccess();
}

bool IGLayer::Render (HDC hdc, int nLeft, int nTop, int nWidth, int nHeight)
{
	if (RequestThreadAccess())
	{
		if (info.pParent)
		{
			IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);	
			// avoid drawing selection in layer rendering
			int nCurWorkingPos = pFrame->GetLayerPos (pFrame->GetWorkingLayer());
			pFrame->SetWorkingLayer(-1);
			long nRes = LayerDrawAllInClearedCache (hdc, 0, 0,
													nWidth, nHeight, NULL,
													false, true);
			pFrame->SetWorkingLayer(nCurWorkingPos);
			if (!nRes)
				return false;
		}
	}
	return (LayerDrawAllFromCache (hdc, nLeft, nTop) == 1);
}

long IGLayer::Draw (HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth, bool bDrawChessBackGround, bool bMixAlpha)
{
	if (!RequestThreadAccess ())
		return 0;
	if (!m_bVisible)
		return 1;
	IGLibrary::IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM eShowSegmType = IGLibrary::IGFRAMEPROPERTY_SHOWSEGMENTATION_NO;
	return GetWorking()	? (GetSegmentationLayer (eShowSegmType) ?  (CxImage::DrawWithSegmentation (hdc, x, y, cx, cy, pClipRect, bSmooth, bDrawChessBackGround, bMixAlpha) == 1)
							: (CxImage::DrawWithSelection (hdc, x, y, cx, cy, pClipRect, bSmooth, bDrawChessBackGround, bMixAlpha) == 1) )
							: (CxImage::Draw (hdc, x, y, cx, cy, pClipRect, bSmooth, bDrawChessBackGround, bMixAlpha) == 1);
}

HANDLE IGLayer::CopyToHandle (bool bCopyAlpha)
{
	if (!RequestThreadAccess ())
		return NULL;
	return convertDDBToDIB (bCopyAlpha ? MakeBitmap (NULL, -1, -1, false, true, false) : MakeBitmap (), BI_RGB, bCopyAlpha);
}

bool IGLayer::Load (const TCHAR* filename, bool bAutoRotate)
{
	// Request thread access
	if (!RequestThreadAccess ())
		return false;
	if (!info.pParent)
		return false;

	if (info.pParent->IsStarted())
	{
		IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);
		int nPosLayer = pFrame->GetLayerPos (this);
		if (nPosLayer < 0)
			return false;
		IGImageProcessing *pLoadImageProcessing = new IGImageProcessing (	pFrame, 
																			new IGImageLoadMessage (filename, bAutoRotate, nPosLayer));
		return pLoadImageProcessing->Start();
	}
	return CxImage::Load (filename, IGFrame::GetImageType (filename));
}

bool IGLayer::Save (const TCHAR* filename, DWORD imagetype)
{
	if (imagetype == CXIMAGE_FORMAT_DZ)
	{	
		// start DZ threads
		if ((info.nNbDZThreads == 0) || (info.nNbDZSubThreads == 0) || !Start())
			return false;
		// in case of deepzoom, write also the xml descriptor file
		wstring wsOutputDZPath (filename);
		wsOutputDZPath += L".xml";
		if (m_spConfigMgr->InterlockSaveDeepZoomConfig (GetWidth(), GetHeight(), wsOutputDZPath) != S_OK)
			return false;
		::wcscpy_s (info.twOutputPath, filename);
		bool bRet = Encode((CxFile*)NULL, imagetype);	// no file in parameter in case of deepzoom
		// terminate DZ threads
		bRet &= Exit();
		return bRet;
	}
	return CxImage::Save (filename, imagetype);
}

bool IGLayer::HasBeen (int nId)
{	
	for (set <int>::const_iterator itLayIdx = m_setHistoryLayerIds.begin();
		itLayIdx != m_setHistoryLayerIds.end();	++itLayIdx)
	{
		if (*itLayIdx == nId)
			return true;
	}
	return false;
}

bool IGLayer::SetId (int nId) 
{
	if (m_nId == nId)
		return true;
	if (!info.pParent)
		return false;
	bool bRet = true;
	if (nId < 0)
		bRet = false;
	IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);		
	if (bRet)
	{
		IGLayer *pExistingLayer = pFrame->GetLayerFromId (nId);
		if (pExistingLayer != NULL)
		{// the layer id is already used
			if (pExistingLayer != this)
			{
				if ((int)pExistingLayer->GetId() == nId)
					bRet = false;
				else
					pExistingLayer->RemoveFromHistory (nId);
			}
		}
	}
	if ((m_nId >= 0) && !HasBeen (m_nId))
		m_setHistoryLayerIds.insert (m_nId);
	m_nId = nId;
	return bRet;
}

void IGLayer::RemoveFromHistory (int nId)
{
	m_setHistoryLayerIds.erase (m_nId);
}

void IGLayer::setFrameOwner (IGFrame *pFrame, bool bAddCalc)
{
	info.pParent = (CxImageThread*)pFrame;
	if (pFrame && bAddCalc)
		pFrame->AddCalc (this);
}

RECT IGLayer::GetUpdateRect() const
{
	return info.rcUpdateBox;
}

void IGLayer::SetWorking()
{		
	if (info.pParent)
	{
		IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);
		pFrame->SetWorkingLayer (pFrame->GetLayerPos (this));
	}
}

bool IGLayer::GetWorking() const
{
	if (info.pParent)
	{
		IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);
		return (pFrame->GetWorkingLayer() == this);
	}
	return false;
}

IGSmartPixel* IGLayer::GetSegmentationLayer (IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM& eShowSegmType)
{
	eShowSegmType = IGFRAMEPROPERTY_SHOWSEGMENTATION_NO;
	if (info.pParent)
	{
		IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);
		pFrame->GetProperty (IGFrameProperties::IGFRAMEPROPERTY_SHOW_SEGMENTATION, eShowSegmType);
		if (eShowSegmType == IGFRAMEPROPERTY_SHOWSEGMENTATION_NO)
			return NULL;
		return GetPixels();
	}
	return NULL;
}

bool IGLayer::Delete()
{
	return DeletePixels(false);
}

bool IGLayer::SmartDelete()
{
	return DeletePixels(true);
}

bool IGLayer::AutoRotate()
{
	if (!info.pParent)
		return false;
	bool bRet = IGSmartLayer::AutoRotate();
	IGFrame* pFrame = static_cast <IGFrame*> (info.pParent);
	pFrame->SetProperty (IGFrameProperties::IGFRAMEPROPERTY_FACEDESCRIPTOR, *m_pFaceDescriptor);
	return bRet;
}