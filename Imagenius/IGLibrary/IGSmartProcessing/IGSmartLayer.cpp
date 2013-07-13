#include "stdafx.h"
#include "IGLibrary.h"
#include "IGSmartLayer.h"
#include "IGFaceEffect.h"
#include "IGException.h"
#include "IGSplashWindow.h"
#include "IGFaceDescriptor.h"
#include "FaceDetector.h"
#include "ximaDZ.h"
#include <algorithm>
#include <map>

using namespace std;
using namespace IGLibrary;

IGException SmartLayerException ("IGSmartLayer");

IGSmartLayer::IGSmartLayer() : m_bReconstructionNeeded (true)
								, m_pPixels (NULL)
								, m_pLayerSkinFiltered (NULL)
								, m_pFaceDescriptor (NULL)
{
}

IGSmartLayer::IGSmartLayer (const CxImage& cpy) : CxImage (cpy)
												, m_bReconstructionNeeded (true)
												, m_pPixels (NULL)
												, m_pLayerSkinFiltered (NULL)
												, m_pFaceDescriptor (NULL)
{
}

IGSmartLayer::IGSmartLayer (const IGSmartLayer& cpy) : CxImage (cpy)
	, m_bReconstructionNeeded (true)
	, m_pPixels (NULL)
	, m_pLayerSkinFiltered (NULL)
	, m_pFaceDescriptor (NULL)
{
	for (vector <IGRegion*>::const_iterator itReg = cpy.m_vRegions.begin(); itReg != cpy.m_vRegions.end(); ++itReg){
		IGRegion *pCpyRegion = new IGRegion (**itReg);
		pCpyRegion->m_vPixels.clear();
		pCpyRegion->m_vBoundaries.clear();
		for (vector <IGSmartPixel*>::iterator itPixel = (*itReg)->m_vPixels.begin(); itPixel != (*itReg)->m_vPixels.end(); ++itPixel){
			IGSmartPixel *pCopyPixel = new IGSmartPixel (**itPixel);
			POINT pt; pt.x = pCopyPixel->m_nX; pt.y = pCopyPixel->m_nY;
			pCopyPixel->LinkToPixel (GetPixelColor24 (pt), SelectionGetPointer (pCopyPixel->m_nX, pCopyPixel->m_nY), pCopyPixel->m_nX, pCopyPixel->m_nY);
			pCpyRegion->AddPixel (pCopyPixel);
		}
		m_vRegions.push_back (pCpyRegion);
	}
	if (cpy.m_pFaceDescriptor)
		m_pFaceDescriptor = new IGFaceDescriptor (*cpy.m_pFaceDescriptor);
}

void IGSmartLayer::CopyFaceDetection (const IGSmartLayer& cpy)
{
	for (vector <IGRegion*>::const_iterator itReg = cpy.m_vRegions.begin(); itReg != cpy.m_vRegions.end(); ++itReg){
		IGRegion *pCpyRegion = new IGRegion (**itReg);
		pCpyRegion->m_vPixels.clear();
		pCpyRegion->m_vBoundaries.clear();
		for (vector <IGSmartPixel*>::iterator itPixel = (*itReg)->m_vPixels.begin(); itPixel != (*itReg)->m_vPixels.end(); ++itPixel){
			IGSmartPixel *pCopyPixel = new IGSmartPixel (**itPixel);
			POINT pt; pt.x = pCopyPixel->m_nX; pt.y = pCopyPixel->m_nY;
			pCopyPixel->LinkToPixel (GetPixelColor24 (pt), SelectionGetPointer (pCopyPixel->m_nX, pCopyPixel->m_nY), pCopyPixel->m_nX, pCopyPixel->m_nY);
			pCpyRegion->AddSimplePixel (pCopyPixel);
		}
		m_vRegions.push_back (pCpyRegion);
	}
	if (cpy.m_pFaceDescriptor)
		m_pFaceDescriptor = new IGFaceDescriptor (*cpy.m_pFaceDescriptor);
}

IGSmartLayer::~IGSmartLayer()
{
	if (!m_pPixels){
		for (vector <IGRegion*>::const_iterator itReg = m_vRegions.begin(); itReg != m_vRegions.end(); ++itReg)
			for (vector <IGSmartPixel*>::iterator itPixel = (*itReg)->m_vPixels.begin(); itPixel != (*itReg)->m_vPixels.end(); ++itPixel)
				delete *itPixel;
	}
	if (m_pPixels)
		delete [] m_pPixels;
	if (m_pLayerSkinFiltered)
		delete m_pLayerSkinFiltered;
	for (vector <vector <IGSmartPixel*>>::reverse_iterator itRevLevel = m_vvTreePixels.rbegin(); itRevLevel != m_vvTreePixels.rend(); ++itRevLevel)
		for (vector <IGSmartPixel*>::iterator itPixel = (*itRevLevel).begin(); itPixel != (*itRevLevel).end(); ++itPixel)
			delete *itPixel;
	if (m_vvTreeRegions.size() > 0){
		while (m_vvTreeRegions.size() > 0){
			vector <IGRegion*> vTreeRegion = m_vvTreeRegions.back();
			while (vTreeRegion.size() > 0){
				delete vTreeRegion.back();
				vTreeRegion.pop_back();
			}
			m_vvTreeRegions.pop_back();
		}
	}
	else
	{
		while (!m_vRegions.empty())
		{
			delete m_vRegions.back();
			m_vRegions.pop_back();
		}
	}
	if (m_pFaceDescriptor)
		delete m_pFaceDescriptor;
}

void IGSmartLayer::destroyFaceDescriptor()
{
	if (m_pFaceDescriptor){
		delete m_pFaceDescriptor;
		m_pFaceDescriptor = NULL;
	}
}

bool IGSmartLayer::SelectionCopyFromScaledLayer (CxImage& layerScaled)
{
	IGSmartLayer& smartLayerScaled = dynamic_cast <IGSmartLayer&> (layerScaled);
	float fScale = (float)head.biWidth / (float)smartLayerScaled.GetWidth();
	m_vSkinPixels.clear();
	if (pSelection)
		delete [] pSelection;
	pSelection = new BYTE [head.biWidth * head.biHeight];
	BYTE *pCurSel = pSelection;
	for (int y = 0; y < head.biHeight; y++){
		for (int x = 0; x < head.biWidth; x++){
			int xScl = (int)((float)x/fScale);
			int yScl = (int)((float)y/fScale);
			*pCurSel++ = smartLayerScaled.SelectionGet(xScl, yScl);
		}
	}
	m_vBoundaries = smartLayerScaled.m_vBoundaries;
	for (vector <IGSmartPixel*>::iterator it = m_vBoundaries.begin(); it != m_vBoundaries.end(); ++it){
		(*it)->m_nX = (int)((float)(*it)->m_nX * fScale);
		(*it)->m_nY = (int)((float)(*it)->m_nY * fScale);
	}
	for (vector <IGRegion*>::iterator it = smartLayerScaled.m_vRegions.begin(); it != smartLayerScaled.m_vRegions.end(); ++it){
		IGRegion *pRegion = new IGRegion ((*it)->m_nLabel, (*it)->m_bIsFace);
		pRegion->m_rcReg.left = (int)((float)(*it)->m_rcReg.left * fScale);
		pRegion->m_rcReg.right = (int)((float)(*it)->m_rcReg.right * fScale);
		pRegion->m_rcReg.top = (int)((float)(*it)->m_rcReg.top * fScale);
		pRegion->m_rcReg.bottom = (int)((float)(*it)->m_rcReg.bottom * fScale);
		pRegion->m_rcEyeLeft.left = (int)((float)(*it)->m_rcEyeLeft.left * fScale);
		pRegion->m_rcEyeLeft.right = (int)((float)(*it)->m_rcEyeLeft.right * fScale);
		pRegion->m_rcEyeLeft.top = (int)((float)(*it)->m_rcEyeLeft.top * fScale);
		pRegion->m_rcEyeLeft.bottom = (int)((float)(*it)->m_rcEyeLeft.bottom * fScale);
		pRegion->m_rcEyeRight.left = (int)((float)(*it)->m_rcEyeRight.left * fScale);
		pRegion->m_rcEyeRight.right = (int)((float)(*it)->m_rcEyeRight.right * fScale);
		pRegion->m_rcEyeRight.top = (int)((float)(*it)->m_rcEyeRight.top * fScale);
		pRegion->m_rcEyeRight.bottom = (int)((float)(*it)->m_rcEyeRight.bottom * fScale);
		pRegion->m_rcMouth.left = (int)((float)(*it)->m_rcMouth.left * fScale);
		pRegion->m_rcMouth.right = (int)((float)(*it)->m_rcMouth.right * fScale);
		pRegion->m_rcMouth.top = (int)((float)(*it)->m_rcMouth.top * fScale);
		pRegion->m_rcMouth.bottom = (int)((float)(*it)->m_rcMouth.bottom * fScale);
		AddRegion (pRegion);
	}
	return true;
}

void IGSmartLayer::UpdateSize (int nWidth, int nHeight)
{
	head.biWidth = nWidth;
	head.biHeight = nHeight;
	info.dwEffWidth = ((((head.biBitCount * nWidth) + 31) / 32) * 4);
	if (m_pPixels){
		delete [] m_pPixels;
		m_pPixels = NULL;
	}
	m_bReconstructionNeeded = true;
}

bool IGSmartLayer::IndexLPE (IGMarker **ppMarkers, int& nNbMarkers, bool bShowMarkerMode)
{
	if (!Construct8())
		return false;
	if (!ComputeGradient(false))
		return false;
	if (ppMarkers && nNbMarkers > 0)
		return LPE_FAH (ppMarkers, nNbMarkers, bShowMarkerMode);
	while (m_vvTreeRegions.size() > 0){
		vector <IGRegion*> vTreeRegion = m_vvTreeRegions.back();
		while (vTreeRegion.size() > 0){
			delete vTreeRegion.back();
			vTreeRegion.pop_back();
		}
		m_vvTreeRegions.pop_back();
	}
	for (vector <vector <IGSmartPixel*>>::reverse_iterator itRevLevel = m_vvTreePixels.rbegin(); itRevLevel != m_vvTreePixels.rend(); ++itRevLevel)
		for (vector <IGSmartPixel*>::iterator itPixel = (*itRevLevel).begin(); itPixel != (*itRevLevel).end(); ++itPixel)
			delete *itPixel;
	m_vvTreePixels.clear();
	nNbMarkers = 0;
	if (!LPE_TopDown (nNbMarkers))
		return false;
	nNbMarkers = m_vRegions.size();
	return true;
	/*
	ppMarkers = new IGMarker*[nNbMarkers];
	int nIdxReg = 0;
	for (vector <IGRegion*>::iterator itReg = m_vRegions.begin(); itReg != m_vRegions.end(); ++itReg, ++nIdxReg){
		ppMarkers[nIdxReg] = new IGMarker(nIdxReg);
		for (vector <IGSmartPixel*>::iterator itPix = (*itReg)->m_vPixels.begin(); itPix != (*itReg)->m_vPixels.end(); ++itPix){
			if ((*itPix)->m_gradient == (*itReg)->m_minGradient)
				ppMarkers[nIdxReg]->AddSimplePixel(*itPix);
		}		
	}
	return LPE_FAH (ppMarkers, nNbMarkers, bShowMarkerMode);*/
	/*
	vector <vector <IGRegion*>>::reverse_iterator itRevRegLevel = m_vvTreeRegions.rbegin();
	for (vector <vector <IGSmartPixel*>>::reverse_iterator itRevLevel = m_vvTreePixels.rbegin(); itRevLevel != m_vvTreePixels.rend(); ++itRevLevel) {
		if (nNbRegs > (int)(*itRevRegLevel).size()){
			++itRevRegLevel;
			continue;
		}
		int nCurNbRegs = (*itRevRegLevel).size();
		multiset <IGSmartPixel> setPixels;
		vector <IGSmartPixel*>::const_iterator itPixel = (*itRevLevel).begin();
		do{
			setPixels.insert (**itPixel);
		}while (++itPixel != (*itRevLevel).end());
		for (multiset <IGSmartPixel>::iterator itSortPixel = setPixels.begin(); itSortPixel != setPixels.end(); ++itSortPixel) {
			(*itSortPixel).GetChildRegion2()->m_nLabel = (*itSortPixel).GetChildRegion()->m_nLabel;
			if (--nCurNbRegs == nNbRegs)
				break;
		}
		for (vector <IGRegion*>::const_iterator itReg = (*itRevRegLevel).begin(); itReg != (*itRevRegLevel).end(); ++itReg)
			(*itReg)->SetLabel ((*itReg)->m_nLabel, true);
		break;
	}*/
}

