#include "StdAfx.h"
#include "IGFrame.h"
#include "IGLayer.h"
#include "IGInternalMessage.h"

using namespace IGLibrary;

IGInternalMessage::IGInternalMessage(IGINTERNALPROC_TYPE eProcType) : IGImageProcMessage (IGREQUEST_UNDEFINED_ID)
																	, m_eProcType (eProcType)
{
}

IGInternalMessage::~IGInternalMessage()
{
}

int IGInternalMessageResize::GetFirstParam (const IGFrame& frame)
{
	if ((m_ptSize.x == -1) || (m_ptSize.y == -1))
	{
		int nNewWidth = frame.GetWidth();
		int nNewHeight = frame.GetHeight();
		m_ptSize.x = nNewWidth;
		m_ptSize.y = nNewHeight;
		if (frame.GetProperty(IGFrameProperties::IGFRAMEPROPERTY_WIDTH, nNewWidth))
		{
			if (frame.GetProperty(IGFrameProperties::IGFRAMEPROPERTY_HEIGHT, nNewHeight))
			{
				m_ptSize.x = nNewWidth;
				m_ptSize.y = nNewHeight;
			}
		}
	}
	return (int)&m_ptSize;
}

// returns 1 => Stretch
//			0 => No stretch
int IGInternalMessageResize::GetSecondParam (const IGFrame& frame)
{
	GetFirstParam(frame); // init m_ptSize
	int nStretchMode = 0;
	if (!frame.GetProperty(IGFrameProperties::IGFRAMEPROPERTY_STRETCHING, nStretchMode))
		return 0;
	IGLayer *pBackgnd = frame.GetLayer(0);
	if (!pBackgnd)
		return 0;
	if (nStretchMode == (int)IGFRAMEPROPERTY_STRETCHING_NO)
	{
		int nOldEndx = pBackgnd->GetXOffset() + pBackgnd->GetWidth();
		int nOldEndy = pBackgnd->GetYOffset() + pBackgnd->GetHeight();
		int nEndx = max (nOldEndx, m_ptSize.x);
		int nEndy = max (nOldEndy, m_ptSize.y);
		return ((nEndx != nOldEndx) || (nEndy != nOldEndy)) ? 1 : 0;
	}
	return 1;
}