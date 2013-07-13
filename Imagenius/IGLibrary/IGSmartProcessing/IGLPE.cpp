#include "stdafx.h"
#include "IGLPE.h"
#include "IGSmartLayer.h"
#include "FaceDetector.h"
#include <set>
#include <map>
#include <stack>

using namespace std;
using namespace IGLibrary;

IGRegion::IGRegion (int nLabel, bool bIsFace) : m_nLabel (nLabel)
								, m_bIsFace (bIsFace)
								, m_minGradient (255)
								, m_bDone (false)
{
	RECT rcReg = {-1,-1,-1,-1};
	m_rcReg = rcReg;
}

IGRegion::IGRegion (const IGRegion& region)
{
	m_nLabel = region.m_nLabel;
	m_rcReg = region.m_rcReg;
	m_rcEyes = region.m_rcEyes;
	m_rcEyeLeft = region.m_rcEyeLeft;
	m_rcEyeRight = region.m_rcEyeRight;
	m_rcInnerEyes = region.m_rcInnerEyes;
	m_rcMouth = region.m_rcMouth;
	m_rcNoze = region.m_rcNoze;
	m_bIsFace = region.m_bIsFace;
	m_vPixels = region.m_vPixels;
	m_vBoundaries = region.m_vBoundaries;
	m_bDone = region.m_bDone;
}

IGRegion::~IGRegion()
{
}

void IGRegion::SetLabel ()
{
	SetLabel(m_nLabel);
}

void IGRegion::SetLabel (int nLabel, bool bMarkDone)
{
	if (bMarkDone && m_bDone)
		return;
	m_nLabel = nLabel;
	m_bDone = bMarkDone;
	if (m_vPixels.size() == 0)
		return;
	if (m_vPixels.front()->GetChildRegion()){
		for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel){
			(*itPixel)->GetChildRegion()->SetLabel (nLabel, bMarkDone);
			(*itPixel)->GetChildRegion2()->SetLabel (nLabel, bMarkDone);
		}
	}
	else{
		for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel)
			(*itPixel)->m_nLabel = nLabel;
	}
}

void IGRegion::Rotate (int nPtCtrX, int nPtCtrY, int nRotate)
{
	if (IsValidRect (m_rcReg))
		m_rcReg = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcReg, nRotate);
	if (IsValidRect (m_rcEyeLeft))
		m_rcEyeLeft = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcEyeLeft, nRotate);
	if (IsValidRect (m_rcEyeRight))
		m_rcEyeRight = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcEyeRight, nRotate);
	if (IsValidRect (m_rcEyes))
		m_rcEyes = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcEyes, nRotate);
	if (IsValidRect (m_rcInnerEyes))
		m_rcInnerEyes = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcInnerEyes, nRotate);
	if (IsValidRect (m_rcMouth))
		m_rcMouth = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcMouth, nRotate);
	if (IsValidRect (m_rcNoze))
		m_rcNoze = Face::RotateRect (nPtCtrX, nPtCtrY, m_rcNoze, nRotate);	
}

void IGRegion::Select (BYTE level)
{
	for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel)
		*(*itPixel)->m_pSelection = level;
}

void IGRegion::Merge (IGRegion& region)
{
	region.SetLabel (m_nLabel);
	for (vector <IGSmartPixel*>::const_iterator itPixel = region.m_vPixels.begin(); itPixel != region.m_vPixels.end(); ++itPixel)
		m_vPixels.push_back (*itPixel);
}

void IGRegion::ApplyMedianColor()
{
	float fWeightedRed = 0.0f;
	float fWeightedGreen = 0.0f;
	float fWeightedBlue = 0.0f;
	float fSumNegGrad = 0.0f;
	for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel){
		BYTE *pRGB = ***itPixel;
		float fNegGrad = 255.0f - (float)((*itPixel)->m_gradient);
		fWeightedRed += fNegGrad * *pRGB++;
		fWeightedGreen += fNegGrad * *pRGB++;
		fWeightedBlue += fNegGrad * *pRGB;
		fSumNegGrad += fNegGrad;
	}
	fWeightedRed /= fSumNegGrad;
	fWeightedGreen /= fSumNegGrad;
	fWeightedBlue /= fSumNegGrad;
	for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel){
		BYTE *pRGB = ***itPixel;
		*pRGB++ = (BYTE)fWeightedRed;
		*pRGB++ = (BYTE)fWeightedGreen;
		*pRGB = (BYTE)fWeightedBlue;
	}
}

const vector <IGSmartPixel*>& IGRegion::GetBoundaries()
{
	if (m_vBoundaries.size() != 0)
		m_vBoundaries.clear();
	for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel){
		const std::vector<IGSmartPixel*>& vNeighbours = (*itPixel)->GetNeighbours();
		for (vector <IGSmartPixel*>::const_iterator itNeighbour = vNeighbours.cbegin(); itNeighbour != vNeighbours.cend(); ++itNeighbour){
			if ((*itNeighbour)->m_nLabel != (*itPixel)->m_nLabel){
				m_vBoundaries.push_back(*itPixel);
				break;
			}
		}
	}
	return m_vBoundaries;
}

void IGRegion::CalcRect()
{
	RECT rcReg = {1000000,1000000,-1,-1};
	for (vector <IGSmartPixel*>::iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel){
		if ((*itPixel)->m_nX < rcReg.left)
			rcReg.left = (*itPixel)->m_nX;
		if ((*itPixel)->m_nY < rcReg.top)
			rcReg.top = (*itPixel)->m_nY;
		if ((*itPixel)->m_nX > rcReg.right)
			rcReg.right = (*itPixel)->m_nX;
		if ((*itPixel)->m_nY > rcReg.bottom)
			rcReg.bottom = (*itPixel)->m_nY;
	}
	m_rcReg = rcReg;
}

RECT IGRegion::GetRect()
{
	return m_rcReg;
}

int IGRegion::GetSaddleRegionLabel(const vector <IGRegion*>& vRegions) const
{
	return GetSaddleRegionLabel (vRegions, m_vBoundaries, m_nLabel);
}

