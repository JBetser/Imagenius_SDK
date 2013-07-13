// IGDockPanel.cpp : Implementation of CIGDockPanel
#include "stdafx.h"
#include "IGDockPanel.h"
#include "IGUserMessages.h"

using namespace Gdiplus;

#define IGDOCKPANEL_BUTTONSIZE			70
#define IGDOCKPANEL_COLOR_BORDER		Color(255, 119, 199, 149)	// Green
#define IGDOCKPANEL_COLOR_BUTTONBORDER	Color(255, 52, 148, 214)	// Blue
#define IGDOCKPANEL_COLOR_BUTTON		Color(100, 52, 148, 214)	// Light Blue
#define IGDOCKPANEL_COLOR_TRIANGLE		Color(100, 0, 0, 0)			// Black
#define IGDOCKPANEL_COLOR_FONT			Color(100, 0, 0, 0)			// Black
#define IGDOCKPANEL_TICKSIZE			5
#define IGDOCKPANEL_TIME				10	// tick = 10ms

// _IIGImageLibraryEvents
_ATL_FUNC_INFO EventPanelOpenInfo = {CC_CDECL, VT_EMPTY, 0, 0};

// CIGDockPanel
CIGDockPanel::CIGDockPanel()	: m_hParent (NULL)
								, m_eSide (IGDOCKPANELSIDE_LEFT)
								, m_eMode (IGDOCKPANELMODE_CLOSED)
								, m_nNbTicks (0)
								, m_nDockSize (0)
								, m_hPopupWnd (NULL)								
								, m_bMouseInside (false)
								, m_hInside (NULL)
								, m_bDoNotLeave (false)
{
	ZeroMemory (&m_rcParent, sizeof (RECT));
	ZeroMemory (&m_rcButton, sizeof (RECT));
}

STDMETHODIMP CIGDockPanel::CreateFrame (OLE_HANDLE hWnd, LPRECT p_rc, LONG nSide, LONG nDockSize, LONG nStyle)
{
	if (!hWnd)
		return E_INVALIDARG;	
	m_eSide = (IGDOCKPANELSIDE)nSide;	
	Move (p_rc);
	_U_RECT urc (&m_rcDockPanel);
	HWND hDockPanel = CComControl<CIGDockPanel>::Create ((HWND)hWnd, urc, L"DockPanel", WS_CHILD | WS_VISIBLE);
	if (!hDockPanel)
		return E_FAIL;
	m_hParent = (HWND)hWnd;	
	m_nDockSize = nDockSize;
	m_nNbMaxTicks = nDockSize / IGDOCKPANEL_TICKSIZE;
	::SetWindowSubclass (hDockPanel, StaticHookPanelProc, 0, (DWORD_PTR)this);

	m_hPopupWnd = ::CreateWindowW (L"Button", L"PopupDockPanel",
		WS_POPUP | nStyle, p_rc->left, p_rc->top, p_rc->right - p_rc->left,
		p_rc->bottom - p_rc->top, hDockPanel, NULL,
		_AtlBaseModule.GetModuleInstance(), 0);
	if (!m_hPopupWnd)
		return E_FAIL;

	::SetWindowSubclass (m_hPopupWnd, StaticHookPopupPanelProc, 0, (DWORD_PTR)this);
	return S_OK;
}

STDMETHODIMP CIGDockPanel::DestroyFrame ()
{
	freeDropTargets();
	if (!m_hWnd)
		return S_OK;
	unregisterControls ();
	::RemoveWindowSubclass (m_hPopupWnd, StaticHookPopupPanelProc, 0);
	::RemoveWindowSubclass (m_hWnd, StaticHookPanelProc, 0);
	DestroyWindow();
	m_hWnd = NULL;
	return S_OK;
}

