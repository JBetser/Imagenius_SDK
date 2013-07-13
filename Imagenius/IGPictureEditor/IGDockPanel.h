// IGDockPanel.h : Declaration of the CIGDockPanel
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "_IIGDockPanelEvents_CP.h"
#include "resource.h"
#include "IGSimpleButton.h"
#include "IGCommunicable.h"	// Drag&drop, copy/paste

#include <gdiplus.h>
#include <string>
#include <list>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef enum {
	IGDOCKPANELSIDE_LEFT	=	1,
	IGDOCKPANELSIDE_RIGHT	=	2,
	IGDOCKPANELSIDE_TOP		=	3,
	IGDOCKPANELSIDE_BOTTOM	=	4	} IGDOCKPANELSIDE;

typedef enum {
	IGDOCKPANELMODE_CLOSED	=	1,
	IGDOCKPANELMODE_CLOSING	=	2,
	IGDOCKPANELMODE_WAIT	=	3,
	IGDOCKPANELMODE_POPUP	=	4	} IGDOCKPANELMODE;

#define IGDOCKPANEL_SIZE						100
#define IGDOCKPANEL_DOCKEDSIZE					15
#define IGDOCKPANEL_TRANSFERMOUSETRACKEVENT		0xFFFF
#define IGDOCKPANEL_COLOR_BACKGROUND			Color(255, 212, 214, 216)	// Gray
#define IGDOCKPANEL_COLORRGB_BACKGROUND			RGB(212, 214, 216)	// Gray

// _IIGDockPanelEvents
extern _ATL_FUNC_INFO EventPanelOpenInfo;

// CIGDockPanel
class ATL_NO_VTABLE CIGDockPanel :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGDockPanel, &IID_IIGDockPanel, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIGDockPanel>,
	public IOleControlImpl<CIGDockPanel>,
	public IOleObjectImpl<CIGDockPanel>,
	public IOleInPlaceActiveObjectImpl<CIGDockPanel>,
	public IViewObjectExImpl<CIGDockPanel>,
	public IOleInPlaceObjectWindowlessImpl<CIGDockPanel>,
	public IConnectionPointContainerImpl<CIGDockPanel>,
	public CProxy_IIGDockPanelEvents<CIGDockPanel>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CIGDockPanel, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CIGDockPanel, &CLSID_IGDockPanel>,
	public CComControl<CIGDockPanel>,
	public IGCommunicable
{
public:
	CIGDockPanel();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IGDOCKPANEL)


BEGIN_COM_MAP(CIGDockPanel)
	COM_INTERFACE_ENTRY(IIGDockPanel)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
END_COM_MAP()

BEGIN_PROP_MAP(CIGDockPanel)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGDockPanel)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGDockPanelEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CIGDockPanel)
	CHAIN_MSG_MAP(CComControl<CIGDockPanel>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

	STDMETHODIMP CreateFrame (OLE_HANDLE hWnd, LPRECT p_rc, LONG nSide, LONG nDockSize = IGDOCKPANEL_SIZE, LONG nStyle = 0);
	STDMETHODIMP SetString (BSTR bstrName);
	STDMETHODIMP DestroyFrame ();
	STDMETHODIMP Move (LPRECT p_rc);
	STDMETHODIMP GetHWND (OLE_HANDLE *p_hWnd);
	STDMETHODIMP GetClientRECT (LPRECT p_rc);
	STDMETHODIMP RegisterControls (OLE_HANDLE hWnd);
	STDMETHODIMP UnRegisterControls (OLE_HANDLE hWnd);
	STDMETHODIMP RedrawControls ();
	STDMETHODIMP AddDropTarget (IDropTarget *pDropTarget);

// IIGDockPanel
public:
	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di);


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	static LRESULT APIENTRY StaticHookPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
  	LRESULT HookPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	static LRESULT APIENTRY StaticHookPopupPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
  	LRESULT HookPopupPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	static LRESULT APIENTRY StaticHookTransferMouseEventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
  	LRESULT HookTransferMouseEventProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static BOOL CALLBACK StaticEnumRegisterChildProc (HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK StaticEnumUnregisterChildProc (HWND hwnd, LPARAM lParam);
	BOOL CALLBACK EnumChildProc (HWND hwnd, LPARAM lParam);

	static BOOL CALLBACK StaticEnumMoveChildProc (HWND hwnd, LPARAM lParam);
	BOOL CALLBACK EnumMoveChildProc (HWND hwnd);

	void openPopup();
	void closePopup();
	void draw (HDC hdc, PAINTSTRUCT *p_ps);
	void drawPopup (HDC hdc);
	void eraseBkgnd (HDC hdc, int nOffSet = 0);
	void movePopup ();
	void registerControls (HWND hWnd = NULL);
	void unregisterControls (HWND hWnd = NULL);
	bool contains (HWND hWnd);

	HWND				m_hParent;
	HWND				m_hPopupWnd;
	HWND				m_hInside;
	IGDOCKPANELSIDE		m_eSide;
	IGDOCKPANELMODE		m_eMode;
	RECT				m_rcDockPanel;
	int					m_nNbTicks;
	RECT				m_rcParent;
	RECT				m_rcButton;
	std::wstring		m_wsDockName;
	int					m_nDockSize;
	int					m_nNbMaxTicks;
	bool				m_bMouseInside;
	std::list <HWND>	m_lControls;
	bool				m_bDoNotLeave;
};

OBJECT_ENTRY_AUTO(__uuidof(IGDockPanel), CIGDockPanel)
