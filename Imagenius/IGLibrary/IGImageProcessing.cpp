#include "StdAfx.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGThread.h"
#include "IGImageProcessing.h"
#include "IGImageProcMessage.h"
#include "IGImageProcMessages.h"
#include "IGLog.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>

using namespace IGLibrary;

#define IGIP_OUTPUT_LAYERWIDTH		60
#define IGIP_OUTPUT_LAYERHEIGHT		50
#define	IGIP_TIMER_EVENT			7
#define IGIP_TIMER_INTERVAL			300	
        
IGCommunicationBase	*IGImageProcessing::g_pCurrentRequest = NULL;

IGCommunicationBase::IGCommunicationBase (const IGCommunicationBase& requestCopy)
{
	m_nId = requestCopy.m_nId;
	m_eType = requestCopy.m_eType;
	m_wsGuid = requestCopy.m_wsGuid;
	m_wsUser = requestCopy.m_wsUser;
	m_wsFrameGuid = requestCopy.m_wsFrameGuid;
	m_wsFrameName = requestCopy.m_wsFrameName;
	m_eStepId = requestCopy.m_eStepId;
	m_nFirstHistoryParam = requestCopy.m_nFirstHistoryParam;
	m_nSecondHistoryParam = requestCopy.m_nSecondHistoryParam;
}

bool IGCommunicationBase::IsWorkspace() const
{
	return (m_eType == IGREQUEST_WORKSPACE);
}

bool IGCommunicationBase::IsFrame() const
{
	return (m_eType == IGREQUEST_FRAME);
}

bool IGCommunicationBase::IsUpdatingFrame() const
{
	return true;
}

bool IGCommunicationBase::IsFullUpdatingFrame()
{
	return false;
}

bool IGCommunicationBase::IsLayer() const
{
	return false;
}

int IGCommunicationBase::GetLayerId() const
{
	return m_nLayerId;
}

int IGCommunicationBase::GetId() const
{
	return m_nId;
}

std::wstring IGCommunicationBase::GetGuid() const
{
	return m_wsGuid;
}

std::wstring IGCommunicationBase::GetUser() const
{
	return m_wsUser;
}

std::wstring IGCommunicationBase::GetFrameName() const
{
	return m_wsFrameName;
}

int IGCommunicationBase::GetFramePos()
{
	if (IsNumber (m_wsFrameGuid.c_str()))
		return _wtoi (m_wsFrameGuid.c_str());
	return -1;
}

bool IGCommunicationBase::IsNumber(LPCWSTR pcwStr)
{
	if (!pcwStr || !*pcwStr)
		return false;
	if (*pcwStr == '-'){
		if (!*++pcwStr)
			return false;
	}
	do
	{
		if ((*pcwStr < '0') || (*pcwStr > '9'))
			return false;
	}while (*++pcwStr);
	return true;
}

IGImageProcessing::IGImageProcessing (IGFrame *pFrame, const wchar_t *pcwXml, HWND hProgress)	: m_pFrame (pFrame)
																								, m_pMessage (NULL)
																								, m_nBegin (0)
																								, m_nEnd (0)
																								, m_pRequest (NULL)
																								, m_nLastProgress (-1)
{
}

IGImageProcessing::IGImageProcessing (IGFrame *pFrame, IGImageProcMessage *pMessage, HWND hProgress): m_pFrame (pFrame)
																									, m_pMessage (pMessage)
																									, m_nBegin (0)
																									, m_nEnd (0)
																									, m_pRequest (NULL)
																									, m_nLastProgress (-1)
{
	m_pMessage->m_hParent = m_pFrame->m_hOwner;
	m_pMessage->m_hProgress = hProgress ? hProgress : m_pFrame->m_spProgress ? m_pFrame->m_spProgress->GetHWND() : NULL;
	m_pMessage->m_pCallback = this;
}

IGImageProcessing::~IGImageProcessing()
{	
#ifdef IGLIBRARY_SERVER
	// release server progression subclass
	if (m_pMessage && m_pMessage->m_hProgress)
	{
		::RemoveWindowSubclass (m_pMessage->m_hProgress, StaticSubclassProgressProc, 0);
	}
#endif
}

