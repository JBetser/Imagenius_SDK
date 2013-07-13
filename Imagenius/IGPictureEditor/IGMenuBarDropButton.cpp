// IGMenuBarDropButton.cpp : Implementation of CIGMenuBarDropButton
#include "stdafx.h"
#include "IGMenuBarDropButton.h"
#include "commandids.h"
#include <msxml2.h>
#include <gdiplus.h>
using namespace Gdiplus;

#define IGMENUBARDROPBUTTON_DRAWNORMAL				0
#define IGMENUBARDROPBUTTON_DRAWSELECT				1
#define IGMENUBARDROPBUTTON_DRAWMOUSEOVER			2
#define IGMENUBARDROPBUTTON_COLOR_SELECT			Color(255, 97, 190, 132)
#define IGMENUBARDROPBUTTON_COLOR_MOUSEOVER			Color(255, 97, 190, 132)
#define IGMENUBARDROPBUTTON_COLOR_CENTERLIGHT		Color(150, 255, 255, 255)
#define IGMENUBARDROPBUTTON_COLOR_OUTLIGHT			Color(50, 255, 255, 255)
#define IGMENUBARDROPBUTTON_COLOR_SEPARATOR			Color(255, 132, 129, 115)
#define IGMENUBARDROPBUTTON_COLOR_MOUSEHOVERMAINMENU	Color(255, 52, 148, 214)
#define IGMENUBARDROPBUTTON_COLOR_FONT_MOUSEHOVERMAINMENU	Color(255, 34, 98, 141)
#define IGMENUBARDROPBUTTON_COLOR_FONT_DISABLED		Color(255, 170, 170, 170)

// CIGMenuBarDropButton
CIGMenuBarDropButton::CIGMenuBarDropButton()	: m_ctlButton(_T("Button"), this, 1)
												, m_nIdClickEvent (IGM_SUBMENU)
												, m_bIsMainMenu (false)												
												, m_idxSubMenu (-1)
												, m_hMenuBarParent (NULL)
												, m_nMode (IGMENUBARDROPBUTTON_DRAWNORMAL)
												, m_bSubMenuOpened (false)
												, m_hFont (NULL)
												, m_bEnabled (true)
												, m_nIconId (-1)
{
	m_bWindowOnly = TRUE;
}

CIGMenuBarDropButton::~CIGMenuBarDropButton()
{
}