bool IGSmartLayer::SelectEyes (BYTE level)
{
	if (m_vRegions.size() < 1)
		return false;
	for (std::vector <IGRegion*>::iterator itRegion = m_vRegions.begin(); itRegion != m_vRegions.cend(); ++itRegion){
		if ((*itRegion)->IsFace()){
			RECT rcLeft,rcRight;
			(*itRegion)->GetEyes (rcLeft,rcRight);
			SelectionAddRect (rcLeft, level);
			SelectionAddRect (rcRight, level);
		}
	}
	SelectionRebuildBox();
	if (!SelectionIsValid())
		return false;
	return true;
}

bool IGSmartLayer::SelectMouth (BYTE level)
{
	if (m_vRegions.size() < 1)
		return false;
	for (std::vector <IGRegion*>::iterator itRegion = m_vRegions.begin(); itRegion != m_vRegions.cend(); ++itRegion){
		if ((*itRegion)->IsFace()){
			RECT rcMouth;
			(*itRegion)->GetMouth (rcMouth);
			SelectionAddRect (rcMouth, level);
		}
	}
	SelectionRebuildBox();
	if (!SelectionIsValid())
		return false;
	return true;
}

bool IGSmartLayer::SelectNoze (BYTE level)
{
	if (m_vRegions.size() < 1)
		return false;
	for (std::vector <IGRegion*>::iterator itRegion = m_vRegions.begin(); itRegion != m_vRegions.cend(); ++itRegion){
		if ((*itRegion)->IsFace()){
			RECT rcMouth;
			(*itRegion)->GetNoze (rcMouth);
			SelectionAddRect (rcMouth, level);
		}
	}
	SelectionRebuildBox();
	if (!SelectionIsValid())
		return false;
	return true;
}

bool IGSmartLayer::Resample()
{
	float fScale;
	if (head.biHeight < head.biWidth){
		fScale = (float)IGSMARTLAYER_INDEXSAMPLE_SIZE / (float)head.biWidth;
		if (fScale < 1.0f) {
			if (!CxImage::Resample (IGSMARTLAYER_INDEXSAMPLE_SIZE,  (int)((float)head.biHeight * fScale) + 1))
				return false;
		}
	}
	else{
		fScale = (float)IGSMARTLAYER_INDEXSAMPLE_SIZE / (float)head.biHeight;
		if (fScale < 1.0f) {
			if (!CxImage::Resample ((int)((float)head.biWidth * fScale) + 1, IGSMARTLAYER_INDEXSAMPLE_SIZE))
				return false;
		}
	}
	return true;
}

void IGSmartLayer::rotatePt (int nRot, int nPtCtrX, int nPtCtrY, int nPtX, int nPtY, int& nRotatedX, int& nRotatedY)
{
	// change of vectorial space, set the origin to the center
	int nX = nPtX - nPtCtrX;
	int nY = nPtY - nPtCtrY;
	// rotate
	float fXTr = cosf((float)nRot * PI / 180.0f);
	float fYTr = sinf((float)nRot * PI / 180.0f);
	nRotatedX = (int)(nX * fXTr + nY * fYTr);
	nRotatedY = (int)(-nX * fYTr + nY * fXTr);
	// change of vectorial space, back to the origin
	nRotatedX = nRotatedX + nPtCtrX;
	nRotatedY = nRotatedY + nPtCtrY;
}

