// IGMainWindow.h : Declaration of the CIGMainWindow

#pragma once
#include "resource.h"       // main symbols

#include "IGPictureEditor_i.h"
#include "IGFrameControl.h"
#include "IGImageLibrary.h"
#include "IGDockPanel.h"
#include "IGToolBox.h"
#include "commandids.h"
#include <IGSingleton.h>
#include <IGSmartPtr.h>
#include <IGTabBar.h>
#include <IGConfigManager.h>
#include <IGFrameManager.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define IGMENUBAR_HEIGHT				25
#define TABBAR_HEIGHTOFFSET				10

// CIGMainWindow
class ATL_NO_VTABLE CIGMainWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIGMainWindow, &CLSID_IGMainWindow>,
	public IDispEventSimpleImpl <1, CIGMainWindow, &__uuidof(_IIGImageLibraryEvents)>,
	public IObjectWithSiteImpl<CIGMainWindow>,
	public IDispatchImpl<IIGMainWindow, &IID_IIGMainWindow, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleObjectImpl<CIGMainWindow>,
	public CComControl<CIGMainWindow>,
	public IOleInPlaceActiveObjectImpl<CIGMainWindow>,
	public IOleInPlaceObjectWindowlessImpl<CIGMainWindow>,
	public IOleClientSite,
	public IOleInPlaceSite
{
public:
	CIGMainWindow();
	virtual ~CIGMainWindow();

DECLARE_REGISTRY_RESOURCEID(IDR_IGMAINWINDOW)

	STDMETHODIMP CreateFrame (OLE_HANDLE hWnd, VARIANT_BOOL bIsServer = VARIANT_FALSE, BSTR bstrServerIP = NULL, BSTR bstrOutputPath = NULL);
	STDMETHODIMP DestroyFrame();
	STDMETHODIMP Connect (SHORT nInputPortId, BSTR bstrServerIp, OLE_HANDLE hWndStatus = NULL);
	STDMETHODIMP ShowOwner (BOOL bVisible);
	STDMETHODIMP GetIGInterface (REFIID iid, OLE_HANDLE *ppvObject);

BEGIN_COM_MAP(CIGMainWindow)
	COM_INTERFACE_ENTRY(IIGMainWindow)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleClientSite)
	COM_INTERFACE_ENTRY(IOleInPlaceSite)
END_COM_MAP()

BEGIN_SINK_MAP(CIGMainWindow)
	SINK_ENTRY_INFO (1, __uuidof (_IIGImageLibraryEvents), 1, &CIGMainWindow::OnOpenImage, &EventOpenImageInfo)
END_SINK_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	HWND	m_hWnd;

	// Standard message handlers
	LRESULT OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

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
	static LRESULT APIENTRY StaticHookMainWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
  	LRESULT HookMainWindowProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// _IIGImageLibraryEvents
	void __stdcall OnOpenImage (BSTR bstrImagePath);

	// Mainmenu message handlers
	LRESULT OnImageLibOpen(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnImageLibClosed(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnOptions(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnAccount(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	
	// Tools message handlers
	LRESULT OnToolsResize(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsRotate(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsBrightness(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsContrast(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsColor(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsRedeye(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsHistogramEqualize(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsFilterErode(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsFilterDilate(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsFilterBlur(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsFilterGaussian(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsFilterGradient(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnToolsFilterZerocross(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);

	bool initAll();
	void initFonts ();
	HANDLE installFont (HINSTANCE hInstance, int numResFont);
	HWND getHWNDfromOLE(IDispatch* pDisp);

	u_long						m_gdiplusToken;
	CComPtr <IIGMultiFrame>		m_spMultiFrame;
	CComPtr <IIGMenuBar>		m_spMenuBar;
	CComPtr <IIGImageLibrary>	m_spImageLibrary;
	CComPtr <IIGDockPanel>		m_spDockPanelLeft;
	CComPtr <IIGDockPanel>		m_spDockPanelRight;
	CComPtr <IIGDockPanel>		m_spDockPanelBottom;
	CComPtr <IIGWorkspace>		m_spWorkspace;
	CComPtr <IIGLayerManager>	m_spLayerManager;
	CComPtr <IIGHistoryManager>	m_spHistoryManager;
	CComPtr <IIGPropertyManager> m_spPropertyMgr;
	CComPtr <IIGPropertyManager> m_spFramePropertyMgr;
	CComPtr <IIGToolBox>		m_spToolBox;
	CComPtr <IIGSocket>			m_spSocket;
	IGLibrary::IGSmartPtr <IGLibrary::IGTabBar>			m_spTabBar;
	IGLibrary::IGSingleton <IGLibrary::IGConfigManager>	m_spConfigMgr;	
	HANDLE						m_hFontAthena;
	HANDLE						m_hFontCursif;
	HFONT						m_hFontUI;
	HFONT						m_hFontMessage;		
	HWND						m_hWndStatus;
	SHORT						m_nInputPortId;	
	CComBSTR					m_spbstrServerIp;
};

OBJECT_ENTRY_AUTO(__uuidof(IGMainWindow), CIGMainWindow)
