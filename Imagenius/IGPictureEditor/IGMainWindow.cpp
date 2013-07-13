// IGMainWindow.cpp : Implementation of CIGMainWindow

#include "stdafx.h"
#include "IGMainWindow.h"
#include "IGRequest.h"
#include <IGLog.h>
#include <gdiplus.h>

// _IIGImageLibraryEvents
_ATL_FUNC_INFO EventOpenImageInfo = {CC_CDECL, VT_EMPTY, 1, { VT_BSTR }};

using namespace Gdiplus;
using namespace IGLibrary;

#define DESTROY_CCOMPTR(ccomptr)\
	while (ccomptr) {ccomptr.Release();}

// CIGMainWindow
CIGMainWindow::CIGMainWindow() : m_hWnd (NULL)
							, m_gdiplusToken(0)
							, m_hFontAthena(NULL)
							, m_hFontCursif(NULL)
							, m_hFontUI (NULL)
							, m_hFontMessage (NULL)
							, m_hWndStatus (NULL)
							, m_nInputPortId (-1)
{
}

CIGMainWindow::~CIGMainWindow()
{	
}

STDMETHODIMP CIGMainWindow::CreateFrame(OLE_HANDLE hWnd, VARIANT_BOOL bIsServer, BSTR bstrServerIP, BSTR bstrOutputPath)
{
	if (!hWnd)
		return E_INVALIDARG;
	m_hWnd = (HWND)hWnd;
	if (bIsServer)
		m_spConfigMgr->SetServerConfig(bstrServerIP);
	if (bstrOutputPath)
		m_spConfigMgr->SetOutputPath(bstrOutputPath);
	if (!initAll())
		return E_FAIL;
	IGRequest::SetMainWindow (this);
	::SetWindowSubclass (m_hWnd, StaticHookMainWindowProc, 0, (DWORD_PTR)this);
	return S_OK;
}

STDMETHODIMP CIGMainWindow::DestroyFrame()
{
	::RemoveWindowSubclass (m_hWnd, StaticHookMainWindowProc, 0);
	if (m_spSocket)
	{
		m_spSocket->DestroyConnection();
		DESTROY_CCOMPTR (m_spSocket)
	}
	if (m_spLayerManager)
	{
		m_spLayerManager->DestroyFrame();
		DESTROY_CCOMPTR (m_spLayerManager)
	}
	if (m_spHistoryManager)
	{
		m_spHistoryManager->DestroyFrame();	
		CComQIPtr <IOleObject> spOleObject (m_spHistoryManager);
		spOleObject->SetClientSite (NULL);	
		DESTROY_CCOMPTR (m_spHistoryManager)
	}
	if (m_spDockPanelLeft)
	{
		m_spDockPanelLeft->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_spDockPanelLeft);
		spOleObject->SetClientSite (NULL);	
		DESTROY_CCOMPTR (m_spDockPanelLeft)
	}
	if (m_spDockPanelRight)
	{
		m_spDockPanelRight->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_spDockPanelRight);
		spOleObject->SetClientSite (NULL);	
		DESTROY_CCOMPTR (m_spDockPanelRight)
	}
	if (m_spDockPanelBottom)
	{
		m_spDockPanelBottom->DestroyFrame();	
		CComQIPtr <IOleObject> spOleObject (m_spDockPanelBottom);
		spOleObject->SetClientSite (NULL);
		DESTROY_CCOMPTR (m_spDockPanelBottom)
	}
	if (m_spMenuBar)
	{
		m_spMenuBar->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_spMenuBar);
		spOleObject->SetClientSite (NULL);
		DESTROY_CCOMPTR (m_spMenuBar)
	}
	if (m_spMultiFrame)
	{
		m_spMultiFrame->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_spMultiFrame);
		spOleObject->SetClientSite (NULL);
		DESTROY_CCOMPTR (m_spMultiFrame)
	}
	if (m_spWorkspace)
	{
		m_spWorkspace->DestroyFrame();
		DESTROY_CCOMPTR (m_spWorkspace)
	}
	if (m_spImageLibrary)
	{
		m_spImageLibrary->DestroyFrame();	
		IDispEventSimpleImpl <1, CIGMainWindow, &__uuidof(_IIGImageLibraryEvents)> ::DispEventUnadvise (m_spImageLibrary);
		CComQIPtr <IOleObject> spOleObject (m_spImageLibrary);
		spOleObject->SetClientSite (NULL);	
		DESTROY_CCOMPTR (m_spImageLibrary)
	}
	m_spConfigMgr->SaveConfiguration();
	DESTROY_CCOMPTR (m_spConfigMgr)	
	if (m_hFontAthena)
		::RemoveFontMemResourceEx (m_hFontAthena);
	if (m_hFontCursif)
		::RemoveFontMemResourceEx (m_hFontCursif);
	if (m_hFontUI)
		::DeleteObject(m_hFontUI);
	if (m_hFontMessage)
		::DeleteObject(m_hFontMessage);

	Format::UnregisterFormats();
	IGRequest::Release();

	GdiplusShutdown (m_gdiplusToken);
	IGLibrary::Term();
	return S_OK;
}