int IGRegion::GetSaddleRegionLabel(const vector <IGRegion*>& vRegions, const vector <IGSmartPixel*>& vBoundaries, int nRegionLabel)
{
	int nSaddleLabel = 0;
	int nSaddleGradient = 256;
	for (vector <IGSmartPixel*>::const_iterator itPixel = vBoundaries.cbegin(); itPixel != vBoundaries.cend(); ++itPixel){
		if ((*itPixel)->m_gradient < nSaddleGradient){
			const vector <IGSmartPixel*>& vNeighbours = (*itPixel)->GetNeighbours();
			for (vector <IGSmartPixel*>::const_iterator itNeighbourPixel = vNeighbours.cbegin(); itNeighbourPixel != vNeighbours.cend(); ++itNeighbourPixel){
				if ((*itNeighbourPixel)->m_nLabel != nRegionLabel){
					if ((*itNeighbourPixel)->m_gradient < nSaddleGradient){
						for (vector <IGRegion*>::const_iterator itRegion = vRegions.cbegin(); itRegion != vRegions.cend(); ++itRegion){
							if ((*itRegion)->m_nLabel == (*itNeighbourPixel)->m_nLabel){
								nSaddleGradient = max ((*itNeighbourPixel)->m_gradient, (*itPixel)->m_gradient);
								nSaddleLabel = (*itNeighbourPixel)->m_nLabel;
								break;
							}
						}
					}
				}
			}
		}
	}
	return nSaddleLabel;
}

void IGRegion::SetEyes (const RECT& rectEye)
{
	m_rcEyes = rectEye;
	m_bIsFace = true;
	for (vector <IGSmartPixel*>::const_iterator itPixel = m_vPixels.cbegin(); itPixel != m_vPixels.cend(); ++itPixel){
		(*itPixel)->m_bEyes = (((*itPixel)->m_nX >= rectEye.left) && ((*itPixel)->m_nX <= rectEye.right) &&
			((*itPixel)->m_nY >= rectEye.top) && ((*itPixel)->m_nY <= rectEye.bottom));
	}
}

void IGRegion::SetEyes()
{
	SetEyes (m_rcEyeLeft, m_rcEyeRight);
}

void IGRegion::SetEyes (const RECT& rectEye1, const RECT& rectEye2)
{
	m_rcEyes.left = min (rectEye1.left, rectEye2.left);
	m_rcEyes.right = max (rectEye1.right, rectEye2.right);
	m_rcEyes.top = min (rectEye1.top, rectEye2.top);
	m_rcEyes.bottom = max (rectEye1.bottom, rectEye2.bottom);
	m_rcEyeLeft = rectEye1.left < rectEye2.left ? rectEye1 : rectEye2;
	m_rcEyeRight = rectEye1.left > rectEye2.left ? rectEye1 : rectEye2;
	m_bIsFace = true;
	for (vector <IGSmartPixel*>::const_iterator itPixel = m_vPixels.cbegin(); itPixel != m_vPixels.cend(); ++itPixel){
		(*itPixel)->m_bEyes = ((((*itPixel)->m_nX >= m_rcEyeLeft.left) && ((*itPixel)->m_nX <= m_rcEyeLeft.right) &&
			((*itPixel)->m_nY >= m_rcEyeLeft.top) && ((*itPixel)->m_nY <= m_rcEyeLeft.bottom)) ||
			(((*itPixel)->m_nX >= m_rcEyeRight.left) && ((*itPixel)->m_nX <= m_rcEyeRight.right) &&
			((*itPixel)->m_nY >= m_rcEyeRight.top) && ((*itPixel)->m_nY <= m_rcEyeRight.bottom)));
	}
}

void IGRegion::SetMouth (const RECT& rcMouth)
{
	m_rcMouth = rcMouth;
}

void IGRegion::SetNoze (const RECT& rcNoze)
{
	m_rcNoze = rcNoze;
}

void IGRegion::SetGenericEyes()
{
	int nAvgX = 0;
	int nAvgY = 0;
	int nHighX = 0;
	int nHighY = 0;
	int nLowX = 100000000;
	int nLowY = 100000000;
	for (vector <IGSmartPixel*>::const_iterator itPixel = m_vPixels.cbegin(); itPixel != m_vPixels.cend(); ++itPixel){
		nAvgX += (*itPixel)->m_nX;
		nAvgY += (*itPixel)->m_nY;
		if ((*itPixel)->m_nX > nHighX)
			nHighX = (*itPixel)->m_nX;
		if ((*itPixel)->m_nY > nHighY)
			nHighY = (*itPixel)->m_nY;
		if ((*itPixel)->m_nX < nLowX)
			nLowX = (*itPixel)->m_nX;
		if ((*itPixel)->m_nY < nLowY)
			nLowY = (*itPixel)->m_nY;
	}
	nAvgX /= m_vPixels.size();
	nAvgY /= m_vPixels.size();
	int nStdDevX = (nHighX - nLowX) / 3;
	int nStdDevY = (nHighY - nLowY) / 4;
	int nSizeEyeX = nStdDevX;
	int nSizeEyeY = (nSizeEyeX * 3) / 4;
	RECT rcLeftEye, rcRightEye, rcIntersectEyeFace;
	rcLeftEye.left = nAvgX - (nStdDevX * 3) / 4 - nSizeEyeX / 2;
	rcLeftEye.right = rcLeftEye.left + nSizeEyeX;
	rcLeftEye.top = nAvgY + nStdDevY / 2 - nSizeEyeY / 2;
	rcLeftEye.bottom = rcLeftEye.top + nSizeEyeY;
	rcIntersectEyeFace.left = max ((int)rcLeftEye.left, (int)m_rcReg.left + nSizeEyeX / 3);
	rcIntersectEyeFace.top = max ((int)rcLeftEye.top, (int)m_rcReg.top);
	rcIntersectEyeFace.right = min ((int)rcLeftEye.right, (int)m_rcReg.right);
	rcIntersectEyeFace.bottom = min ((int)rcLeftEye.bottom, (int)m_rcReg.bottom);
	rcLeftEye = rcIntersectEyeFace;
	rcRightEye = rcLeftEye;
	rcRightEye.left = nAvgX + nStdDevX / 4;
	rcRightEye.right = rcRightEye.left + nSizeEyeX;
	rcIntersectEyeFace.left = max ((int)rcRightEye.left, (int)m_rcReg.left);
	rcIntersectEyeFace.top = max ((int)rcRightEye.top, (int)m_rcReg.top);
	rcIntersectEyeFace.right = min ((int)rcRightEye.right, (int)m_rcReg.right - nSizeEyeX / 3);
	rcIntersectEyeFace.bottom = min ((int)rcRightEye.bottom, (int)m_rcReg.bottom);
	rcRightEye = rcIntersectEyeFace;
	SetEyes (rcLeftEye, rcRightEye);
}

