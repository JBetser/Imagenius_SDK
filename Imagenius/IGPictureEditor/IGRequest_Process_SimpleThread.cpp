#include "StdAfx.h"
#include "IGAnswer.h"
#include "IGRequest_simple.h"
#include "IGWorkspace.h"
#include "IGImageProcMessages.h"
#include "IGProperties.h"
#include "IGTexture.h"

using namespace std;
using namespace IGLibrary;

bool IGRequestAddLayer::SimpleThreadProcess()
{
	return GetFrame()->CreateNewCalc();
}

LRESULT IGRequestAddLayer::PostProcessing (IGImageProcMessage *pMessage)
{
	if (!pMessage->GetResult())
	{
		int nNbLayers = GetFrame()->GetNbLayers();
		if (nNbLayers > m_nFirstHistoryParam)
		{
			// if adding layer failed, remove the loaded layer
			POSTPONE (pMessage);
			GetFrame()->RemoveCalc (nNbLayers - 1);
		}
		return IGREQUEST_FAILED;
	}
	return IGREQUEST_PROCESSED;
}

bool IGRequestRemoveLayer::SimpleThreadProcess()
{
	IGFrame *pCurFrame = GetFrame();
	size_t szNextComa = wstring::npos;
	wstring wsCurId;
	list <int> lpLayerIds;
	do
	{
		szNextComa = m_wsLayerIds.find (L',');
		wsCurId = m_wsLayerIds.substr (0, szNextComa);
		int nCurId = _wtoi (wsCurId.c_str());
		if (!pCurFrame->GetLayer (nCurId))
			return false;
		lpLayerIds.push_back (nCurId);		
		if (szNextComa != wstring::npos)
			m_wsLayerIds = m_wsLayerIds.substr (szNextComa + 1);
	} while (szNextComa != wstring::npos);	
	if ((int)lpLayerIds.size() >= pCurFrame->GetNbLayers())
		return false;
	while (!lpLayerIds.empty())
	{
		// remove max layer pos
		int idLayerMax = 0;
		for(list <int>::const_iterator itLayer = lpLayerIds.begin(); itLayer != lpLayerIds.end(); ++itLayer){
			if (*itLayer > idLayerMax)
				idLayerMax = *itLayer;
		}
		if (FAILED (pCurFrame->RemoveCalc (idLayerMax)))
			return false;
		lpLayerIds.remove(idLayerMax);
	}
	return true;
}

bool IGRequestMoveLayer::SimpleThreadProcess()
{
	return SUCCEEDED (GetFrame()->MoveCalc (m_nLayerId, m_nLayerDestId));
}

