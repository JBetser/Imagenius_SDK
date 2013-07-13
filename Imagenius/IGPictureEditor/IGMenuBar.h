// IGMenuBar.h : Declaration of the CIGMenuBar
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <vector>
#include "IGPictureEditor_i.h"
#include "_IIGMenuBarEvents_CP.h"
#include "IGCommunicable.h"	// Drag&drop, copy/paste

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CIGMenuBar
class ATL_NO_VTABLE CIGMenuBar :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGMenuBar, &IID_IIGMenuBar, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIGMenuBar>,
	public IOleControlImpl<CIGMenuBar>,
	public IOleObjectImpl<CIGMenuBar>,
	public IOleInPlaceActiveObjectImpl<CIGMenuBar>,
	public IViewObjectExImpl<CIGMenuBar>,
	public IOleInPlaceObjectWindowlessImpl<CIGMenuBar>,
	public IConnectionPointContainerImpl<CIGMenuBar>,
	public CProxy_IIGMenuBarEvents<CIGMenuBar>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CIGMenuBar, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CIGMenuBar, &CLSID_IGMenuBar>,
	public CComControl<CIGMenuBar>,
	public IGCommunicable
{
public:

	CIGMenuBar();
	virtual ~CIGMenuBar();

	// COM methods
	STDMETHODIMP CreateFrame(OLE_HANDLE hWnd, LPRECT p_rc, BSTR xmlConfig, LONG idxButton = -1, LONG nRefPosX = 0, LONG nRefPosY = 0, VARIANT_BOOL bVertical = VARIANT_FALSE, LONG idxSubMenu = -1);
	STDMETHODIMP DestroyFrame();
	STDMETHODIMP ShowPopup(LONG nNewRefPosX, LONG nNewRefPosY);
	STDMETHODIMP Select(LONG idxButtonClick);
	STDMETHODIMP UnSelect(LONG idxButton);
	STDMETHODIMP EnableButton(LONG idxButtonClick);
	STDMETHODIMP DisableButton(LONG idxButtonClick);
	STDMETHODIMP Move (LPRECT p_rc);
	STDMETHODIMP AddDropTarget (IDropTarget *pDropTarget);

	// COM properties	
	STDMETHODIMP get_IdxSubMenu(LONG *p_idxSubMenu);
	STDMETHODIMP get_IdxButton(LONG *p_idxButton);	
	STDMETHODIMP put_Font(OLE_HANDLE hFont);

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IGMENUBAR)


BEGIN_COM_MAP(CIGMenuBar)
	COM_INTERFACE_ENTRY(IIGMenuBar)
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

BEGIN_PROP_MAP(CIGMenuBar)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGMenuBar)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGMenuBarEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CIGMenuBar)
	MESSAGE_HANDLER(WM_COMMAND,OnCommand)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_SHOWWINDOW, OnShow)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseUp)
	MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	CHAIN_MSG_MAP(CComControl<CIGMenuBar>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IIGMenuBar
public:
	HRESULT OnDraw(ATL_DRAWINFO& di);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	void hideSubMenus();

	CComPtr <IIGLibraryMenuButton> m_spLibButton;
	CComPtr <IIGMenuBarDropButton> *m_pButtons;
	std::vector <IIGMenuBar*>	m_vSubMenus;
	std::vector <int>			m_vSeparators;
	HWND m_hMenuBarParent;
	int m_nNbButtons;
	int m_idxButton;
	int m_idxSubMenu;
	VARIANT_BOOL m_bVertical;
	POINT m_ptRefPos;
	int m_nButtonSize;
	bool m_bTransparent;
	HFONT m_hFont;
	int m_nMenuBarInitWidth;
};

OBJECT_ENTRY_AUTO(__uuidof(IGMenuBar), CIGMenuBar)