bool IGRegion::Dilate (int nSize)
{
	const std::vector <IGSmartPixel*> vBoundaries = GetBoundaries();
	long ray = nSize / 2;
	long kMax = nSize - ray;
	for (std::vector <IGSmartPixel*>::const_iterator it = vBoundaries.begin(); it != vBoundaries.end(); ++it){
		for (vector <IGSmartPixel*>::const_iterator itNeighbourPixel = m_vPixels.cbegin(); itNeighbourPixel != m_vPixels.cend(); ++itNeighbourPixel){
			if (sqrt ((float)(((*itNeighbourPixel)->m_nX - (*it)->m_nX)*((*itNeighbourPixel)->m_nX - (*it)->m_nX) + ((*itNeighbourPixel)->m_nY - (*it)->m_nY)*((*itNeighbourPixel)->m_nY - (*it)->m_nY))) <= ray){
				(*it)->m_nLabel = m_nLabel;
			}
		}
	}
	return true;
}

bool IGRegion::Erode (int nSize)
{
	const std::vector <IGSmartPixel*> vBoundaries = GetBoundaries();
	long ray = nSize / 2;
	long kMax = nSize - ray;
	for (std::vector <IGSmartPixel*>::const_iterator it = vBoundaries.begin(); it != vBoundaries.end(); ++it){
		for (vector <IGSmartPixel*>::const_iterator itNeighbourPixel = m_vPixels.cbegin(); itNeighbourPixel != m_vPixels.cend(); ++itNeighbourPixel){
			if (sqrt ((float)(((*itNeighbourPixel)->m_nX - (*it)->m_nX)*((*itNeighbourPixel)->m_nX - (*it)->m_nX) + ((*itNeighbourPixel)->m_nY - (*it)->m_nY)*((*itNeighbourPixel)->m_nY - (*it)->m_nY))) <= ray){
				(*it)->m_nLabel = IGMARKER_BACKGROUND;
			}
		}
	}
	return true;
}

IGMarker* IGMarker::Create (ENUM_IGMARKER eType, int nLabel, const POINT& ptCenter, int nSize1, int nSize2, bool bSubdivided)
{
	switch (eType)
	{
	case IGMARKER_CIRCLE:
		return new IGMarkerCircle (nLabel, ptCenter, nSize1);
	case IGMARKER_DISC:
		return new IGMarkerDisc (nLabel, ptCenter, nSize1);
	case IGMARKER_CROWN:
		return new IGMarkerCrown (nLabel, ptCenter, nSize1, nSize2);
	case IGMARKER_HALFCIRCLE:
		return new IGMarkerHalfCircle (nLabel, ptCenter, nSize1);
	case IGMARKER_HALO:
		return new IGMarkerHalo (nLabel, ptCenter, nSize1, nSize2);
	case IGMARKER_RECTANGLE:
		return new IGMarkerRectangle (nLabel, ptCenter, nSize1, nSize2);
	case IGMARKER_FILLEDRECTANGLE:
		return new IGMarkerRectangle (nLabel, ptCenter, nSize1, nSize2, true);
	}
	return NULL;
}

IGMarker::IGMarker (int nLabel) : IGRegion (nLabel)
								, m_pCxMarker (NULL)
								, m_nRot (0)
{
}

IGMarker::IGMarker (int nLabel, CxImage& cxMarker, int nRot) : IGRegion (nLabel)
															, m_pCxMarker (&cxMarker)
															, m_nRot (nRot)
{
}

IGMarker::~IGMarker()
{
}

bool IGMarker::Mark (IGSmartLayer& layer)
{
	if (!m_pCxMarker)
		return false;
	int nHeight = layer.GetHeight();
	if (m_pCxMarker->GetHeight() != nHeight)
		return false;
	int nWidth = layer.GetWidth();
	if (m_pCxMarker->GetWidth() != nWidth)
		return false;
	for (int nRow = 0; nRow < nHeight; nRow++){
		BYTE *pCurSelection = m_pCxMarker->SelectionGetPointer (0, nRow);
		for (int nCol = 0; nCol < nWidth; nCol++){
			if (*pCurSelection++ != 0){
				IGSmartPixel *pCurPixel = layer.GetPixel (nCol, nRow);
				pCurPixel->m_nLabel = m_nLabel;
				pCurPixel->m_gradient = 0;
				m_vPixels.push_back (pCurPixel);
			}
		}
	}
	return true;
}

void IGMarker::Odd()
{
	SetLabel (2 * m_nLabel + 1);
}

IGMarkerSubdivided::IGMarkerSubdivided (int nLabel, CxImage& cxMarker) : IGMarker (nLabel, cxMarker)
{
}

IGMarkerSubdivided::IGMarkerSubdivided(int nLabel, const std::vector <IGSmartPixel*>& vPixels) : IGMarker (nLabel)
{
	m_vPixels = vPixels;
}

IGMarkerSubdivided::~IGMarkerSubdivided()
{
	while (!m_vSubdivisions.empty())
	{
		delete m_vSubdivisions.back();
		m_vSubdivisions.pop_back();
	}
}

