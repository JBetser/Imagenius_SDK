#include "StdAfx.h"
#include <IGFrameHistory.h>
#include <IGImageProcessing.h>
#include <IGImageProcMessage.h>
#include <IGImageProcMessages.h>
#include <IGException.h>
#include <IGLog.h>
#include "IGAnswer.h"
#include "IGRequest_simple.h"

using namespace std;
using namespace IGLibrary;

IIGMainWindow									*IGRequest::g_pDispMainWindow;
IGThreadSafeData <IXMLDOMDocument*>				IGRequest::g_safepXMLDoc;
IGThreadSafeData <bool>							IGRequest::g_safebIsProcessing = false;
IGThreadSafeData <set <wstring>>				IGRequest::g_safesetRunningRequests;
HWND											IGRequest::g_hWndSocket = NULL;

IGRequest::IGRequest (LPCWSTR pcwGuid, IGREQUEST_TYPE eType, int nId) : IGCommunicationBase (pcwGuid, nId, eType)
																		, m_bEnableProgess (true)
																		, m_pWorkspace (NULL)
{
}

IGRequest::IGRequest (const IGCommunicationBase& request) : IGCommunicationBase (request)
															, m_bEnableProgess (true)
															, m_pWorkspace (NULL)
{
}

IGRequest::IGRequest (LPCWSTR pcwGuid, IGREQUEST_TYPE eType, int nId, CComPtr <IXMLDOMNode> spRequestParams)	: IGCommunicationBase (pcwGuid, nId, eType)
																												, m_spRequestParams (spRequestParams)
																												, m_bEnableProgess (true)
																												, m_pWorkspace (NULL)
{
}

IGRequest::~IGRequest(void)
{
}

void IGRequest::Release()
{
	IXMLDOMDocument*& refpDoc = g_safepXMLDoc.Lock();
	if (refpDoc){
		refpDoc->Release();
		refpDoc = NULL;
	}
	g_safepXMLDoc.UnLock();
}

bool IGRequest::CreateXMLDoc()
{
	IXMLDOMDocument *pDoc;
	HRESULT hr = ::CoCreateInstance (CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
									IID_IXMLDOMDocument, (void**)&pDoc);
	if (hr != S_OK)
	{
		g_safepXMLDoc = NULL;
		return false;
	}
	else{
		g_safepXMLDoc = pDoc;
	}
	return true;
}

void IGRequest::Flush(const wstring& wsGuid, bool bSuccess)
{
	g_safesetRunningRequests.Lock().erase(wsGuid);
	g_safesetRunningRequests.UnLock();	
}

bool IGRequest::Initialize()
{
	if (!g_pDispMainWindow)
		return false;
	g_pDispMainWindow->GetIGInterface (IID_IIGWorkspace, (OLE_HANDLE*)&m_pWorkspace);
	if (!(IXMLDOMDocument*)g_safepXMLDoc){
		if (!CreateXMLDoc())
			return false;
	}
	int nDirectionX = 0; int nDirectionY = 0;
	if (GetParameter (IGIPFILTER_PARAM_DIRECTIONX, nDirectionX) && GetParameter (IGIPFILTER_PARAM_DIRECTIONY, nDirectionY)
		&& nDirectionX == 0 && nDirectionY == 0)
		return false;		
	return true;
}

wstring IGRequest::ToString()
{
	wstring wsReq (m_wsGuid);
	wsReq += L" - ";
	wsReq += InternalToString();
	return wsReq;
}

