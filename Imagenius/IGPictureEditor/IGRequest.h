#pragma once
#include <list>
#include <string>
#include <msxml2.h>
#include <IGFrameManager.h>
#include <IGSingleton.h>
#include <IGImageProcessing.h> // Imagenius requests
#include <IGImageProcMessage.h>
#include <IGThreadSafeData.h>
#include <IGLPE.h>
#include <IGIPFilter.h>
#include <IGPropertyDefs.h>
#include <IGTexture.h>
#include <IGFaceDescriptor.h>
#include "IGMainWindow.h"

#define IGREQUEST_NODENAME			L"Request"
#define IGREQUEST_NODETYPE			L"Type"
#define IGREQUEST_NODEID			L"Id"
#define IGREQUEST_GUID				L"Guid"
#define IGREQUEST_FRAMEID			L"FrameId"
#define IGREQUEST_LAYERID			L"LayerId"
#define IGREQUEST_LAYERIDTO			L"LayerIdTo"
#define IGREQUEST_LAYERFROMIDS		L"LayerFromIds"
#define IGREQUEST_PROCESSID			L"ProcessId"
#define IGREQUEST_FILTERID			L"FilterId"
#define IGREQUEST_ORIGINAL			L"Original"
#define IGREQUEST_FACEEFFECTID		L"FaceEffectId"
#define IGREQUEST_PROPIDS			L"PropIds"
#define IGREQUEST_PATH				L"Path"
#define IGREQUEST_USERLOGIN			L"UserLogin"
#define IGREQUEST_BACKGROUNDMODE	L"BackgroundMode"
#define IGREQUEST_COLORMODE			L"ColorMode"
#define IGREQUEST_SELECTIONTYPE		L"SelectionType"
#define IGREQUEST_TOLERANCE			L"Tolerance"
#define IGREQUEST_POINTS			L"Points"
#define IGREQUEST_POINTS2			L"Points2"
#define IGREQUEST_MARKERTYPE		L"MarkerType"
#define IGREQUEST_MARKERTYPE2		L"MarkerType2"
#define IGREQUEST_VISIBLE			L"Visible"
#define IGREQUEST_RESTOREDFRAMEIDS	L"RestoredFrameIds"
#define IGREQUEST_PICTURENAME		L"PictureName"
#define IGREQUEST_PICTURENAME2		L"PictureName2"
#define IGREQUEST_HISTORYID			L"HistoryId"
#define IGREQUEST_ISFRAME			L"IsFrame"
#define IGREQUEST_COLOR				L"Color"
#define IGREQUEST_COLOR2			L"Color2"
#define IGREQUEST_PARAM				L"Param"
#define IGREQUEST_PARAM2			L"Param2"
#define IGREQUEST_PARAM3			L"Param3"
#define IGREQUEST_DIRECTIONX		L"DirectionX"
#define IGREQUEST_DIRECTIONY		L"DirectionY"
#define IGREQUEST_ANGLE				L"Angle"
#define IGREQUEST_RATE				L"Rate"
#define IGREQUEST_TRANSPARENCY		L"Transparency"
#define IGREQUEST_LOADAS			L"LoadAs"
#define IGREQUEST_BRUSHSIZE			L"BrushSize"
#define IGREQUEST_AUTOROTATE		L"AutoRotate"
#define IGREQUEST_FACEDESCRIPTOR	L"FaceDescriptor"

#define POSTPONE(pMsg)	if (!pMsg->IsPosted()) \
							return IGREQUEST_TOBEPOSTED

class IGRequest : public IGLibrary::IGCommunicationBase
{
	friend class IGAnswer;
public:
	IGRequest (LPCWSTR pcwGuid, IGLibrary::IGREQUEST_TYPE eType, int nId);
	IGRequest (const IGCommunicationBase& request);
	IGRequest (LPCWSTR pcwGuid, IGLibrary::IGREQUEST_TYPE eType, int nId, CComPtr <IXMLDOMNode> spRequestParams);
	virtual ~IGRequest();

