// IGToolBox.h : Declaration of the CIGToolBox

#pragma once
#include "resource.h"       // main symbols

#include "IGSmartPtr.h"
#include "IGTool.h"
#include "IGPictureEditor_i.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CIGToolBox

class ATL_NO_VTABLE CIGToolBox :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIGToolBox, &CLSID_IGToolBox>,
	public IDispatchImpl<IIGToolBox, &IID_IIGToolBox, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CIGToolBox();

	STDMETHODIMP Create (OLE_HANDLE hOwner, LPRECT p_rc);
	STDMETHODIMP Destroy();
	STDMETHODIMP put_Frame (IDispatch *pDispFrame);
	STDMETHODIMP get_Frame (IDispatch **pDispFrame);
	STDMETHODIMP put_SelectedToolId (LONG nToolId);
	STDMETHODIMP get_SelectedToolId (LONG *p_nToolId);

DECLARE_REGISTRY_RESOURCEID(IDR_IGTOOLBOX)


BEGIN_COM_MAP(CIGToolBox)
	COM_INTERFACE_ENTRY(IIGToolBox)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	static LRESULT CALLBACK StaticToolBoxProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	LRESULT ToolBoxProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT APIENTRY StaticHookFrameProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
													UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookFrameProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	std::list <IGLibrary::IGSmartPtr <IGLibrary::IGTool>>	m_lspTools;
	IGLibrary::IGSmartPtr <IGLibrary::IGTool>				m_spCurrentTool;
	IIGFrame												*m_pSelectedFrame;
	int		m_nRowSize;
	int		m_nNbToolsByRow;
	HWND	m_hWnd;
	HWND	m_hSubclassedFrame;
};

OBJECT_ENTRY_AUTO(__uuidof(IGToolBox), CIGToolBox)
