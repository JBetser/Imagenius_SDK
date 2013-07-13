#include "stdafx.h"
#include <atlbase.h>
#include "ximage.h"
#include "ximaDZthread.h"
#include "../../IGFrameHistory.h"
#include "../../IGImageProcessing.h"
#include "../../IGImageProcMessage.h"

CxImageThread::CxImageThread(void)
{
}

CxImageThread::~CxImageThread(void)
{
}

bool CxImageThread::Start()
{
	if (!vDZThreads.empty())
		return false;
	// Start all DeepZoom threads
	int nNbThreads = info.nNbDZThreads;
	_ASSERTE (((nNbThreads > 0) && (nNbThreads <= CXIMAGE_MAX_DZTHREADS)) && L"CxImageThread::Start error: invalid number of threads");
	if ((nNbThreads <= 0) || (nNbThreads > CXIMAGE_MAX_DZTHREADS))
		return false;
	while (nNbThreads--)
	{
		CxImageDZThreadSampler *pDZThread = new CxImageDZThreadSampler();
		pDZThread->SetNbDZThreads (0, info.nNbDZSubThreads);
		if (!pDZThread->Start())
			return false;
		vDZThreads.push_back (pDZThread);
	}
	return true;
}

bool CxImageThread::Exit()
{
	bool bSuccess = true;
	// Kill all DeepZoom threads
	while (!vDZThreads.empty())
	{
		CxImageThread *pDZThread = vDZThreads[vDZThreads.size()-1];
		bSuccess = bSuccess && pDZThread->Exit();
		delete pDZThread;
		vDZThreads.pop_back();
	}
	return bSuccess;
}

void CxImageThread::ThreadProc (IGLibrary::IGThreadMessage *pMessage)
{
	IGLibrary::IGImageProcMessage *pIGThreadMessage = (IGLibrary::IGImageProcMessage *)pMessage;
	if (!pIGThreadMessage || !pIGThreadMessage->m_pCallback)
	{
		_ASSERTE (0 && L"Wrong IGThread message, image processing aborted");
		return;
	}
	::CoInitializeEx (NULL, COINIT_APARTMENTTHREADED);
	IGLibrary::IGImageProcessingCallback *pImageProcessingCallback = pIGThreadMessage->m_pCallback;

	// Notify start of image processing
	bool bRes = pImageProcessingCallback->OnImageProcessing();

	// Notify end of image processing
	pImageProcessingCallback->OnFinished();

	// Process notification and kill message
	EndThreadProc (pMessage, bRes);
}

void CxImageThread::EndThreadProc (IGLibrary::IGThreadMessage *pMessage, bool bSuccess)
{
	int nThreadId = pMessage->GetParentThreadId();
	HWND hParent = pMessage->GetParentWnd();
	// message owner can be either a thread or a window
	_ASSERTE ((!!(hParent) ^ !!(nThreadId)) && L"CxImageThread::EndThreadProc Error: Cannot determine the message owner");
	// Notify end of image processing to image frame owner
	if (hParent)
	{
		// The message receiver is responsible for deleting the message object
		::PostMessageW (hParent, pMessage->GetMsgId(), (WPARAM)pMessage, bSuccess ? IGREQUEST_SUCCEEDED : IGREQUEST_FAILED);
	}
	else if (nThreadId)
	{
		// The message receiver is responsible for deleting the message object
		::PostThreadMessageW (nThreadId, pMessage->GetMsgId(), (WPARAM)pMessage, bSuccess ? IGREQUEST_SUCCEEDED : IGREQUEST_FAILED);
	}
	else
	{
		_ASSERTE (0 && L"CxImageThread::EndThreadProc Error: Request lost");
		delete pMessage;
	}
}

////////////////////////////////////////////////////////////////////////////////
/**
 * DeepZoom config - To be called before Thread::Start
	@nNbCroppingAndSavingSubThreads : nb sub threads by sampling threads
 */
void CxImageThread::SetNbDZThreads (int nNbSamplingThreads, int nNbCroppingAndSavingSubThreads)
{
	_ASSERTE ((nNbSamplingThreads > 0) && (nNbCroppingAndSavingSubThreads > 0) && (nNbSamplingThreads + nNbCroppingAndSavingSubThreads <= CXIMAGE_MAX_DZTHREADS));
	if (!((nNbSamplingThreads > 0) && (nNbCroppingAndSavingSubThreads > 0) && (nNbSamplingThreads + nNbCroppingAndSavingSubThreads <= CXIMAGE_MAX_DZTHREADS)))
		return;
	info.nNbDZThreads = nNbSamplingThreads;
	info.nNbDZSubThreads = nNbCroppingAndSavingSubThreads;
}