HRESULT CIGMainWindow::Connect (SHORT nInputPortId, BSTR bstrServerIp, OLE_HANDLE hWndStatus)
{
	if (!m_spSocket)
		m_spSocket.CoCreateInstance (CLSID_IGSocket);	
	IGLog log ((HWND)hWndStatus);
	m_hWndStatus = (HWND)hWndStatus;
	m_nInputPortId = nInputPortId;
	m_spbstrServerIp = bstrServerIp;
	HRESULT hr = m_spSocket->CreateConnection ((OLE_HANDLE)::GetParent (m_hWnd), nInputPortId, bstrServerIp);
	if (hr == S_OK){
		m_spConfigMgr->SetServerConfig(bstrServerIp, nInputPortId);
		wchar_t szTemp[300];
		::wsprintf (szTemp, L"Connection OK - Listening on port %d", nInputPortId);
		log.Add (szTemp);
	}
	return hr;
}

HRESULT CIGMainWindow::ShowOwner (BOOL bVisible)
{
	return ::ShowWindow (::GetAncestor (m_hWnd, GA_ROOTOWNER), bVisible ? SW_SHOW : SW_HIDE) ? S_OK : E_FAIL;
}

HRESULT CIGMainWindow::GetIGInterface (REFIID iid, OLE_HANDLE *ppvObject)
{
	if (::IsEqualGUID (iid, IID_IIGMultiFrame))
		*ppvObject = (OLE_HANDLE)m_spMultiFrame.p;
	else if (::IsEqualGUID (iid, IID_IIGImageLibrary))
		*ppvObject = (OLE_HANDLE)m_spImageLibrary.p;
	else if (::IsEqualGUID (iid, IID_IIGWorkspace))
		*ppvObject = (OLE_HANDLE)m_spWorkspace.p;
	else if (::IsEqualGUID (iid, IID_IIGLayerManager))
		*ppvObject = (OLE_HANDLE)m_spLayerManager.p;
	else if (::IsEqualGUID (iid, IID_IIGHistoryManager))
		*ppvObject = (OLE_HANDLE)m_spHistoryManager.p;
	else if (::IsEqualGUID (iid, IID_IIGPropertyManager))
		*ppvObject = (OLE_HANDLE)m_spPropertyMgr.p;
	else 
		*ppvObject = NULL;
	return (*ppvObject) ? S_OK : E_FAIL;
}

STDMETHODIMP CIGMainWindow::SetClientSite(LPOLECLIENTSITE pSite)
{
    HRESULT hr = CComControlBase::IOleObject_SetClientSite(pSite);
    if(!m_pFont && pSite)
    {
        hr = GetAmbientFontDisp(&m_pFont);
    }
    GetAmbientBackColor(m_clrBackColor);
    GetAmbientForeColor(m_clrForeColor);
    return S_OK;
}

