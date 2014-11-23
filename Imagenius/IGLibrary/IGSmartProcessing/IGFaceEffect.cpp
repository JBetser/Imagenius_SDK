#include "StdAfx.h"
#include "ximadef.h"
#include "IGFaceEffect.h"
#include "IGSmartLayer.h"
#include "IGFrame.h"
#include "IGIPFilter.h"

using namespace IGLibrary;

#define IGFACEEFFECT_NBPOX1		7
#define IGFACEEFFECT_NBPOX2_1	5
#define IGFACEEFFECT_NBPOX2_2	3
#define IGFACEEFFECT_NBPOX2_3	5
#define IGFACEEFFECT_NBBLOOD1	5
#define IGFACEEFFECT_NBBLOOD4	10
#define IGFACEEFFECT_NBTEARS3	7
#define IGFACEEFFECT_NBSTARS1	5
#define IGFACEEFFECT_NBSTARS2	10
#define IGFACEEFFECT_NBTRY		100

IGSmartPtr <IGFrame> IGFaceEffect::mg_spBlood1;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spBlood2;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spBlood3;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spBlood4;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spTears1;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spTears2;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spTears3;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spPox1;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spPox2;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spStar1;
IGSmartPtr <IGFrame> IGFaceEffect::mg_spStar2;