bool IGRequestMove::Initialize()
{
	if (!IGRequest::Initialize())
		return false;
	int nVectorX, nVectorY;
	GetParameter (IGREQUEST_DIRECTIONX, nVectorX);
	GetParameter (IGREQUEST_DIRECTIONY, nVectorY);
	double dSeadragonToPixelRatioX = (double)GetFrame()->GetWidth() / PIXEL_SELECTOR_PRECISION;
	double dSeadragonToPixelRatioY = (double)GetFrame()->GetHeight() / PIXEL_SELECTOR_PRECISION;
	double dRatioWidthHeight = (double)GetFrame()->GetWidth() / (double)GetFrame()->GetHeight();
	if (dRatioWidthHeight > 1)
		dSeadragonToPixelRatioY *= dRatioWidthHeight;
	else
		dSeadragonToPixelRatioX /= dRatioWidthHeight;
	m_ptVector.x = (int)(nVectorX * dSeadragonToPixelRatioX);
	m_ptVector.y = (int)(nVectorY * dSeadragonToPixelRatioY) * -1;
	IGLibrary::IGLayer *pCurLayer = GetFrame()->GetLayer(m_nLayerId);
	// rewind last history move steps
	int nCurStepId = GetFrame()->GetCurrentStepId();
	POINT ptVectorCorrection; ptVectorCorrection.x = 0; ptVectorCorrection.y = 0;
	while (!::wcscmp (GetFrame()->GetStepName (nCurStepId), L"Move") )
	{
		const IGLibrary::IGFrameActionMove& curStep = dynamic_cast <const IGLibrary::IGFrameActionMove&> (GetFrame()->GetAction (nCurStepId));
		ptVectorCorrection.x += curStep.m_ptVector.x;
		ptVectorCorrection.y += curStep.m_ptVector.y;
		nCurStepId--;				
	}
	if (!GetFrame()->RestoreStep (nCurStepId))
		return false;	
	if (pCurLayer->SelectionIsValid()) {			
		m_eStepId = IGLibrary::IGFRAMEHISTORY_STEP_MOVEPIXELS;
		ZeroMemory (&m_rcLayerPosAndVector, sizeof (RECT));
		m_ptVector.y *= -1;
		m_ptVector.x += ptVectorCorrection.x;
		m_ptVector.y += ptVectorCorrection.y;
		m_rcLayerPosAndVector.top = m_nLayerId;
		m_rcLayerPosAndVector.left = m_ptVector.x;
		m_rcLayerPosAndVector.right = m_ptVector.y;
		m_nFirstHistoryParam = (int)&m_rcLayerPosAndVector;			
		ZeroMemory (&m_rcSubLayer, sizeof (RECT));
		pCurLayer->SelectionGetBox (m_rcSubLayer);
		if (m_ptVector.x > 0)
			m_rcSubLayer.right = min (GetFrame()->GetWidth() - 1, m_rcSubLayer.right + m_ptVector.x);
		else
			m_rcSubLayer.left = max (0, m_rcSubLayer.left + m_ptVector.x);
		if (m_ptVector.y > 0)
			m_rcSubLayer.bottom = max (0, m_rcSubLayer.bottom - m_ptVector.y);
		else
			m_rcSubLayer.top = min (GetFrame()->GetHeight() - 1, m_rcSubLayer.top - m_ptVector.y);
		if ((m_rcSubLayer.right == 0) || (m_rcSubLayer.top == 0) ||
			(m_rcSubLayer.right + m_ptVector.x <= 0) || (m_rcSubLayer.top - m_ptVector.y <= 0) ||
			(m_rcSubLayer.left + m_ptVector.x >= GetFrame()->GetWidth() - 1) || (m_rcSubLayer.bottom - m_ptVector.y >= GetFrame()->GetHeight() - 1))
		{
			m_rcSubLayer.right = -1;
			m_rcSubLayer.top = -1;
		}
		m_nSecondHistoryParam = (int)&m_rcSubLayer;
	}
	else {
		m_eStepId = IGLibrary::IGFRAMEHISTORY_STEP_MOVE;
		m_nFirstHistoryParam = m_nLayerId;
		m_ptVector.x += ptVectorCorrection.x;
		m_ptVector.y += ptVectorCorrection.y;
		m_nSecondHistoryParam = (int)&m_ptVector;
	}
	return true;
}

bool IGRequestMove::SimpleThreadProcess()
{
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;
	if (pLayer->SelectionIsValid())
		return pLayer->MovePixels (m_ptVector.x, -m_ptVector.y);
	else
		pLayer->Move (m_ptVector.x, -m_ptVector.y);
	return true;
}