STDMETHODIMP CIGDockPanel::Move (LPRECT p_rc)
{
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
		m_rcDockPanel.left = p_rc->left;
		m_rcDockPanel.top = p_rc->top;
		m_rcDockPanel.right = p_rc->left + IGDOCKPANEL_DOCKEDSIZE + m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		m_rcDockPanel.bottom = p_rc->bottom - IGDOCKPANEL_DOCKEDSIZE;
		break;
	case IGDOCKPANELSIDE_RIGHT:
		m_rcDockPanel.left = p_rc->right - IGDOCKPANEL_DOCKEDSIZE - m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		m_rcDockPanel.top = p_rc->top;
		m_rcDockPanel.right = p_rc->right + m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		m_rcDockPanel.bottom = p_rc->bottom - IGDOCKPANEL_DOCKEDSIZE;
		break;
	case IGDOCKPANELSIDE_TOP:
		m_rcDockPanel.left = p_rc->left + IGDOCKPANEL_DOCKEDSIZE;
		m_rcDockPanel.top = p_rc->top;
		m_rcDockPanel.right = p_rc->right - IGDOCKPANEL_DOCKEDSIZE;
		m_rcDockPanel.bottom = p_rc->top + IGDOCKPANEL_DOCKEDSIZE + m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		break;
	case IGDOCKPANELSIDE_BOTTOM:
		m_rcDockPanel.left = p_rc->left + IGDOCKPANEL_DOCKEDSIZE;
		m_rcDockPanel.top = p_rc->bottom - IGDOCKPANEL_DOCKEDSIZE - m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		m_rcDockPanel.right = p_rc->right - IGDOCKPANEL_DOCKEDSIZE;
		m_rcDockPanel.bottom = p_rc->bottom + m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		break;
	default:	// unknown side
		return E_INVALIDARG;
	}
	// compute button position for further refresh
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
	case IGDOCKPANELSIDE_RIGHT:						
		m_rcButton.left = 0;
		m_rcButton.top = (m_rcDockPanel.bottom - m_rcDockPanel.top - IGDOCKPANEL_BUTTONSIZE) / 2;
		m_rcButton.right = m_rcDockPanel.right - m_rcDockPanel.left;
		m_rcButton.bottom = (m_rcDockPanel.bottom - m_rcDockPanel.top + IGDOCKPANEL_BUTTONSIZE) / 2;
		break;
	case IGDOCKPANELSIDE_TOP:
	case IGDOCKPANELSIDE_BOTTOM:	
		m_rcButton.left = (m_rcDockPanel.right - m_rcDockPanel.left - IGDOCKPANEL_BUTTONSIZE) / 2;
		m_rcButton.top = 0;
		m_rcButton.right = (m_rcDockPanel.right - m_rcDockPanel.left + IGDOCKPANEL_BUTTONSIZE) / 2;
		m_rcButton.bottom = m_rcDockPanel.right - m_rcDockPanel.left;
		break;
	}
	if (m_hWnd)
	{
		switch (m_eSide)
		{
		case IGDOCKPANELSIDE_LEFT:
			::MoveWindow (m_hWnd, m_rcDockPanel.left, m_rcDockPanel.top,
							m_rcDockPanel.right - m_rcDockPanel.left, m_rcDockPanel.bottom - m_rcDockPanel.top, TRUE);
			break;
		case IGDOCKPANELSIDE_RIGHT:
			::MoveWindow (m_hWnd, m_rcDockPanel.left, m_rcDockPanel.top,
							m_rcDockPanel.right - m_rcDockPanel.left, m_rcDockPanel.bottom - m_rcDockPanel.top, TRUE);
			break;
		case IGDOCKPANELSIDE_TOP:
			::MoveWindow (m_hWnd, m_rcDockPanel.left, m_rcDockPanel.top,
							m_rcDockPanel.right - m_rcDockPanel.left, m_rcDockPanel.bottom - m_rcDockPanel.top, TRUE);
			break;
		case IGDOCKPANELSIDE_BOTTOM:
			::MoveWindow (m_hWnd, m_rcDockPanel.left, m_rcDockPanel.top,
							m_rcDockPanel.right - m_rcDockPanel.left, m_rcDockPanel.bottom - m_rcDockPanel.top, TRUE);
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CIGDockPanel::GetClientRECT (LPRECT p_rc)
{
	if (!p_rc)
		return E_INVALIDARG;
	RECT rcPopupWnd = m_rcDockPanel;
	rcPopupWnd.right -= rcPopupWnd.left;
	rcPopupWnd.bottom -= rcPopupWnd.top;
	rcPopupWnd.left = rcPopupWnd.top = 0;
	// compute rectangle coordinates
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
		rcPopupWnd.right = m_nDockSize;
		break;
	case IGDOCKPANELSIDE_RIGHT:						
		rcPopupWnd.left = 0;
		rcPopupWnd.right = m_nDockSize;
		break;
	case IGDOCKPANELSIDE_TOP:
		rcPopupWnd.bottom = m_nDockSize;
		break;
	case IGDOCKPANELSIDE_BOTTOM:			
		rcPopupWnd.top = 0;
		rcPopupWnd.bottom = m_nDockSize;
		break;
	default:
		return E_FAIL;
	}
	p_rc->left = rcPopupWnd.left;
	p_rc->right = rcPopupWnd.right;
	p_rc->top = rcPopupWnd.top;
	p_rc->bottom = rcPopupWnd.bottom;
	return S_OK;
}

STDMETHODIMP CIGDockPanel::RegisterControls (OLE_HANDLE hWnd)
{
	registerControls ((HWND)hWnd);
	return S_OK;
}

STDMETHODIMP CIGDockPanel::UnRegisterControls (OLE_HANDLE hWnd)
{
	unregisterControls ((HWND)hWnd);
	return S_OK;
}

STDMETHODIMP CIGDockPanel::SetString (BSTR bstrName)
{
	m_wsDockName = bstrName;
	return S_OK;
}

STDMETHODIMP CIGDockPanel::GetHWND (OLE_HANDLE *p_hWnd)
{
	if (!p_hWnd)
		return E_INVALIDARG;
	*p_hWnd = (OLE_HANDLE)m_hPopupWnd;
	return S_OK;
}

STDMETHODIMP CIGDockPanel::RedrawControls()
{
	::InvalidateRect (m_hPopupWnd, NULL, TRUE);
	for (std::list<HWND>::const_iterator iterControl = m_lControls.begin(); 
		iterControl != m_lControls.end();
		++iterControl)
	{
		::InvalidateRect (*iterControl, NULL, FALSE);
	}
	return S_OK;
}

STDMETHODIMP CIGDockPanel::AddDropTarget (IDropTarget *pDropTarget)
{
	if (!m_hWnd)
		return E_FAIL;
	if (!pDropTarget)
		return E_INVALIDARG;
	if (!pDropTarget)
		return E_INVALIDARG;
	IGDropTarget *pIGTarget = dynamic_cast <IGDropTarget*> (pDropTarget);	
	if (!pIGTarget)
		return E_INVALIDARG;
	initializeController (m_hWnd, pIGTarget);
	return S_OK;
}

void CIGDockPanel::registerControls (HWND hWnd)
{
	if (hWnd)
	{	
		HWND hControl = (HWND)hWnd;
		if (contains (hWnd))
			return;
		::SetWindowSubclass (hWnd, StaticHookTransferMouseEventProc, 0, (DWORD_PTR)this);
		m_lControls.push_back (hWnd);
	}
	else
	{
		::EnumChildWindows (m_hPopupWnd, StaticEnumRegisterChildProc, (LPARAM)this);
	}
}

void CIGDockPanel::unregisterControls (HWND hWnd)
{
	if (hWnd)
	{
		if (!contains (hWnd))
			return;
		::RemoveWindowSubclass (hWnd, StaticHookTransferMouseEventProc, 0);
		m_lControls.remove (hWnd);
	}
	else
	{
		::EnumChildWindows (m_hPopupWnd, StaticEnumUnregisterChildProc, (LPARAM)this);
	}
}

bool CIGDockPanel::contains (HWND hWnd)
{
	for (std::list<HWND>::const_iterator iterControl = m_lControls.begin(); 
		iterControl != m_lControls.end();
		++iterControl)
	{
		if ((*iterControl) == hWnd)
			return true;
	}
	return false;
}

void CIGDockPanel::movePopup ()
{
	m_nNbTicks = ((m_eMode == IGDOCKPANELMODE_POPUP) ? m_nNbTicks + 1 : m_nNbTicks - 1);
	if (m_nNbTicks > m_nNbMaxTicks)
		m_nNbTicks = m_nNbMaxTicks;
	if (m_nNbTicks < 0)
		m_nNbTicks = 0;
	RECT rcPopup;
	::GetWindowRect (m_hWnd, &rcPopup);
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
		rcPopup.right += m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		break;
	case IGDOCKPANELSIDE_RIGHT:
		rcPopup.left -= m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		break;
	case IGDOCKPANELSIDE_TOP:
		rcPopup.bottom += m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		break;
	case IGDOCKPANELSIDE_BOTTOM:
		rcPopup.top -= m_nNbTicks * IGDOCKPANEL_TICKSIZE;
		break;
	}
	::MoveWindow (m_hPopupWnd, rcPopup.left, rcPopup.top,
							rcPopup.right - rcPopup.left, rcPopup.bottom - rcPopup.top, TRUE);
	// correcting coordinates due to moving
	::EnumChildWindows (m_hPopupWnd, StaticEnumMoveChildProc, (LPARAM)this);
}