void IGMarkerSubdivided::SubDivide (IGSmartLayer& layer)
{
	// construct pixel set
	set <IGSmartPixel*> setPixels;
	bool bNoAdd = !m_vPixels.empty();
	if (bNoAdd){
		for (vector <IGSmartPixel*>::const_iterator itPixel = m_vPixels.begin(); itPixel != m_vPixels.end(); ++itPixel){
			(*itPixel)->m_nLabel = -1;
			setPixels.insert (*itPixel);
		}
	}
	else{
		for (int nRow = 0; nRow < (int)layer.GetHeight(); nRow++){
			BYTE *pCurSelection = m_pCxMarker->SelectionGetPointer (0, nRow);
			for (int nCol = 0; nCol < (int)layer.GetWidth(); nCol++){
				if (*pCurSelection++ != 0){
					IGSmartPixel *pCurPixel = layer.GetPixel (nCol, nRow);
					pCurPixel->m_nLabel = -1;
					setPixels.insert (pCurPixel);
				}
			}
		}
	}
	// construct subdivisions
	m_vSubdivisions.clear();
	int nNumLabel = m_nLabel;
	int nNextNumLabel = m_nLabel + 1;
	IGMarker *pLayerMarker = NULL;
	int nLayerMarkerSize = 0;
	_ASSERTE (!setPixels.empty() && "no pixel in marker subdivision");
	if (setPixels.empty())
		return;
	while (!setPixels.empty())
	{
		// create a new pool
		set <IGSmartPixel*>::iterator itPixels = setPixels.end();
		--itPixels;
		IGSmartPixel *pCurPixel = *itPixels;
		pCurPixel->m_nLabel = nNumLabel;
		setPixels.erase (itPixels);
		// fill plateau
		IGMarker *pvPool = new IGMarker (nNumLabel);
		stack <IGSmartPixel*> stPlateau;
		stPlateau.push (pCurPixel);
		pvPool->AddPixel (pCurPixel);
		while (!stPlateau.empty())
		{
			const vector <IGSmartPixel*>& vPoolQueueExtension = stPlateau.top()->GetNeighbours();
			stPlateau.pop();
			for (vector <IGSmartPixel*>::const_iterator itPlat = vPoolQueueExtension.begin(); itPlat != vPoolQueueExtension.end(); ++itPlat){
				if ((*itPlat)->m_nLabel == -1){
					(*itPlat)->m_nLabel = nNumLabel;
					stPlateau.push (*itPlat);
					pvPool->AddPixel (*itPlat);
					if (!bNoAdd)
						m_vPixels.push_back (*itPlat);
					setPixels.erase (setPixels.find (*itPlat));
				}
			}
		}
		// register plateau as a new subdivision
		m_vSubdivisions.push_back (pvPool);
		nNumLabel = nNextNumLabel;
		nNextNumLabel++;
	}
}

bool IGMarkerSubdivided::Mark (IGSmartLayer& layer)
{
	if (!m_pCxMarker)
		return false;
	SubDivide (layer);
	return true;
}

bool IGMarkerLayer::Mark (IGSmartLayer& layer)
{
	// construct pixel set
	set <IGSmartPixel*> setPixels;
	for (int nRow = 0; nRow < (int)layer.GetHeight(); nRow++){
		BYTE *pCurSelection = m_pCxMarker->SelectionGetPointer (0, nRow);
		for (int nCol = 0; nCol < (int)layer.GetWidth(); nCol++){
			if (*pCurSelection++ != 0){
				IGSmartPixel *pPixel = layer.GetPixel (nCol, nRow);
				pPixel->m_nLabel = -1;
				setPixels.insert (pPixel);
			}
		}
	}
	IGMarker *pLayerMarker = NULL;
	int nLayerMarkerSize = 0;
	_ASSERTE (!setPixels.empty() && "no pixel in marker");
	if (setPixels.empty())
		return false;
	while (!setPixels.empty())
	{
		// create a new pool
		set <IGSmartPixel*>::iterator itPixels = setPixels.end();
		--itPixels;
		IGSmartPixel *pCurPixel = *itPixels;
		pCurPixel->m_nLabel = m_nLabel;
		setPixels.erase (itPixels);
		// fill plateau
		IGMarker *pvPool = new IGMarker (m_nLabel, *m_pCxMarker);
		stack <IGSmartPixel*> stPlateau;
		stPlateau.push (pCurPixel);
		pvPool->AddPixel (pCurPixel);
		while (!stPlateau.empty())
		{
			const vector <IGSmartPixel*>& vPoolQueueExtension = stPlateau.top()->GetNeighbours();
			stPlateau.pop();
			for (vector <IGSmartPixel*>::const_iterator itPlat = vPoolQueueExtension.begin(); itPlat != vPoolQueueExtension.end(); ++itPlat){
				if ((*itPlat)->m_nLabel == -1){
					(*itPlat)->m_nLabel = m_nLabel;
					stPlateau.push (*itPlat);
					pvPool->AddPixel (*itPlat);
					setPixels.erase (setPixels.find (*itPlat));
				}
			}
		}
		if ((int)pvPool->m_vPixels.size() > nLayerMarkerSize){
			if (pLayerMarker){
				pLayerMarker->SetLabel (-2);
				delete pLayerMarker;
			}
			nLayerMarkerSize = pvPool->m_vPixels.size();
			pLayerMarker = pvPool;
		}
		else{
			delete pvPool;
		}
	}
	if (!pLayerMarker)
		return false;
	if (!pLayerMarker->Mark (layer))
		return false;
	m_vPixels = pLayerMarker->m_vPixels;
	delete pLayerMarker;
	return true;
}

void IGMarkerSubdivided::Odd()
{
	for (vector <IGMarker *>::const_iterator itMarker = m_vSubdivisions.begin(); itMarker != m_vSubdivisions.end(); ++itMarker)
		(*itMarker)->Odd();
}

void IGMarkerSubdivided::Select (BYTE level)
{
	for (vector <IGMarker *>::const_iterator itMarker = m_vSubdivisions.begin(); itMarker != m_vSubdivisions.end(); ++itMarker)
		(*itMarker)->Select (level);
}

IGMarker* IGMarkerSubdivided::GetMarker (int nIdx)
{
	if (nIdx >= (int)m_vSubdivisions.size())
		return NULL;
	vector <IGMarker *>::const_iterator itMarker = m_vSubdivisions.begin();
	for (int nIdxSub = 0; nIdxSub < nIdx; nIdxSub++)
		++itMarker;
	return *itMarker;
}

