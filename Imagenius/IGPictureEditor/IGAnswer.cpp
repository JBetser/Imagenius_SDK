#include "StdAfx.h"
#include "IGAnswer.h"
#include "IGSocket.h"
#include <IGFrameHistory.h>
#include <IGImageProcMessage.h>
#include <IGLog.h>

using namespace std;
using namespace IGLibrary;

#define IGANSWER_XMLTEMPLATE				L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Answer Id=\"\" UserLogin=\"\" ReqGuid=\"\">\n<Params/>\n</Answer>"
#define IGANSWER_NODENAME					L"Answer"
#define IGANSWER_ANSWERID					L"Id"
#define IGANSWER_REQGUID					L"ReqGuid"
#define IGANSWER_USERLOGIN					L"UserLogin"

IGThreadSafeData <list <string>>		IGAnswer::g_safelAnswerBuffer;

IGAnswer::IGAnswer (const IGCommunicationBase& request, IGANSWER_ID eAnswerId)	: IGRequest (request)
																				, m_eAnswerId (eAnswerId)
{
	Initialize();
}

IGAnswer::IGAnswer (IGLibrary::IGREQUEST_TYPE eType, IGANSWER_ID eAnswerId)	: IGRequest (L"", eType, (int)eAnswerId)
																			, m_eAnswerId (eAnswerId)
{
	Initialize();
}

IGAnswer::~IGAnswer(void)
{	
}

bool IGAnswer::Initialize()
{
	if (!IGRequest::Initialize())
		return false;
	// load answer template
	VARIANT_BOOL bSuccess;
	IXMLDOMDocument *pDoc = g_safepXMLDoc.Lock();
	HRESULT hr = pDoc->loadXML (CComBSTR (IGANSWER_XMLTEMPLATE), &bSuccess);
	if ((hr != S_OK) || !bSuccess){
		g_safepXMLDoc.UnLock();
		return false;
	}
	// write answer
	hr = pDoc->selectSingleNode (CComBSTR (IGANSWER_NODENAME), &m_spXMLAnswer);
	g_safepXMLDoc.UnLock();
	if ((hr != S_OK) || !m_spXMLAnswer)
		return false;
	return true;
}

bool IGAnswer::Write()
{	
	if (!m_spXMLAnswer)
		return false;	
	CComPtr <IXMLDOMNamedNodeMap> spXMLParamAttributes;
	HRESULT hr = m_spXMLAnswer->get_attributes (&spXMLParamAttributes);
	if ((hr != S_OK) || !spXMLParamAttributes)
		return false;
	// write answer id
	CComPtr <IXMLDOMAttribute> spAnswerIdAttribute;
	IXMLDOMDocument* pDoc = g_safepXMLDoc.Lock();
	hr = pDoc->createAttribute (CComBSTR (IGANSWER_ANSWERID), &spAnswerIdAttribute);
	if ((hr != S_OK) || !spAnswerIdAttribute){
		g_safepXMLDoc.UnLock();
		return false;
	}
	wchar_t wAnswerId[32];
	if (::_itow_s ((int)m_eAnswerId, wAnswerId, 10) != 0){
		g_safepXMLDoc.UnLock();
		return false;
	}
	hr = spAnswerIdAttribute->put_value (CComVariant (wAnswerId));
	CComPtr <IXMLDOMNode> spAnswerIdAttributeAdded;
	hr = spXMLParamAttributes->setNamedItem (spAnswerIdAttribute, &spAnswerIdAttributeAdded);
	// write user login
	CComPtr <IXMLDOMAttribute> spLoginAttribute;
	hr = pDoc->createAttribute (CComBSTR (IGANSWER_USERLOGIN), &spLoginAttribute);
	if ((hr != S_OK) || !spLoginAttribute){
		g_safepXMLDoc.UnLock();
		return false;
	}
	hr = spLoginAttribute->put_value (CComVariant (m_wsUser.c_str()));
	CComPtr <IXMLDOMNode> spLoginAttributeAdded;
	hr = spXMLParamAttributes->setNamedItem (spLoginAttribute, &spLoginAttributeAdded);
	// write request guid
	CComPtr <IXMLDOMAttribute> spReqIdAttribute;
	hr = pDoc->createAttribute (CComBSTR (IGANSWER_REQGUID), &spReqIdAttribute);
	if ((hr != S_OK) || !spReqIdAttribute){
		g_safepXMLDoc.UnLock();
		return false;
	}
	hr = spReqIdAttribute->put_value (CComVariant (m_wsGuid.c_str()));
	CComPtr <IXMLDOMNode> spReqIdAttributeAdded;
	hr = spXMLParamAttributes->setNamedItem (spReqIdAttribute, &spReqIdAttributeAdded);
	// write answer parameters
	if (!m_spRequestParams)
	{
		hr = m_spXMLAnswer->get_firstChild (&m_spRequestParams);
		if ((hr != S_OK) || !m_spRequestParams){
			g_safepXMLDoc.UnLock();
			return false;
		}
	}
	WriteParams();
	CComBSTR spXml;
	hr = pDoc->get_xml (&spXml);
	g_safepXMLDoc.UnLock();
	if ((hr != S_OK) || !spXml)
		return false;
	// conversion to ANSI
	size_t szSizeToConvert = spXml.Length() + 1;
	size_t szSizeConverted = 0;
	char *pcMb = new char[spXml.Length() + 1];
	if (::wcstombs_s (&szSizeConverted, pcMb, szSizeToConvert, spXml, spXml.Length() + 1) != 0)
		return false;
	if (szSizeConverted != szSizeToConvert)
		return false;
	g_safelAnswerBuffer.Lock().push_back(string (pcMb));
	g_safelAnswerBuffer.UnLock();
	delete [] pcMb;
	return true;
}