HRESULT CIGDockPanel::OnDrawAdvanced (ATL_DRAWINFO& di)
{
	return S_OK;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGDockPanel::StaticHookPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGDockPanel* pThis = (CIGDockPanel*)dwRefData;
	if( pThis != NULL )
		return pThis->HookPanelProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGDockPanel::HookPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			draw (hDC, &ps);
			::EndPaint(hWnd, &ps);
			return 0L;	
		}
		break;

	case WM_MOUSEMOVE:
		if (m_eMode == IGDOCKPANELMODE_CLOSED)
		{
			m_eMode = IGDOCKPANELMODE_WAIT;
			::InvalidateRect (m_hWnd, &m_rcButton, TRUE);
			::UpdateWindow (m_hWnd);
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_HOVER | TME_LEAVE;
			trackMouseEvent.hwndTrack = hWnd;
			trackMouseEvent.dwHoverTime = 200;
			::TrackMouseEvent (&trackMouseEvent);
		}		
		break;	

	case WM_MOUSEHOVER:
		if (m_eMode == IGDOCKPANELMODE_WAIT)
		{
			openPopup();			
		}
		break;

	case WM_MOUSELEAVE:
		if (m_eMode == IGDOCKPANELMODE_WAIT)
		{
			m_eMode = IGDOCKPANELMODE_CLOSED;
			::InvalidateRect (m_hWnd, &m_rcButton, TRUE);
			::UpdateWindow (m_hWnd);
		}
		break;

	case WM_LBUTTONUP:
		if ((m_eMode == IGDOCKPANELMODE_CLOSED) || 
			(m_eMode == IGDOCKPANELMODE_WAIT))
		{
			openPopup();
		}
		break;

	// hacked messages
	case WM_ERASEBKGND:
		eraseBkgnd ((HDC)wParam);
		return 1L;
	}
	// Default behavior using the old window proc
	return ::DefSubclassProc(hWnd, msg, wParam, lParam);
}

