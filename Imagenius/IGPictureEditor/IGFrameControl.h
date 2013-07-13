// IGFrameControl.h : Declaration of the CIGFrameControl
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "_IIGFrameEvents_CP.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGLayerManager.h"
#include "IGCommunicable.h"	// Drag&drop, copy/paste
#include <IGFrameControl.h>
#include <IGSmartPtr.h>
#include <IGFrameManager.h>
#include <IGSingleton.h>

#include <ostream>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CIGFrameControl
class ATL_NO_VTABLE CIGFrameControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGFrame, &IID_IIGFrame, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIGFrameControl>,
	public IOleControlImpl<CIGFrameControl>,
	public IOleObjectImpl<CIGFrameControl>,
	public IOleInPlaceActiveObjectImpl<CIGFrameControl>,
	public IViewObjectExImpl<CIGFrameControl>,
	public IOleInPlaceObjectWindowlessImpl<CIGFrameControl>,
	public IConnectionPointContainerImpl<CIGFrameControl>,	
	public CProxy_IIGFrameEvents<CIGFrameControl>,
	public IObjectWithSiteImpl<CIGFrameControl>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CIGFrameControl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CIGFrameControl, &CLSID_IGFrameControl>,
	public CComControl<CIGFrameControl>,
	public IGCommunicable
{
public:
	CIGFrameControl();
	virtual ~CIGFrameControl();

DECLARE_REGISTRY_RESOURCEID(IDR_IGFRAME)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

BEGIN_COM_MAP(CIGFrameControl)
	COM_INTERFACE_ENTRY(IIGFrame)
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
	COM_INTERFACE_ENTRY(IObjectWithSite)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(CIGFrameControl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGFrameControl)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGFrameEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CIGFrameControl)
	MESSAGE_HANDLER(UM_IMAGEPROCESSING, OnImageProcessingEnded)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLeftMouseButtonDown)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseUp)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
	MESSAGE_HANDLER(UM_IGFRAME_LAYERCHANGED, OnLayerChanged)
	MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	CHAIN_MSG_MAP(CComControl<CIGFrameControl>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IIGFrame
public:
	STDMETHODIMP CreateFrame(OLE_HANDLE hWnd, LPRECT prc, BSTR bstrPath, IDispatch *pDispLayerManager = NULL, IDispatch *pDispPropertyManager = NULL, VARIANT_BOOL bAutoRotate = VARIANT_FALSE);
	STDMETHODIMP CreateNewFrame(OLE_HANDLE hWnd, LPRECT prc, LONG nWidth, LONG nHeight, LONG nColorMode, LONG nBackgroundMode, IDispatch *pDispLayerManager = NULL, IDispatch *pDispPropertyManager = NULL);
	STDMETHODIMP CreateFrameFromHandle(OLE_HANDLE hWnd, LPRECT prc, OLE_HANDLE hMem, IDispatch *pDispLayerManager = NULL, IDispatch *pDispPropertyManager = NULL);
	STDMETHODIMP DestroyFrame();
	STDMETHODIMP UpdatePosition ();
	STDMETHODIMP AddLayer(LONG nPos = -1);
	STDMETHODIMP RemoveLayer(LONG nPos);
	STDMETHODIMP MoveLayer(LONG nPosFrom, LONG nPosDest);
	STDMETHODIMP SetToolBox(IDispatch *pDispToolbox);
	STDMETHODIMP ManageSelection(LONG nSelectionActionId, VARIANT *pArrayParams);
	STDMETHODIMP ConvertCoordFrameToPixel (LONG *p_nPosX, LONG *p_nPosY);
	STDMETHODIMP ProcessImage (LONG nImageProcessingId, OLE_HANDLE hParams);
	STDMETHODIMP put_isMinimized (VARIANT_BOOL bMinimized);
	STDMETHODIMP get_isMinimized (VARIANT_BOOL *p_bMinimized);
	STDMETHODIMP get_isMaximized (VARIANT_BOOL *p_bMaximized);
	STDMETHODIMP put_isMaximized (VARIANT_BOOL bMaximized);
	STDMETHODIMP put_isVisible (VARIANT_BOOL bVisible);

	HRESULT OnDraw(ATL_DRAWINFO& di);

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	LRESULT OnLeftMouseButtonDown (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnImageProcessingEnded (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLayerChanged (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	// IGCommunicable
	virtual int exportData (std::ostream& os, int fid, int cid, int x, int y, HWND hWnd);
	virtual int importData (std::istream& is, int fid, int bufsize, int cid, int x, int y, HWND hWnd);

private:
	int initializeController (HWND hwnd);

	IGLibrary::IGSmartPtr <IGLibrary::IGFrame>			m_spFrame;
	IGLibrary::IGSingleton <IGLibrary::IGFrameManager>	m_spFrameMgr;
	CComQIPtr <IIGLayerManager>							m_spLayerMgr;
	CComQIPtr <IIGPropertyManager>						m_spPropertyMgr;
	CComQIPtr <IIGToolBox>								m_spToolBox;
	IGLibrary::IGSingleton <IGLibrary::IGConfigManager>	m_spConfigMgr;
	int m_nFramePosX;
	int m_nFramePosY;
	int m_nFrameWidth;
	int m_nFrameHeight;
	bool m_bMinimized;
	bool m_bMaximized;
};

OBJECT_ENTRY_AUTO(__uuidof(IGFrameControl), CIGFrameControl)