IGMarkerRectangle::IGMarkerRectangle(int nLabel, const POINT& ptCenter, int nWidth, int nHeight, bool bFilled) : IGMarker (nLabel)
																									, m_ptCenter (ptCenter)
																									, m_nWidth (nWidth)
																									, m_nHeight (nHeight)
																									, m_bFilled (bFilled)
{
}

IGMarkerRectangle::~IGMarkerRectangle()
{
}

bool IGMarkerRectangle::Mark (IGSmartLayer& layer)
{
	int nMinX = max(0, m_ptCenter.x - m_nWidth / 2);
	int nMinY = max(0, m_ptCenter.y - m_nHeight / 2);
	int nMaxX = min(m_ptCenter.x + m_nWidth / 2 + 1, (int)layer.GetWidth());
	int nMaxY = min(m_ptCenter.y + m_nHeight / 2 + 1, (int)layer.GetHeight());
	if (m_bFilled) {
		for (int idxPixX = nMinX; idxPixX < nMaxX; idxPixX++)
		{		
			for (int idxPixY = nMinY; idxPixY < nMaxY; idxPixY++)
			{
				IGSmartPixel *pCurPixel = layer.GetPixel (idxPixX, idxPixY);
				pCurPixel->m_nLabel = m_nLabel;
				m_vPixels.push_back (pCurPixel);
			}		
		}
	}
	else {
		for (int idxPixX = nMinX; idxPixX < nMaxX; idxPixX++)
		{		
			IGSmartPixel *pCurPixel = layer.GetPixel (idxPixX, nMinY);
			pCurPixel->m_nLabel = m_nLabel;
			m_vPixels.push_back (pCurPixel);

			pCurPixel = layer.GetPixel (idxPixX, nMaxY - 1);
			pCurPixel->m_nLabel = m_nLabel;
			m_vPixels.push_back (pCurPixel);
		}
		for (int idxPixY = nMinY; idxPixY < nMaxY; idxPixY++)
		{
			IGSmartPixel *pCurPixel = layer.GetPixel (nMinX, idxPixY);
			pCurPixel->m_nLabel = m_nLabel;
			m_vPixels.push_back (pCurPixel);	

			pCurPixel = layer.GetPixel (nMaxX - 1, idxPixY);
			pCurPixel->m_nLabel = m_nLabel;
			m_vPixels.push_back (pCurPixel);	
		}
	}
	return true;
}


IGMarkerCircle::IGMarkerCircle(int nLabel, const POINT& ptCenter, int nRadius, bool bDisc) : IGMarker (nLabel)
																							, m_bDisc (bDisc)
																							, m_ptCenter (ptCenter)
																							, m_nRadius (nRadius)
{
}

IGMarkerCircle::~IGMarkerCircle()
{
}

bool IGMarkerCircle::Mark (IGSmartLayer& layer)
{
	int nMinX = max(0, m_ptCenter.x - m_nRadius);
	int nMinY = max(0, m_ptCenter.y - m_nRadius);
	int nMaxX = min(m_ptCenter.x + m_nRadius + 1, (int)layer.GetWidth());
	int nMaxY = min(m_ptCenter.y + m_nRadius + 1, (int)layer.GetHeight());
	for (int idxPixX = nMinX; idxPixX < nMaxX; idxPixX++)
	{
		for (int idxPixY = nMinY; idxPixY < nMaxY; idxPixY++)
		{
			int nDist = (int)sqrt ((float)((m_ptCenter.x - idxPixX) * (m_ptCenter.x - idxPixX) + (m_ptCenter.y - idxPixY) * (m_ptCenter.y - idxPixY)));
			if (nDist <= m_nRadius)
			{
				IGSmartPixel *pCurPixel = layer.GetPixel (idxPixX, idxPixY);
				if (m_bDisc){					
					pCurPixel->m_nLabel = m_nLabel;
					m_vPixels.push_back (pCurPixel);
				}
				else if (nDist == m_nRadius){
					pCurPixel->m_nLabel = m_nLabel;
					m_vPixels.push_back (pCurPixel);
				}
			}
		}
	}
	return true;
}

IGMarkerHalfCircle::IGMarkerHalfCircle(int nLabel, const POINT& ptCenter, int nRadius, bool bDisc) : IGMarkerCircle (nLabel, ptCenter, nRadius, bDisc)
{
}

IGMarkerHalfCircle::~IGMarkerHalfCircle()
{
}

bool IGMarkerHalfCircle::Mark (IGSmartLayer& layer)
{
	int nMinX = max(0, m_ptCenter.x - m_nRadius);
	int nMinY = m_ptCenter.y;
	int nMaxX = min(m_ptCenter.x + m_nRadius + 1, (int)layer.GetWidth());
	int nMaxY = min(m_ptCenter.y + m_nRadius + 1, (int)layer.GetHeight());
	for (int idxPixX = nMinX; idxPixX < nMaxX; idxPixX++)
	{
		for (int idxPixY = nMinY; idxPixY < nMaxY; idxPixY++)
		{
			int nDist = (int)sqrt ((float)((m_ptCenter.x - idxPixX) * (m_ptCenter.x - idxPixX) + (m_ptCenter.y - idxPixY) * (m_ptCenter.y - idxPixY)));
			if (nDist <= m_nRadius)
			{
				IGSmartPixel *pCurPixel = layer.GetPixel (idxPixX, idxPixY);
				if (m_bDisc){					
					pCurPixel->m_nLabel = m_nLabel;
					m_vPixels.push_back (pCurPixel);
				}
				else if (nDist == m_nRadius){
					pCurPixel->m_nLabel = m_nLabel;
					m_vPixels.push_back (pCurPixel);
				}
			}
		}
	}
	return true;
}

IGMarkerDisc::IGMarkerDisc(int nLabel, const POINT& ptCenter, int nRadius) : IGMarkerCircle (nLabel, ptCenter, nRadius, true)
{
}