void CIGDockPanel::openPopup()
{
	if (m_nNbTicks == 0)
	{
		Fire_OnPanelOpening();
		// now controls are added to DockPanel, register them
		registerControls ();
		m_eMode = IGDOCKPANELMODE_POPUP;
		movePopup ();
		::ShowWindow (m_hPopupWnd, SW_SHOW);	
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hPopupWnd;
		trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
		::TrackMouseEvent (&trackMouseEvent);
		::SetTimer (m_hPopupWnd, 1, IGDOCKPANEL_TIME, NULL);
	}
}

void CIGDockPanel::closePopup()
{
	if (m_eMode == IGDOCKPANELMODE_POPUP)
	{
		m_eMode = IGDOCKPANELMODE_CLOSING;
		unregisterControls ();
		Fire_OnPanelClosing();
		::SetTimer (m_hPopupWnd, 1, IGDOCKPANEL_TIME, NULL);
	}
}

void CIGDockPanel::draw (HDC hdc, PAINTSTRUCT *p_ps)
{	
	Rect rcButton;	
	RectF rcDockTitle;	
	// compute rectangle coordinates
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
	case IGDOCKPANELSIDE_RIGHT:	
		rcButton = Rect (0, (m_rcDockPanel.bottom - m_rcDockPanel.top - IGDOCKPANEL_BUTTONSIZE) / 2,
											m_rcDockPanel.right - m_rcDockPanel.left - 1,
											IGDOCKPANEL_BUTTONSIZE - 1);
		rcDockTitle = RectF (0.0f, 0.0f, (float)(m_rcDockPanel.right - m_rcDockPanel.left),
							(float)(m_rcDockPanel.bottom - m_rcDockPanel.top - IGDOCKPANEL_BUTTONSIZE) / 2.0f);
		break;
	case IGDOCKPANELSIDE_TOP:
	case IGDOCKPANELSIDE_BOTTOM:			
		rcButton = Rect ((m_rcDockPanel.right - m_rcDockPanel.left - IGDOCKPANEL_BUTTONSIZE) / 2, 0,
											IGDOCKPANEL_BUTTONSIZE - 1,
											m_rcDockPanel.bottom - m_rcDockPanel.top - 1);
		rcDockTitle = RectF (0.0f, 0.0f, (float)(m_rcDockPanel.right - m_rcDockPanel.left - IGDOCKPANEL_BUTTONSIZE) / 2.0f,
							(float)(m_rcDockPanel.bottom - m_rcDockPanel.top));
		break;
	default:
		return;
	}

	Graphics graphics (hdc);
	graphics.SetClip (Rect (p_ps->rcPaint.left, p_ps->rcPaint.top,
							p_ps->rcPaint.right - p_ps->rcPaint.left,
							p_ps->rcPaint.bottom - p_ps->rcPaint.top));

	// draw blue square in waiting mode
	if (m_eMode == IGDOCKPANELMODE_WAIT)
	{		
		SolidBrush solBrushButton (IGDOCKPANEL_COLOR_BUTTON);
		graphics.FillRectangle (&solBrushButton, rcButton);

		// draw blue square with black triangle in closed mode
		Pen penButtonBorder (IGDOCKPANEL_COLOR_BUTTONBORDER);
		graphics.DrawRectangle (&penButtonBorder, rcButton);		
	}
	// draw black triangle in closed mode	
	Point trgPoints [3];	
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
		trgPoints [0] = Point (rcButton.X + 2, rcButton.Y + 2);
		trgPoints [1] = Point (rcButton.GetRight() - 2, (m_rcDockPanel.bottom - m_rcDockPanel.top) / 2);
		trgPoints [2] = Point (rcButton.X + 2, rcButton.GetBottom() - 2);
		break;
	case IGDOCKPANELSIDE_RIGHT:	
		trgPoints [0] = Point (rcButton.GetRight() - 2, rcButton.Y + 2);
		trgPoints [1] = Point (rcButton.X + 2, (m_rcDockPanel.bottom - m_rcDockPanel.top) / 2);
		trgPoints [2] = Point (rcButton.GetRight() - 2, rcButton.GetBottom() - 2);
		break;
	case IGDOCKPANELSIDE_TOP:
		trgPoints [0] = Point (rcButton.X + 2, rcButton.Y + 2);
		trgPoints [1] = Point ((m_rcDockPanel.right - m_rcDockPanel.left) / 2, rcButton.GetBottom() - 2);
		trgPoints [2] = Point (rcButton.GetRight() - 2, rcButton.Y + 2);
		break;
	case IGDOCKPANELSIDE_BOTTOM:
		trgPoints [0] = Point (rcButton.X + 2, rcButton.GetBottom() - 2);
		trgPoints [1] = Point ((m_rcDockPanel.right - m_rcDockPanel.left) / 2, rcButton.Y + 2);
		trgPoints [2] = Point (rcButton.GetRight() - 2, rcButton.GetBottom() - 2);
		break;
	}	
	SolidBrush solBrushTrg (IGDOCKPANEL_COLOR_TRIANGLE);
	graphics.SetSmoothingMode (SmoothingModeAntiAlias);
	graphics.FillPolygon (&solBrushTrg, &trgPoints[0], 3);

	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	PointF pointF (0.0f, 0.0f);
	SolidBrush  solBrushFont (IGDOCKPANEL_COLOR_FONT);
	StringFormat format (StringFormat::GenericDefault());
	format.SetAlignment (StringAlignmentCenter);
	graphics.DrawString (m_wsDockName.c_str(), -1, &font, rcDockTitle, &format, &solBrushFont);
}

