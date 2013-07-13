// xImaLyr.cpp : Layers functions
/* 21/04/2003 v1.00 - Davide Pizzolato - www.xdp.it
 * CxImage version 6.0.0 02/Feb/2008
 */
#include "stdafx.h"
#include "ximage.h"


#if CXIMAGE_SUPPORT_LAYERS

////////////////////////////////////////////////////////////////////////////////
/**
 * If the object is an internal layer, GetParent return its parent in the hierarchy.
 */
CxImage* CxImage::GetParent() const
{
	return dynamic_cast <CxImage*> (info.pParent);
}

void CxImage::SetParent (CxImage* pParent)
{
	info.pParent = pParent;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Number of layers allocated directly by the object.
 */
long CxImage::GetNumLayers() const
{
	return info.nNumLayers;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Creates an empty layer. If position is less than 0, the new layer will be placed in the last position
 */
bool CxImage::LayerCreate(long position)
{
	if ( position < 0 || position > info.nNumLayers ) position = info.nNumLayers;

	CxImage *pNewLayer = new CxImage();
	if (info.nNumLayers > 0)
		pNewLayer->Copy (*GetLayer (0), false, false, false, false);

	std::list<CxImage*>::const_iterator iterPict = lpLayers.begin(); 
	for (long nCurPos = 0; iterPict != lpLayers.end() && nCurPos < position;
		nCurPos++)
	{
		++iterPict;
	}
	lpLayers.insert (iterPict, pNewLayer);
	info.nNumLayers = lpLayers.size();
	return true;
}

bool CxImage::LayerCreate(long position, CxImage *pNewLayer, long x, long y)
{
	if ( position < 0 || position > info.nNumLayers ) position = info.nNumLayers;

	std::list<CxImage*>::const_iterator iterPict = lpLayers.begin(); 
	for (long nCurPos = 0; iterPict != lpLayers.end() && nCurPos < position;
		nCurPos++)
	{
		++iterPict;
	}
	pNewLayer->info.xOffset = x;
	pNewLayer->info.yOffset = y;
	lpLayers.insert (iterPict, pNewLayer);
	info.nNumLayers = lpLayers.size();
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Deletes a layer. If position is less than 0, the last layer will be deleted
 */
bool CxImage::LayerDelete(long position, CxImage **ppDeletedLayer)
{
	if ( position >= info.nNumLayers ) return false;
	if ( position < 0) position = info.nNumLayers - 1;
	if ( position < 0) return false;

	// search the layer
	std::list<CxImage*>::const_iterator iterPict = lpLayers.begin(); 
	for (long nCurPos = 0; iterPict != lpLayers.end() && nCurPos < position;
		nCurPos++)
	{
		++iterPict;
	}
	if (iterPict == lpLayers.end())
		return false;

	// remove the layer from the list
	CxImage *pDeletedLayer = *iterPict;
	lpLayers.remove (pDeletedLayer);
	if (ppDeletedLayer)
		*ppDeletedLayer = pDeletedLayer;
	else
		delete pDeletedLayer;

	// update number of layers
	info.nNumLayers = lpLayers.size();
	return true;
}
////////////////////////////////////////////////////////////////////////////////
void CxImage::LayerDeleteAll()
{
	if (!lpLayers.empty())
	{
		for (std::list<CxImage*>::const_iterator iterPict = lpLayers.begin(); 
			iterPict != lpLayers.end(); ++iterPict)
			delete *iterPict;
		lpLayers.clear();
	}
	info.nNumLayers = 0;
}

void CxImage::PushAndSortLayers()
{
	lpPushedLayers = lpLayers;
	// bubble sort
	std::list<CxImage*> lpSortedLayers;
	for (int idxLayer = 0; idxLayer < info.nNumLayers; idxLayer++){
		CxImage* pCurLayer = GetLayer(idxLayer);
		std::list<CxImage*>::const_iterator iterSortLay = lpSortedLayers.begin();
		for (; iterSortLay != lpSortedLayers.end(); ++iterSortLay)
		{
			if (pCurLayer->GetId() < (*iterSortLay)->GetId())
				break;
		}
		lpSortedLayers.insert (iterSortLay, pCurLayer);
	}
	lpLayers = lpSortedLayers;
}

void CxImage::PullLayers()
{
	lpLayers = lpPushedLayers;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Returns a pointer to a layer. If position is less than 0, the last layer will be returned
 */
CxImage* CxImage::GetLayer(long position) const
{
	if ( info.nNumLayers == 0) return NULL;
	if ( position >= info.nNumLayers ) return NULL;
	if ( position < 0) position = info.nNumLayers - 1;

	std::list<CxImage*>::const_iterator iterPict = lpLayers.begin(); 
	for (long nCurPos = 0; iterPict != lpLayers.end() && nCurPos < position;
		nCurPos++)
	{
		++iterPict;
	}

	return *iterPict;
}

long CxImage::GetLayerPos(const CxImage *pLayer) const
{
	long nPos = 0;
	for (std::list<CxImage*>::const_iterator iterPict = lpLayers.begin();
		iterPict != lpLayers.end(); ++iterPict, ++nPos)
	{
		if (*iterPict == pLayer)
			return nPos;
	}
	return -1;
}

/**
 * DeepZoom update - Call that method to generate all tiled pictures
 */
RECT CxImage::GetUpdateRect() const
{
	RECT rcFull = {0, 0, -1, -1};
	RECT rcUpd = {0, 0, 0, 0};
	_ASSERTE ((lpLayers.size() > 0) && L"CxImage::GetUpdateRect Error: no layer available");
	if (lpLayers.size() > 0)
	{
		for (std::list<CxImage*>::const_iterator iterPict = lpLayers.begin();
			iterPict != lpLayers.end(); ++iterPict)
		{
			long nXOffset, nYOffset;
			(*iterPict)->GetOffset (&nXOffset, &nYOffset);
			rcUpd.left += nXOffset; rcUpd.right += nXOffset;
			rcUpd.top -= nYOffset; rcUpd.bottom -= nYOffset;
			if ((*iterPict)->GetSelected())
			{
				RECT rcLaySel;
				(*iterPict)->SelectionGetBox (rcLaySel);
				if (rcLaySel.bottom == -1 || rcLaySel.right == -1){
					rcUpd = rcFull;
				}
				else {
					rcUpd.left += rcLaySel.left; rcUpd.right += rcLaySel.right;
					rcUpd.top += rcLaySel.top; rcUpd.bottom += rcLaySel.bottom;
				}
				return rcUpd;
			}
		}
	}
	return rcFull;
}

void CxImage::SetFullUpdate()
{
	info.bFullDZUpdate = true;
	info.rcUpdateBox.top = 0;info.rcUpdateBox.bottom = -1;
	info.rcUpdateBox.left = 0;info.rcUpdateBox.right = -1;
	info.rcLastUpdateBox.top = 0;info.rcLastUpdateBox.bottom = -1;
	info.rcLastUpdateBox.left = 0;info.rcLastUpdateBox.right = -1;	
}

void CxImage::SetUpdateRect (const RECT& rcUpdate)
{
	info.rcUpdateBox = rcUpdate;	
}

void CxImage::SetLastUpdateRect (const RECT& rcUpdate)
{
	info.rcLastUpdateBox = rcUpdate;
}

const RECT& CxImage::GetLastUpdateRect() const
{
	return info.rcLastUpdateBox;
}

////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_WINDOWS

long CxImage::LayerDrawAll(HDC hdc, const RECT& rect, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bMixAlpha)
{
	return LayerDrawAll(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pClipRect, bSmooth, bShowChessBackground, bMixAlpha);
}

long CxImage::LayerDrawAll(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bMixAlpha)
{
	GdiFlush ();
	long n=0;
	CxImage* pLayer;
	int nLayerLeft = 0;
	int nLayerTop = 0;
	int nLayerWidth = 0;
	int nLayerHeight = 0;
	while (pLayer = GetLayer(n++))
	{
		nLayerWidth = (int)((float)pLayer->head.biWidth * info.fZoomCurrent);
		nLayerHeight = (int)((float)pLayer->head.biHeight * info.fZoomCurrent);
		nLayerLeft = x + (int)((float)pLayer->info.xOffset * info.fZoomCurrent);
		nLayerTop = y + (int)((float)pLayer->info.yOffset * info.fZoomCurrent);	
		if (pLayer->Draw (hdc,	nLayerLeft,
								nLayerTop,
								nLayerWidth,
								nLayerHeight,
								pClipRect,
								bSmooth, 
								(bShowChessBackground && n == 1),
								bMixAlpha) == 0)
			return 0;
	}
	return 1;
}

#endif //CXIMAGE_SUPPORT_WINDOWS
#endif //CXIMAGE_SUPPORT_LAYERS
