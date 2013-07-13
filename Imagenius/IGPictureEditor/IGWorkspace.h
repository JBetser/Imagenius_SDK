// IGWorkspace.h : Declaration of the CIGWorkspace

#pragma once
#include "resource.h"       // main symbols

#include "IGPictureEditor_i.h"
#include "_IIGWorkspaceEvents_CP.h"
#include "IGDockPanel.h"
#include "IGSimpleButton.h"

#include <IGSingleton.h>
#include <IGSmartPtr.h>
#include <IGFrameManager.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define IGWORKSPACE_LISTBOX_X		5
#define IGWORKSPACE_LISTBOX_Y		22
#define IGWORKSPACE_COLOR_BUTTONBORDER	Color(255, 52, 148, 214)	// Blue
#define IGWORKSPACE_COLOR_BUTTON		Color(100, 52, 148, 214)	// Light Blue
#define IGWORKSPACE_COLUMNWIDTH		80

class CIGWorkspaceButtonPanel
{
public:
	CIGWorkspaceButtonPanel (HWND hBackGround, HWND hListBox, RECT *p_rcPanel, IDispatch *pFrame, IDispatch *pDispMultiFrame, IDispatch *pDispDockPanel, IDispatch *pWorkspace);
	~CIGWorkspaceButtonPanel ();

	// returns false if the item is no more displayed
	void MoveLeft ();
	void MoveRight ();
	void Show (int nCmdShow);
	
	HWND m_hMinimizeWnd;
	HWND m_hMaximizeWnd;
	HWND m_hCloseWnd;
	HWND m_hSaveWnd;
	HWND m_hSaveAsWnd;
	HWND m_hUploadWnd;

private:
	void moveButton (HWND hButton, int nOffset);
	bool isInside (HWND hButton);
	bool isInside (HWND hButton, int nOffset);

	HWND m_hListBox;
	HWND m_hBackGround;
	static HWND g_hOverButton;
	HWND m_hOverButton;
	HDC m_hBackgroundDC;
	HBITMAP m_hBackgroundBmp;
	static bool g_bChanged;
	bool m_bButtonChanged;
	bool m_bMaximized;
	bool m_bMinimized;
	IGBUTTONSTATE		m_eButtonState;
	RECT m_rcParent;
	RECT m_rcPanel;
	IGLibrary::IGFrame *m_pCxIconClose;
	IGLibrary::IGFrame *m_pCxIconMinimize;
	IGLibrary::IGFrame *m_pCxIconMaximize;
	IGLibrary::IGFrame *m_pCxIconUnMaximize;
	IGLibrary::IGFrame *m_pCxIconSave;
	IGLibrary::IGFrame *m_pCxIconSaveAs;
	IGLibrary::IGFrame *m_pCxIconUpload;
	static IGLibrary::IGFrame *g_pCxIconClose;
	static IGLibrary::IGFrame *g_pCxIconMinimize;
	static IGLibrary::IGFrame *g_pCxIconMaximize;
	static IGLibrary::IGFrame *g_pCxIconUnMaximize;
	static IGLibrary::IGFrame *g_pCxIconSave;
	static IGLibrary::IGFrame *g_pCxIconSaveAs;
	static IGLibrary::IGFrame *g_pCxIconUpload;
	IIGFrame				  *m_pFrame;
	IIGMultiFrame			  *m_pMultiFrame;
	IIGDockPanel			  *m_pDockPanel;
	IIGWorkspace			  *m_pWorkspace;

	void OnButtonClose ();
	void OnButtonMaximize ();
	void OnButtonMinimize ();
	void OnButtonSave ();
	void OnButtonSaveAs ();
	void OnButtonUpload ();
	void OnEraseBkgnd (HWND hWnd, HDC hdc);
	
	static LRESULT APIENTRY StaticHookPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void createButtonIcon (IGLibrary::IGFrame *&pCxButtonIcon, LPCWSTR pwIcon);
	HWND createButtonWindow (int nLeft, int nTop, int nSize);
	void drawButton (HWND hWnd, HDC hDC, IGLibrary::IGFrame *pCxButtonIcon);
	void redrawButton (HWND hWnd);	
	void redrawButtons ();
	void saveBackground ();
	int getFrameId (); // helper to find m_spFrame id in frame Mgr
};