void CIGDockPanel::drawPopup (HDC hdc)
{
	RECT rcPopup;
	::GetWindowRect (m_hPopupWnd, &rcPopup);
	rcPopup.right -= rcPopup.left;
	rcPopup.bottom -= rcPopup.top;
	rcPopup.left = rcPopup.top = 0;
	RectF rcDockTitle;	
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
		rcDockTitle = RectF ((float)(rcPopup.right - IGDOCKPANEL_DOCKEDSIZE), 0.0f, (float)IGDOCKPANEL_DOCKEDSIZE,
							(float)(rcPopup.bottom - rcPopup.top));
		break;
	case IGDOCKPANELSIDE_RIGHT:						
		rcDockTitle = RectF (0.0f, 0.0f, (float)IGDOCKPANEL_DOCKEDSIZE,
							(float)(rcPopup.bottom - rcPopup.top));
		break;
	case IGDOCKPANELSIDE_TOP:
		rcDockTitle = RectF (0.0f, (float)(rcPopup.bottom - IGDOCKPANEL_DOCKEDSIZE), (float)(rcPopup.right - rcPopup.left),
							(float)IGDOCKPANEL_DOCKEDSIZE);
		break;
	case IGDOCKPANELSIDE_BOTTOM:			
		rcDockTitle = RectF (0.0f, 0.0f, (float)(rcPopup.right - rcPopup.left),
							(float)IGDOCKPANEL_DOCKEDSIZE);
		break;
	default:
		return;
	}
	Graphics graphics (hdc);
	SolidBrush solBrushBkgnd (IGDOCKPANEL_COLOR_BACKGROUND);
	int nOffset = m_nNbTicks * IGDOCKPANEL_TICKSIZE;
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
		graphics.FillRectangle (&solBrushBkgnd, Rect (0, 0,
													nOffset,
													rcPopup.bottom - rcPopup.top));
		break;
	case IGDOCKPANELSIDE_RIGHT:
		graphics.FillRectangle (&solBrushBkgnd, Rect (IGDOCKPANEL_DOCKEDSIZE, 0,
													rcPopup.right - rcPopup.left - IGDOCKPANEL_DOCKEDSIZE,
													rcPopup.bottom - rcPopup.top));
		break;
	case IGDOCKPANELSIDE_TOP:
		graphics.FillRectangle (&solBrushBkgnd, Rect (0, 0,
													rcPopup.right - rcPopup.left,
													nOffset));
		break;
	case IGDOCKPANELSIDE_BOTTOM:
		graphics.FillRectangle (&solBrushBkgnd, Rect (0, IGDOCKPANEL_DOCKEDSIZE,
													rcPopup.right - rcPopup.left,
													rcPopup.bottom - rcPopup.top - IGDOCKPANEL_DOCKEDSIZE));
		break;
	}

	eraseBkgnd (hdc, nOffset);

	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	PointF pointF (0.0f, 0.0f);
	SolidBrush  solBrushFont (IGDOCKPANEL_COLOR_FONT);
	StringFormat format (StringFormat::GenericDefault());
	format.SetAlignment (StringAlignmentCenter);
	graphics.DrawString (m_wsDockName.c_str(), -1, &font, rcDockTitle, &format, &solBrushFont);
}