void IGImageProcessing::SetCurrentRequest (IGCommunicationBase *pRequest) 
{
	if (g_pCurrentRequest && (g_pCurrentRequest != pRequest))
		delete g_pCurrentRequest;
	g_pCurrentRequest = pRequest;
}

void IGImageProcessing::UnregisterRequest (IGCommunicationBase *pRequest) 
{
	if (g_pCurrentRequest == pRequest)
		g_pCurrentRequest = NULL;
}

std::wstring IGImageProcessing::GetCurrentRequestGUID() 
{
	if (!g_pCurrentRequest)
		return L"##Unregistered##";
	return g_pCurrentRequest->GetGuid();
}

LPCWSTR IGImageProcessing::GetName() const
{
	_ASSERTE (m_pMessage && L"Accessing NULL message");
	if (!m_pMessage)
		return L"";
	return m_pMessage->GetImageProcessingName();
}

EnumStepType IGImageProcessing::GetStepId() const
{
	return m_pRequest ? m_pRequest->GetStepId() : IGFRAMEHISTORY_STEP_UNDEFINED;
}

void IGImageProcessing::SetProgressRange (int nBegin, int nEnd)
{
	if ((nBegin > 0) && (nEnd > nBegin) && (nEnd < 100))
	{
		m_nBegin = nBegin;
		m_nEnd = nEnd;
	}
}

bool IGImageProcessing::Start (bool bAsync)
{
	_ASSERTE (m_pMessage && L"IGImageProcessing::Start() Failed");
	if (!m_pMessage)
		return false;

	// Compute the number of steps of this image procesing
	int nNbSteps = m_pMessage->ComputeNbProgressSteps (*m_pFrame);

	// Split the progress bar if necessary
	int nBegin = 0;
	if (m_nBegin && m_nEnd)
	{
		float fRatio = 100.0f / ((float)(m_nEnd - m_nBegin));
		nNbSteps = (int)((float)nNbSteps * fRatio);
		nBegin = (int)((float)m_nBegin * fRatio);
	}

	if (m_pFrame->m_spProgress)
	{
		// Initialize and show the progress bar
		m_pFrame->m_spProgress->ProgressSetMode (SPLASHSCREEN_PROGRESSBAR_PROGRESSION);
		m_pFrame->m_spProgress->ProgressSetRange (nNbSteps);
		m_pFrame->m_spProgress->ProgressSetPos (nBegin);
		m_pFrame->m_spProgress->Show();
	}

	// transfer request from current global request repository to processing object
	if (g_pCurrentRequest)
	{
		m_pMessage->m_pRequest = g_pCurrentRequest;
		g_pCurrentRequest = NULL;		
	}

#ifdef IGLIBRARY_SERVER
	// notify server progression
	if (m_pMessage->m_hProgress)
	{
		if (::SetWindowSubclass (m_pMessage->m_hProgress, StaticSubclassProgressProc, 0, (DWORD_PTR)this))
		{
			if (m_pMessage->m_pRequest)
				m_pRequest = m_pMessage->m_pRequest->Clone(); 
		}			
	}
#endif

	// Post message to image processing thread
	if (bAsync)
		m_pFrame->PostMsg (m_pMessage);
	else
	{
		bool bRet = OnImageProcessing();
		delete m_pMessage;
		return bRet;
	}
	return true;
}

int IGImageProcessing::GetFirstParam() const
{
	if (!m_pMessage)
		return 0;
	int nFirstParam = GetFirstParam (*m_pMessage, *m_pFrame);
	if ((nFirstParam < 0) && m_pRequest)
		nFirstParam = m_pRequest->GetFirstHistoryParam();
	return nFirstParam;
}

int IGImageProcessing::GetSecondParam() const
{
	if (!m_pMessage)
		return 0;
	int nSecondParam = GetSecondParam (*m_pMessage, *m_pFrame);
	if ((nSecondParam < 0) && m_pRequest)
		nSecondParam = m_pRequest->GetSecondHistoryParam();
	return nSecondParam;
}	

