// IGImageLibrary.h : Declaration of the CIGImageLibrary
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "_IIGImageLibraryEvents_CP.h"
#include "commandids.h"
#include "IGImageLibrary_AddPictures.h"
#include <IGConfigManager.h>
#include <IGUserMessages.h>
#include <IGSmartPtr.h>
#include <IGSingleton.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define IGIMAGELIBRARY_MINIPICTUREWIDTH		80
#define IGIMAGELIBRARY_MINIPICTUREHEIGHT	80

// _IIGImageLibraryEvents
extern _ATL_FUNC_INFO EventOpenImageInfo;

// CIGImageLibrary
class ATL_NO_VTABLE CIGImageLibrary :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGImageLibrary, &IID_IIGImageLibrary, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIGImageLibrary>,
	public IOleControlImpl<CIGImageLibrary>,
	public IOleObjectImpl<CIGImageLibrary>,
	public IOleInPlaceActiveObjectImpl<CIGImageLibrary>,
	public IViewObjectExImpl<CIGImageLibrary>,
	public IOleInPlaceObjectWindowlessImpl<CIGImageLibrary>,
	public IConnectionPointContainerImpl<CIGImageLibrary>,
	public CProxy_IIGImageLibraryEvents<CIGImageLibrary>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CIGImageLibrary, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CIGImageLibrary, &CLSID_IGImageLibrary>,
	public CComControl<CIGImageLibrary>
{
public:
	CIGImageLibrary();
	virtual ~CIGImageLibrary();

	// COM methods
	STDMETHODIMP CreateFrame(OLE_HANDLE hWnd, LPRECT p_rc);
	STDMETHODIMP DestroyFrame();
	STDMETHODIMP ShowFrame (LPRECT prc, LONG nId = -1);

	// COM properties
	STDMETHODIMP put_Font(OLE_HANDLE hFont);

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IGIMAGELIBRARY)


BEGIN_COM_MAP(CIGImageLibrary)
	COM_INTERFACE_ENTRY(IIGImageLibrary)
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

BEGIN_PROP_MAP(CIGImageLibrary)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGImageLibrary)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGImageLibraryEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CIGImageLibrary)	
	COMMAND_ID_HANDLER(IGM_IMAGELIB_ADD, OnImageLibAdd)
	COMMAND_ID_HANDLER(IGM_IMAGELIB_OPEN, OnImageLibOpen)
	COMMAND_ID_HANDLER(IGM_IMAGELIB_REMOVE, OnImageLibRemove)
	COMMAND_ID_HANDLER(IGM_IMAGELIB_DEFAULT, OnImageLibDefault)
	COMMAND_ID_HANDLER(IGM_IMAGELIB_CLOSE, OnImageLibClose)
	COMMAND_ID_HANDLER(IGM_IMAGELIBSCROLL_BEGIN, OnImageLibScrollBegin)
	COMMAND_ID_HANDLER(IGM_IMAGELIBSCROLL_PAGELEFT, OnImageLibScrollPageLeft)
	COMMAND_ID_HANDLER(IGM_IMAGELIBSCROLL_LEFT, OnImageLibScrollLeft)
	COMMAND_ID_HANDLER(IGM_IMAGELIBSCROLL_RIGHT, OnImageLibScrollRight)
	COMMAND_ID_HANDLER(IGM_IMAGELIBSCROLL_PAGERIGHT, OnImageLibScrollPageRight)
	COMMAND_ID_HANDLER(IGM_IMAGELIBSCROLL_END, OnImageLibScrollEnd)
	MESSAGE_HANDLER (WM_KILLFOCUS, OnKillFocus)
	MESSAGE_HANDLER (WM_MEASUREITEM, OnMeasureListBoxItem)
	MESSAGE_HANDLER (WM_DRAWITEM, OnDrawListBoxItem)
	MESSAGE_HANDLER (WM_DELETEITEM, OnDeleteItem)
	MESSAGE_HANDLER (UM_ADDPICTURES, OnAddPictures)
	MESSAGE_HANDLER (WM_MOUSEACTIVATE, OnMouseActivate)
	CHAIN_MSG_MAP(CComControl<CIGImageLibrary>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IIGImageLibrary
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

	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	// Menubar message handler methods
	LRESULT OnImageLibAdd(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibOpen(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibRemove(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibDefault(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibClose(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);

	// Add picture thread message callback
	LRESULT OnAddPictures(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Standard message handler methods
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnProcessHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDeleteItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Scrolling message handler methods
	LRESULT OnImageLibScrollBegin (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibScrollPageLeft (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibScrollLeft (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibScrollRight (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibScrollPageRight (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibScrollEnd (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);

private:
	static LRESULT APIENTRY StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookListBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void populateListBox ();
	void addListBoxItem (const IGLibrary::IGLibPicture *picture);
	void drawListBoxItem (HDC hDC, int nItemId);
	void addEmptyItemIfNecessary();
	void updateCurrentScrollPos();
	void updateListBoxScrollPos();
	void redraw();
	void hide ();

	static int g_nNbItemsPerColumn;
	static int g_nNbColumnsPerPage;
	HWND m_hListBox;
	HWND m_hParent;
	HRGN m_hRegion;	
	int m_nMaxPos;
	int m_nCurrentScrollPos;
	int m_nIdToSendBack;
	CComPtr <IIGMenuBar> m_spMenuBar;
	CComPtr <IIGMenuBar> m_spScrollBar;
	IGLibrary::IGSingleton <IGLibrary::IGConfigManager> m_spConfigMgr;
	int *m_pnSelectedItems;
	int m_nNbSelectedItems;
	HFONT m_hFont;
	IGAddPictures	*m_pThreadAddPicture;
	IGLibrary::IGSmartPtr <IGLibrary::IGSplashWindow> m_spProgress;
};

OBJECT_ENTRY_AUTO(__uuidof(IGImageLibrary), CIGImageLibrary)