	static bool ProcessRequests (std::list<std::string>& lsRequests);
	static LRESULT OnProcessEnded (WPARAM wParam, LPARAM lParam);
	static IGRequest *Read (const std::string& sRequest);
	static IIGMainWindow *GetMainWindow();
	static void SetMainWindow (IIGMainWindow *pMainWindow);
	static void Release();
	static void Flush(const std::wstring& wsGuid, bool bSuccess);
		
	bool Process();	
	std::wstring ToString();
	virtual bool IsAsync() const;	
	virtual IGCommunicationBase* Clone() const;	
	
protected:	
	static bool CreateXMLDoc();

	bool GetParameter (const wchar_t *pwParamName, wstring& wsParamValue);
	bool GetParameter (const wchar_t *pwParamName, int& nParamValue);
	bool GetParameter (const wchar_t *pwParamName, list<POINT>& lPts);
	virtual bool SetParameter (const wchar_t *pwParamName, int nParamValue);
	virtual bool SetParameter (const wchar_t *pwParamName, const std::wstring& wsParamValue);
	virtual void WriteParams();
	virtual void OnProgress (int nProgress, LPCWSTR pcwMessage);
	virtual bool InternalProcess();
	virtual LPCWSTR InternalToString();
	virtual LRESULT PostProcessing (IGLibrary::IGImageProcMessage *pMessage) {return pMessage ? (pMessage->GetResult() ? IGREQUEST_SUCCEEDED : IGREQUEST_FAILED) : IGREQUEST_FAILED;}
	virtual bool SimpleThreadProcess() {
		return false;
	}
	virtual bool Answer();
	virtual bool Initialize();
	
	static IIGMainWindow								*g_pDispMainWindow;
	static IGLibrary::IGThreadSafeData <IXMLDOMDocument*>	g_safepXMLDoc;	
	static IGLibrary::IGThreadSafeData <bool>			g_safebIsProcessing;
	static IGLibrary::IGThreadSafeData <std::set <std::wstring>>		g_safesetRunningRequests;
	static HWND											g_hWndSocket;
	CComPtr <IXMLDOMNode>								m_spRequestParams;
	IGLibrary::IGSingleton <IGLibrary::IGConfigManager>	m_spConfigManager;	
	IIGWorkspace										*m_pWorkspace;
	IGLibrary::IGSingleton <IGLibrary::IGFrameManager>	m_spFrameMgr;
	bool												m_bEnableProgess;

private:
	static bool getXMLRequest(const std::string& sRequest, IXMLDOMNode **pXMLRequest);
};

//synchronous
class IGRequestFrame : public IGRequest
{
public:
	IGRequestFrame (LPCWSTR pcwGuid, int nReqId, CComPtr <IXMLDOMNode> spRequestParams) : IGRequest (pcwGuid, IGLibrary::IGREQUEST_FRAME, nReqId, spRequestParams)
	{
		// test special case : Image processing non-frame based
		if (nReqId > 0)
			GetParameter (IGREQUEST_FRAMEID, m_wsFrameGuid);	
		// add the picture name in the xml (for session coherency)
		IGLibrary::IGFrame *pFrame = GetFrame();
		if (pFrame)
			pFrame->GetName (m_wsFrameName);
	}
	IGRequestFrame (LPCWSTR pcwGuid, IGLibrary::IGREQUEST_TYPE eType, int nReqId, CComPtr <IXMLDOMNode> spRequestParams) : IGRequest (pcwGuid, eType, nReqId, spRequestParams)
	{
		// special case : Workspace processing frame based (e.g. Connection)
	}
	IGRequestFrame (const IGRequestFrame& reqCopy) : IGRequest(reqCopy)
	{
	}
	virtual ~IGRequestFrame(){}
	virtual IGLibrary::IGFrame *GetFrame();
	virtual bool IsBusy();
	virtual IGCommunicationBase* Clone() const;	
	virtual void OnProgress (int nProgress, LPCWSTR pcwMessage);
	virtual bool Answer();
	bool SetFrameId (int nPos);
	int GetFrameId();	
};