IGMarkerDisc::~IGMarkerDisc()
{
}

IGMarkerCrown::IGMarkerCrown(int nLabel, const POINT& ptCenter, int nRadius, int nSize) : IGMarkerCircle (nLabel, ptCenter, nRadius)
{
	m_nRadiusMax = nRadius + nSize;
}

IGMarkerCrown::~IGMarkerCrown()
{
}

bool IGMarkerCrown::Mark (IGSmartLayer& layer)
{
	int nMinX = max(0, m_ptCenter.x - m_nRadiusMax);
	int nMinY = max(0, m_ptCenter.y - m_nRadiusMax);
	int nMaxX = min(m_ptCenter.x + m_nRadiusMax + 1, (int)layer.GetWidth());
	int nMaxY = min(m_ptCenter.y + m_nRadiusMax + 1, (int)layer.GetHeight());
	for (int idxPixX = nMinX; idxPixX < nMaxX; idxPixX++)
	{
		for (int idxPixY = nMinY; idxPixY < nMaxY; idxPixY++)
		{
			int nDist = (int)sqrt ((float)((m_ptCenter.x - idxPixX) * (m_ptCenter.x - idxPixX) + (m_ptCenter.y - idxPixY) * (m_ptCenter.y - idxPixY)));
			if (nDist <= m_nRadiusMax && nDist >= m_nRadius)
			{
				IGSmartPixel *pCurPixel = layer.GetPixel (idxPixX, idxPixY);					
				pCurPixel->m_nLabel = m_nLabel;
				m_vPixels.push_back (pCurPixel);
			}
		}
	}
	layer.AddRegion (this);
	return true;
}

IGMarkerHalo::IGMarkerHalo(int nLabel, const POINT& ptCenter, int nRadius, int nSize) : IGMarkerCrown (nLabel, ptCenter, nRadius, nSize)
{
}

IGMarkerHalo::~IGMarkerHalo()
{
}

bool IGMarkerHalo::Mark (IGSmartLayer& layer)
{
	int nMinX = max(0, m_ptCenter.x - m_nRadiusMax);
	int nMinY = max(0, m_ptCenter.y - m_nRadius / 3);
	int nMaxX = min(m_ptCenter.x + m_nRadiusMax + 1, (int)layer.GetWidth());
	int nMaxY = min(m_ptCenter.y + m_nRadiusMax + 1, (int)layer.GetHeight());
	for (int idxPixX = nMinX; idxPixX < nMaxX; idxPixX++)
	{
		for (int idxPixY = nMinY; idxPixY < nMaxY; idxPixY++)
		{
			int nDist = (int)sqrt ((float)((m_ptCenter.x - idxPixX) * (m_ptCenter.x - idxPixX) + (m_ptCenter.y - idxPixY) * (m_ptCenter.y - idxPixY)));
			if (nDist <= m_nRadiusMax && nDist >= m_nRadius)
			{
				IGSmartPixel *pCurPixel = layer.GetPixel (idxPixX, idxPixY);					
				pCurPixel->m_nLabel = m_nLabel;
				m_vPixels.push_back (pCurPixel);
			}
		}
	}
	layer.AddRegion (this);
	return true;
}

bool IGSmartLayer::LPE_TopDown (int nNbRegions, bool bWaterfall)
{
	// construct pixel set
	set <IGSmartPixel*> setPixels;
	IGSmartPixel *pCurPixel = NULL;
	int nSize = 0;
	if (m_vvTreeRegions.size() > 0){
		nSize = m_vvTreeRegions.back().size();
		vector <IGSmartPixel*>::const_iterator itPixel = m_vvTreePixels.back().begin();
		do{
			pCurPixel = *itPixel;
			pCurPixel->m_nLabel = 0;
			setPixels.insert (pCurPixel);
		}while (++itPixel != m_vvTreePixels.back().end());
	}
	else{
		// first segmentation level
		nSize = head.biWidth * head.biHeight;
		IGSmartPixel *pFirstPixel = m_pPixels;
		pCurPixel = pFirstPixel;
		while (nSize--){
			pCurPixel->m_nLabel = 0;
			setPixels.insert (pCurPixel++);
		}
	}
	// top-down LPE
	unsigned int nRandPixelIdx = 0;
	int nNumLabel = 1;
	int nNextNumLabel = 2;
	while (!setPixels.empty())
	{
		// create a new pool		
		vector <IGSmartPixel*> vPool, vPoolQueue;
		set <IGSmartPixel*>::iterator itPixels = setPixels.end();
		--itPixels;
		pCurPixel = *itPixels;
		pCurPixel->m_nLabel = nNumLabel;
		setPixels.erase (itPixels);
		vPoolQueue.push_back (pCurPixel);
		while (!vPoolQueue.empty())
		{
			IGSmartPixel *pCurPoolPixel = vPoolQueue.back();
			// add the current pixel to the pool
			vPool.push_back (pCurPoolPixel);
			vPoolQueue.pop_back();
			// fill plateau
			stack <IGSmartPixel*> stPlateau;
			stPlateau.push (pCurPoolPixel);
			IGSmartPixel *pMinGradient = pCurPoolPixel;
			while (!stPlateau.empty())
			{
				IGSmartPixel *pCurPlateauPixel = stPlateau.top();
				stPlateau.pop();
				const vector <IGSmartPixel*>& vPoolQueueExtension = pCurPlateauPixel->GetNeighbours();
				for (vector <IGSmartPixel*>::const_iterator itPlat = vPoolQueueExtension.begin(); itPlat != vPoolQueueExtension.end(); ++itPlat){
					if ((*itPlat)->m_nLabel != nNumLabel){
						if ((*itPlat)->m_gradient == pCurPoolPixel->m_gradient) {
							(*itPlat)->m_nLabel = nNumLabel;
							stPlateau.push (*itPlat);
							vPool.push_back (*itPlat);
							setPixels.erase (*itPlat);
						}
						else if ((*itPlat)->m_gradient < pMinGradient->m_gradient){
							pMinGradient = *itPlat;
						}
					}
				}
			}
			if (pMinGradient != pCurPoolPixel)
			{
				if (pMinGradient->m_nLabel != 0){
					// merge pools
					nNextNumLabel = nNumLabel;
					nNumLabel = pMinGradient->m_nLabel;
					for (vector <IGSmartPixel*>::const_iterator it = vPool.begin(); it != vPool.end(); ++it)
						(*it)->m_nLabel = nNumLabel;
					for (vector <IGSmartPixel*>::const_iterator it = vPoolQueue.begin(); it != vPoolQueue.end(); ++it)
						(*it)->m_nLabel = nNumLabel;
				}
				else{
					pMinGradient->m_nLabel = nNumLabel;
					setPixels.erase (pMinGradient);
					vPoolQueue.push_back (pMinGradient);
				}
			}
		}
		nNumLabel = nNextNumLabel;
		nNextNumLabel++;
	}	
	if (nNumLabel > 0){		
		while (m_vRegions.size() > 0){
			delete m_vRegions.back();
			m_vRegions.pop_back();
		}
		for (int idxReg = 1; idxReg < nNumLabel; idxReg++)
			m_vRegions.push_back (new IGRegion(idxReg));
		if (m_vvTreeRegions.size() > 0){
			nSize = m_vvTreeRegions.back().size();
			vector <IGSmartPixel*>::const_iterator itPixel = m_vvTreePixels.back().begin();
			IGSmartPixel *pPixel = NULL;
			do{
				pPixel = *itPixel;
				m_vRegions[pPixel->m_nLabel - 1]->AddPixel(pPixel);
			} while (++itPixel != m_vvTreePixels.back().end());
		}
		else{
			// first segmentation level
			if (bWaterfall){
				nSize = head.biWidth * head.biHeight;
				IGSmartPixel *pPixel = m_pPixels;
				while (nSize--){
					m_vRegions[pPixel->m_nLabel - 1]->AddPixel(pPixel);
					pPixel++;
				}
			}
			else{
				nSize = head.biWidth * head.biHeight;
				IGSmartPixel *pPixel = m_pPixels;
				while (nSize--){
					m_vRegions[pPixel->m_nLabel - 1]->AddSimplePixel(pPixel);
					pPixel++;
				}
				return true;
			}
		}
		if (m_vRegions.size() < 3)
			return true;
		m_vvTreeRegions.push_back (m_vRegions);
		if (!ConstructSaddle())
			return false;
		m_vRegions.clear();
		if (nNumLabel - 1 > nNbRegions)
			return LPE_TopDown (nNbRegions, true);
	}
	return true;
}

