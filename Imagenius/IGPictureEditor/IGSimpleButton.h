// IGSimpleButton.h : Declaration of the CIGSimpleButton
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "ximage.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

typedef enum {
	IGBUTTON_MOUSEOUT	=	1,
	IGBUTTON_MOUSEOVER	=	2,
	IGBUTTON_MOUSEDOWN	=	3	} IGBUTTONSTATE;

// CIGSimpleButton
class ATL_NO_VTABLE CIGSimpleButton :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IIGSimpleButton, &IID_IIGSimpleButton, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public CComCoClass<CIGSimpleButton, &CLSID_IGSimpleButton>
{
public:
	CIGSimpleButton();

	STDMETHODIMP CreateFrame (OLE_HANDLE hParent, SHORT nButtonId, LONG nCode, 
							LPRECT p_rc, BSTR bstrResIconNormal, BSTR bstrResIconPushed, 
							VARIANT_BOOL bDoubleState);
	STDMETHODIMP DestroyFrame ();
	STDMETHODIMP Move (LPRECT p_rc);
	STDMETHODIMP Redraw (VARIANT_BOOL bForce = VARIANT_TRUE);
	STDMETHODIMP put_Pushed (VARIANT_BOOL bPushed);
	STDMETHODIMP get_Pushed (VARIANT_BOOL *p_bPushed);
	STDMETHODIMP SetBackground (LONG cBackground);
	STDMETHODIMP GetHWND (OLE_HANDLE *p_hWnd);

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IGSIMPLEBUTTON)

BEGIN_COM_MAP(CIGSimpleButton)
	COM_INTERFACE_ENTRY(IIGSimpleButton)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IIGSimpleButton

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

private:
	LRESULT WndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	/*
	static LRESULT APIENTRY StaticHookButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);*/

	HRESULT createIcons (LPCWSTR pwIconNormal, LPCWSTR pwIconPushed);
	void drawButton (HDC hDC);
	void drawToolBoxButton (HDC hdc);

	IGBUTTONSTATE		m_eButtonState;
	HWND				m_hParent;
	HWND				m_hWnd;
	CxImage				*m_pCxIconNormal;
	CxImage				*m_pCxIconPushed;
	LONG				m_nLeft;
	LONG				m_nTop;
	LONG				m_nWidth;
	LONG				m_nHeight;
	LONG				m_nIconLeft;
	LONG				m_nIconTop;
	LONG				m_nIconWidth;
	LONG				m_nIconHeight;
	VARIANT_BOOL		m_bDoubleState;
	bool				m_bPushed;
	SHORT				m_nButtonId;
	LONG				m_nCode;
	COLORREF			m_cBackground;
};

OBJECT_ENTRY_AUTO(__uuidof(IGSimpleButton), CIGSimpleButton)