//synchronous
class IGRequestLayer : public IGRequestFrame
{
public:
	IGRequestLayer (LPCWSTR pcwGuid, int nReqId, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestFrame (pcwGuid, nReqId, spRequestParams)
	{
		GetParameter (IGREQUEST_LAYERID, m_nLayerId);
	}
	virtual ~IGRequestLayer(){}
	virtual bool IsLayer() const {return true;}
};

//asynchronous
template <IGLibrary::IGImageProcessing::TYPE_IMAGE_PROC_ID IMAGE_PROC_ID, IGLibrary::EnumStepType STEP_ID = IGLibrary::IGFRAMEHISTORY_STEP_UNDEFINED>
class IGRequestImageProcessing : public IGRequestFrame
{
public:
	IGRequestImageProcessing (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams)	: IGRequestFrame (pcwGuid, IMAGE_PROC_ID, spRequestParams)
	{
		m_eStepId = STEP_ID;
	}
	IGRequestImageProcessing (LPCWSTR pcwGuid, IGLibrary::IGREQUEST_TYPE eType, CComPtr <IXMLDOMNode> spRequestParams)	: IGRequestFrame (pcwGuid, -1, spRequestParams) 
	{
		m_nId = IMAGE_PROC_ID;
		m_eStepId = STEP_ID;
		m_eType = eType; 
	}
	virtual ~IGRequestImageProcessing(){}
	virtual bool IsAsync() const {return true;}
	virtual bool InternalProcess() {
		IGImageProcessing *pImageProcessing = new IGImageProcessing (GetFrame(), new IGImageProcMessage(IMAGE_PROC_ID));
		m_wImageProcName = pImageProcessing->GetName();
		return pImageProcessing->Start();
	}	

protected:
	virtual LPCWSTR InternalToString() {return m_wImageProcName.c_str();}

private:
	std::wstring	m_wImageProcName;
};

//asynchronous
template <IGLibrary::IGImageProcessing::TYPE_IMAGE_PROC_ID IMAGE_PROC_ID, IGLibrary::EnumStepType STEP_ID = IGLibrary::IGFRAMEHISTORY_STEP_UNDEFINED>
class IGRequestLayerProcessing : public IGRequestImageProcessing <IMAGE_PROC_ID, STEP_ID>
{
public:
	IGRequestLayerProcessing (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		GetParameter (IGREQUEST_LAYERID, m_nLayerId);
	}
	virtual ~IGRequestLayerProcessing(){}
	virtual bool IsLayer() const {return true;}
};

class IGRequestPing : public IGRequest
{
public:
	IGRequestPing (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequest (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, IGLibrary::IGREQUEST_WORKSPACE_PING, spRequestParams){}
	virtual ~IGRequestPing(){}
	
protected:
	virtual LPCWSTR InternalToString() {return L"Ping";}
	virtual bool InternalProcess();
};

class IGRequestConnect : public IGRequestFrame
{
public:
	IGRequestConnect (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestFrame (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, IGLibrary::IGREQUEST_WORKSPACE_CONNECT, spRequestParams)
		, m_bIsAsync (true)
		, m_eStep (IGREQUESTCONNECT_STEP_CONNECTING)
		, m_nNbFrames (0) {}
	IGRequestConnect (const IGRequestConnect& connectCopy) : IGRequestFrame(connectCopy)
	{	m_bIsAsync = connectCopy.m_bIsAsync;
		m_eStep = connectCopy.m_eStep;
		m_nNbFrames = connectCopy.m_nNbFrames;
		m_wsAllNbLayers = connectCopy.m_wsAllNbLayers;}
	virtual ~IGRequestConnect(){}
	virtual bool IsAsync() const {return m_bIsAsync;}
	virtual LRESULT PostProcessing (IGLibrary::IGImageProcMessage *pMessage);
	virtual bool IsFullUpdatingFrame() {return true;}

protected:
	virtual LPCWSTR InternalToString() {return L"Connect user";}
	virtual bool InternalProcess();
	LRESULT processNextImage (IGLibrary::IGImageProcMessage *pMessage, IGLibrary::IGFrame *pPreviousFrame, bool bLastSucceeded);
	virtual void OnProgress (int nProgress, LPCWSTR pcwMessage);

	std::wstring m_wsImagePath;
	bool	m_bIsAsync;
	enum {	IGREQUESTCONNECT_STEP_CONNECTING, 
			IGREQUESTCONNECT_STEP_LOADING,
			IGREQUESTCONNECT_STEP_CREATINGHISTORY}	m_eStep;
	int		m_nNbFrames;
	std::wstring m_wsAllNbLayers;
};

class IGRequestDisconnect : public IGRequest
{
public:
	IGRequestDisconnect (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequest (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, IGLibrary::IGREQUEST_WORKSPACE_DISCONNECT, spRequestParams)
		, m_bIsAsync (true){}
	virtual ~IGRequestDisconnect(){}
	virtual bool IsAsync() const {return m_bIsAsync;}
	virtual LRESULT PostProcessing (IGLibrary::IGImageProcMessage *pMessage);
	virtual bool IsUpdatingFrame() const {return false;}

protected:
	virtual LPCWSTR InternalToString() {return L"Disconnect user";}
	virtual bool InternalProcess();
	bool	m_bIsAsync;
};

class IGRequestNew : public IGRequestImageProcessing <IGLibrary::IGREQUEST_WORKSPACE_NEWIMAGE>
{
public:
	IGRequestNew (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams)	: IGRequestImageProcessing (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, spRequestParams)
																			, m_eStep (IGREQUESTCONNECT_STEP_CREATINGHISTORY) {}
	virtual ~IGRequestNew(){}
	virtual bool IsFullUpdatingFrame() {return true;}
protected:
	virtual LPCWSTR InternalToString() {return L"New";}
	virtual bool InternalProcess();
	virtual LRESULT PostProcessing (IGLibrary::IGImageProcMessage *pMessage);
	virtual bool IsUpdatingFrame() const {return false;}

