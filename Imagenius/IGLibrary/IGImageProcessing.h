#pragma once

#include <list>

#define IGREQUEST_UNREGISTERED		L"##Unregistered##"
#define IGREQUEST_PROCESSED			0xCACA	// succeeded and no more operation is needed, the message will be deleted just after
#define IGREQUEST_TOBEPOSTED		0xCACB	// needs the message to be posted before processing
#define IGREQUEST_PROCESSING		0xCACC	// the request has launched another thread which is currently being processed
#define IGREQUEST_FAILED			0xCACD	// the request failed
#define IGREQUEST_SUCCEEDED			0xCACE	// the request succeeded, some further operations may be applied

#define	IGIMAGEPROCESSING_LASTSAVEDPICTURE	L"LastSavedPicture.jpg"

namespace IGLibrary
{
	class IGImageProcMessage;
	class IGFrame;

// Imagenius requests
typedef enum {	IGREQUEST_UNDEFINED = 0,
				IGREQUEST_WORKSPACE = 100,
				IGREQUEST_FRAME = 101,
				IGREQUEST_SERVERMANAGER = 102 }	IGREQUEST_TYPE;

typedef enum {	IGREQUEST_UNDEFINED_ID = 0,
				IGREQUEST_WORKSPACE_CONNECT = 1000,
				IGREQUEST_WORKSPACE_PING = 1001,
				IGREQUEST_WORKSPACE_DISCONNECT = 1002,
				IGREQUEST_WORKSPACE_NEWIMAGE = 1003,
				IGREQUEST_WORKSPACE_LOADIMAGE = 1004,
				IGREQUEST_WORKSPACE_TERMINATE = 1005,
				IGREQUEST_WORKSPACE_SHOW = 1006,
				IGREQUEST_WORKSPACE_DESTROYACCOUNT = 1007,

				IGREQUEST_FRAME_SAVE = 2000,
				IGREQUEST_FRAME_ADDLAYER = 2001,
				IGREQUEST_FRAME_REMOVELAYER = 2002,
				IGREQUEST_FRAME_MOVELAYER = 2003,
				IGREQUEST_FRAME_MERGELAYER = 2004,
				IGREQUEST_FRAME_FILTER = 2005,
				IGREQUEST_FRAME_CHANGEPROPERTY = 2006,
				IGREQUEST_FRAME_CLOSE = 2007,
				IGREQUEST_FRAME_UPDATE = 2008,
				IGREQUEST_FRAME_SELECT = 2009,
				IGREQUEST_FRAME_SETLAYERVISIBLE = 2010,
				IGREQUEST_FRAME_GOTOHISTORY = 2011,
				IGREQUEST_FRAME_DRAWLINES = 2012,
				IGREQUEST_FRAME_PICKCOLOR = 2013,
				IGREQUEST_FRAME_INDEX = 2014,
				IGREQUEST_FRAME_COPY = 2015,
				IGREQUEST_FRAME_CUT = 2016,
				IGREQUEST_FRAME_PASTE = 2017,
				IGREQUEST_FRAME_DELETE = 2018,
				IGREQUEST_FRAME_SMARTDELETE = 2019,
				IGREQUEST_FRAME_MOVE = 2020,
				IGREQUEST_FRAME_ROTATEANDRESIZE = 2021,
				IGREQUEST_FRAME_SELECTLAYER = 2022,
				IGREQUEST_FRAME_RUBBER = 2023,
				IGREQUEST_FRAME_SAVE_BM = 2024}	IGREQUEST_ID;

// common base for IGRequest and IGAnswer
class IGCommunicationBase
{
	friend class IGImageProcMessage;
	friend class IGImageProcessing;
public:
	IGCommunicationBase() : m_nId (0), m_eType (IGREQUEST_UNDEFINED), m_wsGuid (IGREQUEST_UNREGISTERED), m_eStepId (IGFRAMEHISTORY_STEP_UNDEFINED), m_nFirstHistoryParam (-1), m_nSecondHistoryParam (-1)	{}
	IGCommunicationBase (LPCWSTR pcwGuid, int nId, IGLibrary::IGREQUEST_TYPE eType) : m_nId (nId), m_eType (eType), m_wsGuid (pcwGuid), m_eStepId (IGFRAMEHISTORY_STEP_UNDEFINED), m_nFirstHistoryParam (-1), m_nSecondHistoryParam (-1) {}
	IGCommunicationBase (const IGCommunicationBase&);
	virtual ~IGCommunicationBase() {};

	static bool IsNumber(LPCWSTR pcwStr);

	bool IsWorkspace() const;
	bool IsFrame() const;
	int GetId() const;
	std::wstring GetGuid() const;
	std::wstring GetUser() const;
	std::wstring GetFrameName() const;
	int GetLayerId() const;
	std::wstring GetFrameGuid() const {return m_wsFrameGuid;}
	int GetFramePos();
	EnumStepType GetStepId() const {return m_eStepId;}
	// history parameters
	int GetFirstHistoryParam() const {return m_nFirstHistoryParam;}
	int GetSecondHistoryParam() const {return m_nSecondHistoryParam;}

