// IGLayerManager.h : Declaration of the CIGLayerManager

#pragma once
#include "resource.h"       // main symbols

#include "IGPictureEditor_i.h"
#include "_IIGLayerManagerEvents_CP.h"
#include "IGDockPanel.h"
#include "IGCommunicable.h"	// Drag&drop, copy/paste

#include <IGSingleton.h>
#include <IGSmartPtr.h>
#include <IGFrameManager.h>
#include <IGScrollBar.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define IGLAYERMANAGER_LISTBOXCOLOR_BACKGROUND	Color(255, 195, 207, 200)	// Gray
#define IGLAYERMANAGER_BUTTONID_VISIBLE			4
#define IGLAYERMANAGER_BUTTONID_TOOL			5

class CIGLayerManager_Item	:	public IGCommunicable
{
	friend class CIGLayerManager;

public:
	CIGLayerManager_Item (HWND hListBox, IGLibrary::IGLayer *pLayer, IIGDockPanel *pDockPanel);
	~CIGLayerManager_Item ();

	void Draw (HDC hdc, RECT *p_rcItem);
	void Redraw ();	
	void Move (LPRECT p_rcItem);

protected:
	// IGCommunicable
	virtual int exportData (std::ostream& os, int fid, int cid, int x, int y, HWND hWnd);

private:
	LRESULT processLeftMouseButtonDown (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT processMouseMove (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT processMouseUp (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	IGLibrary::IGLayer			*m_pLayer;
	HWND						m_hListBox;
	CComPtr<IIGSimpleButton>	m_spButtonVisible;
	CComPtr<IIGSimpleButton>	m_spButtonTool;
};

// CIGLayerManager
class ATL_NO_VTABLE CIGLayerManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIGLayerManager, &CLSID_IGLayerManager>,
	public IConnectionPointContainerImpl<CIGLayerManager>,
	public CProxy_IIGLayerManagerEvents<CIGLayerManager>,
	public IObjectWithSiteImpl<CIGLayerManager>,
	public IDispatchImpl<IIGLayerManager, &IID_IIGLayerManager, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventSimpleImpl <1, CIGLayerManager, &__uuidof(_IIGDockPanelEvents)>
{
public:
	CIGLayerManager();
	virtual ~CIGLayerManager();

	STDMETHODIMP CreateFrame (IDispatch *pDispWindowOwner);
	STDMETHODIMP DestroyFrame ();
	STDMETHODIMP Move (LPRECT p_rc);
	STDMETHODIMP SetWorking (LONG nId);
	STDMETHODIMP PopulateListBox();

DECLARE_REGISTRY_RESOURCEID(IDR_IGLAYERMANAGER)

BEGIN_COM_MAP(CIGLayerManager)
	COM_INTERFACE_ENTRY(IIGLayerManager)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGLayerManager)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGLayerManagerEvents))
END_CONNECTION_POINT_MAP()

BEGIN_SINK_MAP(CIGLayerManager)
	SINK_ENTRY_INFO (1, __uuidof (_IIGDockPanelEvents), 1, &CIGLayerManager::OnPanelOpening, &EventPanelOpenInfo)
	SINK_ENTRY_INFO (1, __uuidof (_IIGDockPanelEvents), 2, &CIGLayerManager::OnPanelOpened, &EventPanelOpenInfo)
	SINK_ENTRY_INFO (1, __uuidof (_IIGDockPanelEvents), 3, &CIGLayerManager::OnPanelClosing, &EventPanelOpenInfo)
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
	LRESULT OnAddLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnRemoveLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnMergeLayers(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnMoveUpLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnMoveDownLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnVisibleLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);
	LRESULT OnWorkingLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled);

	// drag & drop
	LRESULT OnLeftMouseButtonDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	bool drawListBoxItem (HDC hdc, int nIdxItem);
	void updateScrollBar ();
	void setWorking (IGLibrary::IGLayer* pLayer);
	void removeLayer (const IGLibrary::IGLayer *pLayer, IGLibrary::IGLayer **ppDeletedLayer = NULL, bool bRemoveLayers = true);

	// _IIGDockPanelEvents
	void __stdcall OnPanelOpening ();
	void __stdcall OnPanelOpened ();
	void __stdcall OnPanelClosing ();

	CComQIPtr <IIGDockPanel> m_spDockPanelOwner;

	IGLibrary::IGSingleton <IGLibrary::IGFrameManager>			m_spFrameMgr;
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame>					m_spFrame;
	IGLibrary::IGSmartPtr <IGLibrary::IGScrollBar>				m_spScrollBar;
	std::list <CIGLayerManager_Item*>							m_lpItems;

	HWND	m_hWnd;
	HWND	m_hListBox;
	RECT	m_rcListBox;
	CComPtr<IIGSimpleButton> m_spButtonAdd;
	CComPtr<IIGSimpleButton> m_spButtonRemove;
	CComPtr<IIGSimpleButton> m_spButtonMerge;
	CComPtr<IIGSimpleButton> m_spButtonUp;
	CComPtr<IIGSimpleButton> m_spButtonDown;
};

OBJECT_ENTRY_AUTO(__uuidof(IGLayerManager), CIGLayerManager)