	enum {	IGREQUESTCONNECT_STEP_CREATINGHISTORY,
			IGREQUESTCONNECT_STEP_GENERATINGOUTPUT}	m_eStep;
};

class IGRequestLoad : public IGRequestImageProcessing <IGLibrary::IGREQUEST_WORKSPACE_LOADIMAGE>
{
public:
	IGRequestLoad (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams)	: IGRequestImageProcessing (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, spRequestParams)
																			, m_eStep (IGREQUESTLOAD_STEP_LOADING) {
		std::wstring wsLoadAs;
		GetParameter (IGREQUEST_LOADAS, wsLoadAs);
		if (!wsLoadAs.empty())
			m_wsLoadAs = wsLoadAs;
	}
	virtual ~IGRequestLoad(){}
	virtual LRESULT PostProcessing (IGLibrary::IGImageProcMessage *pMessage);
	virtual bool IsFullUpdatingFrame() {return true;}
protected:
	virtual LPCWSTR InternalToString() {return L"Load Frame";}
	virtual bool InternalProcess();
	enum {	IGREQUESTLOAD_STEP_LOADING, 
			IGREQUESTLOAD_STEP_CREATINGHISTORY}	m_eStep;
	std::wstring m_wsLoadAs;
};

class IGRequestSave : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_SAVE>
{
public:
	IGRequestSave (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams){}
	virtual ~IGRequestSave(){}
	bool IsUpdatingFrame() const {return false;}
protected:
	virtual LPCWSTR InternalToString() {return L"Save Frame";}
	virtual bool InternalProcess();
};

class IGRequestCloseFrame : public IGRequestFrame
{
public:
	IGRequestCloseFrame (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestFrame (pcwGuid, IGLibrary::IGREQUEST_FRAME_CLOSE, spRequestParams){
		if (m_wsFrameGuid == L"-1") // all frames
			m_eType = IGLibrary::IGREQUEST_WORKSPACE;
	}
	virtual ~IGRequestCloseFrame(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Close frame";}
	virtual bool InternalProcess();
	virtual bool IsUpdatingFrame() const {return false;}
};

class IGRequestMergeLayer : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_MERGELAYER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERMERGE>
{
public:
	IGRequestMergeLayer (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		GetParameter (IGREQUEST_LAYERID, m_nLayerId);
		GetParameter (IGREQUEST_LAYERFROMIDS, m_wsLayerFromIds);
	}
	virtual ~IGRequestMergeLayer(){}
	virtual bool IsFullUpdatingFrame() {return true;}
protected:
	virtual LPCWSTR InternalToString() {return L"Merge layers";}
	virtual bool InternalProcess();
	std::wstring	m_wsLayerFromIds;
};

class IGRequestFilter : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_FILTER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE>
{
public:
	IGRequestFilter (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams)
																			, m_nFaceEffectId (-1)
																			, m_col1 (0)
																			, m_col2 (0)
																			, m_dParam1 (-1.0)
																			, m_dParam2 (-1.0)
																			, m_dParam3 (-1.0)
	{	GetParameter (IGREQUEST_FILTERID, m_nFilterId);
		int nOriginal = 0;
		GetParameter (IGREQUEST_ORIGINAL, nOriginal);
		m_bOriginal = (nOriginal == 1);
		if (m_nFilterId == IGLibrary::IGIPFILTER_FACE_EFFECT){
			GetParameter (IGREQUEST_FACEEFFECTID, m_nFaceEffectId);
			int nParam;
			if (GetParameter (IGREQUEST_PARAM, nParam))
				m_dParam1 = (double)nParam;
			if (GetParameter (IGREQUEST_PARAM2, nParam))
				m_dParam2 = (double)nParam;
			if (GetParameter (IGREQUEST_PARAM3, nParam))
				m_dParam3 = (double)nParam;
			GetParameter (IGREQUEST_PICTURENAME, m_wsPicName1);
			GetParameter (IGREQUEST_PICTURENAME2, m_wsPicName2);			
			GetParameter (IGREQUEST_FACEDESCRIPTOR, m_wsFaceDescriptor);			
		}else{
			int nStrength = -1, nType = -1, nPosX = -1, nPosY = -1, nDirectionX = -1, nDirectionY = -1, nTolerance = -1;
			GetParameter (IGIPFILTER_PARAM_STRENGTH, nStrength);
			GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_STRENGTH, nStrength);
			if (GetParameter (IGIPFILTER_PARAM_TYPE, nType))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_TYPE, nType);
			if (GetParameter (IGIPFILTER_PARAM_POSX, nPosX))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_POSX, nPosX);
			if (GetParameter (IGIPFILTER_PARAM_POSY, nPosY))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_POSY, nPosY);
			if (GetParameter (IGIPFILTER_PARAM_DIRECTIONX, nDirectionX))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_DIRECTIONX, nDirectionX);
			if (GetParameter (IGIPFILTER_PARAM_DIRECTIONY, nDirectionY))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_DIRECTIONY, nDirectionY);
			if (GetParameter (IGIPFILTER_PARAM_TOLERANCE, nTolerance))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_TOLERANCE, nTolerance);
			std::wstring wsScript;
			if (GetParameter (IGIPFILTER_PARAM_SCRIPT, wsScript))
				GetFrame()->SetRequestProperty (IGIPFILTER_PARAM_SCRIPT, wsScript);
		}
		// transfer the current texture to the frame
		IGLibrary::IGSELECTTEXTURE_TYPE_ENUM eTexture = IGLibrary::IGSELECTTEXTURE_TYPE_ENUM_UNKNOWN;
		m_spFrameMgr->GetProperty (IGLibrary::IGProperties::IGPROPERTY_SELECT_TEXTURE, eTexture);
		IGLibrary::IGTexture texture1, texture2;
		if (m_spFrameMgr->GetProperty(IGLibrary::IGProperties::IGPROPERTY_TEXTURE1, texture1) &&
			m_spFrameMgr->GetProperty(IGLibrary::IGProperties::IGPROPERTY_TEXTURE2, texture2)){
			if (eTexture == IGLibrary::IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1){
				GetFrame()->SetProperty (IGLibrary::IGProperties::IGPROPERTY_CURTEXTURE, texture1);
				GetFrame()->SetProperty (IGLibrary::IGProperties::IGPROPERTY_BKGNDTEXTURE, texture2);
			}
			else{
				GetFrame()->SetProperty (IGLibrary::IGProperties::IGPROPERTY_CURTEXTURE, texture2);
				GetFrame()->SetProperty (IGLibrary::IGProperties::IGPROPERTY_BKGNDTEXTURE, texture1);
			}
		}
	}
	virtual ~IGRequestFilter(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Filter layer";}
	virtual bool InternalProcess();
	virtual bool IsFullUpdatingFrame();
	int		m_nFilterId;
	int		m_nFaceEffectId;
	bool	m_bOriginal;
	COLORREF m_col1;
	COLORREF m_col2;
	double m_dParam1;
	double m_dParam2;
	double m_dParam3;
	std::wstring m_wsPicName1;
	std::wstring m_wsPicName2;
	std::wstring m_wsFaceDescriptor;
};