bool IGSmartLayer::IndexFaces (int nDescriptorIdx)
{
	// creation of skin layer
	ProgressSetRange(10);
	if (m_pLayerSkinFiltered)
		delete m_pLayerSkinFiltered;
	m_pLayerSkinFiltered = new IGSmartLayer();
	m_pLayerSkinFiltered->Copy (*this, true, false, false, false);
	ProgressStepIt();
	if (!m_pLayerSkinFiltered->GrayScale())
		return false;
	ProgressStepIt();
	// ANN face detection
	list <Face> lFaces;
	if (nDescriptorIdx == -1) {
		if (!m_pFaceDescriptor)
			DetectFaces();
		m_pFaceDescriptor->GetFaces (lFaces);
	}
	else
		lFaces.push_back (m_pFaceDescriptor->GetFace(nDescriptorIdx));
	ProgressStepIt();
	// creation of background layer around the faces
	std::list <RECT> lCircleFaces;
	for (std::list <Face>::iterator it = lFaces.begin(); it != lFaces.end(); ++it){
		RECT rcFace = (*it).getFaceCoords();
		RECT rcCircleFace;
		rcCircleFace.left = (rcFace.left + rcFace.right) / 2;
		rcCircleFace.top = (rcFace.top + rcFace.bottom) / 2;
		rcCircleFace.right = (rcFace.right - rcFace.left);
		rcCircleFace.bottom = (rcFace.bottom - rcFace.top);
		lCircleFaces.push_back (rcCircleFace);
	}
	std::list <IGSmartLayer *> lLayerFaces;
	IGLibrary::IGSmartLayer layerBackground (*m_pLayerSkinFiltered);
	::memset (layerBackground.GetBits(), 0xFF, layerBackground.GetWidth() * layerBackground.GetHeight());
	std::list <Face>::iterator it = lFaces.begin();
	RGBQUAD grey255 = {255,255,255,255};
	RGBQUAD black255 = {0,0,0,0};
	float fScaleWidth = (float)(layerBackground.GetWidth()) / (float)(m_pFaceDescriptor->GetWidth());
	float fScaleHeight = (float)(layerBackground.GetHeight()) / (float)(m_pFaceDescriptor->GetHeight());
	for (std::list <RECT>::iterator itC = lCircleFaces.begin(); itC != lCircleFaces.end(); ++itC, ++it){
		RECT& rcCircleFace = (*itC);
		for (std::list <RECT>::iterator itC2 = lCircleFaces.begin(); itC2 != lCircleFaces.end(); ++itC2){
			RECT& rcCircleFace2 = (*itC2);
			if (rcCircleFace2.left != rcCircleFace. left || rcCircleFace2.top != rcCircleFace.top || rcCircleFace2.right != rcCircleFace.right){
				int nDiff = (*itC2).right / 2 + (*itC).right / 2 - (int)sqrt ((float)(((*itC2).left - (*itC).left) * ((*itC2).left - (*itC).left) + ((*itC2).top - (*itC).top) * ((*itC2).top - (*itC).top)));
				if (nDiff > 0){
					rcCircleFace.right -= nDiff / 2;
					rcCircleFace2.right -= nDiff / 2;
				}
			}
		}
		RECT rcFace = (*it).getFaceCoords();
		IGSmartLayer *pLayer = new IGSmartLayer (*m_pLayerSkinFiltered);
		::memset (pLayer->GetBits(), 0x00, pLayer->GetWidth() * pLayer->GetHeight());
		RECT rcEnlargedFace;
		rcEnlargedFace.left = (int)(rcFace.left - (rcFace.right - rcFace.left) * 0.66f);
		rcEnlargedFace.right = (int)(rcFace.right + (rcFace.right - rcFace.left) * 0.66f);
		rcEnlargedFace.top = (int)(rcFace.top - (rcFace.bottom - rcFace.top) * 0.66f);
		rcEnlargedFace.bottom = (int)(rcFace.bottom + (rcFace.bottom - rcFace.top) * 0.66f); 
		int nCenterX = (rcEnlargedFace.left + rcEnlargedFace.right) / 2;
		int nCenterY = (rcEnlargedFace.top + rcEnlargedFace.bottom) / 2;
		int nRayHoriz = (rcEnlargedFace.right - rcEnlargedFace.left) / 2;
		int nRayVert = (rcEnlargedFace.bottom - rcEnlargedFace.top) / 2;
		float fEllipticCoeff = (float)nRayVert / (float)nRayHoriz;
		IGSTRUCTELEM_ELLIPSE ellipseFunc (&fEllipticCoeff);
		for (int x = 0; x < (int)pLayer->GetWidth(); x++){
			for (int y = 0; y < (int)pLayer->GetHeight(); y++){
				int rx, ry;
				rotatePt (-(*it).getRotation(), m_pFaceDescriptor->GetWidth() / 2, m_pFaceDescriptor->GetHeight() / 2, (int)((float)x / fScaleWidth), (int)((float)y / fScaleHeight), rx, ry);
				if (ellipseFunc (rx - nCenterX, ry - nCenterY, nRayVert)){
					if (rx >= 0 && rx < (int)m_pFaceDescriptor->GetWidth() && ry >= 0 && ry < (int)m_pFaceDescriptor->GetHeight()){
						pLayer->SetPixelColor (x, y, grey255);
						layerBackground.SetPixelColor (x, y, black255);
					}
				}
				// the minimal background marker
				if (x < 10 || x >= (int)pLayer->GetWidth() - 10)
					layerBackground.SetPixelColor (x, y, grey255);
			}
		}
		lLayerFaces.push_back (pLayer);
		rcFace.left = rcCircleFace.left - abs (rcCircleFace.right) / 2;
		rcFace.top = rcCircleFace.top - abs (rcCircleFace.bottom) / 2;
		rcFace.right = rcCircleFace.left + abs (rcCircleFace.right) / 2;
		rcFace.bottom = rcCircleFace.top + abs (rcCircleFace.bottom) / 2;
	}	
	ProgressStepIt();
	// construct the face mask, i.e. the head circles. It is the opposite of the background
	IGLibrary::IGSmartLayer layerSkinMask (layerBackground);
	layerSkinMask.Negative();
	layerBackground.SelectionAddColor (grey255);
	ProgressStepIt();
	// create background marker
	int nNbMarkers = lFaces.size() + 1;
	IGLibrary::IGMarker** tMarkers = new IGLibrary::IGMarker* [nNbMarkers];
	tMarkers [0] = new IGMarker (IGMARKER_BACKGROUND, layerBackground);	
	// create face markers
	std::list <IGSmartLayer *>::iterator itFace = lLayerFaces.begin();
	// pre-segmentation processing
	RECT rcNo = {-1,-1,-1,-1};
	std::list <Face>::iterator itFaceDetect = lFaces.begin();
	for (int idxMarker = 1; idxMarker < nNbMarkers; idxMarker++){
		IGSmartLayer *pLayer = new IGSmartLayer (*m_pLayerSkinFiltered);
		::memset (pLayer->GetBits(), 0x00, pLayer->GetWidth() * pLayer->GetHeight());
		tMarkers [idxMarker] = new IGMarker (IGMARKER_REGION1 + idxMarker - 1, *pLayer, (*itFaceDetect).getRotation());
		// deduce missing eyes
		RECT rcEyeLeft = (*itFaceDetect).getEyeLeftCoords();
		RECT rcEyeRight = (*itFaceDetect).getEyeRightCoords();
		if (rcEyeLeft.left != -1 || rcEyeRight.left != -1){
			RECT rcFace = (*itFaceDetect).getFaceCoords();
			RECT rcEye1, rcEye2; rcEye1 = rcEyeLeft;  rcEye2 = rcEyeRight;
			int nXmiddle = (rcFace.right - rcFace.left) / 2;
			if (rcEye1.left == -1){
				if (nXmiddle > (rcEye2.right - rcEye2.left) / 2){
					rcEye1 = rcEye2;
					rcEye2 = rcNo;
				}
			}
			if (rcEye2.left == -1){
				if (nXmiddle < (rcEye1.right - rcEye1.left) / 2){
					rcEye2 = rcEye1;
					rcEye1 = rcNo;
				}
			}
			if (rcEye1.left == -1){
				rcEye1.left = rcEye2.left - (int)(1.2f*(float)(rcEye2.right - rcEye2.left));
				rcEye1.right = rcEye1.left + rcEye2.right - rcEye2.left;
				rcEye1.top = rcEye2.top;
				rcEye1.bottom = rcEye2.bottom;
				RECT rcIntersectEyeFace;
				rcIntersectEyeFace.left = max ((int)rcEye1.left, (int)rcFace.left + (rcFace.right - rcEye2.right));
				rcIntersectEyeFace.top = max ((int)rcEye1.top, (int)rcFace.top);
				rcIntersectEyeFace.right = min ((int)rcEye1.right, (int)rcFace.right);
				rcIntersectEyeFace.bottom = min ((int)rcEye1.bottom, (int)rcFace.bottom);
				rcEye1 = rcIntersectEyeFace;
			}
			else if (rcEye2.left == -1){
				rcEye2.right = rcEye1.right + (int)(1.2f*(float)(rcEye1.right - rcEye1.left));
				rcEye2.left = rcEye2.right - (rcEye1.right - rcEye1.left);
				rcEye2.top = rcEye1.top;
				rcEye2.bottom = rcEye1.bottom;
				RECT rcIntersectEyeFace;
				rcIntersectEyeFace.left = max ((int)rcEye2.left, (int)rcFace.left);
				rcIntersectEyeFace.top = max ((int)rcEye2.top, (int)rcFace.top);
				rcIntersectEyeFace.right = min ((int)rcEye2.right, (int)rcFace.right - (rcEye1.left - rcFace.left));
				rcIntersectEyeFace.bottom = min ((int)rcEye2.bottom, (int)rcFace.bottom);
				rcEye2 = rcIntersectEyeFace;
			}
			tMarkers[idxMarker]->SetEyes (rcEye1, rcEye2);
			tMarkers[idxMarker]->SetMouth ((*itFaceDetect).getMouthCoords());
			tMarkers[idxMarker]->SetNoze ((*itFaceDetect).getNozeCoords());
		}
		else
			tMarkers[idxMarker]->SetGenericEyes(); // set generic eyes for faces with no detected eyes
		++itFaceDetect;
	}
	ProgressStepIt();
	int nIdxFace = 1;
	for (std::list <Face>::iterator it = lFaces.begin(); it != lFaces.end(); ++it){
		CxImage *pLayer = tMarkers[nIdxFace]->GetLayer();
		RECT rcFace = (*it).getFaceCoords();
		RECT rcEyeLeft = (*it).getEyeLeftCoords();
		RECT rcEyeRight = (*it).getEyeRightCoords();
		int nRayVert = (3 * (max (rcEyeLeft.top, rcEyeRight.top) - rcFace.top)) / 4;
		int nFaceX = (rcFace.right + rcFace.left) / 2;
		int nFaceY = (rcFace.bottom + rcFace.top) / 2;
		float fEllipticCoeff = 3.0f;
		IGSTRUCTELEM_ELLIPSE ellipseFunc (&fEllipticCoeff);	
		for (int x = 0; x < (int)pLayer->GetWidth(); x++){
			for (int y = 0; y < (int)pLayer->GetHeight(); y++){
				int rx, ry;
				rotatePt (-(*it).getRotation(), m_pFaceDescriptor->GetWidth() / 2, m_pFaceDescriptor->GetHeight() / 2, (int)((float)x / fScaleWidth), (int)((float)y / fScaleHeight), rx, ry);
				if (ellipseFunc (rx - nFaceX, ry - nFaceY, nRayVert)){
					if (rx >= 0 && rx < (int)m_pFaceDescriptor->GetWidth() && ry >= 0 && ry < (int)m_pFaceDescriptor->GetHeight())
						pLayer->SetPixelColor (x, y, grey255);
				}
			}
		}
		nRayVert = (3 * (max (rcEyeLeft.top, rcEyeRight.top) - rcFace.top)) / 4;
		RECT rcEyes;
		rcEyes.left = (rcEyeLeft.left + rcEyeLeft.right) / 2;
		rcEyes.right = (rcEyeRight.left + rcEyeRight.right) / 2;
		rcEyes.top = min (rcEyeLeft.top, rcEyeRight.top);
		rcEyes.bottom = max (rcEyeLeft.bottom, rcEyeRight.bottom);
		for (int x = rcEyes.left; x < rcEyes.right; x++){
			for (int y = rcEyes.top; y < rcEyes.bottom; y++){
				int rx, ry;
				rotatePt (-(*it).getRotation(), m_pFaceDescriptor->GetWidth() / 2, m_pFaceDescriptor->GetHeight() / 2, (int)((float)x / fScaleWidth), (int)((float)y / fScaleHeight), rx, ry);
				if (rx >= 0 && rx < (int)m_pFaceDescriptor->GetWidth() && ry >= 0 && ry < (int)m_pFaceDescriptor->GetHeight())
					pLayer->SetPixelColor (x, y, grey255);
			}
		}
		pLayer->SelectionAddColor (grey255);
		nIdxFace++;
		++itFace;
	}
	ProgressStepIt();
	// watershed segmentation
	bool bRes = IndexLPE (tMarkers, nNbMarkers);
	ProgressStepIt();
	// release markers
	for (int nIdxMarker = 0; nIdxMarker < nNbMarkers; nIdxMarker++){
		delete tMarkers [nIdxMarker];
	}
	delete [] tMarkers;
	return bRes;
}

bool IGSmartLayer::ProcessFaceEffect(IGIPFaceEffectMessage *pEffectMessage)
{
	SelectionDelete();

	// sampling
	ProgressSetSubRange (0, 3);			// start sub step 1 / 3
	IGSmartLayer sampledLayer (*this);
	if (!sampledLayer.Resample())
		return false;

	ProgressSetSubRange (1, 3);			// start sub step 2 / 3
	ProgressSetRange(10);
	if (!sampledLayer.IndexFaces())
		return false;

	// process background image processing
	IGFaceEffect *pEffect = NULL;
	if (!IGFaceEffect::Create (pEffectMessage, *this, 0, pEffect))
		return false;
	sampledLayer.SelectIndexedRegions(0);
	sampledLayer.SelectionRebuildBox();
	if (!SelectionCopyFromScaledLayer (sampledLayer))
		return false;
	SelectionRebuildBox();
	pEffect->ProcessBackground();		
	delete pEffect;

	for (int nDescriptorIdx = 1; nDescriptorIdx < sampledLayer.GetNbRegions(); nDescriptorIdx++){		
		int nRot = sampledLayer.GetFaceDescriptor()->GetFace (nDescriptorIdx - 1).getRotation();

		sampledLayer.SelectionClear();
		sampledLayer.SelectIndexedRegions (nDescriptorIdx);
		sampledLayer.SelectionRebuildBox();
				
		IGSmartLayer copyLayer ((CxImage&)(*this));
		IGSmartLayer copySampledLayer ((CxImage&)(sampledLayer));
		copySampledLayer.CopyFaceDetection (sampledLayer);	
		copySampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)-nRot * PI / 180.0f), 1.0f, true);
		copySampledLayer.SelectionRebuildBox();
		
		copyLayer.SelectionDelete();
		copyLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)-nRot * PI / 180.0f), 1.0f, true);

		// indexing sampled layer
		ProgressSetSubRange (2, 3);

		// create face image processing task
		if (!copyLayer.SelectionCopyFromScaledLayer (copySampledLayer))
			return false;		
		copyLayer.SelectionRebuildBox();

		if (!IGFaceEffect::Create (pEffectMessage, copyLayer, nDescriptorIdx, pEffect)) 
			return false;
		ProgressStepIt();

		// process face image processing
		if (pEffect->Process())
		{
			bool bNeedSelect = pEffect->IsSelectNeeded();
			ProgressStepIt();
			
			if (bNeedSelect){
				copyLayer.SelectionDelete();
				copyLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f, true);				
				copySampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f, true);				
				copySampledLayer.SelectionRebuildBox();
				if (!copyLayer.SelectionCopyFromScaledLayer (copySampledLayer))
					return false;
			}
			else{
				copyLayer.SelectionRebuildBox();
				copyLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f, true);
			}
			copyLayer.SelectionRebuildBox();
			ProgressStepIt();

			// apply transformation to selection
			long xmin,xmax,ymin,ymax;
			if (pSelection){
				xmin = info.rSelectionBox.left; xmax = info.rSelectionBox.right;
				ymin = info.rSelectionBox.bottom; ymax = info.rSelectionBox.top;
			} else {
				xmin = ymin = 0;
				xmax = head.biWidth - 1; ymax=head.biHeight - 1;
			}
			for(long y=ymin; y<=ymax; y++){
				BYTE *pBits = GetBits(y) + xmin * 3;
				BYTE *pSrcBits = copyLayer.GetBits(y) + xmin * 3;
				for(long x=xmin; x<=xmax; x++){
					if (copyLayer.BlindSelectionIsInside(x, y))
					{
						*pBits++ = *pSrcBits++;
						*pBits++ = *pSrcBits++;
						*pBits++ = *pSrcBits++;
					}
					else {
						pBits += 3;
						pSrcBits += 3;
					}
				}
			}
		}		
		ProgressStepIt();
		delete pEffect;
		clean();
	}
	return true;
}