// asynchronous
bool IGAnswer::NotifySuccess (IGImageProcMessage *pMessage)
{
	if (!pMessage)
		return false;
	IGCommunicationBase *pCom = pMessage->GetRequest();
	if (!pCom)
		return true; // no request to answer, just leave
	IGRequest *pRequest = dynamic_cast <IGRequest*> (pCom);
	if (!pRequest)
		return false;
	bool bRes = process (*pRequest, pMessage->GetImageProcessingId());	
	if (bRes)
		IGRequest::Flush (pRequest->GetGuid(), true);
	return bRes;
}

// synchronous
bool IGAnswer::NotifySuccess (IGRequest *pRequest)
{
	if (!pRequest)
		return false;
	bool bRes = process (*pRequest);
	if (bRes)
		IGRequest::Flush (pRequest->GetGuid(), true);
	return bRes;
}

bool IGAnswer::NotifyFailure (IGCommunicationBase *pRequest)
{
	if (!pRequest)
		return false;
	IGAnswer *pAnswer;
	if (pRequest->IsFrame())
	{
		IGRequestFrame *pReqFrame = dynamic_cast <IGRequestFrame *> (pRequest);
		if (!pReqFrame)
			return false;
		pAnswer = new IGAnswerFrameActionFailed (*pReqFrame);
	}
	else if (pRequest->IsWorkspace())
		pAnswer = new IGAnswerWorkspaceActionFailed (*pRequest);
	else
		return false;
	bool bRes = pAnswer->Write();
	delete pAnswer;
	if (bRes)
		IGRequest::Flush (pRequest->GetGuid(), false);
	return bRes;
}

bool IGAnswer::process (IGRequest& request, int nImProcId)
{
	switch (request.GetId())
	{
		case IGREQUEST_WORKSPACE_PING:
		{	IGAnswerWorkspacePong answer (request);
			return answer.Write();
		}
		case IGREQUEST_WORKSPACE_CONNECT:
		{	IGAnswerWorkspaceConnected answer (request);
			return answer.Write();
		}
		case IGREQUEST_WORKSPACE_DISCONNECT:
		case IGREQUEST_WORKSPACE_DESTROYACCOUNT:
		{	IGAnswerWorkspaceDisconnected answer (request);
			return answer.Write();
		}
		case IGREQUEST_WORKSPACE_NEWIMAGE:
		case IGREQUEST_WORKSPACE_LOADIMAGE:
		{	IGAnswerFrameChanged answer (request);
			return answer.Write();
		}
		default:
			return request.Answer();
	}
}

