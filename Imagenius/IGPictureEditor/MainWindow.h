// MainWindow.h : Declaration of the CMainWindow
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include <IGMainWindow.h>

#include "IGPictureEditor_i.h"
#include "_IMainWindowEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

class CMainWindowLic
{
protected:
   static BOOL VerifyLicenseKey(BSTR bstr)
   {
      return !lstrcmpW(bstr, L"MainWindow license");
   }

   static BOOL GetLicenseKey(DWORD dwReserved, BSTR* pBstr)
   {
   	  if( pBstr == NULL )
 		return FALSE;
      *pBstr = SysAllocString(L"MainWindow license");
      return TRUE;
   }

   static BOOL IsLicenseValid()
   {
	   return TRUE;
   }
};

// CMainWindow
class ATL_NO_VTABLE CMainWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CMainWindow, IDispatch>,
	public IPersistStreamInitImpl<CMainWindow>,
	public IOleControlImpl<CMainWindow>,
	public IOleObjectImpl<CMainWindow>,
	public IOleInPlaceActiveObjectImpl<CMainWindow>,
	public IViewObjectExImpl<CMainWindow>,
	public IOleInPlaceObjectWindowlessImpl<CMainWindow>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CMainWindow>,
	public CProxy_IMainWindowEvents<CMainWindow>,
	public IPersistStorageImpl<CMainWindow>,
	public ISpecifyPropertyPagesImpl<CMainWindow>,
	public IQuickActivateImpl<CMainWindow>,
	public IDataObjectImpl<CMainWindow>,
	public IProvideClassInfo2Impl<&CLSID_MainWindow, &__uuidof(_IMainWindowEvents), &LIBID_IGPictureEditorLib>,
	public IPropertyNotifySinkCP<CMainWindow>,
	public CComCoClass<CMainWindow, &CLSID_MainWindow>,
	public CComControl<CMainWindow>,
	public IObjectSafetyImpl<CMainWindow,
                        INTERFACESAFE_FOR_UNTRUSTED_CALLER |
                        INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public IPersistPropertyBagImpl<CMainWindow>,
	public IOleClientSite,
	public IOleInPlaceSite
{
public:
	CMainWindow();
	virtual ~CMainWindow();

DECLARE_CLASSFACTORY2(CMainWindowLic)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_ACTSLIKELABEL |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_MAINWINDOW)


BEGIN_COM_MAP(CMainWindow)
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
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
	COM_INTERFACE_ENTRY(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IPersistPropertyBag)
	COM_INTERFACE_ENTRY(IObjectSafety)
	COM_INTERFACE_ENTRY(IOleClientSite)
	COM_INTERFACE_ENTRY(IOleInPlaceSite)
END_COM_MAP()

BEGIN_PROP_MAP(CMainWindow)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CMainWindow)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(__uuidof(_IMainWindowEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CMainWindow)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	CHAIN_MSG_MAP(CComControl<CMainWindow>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed Static
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IDispatch,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IMainWindow
public:	
	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	HRESULT OnDraw(ATL_DRAWINFO& di)
	{
		RECT& rc  = *(RECT*)di.prcBounds;
		HDC   hdc = di.hdcDraw;
		HBRUSH hbrBtn = CreateSolidBrush(RGB (217, 242, 217));
		FillRect(hdc, &rc, hbrBtn);	
		DeleteObject(hbrBtn);
		return 0;
	}

	OLE_COLOR m_clrBackColor;
	void OnBackColorChanged()
	{
		ATLTRACE(_T("OnBackColorChanged\n"));
	}
	CComBSTR m_bstrCaption;
	void OnCaptionChanged()
	{
		ATLTRACE(_T("OnCaptionChanged\n"));
	}
	CComPtr<IFontDisp> m_pFont;
	void OnFontChanged()
	{
		ATLTRACE(_T("OnFontChanged\n"));
	}
	OLE_COLOR m_clrForeColor;
	void OnForeColorChanged()
	{
		ATLTRACE(_T("OnForeColorChanged\n"));
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	//IOleClientSite
	STDMETHODIMP SaveObject () {return E_NOTIMPL;}
	STDMETHODIMP GetMoniker (DWORD, DWORD, IMoniker**) {return E_NOTIMPL;}
	STDMETHODIMP GetContainer (IOleContainer**) {return E_NOTIMPL;}
	STDMETHODIMP ShowObject () {return E_NOTIMPL;}
	STDMETHODIMP OnShowWindow (BOOL) {return E_NOTIMPL;}
	STDMETHODIMP RequestNewObjectLayout(void) {return E_NOTIMPL;}
	
	//IOleInPlaceSite
	STDMETHODIMP GetWindow(HWND *hWnd) 
	{	*hWnd = m_hWnd;
		return E_NOTIMPL;}
	STDMETHODIMP ContextSensitiveHelp(BOOL) {return E_NOTIMPL;}
	STDMETHODIMP CanInPlaceActivate( void) {return E_NOTIMPL;}
    STDMETHODIMP OnInPlaceActivate( void)  {return E_NOTIMPL;}    
    STDMETHODIMP OnUIActivate( void)  {return E_NOTIMPL;}
    STDMETHODIMP GetWindowContext( 
        /* [out] */ __RPC__deref_out_opt IOleInPlaceFrame **ppFrame,
        /* [out] */ __RPC__deref_out_opt IOleInPlaceUIWindow **ppDoc,
        /* [out] */ __RPC__out LPRECT lprcPosRect,
        /* [out] */ __RPC__out LPRECT lprcClipRect,
        /* [out][in] */ __RPC__inout LPOLEINPLACEFRAMEINFO lpFrameInfo)  {return E_NOTIMPL;}
    STDMETHODIMP Scroll( 
        /* [in] */ SIZE scrollExtant)  {return E_NOTIMPL;}   
    STDMETHODIMP OnUIDeactivate( 
        /* [in] */ BOOL fUndoable) {return E_NOTIMPL;}   
    STDMETHODIMP OnInPlaceDeactivate( void) {return E_NOTIMPL;}  
    STDMETHODIMP DiscardUndoState( void) {return E_NOTIMPL;}  
    STDMETHODIMP DeactivateAndUndo( void) {return E_NOTIMPL;}   
    STDMETHODIMP OnPosRectChange( 
        /* [in] */ __RPC__in LPCRECT lprcPosRect)  {return E_NOTIMPL;}

	STDMETHODIMP SetClientSite(LPOLECLIENTSITE pSite); 

private:
	CComPtr <IIGMainWindow>		m_spMainWindow;
	bool						m_bInstanceError;
};

OBJECT_ENTRY_AUTO(__uuidof(MainWindow), CMainWindow)