bool IGSmartLayer::LPE_FAH (IGMarker **ppMarkers, int& nNbMarkers, bool bShowMarkerMode, bool bShowGradient)
{
	if (nNbMarkers < 1)
		return false;
	if (bShowGradient)
	{
		int nSize = head.biWidth * head.biHeight;
		IGSmartPixel *pCurPixel = m_pPixels;
		while (nSize--){
			BYTE *pPix = **pCurPixel++;
			*pPix++ = pCurPixel->m_gradient;
			*pPix++ = pCurPixel->m_gradient;
			*pPix++ = pCurPixel->m_gradient;
		}
	}
	// mark the picture
	int nNbSubMarkers = 1;
	ppMarkers[0]->Mark (*this);
	ppMarkers[0]->SetLabel(1);
	for (int idxMarker = 1; idxMarker < nNbMarkers; idxMarker++)
	{
		if (ppMarkers[idxMarker] == NULL)
			return false;
		// set all label with an odd value
		ppMarkers[idxMarker]->Mark (*this);
		ppMarkers[idxMarker]->Odd();
		nNbSubMarkers += ppMarkers[idxMarker]->GetNbMarkers();
	}
	if (bShowGradient || bShowMarkerMode)
		return true;
	// construct pixel set, ordered by gradient
	int nSize = head.biWidth * head.biHeight;
	IGSmartPixel **pSortedPixels = new IGSmartPixel* [nSize];
	IGSmartPixel **pCurSortedPixel = pSortedPixels;
	IGSmartPixel *pCurPixel = m_pPixels;
	int nCurSize = nSize;
	while (nCurSize--)
		*pCurSortedPixel++ = pCurPixel++;
	qsort (pSortedPixels, nSize, sizeof (IGSmartPixel*), IGSmartPixel::less);
	// create the FAH
	vector <list <IGSmartPixel*>*> vLevels;
	vector <list <IGSmartPixel*>*> vFAH;
	int k = -1;
	int nbPixels = 0;
	pCurSortedPixel = pSortedPixels;
	list <IGSmartPixel*> *pCurLevelStack = NULL;
	int nNbRegions = 0;
	bool *tbRegions = new bool [nNbSubMarkers];
	vector <list <IGSmartPixel*>*>::iterator itFAH;
	if (!bShowMarkerMode)
	{
		::memset (tbRegions, 0, sizeof (bool) * nNbSubMarkers);
		for(int idxLevel = 0; idxLevel < 256; idxLevel++)
		{
			bool bNewLevel = true;
			while ((*pCurSortedPixel)->m_gradient == idxLevel)
			{
				if (bNewLevel){
					bNewLevel = false;
					k++;
					pCurLevelStack = new list <IGSmartPixel*>();
					vLevels.push_back (pCurLevelStack);
					vFAH.push_back (new list <IGSmartPixel*>());
					itFAH = vFAH.end();
					--itFAH;
				}
				(*pCurSortedPixel)->m_gradient = k;
				pCurLevelStack->push_back ((*pCurSortedPixel));
				if ((*pCurSortedPixel)->m_nLabel > 0){	// initialize FAH with marked pixels
					int nRealLabel = (((*pCurSortedPixel)->m_nLabel - 1) / 2);
					if (!tbRegions [nRealLabel]){
						tbRegions [nRealLabel] = true;
						nNbRegions++;
					}
					(*itFAH)->push_back (*pCurSortedPixel);
				}
				else{
					(*pCurSortedPixel)->m_nLabel = 0;
				}
				if (++nbPixels == nSize)
					break;
				pCurSortedPixel++;
			}
			if (nbPixels == nSize)
				break;
		}
		m_vRegions.resize (nNbRegions);
		int idxRegion = 0;
		for(int idxMarker = 0; idxMarker < nNbMarkers; idxMarker++)
		{
			for(int idxSubMarker = 0; idxSubMarker < ppMarkers [idxMarker]->GetNbMarkers(); idxSubMarker++){
				if (tbRegions [idxMarker]){
					IGRegion *pNewRegion = new IGRegion (*ppMarkers [idxMarker]->GetMarker (idxSubMarker));
					pNewRegion->SetLabel (2 * idxRegion + 1);
					m_vRegions [idxRegion++] = pNewRegion;
				}
			}
		}
	}
	delete [] pSortedPixels;
	delete [] tbRegions;
	if (m_vRegions.size() < 2)
		return false;
	// start flooding
	int C = 2 * nNbRegions + 1;
	int n = 0;
	int N = vFAH.size();
	list <IGSmartPixel*> *plAlpha = new list <IGSmartPixel*>(); // next intermediary stack
	k = 0;
	while (k < N)
	{
		n++;
		pCurLevelStack = vFAH[k];
		while (!pCurLevelStack->empty())
		{
			//processing pebble i current stack
			pCurPixel = (*pCurLevelStack).front();
			(*pCurLevelStack).pop_front();
			if (pCurPixel->m_nLabel == C){
				pCurPixel->m_gradient = k;
			}
			else{
				pCurPixel->m_nLabel++;
				// processing neighbour pixels
				const vector <IGSmartPixel*>& vNeighbours = pCurPixel->GetNeighbours();
				for (vector <IGSmartPixel*>::const_iterator itPixels = vNeighbours.begin(); itPixels != vNeighbours.end(); ++itPixels){
					IGSmartPixel *pCurNeighbour = *itPixels;
					if (pCurNeighbour->m_nLabel == 0)
					{
						if (pCurNeighbour->m_gradient <= k)
						{
							pCurNeighbour->m_gradient = N + n;
							plAlpha->push_back (pCurNeighbour);
						}
						else if (pCurNeighbour->m_gradient <= N)
								vFAH[pCurNeighbour->m_gradient]->push_back (pCurNeighbour);

						pCurNeighbour->m_nLabel = pCurPixel->m_nLabel - 1;
					}
					else
					{
						if (pCurNeighbour->m_nLabel % 2 == 1)
						{
							if (pCurPixel->m_gradient != pCurNeighbour->m_gradient)
							{
								if (pCurNeighbour->m_nLabel != pCurPixel->m_nLabel - 1)
									pCurNeighbour->m_nLabel = C;
							}
						}
					}
				}
				if (n > 1)
					pCurPixel->m_gradient = k;
			}
		} 

		// transfer temporary stack to current stack
		if (plAlpha->empty()) {
			k++;
			n = 0;
		}
		else {
			while (!plAlpha->empty()){
				pCurLevelStack->push_back (plAlpha->back());
				plAlpha->pop_back();
			}
		}
	}
	// release the level and FAH stacks
	while (!vLevels.empty()){
		delete vLevels.back();
		vLevels.pop_back();
	}
	while (!vFAH.empty()){
		delete vFAH.back();
		vFAH.pop_back();
	}
	for (vector <IGRegion*>::const_iterator itRegion = m_vRegions.begin(); itRegion != m_vRegions.end(); ++itRegion){
		(*itRegion)->m_nLabel = ((*itRegion)->m_nLabel - 1) / 2;
		(*itRegion)->m_vPixels.clear();
	}
	IGSmartPixel *pPixel = GetPixels();
	nSize = head.biWidth * head.biHeight;
	if (nNbMarkers > 0){
		while (nSize--){
			pPixel->m_nLabel = (pPixel->m_nLabel - 1) / 2;
			if (pPixel->m_nLabel < nNbRegions)
				m_vRegions[pPixel->m_nLabel]->AddSimplePixel(pPixel);
			pPixel++;
		}
	}
	else{
		while (nSize--){
			pPixel->m_nLabel = (pPixel->m_nLabel - 1) / 2;
			if (pPixel->m_nLabel < nNbRegions)
				m_vRegions[pPixel->m_nLabel]->AddPixel(pPixel);
			pPixel++;
		}
	}
	nNbMarkers = nNbRegions;
	return m_vRegions.size() > 1;
}