bool IGAnswer::TransferParameter (const IGCommunicationBase& request, LPCWSTR pcwParamName)
{
	if (!m_spRequestParams)
	{
		HRESULT hr = m_spXMLAnswer->get_firstChild (&m_spRequestParams);
		if ((hr != S_OK) || !m_spRequestParams)
			return false;
	}
	IGCommunicationBase *pNConstCommBase = const_cast <IGCommunicationBase*> (&request);
	IGRequest *pRequest = dynamic_cast <IGRequest*> (pNConstCommBase);
	if (!pRequest)
		return false;
	wstring wsParam;
	if (!pRequest->GetParameter (pcwParamName, wsParam))
		return false;
	return SetParameter (pcwParamName, wsParam);
}

bool IGAnswer::SetParameter (const wchar_t *pwParamName, const std::wstring& wsValue)
{
	if (!m_spRequestParams)
	{
		HRESULT hr = m_spXMLAnswer->get_firstChild (&m_spRequestParams);
		if ((hr != S_OK) || !m_spRequestParams)
			return false;
	}
	return IGRequest::SetParameter (pwParamName, wsValue);
}

bool IGAnswer::SetParameter (const wchar_t *pwParamName, int nParamValue)
{
	if (!m_spRequestParams)
	{
		HRESULT hr = m_spXMLAnswer->get_firstChild (&m_spRequestParams);
		if ((hr != S_OK) || !m_spRequestParams)
			return false;
	}
	return IGRequest::SetParameter (pwParamName, nParamValue);
}

IGAnswerWorkspaceConnected::IGAnswerWorkspaceConnected (const IGCommunicationBase& request) : IGAnswer (request, IGANSWER_WORKSPACE_CONNECTED) 
{
	// add the frame ids to connection parameters
	CComBSTR spbstrFrameIds;
	m_pWorkspace->GetFrameIds (&spbstrFrameIds);
	m_wsFrameIds = spbstrFrameIds;

	// add the frame properties to connection parameters
	CComBSTR spbstrFrameProps;
	m_pWorkspace->GetFrameProperties (&spbstrFrameProps);
	m_wsFrameProps = spbstrFrameProps;

	// add the frame names id to connection parameters
	CComBSTR spbstrFrameNames;
	m_pWorkspace->GetFrameNames (&spbstrFrameNames);
	m_wsFrameNames = spbstrFrameNames;

	// add the nb layers to connection parameters
	CComBSTR spbstrFrameNbLayers;
	m_pWorkspace->GetFrameNbLayers (&spbstrFrameNbLayers);
	m_wsFrameNbLayers = spbstrFrameNbLayers;

	// add the layer visibility to connection parameters
	CComBSTR spbstrFrameVisibility;
	m_pWorkspace->GetFrameLayerVisibility (&spbstrFrameVisibility);
	m_wsFrameVisibility = spbstrFrameVisibility;

	// add the step ids to connection parameters
	CComBSTR spbstrFrameStepIds;
	m_pWorkspace->GetFrameStepIds (&spbstrFrameStepIds);
	m_wsFrameStepIds = spbstrFrameStepIds;
	
	// add the current selected frame id to connection parameters
	CComBSTR spbstrActiveFrameId;
	m_pWorkspace->GetActiveFrameId (&spbstrActiveFrameId);
	m_wsActiveFrameId = spbstrActiveFrameId;
	
	// mini pictures
	wstring wsOutputPath (L"igoutput");
	wsOutputPath += m_spConfigManager->GetServerIP();
	wsOutputPath += L"/";
	wstring wsUserOutput (wsOutputPath);
	wsUserOutput += m_spConfigManager->GetUserLogin (true);
	wsUserOutput += L"/";		
	SetParameter (IGANSWER_USEROUTPUT, wsUserOutput.c_str());
	wstring wsMiniPictures;
	m_spConfigManager->GetMiniPictures(wsMiniPictures);
	wstring wsOuputPath;
	m_spConfigManager->GetOutputPath(wsOuputPath);
	int nIdx = 0;
	while((nIdx = wsMiniPictures.find (wsOuputPath)) != wstring::npos)
	{
		m_wsConvertMiniPictures += wsMiniPictures.substr (0, nIdx);
		m_wsConvertMiniPictures += wsOutputPath;
		wsMiniPictures = wsMiniPictures.substr (nIdx + 3);
	}
	m_wsConvertMiniPictures += wsMiniPictures;
	_ASSERTE (m_pWorkspace && L"IGAnswerWorkspaceConnected - Fatal Error: NULL Workspace");

	// workspace properties
	CComBSTR spbstrProperties;
	m_pWorkspace->GetPropertiesAndValues (&spbstrProperties);
	m_wsWorkspaceProperties = spbstrProperties;		
}

