// MainWindow.cpp : Implementation of CMainWindow
#include "stdafx.h"
#include "MainWindow.h"

// CMainWindow
CMainWindow::CMainWindow() : m_bInstanceError (false)
{
	_ASSERTE (0 && L"LET'S DEBUG !!!!");
	/* provoked crash to debug in release mode
	int* p=0;*p=0;*/
	m_bWindowOnly = TRUE;
}

CMainWindow::~CMainWindow()
{	
}

LRESULT CMainWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_spMainWindow.CoCreateInstance (CLSID_IGMainWindow);
	CComQIPtr <IOleObject> spMainWindowOleObject (m_spMainWindow);
	spMainWindowOleObject->SetClientSite ((IOleClientSite *)this);
	if (FAILED (m_spMainWindow->CreateFrame ((OLE_HANDLE)m_hWnd)))
	{
		m_bInstanceError = true;
		m_spMainWindow.Release();
	}
	return TRUE;
}

LRESULT CMainWindow::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spMainWindow)
	{
		m_spMainWindow->DestroyFrame();
		CComQIPtr <IOleObject> spMainWindowOleObject (m_spMainWindow);
		spMainWindowOleObject->SetClientSite (NULL);
		m_spMainWindow.Release();
	}
	return 0L;
}

STDMETHODIMP CMainWindow::SetClientSite(LPOLECLIENTSITE pSite)
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

LRESULT CMainWindow::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bInstanceError)
		return 0L;
	LRESULT lRes = CComControl<CMainWindow>::OnSetFocus(uMsg, wParam, lParam, bHandled);
	if (m_bInPlaceActive)
	{
		DoVerbUIActivate(&m_rcPos,  NULL);
	}
	return lRes;
}