void IGSmartLayer::getRegionBoundaries (std::vector <IGSmartPixel*>& vBoundaries)
{
	IGSmartPixel *pFirstPixel = GetPixels();
	IGSmartPixel *pPixel = pFirstPixel;
	for (int idxPixel = 0; idxPixel < head.biWidth * head.biHeight; idxPixel++){
		if (pPixel->m_nLabel != IGMARKER_BACKGROUND){
			const vector <IGSmartPixel*>& vNeighbourPixels = pPixel->GetNeighbours();
			for (vector <IGSmartPixel*>::const_iterator itNeighbourPixel = vNeighbourPixels.cbegin(); itNeighbourPixel != vNeighbourPixels.cend(); ++itNeighbourPixel){
				if ((*itNeighbourPixel)->m_nLabel != pPixel->m_nLabel){
					vBoundaries.push_back(pPixel);
					break;
				}
			}
		}
		pPixel++;
	}
}

bool IGSmartLayer::GrayScale()
{
	if (head.biBitCount != 8)
		m_bReconstructionNeeded = true;
	return CxImage::GrayScale();
}

bool IGSmartLayer::FilterSkin()
{
	if (head.biBitCount != 24 || head.biWidth < 50 || head.biHeight < 50)
		return false;
	DWORD dwHSVEffWidth = info.dwEffWidth;
	CxImage hsv (*this);
	CxImage rgb (*this);
	hsv.ConvertColorSpace (1, 0);
	if (!GrayScale())
		return false;
	if (!Construct8())
		return false;
	if (!ComputeGradient())
		return false;
	BYTE *pCurPixel = GetBits(5) + 5;
	IGSmartPixel *pCurSmartPixel = GetPixel (5, 5);
	BYTE *pCurPixelHSV = hsv.GetBits(5) + 15;
	DWORD dwWidth24 = head.biWidth * 3;
	DWORD dwOffset = info.dwEffWidth - head.biWidth;
	DWORD dwOffset24 = dwHSVEffWidth - dwWidth24;
	int nHigherLimitSaturation = 70;
	float fLowerLimitSkin = 0.75f;
	float fLowerLimitSigma = 0.25f;
	float fUpperLimitSigma = 0.75f;
	BYTE higherLimitGradient = 100;
	m_vSkinPixels.clear();
	for (int nRow = 5; nRow < head.biHeight - 5; nRow++)
	{	
		for (int nCol = 5; nCol < head.biWidth - 5; nCol++)
		{
			float fSkinCount = 0.0f;
			float fSigma = 0.0f;
			float fMean = 0.0f;
			for (int nFilterRow = -5; nFilterRow < 5; nFilterRow++)
			{	
				for (int nFilterCol = -5; nFilterCol < 5; nFilterCol++)
				{
					BYTE *pCurFilterPixel = pCurPixel + nFilterCol * head.biWidth + nFilterRow;
					BYTE *pCurFilterPixelHSV = pCurPixelHSV + nFilterCol * dwHSVEffWidth + nFilterRow * 3;
					if (pCurFilterPixelHSV[2] < nHigherLimitSaturation)
						fSkinCount += 1.0f;
					fSigma += (float)abs (*pCurFilterPixel - *pCurPixel);
					fMean += (float)*pCurFilterPixel;
				}
			}
			fMean /= 100.0f;
			fSkinCount /= 100.0f;
			fSigma /= (100.0f + abs ((float)*pCurPixel - fMean)) * 100.0f;
			if ((fSkinCount > fLowerLimitSkin) && (fSigma > fLowerLimitSigma) && (fSigma < fUpperLimitSigma) && (pCurSmartPixel->m_gradient < higherLimitGradient)){
				*pCurPixel = 255;
				m_vSkinPixels.push_back (pCurSmartPixel);
				std::vector <IGSmartPixel*> vNeighbours = pCurSmartPixel->GetNeighbours();
				for (std::vector <IGSmartPixel*>::iterator it = vNeighbours.begin(); it != vNeighbours.end(); ++it)
					(*it)->m_nbNeighbourSkinPxls++;
			}
			else
				*pCurPixel = 0;
			pCurPixel++;
			pCurSmartPixel++;
			pCurPixelHSV += 3;
		}
		pCurPixel += dwOffset + 10;
		pCurSmartPixel += 10;
		pCurPixelHSV += dwOffset24 + 30;
	}
	for (int nRow = 0; nRow < 5; nRow++)
		::memset (GetBits(nRow), 0, head.biWidth);
	for (int nRow = head.biHeight - 5; nRow < head.biHeight; nRow++)
		::memset (GetBits(nRow), 0, head.biWidth);
	for (int nRow = 0; nRow < head.biHeight; nRow++)
		::memset (GetBits(nRow), 0, 5);
	for (int nRow = 0; nRow < head.biHeight; nRow++)
		::memset (GetBits(nRow) + head.biWidth - 5, 0, 5);
	return true;
}

bool IGSmartLayer::FilterSkinUnnoised (bool bSelect)
{
	if (!FilterSkin())
		return false;
	if (!unnoiseSkin())
		return false;
	if (bSelect){
		SelectionClear();
		RGBQUAD grey255 = {255,255,255,255};
		SelectionAddColor (grey255);
	}
	return true;
}

bool IGSmartLayer::unnoiseSkin()
{
	if (!Erode(5, true))
		return false;
	if (!Dilate(10, true))
		return false;
	return Erode(10, true);
}

void IGSmartLayer::getBoundaries (bool bOver, bool bSkin)
{
	m_vBoundaries.clear();
	if (bSkin)
	{
		if (bOver){
			for (std::vector <IGSmartPixel*>::iterator it = m_vSkinPixels.begin(); it != m_vSkinPixels.end(); ++it){
				if ((***(*it) == 255) && (*it)->m_nbNeighbourSkinPxls != 4){
					std::vector <IGSmartPixel*> vNeighbours = (*it)->GetNeighbours();
					for (std::vector <IGSmartPixel*>::iterator itN = vNeighbours.begin(); itN != vNeighbours.end(); ++itN){
						if (***(*itN) == 0)
							m_vBoundaries.push_back (*itN);
					}
				}
			}
		}
		else{		
			for (std::vector <IGSmartPixel*>::iterator it = m_vSkinPixels.begin(); it != m_vSkinPixels.end(); ++it){
				if ((***(*it) == 255) && (*it)->m_nbNeighbourSkinPxls != 4)
					m_vBoundaries.push_back (*it);
			}		
		}
	}
	else
	{
		IGSmartPixel *pCurSmartPixel = GetPixel (0, 0);
		int nSize = head.biWidth * head.biHeight;
		if (bOver){
			while (nSize--){
				if ((***pCurSmartPixel == 255) && (pCurSmartPixel->m_nbNeighbourSkinPxls != 4)){
					std::vector <IGSmartPixel*> vNeighbours = pCurSmartPixel->GetNeighbours();
					for (std::vector <IGSmartPixel*>::iterator itN = vNeighbours.begin(); itN != vNeighbours.end(); ++itN){
						if (***(*itN) == 0)
							m_vBoundaries.push_back (*itN);
					}
				}
				pCurSmartPixel++;
			}
		}
		else{		
			while (nSize--){
				if ((***pCurSmartPixel == 255) && (pCurSmartPixel->m_nbNeighbourSkinPxls != 4))
					m_vBoundaries.push_back (pCurSmartPixel);
				pCurSmartPixel++;
			}
		}
	}
}

bool IGSmartLayer::Dilate (int nSize, bool bSkin)
{
	getBoundaries(false, bSkin);
	long ray = nSize / 2;
	long kMax = nSize - ray;
	if (head.biBitCount == 24){
		for (std::vector <IGSmartPixel*>::iterator it = m_vBoundaries.begin(); it != m_vBoundaries.end(); ++it){
			int iY = -ray * info.dwEffWidth;
			for(long y = -ray; y < kMax; y++, iY += info.dwEffWidth)
			{
				if (0 > ((*it)->m_nY + y) || ((*it)->m_nY + y) >= head.biHeight)
					continue;
				int iX = iY - ray * 3;
				for(long x = -ray; x < kMax; x++)
				{
					if (0 > ((*it)->m_nX + x) || ((*it)->m_nX + x) >= head.biWidth){
						iX += 3;
						continue;
					}
					if (sqrt ((float)(x*x + y*y)) <= ray){
						***it[iX++] = 255;
						***it[iX++] = 255;
						***it[iX++] = 255;
					}
					else{
						iX += 3;
					}
				}
			}
		}
	}
	else if (head.biBitCount == 8){
		for (std::vector <IGSmartPixel*>::iterator it = m_vBoundaries.begin(); it != m_vBoundaries.end(); ++it){
			int iY = -ray * info.dwEffWidth;
			for(long y = -ray; y < kMax; y++, iY += info.dwEffWidth)
			{
				if (0 > ((*it)->m_nY + y) || ((*it)->m_nY + y) >= head.biHeight)
					continue;
				int iX = iY - ray;
				for(long x = -ray; x < kMax; x++, iX++)
				{
					if (0 > ((*it)->m_nX + x) || ((*it)->m_nX + x) >= head.biWidth)
						continue;
					if (sqrt ((float)(x*x + y*y)) <= ray)
						(***it)[iX] = 255;
				}
			}
		}
	}
	else return false;

	return IndexSkinBinary();
}

bool IGSmartLayer::Erode(int nSize, bool bSkin)
{
	getBoundaries(true, bSkin);
	long ray = nSize / 2;
	long kMax = nSize - ray;
	if (head.biBitCount == 24){
		for (std::vector <IGSmartPixel*>::iterator it = m_vBoundaries.begin(); it != m_vBoundaries.end(); ++it){
			int iY = -ray * info.dwEffWidth;
			for(long y = -ray; y < kMax; y++, iY += info.dwEffWidth)
			{
				if (0 > ((*it)->m_nY + y) || ((*it)->m_nY + y) >= head.biHeight)
					continue;
				int iX = iY - ray * 3;
				for(long x = -ray; x < kMax; x++)
				{
					if (0 > ((*it)->m_nX + x) || ((*it)->m_nX + x) >= head.biWidth){
						iX += 3;
						continue;
					}
					if (sqrt ((float)(x*x + y*y)) <= ray){
						***it[iX++] = 0;
						***it[iX++] = 0;
						***it[iX++] = 0;
					}
					else{
						iX += 3;
					}
				}
			}
		}
	}
	else if (head.biBitCount == 8){
		for (std::vector <IGSmartPixel*>::iterator it = m_vBoundaries.begin(); it != m_vBoundaries.end(); ++it){
			int iY = -ray * info.dwEffWidth;
			for(long y = -ray; y < kMax; y++, iY += info.dwEffWidth)
			{
				if (0 > ((*it)->m_nY + y) || ((*it)->m_nY + y) >= head.biHeight)
					continue;
				int iX = iY - ray;
				for(long x = -ray; x < kMax; x++, iX++)
				{
					if (0 > ((*it)->m_nX + x) || ((*it)->m_nX + x) >= head.biWidth)
						continue;
					if (sqrt ((float)(x*x + y*y)) <= ray)
						(***it)[iX] = 0;
				}
			}
		}
	}
	else return false;	

	return IndexSkinBinary();
}

