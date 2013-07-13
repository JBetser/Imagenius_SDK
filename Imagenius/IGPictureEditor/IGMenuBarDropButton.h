// IGMenuBarDropButton.h : Declaration of the CIGMenuBarDropButton
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "_IIGMenuBarDropButtonEvents_CP.h"
#include "ximage.h"

#include <string>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CIGMenuBarDropButton
class ATL_NO_VTABLE CIGMenuBarDropButton :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGMenuBarDropButton, &IID_IIGMenuBarDropButton, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIGMenuBarDropButton>,
	public IOleControlImpl<CIGMenuBarDropButton>,
	public IOleObjectImpl<CIGMenuBarDropButton>,
	public IOleInPlaceActiveObjectImpl<CIGMenuBarDropButton>,
	public IViewObjectExImpl<CIGMenuBarDropButton>,
	public IOleInPlaceObjectWindowlessImpl<CIGMenuBarDropButton>,
	public IConnectionPointContainerImpl<CIGMenuBarDropButton>,
	public CProxy_IIGMenuBarDropButtonEvents<CIGMenuBarDropButton>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CIGMenuBarDropButton, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CIGMenuBarDropButton, &CLSID_IGMenuBarDropButton>,
	public CComControl<CIGMenuBarDropButton>
{
public:
	CContainedWindow m_ctlButton;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CIGMenuBarDropButton();
	virtual ~CIGMenuBarDropButton();

	// COM methods
	STDMETHODIMP CreateFrame(OLE_HANDLE hWnd, LPRECT prc, BSTR xmlConfig, VARIANT_BOOL bIsMainMenu, LONG idxButton = -1, LONG idxSubMenu = -1);
	STDMETHODIMP DestroyFrame();	
	STDMETHODIMP UnSelect();
	STDMETHODIMP Select();
	STDMETHODIMP Enable();
	STDMETHODIMP Disable();

	// COM properties	
	STDMETHODIMP get_IdxSubMenu(LONG *p_idxSubMenu);
	STDMETHODIMP get_IdxButtonClick(LONG *p_idxButtonClick);
	STDMETHODIMP put_Font(OLE_HANDLE hFont);	

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_ACTSLIKEBUTTON |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IGMENUBARDROPBUTTON)


BEGIN_COM_MAP(CIGMenuBarDropButton)
	COM_INTERFACE_ENTRY(IIGMenuBarDropButton)
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
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(CIGMenuBarDropButton)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGMenuBarDropButton)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGMenuBarDropButtonEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CIGMenuBarDropButton)
	MESSAGE_HANDLER(WM_COMMAND,OnCommand)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	COMMAND_CODE_HANDLER(BN_CLICKED, OnBNClicked)
	CHAIN_MSG_MAP(CComControl<CIGMenuBarDropButton>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed Button
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnBNClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = CComControl<CIGMenuBarDropButton>::OnSetFocus(uMsg, wParam, lParam, bHandled);
		if (m_bInPlaceActive)
		{
			if(!IsChild(::GetFocus()))
				m_ctlButton.SetFocus();
		}
		return 0;
	}

	HRESULT OnDraw(ATL_DRAWINFO& di);

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CIGMenuBarDropButton>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		::SetWindowPos(m_ctlButton.m_hWnd, NULL, 0,
			0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
		return S_OK;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IIGMenuBarDropButton

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	static LRESULT APIENTRY StaticHookButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void draw (HDC hDC);
	void redraw (HWND hWnd);
	void drawIcon (HDC hdc);

	int m_nIdClickEvent;
	bool m_bIsMainMenu;
	int m_idxSubMenu;
	int m_nMode;
	HWND m_hMenuBarParent;
	std::wstring m_wsButtonText;
	bool m_bSubMenuOpened;
	bool m_bEnabled;
	HFONT m_hFont;
	int m_nIconId;
	CxImage m_cxIcon;
};

OBJECT_ENTRY_AUTO(__uuidof(IGMenuBarDropButton), CIGMenuBarDropButton)