bool IGRequestRotateAndResize::Initialize()
{
	if (!IGRequest::Initialize())
		return false;
	int nRate, nAngle;
	GetParameter (IGREQUEST_ANGLE, nAngle);		
	GetParameter (IGREQUEST_RATE, nRate);
	m_fRate = (float)nRate / (float)PIXEL_SELECTOR_PRECISION;	
	m_ptVector.x = (int)(PIXEL_SELECTOR_PRECISION * cos (PI * (double)nAngle / 180.0));
	m_ptVector.y = (int)(PIXEL_SELECTOR_PRECISION * sin (PI * (double)nAngle / 180.0));
	IGLibrary::IGLayer *pCurLayer = GetFrame()->GetLayer(m_nLayerId);
	if ((int)(m_fRate * pCurLayer->GetWidth()) < 10 || (int)(m_fRate * pCurLayer->GetHeight()) < 10
		|| (int)(m_fRate * pCurLayer->GetWidth()) * (int)(m_fRate * pCurLayer->GetHeight()) * 24 > CXIMAGE_MAX_MEMORY)
		return false;
	// rewind last history move steps
	int nCurStepId = GetFrame()->GetCurrentStepId();
	double dCorrectionAngle = PI * (double)nAngle / 180.0;
	while (!::wcscmp (GetFrame()->GetStepName (nCurStepId), L"RotateAndResize") )
	{
		const IGLibrary::IGFrameActionRotateAndResize& curStep = dynamic_cast <const IGLibrary::IGFrameActionRotateAndResize&> (GetFrame()->GetAction (nCurStepId));
		if (curStep.m_ptVector.y < 0)
			dCorrectionAngle -= acos ((double)curStep.m_ptVector.x / PIXEL_SELECTOR_PRECISION);
		else
			dCorrectionAngle += acos ((double)curStep.m_ptVector.x / PIXEL_SELECTOR_PRECISION);
		m_fRate *= curStep.m_fRate;
		nCurStepId--;				
	}
	m_ptVector.x = (int)(PIXEL_SELECTOR_PRECISION * cos (dCorrectionAngle));
	m_ptVector.y = (int)(PIXEL_SELECTOR_PRECISION * sin (dCorrectionAngle));
	if (!GetFrame()->RestoreStep (nCurStepId))
		return false;
	// layer id and position of the transformation center
	m_rcLayerPosAndVector.top = m_nLayerId;
	m_rcLayerPosAndVector.bottom = (int)(m_fRate * (float)PIXEL_SELECTOR_PRECISION);
	m_rcLayerPosAndVector.left = m_ptVector.x;
	m_rcLayerPosAndVector.right = m_ptVector.y;		
	// rectangle to transform
	if (pCurLayer->SelectionIsValid()) {				
		ZeroMemory (&m_rcSubLayer, sizeof (RECT));
		pCurLayer->SelectionGetBox (m_rcSubLayer);
		float xTransform = (float)m_ptVector.x / (float)PIXEL_SELECTOR_PRECISION;
		float yTransform = (float)m_ptVector.y / (float)PIXEL_SELECTOR_PRECISION;
		float xpivot = (float)(m_rcSubLayer.left + m_rcSubLayer.right) / 2.0f;
		float ypivot = (float)(m_rcSubLayer.top + m_rcSubLayer.bottom) / 2.0f;
		POINT ptTopLeft, ptTopRight, ptBottomLeft, ptBottomRight;
		ptTopLeft.x = (int)(xpivot + (((float)(m_rcSubLayer.left - xpivot) * xTransform + (float)(m_rcSubLayer.top - ypivot) * yTransform) * m_fRate));
		ptTopLeft.y = (int)(ypivot + ((-(float)(m_rcSubLayer.left - xpivot) * yTransform + (float)(m_rcSubLayer.top - ypivot) * xTransform) * m_fRate));
		ptTopRight.x = (int)(xpivot + (((float)(m_rcSubLayer.right - xpivot) * xTransform + (float)(m_rcSubLayer.top - ypivot) * yTransform) * m_fRate));
		ptTopRight.y = (int)(ypivot + ((-(float)(m_rcSubLayer.right - xpivot) * yTransform + (float)(m_rcSubLayer.top - ypivot) * xTransform) * m_fRate));
		ptBottomLeft.x = (int)(xpivot + (((float)(m_rcSubLayer.left - xpivot) * xTransform + (float)(m_rcSubLayer.bottom - ypivot) * yTransform) * m_fRate));
		ptBottomLeft.y = (int)(ypivot + ((-(float)(m_rcSubLayer.left - xpivot) * yTransform + (float)(m_rcSubLayer.bottom - ypivot) * xTransform) * m_fRate));
		ptBottomRight.x = (int)(xpivot + (((float)(m_rcSubLayer.right - xpivot) * xTransform + (float)(m_rcSubLayer.bottom - ypivot) * yTransform) * m_fRate));
		ptBottomRight.y = (int)(ypivot + ((-(float)(m_rcSubLayer.right - xpivot) * yTransform + (float)(m_rcSubLayer.bottom - ypivot) * xTransform) * m_fRate));
		m_rcSubLayer.left = max (0, min (min (ptTopLeft.x, ptTopRight.x), min (ptBottomLeft.x, ptBottomRight.x)));
		m_rcSubLayer.right = min (pCurLayer->GetWidth() - 1, max (max (ptTopLeft.x, ptTopRight.x), max (ptBottomLeft.x, ptBottomRight.x)));
		m_rcSubLayer.top = min (pCurLayer->GetHeight() - 1, max (max (ptTopLeft.y, ptTopRight.y), max (ptBottomLeft.y, ptBottomRight.y)));
		m_rcSubLayer.bottom = max (0, min (min (ptTopLeft.y, ptTopRight.y), min (ptBottomLeft.y, ptBottomRight.y)));			
	}		
	else{
		m_rcSubLayer.left = 0; m_rcSubLayer.right = pCurLayer->GetWidth() - 1;
		m_rcSubLayer.top = pCurLayer->GetHeight() - 1; m_rcSubLayer.bottom = 0;
	}
	m_nFirstHistoryParam = (int)&m_rcLayerPosAndVector;	
	m_nSecondHistoryParam = (int)&m_rcSubLayer;
	return true;
}

