#include "StdAfx.h"
#include "IGBrush.h"
#include "IGProperties.h"

using namespace IGLibrary;

IGBrush::IGBrush(const IGTexture& texture, float fSize, IGBRUSH_TYPE_ENUM eType) : m_texture (texture)
																				, m_eTypeBrush (eType)
																				, m_fBrushSize (fSize)
{
}

IGBrush* IGBrush::CreateBrush (const IGHasProperties& hasProps)
{
	IGBRUSH_TYPE_ENUM eType = IGBRUSH_TYPE_ENUM_DEFAULT;
	if (!hasProps.GetProperty (IGProperties::IGPROPERTY_BRUSHTYPE, eType))
		return NULL;
	double dSize = 1.0;
	if (!hasProps.GetProperty (IGProperties::IGPROPERTY_BRUSHSIZE, dSize))
		return NULL;
	IGTexture texture;
	IGSELECTTEXTURE_TYPE_ENUM eSelText = IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1;
	if (!hasProps.GetProperty (IGProperties::IGPROPERTY_SELECT_TEXTURE, eSelText))
		return NULL;
	if (!hasProps.GetProperty (eSelText == IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1 ? IGProperties::IGPROPERTY_TEXTURE1 : IGProperties::IGPROPERTY_TEXTURE2, texture))
		return NULL;
	switch (eType)
	{
	case IGBRUSH_TYPE_ENUM_SHAPE_ROUND_SOLID:
		return new IGSolidBrushRound (IGColor(texture), (float)dSize);
	case IGBRUSH_TYPE_ENUM_SHAPE_ROUND_AA_SOLID:
		return new IGSolidBrushAARound (IGColor(texture), (float)dSize);
	}
	return NULL;
}

void IGBrush::SetColor (COLORREF col)
{
	m_texture.CopyFrom (col);
}

void IGBrush::SetTransparency (BYTE alpha)
{
	m_texture.SetTransparency (alpha);
}

void IGBrush::SetType (IGBRUSH_TYPE_ENUM eType)
{
	m_eTypeBrush = eType;
}

bool IGSolidBrushRound::Apply (float x, float y, const BYTE *pOriginalBits, BYTE *pBits, BYTE *pAlpha, BYTE *pSel, const int& nWidth, const int& nHeight, const DWORD& dwEffWidth) const
{
	int nFilterSize = (int)m_fBrushSize + 2;
	int nHalfFilterSize = nFilterSize / 2;
	double dHalfFilterSize2 = Square (m_fBrushSize / 2.0f);
	int nFromX = max (0, (int)x - nHalfFilterSize);
	int nToX = min (nWidth, nFromX + nFilterSize);
	int nFromY = max (0, (int)y - nHalfFilterSize);
	int nToY = min (nHeight, nFromY + nFilterSize);
	int nOffsetX = (int)x - nFromX;
	int nOffsetY = (int)y - nFromY;
	int nOffsetLineByte = nWidth - (nToX - nFromX);
	int nOffsetLine32 = dwEffWidth - (nToX - nFromX) * 3;
	pBits -= nOffsetY * dwEffWidth + nOffsetX * 3;
	pOriginalBits -= nOffsetY * dwEffWidth + nOffsetX * 3;
	pAlpha -= nOffsetY * nWidth + nOffsetX;
	if (pSel)
		pSel -= nOffsetY * nWidth + nOffsetX;
	RGBQUAD col = m_texture;
	bool bAlpha = m_texture.HasTransparency();
	if (bAlpha)
	{
		float fAlpha = (float)m_texture.GetTransparency() / 255.0f;
		if (fAlpha < 0.01f)
			return true;	// too transparent to display
		float f1_min_Alpha = 1.0f - fAlpha;
		for(int ny = nFromY; ny < nToY; ny++)
		{
			for(int nx = nFromX; nx < nToX; nx++)
			{
				if (!pSel || *pSel)
				{
					if (Pythagore2 ((float)nx - x, (float)ny - y) <= dHalfFilterSize2)
					{
						*pBits++ = (BYTE)((float)col.rgbBlue * fAlpha + (float)*pOriginalBits++ * f1_min_Alpha);
						*pBits++ = (BYTE)((float)col.rgbGreen * fAlpha + (float)*pOriginalBits++ * f1_min_Alpha);
						*pBits++ = (BYTE)((float)col.rgbRed * fAlpha + (float)*pOriginalBits++ * f1_min_Alpha);
						*pAlpha++ = 255;
					}
					else
					{
						pBits += 3;
						pOriginalBits += 3;
						pAlpha++;
					}
				}
				else
				{
					pBits += 3;
					pOriginalBits += 3;
					pAlpha++;
				}
				if (pSel)
					pSel++;
			}
			pBits += nOffsetLine32;
			pOriginalBits += nOffsetLine32;
			if (pSel)
				pSel += nOffsetLineByte;
			pAlpha += nOffsetLineByte;
		}
	}
	else
	{
		for(int ny = nFromY; ny < nToY; ny++)
		{
			for(int nx = nFromX; nx < nToX; nx++)
			{
				if (!pSel || *pSel)
				{
					if (Pythagore2 ((float)nx - x, (float)ny - y) <= dHalfFilterSize2)
					{
						*pBits++ = col.rgbBlue;
						*pBits++ = col.rgbGreen;
						*pBits++ = col.rgbRed;
						*pAlpha++ = 255;
					}
					else
					{
						pBits += 3;
						pAlpha++;
					}
				}
				else
				{
					pBits += 3;
					pAlpha++;
				}
				if (pSel)
					pSel++;
			}
			pBits += nOffsetLine32;
			if (pSel)
				pSel += nOffsetLineByte;
			pAlpha += nOffsetLineByte;
		}
	}
	return true;
}