	virtual bool IsUpdatingFrame() const;
	virtual bool IsFullUpdatingFrame();
	virtual bool IsLayer() const;
	virtual bool IsAsync() const = 0;	
	virtual LRESULT PostProcessing (IGImageProcMessage *pMessage) {return IGREQUEST_SUCCEEDED;}
	virtual IGCommunicationBase* Clone() const = 0;	
	virtual IGLibrary::IGFrame *GetFrame() {return NULL;}

	inline IGREQUEST_TYPE GetRequestType() {return m_eType;}
	
protected:
	virtual bool InternalProcess() = 0; 
	virtual bool IsBusy() {return false;}
	virtual LPCWSTR InternalToString() = 0;
	virtual void WriteParams() = 0;
	virtual void OnProgress (int nProgress, LPCWSTR pwMessage) = 0;
	virtual bool SimpleThreadProcess() = 0;	
	virtual bool Initialize() {return true;}
	
	int							m_nId;
	IGREQUEST_TYPE				m_eType;
	std::wstring				m_wsGuid;
	std::wstring				m_wsFrameGuid;
	std::wstring				m_wsUser;
	std::wstring				m_wsFrameName;
	int							m_nLayerId;
	EnumStepType				m_eStepId;
	int							m_nFirstHistoryParam;
	int							m_nSecondHistoryParam;
};

class IGImageProcessingCallback
{
	// Class responsible for firing this callback event
	friend class CxImageThread;
	friend class IGImageProcMessage;
public:
	virtual LPCWSTR GetName() const = 0;
	virtual EnumStepType GetStepId() const = 0;

protected:
	virtual ~IGImageProcessingCallback() {};

	virtual bool OnImageProcessing() = 0;
	virtual void OnFinished() = 0;
	virtual int GetFirstParam() const = 0;
	virtual int GetSecondParam() const = 0;
};

class IGImageProcessing : public IGImageProcessingCallback
{
public:
	typedef IGREQUEST_ID TYPE_IMAGE_PROC_ID;

	IGImageProcessing (IGFrame *pFrame, const wchar_t *pcwXml, HWND hProgress = NULL);
	IGImageProcessing (IGFrame *pFrame, IGImageProcMessage *pMessage, HWND hProgress = NULL);

	static void SetCurrentRequest (IGCommunicationBase *pRequest);
	static void UnregisterRequest (IGCommunicationBase *pRequest);
	static std::wstring GetCurrentRequestGUID();
	static void GetDZtoIGRatios (int nWidth, int nHeight, double& dRatioX, double& dRatioY);
	static void GetIGtoDZRatios (int nWidth, int nHeight, double& dRatioX, double& dRatioY);

	// Split the progress range from (0, 100) to (nBegin, nEnd)
	void SetProgressRange (int nBegin, int nEnd);
	bool Start (bool bAsync = true);
	bool GenerateOutput (CxImage& image, IGCommunicationBase& request, LPCWSTR pcwOutput);
	virtual LPCWSTR GetName() const;
	
protected:
	// Image processing thread is responsible for destroying the object
	virtual ~IGImageProcessing();

	// Image processing thread callbacks
	// overloaded by children:	
	virtual EnumStepType GetStepId() const;
	virtual bool OnImageProcessing();	
	virtual bool OnImageProcessing (CxImage& image, IGImageProcMessage& message);
	virtual bool OnGenerateOutput (CxImage& image, IGImageProcMessage& message, LPCWSTR pcwOutput);
	virtual void OnFinished () {};
	virtual int GetFirstParam (IGImageProcMessage& message, CxImage& image) const {return -1;};
	virtual int GetSecondParam (IGImageProcMessage& message, CxImage& image) const {return -1;};
	
	// Image processing thread datas
	IGImageProcMessage				*m_pMessage;
	IGFrame							*m_pFrame;

private:
	// progress window subclass
	static LRESULT APIENTRY StaticSubclassProgressProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
														UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT SubclassProgressProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Image processing thread callbacks
	// called from CxImageThread, IGFrameHistory:
	virtual int GetFirstParam() const;
	virtual int GetSecondParam() const;
	
	static float computeOutputSizeAndRatio (int& nOutputWidth, int& nOutputHeight, int nFrameWidth, int nFrameHeight);

	static IGCommunicationBase	*g_pCurrentRequest;

	// Progress bar datas	
	IGCommunicationBase				*m_pRequest; // Request duplicated datas (for progression)
	int								m_nPos;
	int								m_nLastProgress;
	int								m_nBegin;
	int								m_nEnd;
};

}