IGRequest *IGRequest::Read (const string& sRequest)
{
	IGRequest *pReq = NULL;
	try
	{
		CComPtr <IXMLDOMNode> spXMLRequest;
		if (!getXMLRequest (sRequest, &spXMLRequest))
			return NULL;
		// read attributes
		CComPtr <IXMLDOMNamedNodeMap> spXMLRequestAttributes;
		HRESULT hr = spXMLRequest->get_attributes (&spXMLRequestAttributes);
		if ((hr != S_OK) || !spXMLRequestAttributes)
			return NULL;
		// read request type
		CComPtr <IXMLDOMNode> spXMLRequestType;
		hr = spXMLRequestAttributes->getNamedItem (CComBSTR (IGREQUEST_NODETYPE), &spXMLRequestType);
		if ((hr != S_OK) || !spXMLRequestType)
			return NULL;
		VARIANT varTypeValue;
		hr = spXMLRequestType->get_nodeValue (&varTypeValue);
		if (hr != S_OK)
			return NULL;
		int nTypeValue = ::_wtoi (varTypeValue.bstrVal);
		if ((nTypeValue != IGREQUEST_WORKSPACE) &&
			(nTypeValue != IGREQUEST_FRAME) &&
			(nTypeValue != IGREQUEST_SERVERMANAGER))
			return NULL;
		// read request id
		CComPtr <IXMLDOMNode> spXMLRequestId;
		hr = spXMLRequestAttributes->getNamedItem (CComBSTR (IGREQUEST_NODEID), &spXMLRequestId);
		if ((hr != S_OK) || !spXMLRequestId)
			return NULL;
		VARIANT varIdValue;
		hr = spXMLRequestId->get_nodeValue (&varIdValue);
		if (hr != S_OK)
			return NULL;
		int nIdValue = ::_wtoi (varIdValue.bstrVal);
		// read request guid
		CComPtr <IXMLDOMNode> spXMLRequestGuid;
		hr = spXMLRequestAttributes->getNamedItem (CComBSTR (IGREQUEST_GUID), &spXMLRequestGuid);
		if ((hr != S_OK) || !spXMLRequestGuid)
			return NULL;
		VARIANT varGuidValue;
		hr = spXMLRequestGuid->get_nodeValue (&varGuidValue);
		if (hr != S_OK)
			return NULL;
		wstring wsGuid = varGuidValue.bstrVal ? varGuidValue.bstrVal : L"##Unregistered##";
		// read request parameters
		CComPtr <IXMLDOMNode> spXMLRequestParams;
		hr = spXMLRequest->get_firstChild (&spXMLRequestParams);
		if ((hr != S_OK) || !spXMLRequestParams)
			return NULL;
		switch (nTypeValue)
		{
		case IGREQUEST_WORKSPACE:
			switch (nIdValue)
			{
			case IGREQUEST_WORKSPACE_PING:
				pReq = new IGRequestPing (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_WORKSPACE_CONNECT:
				pReq = new IGRequestConnect (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_WORKSPACE_DISCONNECT:
				pReq = new IGRequestDisconnect (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_WORKSPACE_NEWIMAGE:
				pReq = new IGRequestNew (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_WORKSPACE_LOADIMAGE:
				pReq = new IGRequestLoad (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_WORKSPACE_TERMINATE:
				pReq = new IGRequestTerminate (wsGuid.c_str());
				break;
			case IGREQUEST_WORKSPACE_SHOW:
				pReq = new IGRequestShow (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_WORKSPACE_DESTROYACCOUNT:
				pReq = new IGRequestDestroyAccount (wsGuid.c_str());
				break;			
			}
			break;

		case IGREQUEST_FRAME:
			switch (nIdValue)
			{
			case IGREQUEST_FRAME_SAVE:
			case IGREQUEST_FRAME_SAVE_BM:
				pReq = new IGRequestSave (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_ADDLAYER:
				pReq = new IGRequestAddLayer (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_REMOVELAYER:
				pReq = new IGRequestRemoveLayer (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_MOVELAYER:
				pReq = new IGRequestMoveLayer (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_MERGELAYER:
				pReq = new IGRequestMergeLayer (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_FILTER:
				pReq = new IGRequestFilter (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_INDEX:
				pReq = new IGRequestIndex (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_CHANGEPROPERTY:
				pReq = new IGRequestChangeProperty (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_CLOSE:
				pReq = new IGRequestCloseFrame (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_UPDATE:
				pReq = new IGRequestUpdateFrame (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_SELECT:
				pReq = new IGRequestSelectFrame (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_SETLAYERVISIBLE:
				pReq = new IGRequestSetLayerVisible (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_GOTOHISTORY:
				pReq = new IGRequestGotoHistory (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_DRAWLINES:
				pReq = new IGRequestDrawLines (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_PICKCOLOR:
				pReq = new IGRequestPickColor (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_COPY:
				pReq = new IGRequestCopy (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_CUT:
				pReq = new IGRequestCut (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_PASTE:
				pReq = new IGRequestPaste (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_DELETE:
				pReq = new IGRequestDelete (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_SMARTDELETE:
				pReq = new IGRequestSmartDelete (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_MOVE:
				pReq = new IGRequestMove (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_ROTATEANDRESIZE:
				pReq = new IGRequestRotateAndResize (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_SELECTLAYER:
				pReq = new IGRequestSelectLayer (wsGuid.c_str(), spXMLRequestParams);
				break;
			case IGREQUEST_FRAME_RUBBER:
				pReq = new IGRequestRubber (wsGuid.c_str(), spXMLRequestParams);
				break;
			}
			break;
		}
		if (!pReq){
			IGLog log;
			log.Add (L"IGRequest::Read Failed - Unknown request");
			return NULL;
		}
		if (!pReq->Initialize()){
			wstring wsLog (L"IGRequest::Read Failed - Error during initialization of request: ");
			wsLog += pReq->ToString();
			IGLog log;
			log.Add (wsLog.c_str());	
			delete pReq;
			return NULL;
		}
		IGImageProcessing::SetCurrentRequest (pReq);
		return pReq;
	}
	catch (IGException& e)
	{
		IGLog log;
		wstring excStr (L"IGException caught reading IGRequest: ");
		excStr += pReq->ToString();
		excStr += L" - ";
		size_t szBuf = ::strlen(e.what()) + 1;
		wchar_t *pwConv = new wchar_t[szBuf]; size_t szNbChConv = 0;
		::mbstowcs_s (&szNbChConv, pwConv, szBuf, e.what(), szBuf);
		excStr += pwConv;
		delete [] pwConv;
		log.Add (excStr.c_str());
	}
	catch (exception& e)
	{
		IGLog log;
		wstring excStr (L"exception caught reading IGRequest: ");
		excStr += pReq->ToString();
		excStr += L" - ";
		size_t szBuf = ::strlen(e.what()) + 1;
		wchar_t *pwConv = new wchar_t[szBuf]; size_t szNbChConv = 0;
		::mbstowcs_s (&szNbChConv, pwConv, szBuf, e.what(), szBuf);
		excStr += pwConv;
		delete [] pwConv;
		log.Add (excStr.c_str());
	}
	catch (...)
	{
		IGLog log;
		wstring excStr (L"Unknown exception caught reading IGRequest: ");
		excStr += pReq->ToString();
		log.Add (excStr.c_str());
	}
	return NULL;
}

bool IGRequest::Answer()
{
	IGAnswerActionDone answer (*this);
	return answer.Write();
}

bool IGRequest::Process()
{
	IIGMainWindow *pMainWindow = GetMainWindow();
	wstring wsLog (L"Processing ");
	wsLog += ToString();
	wstring wsBeforeLog (wsLog);
	wsBeforeLog += L".";
	IGLog log;
	log.Add (wsBeforeLog.c_str());	
	bool bRes = true;
	// user connection and server requests do not need user logon to be performed
	if ((m_eType != IGREQUEST_SERVERMANAGER) && 		
		(m_nId != IGREQUEST_WORKSPACE_CONNECT) &&
		(m_nId != IGREQUEST_WORKSPACE_DISCONNECT))
	{
		if (!m_spConfigManager->IsUserLogged())
			bRes = false;		
	}
	// update user login for the answer notification before processing request
	m_wsUser = m_spConfigManager->GetUserLogin (true);
	IGFrame *pFrame = GetFrame();
	if (IsFrame() && !pFrame)
		bRes = false;
	if (bRes)
	{
		if (IsFrame() && IsLayer())
			pFrame->SetWorkingLayer (GetLayerId());
		if (bRes)
			bRes = InternalProcess();
		// update user login in case of connection
		if (m_nId == IGREQUEST_WORKSPACE_CONNECT)
			m_wsUser = m_spConfigManager->GetUserLogin (true);
	}
	// in case of synchronous request, send the answer
	bool bAsync = IsAsync();	
	if (bRes && !bAsync){
		if (IsFrame() && m_nId != IGREQUEST_FRAME_CLOSE)	// update the frame properties
			pFrame->Update();
		bRes = IGAnswer::NotifySuccess (this);
	}
	// in case of error send failure notification
	if (!bRes) 
		IGAnswer::NotifyFailure (this);
	wsLog += (bRes ? (bAsync ? L" launched..." : L" succeeded.") : L" failed.");
	log.Add (wsLog.c_str());
	return bRes;
}

LRESULT IGRequest::OnProcessEnded (WPARAM wParam, LPARAM lParam)
{
	bool bRes = (lParam == IGREQUEST_SUCCEEDED);
	IIGMainWindow *pMainWindow = GetMainWindow();	
	IGImageProcMessage *pMessage = (IGImageProcMessage *)wParam;
	wstring wsLog (L"Processing ");
	bool bAnswer = true;
	int nImageProcId = -1;
	LRESULT lPostProcessingRes = IGREQUEST_SUCCEEDED;
	try
	{
		if (pMessage)
		{
			if ((nImageProcId = pMessage->GetRequestId()) >= 0)
			{
				wsLog += pMessage->GetGuid() + L" - Request Id: ";
				wchar_t twReqId[32];
				::_itow_s (nImageProcId, twReqId, 10);
				wsLog += twReqId; wsLog += L" - ";
				wsLog += pMessage->GetImageProcessingName();

				// check if current request needs to perform additional operations
				lPostProcessingRes = pMessage->GetRequest()->PostProcessing (pMessage);
				if (lPostProcessingRes == IGREQUEST_TOBEPOSTED)
					return IGREQUEST_TOBEPOSTED;
				else if (lPostProcessingRes == IGREQUEST_FAILED)
					bRes = false;
				else if (lPostProcessingRes == IGREQUEST_PROCESSED)
					bRes = true;
			}
		}
		else
		{
			wsLog = L"An unidentified corrupted image processing has";
			bAnswer = false;
		}
		wsLog += bRes ? L" succeeded." : L" failed.";
		IGLog log;
		log.Add (wsLog.c_str());
		if ((lPostProcessingRes == IGREQUEST_PROCESSED) ||
			(lPostProcessingRes == IGREQUEST_FAILED))
		{
			// last step ended successfully
			wsLog = L"Processing ";
			wsLog += pMessage->GetGuid() + L" - ";
			wsLog += (lPostProcessingRes == IGREQUEST_PROCESSED) ? L"Process succeeded." : L"Process failed.";
			log.Add (wsLog.c_str());
		}
		if (bAnswer && (lPostProcessingRes != IGREQUEST_PROCESSING))
		{
			// send success
			if (bRes){
				IGCommunicationBase *pRequest = pMessage->GetRequest();				
				if (pRequest->IsFrame() && nImageProcId != IGREQUEST_FRAME_CLOSE)	// update the frame properties
					pRequest->GetFrame()->Update();
				bRes = IGAnswer::NotifySuccess (pMessage);
			}
			else // send failure
				IGAnswer::NotifyFailure (pMessage->GetRequest());
		}
	}
	catch (IGException& e)
	{
		IGLog log;
		wstring excStr (L"IGException caught ending process of IGRequest with info: ");
		excStr += wsLog;
		excStr += L" - ";
		size_t szBuf = ::strlen(e.what()) + 1;
		wchar_t *pwConv = new wchar_t[szBuf]; size_t szNbChConv = 0;
		::mbstowcs_s (&szNbChConv, pwConv, szBuf, e.what(), szBuf);
		excStr += pwConv;
		delete [] pwConv;
		log.Add (excStr.c_str());
		bRes = false;
	}
	catch (exception& e)
	{
		IGLog log;
		wstring excStr (L"exception caught ending process of IGRequest with info: ");
		excStr += wsLog;
		excStr += L" - ";
		size_t szBuf = ::strlen(e.what()) + 1;
		wchar_t *pwConv = new wchar_t[szBuf]; size_t szNbChConv = 0;
		::mbstowcs_s (&szNbChConv, pwConv, szBuf, e.what(), szBuf);
		excStr += pwConv;
		delete [] pwConv;
		log.Add (excStr.c_str());
		bRes = false;
	}
	catch (...)
	{
		IGLog log;
		wstring excStr (L"Unknown exception caught ending process of IGRequest with info: ");
		excStr += wsLog;
		log.Add (excStr.c_str());
		bRes = false;
	}
	if (!bRes && (lPostProcessingRes != IGREQUEST_PROCESSING))
		lPostProcessingRes = IGREQUEST_FAILED;
	if ((lPostProcessingRes == IGREQUEST_PROCESSED) ||
		(lPostProcessingRes == IGREQUEST_FAILED))
		delete pMessage;
	return lPostProcessingRes;
}

bool IGRequest::ProcessRequests (list<string>& lsRequests)
{
	if (lsRequests.empty())
		return true;
	bool& bIsProcessing = g_safebIsProcessing.Lock();
	if (bIsProcessing){
		g_safebIsProcessing.UnLock();
		return false;
	}
	bIsProcessing = true;
	HRESULT hr = S_OK;
	IGRequest *pCurRequest = NULL;
	list<wstring> lnBusyFrames;
	int nIndex = 0;
	for (list<string>::const_iterator iterRequests = lsRequests.begin(); 
		iterRequests != lsRequests.end();
		++iterRequests, nIndex++)
	{
		pCurRequest = Read (*iterRequests);
		if (!pCurRequest)
		{
			lsRequests.erase (iterRequests);
			bIsProcessing = false;
			g_safebIsProcessing.UnLock();
			return false;
		}
		set <wstring>& setRunningReqs = g_safesetRunningRequests.Lock();
		bool bReqRunning = (setRunningReqs.find(pCurRequest->GetGuid()) != setRunningReqs.end());
		if (bReqRunning)
		{
			// the request is being processed
			lsRequests.erase (iterRequests);
			bIsProcessing = false;
			g_safesetRunningRequests.UnLock();	
			g_safebIsProcessing.UnLock();
			return true;
		}
		setRunningReqs.insert(pCurRequest->GetGuid());
		g_safesetRunningRequests.UnLock();	
		if (pCurRequest->IsWorkspace())
		{
			lsRequests.erase (iterRequests);
			bool bRet = pCurRequest->Process();
			bIsProcessing = false;
			g_safebIsProcessing.UnLock();
			return bRet;
		}
		else
		{
			bool bWasBusy = false;
			if (!lnBusyFrames.empty())
			{
				for (list<wstring>::const_iterator iterBusyIds = lnBusyFrames.begin(); 
					iterBusyIds != lnBusyFrames.end();
					++iterBusyIds)
				{
					if (!::wcscmp ((*iterBusyIds).c_str(), pCurRequest->GetFrameGuid().c_str()))
					{
						bWasBusy = true;
						break;
					}
				}
			}
			if (!bWasBusy)
			{
				if (pCurRequest->IsBusy())
				{
					lnBusyFrames.push_back (pCurRequest->GetFrameGuid());
				}
				else
				{
					lsRequests.erase (iterRequests);
					bool bRet = pCurRequest->Process();
					bIsProcessing = false;
					g_safebIsProcessing.UnLock();
					return bRet;
				}
			}
		}
	}
	bIsProcessing = false;
	g_safebIsProcessing.UnLock();
	return true;
}

bool IGRequest::GetParameter (const wchar_t *pwParamName, wstring& wsParamValue)
{
	if (!m_spRequestParams)
		return false;
	CComPtr <IXMLDOMNamedNodeMap> spXMLParamAttributes;
	HRESULT hr = m_spRequestParams->get_attributes (&spXMLParamAttributes);
	if ((hr != S_OK) || !spXMLParamAttributes)
		return false;
	// read request type
	CComPtr <IXMLDOMNode> spXMLParamFrameId;
	hr = spXMLParamAttributes->getNamedItem (CComBSTR (pwParamName), &spXMLParamFrameId);
	if ((hr != S_OK) || !spXMLParamFrameId)
		return false;
	VARIANT varFrameIdValue;
	hr = spXMLParamFrameId->get_nodeValue (&varFrameIdValue);
	if (hr != S_OK)
		return false;
	if (!varFrameIdValue.bstrVal)
		return false;
	wsParamValue = varFrameIdValue.bstrVal;
	return true;
}

bool IGRequest::GetParameter (const wchar_t *pwParamName, int& nParamValue)
{
	if (!m_spRequestParams)
		return false;
	CComPtr <IXMLDOMNamedNodeMap> spXMLParamAttributes;
	HRESULT hr = m_spRequestParams->get_attributes (&spXMLParamAttributes);
	if ((hr != S_OK) || !spXMLParamAttributes)
		return false;
	// read request type
	CComPtr <IXMLDOMNode> spXMLParamFrameId;
	hr = spXMLParamAttributes->getNamedItem (CComBSTR (pwParamName), &spXMLParamFrameId);
	if ((hr != S_OK) || !spXMLParamFrameId)
		return false;
	VARIANT varFrameIdValue;
	hr = spXMLParamFrameId->get_nodeValue (&varFrameIdValue);
	if (hr != S_OK)
		return false;
	if (IsNumber (varFrameIdValue.bstrVal))
	{
		nParamValue = ::_wtoi (varFrameIdValue.bstrVal);
		return true;
	}
	return false;
}

bool IGRequest::GetParameter (const wchar_t *pwParamName, list<POINT>& lPts)
{
	wstring wsPoints;
	if (!GetParameter (pwParamName, wsPoints))
		return false;
	IGLibrary::IGFrame *pFrame = GetFrame();
	if (!pFrame)
		return false;
	IGFrame& image = *pFrame;
	IGLayer *pLayer = image.GetLayer (m_nLayerId);
	if (!pLayer)
		return false;
	size_t szNextComa = wstring::npos;
	double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
	IGConvertible::FromDZtoIGRatios (image.GetWidth(), image.GetHeight(), dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
	wstring wsCurCoord;
	POINT ptCur;
	bool bXCoord = true;
	if (!wsPoints.empty())
	{
		do
		{
			szNextComa = wsPoints.find (L',');
			wsCurCoord = wsPoints.substr (0, szNextComa);
			if (bXCoord)
				ptCur.x = (int)(_wtoi (wsCurCoord.c_str()) * dSeadragonToPixelRatioX);
			else
				ptCur.y = (int)(_wtoi (wsCurCoord.c_str()) * dSeadragonToPixelRatioY);
			if (szNextComa != wstring::npos)
				wsPoints = wsPoints.substr (szNextComa + 1);
			if (!bXCoord)
				lPts.push_back (ptCur);
			bXCoord = !bXCoord;
		} while (szNextComa != wstring::npos);
		// no coord
		if (lPts.empty())
			return false;
	}
	return true;
}

bool IGRequest::SetParameter (const wchar_t *pwParamName, const wstring& wsParamValue)
{
	CComPtr <IXMLDOMNamedNodeMap> spXMLParamAttributes;
	HRESULT hr = m_spRequestParams->get_attributes (&spXMLParamAttributes);
	if ((hr != S_OK) || !spXMLParamAttributes)
		return false;
	CComPtr <IXMLDOMAttribute> spNewAttribute;
	IXMLDOMDocument *pDoc = g_safepXMLDoc.Lock();
	hr = pDoc->createAttribute (CComBSTR (pwParamName), &spNewAttribute);
	g_safepXMLDoc.UnLock();
	if ((hr != S_OK) || !spNewAttribute)
		return false;
	hr = spNewAttribute->put_value (CComVariant (wsParamValue.c_str()));
	if (hr != S_OK)
		return false;
	CComPtr <IXMLDOMNode> spNewAttributeAdded;
	hr = spXMLParamAttributes->setNamedItem (spNewAttribute, &spNewAttributeAdded);
	return hr == S_OK;
}

bool IGRequest::SetParameter (const wchar_t *pwParamName, int nParamValue)
{
	CComPtr <IXMLDOMNamedNodeMap> spXMLParamAttributes;
	HRESULT hr = m_spRequestParams->get_attributes (&spXMLParamAttributes);
	if ((hr != S_OK) || !spXMLParamAttributes)
		return false;
	CComPtr <IXMLDOMAttribute> spNewAttribute;
	IXMLDOMDocument *pDoc = g_safepXMLDoc.Lock();
	hr = pDoc->createAttribute (CComBSTR (pwParamName), &spNewAttribute);
	g_safepXMLDoc.UnLock();
	if ((hr != S_OK) || !spNewAttribute)
		return false;
	wchar_t wParamValue [32];
	_itow (nParamValue, wParamValue, 10);
	hr = spNewAttribute->put_value (CComVariant (wParamValue));
	if (hr != S_OK)
		return false;
	CComPtr <IXMLDOMNode> spNewAttributeAdded;
	hr = spXMLParamAttributes->setNamedItem (spNewAttribute, &spNewAttributeAdded);
	return hr == S_OK;
}

void IGRequest::WriteParams()
{	// do nothing
}

void IGRequest::OnProgress (int nProgress, LPCWSTR pcwMessage)
{
	if (m_bEnableProgess)
	{
		IGAnswerProgress<IGRequest, IGAnswer> progress (*this, nProgress, pcwMessage);
		progress.Write();
	}
}

IIGMainWindow *IGRequest::GetMainWindow()
{
	_ASSERTE (g_pDispMainWindow && L"IGMainWindow is NULL");
	return g_pDispMainWindow;
}

void IGRequest::SetMainWindow (IIGMainWindow *pMainWindow)
{
	g_pDispMainWindow = pMainWindow;
	CComQIPtr <IOleWindow> spWindow (g_pDispMainWindow);
	HWND hWndMain = NULL;
	spWindow->GetWindow (&hWndMain);
	spWindow.Detach();	
	g_hWndSocket = ::GetParent (hWndMain);
}

bool IGRequest::IsAsync() const
{
	return false;
}

IGCommunicationBase* IGRequest::Clone() const
{
	return new IGRequest (*this);
}

bool IGRequest::InternalProcess()
{
	return false;
}

LPCWSTR IGRequest::InternalToString()
{
	return NULL;
}

bool IGRequest::getXMLRequest(const string& sRequest, IXMLDOMNode **ppXMLRequest)
{
	// create xml document if necessary
	IXMLDOMDocument*& gref_pXMLDoc = g_safepXMLDoc.Lock();
	if (!gref_pXMLDoc)
	{
		if (!CreateXMLDoc()){
			g_safepXMLDoc.UnLock();
			return false;
		}
	}
	// read request
	VARIANT_BOOL bSuccess;
	HRESULT hr = gref_pXMLDoc->loadXML (CComBSTR (sRequest.c_str()), &bSuccess);
	if ((hr != S_OK) || !bSuccess){
		g_safepXMLDoc.UnLock();
		return false;
	}
	hr = gref_pXMLDoc->selectSingleNode (CComBSTR (IGREQUEST_NODENAME), ppXMLRequest);
	g_safepXMLDoc.UnLock();
	if ((hr != S_OK) || !*ppXMLRequest)
		return false;
	return true;
}

bool IGRequestFrame::SetFrameId (int nPos)
{
	IGSmartPtr <IGFrame> spFrame;
	if (!m_spFrameMgr->GetFrame (nPos, spFrame))
		return false;
	if (!spFrame)
		return false;
	m_wsFrameGuid = spFrame->GetId();
	return true;
}

int IGRequestFrame::GetFrameId()
{
	int nFrameId = GetFramePos();
	IGSmartPtr <IGFrame> spFrame;
	if (nFrameId < 0)
		return m_spFrameMgr->GetFramePos (GetFrameGuid().c_str());
	return nFrameId;
}

bool IGRequestFrame::IsBusy()
{
	IGFrame *pFrame = GetFrame();
	if (!pFrame)
		return false;
	return !pFrame->RequestAccess();
}

IGFrame *IGRequestFrame::GetFrame()
{
	bool bFrameFound = false;
	int nFrameId = GetFramePos();
	IGSmartPtr <IGFrame> spFrame;
	if (nFrameId < 0)
	{
		bFrameFound = m_spFrameMgr->GetFrame (GetFrameGuid().c_str(), spFrame);
		if (!bFrameFound || !spFrame)
			return NULL;
		return spFrame;
	}
	bFrameFound = m_spFrameMgr->GetFrame (nFrameId, spFrame);
	if (!bFrameFound || !spFrame)
		return NULL;
	return spFrame;
}

IGCommunicationBase* IGRequestFrame::Clone() const
{
	return new IGRequestFrame (*this);
}

void IGRequestFrame::OnProgress (int nProgress, LPCWSTR pcwMessage)
{
	IGAnswerProgress<IGRequestFrame, IGAnswerFrame> progress (*this, nProgress, pcwMessage);
	progress.Write();
}

bool IGRequestFrame::Answer()
{
	if (IsUpdatingFrame())
	{
		IGAnswerFrameChanged answer (*this);
		return answer.Write();
	}
	IGAnswerActionDone answer (*this);
	return answer.Write();
}