// CIGWorkspace
class ATL_NO_VTABLE CIGWorkspace :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIGWorkspace, &CLSID_IGWorkspace>,
	public IConnectionPointContainerImpl<CIGWorkspace>,
	public CProxy_IIGWorkspaceEvents<CIGWorkspace>,
	public IObjectWithSiteImpl<CIGWorkspace>,
	public IDispatchImpl<IIGWorkspace, &IID_IIGWorkspace, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventSimpleImpl <1, CIGWorkspace, &__uuidof(_IIGDockPanelEvents)>
{
public:
	CIGWorkspace();
	virtual ~CIGWorkspace();

	STDMETHODIMP CreateFrame (IDispatch *pDispWindowOwner, IDispatch *pDispMultiFrame);
	STDMETHODIMP DestroyFrame ();
	STDMETHODIMP Move (LPRECT p_rc);
	STDMETHODIMP AddFrame (BSTR bstrImagePath, VARIANT_BOOL bAbsolutePath = VARIANT_FALSE, VARIANT_BOOL bAutoRotate = VARIANT_FALSE);
	STDMETHODIMP AddFrameFromHandle (OLE_HANDLE hMem);
	STDMETHODIMP RemoveFrame (LONG nFrameId);
	STDMETHODIMP SaveFrame (LONG nFrameId, VARIANT_BOOL bSilent = VARIANT_FALSE, BSTR bstrImagePath = NULL, VARIANT_BOOL bCloseAfterSave = VARIANT_FALSE);
	STDMETHODIMP GetActiveFrame (OLE_HANDLE *p_hFrame);
	STDMETHODIMP CreateNewFrame (LONG nWidth, LONG nHeight, LONG nColorMode, LONG nBackgroundMode);
	STDMETHODIMP ConnectUser (BSTR bstrUserLogin, BSTR bstrGuid = NULL, LONG *p_nNbFrames = NULL);
	STDMETHODIMP DisconnectUser (VARIANT_BOOL bSaveAllToTemp = VARIANT_TRUE);
	STDMETHODIMP GetFrameProperty (LONG nFrameId, BSTR bstrPropId, VARIANT *pPropVal);
	STDMETHODIMP GetFrameIds (BSTR *p_bstrIds);
	STDMETHODIMP GetActiveFrameId (BSTR *p_bstrIds);
	STDMETHODIMP GetFrameProperties (BSTR *p_bstrFrameProperties);
	STDMETHODIMP GetFrameStepIds (BSTR *p_bstrFrameStepIds);
	STDMETHODIMP GetFrameNames (BSTR *p_bstrNames);
	STDMETHODIMP GetFrameNbLayers (BSTR *p_bstrNbLayers);
	STDMETHODIMP GetFrameLayerVisibility (BSTR *p_bstrLayerVisibility);
	STDMETHODIMP SetFrameProperty (LONG nFrameId, BSTR bstrPropId, VARIANT *pPropVal);
	STDMETHODIMP UpdateFrame (LONG nFrameId);
	STDMETHODIMP GetProperty (BSTR bstrPropId, VARIANT *pPropVal);
	STDMETHODIMP SetProperty (BSTR bstrPropId, VARIANT *pPropVal);
	STDMETHODIMP GetPropertiesAndValues (BSTR *p_bstrPropId);
	STDMETHODIMP Update();
	STDMETHODIMP Copy(BSTR bstrImageGuid);
	STDMETHODIMP Cut(BSTR bstrImageGuid);
	STDMETHODIMP Paste(BSTR bstrImageGuid);

DECLARE_REGISTRY_RESOURCEID(IDR_IGWORKSPACE)

BEGIN_COM_MAP(CIGWorkspace)
	COM_INTERFACE_ENTRY(IIGWorkspace)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGWorkspace)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGWorkspaceEvents))
END_CONNECTION_POINT_MAP()

BEGIN_SINK_MAP(CIGWorkspace)
	SINK_ENTRY_INFO (1, __uuidof (_IIGDockPanelEvents), 1, &CIGWorkspace::OnPanelOpening, &EventPanelOpenInfo)
	SINK_ENTRY_INFO (1, __uuidof (_IIGDockPanelEvents), 2, &CIGWorkspace::OnPanelOpened, &EventPanelOpenInfo)
	SINK_ENTRY_INFO (1, __uuidof (_IIGDockPanelEvents), 3, &CIGWorkspace::OnPanelClosing, &EventPanelOpenInfo)
END_SINK_MAP()

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

	static LRESULT APIENTRY StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
  	LRESULT HookListBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT APIENTRY StaticHookButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
  	LRESULT HookButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	bool drawListBoxItem (HDC hdc, int nIdxItem);
	void populateListBox ();
	void drawButtonNew (HDC hdc);
	void drawButton (HDC hdc, bool bIsLeft);
	void redrawButton (HWND hWnd);
	void updateCurrentScrollPos();
	void updateListBoxScrollPos();
	void registerButtonPanel (CIGWorkspaceButtonPanel *pButtonPanel);
	void unregisterButtonPanel (CIGWorkspaceButtonPanel *pButtonPanel);
	void checkLeftRightButtonState ();
	std::wstring createTempFile (bool bRecover = false);
	HRESULT copyMiniPictures(BSTR bstrGuid);

	// _IIGDockPanelEvents
	void __stdcall OnPanelOpening ();
	void __stdcall OnPanelOpened ();
	void __stdcall OnPanelClosing ();

	// private left/right button click event
	void OnButtonClick (bool bIsLeft);

	void OnButtonNewClick ();

	HWND	m_hWnd;
	HWND	m_hListBox;
	HWND	m_hButtonLeft;
	HWND	m_hButtonRight;
	HWND	m_hButtonNew;
	HWND	m_hOverButton;
	RECT	m_rcWindow;
	RECT	m_rcListBox;
	RECT	m_rcButtonLeft;
	RECT	m_rcButtonRight;
	RECT	m_rcButtonNew;
	IGBUTTONSTATE		m_eButtonState;
	int		m_nNbVisibleItems;
	int		m_nCurrentScrollPos;
	CxImage	m_cxIconNewFrame;
	std::list <IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>> m_lspButtonPanels;
	CComQIPtr <IIGDockPanel> m_spDockPanelOwner;
	CComQIPtr <IIGMultiFrame> m_spMultiFrame;
	IGLibrary::IGSingleton <IGLibrary::IGFrameManager>	m_spFrameMgr;
	IGLibrary::IGSingleton <IGLibrary::IGConfigManager>	m_spConfigMgr;
};

OBJECT_ENTRY_AUTO(__uuidof(IGWorkspace), CIGWorkspace)