bool IGImageProcessing::OnImageProcessing()
{
	_ASSERTE (m_pMessage && L"IGImageProcessing::OnImageProcessing() Failed - NULL message");
	if (!m_pMessage)	
		return false;
	_ASSERTE (m_pFrame && L"IGImageProcessing::OnImageProcessing() Failed - NULL frame");
	if (!m_pFrame)
		return false;

	// return value
	bool bRet = true;
	// update the history if history is needed
	EnumStepType eStepId = GetStepId();
	if (eStepId != IGFRAMEHISTORY_STEP_UNDEFINED){
		try{
			bRet = m_pFrame->AddNewStep (eStepId, GetName(), GetFirstParam(), GetSecondParam(), -1, false);}
		catch (const exception& exc){
			wchar_t twExc [65536];
			::MultiByteToWideChar (CP_THREAD_ACP, 0, exc.what(), -1, twExc, 65536);
			IGLog log;
			log.AddWithHeader (twExc, L"History add step error - ");
			bRet = false;
		}
	}

	// call the child specific image processing
	if (bRet)
	{
		// disable history recording
		m_pFrame->SetHistoryEnabled (false);

		try{
			// process request
			bRet = OnImageProcessing (*m_pFrame, *m_pMessage);
			// update selection box
			m_pFrame->GetWorkingLayer()->SelectionRebuildBox();
		}
		catch (const exception& exc){
			wchar_t twExc [65536];
			::MultiByteToWideChar (CP_THREAD_ACP, 0, exc.what(), -1, twExc, 65536);
			IGLog log;
			log.AddWithHeader (twExc, L"Image processing error - ");
			bRet = false;
		}
		catch (...){
			IGLog log;
			log.Add (L"Image processing error - Unknown exception");
			bRet = false;
		}

		// re-enable history recording
		m_pFrame->SetHistoryEnabled();

#ifdef IGLIBRARY_SERVER
		// for server version, generate the output frame
		if (bRet)
		{
			LPCWSTR pcwOutput = IGFrame::GetUserOutputPath();
			if (pcwOutput && m_pMessage->NeedOuputGeneration()){
				try{
					bRet = OnGenerateOutput (*m_pFrame, *m_pMessage, pcwOutput);}
				catch (const exception& exc){
					wchar_t twExc [65536];
					::MultiByteToWideChar (CP_THREAD_ACP, 0, exc.what(), -1, twExc, 65536);
					IGLog log;
					log.AddWithHeader (twExc, L"Output generation error - ");
					bRet = false;
				}
			}
		}	
#endif

		// if processing succeeded, save this new history step
		if (eStepId != IGFRAMEHISTORY_STEP_UNDEFINED)
		{
			try{
				if (bRet)
					bRet = m_pFrame->SaveStep();
			}
			catch (const exception& exc){
				wchar_t twExc [65536];
				::MultiByteToWideChar (CP_THREAD_ACP, 0, exc.what(), -1, twExc, 65536);
				IGLog log;
				log.AddWithHeader (twExc, L"History save step error - ");
				bRet = false;
			}
			// if an error occurs, try to cancel the history step
			if (!bRet)
				m_pFrame->CancelSteps(); // remove the current step
		}
	}
	// erase all request parameters
	m_pFrame->ClearRequestProperties();

	if (!bRet){
		m_pFrame->GetWorkingLayer()->SelectionRebuildBox();	// the selection may not have been rebuilt
		m_pMessage->m_bResult = false;
	}

	// hide and reset the progress bar
	if (m_pMessage->m_hProgress)
	{
		::ShowWindowAsync (m_pMessage->m_hProgress, SW_HIDE);
		m_pFrame->m_spProgress->ProgressSetRange (100);
	}
	return bRet;
}

