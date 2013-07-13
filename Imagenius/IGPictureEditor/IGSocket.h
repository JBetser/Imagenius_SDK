// IGSocket.h : Declaration of the CIGSocket

#pragma once
#include "resource.h"       // main symbols

#include <list>
#include "IGPictureEditor_i.h"

#define IGSOCKET_MAXREQUESTLEN		32768
#define IGSOCKET_MAXANSWERLEN		1048576
#define IGSOCKET_MAXNBANSWERS		3

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef struct
{
	LONG nSize;
	char cXml [IGSOCKET_MAXREQUESTLEN];
} IGSOCKET_WRITE;

#define IGREQUESTQUEUE_MAXNUMBER	32
#define IGSOCKET_TIMER_INTERVAL		100	

#ifdef DEBUG
#define IGSOCKET_TIMER_CONNECTION_TIMEOUT	3600000
#else
#define IGSOCKET_TIMER_CONNECTION_TIMEOUT	5000
#endif

#define IGSOCKET_HEARTHBEAT_TIME			2000
#define IGSOCKET_HEARTHBEAT_NBTIMERTICKS	(IGSOCKET_HEARTHBEAT_TIME / IGSOCKET_TIMER_INTERVAL)

// CIGSocket
class ATL_NO_VTABLE CIGSocket :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIGSocket, &CLSID_IGSocket>,
	public IDispatchImpl<IIGSocket, &IID_IIGSocket, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CIGSocket();
	~CIGSocket();

	STDMETHODIMP CreateConnection (OLE_HANDLE hWndApp, SHORT nInputPortId, BSTR bstrServerIp);
	STDMETHODIMP DestroyConnection();

DECLARE_REGISTRY_RESOURCEID(IDR_IGSOCKET)


BEGIN_COM_MAP(CIGSocket)
	COM_INTERFACE_ENTRY(IIGSocket)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	static LRESULT APIENTRY StaticHookAppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookAppWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	bool listenOnPort (SHORT nPortId);
	bool processRequests();
	bool processAnswers();
	void harakiri();

	SOCKET						m_socket;
	SOCKADDR_IN					m_socketInfo;
	HWND						m_hWndApplication;
	std::list<std::string>		m_lsRequests;
	std::string					m_sCurRequest;
	CComBSTR					m_spbstrServerIp;
	int							m_nInputPortId;
	bool						m_bIsConnected;
	int							m_nTimeOutTicks;
	int							m_nHearthbeatTicks;
};

OBJECT_ENTRY_AUTO(__uuidof(IGSocket), CIGSocket)