bool IGSolidBrushAARound::Apply (float x, float y, const BYTE *pOriginalBits, BYTE *pBits, BYTE *pAlpha, BYTE *pSel, const int& nWidth, const int& nHeight, const DWORD& dwEffWidth) const
{
	if (m_nAAStep++ == 0)
		return Apply (x-0.5f, y, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	int nFilterSize = (int)m_fBrushSize + 2;
	int nHalfFilterSize = nFilterSize / 2;
	float fHalfFilerSize = 1.0f + m_fBrushSize / 2.0f;
	int nFromX = max (0, (int)x - nHalfFilterSize);
	int nToX = min (nWidth, nFromX + nFilterSize);
	int nFromY = max (0, (int)y - nHalfFilterSize);
	int nToY = min (nHeight, nFromY + nFilterSize);
	int nOffsetX = (int)x - nFromX;
	int nOffsetY = (int)y - nFromY;
	int nOffsetLineByte = nWidth - (nToX - nFromX);
	int nOffsetLine32 = dwEffWidth - (nToX - nFromX) * 3;
	BYTE *pCurBits = pBits - nOffsetY * dwEffWidth - nOffsetX * 3;
	const BYTE *pCurOriginalBits = pOriginalBits - nOffsetY * dwEffWidth - nOffsetX * 3;
	BYTE *pCurAlpha = pAlpha - nOffsetY * nWidth - nOffsetX;
	BYTE *pCurSel = pSel;
	if (pCurSel)
		pCurSel -= nOffsetY * nWidth + nOffsetX;
	float fQuarter = 1.0f / 4.0f;
	RGBQUAD col = m_texture;
	bool bAlpha = m_texture.HasTransparency();
	if (bAlpha)
	{
		float fAlpha = (float)m_texture.GetTransparency() / 255.0f;
		if (fAlpha < 0.01f)
			return true;	// too transparent to display
		float f1_min_Alpha = 1.0f - fAlpha;
		for(int ny = nFromY; ny < nToY; ny++)
		{
			for(int nx = nFromX; nx < nToX; nx++)
			{
				if (!pCurSel || *pCurSel)
				{
					float fDist = (float)sqrt (Pythagore2 ((float)nx - x, (float)ny - y));
					if (fDist <= fHalfFilerSize)
					{
						float fAACoeff = (fDist <= fHalfFilerSize - 1.0f) ? 1.0f : fQuarter;						
						float fPrevAACoeff = (col.rgbBlue == *pCurOriginalBits ? 1.0f : ((float)*pCurBits - (float)*pCurOriginalBits) / ((float)col.rgbBlue - (float)*pCurOriginalBits)) / fAlpha;
						float fNextAACoeff = min (1.0f, fAACoeff + fPrevAACoeff) * fAlpha;
						float f1_min_AACoeff = 1.0f - fNextAACoeff;
						*pCurBits++ = (BYTE)((float)col.rgbBlue * fNextAACoeff + (float)*pCurOriginalBits++ * f1_min_AACoeff);
						fPrevAACoeff = (col.rgbGreen == *pCurOriginalBits ? 1.0f : ((float)*pCurBits - (float)*pCurOriginalBits) / ((float)col.rgbGreen - (float)*pCurOriginalBits)) / fAlpha;
						fNextAACoeff = min (1.0f, fAACoeff + fPrevAACoeff) * fAlpha;
						f1_min_AACoeff = 1.0f - fNextAACoeff;
						*pCurBits++ = (BYTE)((float)col.rgbGreen * fNextAACoeff + (float)*pCurOriginalBits++ * f1_min_AACoeff);
						fPrevAACoeff = (col.rgbRed == *pCurOriginalBits ? 1.0f : ((float)*pCurBits - (float)*pCurOriginalBits) / ((float)col.rgbRed - (float)*pCurOriginalBits)) / fAlpha;
						fNextAACoeff = min (1.0f, fAACoeff + fPrevAACoeff) * fAlpha;
						f1_min_AACoeff = 1.0f - fNextAACoeff;
						*pCurBits++ = (BYTE)((float)col.rgbRed * fNextAACoeff + (float)*pCurOriginalBits++ * f1_min_AACoeff);
						*pCurAlpha++ = 255;
					}
					else
					{
						pCurBits += 3;
						pCurOriginalBits += 3;
						pCurAlpha++;
					}
				}
				else
				{
					pCurBits += 3;
					pCurOriginalBits += 3;
					pCurAlpha++;
				}
				if (pCurSel)
					pCurSel++;
			}
			pCurBits += nOffsetLine32;
			pCurOriginalBits += nOffsetLine32;
			if (pCurSel)
				pCurSel += nOffsetLineByte;
			pCurAlpha += nOffsetLineByte;
		}
	}
	else
	{
		for(int ny = nFromY; ny < nToY; ny++)
		{
			for(int nx = nFromX; nx < nToX; nx++)
			{
				if (!pCurSel || *pCurSel)
				{
					float fDist = (float)sqrt (Pythagore2 ((float)nx - x, (float)ny - y));
					if (fDist <= fHalfFilerSize)
					{
						float fAACoeff = (fDist <= fHalfFilerSize - 1.0f) ? 1.0f : fQuarter;						
						float fPrevAACoeff = (col.rgbBlue == *pCurOriginalBits ? 1.0f : ((float)*pCurBits - (float)*pCurOriginalBits) / ((float)col.rgbBlue - (float)*pCurOriginalBits));
						float fNextAACoeff = min (1.0f, fAACoeff + fPrevAACoeff);
						float f1_min_AACoeff = 1.0f - fNextAACoeff;
						*pCurBits++ = (BYTE)((float)col.rgbBlue * fNextAACoeff + (float)*pCurOriginalBits++ * f1_min_AACoeff);
						fPrevAACoeff = (col.rgbGreen == *pCurOriginalBits ? 1.0f : ((float)*pCurBits - (float)*pCurOriginalBits) / ((float)col.rgbGreen - (float)*pCurOriginalBits));
						fNextAACoeff = min (1.0f, fAACoeff + fPrevAACoeff);
						f1_min_AACoeff = 1.0f - fNextAACoeff;
						*pCurBits++ = (BYTE)((float)col.rgbGreen * fNextAACoeff + (float)*pCurOriginalBits++ * f1_min_AACoeff);
						fPrevAACoeff = (col.rgbRed == *pCurOriginalBits ? 1.0f : ((float)*pCurBits - (float)*pCurOriginalBits) / ((float)col.rgbRed - (float)*pCurOriginalBits));
						fNextAACoeff = min (1.0f, fAACoeff + fPrevAACoeff);
						f1_min_AACoeff = 1.0f - fNextAACoeff;
						*pCurBits++ = (BYTE)((float)col.rgbRed * fNextAACoeff + (float)*pCurOriginalBits++ * f1_min_AACoeff);
						*pCurAlpha++ = 255;
					}
					else
					{
						pCurBits += 3;
						pCurOriginalBits += 3;
						pCurAlpha++;
					}
				}
				else
				{
					pCurBits += 3;
					pCurOriginalBits += 3;
					pCurAlpha++;
				}
				if (pCurSel)
					pCurSel++;
			}
			pCurBits += nOffsetLine32;
			pCurOriginalBits += nOffsetLine32;
			if (pCurSel)
				pCurSel += nOffsetLineByte;
			pCurAlpha += nOffsetLineByte;
		}
	}
	if (m_nAAStep++ == 1)
		return Apply (x+0.5f, y-0.5f, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	if (m_nAAStep++ == 2)
		return Apply (x, y+1.0f, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	if (m_nAAStep++ == 3)
		return Apply (x+0.5f, y-0.5f, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);	
	if (m_nAAStep++ == 4)
		return Apply (x, y-0.5f, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	if (m_nAAStep++ == 5)
		return Apply (x, y+1.0f, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	if (m_nAAStep++ == 6)
		return Apply (x-1.0f, y, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	if (m_nAAStep++ == 7)
		return Apply (x, y-1.0f, pOriginalBits, pBits, pAlpha, pSel, nWidth, nHeight, dwEffWidth);
	if (m_nAAStep == 8)
		m_nAAStep = 0;
	return true;
}