// The generic method for image processing
bool IGImageProcessing::OnImageProcessing (CxImage& image, IGImageProcMessage& message)
{
	bool bRes = false;
	switch (message.GetImageProcessingId())
	{
	case IGREQUEST_WORKSPACE_LOADIMAGE:
		{
			IGSplashWindow::SendSetSubRange (image.GetProgressBar(), 0, 3);
			IGImageLoadMessage *pIGThreadMessage = (IGImageLoadMessage *)&message;
			if (pIGThreadMessage->m_nIdxLayer >= 0)
			{
				CxImage *pLayer = image.GetLayer (pIGThreadMessage->m_nIdxLayer);
				bRes = pLayer->Load (pIGThreadMessage->m_wsImagePath.c_str(), IGFrame::GetImageType (pIGThreadMessage->m_wsImagePath.c_str()));
				if (bRes && pIGThreadMessage->m_bAutoRotate){
					pLayer->AutoRotate();
					//m_pFrame->SetProperty (IGLibrary::IGProperties::IGPROPERTY_CURTEXTURE, pLayer->GetFac)
					image.SetSize(pLayer->GetWidth(), pLayer->GetHeight());
				}
			}
			else
			{
				bRes = image.Load (pIGThreadMessage->m_wsImagePath.c_str(), IGFrame::GetImageType (pIGThreadMessage->m_wsImagePath.c_str()));
				if (bRes && pIGThreadMessage->m_bAutoRotate)
					image.AutoRotate();
			}
		}
		break;

	case IGREQUEST_FRAME_SAVE:
		{
			IGImageSaveMessage *pIGThreadMessage = (IGImageSaveMessage *)&message;
			bool bExport = false;
			if (message.GetRequestId() == IGREQUEST_FRAME_SAVE)	// export a jpeg to output dir in case of saving request to generate mini pic
				bExport = true;
			wstring wsOutputPath;
			if (bExport)
			{
				LPCWSTR pcwOutput = IGFrame::GetUserOutputPath();
				if (!pcwOutput)
					return false;
				wsOutputPath += pcwOutput;
				wsOutputPath += L"\\";
				wsOutputPath += image.GetId();
				wsOutputPath += L"\\";
				wsOutputPath += IGIMAGEPROCESSING_LASTSAVEDPICTURE;
			}
			if (pIGThreadMessage->m_dwImageType == CXIMAGE_FORMAT_IG)
			{
				bRes = image.Save (pIGThreadMessage->m_wsImagePath.c_str(), CXIMAGE_FORMAT_IG);
				if (bExport)
					bRes &= image.Save (wsOutputPath.c_str(), CXIMAGE_FORMAT_JPG);
			}
			else
			{
				// other formats --> mix layers before saving
				int nNbLayers = image.GetNumLayers();
				if (nNbLayers <= 0)
					return false;
				IGLayer *pNewLayer = new IGLayer (NULL);
				pNewLayer->Copy (*image.GetLayer (0), true, false, true, false);
				for (int idxLayer = 1; idxLayer < nNbLayers; idxLayer++)
				{
					pNewLayer->Mix (*image.GetLayer (idxLayer), CxImage::OpAlphaBlend, true);
				}
				bRes = pNewLayer->Save (pIGThreadMessage->m_wsImagePath.c_str(), pIGThreadMessage->m_dwImageType);	
				if (bExport)
					bRes &= pNewLayer->Save (wsOutputPath.c_str(), CXIMAGE_FORMAT_JPG);
				delete pNewLayer;
			}
			IGSplashWindow::SendSetSubRange (image.GetProgressBar(), 0, 0);
		}
		break;	
	case IGREQUEST_FRAME_UPDATE:
		return true;	
	default:
		// regular "straight" thread processing
		// process directly through the request object
		IGCommunicationBase *pReq = message.GetRequest();
		if (pReq)
			bRes = pReq->SimpleThreadProcess();
		break;
	}
	return bRes;
}