void CIGDockPanel::eraseBkgnd (HDC hdc, int nOffset)
{	
	Pen penGreen (IGDOCKPANEL_COLOR_BORDER);
	Graphics graphics (hdc);	
	switch (m_eSide)
	{
	case IGDOCKPANELSIDE_LEFT:
	{
		LinearGradientBrush lingradBrushBorder (Point (nOffset - 1, 0),
												Point (nOffset + m_rcDockPanel.right - m_rcDockPanel.left, 0),
												IGDOCKPANEL_COLOR_BACKGROUND,
												IGDOCKPANEL_COLOR_BORDER);
		graphics.FillRectangle (&lingradBrushBorder, Rect (nOffset, 0,
													m_rcDockPanel.right - m_rcDockPanel.left,
													m_rcDockPanel.bottom - m_rcDockPanel.top));
		graphics.DrawLine (&penGreen, nOffset - 1, 0,
										nOffset - 1,
										m_rcDockPanel.bottom - m_rcDockPanel.top);
	}
	break;
	case IGDOCKPANELSIDE_RIGHT:
	{
		LinearGradientBrush lingradBrushBorder (Point (0, 0),
												Point (m_rcDockPanel.right - m_rcDockPanel.left, 0),
												IGDOCKPANEL_COLOR_BORDER,
												IGDOCKPANEL_COLOR_BACKGROUND);
		graphics.FillRectangle (&lingradBrushBorder, Rect (0, 0,
													m_rcDockPanel.right - m_rcDockPanel.left,
													m_rcDockPanel.bottom - m_rcDockPanel.top));
		graphics.DrawLine (&penGreen, m_rcDockPanel.right - m_rcDockPanel.left + 1, 0,
										m_rcDockPanel.right - m_rcDockPanel.left + 1,
										m_rcDockPanel.bottom - m_rcDockPanel.top);
	}
	break;
	case IGDOCKPANELSIDE_TOP:
	{
		LinearGradientBrush lingradBrushBorder (Point (0, nOffset),
												Point (0, nOffset + m_rcDockPanel.bottom - m_rcDockPanel.top),
												IGDOCKPANEL_COLOR_BACKGROUND,
												IGDOCKPANEL_COLOR_BORDER);
		graphics.FillRectangle (&lingradBrushBorder, Rect (0, nOffset,
													m_rcDockPanel.right - m_rcDockPanel.left,
													m_rcDockPanel.bottom - m_rcDockPanel.top));
		graphics.DrawLine (&penGreen, 0, nOffset - 1,
										m_rcDockPanel.right - m_rcDockPanel.left,
										nOffset - 1);
	}
	break;
	case IGDOCKPANELSIDE_BOTTOM:
	{
		LinearGradientBrush lingradBrushBorder (Point (0, 0),
												Point (0, m_rcDockPanel.bottom - m_rcDockPanel.top),
												IGDOCKPANEL_COLOR_BORDER,
												IGDOCKPANEL_COLOR_BACKGROUND);	
		graphics.FillRectangle (&lingradBrushBorder, Rect (0, 0,
													m_rcDockPanel.right - m_rcDockPanel.left,
													m_rcDockPanel.bottom - m_rcDockPanel.top));
		graphics.DrawLine (&penGreen, 0, m_rcDockPanel.bottom - m_rcDockPanel.top + 1,
										m_rcDockPanel.right - m_rcDockPanel.left,
										m_rcDockPanel.bottom - m_rcDockPanel.top + 1);
	}
	break;
	}
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGDockPanel::StaticHookPopupPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGDockPanel* pThis = (CIGDockPanel*)dwRefData;
	if( pThis != NULL )
		return pThis->HookPopupPanelProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGDockPanel::HookPopupPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_ERASEBKGND:
		return 1L;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hDC = ::BeginPaint(hWnd, &ps);
		drawPopup (hDC);
		::EndPaint(hWnd, &ps);
		return 0L;
	}

	case WM_MOUSELEAVE:
		if (wParam == IGDOCKPANEL_TRANSFERMOUSETRACKEVENT)
		{	
			m_bMouseInside = false;
			m_bDoNotLeave = false;
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = m_hPopupWnd;
			trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
			::TrackMouseEvent (&trackMouseEvent);	
		}
		else if ((m_eMode == IGDOCKPANELMODE_POPUP) && !m_bDoNotLeave)
		{
			if (m_hInside == hWnd)
			{
				closePopup ();
			}
			m_hInside = NULL;
		}		
		break;

	case WM_MOUSEMOVE:		
		if (wParam == IGDOCKPANEL_TRANSFERMOUSETRACKEVENT)
			m_bDoNotLeave = true;
		else if ((m_hInside != hWnd) && (m_eMode == IGDOCKPANELMODE_POPUP))
		{
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = m_hPopupWnd;
			trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
			::TrackMouseEvent (&trackMouseEvent);			
		}
		m_hInside = hWnd;
		break;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_SETFOCUS:
		return 0L;

	case WM_TIMER:
		movePopup ();
		::InvalidateRect (m_hPopupWnd, NULL, FALSE);
		::UpdateWindow (m_hPopupWnd);
		if (m_nNbTicks == m_nNbMaxTicks || m_nNbTicks == 0)
		{
			::KillTimer (m_hPopupWnd, 1);
			if (m_nNbTicks == 0)
			{
				m_hInside = m_hWnd;
				m_eMode = IGDOCKPANELMODE_CLOSED;
				::ShowWindow (m_hPopupWnd, SW_HIDE);
			}	
			else // m_nNbTicks == m_nNbMaxTicks
			{
				Fire_OnPanelOpened();
			}
		}
		break;
	}
	return ::DefSubclassProc(hWnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGDockPanel::StaticHookTransferMouseEventProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGDockPanel* pThis = (CIGDockPanel*)dwRefData;
	if( pThis != NULL )
		return pThis->HookTransferMouseEventProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGDockPanel::HookTransferMouseEventProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch(msg)
	{
	case WM_MOUSEMOVE:
		{	
			m_hInside = hWnd;
			if (!m_bMouseInside)
			{
				m_bMouseInside = true;
				::SendMessageW (m_hPopupWnd, msg, IGDOCKPANEL_TRANSFERMOUSETRACKEVENT, lParam);
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = hWnd;
				trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
				::TrackMouseEvent (&trackMouseEvent);
			}
		}
		break;

	case WM_MOUSELEAVE:
		{
			if (m_bMouseInside)
			{
				m_bMouseInside = false;
				for (std::list<HWND>::const_iterator iterControls = m_lControls.begin(); 
					iterControls != m_lControls.end();
					++iterControls)
				{
					if ((*iterControls == m_hInside) && (m_hInside != hWnd))
					{
						m_bMouseInside = true;
						TRACKMOUSEEVENT trackMouseEvent;
						trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
						trackMouseEvent.dwFlags = TME_LEAVE;
						trackMouseEvent.hwndTrack = m_hInside;
						trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
						::TrackMouseEvent (&trackMouseEvent);
						break;
					}
				}				
			}
			if (!m_bMouseInside)
			{
				if (m_hInside != m_hPopupWnd)
				{
					closePopup ();
				}
				else
				{
					::SendMessageW (m_hPopupWnd, msg, IGDOCKPANEL_TRANSFERMOUSETRACKEVENT, lParam);
				}
			}
		}
		break;
	}
	return ::DefSubclassProc(hWnd, msg, wParam, lParam);
}

