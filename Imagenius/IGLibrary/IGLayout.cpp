#include "StdAfx.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGFrameManager.h"
#include "IGLibPicture.h"
#include <typeinfo>

using namespace IGLibrary;

IGLayout::IGLayout ()	: m_nOrder (-1)	
						, m_nTop (0)
						, m_nLeft (0)
						, m_nWidth (0)
						, m_nHeight (0)
						, m_fTop (0.0f)
						, m_fLeft (0.0f)
						, m_fWidth (0.0f)
						, m_fHeight (0.0f)
						, m_nMultiFrameWidth (0)
						, m_nMultiFrameHeight (0)
						, m_fLayoutTop (0.0f)
						, m_fLayoutLeft (0.0f)
						, m_fLayoutWidth (0.0f)
						, m_fLayoutHeight (0.0f)
						, m_fLayoutTryTop (0.0f)
						, m_fLayoutTryLeft (0.0f)
						, m_nZoomWidth (0)
						, m_nZoomHeight (0)
						, m_nZoomPosX (0)
						, m_nZoomPosY (0)
						, m_dwRenderTickCount (0)
{
}

IGLayout::~IGLayout()
{
}

int IGLayout::GetOrder () const
{
	return m_nOrder;
}

void IGLayout::SetOrder (int nOrder)
{
	m_nOrder = nOrder;
}

float IGLayout::GetFrameLeftF() const
{
	return m_fLeft;
}

float IGLayout::GetFrameTopF() const
{
	return m_fTop;
}

float IGLayout::GetFrameWidthF() const
{
	return m_fWidth;
}

float IGLayout::GetFrameHeightF() const
{
	return m_fHeight;
}

void IGLayout::SetFrameWidthF (float fWidth)
{
	m_fWidth = fWidth;
	m_nWidth = approxFloat (m_fWidth * (float)m_nMultiFrameWidth);
}

void IGLayout::SetFrameHeightF (float fHeight)
{
	m_fHeight = fHeight;
	m_nHeight = approxFloat (m_fHeight * (float)m_nMultiFrameHeight);
}

void IGLayout::SetFrameLeftF (float fLeft)
{
	m_fLeft = fLeft;
	m_nLeft = approxFloat (m_fLeft * (float)m_nMultiFrameWidth);
}
	
void IGLayout::SetFrameTopF (float fTop)
{
	m_fTop = fTop;
	m_nTop = approxFloat (m_fTop * (float)m_nMultiFrameHeight);	
}

void IGLayout::SetMultiFrameSize (int nWidth, int nHeight)
{
	m_nMultiFrameWidth = nWidth;
	m_nMultiFrameHeight = nHeight;
	UpdateFrameSize();
}

void IGLayout::UpdateFrameSize()
{
	m_nTop = approxFloat (m_fTop * (float)m_nMultiFrameHeight) + IGFRAME_BORDERWIDTH;
	m_nLeft = approxFloat (m_fLeft * (float)m_nMultiFrameWidth) + IGFRAME_BORDERWIDTH;
	int nRight = approxFloat ((m_fLeft + m_fWidth) * (float)m_nMultiFrameWidth) - IGFRAME_BORDERWIDTH;
	int nBottom = approxFloat ((m_fTop + m_fHeight) * (float)m_nMultiFrameHeight) - IGFRAME_BORDERWIDTH;
	m_nHeight = nBottom - m_nTop;
	m_nWidth = nRight - m_nLeft;	
}

void IGLayout::ResetTry ()
{
	m_fLayoutTryTop = m_fLayoutTop;
	m_fLayoutTryLeft = m_fLayoutLeft;
}

void IGLayout::computeLayoutSize (int nWidth, int nHeight)
{
	m_fLayoutWidth = sqrtf ((float)nWidth / (float)nHeight);
	m_fLayoutHeight = sqrtf ((float)nHeight / (float)nWidth);
}

void IGLayout::SetLayout (float fLeft, float fTop, float fWidth, float fHeight, bool bTry)
{
	if (bTry)
	{
		m_fLayoutTryTop = fTop;
		m_fLayoutTryLeft = fLeft;
	}
	else
	{
		m_fLayoutTop = fTop;
		m_fLayoutLeft = fLeft;
	}
	m_fLayoutWidth = fWidth;
	m_fLayoutHeight = fHeight;
}

void IGLayout::GetLayout (float &fLeft, float &fTop, float &fWidth, float &fHeight, bool bTry) const
{
	fTop = bTry ? m_fLayoutTryTop : m_fLayoutTop;
	fLeft =	bTry ? m_fLayoutTryLeft : m_fLayoutLeft;
	fWidth = m_fLayoutWidth;
	fHeight = m_fLayoutHeight;
}

void IGLayout::GetLayoutSize (int &nWidth, int &nHeight) const
{
	nWidth = (int)(m_fLayoutWidth * (float)m_nMultiFrameWidth);
	nHeight = (int)(m_fLayoutHeight * (float)m_nMultiFrameHeight);
}

int IGLayout::approxFloat (float fValue)
{
	int nValDown = (int)fValue;
	int nValUp = nValDown + 1;
	return (fValue - (float)nValDown < (float)nValUp - fValue) ? nValDown : nValUp;
}