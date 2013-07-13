#include "StdAfx.h"
#include "ximage.h"
#include "ximathread.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGImageProcessing.h"
#include "IGImageProcMessage.h"
#include "IGUserMessages.h"

using namespace IGLibrary;

IGImageProcMessage::IGImageProcMessage (IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcessingId)
											: m_nImageProcessingId (nImageProcessingId)
											, m_pCallback (NULL)
											, m_pRequest (NULL)
											, m_bPosted (false)
											, m_bResult (true)
											, m_nRange (100)
{
}

IGImageProcMessage::~IGImageProcMessage ()
{
	// Delete the image processing object
	if (m_pCallback)
		delete m_pCallback;
	// Delete the processed request
	if (m_pRequest)
	{
		IGImageProcessing::UnregisterRequest (m_pRequest);
		delete m_pRequest;
	}	
}

UINT IGImageProcMessage::GetMsgId () const
{
	return UM_IMAGEPROCESSING;
}

bool IGImageProcMessage::IsValid ()
{
	return InternalIsValid <IGImageProcMessage> ();
}

UINT IGImageProcMessage::ComputeNbProgressSteps (const IGFrame &frame)
{
	m_nRange = frame.GetWidth() * frame.GetHeight();
	return m_nRange;
}

UINT IGImageProcMessage::GetNbProgressSteps() const
{
	return m_nRange;
}

std::wstring IGImageProcMessage::GetGuid() const
{
	if (!m_pRequest)
		return IGImageProcessing::GetCurrentRequestGUID();
	return m_pRequest->m_wsGuid;
}

int	IGImageProcMessage::GetRequestId() const
{
	if (!m_pRequest)
		return -1;
	return m_pRequest->GetId();
}


IGImageProcessing::TYPE_IMAGE_PROC_ID IGImageProcMessage::GetImageProcessingId() const
{
	return m_nImageProcessingId;
}

LPCWSTR IGImageProcMessage::GetImageProcessingName() const
{
	return GetImageProcessingName (m_nImageProcessingId);
}

LPCWSTR IGImageProcMessage::GetImageProcessingName (IGREQUEST_ID eImageProcessingId) const
{
	if (m_pRequest)
		return m_pRequest->InternalToString();
	else
	{
		switch (eImageProcessingId)
		{
		case IGREQUEST_FRAME_MERGELAYER:
			return L"Merge layers";
		case IGREQUEST_FRAME_SAVE:
			return L"Save frame";
		case IGREQUEST_WORKSPACE_LOADIMAGE:
			return L"Load frame";
		case IGREQUEST_FRAME_FILTER:
			return L"Filter frame";
		case IGREQUEST_FRAME_SELECT:
			return L"Select region";
		}
	}
	return L"Undefined";
}

IGCommunicationBase	*IGImageProcMessage::GetRequest()
{
	return m_pRequest;
}