bool IGRequestRotateAndResize::SimpleThreadProcess()
{
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;
	float xTransform = (float)m_ptVector.x / (float)PIXEL_SELECTOR_PRECISION;
	float yTransform = (float)m_ptVector.y / (float)PIXEL_SELECTOR_PRECISION;
	return pLayer->RotateAndResize (xTransform, yTransform, m_fRate);
}

bool IGRequestSetLayerVisible::SimpleThreadProcess()
{
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;
	pLayer->SetVisible(m_wsIsVisible == L"1");
	return true;
}

bool IGRequestSelectLayer::Initialize()
{
	if (!IGRequest::Initialize())
		return false;
	m_selParams.eSelectionType = IGSELECTION_CLEAR;
	m_nFirstHistoryParam = (int)(&m_selParams);
	m_nSecondHistoryParam = (int)(&m_lEmpty);
	return true;
}

bool IGRequestSelectLayer::SimpleThreadProcess()
{
	if (m_bFull){
		IGLibrary::SELECTIONPARAMS selParams;
		selParams.eSelectionType = IGSELECTION_CLEAR;
		list <POINT> lPts;
		GetFrame()->ManageSelection (selParams, lPts);
	}
	GetFrame()->SetWorkingLayer (m_nSelectedLayerId);
	return true;
}

bool IGRequestSelectLayer::IsFullUpdatingFrame()
{
	return m_bFull;
}

bool IGRequestSelectFrame::Initialize()
{
	if (!IGRequest::Initialize())
		return false;
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;
	int nSelectionType;
	GetParameter (IGREQUEST_SELECTIONTYPE, nSelectionType);
	GetParameter (IGREQUEST_TOLERANCE, m_selParams.nTolerance);
	m_selParams.eSelectionType = (IGSELECTIONENUM)nSelectionType;
	m_nFirstHistoryParam = (int)(&m_selParams);
	for (list<POINT>::iterator itPts = m_lPts.begin(); itPts != m_lPts.end(); ++itPts){
		long nOffsetX, nOffsetY;
		pLayer->GetOffset (&nOffsetX, &nOffsetY);
		(*itPts).x -= nOffsetX; (*itPts).y -= nOffsetY;
		if ((*itPts).x < 0)
			(*itPts).x = 0;
		if ((*itPts).x >= pLayer->GetWidth())
			(*itPts).x = pLayer->GetWidth() - 1;
		if ((*itPts).y < 0)
			(*itPts).y = 0;
		if ((*itPts).y >= pLayer->GetHeight())
			(*itPts).y = pLayer->GetHeight() - 1;
	}
	m_nSecondHistoryParam = (int)(&m_lPts);
	return true;
}

bool IGRequestSelectFrame::SimpleThreadProcess()
{
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;
	if (((m_selParams.eSelectionType & IGSELECTION_INVERT) == IGSELECTION_INVERT) &&
		!pLayer->SelectionIsValid())
		return false;	// invert selection needs a valid selection
	m_bFullUpdate = ((((m_selParams.eSelectionType & IGSELECTION_CLEAR) == IGSELECTION_CLEAR) && pLayer->SelectionIsValid())
		|| (((m_selParams.eSelectionType & IGSELECTION_CLEAR) != IGSELECTION_CLEAR) && !pLayer->SelectionIsValid()));
	return GetFrame()->ManageSelection (m_selParams, m_lPts);
}

bool IGRequestSelectFrame::IsFullUpdatingFrame()
{
	return m_bFullUpdate;
}

bool IGRequestGotoHistory::SimpleThreadProcess()
{	
	return GetFrame()->RestoreStep (m_nHistoryId);
}

