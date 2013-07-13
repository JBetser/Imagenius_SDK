#include "StdAfx.h"
#include "IGFrame.h"
#include "IGInternalProcessing.h"

using namespace IGLibrary;

IGInternalProcessing::IGInternalProcessing(IGFrame *pFrame, IGInternalMessage *pMessage) : IGImageProcessing (pFrame, pMessage)
																										, m_pMessage (pMessage)
{
	m_pFrame->SetHistoryEnabled();
}

IGInternalProcessing::~IGInternalProcessing()
{
}

bool IGInternalProcessing::OnImageProcessing (CxImage& image, IGImageProcMessage& message)
{
	if (!m_pFrame || !m_pMessage)
		return false;
	switch (m_pMessage->GetInternalProcType())
	{
		case IGINTERNALPROC_RESAMPLE:
		{	IGFRAMEPROPERTY_STRETCHING_ENUM eStretchMode;
			if (!m_pFrame->GetProperty(IGFrameProperties::IGFRAMEPROPERTY_STRETCHING, eStretchMode))
				return false;
			int nNewWidth = m_pFrame->GetWidth();
			int nNewHeight = m_pFrame->GetHeight();
			if (!m_pFrame->GetProperty(IGFrameProperties::IGFRAMEPROPERTY_WIDTH, nNewWidth))
				return false;
			if (!m_pFrame->GetProperty(IGFrameProperties::IGFRAMEPROPERTY_HEIGHT, nNewHeight))
				return false;
			return SUCCEEDED (m_pFrame->Resample (nNewWidth, nNewHeight, eStretchMode));
		}
	}
	return false;
}

EnumStepType IGInternalProcessing::GetStepId() const
{
	return m_pMessage->GetStepId();
}

int IGInternalProcessing::GetFirstParam (IGImageProcMessage& message, CxImage& image) const
{
	if (!m_pFrame || !m_pMessage)
		return 0L;
	return m_pMessage->GetFirstParam (*m_pFrame);
}

int IGInternalProcessing::GetSecondParam (IGImageProcMessage& message, CxImage& image) const
{
	if (!m_pFrame || !m_pMessage)
		return 0L;
	return m_pMessage->GetSecondParam (*m_pFrame);
}