void IGRegion::AddSimplePixel (IGSmartPixel *p24bPixel)
{
	p24bPixel->m_pRegion = this;
	m_vPixels.push_back (p24bPixel);
}

void IGRegion::AddPixel (IGSmartPixel *p24bPixel)
{
	p24bPixel->m_pRegion = this;
	m_vPixels.push_back (p24bPixel);
	vector <IGSmartPixel*> vNeighbours = p24bPixel->GetNeighbours();
	for (vector <IGSmartPixel*>::const_iterator itNei = vNeighbours.begin(); itNei != vNeighbours.end(); ++itNei){
		if ((*itNei)->m_nLabel != m_nLabel){
			bool bFound = false;
			for (vector <IGSmartPixel*>::iterator itNeiSaddle = m_vNeighbourSaddles.begin(); itNeiSaddle != m_vNeighbourSaddles.end(); ++itNeiSaddle){
				if ((*itNeiSaddle)->m_nLabel == (*itNei)->m_nLabel){
					bFound = true;
					if (max (p24bPixel->m_gradient, (*itNei)->m_gradient) < (*itNeiSaddle)->m_gradient){
						(*itNei)->m_gradient = max (p24bPixel->m_gradient, (*itNei)->m_gradient);
						(*itNeiSaddle) = *itNei;
					}
					break;
				}
			}
			if (!bFound){
				(*itNei)->m_nNeighbourLabel = m_nLabel;
				(*itNei)->m_gradient = max (p24bPixel->m_gradient, (*itNei)->m_gradient);
				m_vNeighbourSaddles.push_back(*itNei);
			}
		}
	}
	if (p24bPixel->m_gradient < m_minGradient)
		m_minGradient = p24bPixel->m_gradient;
}