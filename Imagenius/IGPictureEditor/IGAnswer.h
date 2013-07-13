#pragma once
#include <list>
#include <string>
#include <msxml2.h>
#include <IGFrameManager.h>
#include <IGSingleton.h>
#include <IGImageProcessing.h> // Imagenius requests
#include "IGMainWindow.h"
#include "IGRequest.h"

typedef enum {	IGANSWER_WORKSPACE_CONNECTED = 3000,
				IGANSWER_WORKSPACE_DISCONNECTED = 3001,
				IGANSWER_WORKSPACE_ACTIONFAILED	= 3002,
				IGANSWER_WORKSPACE_PONG = 3003,
				IGANSWER_WORKSPACE_HEARTHBEAT = 3004,
				IGANSWER_WORKSPACE_PROPERTYCHANGED = 3005,
				IGANSWER_FRAME_CHANGED = 4000,
				IGANSWER_FRAME_ACTIONDONE = 4001,
				IGANSWER_FRAME_ACTIONFAILED	= 4002,
				IGANSWER_FRAME_PROGRESS	= 4003 } IGANSWER_ID;

#define IGANSWER_FRAMEACTION		L"Action"
#define IGANSWER_PROGRESS			L"Progress"
#define IGANSWER_SERVERIP			L"ServerIP"
#define IGANSWER_MESSAGE			L"Message"
#define IGANSWER_USEROUTPUT			L"UserOutput"
#define IGANSWER_IMAGELIBRARY		L"ImageLibrary"
#define IGANSWER_NBLAYERS			L"NbLayers"
#define IGANSWER_LAYERVISIBILITY	L"LayerVisibility"
#define IGANSWER_STEPIDS			L"StepIds"
#define IGANSWER_FRAMEPROPERTIES	L"FrameProperties"
#define IGANSWER_FRAMENAMES			L"FrameNames"
#define IGANSWER_WORKSPACEPROPERTIES	L"WorkspaceProperties"
#define IGANSWER_ALLNBLAYERS		L"AllNbLayers"
#define IGANSWER_ALLLAYERVISIBILITY	L"AllLayerVisibility"
#define IGANSWER_ALLSTEPIDS			L"AllStepIds"
#define IGANSWER_SELECTEDFRAMEID	L"SelectedFrameId"

class IGAnswer : public IGRequest
{
	friend class CIGSocket;
public:
	IGAnswer (IGLibrary::IGREQUEST_TYPE eType, IGANSWER_ID eAnswerId);
	IGAnswer (const IGCommunicationBase& request, IGANSWER_ID eAnswerId);
	virtual ~IGAnswer();

	static bool NotifySuccess (IGLibrary::IGImageProcMessage *pMessage);
	static bool NotifySuccess (IGRequest *pRequest);
	static bool NotifyFailure (IGCommunicationBase *pRequest);
	bool Write();

	virtual bool IsAsync() const {return false;}

protected:
	virtual void WriteParams() {}
	virtual bool InternalProcess() {return false;}
	
	void InternalWriteParams() {}
	bool TransferParameter (const IGCommunicationBase& request, LPCWSTR pcwParamName);
	virtual bool SetParameter (const wchar_t *pwParamName, int nParamValue);
	virtual bool SetParameter (const wchar_t *pwParamName, const std::wstring& wsParamValue);
	virtual bool Initialize();

private:
	static bool process (IGRequest& request, int nImProcId = -1);

	static IGLibrary::IGThreadSafeData <std::list <std::string>>		g_safelAnswerBuffer;
	IGANSWER_ID				m_eAnswerId;
	CComPtr <IXMLDOMNode>	m_spXMLAnswer;
};

