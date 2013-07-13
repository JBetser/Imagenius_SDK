// IGMultiFrame.h : Declaration of the CIGMultiFrame
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "_IIGMultiFrameEvents_CP.h"
#include <IGSingleton.h>
#include <IGFrameManager.h>
#include "IGCommunicable.h"	// Drag&drop, copy/paste

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

#define IGFRAMES_MAX	10

// CIGMultiFrame
class ATL_NO_VTABLE CIGMultiFrame :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGMultiFrame, &IID_IIGMultiFrame, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CIGMultiFrame>,
	public IOleControlImpl<CIGMultiFrame>,
	public IOleObjectImpl<CIGMultiFrame>,
	public IOleInPlaceActiveObjectImpl<CIGMultiFrame>,
	public IViewObjectExImpl<CIGMultiFrame>,
	public IOleInPlaceObjectWindowlessImpl<CIGMultiFrame>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CIGMultiFrame>,
	public CProxy_IIGMultiFrameEvents<CIGMultiFrame>,
	public IObjectWithSiteImpl<CIGMultiFrame>,
	public IPersistStorageImpl<CIGMultiFrame>,
	public ISpecifyPropertyPagesImpl<CIGMultiFrame>,
	public IQuickActivateImpl<CIGMultiFrame>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CIGMultiFrame>,
#endif
	public IProvideClassInfo2Impl<&CLSID_IGMultiFrame, &__uuidof(_IIGMultiFrameEvents), &LIBID_IGPictureEditorLib>,
	public IPropertyNotifySinkCP<CIGMultiFrame>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CIGMultiFrame, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CIGMultiFrame, &CLSID_IGMultiFrame>,
	public CComControl<CIGMultiFrame>,
	public IGCommunicable
{
public:
	CIGMultiFrame();
	virtual ~CIGMultiFrame();

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IGMULTIFRAME)


BEGIN_COM_MAP(CIGMultiFrame)
	COM_INTERFACE_ENTRY(IIGMultiFrame)
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
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(IObjectWithSite)
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(CIGMultiFrame)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CIGMultiFrame)
	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink)
	CONNECTION_POINT_ENTRY(__uuidof(_IIGMultiFrameEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CIGMultiFrame)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseUp)
	MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
	CHAIN_MSG_MAP(CComControl<CIGMultiFrame>)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

BEGIN_SINK_MAP(CIGMultiFrame)
	//Make sure the Event Handlers have __stdcall calling convention
END_SINK_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IIGMultiFrame,
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

// IIGMultiFrame
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	STDMETHODIMP CreateFrame(OLE_HANDLE hWnd, LPRECT rc, IDispatch *pDispLayerManager = NULL, IDispatch *pDispWorkspace = NULL, IDispatch *pDispToolbox = NULL, IDispatch *pDispPropertyMgr = NULL);
	STDMETHODIMP DestroyFrame();
	STDMETHODIMP RemoveFrame (LONG nFrameId);
	STDMETHODIMP AddImage (BSTR bstrImagePath, VARIANT_BOOL bAutoRotate = VARIANT_FALSE);
	STDMETHODIMP AddImageFromHandle (OLE_HANDLE hMem);
	STDMETHODIMP OpenImageLib (RECT rc);
	STDMETHODIMP Move (LPRECT p_rc);
	STDMETHODIMP DrawFrame (LONG IdxBitmap, OLE_HANDLE hDC, UINT_PTR pRect);
	STDMETHODIMP GetDropTarget (IDropTarget **ppDropTarget);
	STDMETHODIMP AddNewImage (LONG nWidth, LONG nHeight, LONG nColorMode, LONG nBackgroundMode);

	STDMETHODIMP put_isMinimized (OLE_HANDLE hPtrFrame, VARIANT_BOOL bMinimized);
	STDMETHODIMP get_isMinimized (OLE_HANDLE hPtrFrame, VARIANT_BOOL *p_bMinimized);
	STDMETHODIMP get_frameId (OLE_HANDLE hPtrFrame, LONG *p_nFrameId); // find a frame id
	STDMETHODIMP get_frameSize (OLE_HANDLE hPtrFrame, LONG *p_nWidth, LONG *p_nHeight); // get a frame size
	STDMETHODIMP get_isMaximized (OLE_HANDLE hPtrFrame, VARIANT_BOOL *p_bMaximized);
	STDMETHODIMP put_isMaximized (OLE_HANDLE hPtrFrame, VARIANT_BOOL bMaximized);

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
	// IGCommunicable
	virtual int importData (std::istream& is, int fid, int bufsize, int cid, int x, int y, HWND hWnd);

private:
	bool frameExists (IIGFrame *pFrame);
	void addFrame (IIGFrame *pFrame);
	HRESULT addFrame (BSTR bstrImagePath, VARIANT_BOOL bAutoRotate);
	HRESULT addFrame (HANDLE hMem);
	bool removeFrame (IIGFrame *pFrame);
	void autoArrange();	
	void unMaximize (); // unmaximize maximized frame is there is one

	int m_nNbFrames;
	CComPtr <IIGFrame>									m_ppFrames [IGFRAMES_MAX + 1];	
	CComQIPtr <IIGLayerManager>							m_spLayerMgr;
	CComQIPtr <IIGWorkspace>							m_spWorkspace;
	CComQIPtr <IIGToolBox>								m_spToolBox;
	CComQIPtr <IIGPropertyManager>						m_spPropertyMgr;
	IGLibrary::IGSingleton <IGLibrary::IGFrameManager>	m_spFrameMgr;
	IGLibrary::IGFrame									m_cxLogo;
};

OBJECT_ENTRY_AUTO(__uuidof(IGMultiFrame), CIGMultiFrame)