bool IGSmartLayer::IndexSkinBinary()
{
	m_vSkinPixels.clear();
	IGSmartPixel *pCurSmartPixel = GetPixel (0, 0);
	if (!pCurSmartPixel)
		return false;
	int nImageSize = head.biWidth * head.biHeight;
	while (nImageSize--){
		if (***pCurSmartPixel == 255){
			m_vSkinPixels.push_back (pCurSmartPixel);
			std::vector <IGSmartPixel*> vNeighbours = pCurSmartPixel->GetNeighbours();
			for (std::vector <IGSmartPixel*>::iterator it = vNeighbours.begin(); it != vNeighbours.end(); ++it)
				(*it)->m_nbNeighbourSkinPxls++;
		}
		pCurSmartPixel++;
	}
	return true;
}

bool IGSmartLayer::Construct4(bool bForce)
{
	if (!m_bReconstructionNeeded && !bForce){
		clean();
		return true;
	}
	if (head.biWidth < 3 || head.biHeight < 3)
		return false;
	int nNextPixelCoeff = 1;
	if (head.biBitCount == 24)
		nNextPixelCoeff = 3;
	else if (head.biBitCount != 8)
		return false;
	if (m_pPixels)
		delete [] m_pPixels;
	m_pPixels = new IGSmartPixel [head.biWidth * head.biHeight];
	if (!m_pPixels)
		return false;
	if (!SelectionIsValid()){
		if (!SelectionClear())
			return false;
	}
	// construct middle
	BYTE *pCurPixel = GetBits(1) + nNextPixelCoeff;
	BYTE *pCurSelection = SelectionGetPointer (1, 1);
	IGSmartPixel *pCurSmartPixel = GetPixel (1, 1);
	int nRow, nCol;
	std::vector <IGSmartPixel*> vPixels(4);
	DWORD dwWidth24 = head.biWidth * nNextPixelCoeff;
	DWORD dwOffset24 = info.dwEffWidth - dwWidth24;
	for (nRow = 1; nRow < head.biHeight - 1; nRow++)
	{	
		for (nCol = 1; nCol < head.biWidth - 1; nCol++)
		{
			pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
			pCurSmartPixel->m_nX = nCol;
			pCurSmartPixel->m_nY = nRow;
			vPixels[0] = GetPixel (nCol, nRow - 1);
			vPixels[1] = GetPixel (nCol - 1, nRow); vPixels[2] = GetPixel (nCol + 1, nRow);
			vPixels[3] = GetPixel (nCol, nRow + 1);
			pCurSmartPixel->SetNeighbours (vPixels);
			pCurPixel += nNextPixelCoeff;
			pCurSmartPixel++;
			pCurSelection++;
		}
		pCurPixel += dwOffset24 + 2 * nNextPixelCoeff; 
		pCurSmartPixel += 2;
		pCurSelection += 2;
	}

	// construct top
	nRow = 0;
	pCurPixel = GetBits(nRow) + nNextPixelCoeff;
	pCurSmartPixel = GetPixel (1, nRow);
	pCurSelection = SelectionGetPointer (1, nRow);
	vPixels.resize(3);
	for (nCol = 1; nCol < head.biWidth - 1; nCol++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol - 1, nRow); vPixels[1] = GetPixel (nCol + 1, nRow);
		vPixels[2] = GetPixel (nCol, nRow + 1);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += nNextPixelCoeff;
		pCurSmartPixel++;
		pCurSelection++;
	}

	// construct bottom
	nRow = head.biHeight - 1;
	pCurPixel = GetBits(nRow) + nNextPixelCoeff;
	pCurSmartPixel = GetPixel (1, nRow);
	pCurSelection = SelectionGetPointer (1, nRow);
	for (nCol = 1; nCol < head.biWidth - 1; nCol++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol, nRow - 1);
		vPixels[1] = GetPixel (nCol - 1, nRow); vPixels[2] = GetPixel (nCol + 1, nRow);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += nNextPixelCoeff;
		pCurSmartPixel++;
		pCurSelection++;
	}

	// construct left
	nCol = 0;
	pCurPixel = GetBits(1);
	pCurSmartPixel = GetPixel (nCol, 1);
	pCurSelection = SelectionGetPointer (nCol, 1);
	for (nRow = 1; nRow < head.biHeight - 1; nRow++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol, nRow - 1);
		vPixels[1] = GetPixel (nCol + 1, nRow);
		vPixels[2] = GetPixel (nCol, nRow + 1);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += info.dwEffWidth;
		pCurSmartPixel += head.biWidth;
		pCurSelection += head.biWidth;
	}

	// construct right
	nCol = head.biWidth - 1;
	pCurPixel = GetBits(1) + nCol * nNextPixelCoeff;
	pCurSmartPixel = GetPixel (nCol, 1);
	pCurSelection = SelectionGetPointer (nCol, 1);
	for (nRow = 1; nRow < head.biHeight - 1; nRow++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol, nRow - 1);
		vPixels[1] = GetPixel (nCol - 1, nRow);
		vPixels[2] = GetPixel (nCol, nRow + 1);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += info.dwEffWidth;
		pCurSmartPixel += head.biWidth;
		pCurSelection += head.biWidth;
	}

	// construct corners
	std::vector <IGSmartPixel*> vPixelsTL(2), vPixelsTR(2), vPixelsBL(2), vPixelsBR(2);
	pCurSmartPixel = GetPixel (0, 0);
	pCurSelection = SelectionGetPointer (0, 0);
	pCurSmartPixel->LinkToPixel (GetBits(0), pCurSelection, 0, 0);
	vPixelsTL[0] = GetPixel (1, 0); vPixelsTL[1] = GetPixel (0, 1);
	pCurSmartPixel->SetNeighbours (vPixelsTL);
	pCurSmartPixel = GetPixel (head.biWidth - 1, 0);
	pCurSelection = SelectionGetPointer (head.biWidth - 1, 0);
	pCurSmartPixel->LinkToPixel (GetBits(0) + (head.biWidth - 1) * nNextPixelCoeff, pCurSelection, head.biWidth - 1, 0);
	vPixelsTR[0] = GetPixel (head.biWidth - 1, 0); vPixelsTR[1] = GetPixel (head.biWidth - 1, 1);
	pCurSmartPixel->SetNeighbours (vPixelsTR);
	pCurSmartPixel = GetPixel (0, head.biHeight - 1);
	pCurSelection = SelectionGetPointer (0, head.biHeight - 1);
	pCurSmartPixel->LinkToPixel (GetBits(head.biHeight - 1), pCurSelection, 0, head.biHeight - 1);
	vPixelsBL[0] = GetPixel (0, head.biHeight - 2); vPixelsBL[1] = GetPixel (1, head.biHeight - 1);
	pCurSmartPixel->SetNeighbours (vPixelsBL);
	pCurSmartPixel = GetPixel (head.biWidth - 1, head.biHeight - 1);
	pCurSelection = SelectionGetPointer (head.biWidth - 1, head.biHeight - 1);
	pCurSmartPixel->LinkToPixel (GetBits(head.biHeight - 1) + (head.biWidth - 1) * nNextPixelCoeff, pCurSelection, head.biWidth - 1, head.biHeight - 1);
	vPixelsBR[0] = GetPixel (head.biWidth - 1, head.biHeight - 2); vPixelsBR[1] = GetPixel (head.biWidth - 2, head.biHeight - 1);
	pCurSmartPixel->SetNeighbours (vPixelsBR);

	m_bReconstructionNeeded = false;
	return true;
}