bool IGImageProcessing::OnGenerateOutput (CxImage& image, IGImageProcMessage& message, LPCWSTR pcwOutput)
{
	IGCommunicationBase	*pReq = message.GetRequest();
	if (!pReq)
		return true;
	switch (pReq->GetId())
	{
	case IGREQUEST_WORKSPACE_CONNECT:	
	case IGREQUEST_WORKSPACE_LOADIMAGE:	
		IGSplashWindow::SendSetSubRange (image.GetProgressBar(), 1, 3);
	break;
	default:
		IGSplashWindow::SendSetSubRange (image.GetProgressBar(), 1, 2);
	break;
	}
	// generate the output frame path	
	bool bRet = GenerateOutput (image, *pReq, pcwOutput);

	switch (pReq->GetId())
	{
		case IGREQUEST_WORKSPACE_CONNECT:	
		case IGREQUEST_WORKSPACE_LOADIMAGE:	
			IGSplashWindow::SendSetSubRange (image.GetProgressBar(), 2, 3);
			break;
	}
	return bRet;
}

bool IGImageProcessing::GenerateOutput (CxImage& image, IGCommunicationBase& request, LPCWSTR pcwOutput)
{
	wstring wsOutputPath = pcwOutput;
	wsOutputPath += L"\\";
	wsOutputPath += image.GetId();	
	int nRes = ::SHCreateDirectory (NULL, wsOutputPath.c_str());
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return false;
	wsOutputPath += L"\\";
	// generate the output frame layers
	wstring wsLayerOutputPath;
	wchar_t wLayPos [32];
	CxImage *pFirstLayer = image.GetLayer(0);
	int nImageWidth = (image.GetWidth() > 0 ? image.GetWidth() : pFirstLayer->GetWidth());
	int nImageHeight = (image.GetHeight() > 0 ? image.GetHeight() : pFirstLayer->GetHeight());

	// compute the number of steps
	std::list<CxImage*> lLayers;	// layer merge steps
	for (int idxLayer = 0; idxLayer < image.GetNumLayers(); idxLayer++)
	{
		lLayers.push_back (image.GetLayer(idxLayer));
	}
	int nNbLayerSteps = image.GetNumLayers() * IGIP_OUTPUT_LAYERHEIGHT;	// layer saving steps
	image.ProgressSetMessage (L"Generating output frames...");	
	image.ProgressSetSubRange (0, 3);			// start 1/3 main step

	// generate the base output frame
	float fCurZoom = image.GetCurrentZoom();
	image.SetCurrentZoom(1.0f);
	HBITMAP hBmpOutput = image.MakeBitmap(NULL, nImageWidth, nImageHeight, false, true);
	IGLayer layerOutput (hBmpOutput, NULL);
	::DeleteObject (hBmpOutput);
	image.SetCurrentZoom(fCurZoom);
	layerOutput.SetProgressBar (image.GetProgressBar());

	image.ProgressSetSubRange (1, 3);			// 1/3 main step ended

	// generate the deep zoom output frame using 2 + 2*4 = 10 threads
	layerOutput.SetNbDZThreads (2, 4);
	wstring wsOutputDZPath (wsOutputPath);
	wsOutputDZPath += L"DZ";
	wsOutputDZPath += request.GetGuid();
	if (request.IsFullUpdatingFrame())
	{
		layerOutput.SetFullUpdate();
	}
	else
	{
		RECT rcUpdate = image.GetUpdateRect();
		RECT rcConvUpdate = rcUpdate;
		rcConvUpdate.top = layerOutput.GetHeight() - 1 - rcUpdate.top;
		rcConvUpdate.bottom = layerOutput.GetHeight() - 1 - rcUpdate.bottom;
		layerOutput.SetUpdateRect (rcConvUpdate);

		rcUpdate = image.GetLastUpdateRect();
		rcConvUpdate = rcUpdate;
		rcConvUpdate.top = layerOutput.GetHeight() - 1 - rcUpdate.top;
		rcConvUpdate.bottom = layerOutput.GetHeight() - 1 - rcUpdate.bottom;
		layerOutput.SetLastUpdateRect (rcConvUpdate);
	}
	image.SetLastUpdateRect (image.GetUpdateRect());
	layerOutput.SetLastOutputPath (image.GetLastOutputPath().c_str());
#ifdef _DEBUG
	wstring wsDebugOutput (wsOutputPath);
	wsDebugOutput += L"lastOutput.jpg";
	layerOutput.Save (wsDebugOutput.c_str(), CXIMAGE_FORMAT_JPG);
#endif
	if (!layerOutput.Save (wsOutputDZPath.c_str(), CXIMAGE_FORMAT_DZ))
		return false;
	image.SetLastOutputPath (wsOutputDZPath.c_str());

	image.ProgressSetSubRange (2, 3);			// 2/3 main step ended
	image.ProgressSetRange (nNbLayerSteps, true);	// lock range

	// generating layer thumbnails
	nImageWidth = IGIP_OUTPUT_LAYERWIDTH;
	nImageHeight = IGIP_OUTPUT_LAYERHEIGHT;
	for (int idxLayer = 0; idxLayer < image.GetNumLayers(); idxLayer++)
	{
		CxImage curLayer (*image.GetLayer(idxLayer), true, false, false);
		computeOutputSizeAndRatio (nImageWidth, nImageHeight, curLayer.GetWidth(), curLayer.GetHeight());
		curLayer.Resample2 (nImageWidth, nImageHeight);
		wsLayerOutputPath = wsOutputPath + L"L";		
		_itow (idxLayer, wLayPos, 10);
		wsLayerOutputPath += wLayPos;
		wsLayerOutputPath += L"-";
		wsLayerOutputPath += request.GetGuid();
		wsLayerOutputPath += L".jpg";
		if (!curLayer.Save (wsLayerOutputPath.c_str(), CXIMAGE_FORMAT_JPG))
			return false;
	}
	image.SetLastReqGuid(request.GetGuid().c_str());
	image.ProgressSetRange (100, 1);	// reset and unlock range
	return true;
}