class IGRequestIndex : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_INDEX>
{
public:
	IGRequestIndex (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams)
		, m_pMarker (NULL)
		, m_pMarker2 (NULL)
	{	GetParameter (IGREQUEST_FILTERID, m_nIndexerId);
		int nMarkerType;
		GetParameter (IGREQUEST_MARKERTYPE, nMarkerType);
		m_eMarkerType = (IGLibrary::IGMarker::ENUM_IGMARKER)nMarkerType;
		GetParameter (IGREQUEST_MARKERTYPE2, nMarkerType);
		m_eMarkerType2 = (IGLibrary::IGMarker::ENUM_IGMARKER)nMarkerType;
		GetParameter (IGREQUEST_POINTS, m_lPts);	
		GetParameter (IGREQUEST_POINTS2, m_lPts2);
		if (m_lPts.size() >= 2 && m_lPts2.size() >= 2){
			POINT pt1Mk1, pt2Mk1, pt1Mk2, pt2Mk2;
			pt1Mk1.x = -1; pt1Mk1.y = -1; pt2Mk1.x = -1; pt2Mk1.y = -1;
			pt1Mk2.x = -1; pt1Mk2.y = -1; pt2Mk2.x = -1; pt2Mk2.y = -1;
			std::list <POINT>::const_iterator itPts;
			if (m_lPts.size() > 0){
				itPts = m_lPts.cbegin();
				pt1Mk1 = *itPts;
				if (m_lPts.size() > 1){
					pt2Mk1 = *++itPts;
				}
			}
			if (m_lPts2.size() > 0){
				itPts = m_lPts2.cbegin();
				pt1Mk2 = *itPts;
				if (m_lPts2.size() > 1){
					pt2Mk2 = *++itPts;
				}
			}
			m_pMarker = IGLibrary::IGMarker::Create (m_eMarkerType, 0, pt1Mk1, pt2Mk1.x, pt2Mk1.y);
			m_pMarker2 = IGLibrary::IGMarker::Create (m_eMarkerType2, 1, pt1Mk2, pt2Mk2.x, pt2Mk2.y);
		}
	}
	virtual ~IGRequestIndex(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Index layer";}
	virtual bool InternalProcess();
	virtual bool IsFullUpdatingFrame();

	int		m_nIndexerId;
	IGLibrary::IGMarker::ENUM_IGMARKER m_eMarkerType, m_eMarkerType2;
	std::list <POINT> m_lPts, m_lPts2;
	IGLibrary::IGMarker *m_pMarker, *m_pMarker2;
};

template <IGLibrary::IGImageProcessing::TYPE_IMAGE_PROC_ID IMAGE_PROC_ID, IGLibrary::EnumStepType STEP_ID = IGLibrary::IGFRAMEHISTORY_STEP_UNDEFINED>
class IGRequestLayerOutlineProcessing : public IGRequestLayerProcessing <IMAGE_PROC_ID, STEP_ID>
{
public:
	IGRequestLayerOutlineProcessing (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) 
		: IGRequestLayerProcessing <IMAGE_PROC_ID, STEP_ID> (pcwGuid, spRequestParams){
		bool bInit = GetParameter (IGREQUEST_POINTS, m_lPts);		
		_ASSERTE (bInit && "IGRequestLayerOutlineProcessing - Initialization error");
	}
	virtual ~IGRequestLayerOutlineProcessing(){}
protected:
	list <POINT> m_lPts;	// frame coordinate range = [0;1000000]
};

template <IGLibrary::IGImageProcessing::TYPE_IMAGE_PROC_ID IMAGE_PROC_ID, IGLibrary::EnumStepType STEP_ID = IGLibrary::IGFRAMEHISTORY_STEP_UNDEFINED>
class IGRequestDrawProcessing : public IGRequestLayerOutlineProcessing <IMAGE_PROC_ID, STEP_ID>
{
public:
	IGRequestDrawProcessing (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) 
		: IGRequestLayerOutlineProcessing <IMAGE_PROC_ID, STEP_ID> (pcwGuid, spRequestParams){
		GetParameter (IGREQUEST_TRANSPARENCY, m_nTransparency);
		int nBrushSize = -1;
		GetParameter (IGREQUEST_BRUSHSIZE, nBrushSize);
		IGLibrary::IGSingleton <IGLibrary::IGFrameManager>	spFrameMgr;
		spFrameMgr->SetProperty (IGLibrary::IGProperties::IGPROPERTY_BRUSHSIZE, nBrushSize);
	}
	virtual ~IGRequestDrawProcessing(){}
protected:
	int m_nTransparency;
};

class IGRequestDrawLines : public IGRequestDrawProcessing <IGLibrary::IGREQUEST_FRAME_DRAWLINES, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE>
{
public:
	IGRequestDrawLines (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) 
		: IGRequestDrawProcessing <IGLibrary::IGREQUEST_FRAME_DRAWLINES, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE> (pcwGuid, spRequestParams){
	}
	virtual ~IGRequestDrawLines(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Draw Lines";}
	virtual bool InternalProcess();
};

class IGRequestRubber : public IGRequestDrawProcessing <IGLibrary::IGREQUEST_FRAME_RUBBER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE>
{
public:
	IGRequestRubber (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) 
		: IGRequestDrawProcessing <IGLibrary::IGREQUEST_FRAME_RUBBER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE> (pcwGuid, spRequestParams){
	}
	virtual ~IGRequestRubber(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Rubber";}
	virtual bool InternalProcess();
};

class IGRequestTerminate : public IGRequest
{
public:
	IGRequestTerminate(LPCWSTR pcwGuid) : IGRequest (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, IGLibrary::IGREQUEST_WORKSPACE_TERMINATE){}
	virtual ~IGRequestTerminate(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Terminate";}
	virtual bool InternalProcess();
};

class IGRequestShow : public IGRequest
{
public:
	IGRequestShow(LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequest (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, IGLibrary::IGREQUEST_WORKSPACE_SHOW, spRequestParams)
	{	GetParameter (IGREQUEST_VISIBLE, m_wsIsVisible);
	}
	virtual ~IGRequestShow(){}
	bool IsVisible() {return (m_wsIsVisible == L"1");}
protected:
	virtual LPCWSTR InternalToString() {return L"Show";}
	virtual bool InternalProcess();
	wstring	m_wsIsVisible;
};

class IGRequestDestroyAccount : public IGRequest
{
public:
	IGRequestDestroyAccount(LPCWSTR pcwGuid) : IGRequest (pcwGuid, IGLibrary::IGREQUEST_WORKSPACE, IGLibrary::IGREQUEST_WORKSPACE_DESTROYACCOUNT){}
	virtual ~IGRequestDestroyAccount(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Destroy Account";}
	virtual bool InternalProcess();
};