bool IGSmartLayer::Construct8(bool bForce)
{
	if (!m_bReconstructionNeeded && !bForce){
		clean();
		return true;
	}
	if (head.biWidth < 3 || head.biHeight < 3)
		return false;
	int nNextPixelCoeff = 1;
	if (head.biBitCount == 24)
		nNextPixelCoeff = 3;
	else if (head.biBitCount != 8)
		return false;
	if (m_pPixels)
		delete [] m_pPixels;
	m_pPixels = new IGSmartPixel [head.biWidth * head.biHeight];
	if (!m_pPixels)
		return false;
	if (!SelectionIsValid()){
		if (!SelectionCreate())
			return false;
	}

	// construct middle
	BYTE *pCurPixel = GetBits(1) + nNextPixelCoeff;
	IGSmartPixel *pCurSmartPixel = GetPixel (1, 1);
	BYTE *pCurSelection = SelectionGetPointer (1, 1);
	int nRow, nCol;
	std::vector <IGSmartPixel*> vPixels(8);
	DWORD dwWidth24 = head.biWidth * nNextPixelCoeff;
	DWORD dwOffset24 = info.dwEffWidth - dwWidth24;
	for (nRow = 1; nRow < head.biHeight - 1; nRow++)
	{	
		for (nCol = 1; nCol < head.biWidth - 1; nCol++)
		{
			pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
			vPixels[0] = GetPixel (nCol - 1, nRow - 1); vPixels[1] = GetPixel (nCol, nRow - 1); vPixels[2] = GetPixel (nCol + 1, nRow - 1);
			vPixels[3] = GetPixel (nCol - 1, nRow); vPixels[4] = GetPixel (nCol + 1, nRow);
			vPixels[5] = GetPixel (nCol - 1, nRow + 1); vPixels[6] = GetPixel (nCol, nRow + 1); vPixels[7] = GetPixel (nCol + 1, nRow + 1);
			pCurSmartPixel->SetNeighbours (vPixels);
			pCurPixel += nNextPixelCoeff;
			pCurSmartPixel++;
			pCurSelection++;
		}
		pCurPixel += dwOffset24 + 2 * nNextPixelCoeff; 
		pCurSmartPixel += 2;
		pCurSelection += 2;
	}

	// construct top
	nRow = 0;
	pCurPixel = GetBits(nRow) + nNextPixelCoeff;
	pCurSmartPixel = GetPixel (1, nRow);
	pCurSelection = SelectionGetPointer (1, nRow);
	vPixels.resize(5);
	for (nCol = 1; nCol < head.biWidth - 1; nCol++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol - 1, nRow); vPixels[1] = GetPixel (nCol + 1, nRow);
		vPixels[2] = GetPixel (nCol - 1, nRow + 1); vPixels[3] = GetPixel (nCol, nRow + 1); vPixels[4] = GetPixel (nCol + 1, nRow + 1);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += nNextPixelCoeff;
		pCurSmartPixel++;
		pCurSelection++;
	}

	// construct bottom
	nRow = head.biHeight - 1;
	pCurPixel = GetBits(nRow) + nNextPixelCoeff;
	pCurSmartPixel = GetPixel (1, nRow);
	pCurSelection = SelectionGetPointer (1, nRow);
	for (nCol = 1; nCol < head.biWidth - 1; nCol++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol - 1, nRow - 1); vPixels[1] = GetPixel (nCol, nRow - 1); vPixels[2] = GetPixel (nCol + 1, nRow - 1);
		vPixels[3] = GetPixel (nCol - 1, nRow); vPixels[4] = GetPixel (nCol + 1, nRow);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += nNextPixelCoeff;
		pCurSmartPixel++;
		pCurSelection++;
	}

	// construct left
	nCol = 0;
	pCurPixel = GetBits(1);
	pCurSmartPixel = GetPixel (nCol, 1);
	pCurSelection = SelectionGetPointer (nCol, 1);
	for (nRow = 1; nRow < head.biHeight - 1; nRow++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol, nRow - 1); vPixels[1] = GetPixel (nCol + 1, nRow - 1);
		vPixels[2] = GetPixel (nCol + 1, nRow);
		vPixels[3] = GetPixel (nCol, nRow + 1); vPixels[4] = GetPixel (nCol + 1, nRow + 1);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += info.dwEffWidth;
		pCurSmartPixel += head.biWidth;
		pCurSelection += head.biWidth;
	}

	// construct right
	nCol = head.biWidth - 1;
	pCurPixel = GetBits(1) + nCol * nNextPixelCoeff;
	pCurSmartPixel = GetPixel (nCol, 1);
	pCurSelection = SelectionGetPointer (nCol, 1);
	for (nRow = 1; nRow < head.biHeight - 1; nRow++)
	{
		pCurSmartPixel->LinkToPixel (pCurPixel, pCurSelection, nCol, nRow);
		vPixels[0] = GetPixel (nCol - 1, nRow - 1); vPixels[1] = GetPixel (nCol, nRow - 1);
		vPixels[2] = GetPixel (nCol - 1, nRow);
		vPixels[3] = GetPixel (nCol - 1, nRow + 1); vPixels[4] = GetPixel (nCol, nRow + 1);
		pCurSmartPixel->SetNeighbours (vPixels);
		pCurPixel += info.dwEffWidth;
		pCurSmartPixel += head.biWidth;
		pCurSelection += head.biWidth;
	}

	// construct corners
	std::vector <IGSmartPixel*> vPixelsTL(3), vPixelsTR(3), vPixelsBL(3), vPixelsBR(3);
	pCurSmartPixel = GetPixel (0, 0);
	pCurSelection = SelectionGetPointer (0, 0);
	pCurSmartPixel->LinkToPixel (GetBits(0), pCurSelection, 0, 0);
	vPixelsTL[0] = GetPixel (1, 0); vPixelsTL[1] = GetPixel (0, 1); vPixelsTL[2] = GetPixel (1, 1);
	pCurSmartPixel->SetNeighbours (vPixelsTL);
	pCurSmartPixel = GetPixel (head.biWidth - 1, 0);
	pCurSelection = SelectionGetPointer (head.biWidth - 1, 0);
	pCurSmartPixel->LinkToPixel (GetBits(0) + (head.biWidth - 1) * nNextPixelCoeff, pCurSelection, head.biWidth - 1, 0);
	vPixelsTR[0] = GetPixel (head.biWidth - 1, 0); vPixelsTR[1] = GetPixel (head.biWidth - 2, 1); vPixelsTR[2] = GetPixel (head.biWidth - 1, 1);
	pCurSmartPixel->SetNeighbours (vPixelsTR);
	pCurSmartPixel = GetPixel (0, head.biHeight - 1);
	pCurSelection = SelectionGetPointer (0, head.biHeight - 1);
	pCurSmartPixel->LinkToPixel (GetBits(head.biHeight - 1), pCurSelection, 0, head.biHeight - 1);
	vPixelsBL[0] = GetPixel (0, head.biHeight - 2); vPixelsBL[1] = GetPixel (1, head.biHeight - 2); vPixelsBL[2] = GetPixel (1, head.biHeight - 1);
	pCurSmartPixel->SetNeighbours (vPixelsBL);
	pCurSmartPixel = GetPixel (head.biWidth - 1, head.biHeight - 1);
	pCurSelection = SelectionGetPointer (head.biWidth - 1, head.biHeight - 1);
	pCurSmartPixel->LinkToPixel (GetBits(head.biHeight - 1) + (head.biWidth - 1) * nNextPixelCoeff, pCurSelection, head.biWidth - 1, head.biHeight - 1);
	vPixelsBR[0] = GetPixel (head.biWidth - 2, head.biHeight - 2); vPixelsBR[1] = GetPixel (head.biWidth - 1, head.biHeight - 2); vPixelsBR[2] = GetPixel (head.biWidth - 2, head.biHeight - 1);
	pCurSmartPixel->SetNeighbours (vPixelsBR);

	m_bReconstructionNeeded = false;
	return true;
}

bool IGSmartLayer::ConstructSaddle()
{
	if (m_vRegions.size() < 2){
		return true;
	}
	if (head.biWidth < 3 || head.biHeight < 3)
		return false;
	int nNextPixelCoeff = 1;
	if (head.biBitCount == 24)
		nNextPixelCoeff = 3;
	else if (head.biBitCount != 8)
		return false;
	/*
		//Selection du step de segmentation adapté
		Segmentation3D NewSegModel=new Segmentation3D((Objet3D)mainform.CurScene.WFModel.Objets[0],mainform.CurSegOptions,mainform);
		ArrayList CurSegmentationStep=(ArrayList)mainform.SegmentationSteps[0];
		Segmentation3D CurSeg=(Segmentation3D)CurSegmentationStep[0];
		int index_step=0;
		while((CurSeg.nb_labels>NbRegions)&&(index_step<mainform.SegmentationSteps.Count))
		{
			CurSegmentationStep=(ArrayList)mainform.SegmentationSteps[index_step];
			CurSeg=(Segmentation3D)CurSegmentationStep[0];
			index_step++;
		}
		if(index_step>0)
		{
			CurSegmentationStep=(ArrayList)mainform.SegmentationSteps[index_step-1];
			CurSeg=(Segmentation3D)CurSegmentationStep[0];
		}
		CurSeg.ReferencerSelles(true);	
		// Hierarchisation des points selles
		ArrayList Priorities=new ArrayList();
		foreach(Selle CurSelle in CurSeg.SellesList)
		{
			int index=0;
			foreach(Selle CurPriorSelle in Priorities)
			{
				if(CurPriorSelle.Courbure<CurSelle.Courbure)
					index++;
				else
					break;
			}
			Priorities.Insert(index,CurSelle);
		}			
		ArrayList WorkingRegList=new ArrayList();
		foreach(Region reg in CurSeg.RegList)
		{
			ArrayList newreg=new ArrayList();
			foreach(SegVertex vert in reg.RegionVertexList)
				newreg.Add(NewSegModel.VertexList[CurSeg.VertexList.IndexOf(vert)]);
			WorkingRegList.Add(newreg);
		}
		foreach(SegVertex Vert in NewSegModel.VertexList)
			Vert.Label=CurSeg.nb_labels;
		foreach(ArrayList CurReg in WorkingRegList)
			foreach(SegVertex CurVert in CurReg)
		{
			CurVert.Label=WorkingRegList.IndexOf(CurReg);
		}
		int CurNbRegions=CurSeg.nb_labels;

		// Merging process
		ArrayList LabelIndices=new ArrayList();
		int[] RegAppart=new int[CurSeg.nb_labels];
		for(int i=0;i<CurSeg.nb_labels;i++)
		{
			RegAppart[i]=i;
			LabelIndices.Add(i);
		}
		while((CurNbRegions>NbRegions)&&(Priorities.Count>0))
		{
			Selle CurSelle=(Selle)Priorities[0];
			Priorities.Remove(CurSelle);
			int FusLab1=RegAppart[CurSelle.IndexRegion1];
			int FusLab2=RegAppart[CurSelle.IndexRegion2];
			ArrayList Reg1=(ArrayList)WorkingRegList[FusLab1];
			ArrayList Reg2=(ArrayList)WorkingRegList[FusLab2];				
			if((FusLab1!=CurSeg.nb_labels)&&(FusLab2!=CurSeg.nb_labels))
			{
				if((FusLab1!=FusLab2)&&(LabelIndices.Contains(FusLab1))&&(LabelIndices.Contains(FusLab2)))
				{
					Reg1.AddRange(Reg2);						
					foreach(SegVertex CurVert in Reg2)
						CurVert.Label=FusLab1;
					Reg2.Clear();
					RegAppart[FusLab2]=FusLab1;
					LabelIndices.Remove(FusLab2);
					CurNbRegions=NewSegModel.CalculerNbRegions()-1;						
				}
			}
		}

		// Filtrage des fausses LPE 
		ArrayList WaterShed=new ArrayList();
		foreach(SegVertex CurVert in NewSegModel.VertexList)
			if(CurVert.Label==CurSeg.nb_labels)
				WaterShed.Add(CurVert);	
		foreach(SegVertex CurVert in WaterShed)
		{
			SegVertex FirstVoisin=null;
			bool BadLPE=true;
			for(int i=0;i<CurVert.VertexVoisins.Count;i++)
			{
				SegVertex CurVoisin=(SegVertex)NewSegModel.VertexList[(int)CurVert.VertexVoisins[i]];
				if(CurVoisin.Label!=CurVert.Label)
				{
					if(FirstVoisin==null)
						FirstVoisin=CurVoisin;
					else
					{
						if(FirstVoisin.Label!=CurVoisin.Label)
							BadLPE=false;
					}
				}
			}
			if((FirstVoisin!=null)&&(BadLPE))
				CurVert.Label=FirstVoisin.Label;
		}

		//Renumérotation des régions
		LabelIndices.Clear();
		WorkingRegList.Clear();
		foreach(SegVertex Vert in NewSegModel.VertexList)
		{
			if(Vert.Label==CurSeg.nb_labels)
				Vert.Label=CurNbRegions;
			else
			{
				if(!LabelIndices.Contains(Vert.Label))
				{
					LabelIndices.Add(Vert.Label);
					WorkingRegList.Add(new ArrayList());
				}
				Vert.Label=LabelIndices.IndexOf(Vert.Label);
				ArrayList curreg=(ArrayList)WorkingRegList[Vert.Label];				
				curreg.Add(Vert);
			}
		}
		// Creation de la nouvelle segmentation
		NewSegModel.RegList=WorkingRegList;
		NewSegModel.nb_labels=WorkingRegList.Count;
		ArrayList NewSegsList=new ArrayList();
		NewSegsList.Add(NewSegModel);
		mainform.CurSegModel=NewSegsList;
		mainform.CurSegOptions.Cascades_niveau_total=index_step;
		SetNiveauCourant(index_step);
		SetNbRegions(NewSegModel.nb_labels);
		mainform.VueOpenGL.Refresh();		
		*/
	
	vector <IGSmartPixel *> vPixels;
	set <pair <int, int>> doneSaddles;
	int nIdxPixl = 0;
	for (vector <IGRegion*>::iterator itReg = m_vRegions.begin(); itReg != m_vRegions.end(); ++itReg){
		for (vector <IGSmartPixel*>::iterator itNeighbourSaddle = (*itReg)->m_vNeighbourSaddles.begin(); itNeighbourSaddle != (*itReg)->m_vNeighbourSaddles.end(); ++itNeighbourSaddle){
			if (doneSaddles.find (pair <int, int> (min ((*itReg)->m_nLabel, (*itNeighbourSaddle)->m_nLabel), max ((*itReg)->m_nLabel, (*itNeighbourSaddle)->m_nLabel))) == doneSaddles.end()){
				IGSmartPixel *pPixel = new IGSmartPixel();
				if ((*itNeighbourSaddle)->GetRegion())
					pPixel->m_gradient = max ((*itNeighbourSaddle)->m_gradient - (*itReg)->m_minGradient, (*itNeighbourSaddle)->m_gradient - (*itNeighbourSaddle)->GetRegion()->m_minGradient);
				else
					pPixel->m_gradient = (*itNeighbourSaddle)->m_gradient - (*itReg)->m_minGradient;
				pPixel->LinkToChildRegions (*itReg, (*itNeighbourSaddle)->GetRegion());
				vPixels.push_back (pPixel);
				(*itReg)->m_vNeighbourSaddlesReconstruct.push_back (pPixel);
				if ((*itNeighbourSaddle)->GetRegion())
					(*itNeighbourSaddle)->GetRegion()->m_vNeighbourSaddlesReconstruct.push_back (pPixel);
				doneSaddles.insert (pair <int, int> (min ((*itReg)->m_nLabel, (*itNeighbourSaddle)->m_nLabel), max ((*itReg)->m_nLabel, (*itNeighbourSaddle)->m_nLabel)));
			}
		}
		nIdxPixl++;
	}
	for (vector <IGRegion*>::iterator itReg = m_vRegions.begin(); itReg != m_vRegions.end(); ++itReg){
		for (vector <IGSmartPixel*>::iterator itSaddle = (*itReg)->m_vNeighbourSaddlesReconstruct.begin(); itSaddle != (*itReg)->m_vNeighbourSaddlesReconstruct.end(); ++itSaddle){
			for (vector <IGSmartPixel*>::iterator itNeighbourSaddle = (*itReg)->m_vNeighbourSaddlesReconstruct.begin(); itNeighbourSaddle != (*itReg)->m_vNeighbourSaddlesReconstruct.end(); ++itNeighbourSaddle){
				if (*itNeighbourSaddle != *itSaddle)
					(*itSaddle)->AddNeighbour (*itNeighbourSaddle);
			}
		}
	}
	if (vPixels.size() > 0)
		m_vvTreePixels.push_back(vPixels);
	return true;
}