IGAnswerFrame::IGAnswerFrame (IGRequest& request, IGANSWER_ID nAnswerId) : IGAnswer (request, nAnswerId) 
{
	m_wsFrameGuid = request.GetFrameGuid();
	m_wsFrameName = request.GetFrameName();
}

IGAnswerFrameWithProps::IGAnswerFrameWithProps (IGRequest& request, IGANSWER_ID nAnswerId) : IGAnswerFrame (request, nAnswerId) 
{
	CComBSTR spbstrProperties;
	m_pWorkspace->GetPropertiesAndValues (&spbstrProperties);
	m_wsWkspceProperties = spbstrProperties;
	IGLibrary::IGFrame* pFrame (request.GetFrame());
	if (pFrame)
	{
		if ((request.GetId() != IGLibrary::IGREQUEST_WORKSPACE_LOADIMAGE) &&
			(request.GetId() != IGLibrary::IGREQUEST_WORKSPACE_CONNECT) &&
			(request.GetId() != IGLibrary::IGREQUEST_WORKSPACE_NEWIMAGE) &&
			(request.GetId() != IGLibrary::IGREQUEST_FRAME_CLOSE) &&
			(nAnswerId != IGANSWER_FRAME_PROGRESS))
		{
			pFrame->GetStepIds (m_wsStepIds);	// history
		}
		pFrame->GetPropertiesAndValues (m_wsProperties);	// frame properties
	}
}

IGAnswerFrameChanged::IGAnswerFrameChanged (IGRequest& request) : IGAnswerFrameWithProps (request, IGANSWER_FRAME_CHANGED)
																, m_nNbLayers(0)
{
	if (request.GetId() != IGLibrary::IGREQUEST_FRAME_CLOSE)
	{		
		std::wstring wsPicPath;
		IGLibrary::IGFrame *pFrame = NULL;
		if (request.GetId() == IGLibrary::IGREQUEST_WORKSPACE_LOADIMAGE){
			IGSmartPtr <IGFrame> spFrame;
			if (m_spFrameMgr->GetFrame (m_spFrameMgr->GetNbFrames() - 1, spFrame))
				pFrame = spFrame;
		}
		else
			pFrame = request.GetFrame();
		pFrame->GetPath (wsPicPath);
		std::wstring wsUserPicturesPath;
		m_spConfigManager->GetUserPicturesPath (wsUserPicturesPath);
		if (!wsPicPath.empty())
			m_wsFrameName = wsPicPath.substr (wsUserPicturesPath.length());
		int nIdxLastBackSlash = m_wsFrameName.find_last_of(L'\\');
		if (nIdxLastBackSlash != std::wstring::npos)
			m_wsFrameName = m_wsFrameName.substr (nIdxLastBackSlash + 1);
		m_nNbLayers = pFrame->GetNbLayers();
		for(int idxLayer = 0; idxLayer < m_nNbLayers; idxLayer++){
			m_wsLayerVisibility += pFrame->GetLayer(idxLayer)->GetVisible() ? L"1" : L"0";
			if (idxLayer < m_nNbLayers - 1)
				m_wsLayerVisibility += L"_";
		}
	}
}

IGAnswerPropertyChanged::IGAnswerPropertyChanged (IGRequest& request) : IGAnswerFrameWithProps (request, IGANSWER_WORKSPACE_PROPERTYCHANGED) 
{
	IGLibrary::IGFrame *pFrame = request.GetFrame();
	m_bIsFrame = (pFrame != NULL && request.GetRequestType() == IGREQUEST_FRAME);
	if (m_bIsFrame)
		pFrame->GetPropertiesAndValues (m_wsProperties);
	else
	{
		CComBSTR spbstrProperties;
		m_pWorkspace->GetPropertiesAndValues (&spbstrProperties);
		m_wsProperties = spbstrProperties;
	}
}