class IGAnswerWorkspaceConnected : public IGAnswer
{
public:
	IGAnswerWorkspaceConnected (const IGCommunicationBase& request);
	virtual ~IGAnswerWorkspaceConnected() {}
	virtual LPCWSTR InternalToString() {return L"Workspace Connected";}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswer::InternalWriteParams();
		SetParameter (IGREQUEST_RESTOREDFRAMEIDS, m_wsFrameIds);
		SetParameter (IGANSWER_FRAMEPROPERTIES, m_wsFrameProps);
		SetParameter (IGANSWER_FRAMENAMES, m_wsFrameNames);
		SetParameter (IGANSWER_ALLNBLAYERS, m_wsFrameNbLayers);
		SetParameter (IGANSWER_ALLLAYERVISIBILITY, m_wsFrameVisibility);
		SetParameter (IGANSWER_ALLSTEPIDS, m_wsFrameStepIds);
		SetParameter (IGANSWER_SELECTEDFRAMEID, m_wsActiveFrameId);
		SetParameter (IGANSWER_IMAGELIBRARY, m_wsConvertMiniPictures);
		SetParameter (IGANSWER_WORKSPACEPROPERTIES, m_wsWorkspaceProperties);
	}
	std::wstring	m_wsFrameIds;
	std::wstring	m_wsFrameProps;
	std::wstring	m_wsFrameNames;
	std::wstring	m_wsFrameNbLayers;
	std::wstring	m_wsFrameVisibility;
	std::wstring	m_wsFrameStepIds;
	std::wstring	m_wsActiveFrameId;
	std::wstring	m_wsConvertMiniPictures;
	std::wstring	m_wsWorkspaceProperties;
};

class IGAnswerWorkspaceDisconnected : public IGAnswer
{
public:
	IGAnswerWorkspaceDisconnected (const IGCommunicationBase& request) : IGAnswer (request, IGANSWER_WORKSPACE_DISCONNECTED) {}
	virtual ~IGAnswerWorkspaceDisconnected() {}
	virtual LPCWSTR InternalToString() {return L"Workspace Disconnected";}
};

class IGAnswerWorkspaceActionFailed : public IGAnswer
{
public:
	IGAnswerWorkspaceActionFailed (const IGCommunicationBase& request) : IGAnswer (request, IGANSWER_WORKSPACE_ACTIONFAILED) {}
	virtual ~IGAnswerWorkspaceActionFailed() {}
	virtual LPCWSTR InternalToString() { return L"Action Failed";}
};

class IGAnswerWorkspacePong : public IGAnswer
{
public:
	IGAnswerWorkspacePong (const IGCommunicationBase& request) : IGAnswer (request, IGANSWER_WORKSPACE_PONG) {}
	virtual ~IGAnswerWorkspacePong() {}
	virtual LPCWSTR InternalToString() {return L"Pong";}
};

class IGAnswerFrame : public IGAnswer
{
public:
	IGAnswerFrame (IGRequest& request, IGANSWER_ID nAnswerId);
	virtual ~IGAnswerFrame() {}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswer::InternalWriteParams();
		SetParameter (IGREQUEST_FRAMEID, m_wsFrameGuid);
		SetParameter (IGREQUEST_PICTURENAME, m_wsFrameName);
	}
	std::wstring		m_wsFrameId;
};

class IGAnswerFrameWithProps : public IGAnswerFrame
{
public:
	IGAnswerFrameWithProps (IGRequest& request, IGANSWER_ID nAnswerId);
	virtual ~IGAnswerFrameWithProps() {}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswerFrame::InternalWriteParams();
		SetParameter (IGANSWER_STEPIDS, m_wsStepIds);
		SetParameter (IGANSWER_WORKSPACEPROPERTIES, m_wsWkspceProperties);
		SetParameter (IGANSWER_FRAMEPROPERTIES, m_wsProperties);
	}
	std::wstring		m_wsStepIds;
	std::wstring		m_wsWkspceProperties;
	std::wstring		m_wsProperties;
};

