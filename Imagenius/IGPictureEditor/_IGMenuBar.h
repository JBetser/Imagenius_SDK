// IGMenuBar.h : Declaration of the CIGMenuBar
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "IGPictureEditor_i.h"
#include "_IIGMenuBarEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// CIGMenuBar
class ATL_NO_VTABLE CIGMenuBar :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CAxDialogImpl<CIGMenuBar>,
	public CComCoClass<CIGMenuBar, &CLSID_IGMenuBar>,
	public IDispatchImpl<IIGMenuBar, &IID_IIGMenuBar, &LIBID_IGPictureEditorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:


	CIGMenuBar() : m_nNbButtons(0)
	{
	}

	STDMETHODIMP CreateFrame(OLE_HANDLE hWnd, LPRECT p_rc, BSTR xmlConfig);

	enum { IDD = IDD_IGMENUBAR };

BEGIN_MSG_MAP(CIGMenuBar)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	CHAIN_MSG_MAP(CAxDialogImpl<CIGMenuBar>)
END_MSG_MAP()

BEGIN_COM_MAP(CIGMenuBar)
	COM_INTERFACE_ENTRY(IIGMenuBar)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

public:

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CAxDialogImpl<CIGMenuBar>::OnInitDialog(uMsg, wParam, lParam, bHandled);
		bHandled = TRUE;
		return 1;  // Let the system set the focus
	}

private:
	int m_nNbButtons;
};

//OBJECT_ENTRY_AUTO(__uuidof(IGMenuBar), CIGMenuBar)