LRESULT CIGMainWindow::OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = LOWORD (lParam);
	rc.bottom = HIWORD (lParam);
	int nWidth = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;
	// new menubar coordinates
	rc.right = nWidth;
	rc.bottom = IGMENUBAR_HEIGHT;
	if (m_spMenuBar)
		m_spMenuBar->Move (&rc);

	// new multiframe coordinates
	rc.left = m_spDockPanelLeft ? IGDOCKPANEL_DOCKEDSIZE : 0;
	rc.right = nWidth - IGDOCKPANEL_DOCKEDSIZE;
	rc.top = m_spMenuBar ? IGMENUBAR_HEIGHT : 0;
	rc.bottom = nHeight - IGDOCKPANEL_DOCKEDSIZE;
	m_spMultiFrame->Move (&rc);	

	// new dockpanels coordinates
	rc.left = 0;
	rc.top = m_spMenuBar ? IGMENUBAR_HEIGHT : 0;
	rc.right = nWidth;
	rc.bottom = nHeight;
	if (m_spDockPanelLeft)
		m_spDockPanelLeft->Move (&rc);
	m_spDockPanelRight->Move (&rc);
	m_spDockPanelBottom->Move (&rc);

	rc.bottom -= IGDOCKPANEL_DOCKEDSIZE;
	m_spLayerManager->Move (&rc);

	m_spTabBar->Move ((nHeight - IGMENUBAR_HEIGHT - IGDOCKPANEL_DOCKEDSIZE + TABBAR_HEIGHTOFFSET) / 2);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGMainWindow::OnImageLibOpen(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	RECT rcSender;
	::ZeroMemory (&rcSender, sizeof (RECT));
	::GetWindowRect (hWnd, &rcSender);
	if (m_spMenuBar)
		m_spMenuBar->Select (wId);
	m_spImageLibrary->ShowFrame (&rcSender, wId);
	return TRUE;
}

LRESULT CIGMainWindow::OnImageLibClosed(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (m_spMenuBar)
		m_spMenuBar->UnSelect (wCode);
	return TRUE;
}

void CIGMainWindow::OnOpenImage (BSTR bstrImagePath)
{
	m_spWorkspace->AddFrame (bstrImagePath);
}

LRESULT CIGMainWindow::OnOptions(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	// TODO : Options dialog
	return TRUE;
}

LRESULT CIGMainWindow::OnAccount(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	// TODO : Account dialog
	return TRUE;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGMainWindow::StaticHookMainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
															UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGMainWindow* pThis = (CIGMainWindow*)dwRefData;
	if( pThis != NULL )
		return pThis->HookMainWindowProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGMainWindow::HookMainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;
	BOOL bHandled = false;
	MESSAGE_HANDLER(WM_SIZE, OnResize)
	COMMAND_ID_HANDLER(IGM_MAINMENU_OPENIMAGELIB, OnImageLibOpen)
	COMMAND_ID_HANDLER(IGM_IMAGELIB_CLOSE, OnImageLibClosed)
	COMMAND_ID_HANDLER(IGM_MAINMENU_OPTIONS, OnOptions)
	COMMAND_ID_HANDLER(IGM_MAINMENU_ACCOUNT, OnAccount)
	COMMAND_ID_HANDLER(IGM_TOOLS_RESIZE, OnToolsResize)
	COMMAND_ID_HANDLER(IGM_TOOLS_ROTATE, OnToolsRotate)
	COMMAND_ID_HANDLER(IGM_TOOLS_BRIGHTNESS, OnToolsBrightness)
	COMMAND_ID_HANDLER(IGM_TOOLS_CONTRAST, OnToolsContrast)
	COMMAND_ID_HANDLER(IGM_TOOLS_COLOR, OnToolsColor)
	COMMAND_ID_HANDLER(IGM_TOOLS_REDEYE, OnToolsRedeye)
	COMMAND_ID_HANDLER(IGM_TOOLS_HISTOGRAM_EQUALIZE, OnToolsHistogramEqualize)
	COMMAND_ID_HANDLER(IGM_TOOLS_FILTER_ERODE, OnToolsFilterErode)
	COMMAND_ID_HANDLER(IGM_TOOLS_FILTER_DILATE, OnToolsFilterDilate)
	COMMAND_ID_HANDLER(IGM_TOOLS_FILTER_BLUR, OnToolsFilterBlur)
	COMMAND_ID_HANDLER(IGM_TOOLS_FILTER_GAUSSIAN, OnToolsFilterGaussian)
	COMMAND_ID_HANDLER(IGM_TOOLS_FILTER_GRADIENT, OnToolsFilterGradient)
	COMMAND_ID_HANDLER(IGM_TOOLS_FILTER_ZEROCROSS, OnToolsFilterZerocross)
	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}