bool IGFaceEffect::Init (HRSRC hBlood1, HRSRC hBlood2, HRSRC hBlood3, HRSRC hBlood4,
							HRSRC hTears1, HRSRC hTears2, HRSRC hTears3,
							HRSRC hPox1, HRSRC hPox2,
							HRSRC hStar1, HRSRC hStar2)
{
	if (!hBlood1 || !hBlood2 || !hBlood3 || !hBlood4 || 
		!hTears1 || !hTears2 || !hTears3 || 
		!hPox1 || !hPox2 || !hStar1 || !hStar2)
		return false;
	mg_spBlood1 = new IGFrame (hBlood1, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spBlood2 = new IGFrame (hBlood2, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spBlood3 = new IGFrame (hBlood3, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spBlood4 = new IGFrame (hBlood4, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spTears1 = new IGFrame (hTears1, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spTears2 = new IGFrame (hTears2, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spTears3 = new IGFrame (hTears3, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spPox1 = new IGFrame (hPox1, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spPox2 = new IGFrame (hPox2, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spStar1 = new IGFrame (hStar1, CXIMAGE_FORMAT_PNG, GetInstance());
	mg_spStar2 = new IGFrame (hStar2, CXIMAGE_FORMAT_PNG, GetInstance());
	return true;
}

IGFaceEffect::IGFaceEffect (IGSmartLayer& layer, bool bSelectFace) : m_layer (layer)
																	, m_bSelectFace (bSelectFace)
{
}

IGFaceEffect::~IGFaceEffect(void)
{
}

bool IGFaceEffect::Create (IGIPFaceEffectMessage *pEffectMessage, IGSmartLayer& layer, int nDescriptorIdx, IGFaceEffect*& pEffect)
{
	if (!pEffectMessage)
		return false;
	switch (pEffectMessage->m_eFaceEffectType){
	case IGIPFACE_EFFECT_COLORIZE:
		pEffect = new IGFaceEffectColorize (layer, pEffectMessage->m_col1, (float)pEffectMessage->m_dParam1 / 100.0f, (int)pEffectMessage->m_dParam2);
		break;
	case IGIPFACE_EFFECT_MATIFY:
		pEffect = new IGFaceEffectColorize (layer, IGIPFACE_EFFECT_MATIFY_COLORCODE, (float)pEffectMessage->m_dParam1 / 100.0f, 1);
		break;
	case IGIPFACE_EFFECT_BRIGHTNESS:
		pEffect = new IGFaceEffectBrightness (layer, (int)(pEffectMessage->m_dParam1 * 2.55f), (int)pEffectMessage->m_dParam2);
		break;
	case IGIPFACE_EFFECT_SHARPEN:
		pEffect = new IGFaceEffectSharpen (layer, (float)pEffectMessage->m_dParam1 / 1000.0f, (float)pEffectMessage->m_dParam2 / 100.0f, (int)((float)pEffectMessage->m_dParam3 / 100.0f));
		break;
	case IGIPFACE_EFFECT_BLUR:
		pEffect = new IGFaceEffectBlur (layer, (float)pEffectMessage->m_dParam1 / 10.0f, (int)pEffectMessage->m_dParam2);
		break;
	case IGIPFACE_EFFECT_SATURATE:
		pEffect = new IGFaceEffectSaturate (layer, (int)pEffectMessage->m_dParam1 * 2);
		break;
	case IGIPFACE_EFFECT_REDEYE:
		pEffect = new IGFaceEffectRedEyeRemove (layer, (float)pEffectMessage->m_dParam1 / 100.0f);
		break;
	case IGIPFACE_EFFECT_EYE_COLOR:
		pEffect = new IGFaceEffectChangeEyeColor (layer, pEffectMessage->m_col1, (float)pEffectMessage->m_dParam2, (int)pEffectMessage->m_dParam1 == 1);
		break;
	case IGIPFACE_EFFECT_MORPHING:
		pEffect = new IGFaceEffectMorphing (layer, (int)pEffectMessage->m_dParam1, (float)(pEffectMessage->m_dParam2 / PIXEL_SELECTOR_PRECISION), (float)(pEffectMessage->m_dParam3 / PIXEL_SELECTOR_PRECISION));
		break;
	case IGIPFACE_EFFECT_REPAIR:
		pEffect = new IGFaceEffectRepair (layer);
		break;
	case IGIPFACE_EFFECT_POX:
		pEffect = new IGFaceEffectPox (layer, (int)pEffectMessage->m_dParam1 / 100.0f);
		break;
	case IGIPFACE_EFFECT_HOOLIGAN:
		pEffect = new IGFaceEffectHooligan (layer, (int)pEffectMessage->m_dParam1 / 100.0f);
		break;
	case IGIPFACE_EFFECT_SURPRISED:
		pEffect = new IGFaceEffectSurprised (layer, (int)pEffectMessage->m_dParam1 / 100.0f);
		break;
	case IGIPFACE_EFFECT_CARTOON:
		pEffect = new IGFaceEffectCartoon (layer);
		break;
	case IGIPFACE_EFFECT_RANDOM:
		{
			IGIPFaceEffectMessage *pEffMessCopy = new IGIPFaceEffectMessage (*pEffectMessage);
			int nRndEffectId = (rand() % 7);
			double dStrength = 0.5 + (double)(rand() % 2) * 0.25;
			switch (nRndEffectId){
			case 0:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_MORPHING;
				pEffMessCopy->m_dParam1 = IGIPMORPHING_TYPE_SMILE;
				pEffMessCopy->m_dParam2 = dStrength * PIXEL_SELECTOR_PRECISION;
				break;
			case 1:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_MORPHING;
				pEffMessCopy->m_dParam1 = IGIPMORPHING_TYPE_SAD;
				pEffMessCopy->m_dParam2 = dStrength * PIXEL_SELECTOR_PRECISION;
				break;
			case 2:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_MORPHING;
				pEffMessCopy->m_dParam1 = IGIPMORPHING_TYPE_ANGRY;
				pEffMessCopy->m_dParam2 = dStrength * PIXEL_SELECTOR_PRECISION;
				break;
			case 3:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_MORPHING;
				pEffMessCopy->m_dParam1 = IGIPMORPHING_TYPE_UGLY;
				pEffMessCopy->m_dParam2 = dStrength * PIXEL_SELECTOR_PRECISION;
				break;
			case 4:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_SURPRISED;
				pEffMessCopy->m_dParam1 = dStrength * 100.0;
				pEffMessCopy->m_dParam2 = dStrength * 100.0;
				break;
			case 5:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_HOOLIGAN;
				pEffMessCopy->m_dParam1 = dStrength * 100.0;
				pEffMessCopy->m_dParam2 = dStrength * 100.0;
				break;
			case 6:
				pEffMessCopy->m_eFaceEffectType = IGIPFACE_EFFECT_POX;
				pEffMessCopy->m_dParam1 = dStrength * 100.0;
				pEffMessCopy->m_dParam2 = dStrength * 100.0;
				break;
			}
			pEffMessCopy->m_dParam3 = pEffMessCopy->m_dParam2;
			return Create (pEffMessCopy, layer, nDescriptorIdx, pEffect);
		}
	default:
		pEffect = NULL;
		return false;
	}
	pEffect->SetFaceIdx (nDescriptorIdx);
	return true;
}

bool IGFaceEffect::Process()
{
	if (!m_bSelectFace)
		m_layer.SelectionCreate();
	return InternalProcess();
}

void IGFaceEffect::SetFaceIdx (int nDescriptorIdx) 
{ 
	m_pCurReg = m_layer.GetRegion(nDescriptorIdx); 
}

void IGFaceEffect::applyTexture (CxImage& text, int nNbOcc, float fRelativeSize, bool bNeedSel, bool bKeepSize, bool bApplyRot, bool bAvoidEyesAndMouth, bool bOnMinGradient)
{
	RECT rcSel;
	m_layer.SelectionGetBox (rcSel);
	float fSelWidth = (float)abs (rcSel.right - rcSel.left);
	float fSelHeight = (float)abs (rcSel.bottom - rcSel.top);	
	float fRatio = (fRelativeSize * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)text.GetWidth() * (float)text.GetWidth() + (float)text.GetHeight() * (float)text.GetHeight());
	text.Resample ((long)(fRatio * (float)text.GetWidth()), (long)(fRatio * (float)text.GetHeight()));
	RECT rcEyeLeft,rcEyeRight,rcMouth;
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nWidth,nHeight,nOrigX,nOrigY;
	if (bAvoidEyesAndMouth){
		m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);		
		int nInsideEyeLeftWidth = (rcEyeLeft.right - rcEyeLeft.left) / 2;
		int nInsideEyeLeftHeight = (rcEyeLeft.bottom - rcEyeLeft.top) / 2;
		int nInsideEyeRightWidth = (rcEyeRight.right - rcEyeRight.left) / 2;
		int nInsideEyeRightHeight = (rcEyeRight.bottom - rcEyeRight.top) / 2;
		rcInsideEyeLeft.left = rcEyeLeft.left + nInsideEyeLeftWidth / 4;
		rcInsideEyeLeft.right = rcEyeLeft.right - nInsideEyeLeftWidth / 4;
		rcInsideEyeLeft.top = rcEyeLeft.top + nInsideEyeLeftHeight / 4;
		rcInsideEyeLeft.bottom = rcEyeLeft.bottom - nInsideEyeLeftHeight / 4;
		rcInsideEyeRight.left = rcEyeRight.left + nInsideEyeRightWidth / 4;
		rcInsideEyeRight.right = rcEyeRight.right - nInsideEyeRightWidth / 4;
		rcInsideEyeRight.top = rcEyeRight.top + nInsideEyeRightHeight / 4;
		rcInsideEyeRight.bottom = rcEyeRight.bottom - nInsideEyeRightHeight / 4;
		m_pCurReg->GetMouth (rcMouth);
		RECT rcFace;
		rcFace.left = rcInsideEyeLeft.left;
		rcFace.right = rcInsideEyeRight.right;
		rcFace.top = rcMouth.top;
		rcFace.bottom = max (max (rcInsideEyeLeft.top, rcInsideEyeLeft.bottom), max (rcInsideEyeRight.top, rcInsideEyeRight.bottom)) + min (nInsideEyeLeftHeight, nInsideEyeRightHeight);
		nWidth = abs (rcFace.right - rcFace.left) - 2 * text.GetWidth();
		nHeight = (rcFace.bottom - rcFace.top) - 2 * text.GetHeight();
		nOrigX = rcFace.left;
		nOrigY = rcFace.top;
	}
	else{
		nWidth = (int)fSelWidth - text.GetWidth();
		nHeight = (int)fSelHeight - text.GetHeight();
		nOrigX = min (rcSel.left, rcSel.right) + text.GetWidth() / 2;
		nOrigY = min (rcSel.bottom, rcSel.top) + text.GetHeight() / 2;
	}
	
	list <pair <int, int>> lPos;
	int nNbTry = 0;
	while (nNbOcc > 0){
		if (nNbTry++ == IGFACEEFFECT_NBTRY)
			break;
		int nPosX = nOrigX + text.GetWidth() + rand() % nWidth;
		int nPosY = nOrigY + text.GetHeight() + rand() % nHeight;
		int nMidPosX = nPosX + text.GetWidth() / 2;
		int nMidPosY = nPosY - text.GetHeight() / 2;	
		if (bNeedSel) {
			if (!m_layer.SelectionIsInside (nPosX, nPosY) || !m_layer.SelectionIsInside (nPosX + text.GetWidth() - 1, nPosY) ||
				!m_layer.SelectionIsInside (nPosX, nPosY - (text.GetHeight() - 1)) || !m_layer.SelectionIsInside (nPosX + text.GetWidth() - 1, nPosY - (text.GetHeight() - 1)))
				continue;
		}
		if (bAvoidEyesAndMouth){
			if (nMidPosX > rcInsideEyeLeft.left && nMidPosX < rcInsideEyeLeft.right &&
				nMidPosY > rcInsideEyeLeft.top && nMidPosY < rcInsideEyeLeft.bottom)
				continue;
			if (nMidPosX > rcInsideEyeRight.left && nMidPosX < rcInsideEyeRight.right &&
				nMidPosY > rcInsideEyeRight.top && nMidPosY < rcInsideEyeRight.bottom)
				continue;
			if (nMidPosX > rcMouth.left && nMidPosX < rcMouth.right &&
				nMidPosY > rcMouth.top && nMidPosY < rcMouth.bottom)
				continue;
		}
		bool bTooClose = false;
		for (list <pair <int, int>>::iterator itPos = lPos.begin(); itPos != lPos.end(); ++itPos){
			if (sqrtf ((float)(nPosX - (*itPos).first) * (float)(nPosX - (*itPos).first) + 
						(float)(nPosY - (*itPos).second) * (float)(nPosY - (*itPos).second)) < 
						sqrtf ((float)text.GetWidth() * (float)text.GetWidth() + (float)text.GetHeight() * (float)text.GetHeight())){
				bTooClose = true;
				break;
			}
		}
		if (bTooClose)
			continue;
		if (bApplyRot)
			text.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, bKeepSize);
		nMidPosX = max (0, nPosX + text.GetWidth() / 2);
		nMidPosY = max (0, nPosY - text.GetHeight() / 2);
		if (bOnMinGradient){
			m_layer.GetMinGradientPos (nMidPosX, nMidPosY, (int)sqrtf ((float)(text.GetWidth() * text.GetWidth() / 4 + text.GetHeight() * text.GetHeight() / 4)), nPosX, nPosY);
			nPosX = max (0, nPosX - text.GetWidth() / 2);
			nPosY = max (0, nPosY + text.GetHeight() / 2);
		}
		text.SetOffset (nPosX, m_layer.GetHeight() - 1 - nPosY);
		m_layer.Mix (text);
		lPos.push_back (pair <int, int> (nPosX, nPosY));
		nNbOcc--;
	}
}

void IGFaceEffect::applyDoubleTexture (CxImage& text1, CxImage& text2, int nNbOcc, float fRelativeSize1, float fRelativeSize2, bool bNeedSel, bool bApplyRot, bool bOnMinGradient)
{
	RECT rcSel;
	m_layer.SelectionGetBox (rcSel);
	float fSelWidth = (float)abs (rcSel.right - rcSel.left);
	float fSelHeight = (float)abs (rcSel.bottom - rcSel.top);	
	float fRatio = (fRelativeSize1 * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)text1.GetWidth() * (float)text1.GetWidth() + (float)text1.GetHeight() * (float)text1.GetHeight());
	text1.Resample ((long)(fRatio * (float)text1.GetWidth()), (long)(fRatio * (float)text1.GetHeight()));
	fRatio = (fRelativeSize2 * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
						sqrtf ((float)text2.GetWidth() * (float)text2.GetWidth() + (float)text2.GetHeight() * (float)text2.GetHeight());
	text2.Resample ((long)(fRatio * (float)text2.GetWidth()), (long)(fRatio * (float)text2.GetHeight()));
	RECT rcEyeLeft,rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nInsideEyeLeftWidth = (rcEyeLeft.right - rcEyeLeft.left) / 2;
	int nInsideEyeLeftHeight = (rcEyeLeft.bottom - rcEyeLeft.top) / 2;
	int nInsideEyeRightWidth = (rcEyeRight.right - rcEyeRight.left) / 2;
	int nInsideEyeRightHeight = (rcEyeRight.bottom - rcEyeRight.top) / 2;
	rcInsideEyeLeft.left = rcEyeLeft.left + nInsideEyeLeftWidth / 4;
	rcInsideEyeLeft.right = rcEyeLeft.right - nInsideEyeLeftWidth / 4;
	rcInsideEyeLeft.top = rcEyeLeft.top + nInsideEyeLeftHeight / 4;
	rcInsideEyeLeft.bottom = rcEyeLeft.bottom - nInsideEyeLeftHeight / 4;
	rcInsideEyeRight.left = rcEyeRight.left + nInsideEyeRightWidth / 4;
	rcInsideEyeRight.right = rcEyeRight.right - nInsideEyeRightWidth / 4;
	rcInsideEyeRight.top = rcEyeRight.top + nInsideEyeRightHeight / 4;
	rcInsideEyeRight.bottom = rcEyeRight.bottom - nInsideEyeRightHeight / 4;
	RECT rcMouth;
	m_pCurReg->GetMouth (rcMouth);
	RECT rcFace;
	rcFace.left = rcInsideEyeLeft.left;
	rcFace.right = rcInsideEyeRight.right;
	rcFace.top = rcMouth.top;
	rcFace.bottom = max (max (rcInsideEyeLeft.top, rcInsideEyeLeft.bottom), max (rcInsideEyeRight.top, rcInsideEyeRight.bottom)) + min (nInsideEyeLeftHeight, nInsideEyeRightHeight);
	int nMaxPoxWidth = max (text1.GetWidth(), text2.GetWidth());
	int nMaxPoxHeight = max (text1.GetHeight(), text2.GetHeight());
	int nWidth = abs (rcFace.right - rcFace.left) - 2 * nMaxPoxWidth;
	int	nHeight = abs (rcFace.bottom - rcFace.top) - 2 * nMaxPoxHeight;
	list <pair <int, int>> lPos;
	int nNbTry = 0;
	while (nNbOcc > 0){
		if (nNbTry++ == IGFACEEFFECT_NBTRY)
			break;
		int nPosX = rcFace.left + text1.GetWidth() + rand() % nWidth;
		int nPosY = rcFace.top + text1.GetHeight() + rand() % nHeight;
		int nMidPosX = nPosX + text1.GetWidth() / 2;
		int nMidPosY = nPosY - text1.GetHeight() / 2;
		if (bNeedSel) {
			if (!m_layer.SelectionIsInside (nPosX, nPosY) || !m_layer.SelectionIsInside (nPosX + nMaxPoxWidth - 1, nPosY) ||
				!m_layer.SelectionIsInside (nPosX, nPosY - (nMaxPoxHeight - 1)) || !m_layer.SelectionIsInside (nPosX + nMaxPoxWidth - 1, nPosY - (nMaxPoxHeight - 1)))
				continue;
		}
		if (nMidPosX > rcInsideEyeLeft.left && nMidPosX < rcInsideEyeLeft.right &&
			nMidPosY > rcInsideEyeLeft.top && nMidPosY < rcInsideEyeLeft.bottom)
			continue;
		if (nMidPosX > rcInsideEyeRight.left && nMidPosX < rcInsideEyeRight.right &&
			nMidPosY > rcInsideEyeRight.top && nMidPosY < rcInsideEyeRight.bottom)
			continue;
		if (nMidPosX > rcMouth.left && nMidPosX < rcMouth.right &&
			nMidPosY > rcMouth.top && nMidPosY < rcMouth.bottom)
			continue;
		bool bTooClose = false;
		for (list <pair <int, int>>::iterator itPos = lPos.begin(); itPos != lPos.end(); ++itPos){
			if (sqrtf ((float)(nPosX - (*itPos).first) * (float)(nPosX - (*itPos).first) + 
						(float)(nPosY - (*itPos).second) * (float)(nPosY - (*itPos).second)) < 
						sqrtf ((float)nMaxPoxWidth * (float)nMaxPoxWidth + (float)nMaxPoxHeight * (float)nMaxPoxHeight)){
				bTooClose = true;
				break;
			}
		}
		if (bTooClose)
			continue;
		if (bApplyRot)
			text1.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
		nMidPosX = max (0, nPosX + text1.GetWidth() / 2);
		nMidPosY = max (0, nPosY - text1.GetHeight() / 2);
		if (bOnMinGradient){
			m_layer.GetMinGradientPos (nMidPosX, nMidPosY, (int)sqrtf ((float)(text1.GetWidth() * text1.GetWidth() / 4 + text1.GetHeight() * text1.GetHeight() / 4)), nPosX, nPosY);
			nPosX = max (0, nPosX - text1.GetWidth() / 2);
			nPosY = max (0, nPosY + text1.GetHeight() / 2);
		}
		text1.SetOffset (nPosX, m_layer.GetHeight() - nPosY);
		m_layer.Mix (text1);
		nPosX += abs ((long)(text1.GetWidth() - text2.GetWidth())) / 2;
		nPosY -= abs ((long)(text1.GetHeight() - text2.GetHeight())) / 2;
		if (bApplyRot)
			text2.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
		text2.SetOffset (nPosX, m_layer.GetHeight() - 1 - nPosY);
		m_layer.Mix (text2);
		lPos.push_back (pair <int, int> (nPosX, nPosY));
		nNbOcc--;
	}
}

IGFaceEffectColorize::IGFaceEffectColorize (IGSmartLayer& layer, COLORREF color, float fAlpha, int nMixVal) : IGFaceEffect (layer, true)
																								, m_color (color)
																								, m_fAlpha (fAlpha)
																								, m_bFun (nMixVal == 1)
{
	if (m_fAlpha < 0.0f)
		m_fAlpha = 1.0f;
}

bool IGFaceEffectColorize::InternalProcess()
{
	RGBQUAD rgbq;
	RECT rcReg = m_pCurReg->GetRect();
	RECT rcEyeLeft,rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	RECT rcMouth;
	m_pCurReg->GetMouth (rcMouth);
	rgbq.rgbRed = GetRValue (m_color);
	rgbq.rgbBlue = GetBValue (m_color);
	rgbq.rgbGreen = GetGValue (m_color);
	rgbq.rgbReserved = 0x00;
	m_layer.FaceColorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, m_fAlpha, rcEyeLeft, rcEyeRight, rcMouth, true, m_bFun);
	return true;
}

IGFaceEffectCartoon::IGFaceEffectCartoon (IGSmartLayer& layer) : IGFaceEffect (layer, true)
{
}

bool IGFaceEffectCartoon::InternalProcess()
{
	m_layer.Cartoon();
	return true;
}

IGFaceEffectBrightness::IGFaceEffectBrightness (IGSmartLayer& layer, int nBrightness, int nContrast) : IGFaceEffect (layer, true)
																							, m_nBrightness (nBrightness)
																							, m_nContrast (nContrast)
{
	if (m_nBrightness < 0)
		m_nBrightness = 355;
	if (m_nContrast < 0)
		m_nContrast = 100;
}

bool IGFaceEffectBrightness::InternalProcess()
{	
	return m_layer.Light (m_nBrightness - 255, m_nContrast - 100);
}

IGFaceEffectBlur::IGFaceEffectBlur (IGSmartLayer& layer, float fRadius, int nThreshold) : IGFaceEffect (layer, true)
	, m_fRadius (fRadius)
	, m_nThreshold (nThreshold)
{
	if (m_fRadius < 0.0f)
		m_fRadius = 3.0f;
	if (m_nThreshold < 0)
		m_nThreshold = 0;
}

bool IGFaceEffectBlur::InternalProcess()
{	
	return m_layer.SelectiveBlur (m_fRadius, m_nThreshold);
}

IGFaceEffectSharpen::IGFaceEffectSharpen (IGSmartLayer& layer, float fRadius, float fAmount, int nThreshold) : IGFaceEffect (layer, true)
	, m_fRadius (fRadius)
	, m_fAmount (fAmount)
	, m_nThreshold (nThreshold)
{
	if (m_fRadius < 0.0f)
		m_fRadius = 5.0f;
	if (m_fAmount < 0.0f)
		m_fAmount = 0.5;
	if (m_nThreshold < 0)
		m_nThreshold = 0;
}

bool IGFaceEffectSharpen::InternalProcess()
{	
	return m_layer.UnsharpMask (m_fRadius, m_fAmount, m_nThreshold);
}

IGFaceEffectRedEyeRemove::IGFaceEffectRedEyeRemove (IGSmartLayer& layer, float fStrength) : IGFaceEffect (layer, false)
	, m_fStrength (fStrength)
{
	if (m_fStrength < 0.0f)
		m_fStrength = 0.8f;
}

bool IGFaceEffectRedEyeRemove::InternalProcess()
{
	RECT rcEyeLeft, rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	m_layer.SelectionEyesMouth();
	m_layer.SelectionRebuildBox();
	m_layer.RedEyeRemove (rcEyeLeft, rcEyeRight, m_fStrength);
	return true;
}

IGFaceEffectChangeEyeColor::IGFaceEffectChangeEyeColor (IGSmartLayer& layer, COLORREF color, float fStrength, bool bAuto) : IGFaceEffect (layer, false)
	, m_color (color)
	, m_bAuto (bAuto)
	, m_fStrength (fStrength)
{
}

bool IGFaceEffectChangeEyeColor::InternalProcess()
{
	RGBQUAD rgbq;
	rgbq.rgbRed = GetRValue (m_color);
	rgbq.rgbBlue = GetBValue (m_color);
	rgbq.rgbGreen = GetGValue (m_color);
	rgbq.rgbReserved = 0x00;
	rgbq = m_layer.RGBtoHSL (rgbq);	
	RECT rcEyeLeft, rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	m_layer.SelectionAddRect (rcEyeLeft);
	m_layer.SelectionAddRect (rcEyeRight);
	m_layer.SelectionRebuildBox();
	m_layer.ChangeEyeColor (rcEyeLeft, rcEyeRight, rgbq.rgbRed, rgbq.rgbGreen, m_fStrength);
	return true;
}

IGFaceEffectMorphing::IGFaceEffectMorphing (IGSmartLayer& layer, int nType, float fCoeff1, float fCoeff2) : IGFaceEffect (layer, false)
	, m_nType (nType)
	, m_fCoeff1 (fCoeff1)
	, m_fCoeff2 (fCoeff2)
{
}

bool IGFaceEffectMorphing::InternalProcess()
{
	RECT rcReg = m_pCurReg->GetRect();
	RECT rcEyeLeft,rcEyeRight,rcMouth;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	m_pCurReg->GetMouth (rcMouth);
	switch ((IGIPMORPHING_TYPE)m_nType){
	case IGIPMORPHING_TYPE_CHINESE:
		m_layer.Morphing (rcEyeLeft.left + (int)(0.25f * (float)(rcEyeLeft.right - rcEyeLeft.left)), (rcEyeLeft.top + rcEyeLeft.bottom) / 2, -m_fCoeff1 * (float)(rcEyeLeft.right - rcEyeLeft.left), 0.0f, 0.0f, CXIMAGE_MORPHING_TYPE_DIRECTIONNAL, true);
		m_layer.Morphing (rcEyeRight.right - (int)(0.25f * (float)(rcEyeRight.right - rcEyeRight.left)), (rcEyeRight.top + rcEyeRight.bottom) / 2, m_fCoeff1 * (float)(rcEyeRight.right - rcEyeRight.left), 0.0f, 0.0f, CXIMAGE_MORPHING_TYPE_DIRECTIONNAL, true);
		break;
	case IGIPMORPHING_TYPE_CROSSEYED:
		crossEyed (rcEyeLeft, rcEyeRight);
		break;
	case IGIPMORPHING_TYPE_BIGNOSE:
		bigNose (rcEyeLeft, rcEyeRight, rcMouth);
		break;
	case IGIPMORPHING_TYPE_SMILE:
		smile (rcEyeLeft, rcEyeRight, rcMouth);
		break;
	case IGIPMORPHING_TYPE_SAD:
		sad (rcEyeLeft, rcEyeRight, rcMouth);	
		break;
	case IGIPMORPHING_TYPE_ANGRY:
		angry (rcEyeLeft, rcEyeRight, rcMouth);
		break;
	case IGIPMORPHING_TYPE_UGLY:
		ugly (rcEyeLeft, rcEyeRight, rcMouth);
		break;
	}	
	return true;
}

bool IGFaceEffectMorphing::ugly (const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth)
{
	m_fCoeff2 = m_fCoeff1;			
	if (m_fCoeff1 > 0.66f){
		m_fCoeff1 = 0.5f;
		m_fCoeff2 = 0.5f;
		crossEyed (rcEyeLeft, rcEyeRight);	
		smile (rcEyeLeft, rcEyeRight, rcMouth, true);			
		bigNose (rcEyeLeft, rcEyeRight, rcMouth);
	}
	else if (m_fCoeff1 > 0.33f){
		RECT rcInsideEyeLeft,rcInsideEyeRight;
		int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
		int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
		int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
		int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
		int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
		int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
		int nInsideEyeRightWidth = nEyeRightWidth / 4;
		int nInsideEyeRightHeight = nEyeRightHeight / 4;
		rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
		rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
		rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
		rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
		rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 8.0f);
		rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 8.0f);
		rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 8.0f);
		rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 8.0f);
		m_layer.Morphing ((rcEyeLeft.right + rcEyeRight.left) / 2, min (rcEyeLeft.top, rcEyeRight.top), 0.0f, - (float)max (nEyeLeftHeight, nEyeRightHeight) * m_fCoeff1 * 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		if (rand() % 2 == 0)
			m_layer.Morphing ((rcEyeLeft.left + rcEyeLeft.right) / 2, (rcEyeLeft.bottom + rcInsideEyeLeft.bottom) / 2, 0.0f, (float)(rcEyeRight.bottom - rcEyeRight.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		else
			m_layer.Morphing ((rcEyeRight.left + rcEyeRight.right) / 2, (rcEyeLeft.bottom + rcInsideEyeLeft.bottom) / 2, 0.0f, (float)(rcEyeRight.bottom - rcEyeRight.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		m_layer.Morphing ((rcMouth.left + rcMouth.right) / 2, rcMouth.bottom - (int)(1.75f * (float)(rcMouth.bottom - rcMouth.top)), - 0.75f * (float)(rcMouth.right - rcMouth.left), 0.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		m_layer.Morphing ((rcMouth.left + rcMouth.right) / 2, rcMouth.bottom - (int)(1.75f * (float)(rcMouth.bottom - rcMouth.top)), 0.75f * (float)(rcMouth.right - rcMouth.left), 0.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	}
	else {
		m_fCoeff1 = 0.5f;
		m_fCoeff2 = 0.5f;
		bigNose (rcEyeLeft, rcEyeRight, rcMouth);
	}
	return true;
}

bool IGFaceEffectMorphing::ProcessBackground()
{
	if (m_nType == IGIPMORPHING_TYPE_SMILE){
		RECT rcNo = {-1, -1, -1, -1};
		if (m_fCoeff1 > 0.55f){
			if (m_fCoeff1 > 0.66f){
				CxImage star1 (*(CxImage*)(mg_spStar1->GetLayer(0)));
				applyTexture (star1, IGFACEEFFECT_NBSTARS1, 0.15f + (float)(rand() % 5) * 0.01f, true, true, true, false);
				CxImage star2 (*(CxImage*)(mg_spStar2->GetLayer(0)));
				applyTexture (star2, IGFACEEFFECT_NBSTARS2, 0.05f + (float)(rand() % 5) * 0.01f, true, true, true, false);
			}
			m_layer.SelectionErode (20);
			m_layer.GaussianBlur (2.0f);
			m_layer.SelectionErode (10);
			m_layer.GaussianBlur (2.0f);
			RGBQUAD rgbWhite;
			rgbWhite.rgbRed = 255;
			rgbWhite.rgbBlue = 255;
			rgbWhite.rgbGreen = 255;
			if (!m_layer.ColorizeTurb (rgbWhite.rgbRed, rgbWhite.rgbGreen, rgbWhite.rgbBlue, 0.2f))
				return false;
			m_layer.SelectionErode (20);				
			if (m_fCoeff1 > 0.66f){
				m_layer.GaussianBlur (2.0f);
				RGBQUAD rgbWhite;
				rgbWhite.rgbRed = 255;
				rgbWhite.rgbBlue = 200;
				rgbWhite.rgbGreen = 175;
				if (!m_layer.ColorizeTurb (rgbWhite.rgbRed, rgbWhite.rgbGreen, rgbWhite.rgbBlue, 0.3f))
					return false;
			}
			else {
				RGBQUAD rgbWhite;
				rgbWhite.rgbRed = 255;
				rgbWhite.rgbBlue = 255;
				rgbWhite.rgbGreen = 255;
				if (!m_layer.ColorizeTurb (rgbWhite.rgbRed, rgbWhite.rgbGreen, rgbWhite.rgbBlue, 0.3f))
					return false;
			}
		}
	}
	return true;
}

bool IGFaceEffectMorphing::angry (const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth)
{
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
	int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
	int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
	int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
	int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
	int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
	int nInsideEyeRightWidth = nEyeRightWidth / 4;
	int nInsideEyeRightHeight = nEyeRightHeight / 4;
	rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	m_fCoeff2 = 0.5f + (max (min (m_fCoeff1, 1.0f), 0.0f)  / 2.0f);
	m_layer.Morphing (rcEyeLeft.right, rcEyeLeft.bottom, (rcEyeLeft.left - rcEyeLeft.right) * m_fCoeff2 / 2.0f, (rcEyeLeft.top - rcEyeLeft.bottom) * m_fCoeff2 / 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcEyeRight.left, rcEyeRight.bottom, (rcEyeRight.right - rcEyeRight.left) * m_fCoeff2 / 2.0f, (rcEyeRight.top - rcEyeRight.bottom) * m_fCoeff2 / 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	
	m_layer.Morphing (rcMouth.left, rcMouth.bottom, 0.0f, (float)(rcMouth.top - rcMouth.bottom) * 0.75f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.right, rcMouth.bottom, 0.0f, (float)(rcMouth.top - rcMouth.bottom) * 0.75f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);

	if (m_fCoeff1 > 0.33f){
		if (m_fCoeff1 > 0.66f){
			m_layer.ScaleEyeValue (-0.25f, rcEyeLeft);
			m_layer.ScaleEyeValue (-0.25f, rcEyeRight);
		}
		else{
			RECT rcMiddle;
			rcMiddle.left = rcEyeLeft.left + nEyeLeftWidth / 2;
			rcMiddle.right = rcEyeLeft.right + nEyeLeftHeight / 2;
			rcMiddle.top = rcEyeLeft.top;
			rcMiddle.bottom = rcEyeLeft.bottom;
			m_layer.ScaleEyeValue (-0.25f, rcMiddle);
		}
	}
	return true;
}

bool IGFaceEffectMorphing::sad (const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth)
{
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
	int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
	int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
	int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
	int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
	int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
	int nInsideEyeRightWidth = nEyeRightWidth / 4;
	int nInsideEyeRightHeight = nEyeRightHeight / 4;
	rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	m_layer.ScaleEyeValue (-0.25f, rcInsideEyeLeft);
	m_layer.ScaleEyeValue (-0.25f, rcInsideEyeRight);

	RECT rcFace;
	rcFace.left = rcInsideEyeLeft.left;
	rcFace.right = rcInsideEyeRight.right;
	rcFace.top = rcMouth.top;
	rcFace.bottom = max (max (rcInsideEyeLeft.top, rcInsideEyeLeft.bottom), max (rcInsideEyeRight.top, rcInsideEyeRight.bottom)) + min (nInsideEyeLeftHeight, nInsideEyeRightHeight);
	float fSelWidth = (float)abs (rcFace.right - rcFace.left);
	float fSelHeight = (float)abs (rcFace.bottom - rcFace.top);	
	RECT rcUnderEyeLeft, rcUnderEyeRight;
	rcUnderEyeLeft.left = rcEyeLeft.left;
	rcUnderEyeLeft.right = rcEyeLeft.right;
	rcUnderEyeLeft.top = rcMouth.top;
	rcUnderEyeLeft.bottom = (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2;
	rcUnderEyeRight.left = rcEyeRight.left;
	rcUnderEyeRight.right = rcEyeRight.right;
	rcUnderEyeRight.top = rcMouth.top;
	rcUnderEyeRight.bottom = (rcInsideEyeRight.top + rcInsideEyeRight.bottom) / 2;
	m_layer.SelectionAddRect (rcUnderEyeLeft);
	m_layer.SelectionAddRect (rcUnderEyeRight);
	m_layer.SelectionRebuildBox();
	if (m_fCoeff1 > 0.66f){		
		CxImage tears3 (*(CxImage*)(mg_spTears3->GetLayer(0)));
		applyTexture (tears3, IGFACEEFFECT_NBTEARS3, 0.075f);
	}

	m_layer.Morphing (rcMouth.left + (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * (float)(rcMouth.right - rcMouth.left), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.left - (int)(0.5f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * (float)(rcMouth.bottom - rcMouth.top), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.right - (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * (float)(rcMouth.right - rcMouth.left), -0.5f  * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.right + (int)(0.5f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * (float)(rcMouth.bottom - rcMouth.top), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	
	m_layer.Morphing ((rcInsideEyeLeft.left + rcInsideEyeLeft.right) / 2, rcInsideEyeLeft.bottom - nInsideEyeLeftHeight / 2, 0.0f, (rcInsideEyeLeft.bottom - rcInsideEyeLeft.top) * 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing ((rcInsideEyeRight.left + rcInsideEyeRight.right) / 2, rcInsideEyeRight.bottom - nInsideEyeRightHeight / 2, 0.0f, (rcInsideEyeRight.bottom - rcInsideEyeRight.top) * 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing ((rcMouth.left + rcMouth.right) / 2, rcMouth.top + (int)((rcMouth.top - rcMouth.bottom) * m_fCoeff1) , 0.0f, 2.0f * m_fCoeff1 * (rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	
	if (m_fCoeff1 > 0.33f){
		RECT rcUnderEyeLeft, rcUnderEyeRight;
		rcUnderEyeLeft.left = rcEyeLeft.left;
		rcUnderEyeLeft.right = rcEyeLeft.right;
		rcUnderEyeLeft.bottom = rcInsideEyeLeft.top + (int)((float)nInsideEyeLeftHeight * 2.0f / 3.0f);
		rcUnderEyeLeft.top = rcInsideEyeLeft.top - (int)((float)nInsideEyeLeftHeight / 3.0f);
		rcUnderEyeRight.left = rcEyeRight.left;
		rcUnderEyeRight.right = rcEyeRight.right;
		rcUnderEyeRight.bottom = rcInsideEyeRight.top + (int)((float)nInsideEyeRightHeight * 2.0f / 3.0f);
		rcUnderEyeRight.top = rcInsideEyeRight.top - (int)((float)nInsideEyeRightHeight / 3.0f);
		CxImage tears1 (*(CxImage*)(mg_spTears1->GetLayer(0)));		
		float fRatio = (sqrtf (1.5f * (float)nInsideEyeLeftWidth * 1.5f * (float)nInsideEyeLeftWidth + 1.5f * (float)nInsideEyeLeftHeight * 1.5f * (float)nInsideEyeLeftHeight)) / 
					sqrtf ((float)tears1.GetWidth() * (float)tears1.GetWidth() + (float)tears1.GetHeight() * (float)tears1.GetHeight());
		tears1.Resample ((long)(fRatio * (float)tears1.GetWidth()), (long)(fRatio * (float)tears1.GetHeight()));
		
		CxImage tears2 (*(CxImage*)(mg_spTears2->GetLayer(0)));
		fRatio = (0.2f * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)tears2.GetWidth() * (float)tears2.GetWidth() + (float)tears2.GetHeight() * (float)tears2.GetHeight());
		tears2.Resample ((long)(fRatio * (float)tears2.GetWidth()), (long)(fRatio * (float)tears2.GetHeight()));
		if (rand() % 2 == 0){
			tears1.SetOffset ((rcInsideEyeLeft.left + rcInsideEyeLeft.right - tears1.GetWidth()) / 2, (int)(m_layer.GetHeight() - 1 - rcUnderEyeLeft.top - (float)nInsideEyeLeftHeight / 3.0f));
			m_layer.Mix (tears1);
			if (rand() % 2 == 0){
				tears2.SetOffset (rcInsideEyeLeft.left, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
				m_layer.Mix (tears2);	
				if (m_fCoeff1 > 0.66f){
					tears2.SetOffset (rcInsideEyeRight.right, m_layer.GetHeight() - 1 - (rcInsideEyeRight.top + rcInsideEyeRight.bottom) / 2);
					m_layer.Mix (tears2);
				}
			}
			else{
				tears2.SetOffset (rcInsideEyeRight.left, m_layer.GetHeight() - 1 - (rcInsideEyeRight.top + rcInsideEyeRight.bottom) / 2);
				m_layer.Mix (tears2);	
				if (m_fCoeff1 > 0.66f){
					tears2.SetOffset (rcInsideEyeLeft.right, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
					m_layer.Mix (tears2);
				}
			}
		}
		else{
			tears1.SetOffset ((rcInsideEyeRight.left + rcInsideEyeRight.right - tears1.GetWidth()) / 2, (int)(m_layer.GetHeight() - 1 - rcUnderEyeRight.top - (float)nInsideEyeRightHeight / 3.0f));
			m_layer.Mix (tears1);
			if (rand() % 2 == 0){
				tears2.SetOffset (rcInsideEyeLeft.right, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
				m_layer.Mix (tears2);
				if (m_fCoeff1 > 0.66f){
					tears2.SetOffset (rcInsideEyeRight.left, m_layer.GetHeight() - 1 - (rcInsideEyeRight.top + rcInsideEyeRight.bottom) / 2);
					m_layer.Mix (tears2);
				}
			}
			else{
				tears2.SetOffset (rcInsideEyeRight.right, m_layer.GetHeight() - 1 - (rcInsideEyeRight.top + rcInsideEyeRight.bottom) / 2);
				m_layer.Mix (tears2);
				if (m_fCoeff1 > 0.66f){
					tears2.SetOffset (rcInsideEyeLeft.left, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
					m_layer.Mix (tears2);
				}
			}
		}
	}
	return true;
}

bool IGFaceEffectMorphing::crossEyed(const RECT& rcEyeLeft, const RECT& rcEyeRight)
{
	m_layer.Morphing ((3 * rcEyeLeft.left + rcEyeLeft.right) / 4, (rcEyeLeft.top + rcEyeLeft.bottom) / 2, (0.25f + m_fCoeff1) * (float)(rcEyeLeft.right - rcEyeLeft.left), 0.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing ((rcEyeRight.left + 3 * rcEyeRight.right) / 4, (rcEyeRight.top + rcEyeRight.bottom) / 2, -(0.25f + m_fCoeff2) * (float)(rcEyeRight.right - rcEyeRight.left), 0.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	return true;
}

bool IGFaceEffectMorphing::bigNose(const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth)
{
	m_layer.Morphing ((rcEyeLeft.right + rcEyeRight.left) / 2 - (int)(0.2f * (float)(rcEyeLeft.right - rcEyeLeft.left)), rcMouth.bottom + (rcMouth.bottom - rcMouth.top) / 2, -m_fCoeff1 * (float)(rcEyeLeft.right - rcEyeLeft.left), 0.25f * m_fCoeff1 * (float)(rcEyeLeft.right - rcEyeLeft.left), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing ((rcEyeLeft.right + rcEyeRight.left) / 2 + (int)(0.2f * (float)(rcEyeLeft.right - rcEyeLeft.left)), rcMouth.bottom + (rcMouth.bottom - rcMouth.top) / 2, m_fCoeff1 * (float)(rcEyeRight.right - rcEyeRight.left), 0.25f * m_fCoeff1 * (float)(rcEyeLeft.right - rcEyeLeft.left), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	return true;
}

bool IGFaceEffectMorphing::smile(const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth, bool bNoLove)
{			
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
	int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
	int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
	int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
	int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
	int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
	int nInsideEyeRightWidth = nEyeRightWidth / 4;
	int nInsideEyeRightHeight = nEyeRightHeight / 4;
	rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	RECT rcFace;
	rcFace.left = rcInsideEyeLeft.left;
	rcFace.right = rcInsideEyeRight.right;
	rcFace.top = rcMouth.top;
	rcFace.bottom = max (max (rcInsideEyeLeft.top, rcInsideEyeLeft.bottom), max (rcInsideEyeRight.top, rcInsideEyeRight.bottom)) + min (nInsideEyeLeftHeight, nInsideEyeRightHeight);
	float fSelWidth = (float)abs (rcFace.right - rcFace.left);
	float fSelHeight = (float)abs (rcFace.bottom - rcFace.top);	

	if (!bNoLove){
		CxImage star1 (*(CxImage*)(mg_spStar1->GetLayer(0)));
		float fRatio = (0.25f * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)star1.GetWidth() * (float)star1.GetWidth() + (float)star1.GetHeight() * (float)star1.GetHeight());
		star1.Resample ((long)(fRatio * (float)star1.GetWidth()), (long)(fRatio * (float)star1.GetHeight()));
		if (rand() % 2 == 0)
			star1.SetOffset ((rcInsideEyeLeft.left + rcInsideEyeLeft.right - star1.GetWidth()) / 2 + (rand() % 2 == 0 ? 1 : -1) * (int)(0.25f * (float)(rcInsideEyeLeft.right - rcInsideEyeLeft.left)), m_layer.GetHeight() - 1 - ((int)(rcInsideEyeLeft.top + (rcInsideEyeLeft.bottom - rcInsideEyeLeft.top + star1.GetHeight()) / 2 + (int)(0.25f * (float)(rcInsideEyeLeft.bottom - rcInsideEyeLeft.top)))));
		else
			star1.SetOffset ((rcInsideEyeRight.left + rcInsideEyeRight.right - star1.GetWidth()) / 2 + (rand() % 2 == 0 ? 1 : -1) * (int)(0.25f * (float)(rcInsideEyeLeft.right - rcInsideEyeLeft.left)), m_layer.GetHeight() - 1 - ((int)(rcInsideEyeRight.top + (rcInsideEyeRight.bottom - rcInsideEyeRight.top + star1.GetHeight()) / 2 + (int)(0.25f * (float)(rcInsideEyeLeft.bottom - rcInsideEyeLeft.top)))));
		star1.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
		if (m_fCoeff1 > 0.33)
			m_layer.Mix (star1);		
	}
	
	m_layer.SelectionAddRect (rcFace);
	m_layer.SelectionRebuildBox();
	if (m_fCoeff1 < 0.33f){
		if ((rcMouth.left + rcMouth.right) / 2 - rcEyeLeft.left < rcEyeRight.right - (rcMouth.left + rcMouth.right) / 2){
			m_layer.Morphing (rcMouth.right - (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, m_fCoeff1 * (float)(rcMouth.right - rcMouth.left), 0.5f * m_fCoeff1 * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
			m_layer.Morphing (rcMouth.right + (int)((m_fCoeff1 - 0.2f) * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * m_fCoeff1 * (float)(rcMouth.bottom - rcMouth.top), 2.0f * m_fCoeff1 * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		}
		else{
			m_layer.Morphing (rcMouth.left + (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, -m_fCoeff1 * (float)(rcMouth.right - rcMouth.left), 0.5f * m_fCoeff1 * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
			m_layer.Morphing (rcMouth.left - (int)((m_fCoeff1 - 0.2f) * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * m_fCoeff1 * (float)(rcMouth.bottom - rcMouth.top), 2.0f * m_fCoeff1 * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		}
	}
	else{
		if (!bNoLove){
			m_layer.ScaleEyeValue (0.2f, rcFace);
			if (m_fCoeff1 > 0.66){
				RGBQUAD rgbRed;
				rgbRed.rgbRed = 200;
				rgbRed.rgbGreen = 100;
				rgbRed.rgbBlue = 150;
				RECT rcUnderEyeLeft, rcUnderEyeRight;
				rcUnderEyeLeft.left = rcEyeLeft.left;
				rcUnderEyeLeft.right = rcEyeLeft.right;
				rcUnderEyeLeft.bottom = rcEyeLeft.top;
				rcUnderEyeLeft.top = rcEyeLeft.top - (int)((float)nInsideEyeLeftHeight * 1.25f);
				rcUnderEyeRight.left = rcEyeRight.left;
				rcUnderEyeRight.right = rcEyeRight.right;
				rcUnderEyeRight.bottom = rcEyeRight.top;
				rcUnderEyeRight.top = rcEyeRight.top - (int)((float)nInsideEyeRightHeight * 1.25f);
				m_layer.ScaleEyeValue (-0.5f, rcUnderEyeLeft);
				m_layer.ScaleEyeValue (-0.5f, rcUnderEyeRight);
				m_layer.AddEyeRGB (rgbRed, rcUnderEyeLeft);
				m_layer.AddEyeRGB (rgbRed, rcUnderEyeRight);
				m_layer.AddEyeSaturation (50, rcUnderEyeLeft);
				m_layer.AddEyeSaturation (50, rcUnderEyeRight);
			}
		}
		float fCoeffSmile = m_fCoeff1 - 0.33f;
		m_layer.Morphing (rcMouth.left + (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, -fCoeffSmile * (float)(rcMouth.right - rcMouth.left), 0.5f * fCoeffSmile * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		m_layer.Morphing (rcMouth.left - (int)((fCoeffSmile - 0.2f) * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * fCoeffSmile * (float)(rcMouth.bottom - rcMouth.top), 2.0f * fCoeffSmile * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		m_layer.Morphing (rcMouth.right - (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, fCoeffSmile * (float)(rcMouth.right - rcMouth.left), 0.5f * fCoeffSmile * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		m_layer.Morphing (rcMouth.right + (int)((fCoeffSmile - 0.2f) * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * fCoeffSmile * (float)(rcMouth.bottom - rcMouth.top), 2.0f * fCoeffSmile * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	}
	return true;
}

IGFaceEffectSaturate::IGFaceEffectSaturate (IGSmartLayer& layer, int nSaturation) : IGFaceEffect (layer, true)
	, m_nSaturation (nSaturation)
{
	if (m_nSaturation < 0)
		m_nSaturation = 150;
}

bool IGFaceEffectSaturate::InternalProcess()
{	
	return m_layer.Saturate (m_nSaturation - 100);
}

IGFaceEffectRepair::IGFaceEffectRepair (IGSmartLayer& layer) : IGFaceEffect (layer, true)
{
}

bool IGFaceEffectRepair::InternalProcess()
{
	// skin selection
	m_layer.SelectionEyesMouth();
	m_layer.SelectionRebuildBox();
	return m_layer.Repair (0.25f, 3, 0);
}

IGFaceEffectPox::IGFaceEffectPox (IGSmartLayer& layer, float fStrength) : IGFaceEffect (layer, true),
																			m_fStrength (fStrength)
{	
}

bool IGFaceEffectPox::InternalProcess()
{
	RGBQUAD rgbq;
	rgbq.rgbReserved = 0x00;
	RECT rcNo = {-1, -1, -1, -1};
	RECT rcEyeLeft,rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	RECT rcMouth;
	m_pCurReg->GetMouth (rcMouth);		
	if (m_fStrength < 0.33f){
		rgbq.rgbRed = 130;
		rgbq.rgbBlue = 100;
		rgbq.rgbGreen = 130;		
		if (!m_layer.FaceColorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 0.5f, rcEyeLeft, rcEyeRight, rcMouth))
			return false;	
		RECT rcInsideEyeLeft,rcInsideEyeRight;
		int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
		int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
		int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
		int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
		int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
		int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
		int nInsideEyeRightWidth = nEyeRightWidth / 4;
		int nInsideEyeRightHeight = nEyeRightHeight / 4;
		rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 10.0f);
		rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 10.0f);
		rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 10.0f);
		rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 10.0f);
		rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 10.0f);
		rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 10.0f);
		rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 10.0f);
		rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 10.0f);
		rgbq.rgbRed = 255;
		rgbq.rgbBlue = 0;
		rgbq.rgbGreen = 0;	
		RGBQUAD colHSL = CxImage::RGBtoHSL (rgbq);			
		m_layer.SetEyeHueSat (colHSL.rgbRed, colHSL.rgbGreen, rcEyeLeft, 180, false);
		m_layer.SetEyeHueSat (colHSL.rgbRed, colHSL.rgbGreen, rcEyeRight, 180, false);	
		m_layer.SetEyeHueSat (colHSL.rgbRed, colHSL.rgbGreen, rcInsideEyeLeft, 0, true);
		m_layer.SetEyeHueSat (colHSL.rgbRed, colHSL.rgbGreen, rcInsideEyeRight, 0, true);		
	}
	else if (m_fStrength <= 0.66f){
		if (!m_layer.Construct8())
			return false;
		if (!m_layer.ComputeGradient())
			return false;	
		rgbq.rgbRed = 200;
		rgbq.rgbBlue = 50;
		rgbq.rgbGreen = 50;		
		if (!m_layer.FaceColorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 0.25f, rcEyeLeft, rcEyeRight, rcMouth))
			return false;
		CxImage pox1 (*(CxImage*)(mg_spPox1->GetLayer(0)));
		applyTexture (pox1, IGFACEEFFECT_NBPOX1, 0.05f, true, true, true, true, true);
	}
	else {
		if (!m_layer.Construct8())
			return false;
		if (!m_layer.ComputeGradient())
			return false;	
		rgbq.rgbRed = 100;
		rgbq.rgbBlue = 100;
		rgbq.rgbGreen = 130;
		if (!m_layer.FaceColorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 0.25f, rcEyeLeft, rcEyeRight, rcMouth))
			return false;
		rgbq.rgbRed = 200;
		rgbq.rgbBlue = 50;
		rgbq.rgbGreen = 50;	
		if (!m_layer.FaceColorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 0.5f, rcEyeLeft, rcEyeRight, rcMouth))
			return false;
		CxImage pox1 (*(CxImage*)(mg_spPox1->GetLayer(0)));
		applyTexture (pox1, IGFACEEFFECT_NBPOX2_1, 0.05f, true, true, true, true, true);

		CxImage pox2 (*(CxImage*)(mg_spPox2->GetLayer(0)));
		applyTexture (pox2, IGFACEEFFECT_NBPOX2_2, 0.025f, true, true, true, true, true);

		applyDoubleTexture (pox1, pox2, IGFACEEFFECT_NBPOX2_3, 0.05f, 0.025f, true, true, true);
	}
	m_layer.SelectionClear();
	m_layer.Morphing (rcMouth.left + (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * (float)(rcMouth.right - rcMouth.left), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.left - (int)(0.5f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * (float)(rcMouth.bottom - rcMouth.top), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.right - (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * (float)(rcMouth.right - rcMouth.left), -0.5f  * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.right + (int)(0.5f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * (float)(rcMouth.bottom - rcMouth.top), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	return true;
}

IGFaceEffectHooligan::IGFaceEffectHooligan (IGSmartLayer& layer, float fStrength) : IGFaceEffect (layer, true),
																			m_fStrength (fStrength)
{	
}

bool IGFaceEffectHooligan::InternalProcess()
{
	RECT rcEyeLeft,rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	RECT rcMouth;
	m_pCurReg->GetMouth (rcMouth);	
	m_layer.Morphing (rcEyeLeft.right, rcEyeLeft.bottom, (rcEyeLeft.left - rcEyeLeft.right) * (0.5f + (float)(rand() % 50) / 100.0f) / 2.0f, (rcEyeLeft.top - rcEyeLeft.bottom) * (0.5f + (float)(rand() % 50) / 100.0f) / 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcEyeRight.left, rcEyeRight.bottom, (rcEyeRight.right - rcEyeRight.left) * (0.5f + (float)(rand() % 50) / 100.0f) / 2.0f, (rcEyeRight.top - rcEyeRight.bottom) *(0.5f + (float)(rand() % 50) / 100.0f) / 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.left, rcMouth.bottom, 0.0f, (float)(rcMouth.top - rcMouth.bottom) * (0.5f + (float)(rand() % 50) / 100.0f), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing (rcMouth.right, rcMouth.bottom, 0.0f, (float)(rcMouth.top - rcMouth.bottom) * (0.5f + (float)(rand() % 50) / 100.0f), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.SelectionRebuildBox();	

	if (m_fStrength > 0.66f){
		CxImage blood4 (*(CxImage*)(mg_spBlood4->GetLayer(0)));
		applyTexture (blood4, IGFACEEFFECT_NBBLOOD4, 0.03f, false);
	}

	CxImage blood1 (*(CxImage*)(mg_spBlood1->GetLayer(0)));
	CxImage blood2 (*(CxImage*)(mg_spBlood2->GetLayer(0)));
	CxImage blood3 (*(CxImage*)(mg_spBlood3->GetLayer(0)));
	RECT rcSel;
	m_layer.SelectionGetBox (rcSel);
	float fSelWidth = (float)abs (rcSel.right - rcSel.left);
	float fSelHeight = (float)abs (rcSel.bottom - rcSel.top);	
	float fRatio = (0.1f * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)blood1.GetWidth() * (float)blood1.GetWidth() + (float)blood1.GetHeight() * (float)blood1.GetHeight());
	blood1.Resample ((long)(fRatio * (float)blood1.GetWidth()), (long)(fRatio * (float)blood1.GetHeight()));	
	fRatio = (0.1f * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)blood2.GetWidth() * (float)blood2.GetWidth() + (float)blood2.GetHeight() * (float)blood2.GetHeight());
	blood2.Resample ((long)(fRatio * (float)blood2.GetWidth()), (long)(fRatio * (float)blood2.GetHeight()));	
	fRatio = (0.1f * sqrtf (fSelWidth * fSelWidth + fSelHeight * fSelHeight)) / 
					sqrtf ((float)blood3.GetWidth() * (float)blood3.GetWidth() + (float)blood3.GetHeight() * (float)blood3.GetHeight());
	blood3.Resample ((long)(fRatio * (float)blood3.GetWidth()), (long)(fRatio * (float)blood3.GetHeight()));	
		
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
	int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
	int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
	int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
	int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
	int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
	int nInsideEyeRightWidth = nEyeRightWidth / 4;
	int nInsideEyeRightHeight = nEyeRightHeight / 4;
	rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	RGBQUAD col;
	col.rgbRed = 0; col.rgbGreen = 0; col.rgbBlue = 25; col.rgbReserved = 0;
	if (rand() % 2 == 0){
		RECT rcUnderEyeLeft;
		rcUnderEyeLeft.left = rcEyeLeft.left;
		rcUnderEyeLeft.right = rcEyeLeft.right;
		rcUnderEyeLeft.bottom = rcInsideEyeLeft.top + (int)((float)nInsideEyeLeftHeight);
		rcUnderEyeLeft.top = rcInsideEyeLeft.top - (int)((float)nInsideEyeLeftHeight);
		m_layer.ScaleEyeValue (-0.4f, rcUnderEyeLeft, true);			
		CxImage blood3_2 (blood3);
		blood3_2.Resample ((long)(0.5f * (float)blood3.GetWidth()), (long)(0.5f * (float)blood3.GetHeight()));
		blood3_2.SetOffset ((rcInsideEyeLeft.left + rcInsideEyeLeft.right - blood3_2.GetWidth()) / 2, m_layer.GetHeight() - 1 - rcUnderEyeLeft.top);
		blood3_2.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
		m_layer.Mix (blood3_2);
		rcUnderEyeLeft.top += nInsideEyeLeftHeight;
		rcUnderEyeLeft.bottom += nInsideEyeLeftHeight;
		m_layer.AddEyeRGB (col, rcUnderEyeLeft, true);
		if (m_fStrength > 0.33f){
			RECT rcUnderEyeRight, rcOverEyeRight;
			rcUnderEyeRight.left = rcEyeRight.left;
			rcUnderEyeRight.right = rcEyeRight.right;
			rcUnderEyeRight.bottom = rcInsideEyeRight.top + (int)((float)nInsideEyeRightHeight * 1.5f);
			rcUnderEyeRight.top = rcInsideEyeRight.top - (int)((float)nInsideEyeRightHeight * 1.5f);
			rcOverEyeRight.left = rcEyeRight.left;
			rcOverEyeRight.right = rcEyeRight.right;
			rcOverEyeRight.bottom = rcInsideEyeRight.bottom + (int)((float)nInsideEyeRightHeight * 3.0f);
			rcOverEyeRight.top = rcInsideEyeRight.bottom + (int)((float)nInsideEyeRightHeight * 0.5f);
			m_layer.ScaleEyeValue (-0.4f, rcUnderEyeRight, true);
			m_layer.ScaleEyeValue (-0.4f, rcOverEyeRight, true);
			if (m_fStrength > 0.66f){
				blood3.SetOffset ((rcInsideEyeRight.left + rcInsideEyeRight.right - blood3.GetWidth()) / 2, m_layer.GetHeight() - 1 - rcUnderEyeRight.top);
				blood3.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
				m_layer.Mix (blood3);	
				rcUnderEyeRight.left = rcEyeRight.left;
				rcUnderEyeRight.right = rcEyeRight.right;
				rcUnderEyeRight.top -= nInsideEyeRightHeight;
				rcUnderEyeRight.bottom -= nInsideEyeRightHeight;	
				rcOverEyeRight.left = rcEyeRight.left;
				rcOverEyeRight.right = rcEyeRight.right;
				rcOverEyeRight.top -= nInsideEyeRightHeight;
				rcOverEyeRight.bottom -= nInsideEyeRightHeight;	
				m_layer.AddEyeRGB (col, rcUnderEyeRight, true);
				m_layer.AddEyeRGB (col, rcOverEyeRight, true);
				col.rgbRed = 25;
				col.rgbBlue = 75;
				rcUnderEyeRight.left += nInsideEyeRightWidth / 2;
				rcUnderEyeRight.right -= nInsideEyeRightWidth / 2;
				rcUnderEyeRight.top += nInsideEyeRightHeight;
				rcUnderEyeRight.bottom -= nInsideEyeRightHeight;
				rcOverEyeRight.left += nInsideEyeRightWidth / 2;
				rcOverEyeRight.right -= nInsideEyeRightWidth / 2;
				rcOverEyeRight.top += (int)((float)nInsideEyeRightHeight * 0.75f);
				rcOverEyeRight.bottom -= (int)((float)nInsideEyeRightHeight * 0.75f);
				m_layer.ScaleEyeValue (-0.4f, rcUnderEyeRight, true);
				m_layer.AddEyeSaturation (-50, rcUnderEyeRight, true);
				m_layer.AddEyeRGB (col, rcUnderEyeRight, true);
				m_layer.ScaleEyeValue (-0.4f, rcOverEyeRight, true);
				m_layer.AddEyeSaturation (-50, rcOverEyeRight, true);
				m_layer.AddEyeRGB (col, rcOverEyeRight, true);
			}
		}
	}
	else{
		RECT rcUnderEyeRight;
		rcUnderEyeRight.left = rcEyeRight.left;
		rcUnderEyeRight.right = rcEyeRight.right;
		rcUnderEyeRight.bottom = rcInsideEyeRight.top + (int)((float)nInsideEyeRightHeight);
		rcUnderEyeRight.top = rcInsideEyeRight.top - (int)((float)nInsideEyeRightHeight);
		m_layer.ScaleEyeValue (-0.4f, rcUnderEyeRight, true);		
		CxImage blood3_2 (blood3);
		blood3_2.Resample ((long)(0.5f * (float)blood3.GetWidth()), (long)(0.5f * (float)blood3.GetHeight()));
		blood3_2.SetOffset ((rcInsideEyeRight.left + rcInsideEyeRight.right - blood3_2.GetWidth()) / 2, m_layer.GetHeight() - 1 - rcUnderEyeRight.top);
		blood3_2.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
		m_layer.Mix (blood3_2);	
		rcUnderEyeRight.top += nInsideEyeRightHeight;
		rcUnderEyeRight.bottom += nInsideEyeRightHeight;
		m_layer.AddEyeRGB (col, rcUnderEyeRight, true);
		if (m_fStrength > 0.33f){
			RECT rcUnderEyeLeft, rcOverEyeLeft;
			rcUnderEyeLeft.left = rcEyeLeft.left;
			rcUnderEyeLeft.right = rcEyeLeft.right;
			rcUnderEyeLeft.bottom = rcInsideEyeLeft.top + (int)((float)nInsideEyeLeftHeight * 1.5f);
			rcUnderEyeLeft.top = rcInsideEyeLeft.top - (int)((float)nInsideEyeLeftHeight * 1.5f);
			rcOverEyeLeft.left = rcEyeLeft.left;
			rcOverEyeLeft.right = rcEyeLeft.right;
			rcOverEyeLeft.bottom = rcInsideEyeLeft.bottom + (int)((float)nInsideEyeLeftHeight * 3.0f);
			rcOverEyeLeft.top = rcInsideEyeLeft.bottom + (int)((float)nInsideEyeLeftHeight * 0.5f);
			m_layer.ScaleEyeValue (-0.4f, rcUnderEyeLeft, true);
			m_layer.ScaleEyeValue (-0.4f, rcOverEyeLeft, true);	
			if (m_fStrength > 0.66f){
				blood3.SetOffset ((rcInsideEyeLeft.left + rcInsideEyeLeft.right - blood3.GetWidth()) / 2, m_layer.GetHeight() - 1 - rcUnderEyeLeft.top);
				blood3.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
				m_layer.Mix (blood3);
				rcUnderEyeLeft.left = rcEyeLeft.left;
				rcUnderEyeLeft.right = rcEyeLeft.right;
				rcUnderEyeLeft.top -= nInsideEyeLeftHeight;
				rcUnderEyeLeft.bottom -= nInsideEyeLeftHeight;
				rcOverEyeLeft.left = rcEyeLeft.left;
				rcOverEyeLeft.right = rcEyeLeft.right;
				rcOverEyeLeft.top -= nInsideEyeLeftHeight;
				rcOverEyeLeft.bottom -= nInsideEyeLeftHeight;
				m_layer.AddEyeRGB (col, rcUnderEyeLeft, true);
				m_layer.AddEyeRGB (col, rcOverEyeLeft, true);
				col.rgbRed = 25;
				col.rgbBlue = 75;
				rcUnderEyeLeft.left += nInsideEyeLeftWidth / 2;
				rcUnderEyeLeft.right -= nInsideEyeLeftWidth / 2;
				rcUnderEyeLeft.top += nInsideEyeLeftHeight;
				rcUnderEyeLeft.bottom -= nInsideEyeLeftHeight;
				rcOverEyeLeft.left += nInsideEyeLeftWidth / 2;
				rcOverEyeLeft.right -= nInsideEyeLeftWidth / 2;
				rcOverEyeLeft.top += (int)((float)nInsideEyeLeftHeight * 0.75f);
				rcOverEyeLeft.bottom -= (int)((float)nInsideEyeLeftHeight * 0.75f);
				m_layer.ScaleEyeValue (-0.4f, rcUnderEyeLeft, true);
				m_layer.AddEyeSaturation (-50, rcUnderEyeLeft, true);
				m_layer.AddEyeRGB (col, rcUnderEyeLeft, true);
				m_layer.ScaleEyeValue (-0.4f, rcOverEyeLeft, true);
				m_layer.AddEyeSaturation (-50, rcOverEyeLeft, true);
				m_layer.AddEyeRGB (col, rcOverEyeLeft, true);
			}
		}
	}	
	m_layer.SetEyeHueSat (255, 0, rcInsideEyeLeft, 0, true);
	m_layer.SetEyeHueSat (255, 0, rcInsideEyeRight, 0, true);
	if (rand() % 2 == 0){
		blood2.SetOffset (rcEyeLeft.left, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
		m_layer.Mix (blood2);		
	}
	else{
		blood1.SetOffset (rcEyeLeft.right, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
		m_layer.Mix (blood1);
	}
	if (rand() % 2 == 0){
		blood2.SetOffset (rcEyeLeft.left, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
		m_layer.Mix (blood2);
	}
	else{
		blood1.SetOffset (rcEyeLeft.right, m_layer.GetHeight() - 1 - (rcInsideEyeLeft.top + rcInsideEyeLeft.bottom) / 2);
		m_layer.Mix (blood1);
	}
	if (rand() % 2 == 0){
		blood2.SetOffset (rcMouth.left, m_layer.GetHeight() - 1 - (rcMouth.top + rcMouth.bottom) / 2);
		m_layer.Mix (blood2);
	}
	else{
		blood1.SetOffset (rcMouth.right, m_layer.GetHeight() - 1 - (rcMouth.top + rcMouth.bottom) / 2);
		m_layer.Mix (blood1);
	}		
	if (m_fStrength > 0.66f){
		blood3.SetOffset ((rcMouth.left + rcMouth.right - blood3.GetWidth()) / 2, m_layer.GetHeight() - 1 - rcMouth.top);
		blood3.Rotate ((float)(rand() % 360), NULL, CxImage::IM_BILINEAR, CxImage::OM_BACKGROUND, NULL, true, true);
		m_layer.Mix (blood3);
	}
	return true;
}

IGFaceEffectSurprised::IGFaceEffectSurprised (IGSmartLayer& layer, float fStrength) : IGFaceEffect (layer, false),
																						m_fStrength (fStrength)
{	
}

bool IGFaceEffectSurprised::InternalProcess()
{
	RECT rcEyeLeft,rcEyeRight;
	m_pCurReg->GetEyes (rcEyeLeft, rcEyeRight);
	RECT rcMouth;
	m_pCurReg->GetMouth (rcMouth);
	RECT rcInsideEyeLeft,rcInsideEyeRight;
	int nEyeLeftWidth = rcEyeLeft.right - rcEyeLeft.left;
	int nEyeLeftHeight = rcEyeLeft.bottom - rcEyeLeft.top;
	int nEyeRightWidth = rcEyeRight.right - rcEyeRight.left;
	int nEyeRightHeight = rcEyeRight.bottom - rcEyeRight.top;
	int nInsideEyeLeftWidth = nEyeLeftWidth / 4;
	int nInsideEyeLeftHeight = nEyeLeftHeight / 4;
	int nInsideEyeRightWidth = nEyeRightWidth / 4;
	int nInsideEyeRightHeight = nEyeRightHeight / 4;
	rcInsideEyeLeft.left = rcEyeLeft.left + (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.right = rcEyeLeft.right - (int)((float)nEyeLeftWidth * 3.0f / 8.0f);
	rcInsideEyeLeft.top = rcEyeLeft.top + (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeLeft.bottom = rcEyeLeft.bottom - (int)((float)nEyeLeftHeight * 3.0f / 8.0f);
	rcInsideEyeRight.left = rcEyeRight.left + (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.right = rcEyeRight.right - (int)((float)nEyeRightWidth * 3.0f / 8.0f);
	rcInsideEyeRight.top = rcEyeRight.top + (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	rcInsideEyeRight.bottom = rcEyeRight.bottom - (int)((float)nEyeRightHeight * 3.0f / 8.0f);
	m_layer.Morphing ((rcEyeLeft.left + 3 * rcEyeLeft.right) / 4, (rcEyeLeft.bottom + rcInsideEyeLeft.bottom) / 2, 0.0f, (float)(rcEyeRight.bottom - rcEyeRight.top) * min (m_fStrength, 0.5f) * 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	m_layer.Morphing ((3 * rcEyeRight.left + rcEyeRight.right) / 4, (rcEyeRight.bottom + rcInsideEyeRight.bottom) / 2, 0.0f, (float)(rcEyeRight.bottom - rcEyeRight.top) * min (m_fStrength, 0.5f) * 2.0f, 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
	
	if (m_fStrength > 0.33f){
		RECT rcFace;
		rcFace.left = rcInsideEyeLeft.left;
		rcFace.right = rcInsideEyeRight.right;
		rcFace.top = rcMouth.top;
		rcFace.bottom = max (max (rcInsideEyeLeft.top, rcInsideEyeLeft.bottom), max (rcInsideEyeRight.top, rcInsideEyeRight.bottom)) + min (nInsideEyeLeftHeight, nInsideEyeRightHeight);
		int nWidth = rcFace.right - rcFace.left;
		int nHeight = rcFace.bottom - rcFace.top;
		CxImage tears3 (*(CxImage*)(mg_spTears3->GetLayer(0)));
		float fRatio = 0.1f * (sqrtf ((float)(nWidth * nWidth) + (float)(nHeight * nHeight))) / 
					sqrtf ((float)tears3.GetWidth() * (float)tears3.GetWidth() + (float)tears3.GetHeight() * (float)tears3.GetHeight());
		tears3.Resample ((long)(fRatio * (float)tears3.GetWidth()), (long)(fRatio * (float)tears3.GetHeight()));
		if (rand() % 2 ==0)
			tears3.SetOffset (rcInsideEyeLeft.left - (rcInsideEyeLeft.right - rcInsideEyeLeft.left) / 2, (int)(m_layer.GetHeight() - 1 - rcInsideEyeLeft.bottom - (float)nInsideEyeLeftHeight * 2.0f));
		else
			tears3.SetOffset (rcInsideEyeRight.right + (rcInsideEyeRight.right - rcInsideEyeRight.left) / 2, (int)(m_layer.GetHeight() - 1 - rcInsideEyeRight.bottom - (float)nInsideEyeRightHeight * 2.0f));
		m_layer.Mix (tears3);

		if (m_fStrength > 0.66f){
			RGBQUAD rgbq;
			rgbq.rgbRed = 255;
			rgbq.rgbBlue = 255;
			rgbq.rgbGreen = 255;		
			if (!m_layer.FaceColorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 0.5f, rcEyeLeft, rcEyeRight, rcMouth))
				return false;	
			m_layer.Morphing (rcMouth.left + (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * (float)(rcMouth.right - rcMouth.left), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
			m_layer.Morphing (rcMouth.left - (int)(0.5f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, - 0.5f * (float)(rcMouth.bottom - rcMouth.top), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
			m_layer.Morphing (rcMouth.right - (int)(0.2f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * (float)(rcMouth.right - rcMouth.left), -0.5f  * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
			m_layer.Morphing (rcMouth.right + (int)(0.5f * (float)(rcMouth.right - rcMouth.left)), (rcMouth.top + rcMouth.bottom) / 2, 0.5f * (float)(rcMouth.bottom - rcMouth.top), -0.5f * (float)(rcMouth.bottom - rcMouth.top), 0.0f, CXIMAGE_MORPHING_TYPE_WIDE, true);
		}
	}
	return true;
}