LRESULT APIENTRY IGImageProcessing::StaticSubclassProgressProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
																		UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	IGImageProcessing* pThis = (IGImageProcessing*)dwRefData;
	if( pThis != NULL )
		return pThis->SubclassProgressProc (hwnd, msg, wParam, lParam);

	return ::DefSubclassProc (hwnd, msg, wParam, lParam);
}

// progress window subclass
LRESULT IGImageProcessing::SubclassProgressProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc (hWnd, msg, wParam, lParam);

	switch(msg)
	{
		case UM_PROGRESS_STEPIT:
			if (m_pRequest)
			{
				int *pProgress = new int[1];
				::SendMessageW (hWnd, UM_PROGRESS_GETPROGRESS, (WPARAM)pProgress, 0);
				int nProgress = *pProgress;
				delete [] pProgress;
				if ((nProgress - (int)m_nLastProgress) > 5)
				{
					int nLen = ::GetWindowTextLengthW (hWnd) + 1;
					wchar_t *pwProgress = new wchar_t[nLen];
					::GetWindowTextW (hWnd, pwProgress, nLen);
					m_pRequest->OnProgress (nProgress, pwProgress);	
					delete [] pwProgress;
					m_nLastProgress = nProgress;
				}
			}
			break;	
	}

	return lRes;
}

float IGImageProcessing::computeOutputSizeAndRatio (int& nOutputWidth, int& nOutputHeight, int nFrameWidth, int nFrameHeight)
{
	float fRatio = 1.0f;
	if ((nFrameWidth <= nOutputWidth) &&
		(nFrameHeight <= nOutputHeight))
	{
		if ((nFrameWidth > 0) &&
			(nFrameHeight > 0))
		{
			nOutputWidth = nFrameWidth;
			nOutputHeight = nFrameHeight;
		}
	}
	else
	{
		float fRatioWidth = (float)nOutputWidth / (float)nFrameWidth;
		float fRatioHeight = (float)nOutputHeight / (float)nFrameHeight;
		fRatio = min (fRatioWidth, fRatioHeight);
		nOutputWidth = (int)((float)nFrameWidth * fRatio);
		nOutputHeight = (int)((float)nFrameHeight * fRatio);
	}
	return fRatio;
}