bool IGRequestChangeProperty::SimpleThreadProcess()
{
	wstring wsPropIds;
	if (!GetParameter (IGREQUEST_PROPIDS, wsPropIds))
		return false;
	int nIsFrame = 0;
	if (!GetParameter (IGREQUEST_ISFRAME, nIsFrame))
		return false;
	bool bIsFrameProp = (nIsFrame == 1);
	size_t szNextComa = wstring::npos;
	wstring wsCurId;
	int nPropVal;
	int nFrameId = GetFrameId();
	do
	{
		szNextComa = wsPropIds.find (L',');
		wsCurId = wsPropIds.substr (0, szNextComa);
		CComVariant spVarVal;
		wstring wsVal;
		if (GetParameter (wsCurId.c_str(), nPropVal))
			spVarVal = nPropVal;
		else if (GetParameter (wsCurId.c_str(), wsVal))
			spVarVal = CComBSTR (wsVal.c_str());
		else
			return false;
		VARIANT varVal = spVarVal;
		if (bIsFrameProp)
		{
			if (FAILED (m_pWorkspace->SetFrameProperty (nFrameId, CComBSTR (wsCurId.c_str()), &varVal)))
				return false;
		}
		else
		{
			if (FAILED (m_pWorkspace->SetProperty (CComBSTR (wsCurId.c_str()), &varVal)))
				return false;
		}
		if (szNextComa != wstring::npos)
			wsPropIds = wsPropIds.substr (szNextComa + 1);
	} while (szNextComa != wstring::npos);
	return SUCCEEDED (bIsFrameProp ? m_pWorkspace->UpdateFrame (nFrameId) : m_pWorkspace->Update ());
}

bool IGRequestChangeProperty::Answer()
{
	int nIsFrame = 0;
	if (!GetParameter (IGREQUEST_ISFRAME, nIsFrame))
		return false;
	bool bIsFrameProp = (nIsFrame == 1);
	if (bIsFrameProp){
		IGAnswerFrameChanged answer (*this);
		return answer.Write();
	}
	else{
		IGAnswerPropertyChanged answer (*this);
		return answer.Write();
	}
}

IGRequestPickColor::IGRequestPickColor (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestDrawProcessing (pcwGuid, spRequestParams)
{
	m_eType = IGREQUEST_WORKSPACE;
	bool bInit = Initialize();		
	_ASSERTE (bInit && "IGRequestPickColor - Initialization error");
	if (bInit)
	{
		ZeroMemory (&m_rgbColor, sizeof (RGBQUAD));
		ZeroMemory (&m_ptCoords, sizeof (POINT));
		m_ptCoords = *m_lPts.begin();
	}
}

bool IGRequestPickColor::SimpleThreadProcess()
{
	bool bRes = GetFrame()->PickColor (m_ptCoords, m_rgbColor);
	if (bRes)
	{
		BYTE red = m_rgbColor.rgbRed;
		m_rgbColor.rgbRed = m_rgbColor.rgbBlue;
		m_rgbColor.rgbBlue = red;
		CComVariant spvarTextureSel (IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1, VT_I4);
		m_pWorkspace->GetProperty (CComBSTR (IGProperties::IGPROPERTY_SELECT_TEXTURE), &spvarTextureSel);
		IGTexture texture (IGColor ((COLORREF)*(int*)&m_rgbColor, (BYTE)(*(int*)&m_rgbColor >> 24)));
		VARIANT varTexture;
		varTexture.vt = VT_UNKNOWN;
		varTexture.punkVal = &texture;
		if (spvarTextureSel.intVal == IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1)
			m_pWorkspace->SetProperty (CComBSTR (IGProperties::IGPROPERTY_TEXTURE1), &varTexture);
		else
			m_pWorkspace->SetProperty (CComBSTR (IGProperties::IGPROPERTY_TEXTURE2), &varTexture);
	}
	return bRes;
}

bool IGRequestPickColor::Answer()
{
	IGAnswerPropertyChanged answer (*this);
	return answer.Write();
}

bool IGRequestDelete::SimpleThreadProcess()
{
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;	
	return pLayer->Delete();
}

bool IGRequestSmartDelete::SimpleThreadProcess()
{
	IGLayer *pLayer = GetFrame()->GetLayer(m_nLayerId);
	if (!pLayer)
		return false;	
	return pLayer->SmartDelete();
}

bool IGRequestCopy::SimpleThreadProcess()
{
	return m_pWorkspace->Copy(CComBSTR(GetFrame()->GetId())) == S_OK;
}

bool IGRequestCut::SimpleThreadProcess()
{
	return m_pWorkspace->Cut(CComBSTR(GetFrame()->GetId())) == S_OK;
}

bool IGRequestPaste::SimpleThreadProcess()
{
	return m_pWorkspace->Paste(CComBSTR(GetFrame()->GetId())) == S_OK;
}