BOOL CALLBACK CIGDockPanel::StaticEnumRegisterChildProc (HWND hwnd, LPARAM lParam)
{	
	CIGDockPanel* pThis = (CIGDockPanel*)lParam;
	if( pThis != NULL )
		return pThis->EnumChildProc(hwnd, (WPARAM)TRUE);
	return FALSE;
}

BOOL CALLBACK CIGDockPanel::StaticEnumUnregisterChildProc (HWND hwnd, LPARAM lParam)
{	
	CIGDockPanel* pThis = (CIGDockPanel*)lParam;
	if( pThis != NULL )
		return pThis->EnumChildProc(hwnd, (WPARAM)FALSE);
	return FALSE;
}

BOOL CALLBACK CIGDockPanel::EnumChildProc (HWND hwnd, LPARAM lParam)
{
	if ((BOOL)lParam)
		registerControls (hwnd);
	else
		unregisterControls (hwnd);
	return TRUE;
}

BOOL CALLBACK CIGDockPanel::StaticEnumMoveChildProc (HWND hwnd, LPARAM lParam)
{	
	CIGDockPanel* pThis = (CIGDockPanel*)lParam;
	if( pThis != NULL )
		return pThis->EnumMoveChildProc(hwnd);
	return FALSE;
}

BOOL CALLBACK CIGDockPanel::EnumMoveChildProc (HWND hwnd)
{
	::SendMessageW (hwnd, UM_CUSTOMMOVE, 0, 0);
	return TRUE;
}