void IGSmartLayer::clean()
{
	while (!m_vRegions.empty())
	{
		delete m_vRegions.back();
		m_vRegions.pop_back();
	}
	m_vBoundaries.clear();
	SelectionDelete();
	if (!m_pPixels)
		return;
	int nSize = head.biWidth * head.biHeight;
	IGSmartPixel *pCurSmartPixel = GetPixel (0, 0);
	while (nSize--)
	{
		pCurSmartPixel->Clean();
		pCurSmartPixel++;
	}
}

bool IGSmartLayer::ComputeGradient (bool bApplyResultToPixels)
{
	CxImage g1 (*this);
	/*
	if (!g1.GrayScale())
		return false;
	if (!g1.GaussianBlur (2.0f))
		return false;*/
	if (!g1.GradientMorpho(4))
		return false;
	if (bApplyResultToPixels){
		BYTE *pSrcPixel = NULL;
		BYTE *pCurPixel = NULL;
		if (GetBpp() == 8){
			for (int nRow = 0; nRow < head.biHeight; nRow++)
			{	
				pSrcPixel = g1.GetBits(nRow);
				pCurPixel = GetBits(nRow);
				for (int nCol = 0; nCol < head.biWidth; nCol++)
				{
					*pCurPixel++ = 255 - *pSrcPixel++;
					pSrcPixel+=2;
				}
			}
		}
		else if (GetBpp() == 24){
			for (int nRow = 0; nRow < head.biHeight; nRow++)
			{	
				pSrcPixel = g1.GetBits(nRow);
				pCurPixel = GetBits(nRow);
				for (int nCol = 0; nCol < head.biWidth; nCol++)
				{
					*pCurPixel++ = 255 - *pSrcPixel++;
					*pCurPixel++ = 255 - *pSrcPixel++;
					*pCurPixel++ = 255 - *pSrcPixel++;
				}
			}
		}
		else
			return false;
		return true;
	}
	if ((head.biBitCount != 24) && (head.biBitCount != 8))
		return false;
	BYTE *pCurPixel = NULL;
	IGSmartPixel *pCurSmartPixel = GetPixel (0, 0);
	for (int nRow = 0; nRow < head.biHeight; nRow++)
	{	
		pCurPixel = g1.GetBits(nRow);
		for (int nCol = 0; nCol < head.biWidth; nCol++)
		{
			pCurSmartPixel->m_gradient = *pCurPixel;
			pCurPixel += 3;
			pCurSmartPixel++;
		}
	}
	return true;
}

void IGSmartLayer::GetMinGradientPos (int nPosX, int nPosY, int nRadius, int& nResPosX, int& nResPosY)
{
	IGSmartPixel *pCurSmartPixel = GetPixel (nPosX, nPosY);
	nResPosX = nPosX;
	nResPosY = nPosY;
	int nMaxGradient = pCurSmartPixel->m_gradient;
	pCurSmartPixel = GetPixel (max (0, nPosX - nRadius), max (0, nPosY - nRadius));
	for (int nRow = max (0, nPosY - nRadius); nRow < min (head.biHeight, nPosY + nRadius); nRow++)
	{	
		for (int nCol = max (0, nPosX - nRadius); nCol < min (head.biWidth, nPosX + nRadius); nCol++)
		{
			if (pCurSmartPixel->m_gradient < nMaxGradient){
				nResPosX = nCol;
				nResPosY = nRow;
			}
			pCurSmartPixel++;
		}
	}
}

void IGSmartLayer::FilterMorphoBackgroundRegions()
{
	vector <IGRegion*> vRemoveRegions;
	vector <IGRegion*>::const_iterator itRegion = m_vRegions.cbegin();
	IGRegion *pBackground = *itRegion;
	++itRegion;
	float fRatioHigherSides = 0.15f;
	for (; itRegion != m_vRegions.cend(); ++itRegion){
		float fNbSides = 0.0f;
		vector <IGSmartPixel*> vPixels = (*itRegion)->m_vPixels;
		for (vector <IGSmartPixel*>::const_iterator itPixel = vPixels.cbegin(); itPixel != vPixels.cend(); ++itPixel){
			int nX = (*itPixel)->m_nX;
			int nY = (*itPixel)->m_nY;
			if (nX < 5 || nX > head.biWidth - 6 || nY < 5 || nY > head.biHeight - 6){
				fNbSides += 1.0f;
			}
		}
		if (fNbSides / ((float)(*itRegion)->m_vPixels.size()) > fRatioHigherSides){
			vRemoveRegions.push_back(*itRegion);
			pBackground->Merge (**itRegion);
		}
	}
	for (vector <IGRegion*>::const_iterator itRegion = vRemoveRegions.cbegin(); itRegion != vRemoveRegions.cend(); ++itRegion)
		m_vRegions.erase (std::find (m_vRegions.begin(), m_vRegions.end(), *itRegion));
}

bool IGSmartLayer::FilterSmallRegions(IGRegion *pBackground)
{
	return FilterSmallRegions (pBackground, m_vRegions);
}

bool IGSmartLayer::FilterSmallRegions (IGRegion *pBackground, std::vector <IGRegion*> vSubdivisions)
{
	if ((int)vSubdivisions.size() <= 1)
		return true;
	multimap<int, IGRegion*> mapRegions;
	for (vector <IGRegion*>::const_iterator itRegion = vSubdivisions.cbegin(); itRegion != vSubdivisions.cend(); ++itRegion){
		mapRegions.insert(pair <int, IGRegion*> ((*itRegion)->m_vPixels.size(), *itRegion));
	}
	multimap<int, IGRegion*>::const_reverse_iterator itRevRegion = mapRegions.crbegin();
	while (++itRevRegion != mapRegions.crend()) {
		if (!MergeToSaddleRegion (pBackground, vSubdivisions, *(*itRevRegion).second))
			return false;
	}
	return true;
}

bool IGSmartLayer::FilterSmallRegions(std::vector <IGRegion*> vRegions)
{
	if (vRegions.size() <= 1)
		return true;
	vector <IGRegion*> vGarbage;
	vector <IGRegion*>::const_iterator itRegion = vRegions.cbegin();
	IGRegion *pBackground = *itRegion;
	++itRegion;
	for (; itRegion != vRegions.cend(); ++itRegion){
		vector <IGRegion*>::const_iterator itRegion2 = vRegions.cbegin();
		++itRegion2;
		for (; itRegion2 != vRegions.cend(); ++itRegion2){
			if ((*itRegion)->m_vPixels.size() < (*itRegion2)->m_vPixels.size() / 5){
				pBackground->Merge (**itRegion);
				vGarbage.push_back (*itRegion);
				break;
			}
		}
	}
	for (vector <IGRegion*>::const_iterator itRegion = vGarbage.cbegin(); itRegion != vGarbage.cend(); ++itRegion)
		m_vRegions.erase (std::find (m_vRegions.cbegin(), m_vRegions.cend(), *itRegion));
	return true;
}