STDMETHODIMP CIGMenuBarDropButton::CreateFrame(OLE_HANDLE hWnd, LPRECT prc, BSTR xmlConfig, VARIANT_BOOL bIsMainMenu, LONG idxButton, LONG idxSubMenu)
{
	CComPtr <IXMLDOMDocument> spXMLDoc;
	HRESULT hr = ::CoCreateInstance(CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
									IID_IXMLDOMDocument, (void**)&spXMLDoc);
	if (FAILED(hr))
		return E_FAIL;

	VARIANT_BOOL bSucceeded;
	hr = spXMLDoc->loadXML (xmlConfig, &bSucceeded);
	if ((hr != S_OK) || !bSucceeded)
		return E_FAIL;

	CComPtr <IXMLDOMNode> spMenuBarDropButton;
	hr = spXMLDoc->get_firstChild (&spMenuBarDropButton);
	if ((hr != S_OK) || !spMenuBarDropButton)
		return E_FAIL;

	CComPtr <IXMLDOMNamedNodeMap> spAttributes;
	spMenuBarDropButton->get_attributes (&spAttributes);
	CComPtr <IXMLDOMNode> spNodeButtonName;	
	spAttributes->getNamedItem (CComBSTR (L"name"), &spNodeButtonName);
	CComVariant spButtonName;	
	spNodeButtonName->get_nodeValue (&spButtonName);

	// button command idx
	CComPtr <IXMLDOMNode> spNodeButtonClickEvent;
	spAttributes->getNamedItem (CComBSTR (L"click"), &spNodeButtonClickEvent);
	if (spNodeButtonClickEvent)
	{
		CComVariant spButtonClickEvent;
		spNodeButtonClickEvent->get_nodeValue (&spButtonClickEvent);
		::swscanf_s (spButtonClickEvent.bstrVal, L"%d", &m_nIdClickEvent);
	}

	// icon idx
	CComPtr <IXMLDOMNode> spNodeButtonIcon;
	spAttributes->getNamedItem (CComBSTR (L"icon"), &spNodeButtonIcon);
	if (spNodeButtonIcon)
	{
		CComVariant spButtonIcon;
		spNodeButtonIcon->get_nodeValue (&spButtonIcon);
		::swscanf_s (spButtonIcon.bstrVal, L"%d", &m_nIconId);
		HRSRC hResIcon = ::FindResourceW (getInstance(), MAKEINTRESOURCE(m_nIconId), L"PNG");
		m_cxIcon.LayerCreate(0);
		m_cxIcon.GetLayer(0)->LoadResource (hResIcon, CXIMAGE_FORMAT_PNG, getInstance());
	}

	// this idx must be set before button creation 
	m_bIsMainMenu = (bIsMainMenu == VARIANT_TRUE);
	_U_RECT urc (prc);
	HWND hMenuBarButton = CComControl<CIGMenuBarDropButton>::Create ((HWND)hWnd, urc, spButtonName.bstrVal, WS_CHILD | WS_VISIBLE);
	if (!hMenuBarButton)
		return E_FAIL;
	m_hMenuBarParent = (HWND)hWnd;
	m_idxSubMenu = idxSubMenu;

	::SetWindowSubclass (m_ctlButton.m_hWnd, StaticHookButtonProc, 0, (DWORD_PTR)this);
	
	m_wsButtonText  = spButtonName.bstrVal;
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::DestroyFrame()
{
	::RemoveWindowSubclass (m_ctlButton.m_hWnd, StaticHookButtonProc, 0);
	if (m_hWnd)
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::get_IdxSubMenu(LONG *p_idxSubMenu)
{
	*p_idxSubMenu = m_idxSubMenu;
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::Select()
{
	if (!m_bSubMenuOpened)
	{
		m_bSubMenuOpened = true;
		m_nMode = IGMENUBARDROPBUTTON_DRAWMOUSEOVER;
		redraw ((HWND)m_ctlButton);
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::UnSelect()
{
	if (m_bSubMenuOpened)
	{
		m_bSubMenuOpened = false;
		m_nMode = IGMENUBARDROPBUTTON_DRAWNORMAL;
		redraw ((HWND)m_ctlButton);
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::Enable()
{
	::EnableWindow (m_hWnd, TRUE);
	redraw ((HWND)m_ctlButton);
	m_bEnabled = true;
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::Disable()
{
	::EnableWindow (m_hWnd, FALSE);
	redraw ((HWND)m_ctlButton);
	m_bEnabled = false;
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::get_IdxButtonClick(LONG *p_idxButtonClick)
{
	*p_idxButtonClick = m_nIdClickEvent;
	return S_OK;
}

STDMETHODIMP CIGMenuBarDropButton::put_Font(OLE_HANDLE hFont)
{
	m_hFont = (HFONT)hFont;
	return S_OK;
}

LRESULT CIGMenuBarDropButton::OnBNClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (m_nIdClickEvent == IGM_SUBMENU)
	{
		m_bSubMenuOpened = true;
		m_nMode = IGMENUBARDROPBUTTON_DRAWMOUSEOVER;
		::PostMessageW (m_hMenuBarParent, WM_COMMAND, MAKEWPARAM (m_nIdClickEvent, IGM_SUBMENU), (LPARAM)m_idxSubMenu);
	}
	else
		::PostMessageW (m_hMenuBarParent, WM_COMMAND, MAKEWPARAM (m_nIdClickEvent, IGM_COMMAND), (LPARAM)hWndCtl);
	bHandled = TRUE;	
	return 0;
}

LRESULT CIGMenuBarDropButton::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (HIWORD(wParam) == IGM_COMMAND)
		::PostMessageW (m_hMenuBarParent, WM_COMMAND, wParam, lParam);
	else
		bHandled = FALSE;	
	return 0;
}

LRESULT CIGMenuBarDropButton::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bIsMainMenu)
	{
		RECT rc;
		::GetWindowRect (m_hWnd, &rc);
		int nWidth = rc.right - rc.left;
		int nHeight = rc.bottom - rc.top;
		rc.left = rc.top = 0;
		Graphics graphics ((HDC)wParam);
		GraphicsPath path;
		path.AddEllipse (Rect (rc.left, rc.top,
								nWidth, nHeight));
		PathGradientBrush pthGrBrush (&path);
		pthGrBrush.SetCenterPoint (PointF ((float)rc.left + 0.7f * (float)nWidth,
										  (float)rc.top + 0.3f * (float)nHeight));
		pthGrBrush.SetCenterColor (IGMENUBARDROPBUTTON_COLOR_CENTERLIGHT);
		Color colors[] = {IGMENUBARDROPBUTTON_COLOR_OUTLIGHT};
		int count = 1;
		pthGrBrush.SetSurroundColors(colors, &count);
		graphics.FillPath (&pthGrBrush, &path);		
	}
	bHandled = TRUE;
	return 0;
}

HRESULT CIGMenuBarDropButton::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	HDC hDC = di.hdcDraw;
	return S_OK;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGMenuBarDropButton::StaticHookButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGMenuBarDropButton* pThis = (CIGMenuBarDropButton*)dwRefData;
	if( pThis != NULL )
		return pThis->HookButtonProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGMenuBarDropButton::HookButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_KILLFOCUS:
		::PostMessage (m_hMenuBarParent, msg, wParam, (LPARAM)m_idxSubMenu);
		if (!m_bSubMenuOpened)
			m_nMode = IGMENUBARDROPBUTTON_DRAWNORMAL;
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			draw (hDC);
			::EndPaint(hWnd, &ps);
			return 0L;	
		}
		break;

	case WM_MOUSEMOVE:
		if (m_nMode != IGMENUBARDROPBUTTON_DRAWMOUSEOVER)
		{
			m_nMode = IGMENUBARDROPBUTTON_DRAWMOUSEOVER;
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_LEAVE | TME_HOVER;
			trackMouseEvent.hwndTrack = hWnd;
			trackMouseEvent.dwHoverTime = 200;
			::TrackMouseEvent (&trackMouseEvent);
			redraw (hWnd);
		}		
		break;

	case WM_MOUSEHOVER:
		if (m_nIdClickEvent == IGM_SUBMENU)
		{
			m_bSubMenuOpened = true;
			m_nMode = IGMENUBARDROPBUTTON_DRAWMOUSEOVER;
			::PostMessageW (m_hMenuBarParent, WM_COMMAND, MAKEWPARAM (m_nIdClickEvent, IGM_SUBMENU), (LPARAM)m_idxSubMenu);
		}
		else
			::PostMessageW (m_hMenuBarParent, WM_COMMAND, MAKEWPARAM (m_nIdClickEvent, IGM_SUBMENU), (LPARAM)-1);
		break;

	case WM_MOUSELEAVE:
		if (!m_bSubMenuOpened)
		{
			m_nMode = IGMENUBARDROPBUTTON_DRAWNORMAL;
			redraw (hWnd);
		}
		break;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		return 0L;

	case WM_LBUTTONUP:
		if (m_nIdClickEvent == IGM_SUBMENU)
		{
			m_bSubMenuOpened = true;
			m_nMode = IGMENUBARDROPBUTTON_DRAWMOUSEOVER;
		}
		::DefSubclassProc(hWnd, WM_LBUTTONDOWN, wParam, lParam);
		break;

	// hacked messages
	case WM_ERASEBKGND:
		return 1L;

	case WM_SHOWWINDOW:
		if  (!(BOOL)wParam)
			m_bSubMenuOpened = false;
		break;
	}
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc(hWnd, msg, wParam, lParam);	

	switch(msg)
	{
		case WM_LBUTTONUP:
			redraw (hWnd);
			break;
	}
	return lRes;
}

LRESULT CIGMenuBarDropButton::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RECT rc;
	GetWindowRect(&rc);
	rc.right -= rc.left;
	rc.bottom -= rc.top;
	rc.top = rc.left = 0;
	if (!m_bIsMainMenu)	// if it is not a main menu move button to the right
		rc.left = rc.bottom - rc.top;
	m_ctlButton.Create(m_hWnd, rc);
	return 0;
}

LRESULT CIGMenuBarDropButton::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return MA_ACTIVATEANDEAT;
}