class IGAnswerFrameChanged : public IGAnswerFrameWithProps
{
public:
	IGAnswerFrameChanged (IGRequest& request);
	virtual ~IGAnswerFrameChanged() {}
	virtual LPCWSTR InternalToString() {return L"Frame Changed";}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswerFrameWithProps::InternalWriteParams();
		SetParameter (IGANSWER_NBLAYERS, m_nNbLayers);
		SetParameter (IGANSWER_LAYERVISIBILITY, m_wsLayerVisibility);	
	}
	int m_nNbLayers;
	std::wstring m_wsLayerVisibility;	
};

class IGAnswerActionDone : public IGAnswerFrameWithProps
{
public:
	IGAnswerActionDone (IGRequest& request, IGANSWER_ID nAnswerId = IGANSWER_FRAME_ACTIONDONE) : IGAnswerFrameWithProps (request, nAnswerId) {
		IGLibrary::IGFrame *pFrame = request.GetFrame();
		if (pFrame)
			m_wsLastOutputPath = pFrame->GetLastOutputPath();
	}
	virtual ~IGAnswerActionDone() {}
	virtual LPCWSTR InternalToString() { return L"Action Done";}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswerFrameWithProps::InternalWriteParams();
		SetParameter (IGANSWER_FRAMEACTION, GetId());
		SetParameter (IGREQUEST_PATH, m_wsLastOutputPath);
	}
	std::wstring m_wsLastOutputPath;
};

class IGAnswerPropertyChanged : public IGAnswerFrameWithProps
{
public:
	IGAnswerPropertyChanged (IGRequest& request);
	virtual ~IGAnswerPropertyChanged() {}
	virtual LPCWSTR InternalToString() {return L"Property Changed";}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswerFrameWithProps::InternalWriteParams();
		SetParameter (IGREQUEST_ISFRAME, m_bIsFrame ? L"1" : L"0");
		SetParameter (m_bIsFrame ? IGANSWER_FRAMEPROPERTIES : IGANSWER_WORKSPACEPROPERTIES, m_wsProperties);
	}
	std::wstring m_wsProperties;
	bool		m_bIsFrame;
};

class IGAnswerFrameActionFailed : public IGAnswerActionDone
{
public:
	IGAnswerFrameActionFailed (IGRequestFrame& request) : IGAnswerActionDone (request, IGANSWER_FRAME_ACTIONFAILED) {}
	virtual ~IGAnswerFrameActionFailed() {}
	virtual LPCWSTR InternalToString() { return L"Action Failed";}
};

class IGAnswerHearthbeat : public IGAnswer
{
public:
	IGAnswerHearthbeat() : IGAnswer(IGLibrary::IGREQUEST_WORKSPACE, IGANSWER_WORKSPACE_HEARTHBEAT)
	{
		m_wsUser = m_spConfigManager->GetUserLogin (true);
	}
	virtual ~IGAnswerHearthbeat() {}
	virtual LPCWSTR InternalToString() { return L"Hearthbeat";}
protected:
	virtual void WriteParams() {InternalWriteParams();}
	void InternalWriteParams()
	{
		IGAnswer::InternalWriteParams();
		SetParameter (IGANSWER_SERVERIP, m_spConfigManager->GetServerIP());
	}
};

template <class REQUEST_CLASS, class ANSWER_CLASS>
class IGAnswerProgress : public ANSWER_CLASS
{
public:
	IGAnswerProgress (REQUEST_CLASS& request, int nProgress, const wchar_t *pcwMessage) : ANSWER_CLASS (request, IGANSWER_FRAME_PROGRESS){
		m_nProgress = nProgress;
		m_wsMessage = pcwMessage;
	}
	virtual ~IGAnswerProgress() {}
	virtual LPCWSTR InternalToString() { return IGANSWER_PROGRESS; }
protected:
	virtual void WriteParams() {
		InternalWriteParams();
		wchar_t wProgress [32];
		::_itow (m_nProgress, wProgress, 10);
		SetParameter (IGANSWER_PROGRESS, wProgress);
		SetParameter (IGANSWER_MESSAGE, m_wsMessage.c_str());
	}
private:
	int		m_nProgress;
	std::wstring m_wsMessage;
};