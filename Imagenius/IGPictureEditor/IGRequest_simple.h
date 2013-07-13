#pragma once

// definition of regular "straight" thread requests, i.e. requests that do not need to communicate with the main thread
class IGRequestAddLayer : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_ADDLAYER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERADD>
{
public:
	IGRequestAddLayer (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		IGLibrary::IGFrame *pFrame = GetFrame();
		if (pFrame)
			m_nFirstHistoryParam = pFrame->GetNbLayers();
	}
	virtual ~IGRequestAddLayer(){}
	virtual LRESULT PostProcessing (IGLibrary::IGImageProcMessage *pMessage);
protected:
	virtual LPCWSTR InternalToString() {return L"Add Layer";}
	virtual bool SimpleThreadProcess();
};

class IGRequestRemoveLayer : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_REMOVELAYER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERREMOVE>
{
public:
	IGRequestRemoveLayer (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams){
		GetParameter (IGREQUEST_LAYERID, m_wsLayerIds);
		m_nFirstHistoryParam = ::_wtoi (m_wsLayerIds.substr (0, m_wsLayerIds.find (L',')).c_str());
	}
	virtual ~IGRequestRemoveLayer(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Remove Layer";}
	virtual bool SimpleThreadProcess();
	wstring m_wsLayerIds;
};

class IGRequestUpdateFrame : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_UPDATE>
{
public:
	IGRequestUpdateFrame (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams){}
	virtual ~IGRequestUpdateFrame(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Update frame";}
};

class IGRequestMoveLayer : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_MOVELAYER, IGLibrary::IGFRAMEHISTORY_STEP_LAYERMOVE>
{
public:
	IGRequestMoveLayer (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams)
	{
		GetParameter (IGREQUEST_LAYERIDTO, m_nLayerDestId);
		m_nFirstHistoryParam = m_nLayerId;
		m_nSecondHistoryParam = m_nLayerDestId;
	}
	virtual ~IGRequestMoveLayer(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Move Layer";}
	virtual bool SimpleThreadProcess();
	int		m_nLayerDestId;
};

class IGRequestMove : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_MOVE>
{
public:
	IGRequestMove (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams)	{}
	virtual ~IGRequestMove(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Move";}
	virtual bool SimpleThreadProcess();
	virtual bool Initialize();
	POINT	m_ptVector;
	RECT	m_rcSubLayer;
	RECT	m_rcLayerPosAndVector;
};

class IGRequestRotateAndResize : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_ROTATEANDRESIZE, IGLibrary::IGFRAMEHISTORY_STEP_ROTATEANDRESIZE>
{
public:
	IGRequestRotateAndResize (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams)	{}
	virtual ~IGRequestRotateAndResize(){}
protected:
	virtual LPCWSTR InternalToString() {return L"RotateAndResize";}
	virtual bool SimpleThreadProcess();
	virtual bool Initialize();
	POINT m_ptVector;
	RECT	m_rcSubLayer;
	float m_fRate;
	RECT	m_rcLayerPosAndVector;
};

class IGRequestSetLayerVisible : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_SETLAYERVISIBLE>
{
public:
	IGRequestSetLayerVisible (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams)
	{
		GetParameter (IGREQUEST_VISIBLE, m_wsIsVisible);
	}
	virtual ~IGRequestSetLayerVisible(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Set layer visibility";}
	virtual bool SimpleThreadProcess();
	wstring		m_wsIsVisible;
};

class IGRequestSelectLayer : public IGRequestLayerProcessing <IGLibrary::IGREQUEST_FRAME_SELECTLAYER, IGLibrary::IGFRAMEHISTORY_STEP_SELECTION>
{
public:
	IGRequestSelectLayer (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerProcessing (pcwGuid, spRequestParams) {
		int nCurSelectedId = GetFrame()->GetLayerPos(GetFrame()->GetWorkingLayer());
		m_bFull = (GetFrame()->GetWorkingLayer()->SelectionIsValid() && m_nLayerId != nCurSelectedId);
		m_nSelectedLayerId = m_nLayerId;
		m_nLayerId = nCurSelectedId;
		if (!m_bFull)
			m_eStepId = IGLibrary::IGFRAMEHISTORY_STEP_UNDEFINED;
	}
	virtual ~IGRequestSelectLayer(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Select layer";}
	virtual bool Initialize();
	virtual bool SimpleThreadProcess();
	virtual bool IsFullUpdatingFrame();
	virtual bool IsUpdatingFrame() const {return m_bFull;}
	bool m_bFull;
	int m_nSelectedLayerId;
	list <POINT> m_lEmpty;	// just for clear selection format compliance
	IGLibrary::SELECTIONPARAMS m_selParams;
};

class IGRequestSelectFrame : public IGRequestLayerOutlineProcessing <IGLibrary::IGREQUEST_FRAME_SELECT, IGLibrary::IGFRAMEHISTORY_STEP_SELECTION>
{
public:
	IGRequestSelectFrame (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestLayerOutlineProcessing (pcwGuid, spRequestParams)
																					, m_bFullUpdate(false)	{}
	virtual ~IGRequestSelectFrame(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Select region";}
	virtual bool SimpleThreadProcess();
	virtual bool IsFullUpdatingFrame();
	virtual bool Initialize();
	IGLibrary::SELECTIONPARAMS m_selParams;
	bool m_bFullUpdate;
};

class IGRequestGotoHistory : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_GOTOHISTORY>
{
public:
	IGRequestGotoHistory (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) 
		: IGRequestImageProcessing (pcwGuid, spRequestParams){
		GetParameter (IGREQUEST_HISTORYID, m_nHistoryId);
	}
	virtual ~IGRequestGotoHistory(){}
	virtual bool IsFullUpdatingFrame() {return true;}
protected:
	virtual LPCWSTR InternalToString() {return L"Goto history";}
	virtual bool SimpleThreadProcess();
	int		m_nHistoryId;
};

class IGRequestChangeProperty : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_CHANGEPROPERTY>
{
public:
	IGRequestChangeProperty (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) 
		: IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_CHANGEPROPERTY> (pcwGuid, spRequestParams){
		int nIsFrame = 0;
		GetParameter (IGREQUEST_ISFRAME, nIsFrame);
		if (nIsFrame == 0)
			m_eType = IGLibrary::IGREQUEST_WORKSPACE;
	}
	virtual ~IGRequestChangeProperty(){}
protected:
	virtual bool InternalProcess() {
		if (IsFrame())
			return IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_CHANGEPROPERTY>::InternalProcess();
		else
			return SimpleThreadProcess();
	}
	virtual bool IsAsync() const { return IsFrame();}
	virtual LPCWSTR InternalToString() {return L"Change frame property";}
	virtual bool SimpleThreadProcess();
	virtual bool Answer();
};

class IGRequestPickColor : public IGRequestDrawProcessing <IGLibrary::IGREQUEST_FRAME_PICKCOLOR>
{
public:
	IGRequestPickColor (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams);
	virtual ~IGRequestPickColor(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Pick Color";}
	virtual bool SimpleThreadProcess();
	virtual bool IsUpdatingFrame() const {return false;}
	virtual bool Answer();
	RGBQUAD m_rgbColor;
	POINT m_ptCoords;
};

class IGRequestCopy : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_COPY>
{
public:
	IGRequestCopy (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{}
	virtual ~IGRequestCopy(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Copy";}
	virtual bool IsUpdatingFrame() const {return false;}
	virtual bool SimpleThreadProcess();
};

class IGRequestCut : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_CUT, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE>
{
public:
	IGRequestCut (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		IGLibrary::IGFrame *pFrame = GetFrame();
		if (pFrame)
			m_nFirstHistoryParam = pFrame->GetLayerPos(pFrame->GetWorkingLayer());
		ZeroMemory (&m_rcSubLayer, sizeof (RECT));
		if (pFrame->GetWorkingLayer())
			pFrame->GetWorkingLayer()->SelectionGetBox (m_rcSubLayer);
		if ((m_rcSubLayer.right == 0) || (m_rcSubLayer.top == 0))
		{
			m_rcSubLayer.right = -1;
			m_rcSubLayer.top = -1;
		}
		m_nSecondHistoryParam = (int)&m_rcSubLayer;
	}
	virtual ~IGRequestCut(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Cut";}
	virtual bool SimpleThreadProcess();
	RECT m_rcSubLayer;
};

class IGRequestPaste : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_PASTE, IGLibrary::IGFRAMEHISTORY_STEP_LAYERADD>
{
public:
	IGRequestPaste (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		IGLibrary::IGFrame *pFrame = GetFrame();
		if (pFrame)
			m_nFirstHistoryParam = pFrame->GetNbLayers();
	}
	virtual ~IGRequestPaste(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Paste";}
	virtual bool SimpleThreadProcess();
};

class IGRequestDelete : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_DELETE, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE>
{
public:
	IGRequestDelete (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		IGLibrary::IGFrame *pFrame = GetFrame();
		if (pFrame)
			m_nFirstHistoryParam = pFrame->GetLayerPos(pFrame->GetWorkingLayer());
		ZeroMemory (&m_rcSubLayer, sizeof (RECT));
		if (pFrame->GetWorkingLayer())
			pFrame->GetWorkingLayer()->SelectionGetBox (m_rcSubLayer);
		if ((m_rcSubLayer.right == 0) || (m_rcSubLayer.top == 0))
		{
			m_rcSubLayer.right = -1;
			m_rcSubLayer.top = -1;
		}
		m_nSecondHistoryParam = (int)&m_rcSubLayer;
	}
	virtual ~IGRequestDelete(){}
protected:
	virtual LPCWSTR InternalToString() {return L"Delete";}
	virtual bool SimpleThreadProcess();
	RECT m_rcSubLayer;
};

class IGRequestSmartDelete : public IGRequestImageProcessing <IGLibrary::IGREQUEST_FRAME_DELETE, IGLibrary::IGFRAMEHISTORY_STEP_LAYERCHANGE>
{
public:
	IGRequestSmartDelete (LPCWSTR pcwGuid, CComPtr <IXMLDOMNode> spRequestParams) : IGRequestImageProcessing (pcwGuid, spRequestParams)
	{
		IGLibrary::IGFrame *pFrame = GetFrame();
		if (pFrame)
			m_nFirstHistoryParam = pFrame->GetLayerPos(pFrame->GetWorkingLayer());
		ZeroMemory (&m_rcSubLayer, sizeof (RECT));
		if (pFrame->GetWorkingLayer())
			pFrame->GetWorkingLayer()->SelectionGetBox (m_rcSubLayer);
		if ((m_rcSubLayer.right == 0) || (m_rcSubLayer.top == 0))
		{
			m_rcSubLayer.right = -1;
			m_rcSubLayer.top = -1;
		}
		m_nSecondHistoryParam = (int)&m_rcSubLayer;
	}
	virtual ~IGRequestSmartDelete(){}
protected:
	virtual LPCWSTR InternalToString() {return L"SmartDelete";}
	virtual bool SimpleThreadProcess();
	RECT m_rcSubLayer;
};