bool IGSmartLayer::FilterFaceWithEyes (const list <pair <BYTE, RECT>>& lpairParamValRects, bool bFinalStep)
{
	list <pair <BYTE, RECT>> lpairValRects;
	for (list <pair <BYTE, RECT>>::const_iterator citHighestEyeRects = lpairParamValRects.cbegin(); citHighestEyeRects != lpairParamValRects.cend(); ++citHighestEyeRects)
		lpairValRects.push_back (*citHighestEyeRects);
	bool bMerging = true;
	while (bMerging)
	{
		bMerging = false;
		vector <IGRegion*> vRemoveRegions;
		vector <IGRegion*>::const_iterator itRegion = m_vRegions.cbegin();
		IGRegion *pBackground = (*itRegion);
		++itRegion;
		for (; itRegion != m_vRegions.cend(); ++itRegion){
			vector <IGSmartPixel*>& vPixels = (*itRegion)->m_vPixels;
			float fRegionSize = (float)vPixels.size();
			float fRegionX = 0.0f;
			float fRegionY = 0.0f;
			float fMinX = (float)head.biWidth, fMaxX = -1.0f, fMinY = (float)head.biHeight, fMaxY = -1.0f;
			for (vector <IGSmartPixel*>::const_iterator itPixel = vPixels.cbegin(); itPixel != vPixels.cend(); ++itPixel){
				float fX = (float)(*itPixel)->m_nX;
				float fY = (float)(*itPixel)->m_nY;
				fRegionX += fX;
				fRegionY += fY;
				if (fX < fMinX)
					fMinX = fX;
				if (fX > fMaxX)
					fMaxX = fX;
				if (fY < fMinY)
					fMinY = fY;
				if (fY > fMaxY)
					fMaxY = fY;
			}
			fRegionX /= fRegionSize;
			fRegionY /= fRegionSize;
			bool bFaceEyeDetected = false;
			RECT rectEyeDetected;
			for (list <pair <BYTE, RECT>>::const_iterator itRect = lpairValRects.cbegin(); itRect != lpairValRects.cend(); ++itRect){
				float fCenterOffsetX = ((float)((*itRect).second.left + (*itRect).second.right) / 2.0f - fRegionX) / (fMaxX - fMinX);
				float fCenterOffsetY = ((float)((*itRect).second.top + (*itRect).second.bottom) / 2.0f - fRegionY) / (fMaxY - fMinY);
				float fRatio = (float)(((*itRect).second.right - (*itRect).second.left) * ((*itRect).second.bottom - (*itRect).second.top)) / fRegionSize;
				float fRatioArea = (float)(((*itRect).second.right - (*itRect).second.left) * ((*itRect).second.bottom - (*itRect).second.top)) / ((fMaxX - fMinX) * (fMaxY - fMinY));
				float fRatioTop = ((float)(*itRect).second.bottom - (float)fMinY) / ((float)fMaxY - (float)fMinY);
				float fRatioBottom = ((*itRect).second.top - (float)fMinY) / ((float)fMaxY - (float)fMinY);
				float fRatioAspect = ((float)((*itRect).second.bottom - (*itRect).second.top) / (float)(fMaxY - fMinY)) / ((float)((*itRect).second.right - (*itRect).second.left) / (float)(fMaxX - fMinX));
				if ((abs (fCenterOffsetX) < 0.2f) && (fCenterOffsetY > 0.0f) && (fCenterOffsetY < 0.5f) && (fRatio > 0.03f) && (fRatioArea < 0.75f) && (fRatioAspect < 0.6f) 
							&& (fRatioTop < 0.9f) && (fRatioBottom > 0.1f)){
					bFaceEyeDetected = true;
					rectEyeDetected = (*itRect).second;
					break;
				}
			}
			if (bFaceEyeDetected){
				rectEyeDetected.left = max ((int)fMinX, rectEyeDetected.left);
				rectEyeDetected.right = min ((int)fMaxX, rectEyeDetected.right);
				rectEyeDetected.top = max ((int)fMinY, rectEyeDetected.top);
				rectEyeDetected.bottom = min ((int)fMaxY, rectEyeDetected.bottom);
				for (int idxRow = rectEyeDetected.top; idxRow < rectEyeDetected.bottom; idxRow++){
					for (int idxCol = rectEyeDetected.left; idxCol < rectEyeDetected.right; idxCol++){
						IGSmartPixel *pPixel = GetPixel(idxCol, idxRow);
						if (pPixel->m_nLabel == pBackground->m_nLabel){
							pPixel->m_nLabel = (*itRegion)->m_nLabel;
							(*itRegion)->AddPixel(pPixel);
						}
					}
				}
				(*itRegion)->SetEyes (rectEyeDetected);
			}
			else{
				if (MergeToSaddleRegion (pBackground, **itRegion)){
					vRemoveRegions.push_back (*itRegion);
					bMerging = true;
				}
			}
			if (!bFinalStep && bMerging)
				break;
		}
		for (vector <IGRegion*>::const_iterator itRegion = vRemoveRegions.cbegin(); itRegion != vRemoveRegions.cend(); ++itRegion){
			m_vRegions.erase (std::find (m_vRegions.begin(), m_vRegions.end(), *itRegion));
		}
	}
	if (!bFinalStep)
		return FilterFaceWithEyes (lpairParamValRects, true);
	return true;
}

void IGSmartLayer::SelectIndexedRegions (int nIdxReg, BYTE level)
{
	if ((int)m_vRegions.size() <= nIdxReg)
		return;
	if (nIdxReg == -1){
		vector <IGRegion*>::const_iterator itRegion = m_vRegions.cbegin();
		++itRegion;
		for (; itRegion != m_vRegions.cend(); ++itRegion)
			(*itRegion)->Select (level);
	}
	else
		m_vRegions[nIdxReg]->Select (level);
}

IGRegion* IGSmartLayer::GetRegion(int nLabel)
{
	for (vector <IGRegion*>::const_iterator itRegion = m_vRegions.cbegin(); itRegion != m_vRegions.cend(); ++itRegion){
		if ((*itRegion)->m_nLabel == nLabel)
			return *itRegion;
	}
	return NULL;
}

bool IGSmartLayer::MergeToSaddleRegion (IGRegion *pBackground, IGRegion& region, bool bExcludeBackground)
{
	return MergeToSaddleRegion (pBackground, m_vRegions, region, bExcludeBackground);
}

bool IGSmartLayer::MergeToSaddleRegion (IGRegion *pBackground, const vector <IGRegion*>& vRegions, IGRegion& region, bool bExcludeBackground)
{
	if (region.m_vPixels.size() == 0)
		return true;
	region.GetBoundaries();
	int nSaddleLabel = region.GetSaddleRegionLabel(vRegions);
	if (bExcludeBackground && (nSaddleLabel == 0))
		return false;
	IGRegion *pSaddleRegion = NULL;
	if (nSaddleLabel == 0){
		pSaddleRegion = pBackground;
	}
	else{
		for (vector <IGRegion*>::const_iterator itRegion = vRegions.cbegin(); itRegion != vRegions.cend(); ++itRegion){
			if ((*itRegion)->m_nLabel == nSaddleLabel && (!bExcludeBackground || *itRegion != vRegions.front())){
				pSaddleRegion = *itRegion;
				break;
			}
		}
	}
	if (!pSaddleRegion)
		throw IGEXCEPTION (SmartLayerException, "MergeToSaddleRegion", "No saddle region found");
	pSaddleRegion->Merge (region);
	return true;
}

int IGSmartLayer::DetectFaces()
{
	if (!m_pFaceDescriptor) {
		m_pFaceDescriptor = new IGFaceDescriptor();
		// sampling
		IGSmartLayer sampledLayer (*this);
		if (sampledLayer.Resample())
			m_pFaceDescriptor->Detect(GetWidth(), GetHeight(), sampledLayer);		
	}
	return 0;
}

IGFaceDescriptor* IGSmartLayer::GetFaceDescriptor()
{
	return m_pFaceDescriptor;
}

void IGSmartLayer::SetFaceDescriptor (const std::wstring& wsDescriptor)
{
	if (m_pFaceDescriptor)
		delete m_pFaceDescriptor;
	m_pFaceDescriptor = new IGFaceDescriptor (*this, wsDescriptor);
}

double round(double r) {
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

bool IGSmartLayer::AutoRotate()
{
	// get the best orientation for face recognition
	int nRot = DetectFaces();
	nRot = 90 * (int)round((float)nRot / 90.0f);
	if (nRot == 0)
		return true;
	// rotate the face detection regions
	m_pFaceDescriptor->Rotate (nRot);
	// rotate the picture
	float fXTr = cosf((float)nRot * PI / 180.0f);
	float fYTr = sinf((float)nRot * PI / 180.0f);
	int nLayWidth = abs((int)(head.biWidth * fXTr + head.biHeight * fYTr));
	int nLayHeight = abs((int)(-head.biWidth * fYTr + head.biHeight * fXTr));
	int nLayWidthCpy = max (nLayWidth, head.biWidth);
	int nLayHeightCpy = max (nLayHeight, head.biHeight);
	CxImage layerCpy (nLayWidthCpy, nLayHeightCpy, 24);
	if (!layerCpy.RotateAndResize (fXTr, fYTr, 1.0f, true, this))
		return false;
	layerCpy.AlphaCreate();
	layerCpy.SelectionCreate();
	RECT rcCpy; rcCpy.left = 0; rcCpy.top = 0; rcCpy.right = nLayWidthCpy - 1; rcCpy.bottom = nLayHeightCpy - 1;
	layerCpy.SelectionAddRect (rcCpy);
	layerCpy.SelectionRebuildBox();
	if (nRot < 0)
		layerCpy.MovePixels ((head.biWidth > head.biHeight && nLayWidth < head.biWidth) ? nLayWidth - head.biWidth : 0, (head.biHeight > head.biWidth && nLayHeight < head.biHeight) ? head.biHeight - nLayHeight : 0);
	RECT rc; rc.left = 0; rc.top = 0; rc.right = nLayWidth - 1; rc.bottom = nLayHeight - 1;
	layerCpy.Crop (rc);
	// Transfer everything from the rotated layer except progress bar and parent frame
	HWND hProgBar = GetProgressBar();
	CxImage *pParent = GetParent();
	Transfer (layerCpy);
	// redo the detection on rotated picture
	//destroyFaceDescriptor();
	//DetectFaces();
	SetProgressBar (hProgBar);
	SetParent (pParent);
	return true;
}

void IGSmartLayer::ProgressStepIt()
{
	IGSplashWindow::SendStepIt (info.hProgress);
}

void IGSmartLayer::ProgressSetRange (UINT nMax, bool bLock)
{
	IGSplashWindow::SendSetRange (info.hProgress, nMax, bLock);
}

void IGSmartLayer::ProgressSetSubRange (int nCurStartRange, int nNbSubRanges)
{
	IGSplashWindow::SendSetSubRange (info.hProgress, nCurStartRange, nNbSubRanges);
}

void IGSmartLayer::ProgressSetMessage (wchar_t *pwMessage)
{
	IGSplashWindow::SendSetMessage (info.hProgress, pwMessage);
}


bool IGSmartLayer::SelectionEyesMouth (BYTE level)
{
	if (pSelection==NULL)
		return true;
	//if (src.GetWidth() != head.biWidth || src.GetHeight() != head.biHeight)
	//	return false;
	int nSize = head.biWidth * head.biHeight;
	//BYTE *pCopySelection = new BYTE [nSize];
	//::memcpy (pCopySelection, pSelection, nSize);
	BYTE *pCurDstSel = pSelection + min (info.rSelectionBox.top, info.rSelectionBox.bottom) * head.biWidth + min (info.rSelectionBox.left, info.rSelectionBox.right);
	//BYTE *pCurAndSel = src.SelectionGetPointer() + min (info.rSelectionBox.top, info.rSelectionBox.bottom) * head.biWidth + min (info.rSelectionBox.left, info.rSelectionBox.right);
	int nRectHeight = abs (info.rSelectionBox.top - info.rSelectionBox.bottom) + 1;
	int nRectWidth = abs (info.rSelectionBox.left - info.rSelectionBox.right) + 1;
	for (vector <IGRegion*>::const_iterator itRegion = m_vRegions.cbegin(); itRegion != m_vRegions.cend(); ++itRegion){
		if ((*itRegion)->IsFace()){
			(*itRegion)->Select (level);
			RECT rcEyeLeft, rcEyeRight, rcMouth;
			(*itRegion)->GetEyes (rcEyeLeft, rcEyeRight);
			(*itRegion)->GetMouth (rcMouth);
			SelectionAddRect (rcEyeLeft, level == 0 ? 255 : 0);
			SelectionAddRect (rcEyeRight, level == 0 ? 255 : 0);
			SelectionAddRect (rcMouth, level == 0 ? 255 : 0);
		}
	}
	/*
	for (long y = 0; y < nRectHeight; y++){
		for (long x = 0; x < nRectWidth; x++){
			if (*pCurAndSel == level)
				*pCurDstSel = level;
			pCurDstSel++;
			pCurAndSel++;
		}
		pCurDstSel += head.biWidth - nRectWidth;
		pCurAndSel += head.biWidth - nRectWidth;
	}
	delete [] pCopySelection;*/
	return true;
}