void CIGMenuBarDropButton::redraw (HWND hWnd)
{
	RECT rcButton,rcMenuBarParent;
	::GetWindowRect (m_hWnd, &rcButton);
	::GetWindowRect (m_hMenuBarParent, &rcMenuBarParent);
	rcButton.left -= rcMenuBarParent.left;
	rcButton.right -= rcMenuBarParent.left;			
	rcButton.top -= rcMenuBarParent.top;
	rcButton.bottom -= rcMenuBarParent.top;
	::InvalidateRect (m_hMenuBarParent, &rcButton, TRUE);
	::UpdateWindow (m_hMenuBarParent);
	::InvalidateRect (m_hWnd, NULL, FALSE);
	::InvalidateRect (hWnd, NULL, FALSE);
}

void CIGMenuBarDropButton::draw (HDC hDC)
{
	RECT rc;
	m_ctlButton.GetWindowRect(&rc);
	rc.right -= rc.left;
	rc.bottom -= rc.top;
	rc.top = rc.left = 0;
	Graphics graphics (hDC);
	graphics.SetSmoothingMode (SmoothingModeAntiAlias);
	SolidBrush trgBrush (Color (255, 0, 0, 0));
	// draw different kinds of selection
	switch (m_nMode)
	{
	case IGMENUBARDROPBUTTON_DRAWNORMAL:
		{
			if (m_bIsMainMenu)
			{
				trgBrush.SetColor (IGMENUBARDROPBUTTON_COLOR_FONT_MOUSEHOVERMAINMENU);
				SolidBrush solBrush (IGMENUBARDROPBUTTON_COLOR_SELECT);
				graphics.FillEllipse (&solBrush, Rect (rc.left, rc.top,
														rc.right - rc.left, rc.bottom - rc.top));
				GraphicsPath path;
				path.AddEllipse (Rect (rc.left, rc.top,
										rc.right - rc.left, rc.bottom - rc.top));
				PathGradientBrush pthGrBrush (&path);
				pthGrBrush.SetCenterPoint (PointF ((float)rc.left + 0.7f * (float)(rc.right - rc.left),
												  (float)rc.top + 0.3f * (float)(rc.bottom - rc.top)));
				pthGrBrush.SetCenterColor (IGMENUBARDROPBUTTON_COLOR_CENTERLIGHT);
				Color colors[] = {IGMENUBARDROPBUTTON_COLOR_OUTLIGHT};
				int count = 1;
				pthGrBrush.SetSurroundColors(colors, &count);
				graphics.FillPath (&pthGrBrush, &path);
			}
			else
			{
				trgBrush.SetColor (IGMENUBARDROPBUTTON_COLOR_FONT_MOUSEHOVERMAINMENU);
			}
		}
		break;
	case IGMENUBARDROPBUTTON_DRAWSELECT:
		{
			SolidBrush selectBrush (IGMENUBARDROPBUTTON_COLOR_SELECT);
			graphics.FillRectangle (&selectBrush, Rect (rc.left + 5, rc.top + 2,
														rc.right - rc.left - 10, rc.bottom - rc.top - 4));
		}
		break;
	case IGMENUBARDROPBUTTON_DRAWMOUSEOVER:
		{
			if (!m_bIsMainMenu)
			{
				SolidBrush selectBrush (IGMENUBARDROPBUTTON_COLOR_MOUSEOVER);
				graphics.FillRectangle (&selectBrush, Rect (rc.left + 5, rc.top + 2,
															rc.right - rc.left - 10, rc.bottom - rc.top - 4));
				Pen penSeparatorBlack(IGMENUBARDROPBUTTON_COLOR_SEPARATOR, 1);
				graphics.DrawRectangle (&penSeparatorBlack, Rect (rc.left + 5, rc.top + 2,
																rc.right - rc.left - 10, rc.bottom - rc.top - 4));
			}
			else
			{
				GraphicsPath path;
				path.AddEllipse (Rect (rc.left, rc.top,
										rc.right - rc.left, rc.bottom - rc.top));
				PathGradientBrush pthGrBrush (&path);
				pthGrBrush.SetCenterPoint (PointF ((float)rc.left + 0.7f * (float)(rc.right - rc.left),
												  (float)rc.top + 0.3f * (float)(rc.bottom - rc.top)));
				pthGrBrush.SetCenterColor (IGMENUBARDROPBUTTON_COLOR_CENTERLIGHT);
				Color colors[] = {IGMENUBARDROPBUTTON_COLOR_MOUSEHOVERMAINMENU};
				int count = 1;
				pthGrBrush.SetSurroundColors(colors, &count);
				graphics.FillPath (&pthGrBrush, &path);
			}
		}
		break;
	}

	if (m_bIsMainMenu && (m_nIconId >= 0))
	{
		drawIcon (hDC);
	}
	else
	{
		FontFamily fontFamily(L"Times New Roman");
		Font font(&fontFamily, 16, FontStyleRegular, UnitPixel);
		PointF      pointF(20.0f, 5.0f);
		SolidBrush  solidBrush (m_bEnabled ? Color(IGMENUBARDROPBUTTON_COLOR_FONT_MOUSEHOVERMAINMENU)
											: Color(IGMENUBARDROPBUTTON_COLOR_FONT_DISABLED));
		StringFormat	format(StringFormat::GenericDefault());
		format.SetAlignment (StringAlignmentCenter);
		graphics.DrawString (m_wsButtonText.c_str(), -1, &font, RectF ((float)rc.left, (float)rc.top + ((float)(rc.bottom - rc.top)) / 8.0f, 
																		(float)(rc.right - rc.left), (float)(rc.bottom - rc.top)), &format, &solidBrush);
	}
	if (m_nIdClickEvent == IGM_SUBMENU)
	{
		GraphicsPath pathTrg;
		if (m_bIsMainMenu)
		{			
			pathTrg.AddLine (PointF ((float)(rc.right - rc.left - 16), (float)(rc.bottom - rc.top) / 2.0f - 3.0f), 
							PointF ((float)(rc.right - rc.left - 10), (float)(rc.bottom - rc.top) / 2.0f - 3.0f));
			pathTrg.AddLine (PointF ((float)(rc.right - rc.left - 13), (float)(rc.bottom - rc.top) / 2.0f + 4.0f), 
							PointF ((float)(rc.right - rc.left - 16), (float)(rc.bottom - rc.top) / 2.0f - 3.0f));
		}
		else
		{
			pathTrg.AddLine (PointF ((float)(rc.right - rc.left - 13), (float)(rc.bottom - rc.top) / 2.0f - 3.0f), 
							PointF ((float)(rc.right - rc.left - 13), (float)(rc.bottom - rc.top) / 2.0f + 3.0f));
			pathTrg.AddLine (PointF ((float)(rc.right - rc.left - 6), (float)(rc.bottom - rc.top) / 2.0f), 
							PointF ((float)(rc.right - rc.left - 13), (float)(rc.bottom - rc.top) / 2.0f - 3.0f));
		}
		graphics.FillPath (&trgBrush, &pathTrg);
	}
}

void CIGMenuBarDropButton::drawIcon (HDC hdc)
{
	RECT rcWnd;
	::GetWindowRect (m_hWnd, &rcWnd);
	rcWnd.right -= rcWnd.left;
	rcWnd.bottom -= rcWnd.top;
	rcWnd.left = rcWnd.top = 0;
	int x = (rcWnd.right - rcWnd.left - m_cxIcon.GetLayer(0)->GetWidth()) / 2;
	int y = (rcWnd.bottom - rcWnd.top - m_cxIcon.GetLayer(0)->GetHeight()) / 2;
	if (x < 0)     
		x = 0;
	if (y < 0)      
		y = 0;	
	m_cxIcon.LayerDrawAll (hdc, x, y);
}