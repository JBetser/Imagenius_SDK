// IGSimpleButton.cpp : Implementation of CIGSimpleButton
#include "stdafx.h"
#include "IGSimpleButton.h"
#include "dllmain.h"
#include <gdiplus.h>

#define IGBUTTON_OVERCOLOR_BORDER			Color(255, 206, 206, 195)	// Black
#define IGBUTTON_PUSHEDCOLOR_BORDER		Color(255, 150, 150, 150)	// Black
#define IGBUTTON_SELECTCOLOR_BORDER		Color(255, 52, 148, 214)	// Blue
#define IGBUTTON_SELECTCOLOR_MASK		Color(150, 52, 148, 214)	// Blue
#define IGBUTTON_SELECTCOLOR_OVER		Color(100, 52, 148, 214)	// White
#define IGBUTTON_SELECTCOLOR_FILL		Color(100, 52, 148, 214)	// Light Blue
#define IGBUTTON_BACKGROUNDCOLOR		Color(255, 221, 239, 251)	// White
#define IGBUTTON_LIGHTCOLOR_IN			Color(0, 52, 148, 214)	// White
#define IGBUTTON_LIGHTCOLOR_OUT			Color(230, 255, 255, 255)	// White
#define IGBUTTON_CORNERRAY				7
#define IGBUTTON_CORNERDIAM				( 2 * IGBUTTON_CORNERRAY )
#define IGBUTTON_CLASSNAME				L"IGSimpleButton"

using namespace Gdiplus;

// CIGSimpleButton
CIGSimpleButton::CIGSimpleButton () : m_hParent (NULL)
									, m_hWnd (NULL)
									, m_pCxIconNormal (NULL)
									, m_pCxIconPushed (NULL)
									, m_nLeft (0)
									, m_nTop (0)
									, m_nWidth (0)
									, m_nHeight (0)
									, m_eButtonState (IGBUTTON_MOUSEOUT)
									, m_bDoubleState (VARIANT_FALSE)
									, m_nIconLeft (0)
									, m_nIconTop (0)
									, m_nIconWidth (0)
									, m_nIconHeight (0)
									, m_bPushed (false)
									, m_nButtonId (0)
									, m_nCode (0)
									, m_cBackground (CLR_INVALID)
{
}

STDMETHODIMP CIGSimpleButton::CreateFrame (OLE_HANDLE hParent, SHORT nButtonId, LONG nCode, LPRECT p_rc, BSTR bstrResIconNormal, BSTR bstrResIconPushed, VARIANT_BOOL bDoubleState)
{
	if (!hParent || !nButtonId || !p_rc)
		return E_INVALIDARG;
	m_hParent = (HWND)hParent;
	m_nButtonId = nButtonId;
	m_nCode = nCode;
	HRESULT hr = createIcons (bstrResIconNormal, bstrResIconPushed);
	if (hr != S_OK)
		return E_FAIL;
	m_bDoubleState = bDoubleState;
	m_nLeft = p_rc->left;
	m_nTop = p_rc->top;	
	m_nWidth = p_rc->right - p_rc->left;
	m_nHeight = p_rc->bottom - p_rc->top;
	m_nIconLeft = (m_nWidth - m_nIconWidth) / 2;
	m_nIconTop = (m_nHeight - m_nIconHeight) / 2;

	WNDCLASSEX wndClass;	
    ZeroMemory (&wndClass, sizeof(wndClass));
    wndClass.cbSize        = sizeof(wndClass);
	wndClass.lpfnWndProc   = &CIGSimpleButton::StaticWndProc;
    wndClass.hInstance     = getInstance();
    wndClass.hCursor       = LoadCursor (0, IDC_ARROW);  
    wndClass.hbrBackground = NULL;
    wndClass.lpszClassName = IGBUTTON_CLASSNAME;
	::RegisterClassEx (&wndClass);
	m_hWnd = ::CreateWindowExW (WS_EX_LEFT, IGBUTTON_CLASSNAME, L"",
									WS_CHILD | WS_VISIBLE,
									m_nLeft, m_nTop,
									m_nWidth, m_nHeight,
									(HWND)hParent,	NULL, getInstance(), (LPVOID)this);
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::DestroyFrame ()
{
	if (m_hWnd)
		::DestroyWindow (m_hWnd);
	if (m_pCxIconNormal)
		delete m_pCxIconNormal;
	if (m_pCxIconPushed)
		delete m_pCxIconPushed;	
	::UnregisterClassW (IGBUTTON_CLASSNAME, getInstance());
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::Move (LPRECT p_rc)
{
	m_nLeft = p_rc->left;
	m_nTop = p_rc->top;
	m_nWidth = p_rc->right - p_rc->left;
	m_nHeight = p_rc->bottom - p_rc->top;
	::MoveWindow (m_hWnd, m_nLeft, m_nTop,
					m_nWidth, m_nHeight, TRUE);
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::Redraw (VARIANT_BOOL bForce)
{
	RECT rcButton;
	::GetWindowRect (m_hWnd, &rcButton);
	RECT rcWnd;
	::GetWindowRect (m_hParent, &rcWnd);
	rcButton.left -= rcWnd.left;
	rcButton.top -= rcWnd.top;
	rcButton.right -= rcWnd.left;
	rcButton.bottom -= rcWnd.top;

	if (m_cBackground != CLR_INVALID)
	{
		::InvalidateRect (m_hWnd, NULL, FALSE);
		return S_FALSE;
	}

	::InvalidateRect (m_hParent, &rcButton, TRUE);
	if (bForce)
	{
		::UpdateWindow (m_hParent);
		::InvalidateRect (m_hWnd, &rcButton, FALSE);
	}
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::put_Pushed (VARIANT_BOOL bPushed)
{
	m_bPushed = (bPushed == VARIANT_TRUE);
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::get_Pushed (VARIANT_BOOL *p_bPushed)
{
	if (!p_bPushed)
		return E_INVALIDARG;
	*p_bPushed = (m_bPushed ? VARIANT_TRUE : VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::SetBackground (LONG cBackground)
{
	m_cBackground = (COLORREF)cBackground;
	return S_OK;
}

STDMETHODIMP CIGSimpleButton::GetHWND (OLE_HANDLE *p_hWnd)
{
	if (!p_hWnd)
		return E_FAIL;
	*p_hWnd = (OLE_HANDLE)m_hWnd;
	return S_OK;
}

HRESULT CIGSimpleButton::createIcons (LPCWSTR pwIconNormal, LPCWSTR pwIconPushed)
{
	if (pwIconNormal)
	{
		HRSRC hResIconNormal = ::FindResourceW (getInstance(), pwIconNormal, L"PNG");
		if (!hResIconNormal)
			return E_FAIL;
		m_pCxIconNormal = new CxImage ();
		m_pCxIconNormal->LayerCreate (0);
		if (!m_pCxIconNormal->GetLayer(0)->LoadResource (hResIconNormal, CXIMAGE_FORMAT_PNG, getInstance()))
			return E_FAIL;
		m_nIconWidth = m_pCxIconNormal->GetLayer(0)->GetWidth();
		m_nIconHeight = m_pCxIconNormal->GetLayer(0)->GetHeight();
	}
	if (pwIconPushed)
	{
		HRSRC hResIconPushed = ::FindResourceW (getInstance(), pwIconPushed, L"PNG");
		if (!hResIconPushed)
			return E_FAIL;
		m_pCxIconPushed = new CxImage ();
		m_pCxIconPushed->LayerCreate (0);
		if (!m_pCxIconPushed->GetLayer(0)->LoadResource (hResIconPushed, CXIMAGE_FORMAT_PNG, getInstance()))
			return E_FAIL;
		if (!pwIconNormal)
		{
			m_nIconWidth = m_pCxIconPushed->GetLayer(0)->GetWidth();
			m_nIconHeight = m_pCxIconPushed->GetLayer(0)->GetHeight();
		}
	}
	return S_OK;
}

void CIGSimpleButton::drawToolBoxButton (HDC hdc)
{
	Graphics graphics (hdc);
	graphics.SetInterpolationMode (InterpolationModeBicubic);
	Rect rcGdiButton = Rect (0, 0,
							m_nWidth,
							m_nHeight);

	// Border part
	int nWidth = m_nWidth - 2;
	int nHeight = m_nHeight - 2;
	GraphicsPath pathBorder;
	pathBorder.StartFigure();
	pathBorder.AddLine (Point (0, IGBUTTON_CORNERRAY), Point (0, nHeight - IGBUTTON_CORNERRAY));
	pathBorder.AddArc	(Rect (0, nHeight - IGBUTTON_CORNERDIAM,
						IGBUTTON_CORNERDIAM, IGBUTTON_CORNERDIAM), 180.0f, -90.0f);
	pathBorder.AddLine (Point (IGBUTTON_CORNERRAY, nHeight), Point (nWidth - IGBUTTON_CORNERRAY, nHeight));
	pathBorder.AddArc	(Rect (nWidth - IGBUTTON_CORNERDIAM, nHeight - IGBUTTON_CORNERDIAM,
						IGBUTTON_CORNERDIAM, IGBUTTON_CORNERDIAM), 90.0f, -90.0f);
	pathBorder.AddLine (Point (nWidth, nHeight - IGBUTTON_CORNERRAY), Point (nWidth, IGBUTTON_CORNERRAY));
	pathBorder.AddArc	(Rect (nWidth - IGBUTTON_CORNERDIAM, 0,
						IGBUTTON_CORNERDIAM, IGBUTTON_CORNERDIAM), 0.0f, -90.0f);
	pathBorder.AddLine (Point (nWidth - IGBUTTON_CORNERRAY, 0), Point (IGBUTTON_CORNERRAY, 0));
	pathBorder.AddArc	(Rect (0, 0,
						IGBUTTON_CORNERDIAM, IGBUTTON_CORNERDIAM), -90.0f, -90.0f);
	pathBorder.CloseFigure();
	
	if (m_bPushed)
	{
		Matrix mat;
		mat.Translate (1.0, 1.0);
		pathBorder.Transform (&mat);		
	}
	else
	{
	}

	if (m_eButtonState == IGBUTTON_MOUSEOVER)
	{
		LinearGradientBrush lingradBrush (Point (nWidth + 1, nHeight + 1),
												Point (-1, -1),
												IGBUTTON_LIGHTCOLOR_IN,
												IGBUTTON_LIGHTCOLOR_OUT);
		graphics.FillPath (&lingradBrush, &pathBorder);
	}

	if (m_bPushed || m_eButtonState == IGBUTTON_MOUSEOVER)
	{
		Pen borderPen (m_bPushed ? IGBUTTON_PUSHEDCOLOR_BORDER : IGBUTTON_OVERCOLOR_BORDER );	
		graphics.DrawPath (&borderPen, &pathBorder);
	}

	if (m_bPushed)
	{
		SolidBrush maskBrush (IGBUTTON_SELECTCOLOR_MASK);
		graphics.FillPath (&maskBrush, &pathBorder);

		m_pCxIconNormal->LayerDrawAll (hdc, m_nIconLeft + 1, m_nIconTop + 1);		
	}
	else
	{		
		m_pCxIconNormal->LayerDrawAll (hdc, m_nIconLeft, m_nIconTop);
	}
}

void CIGSimpleButton::drawButton (HDC hdc)
{
	Graphics graphics (hdc);
	Rect rcGdiButton = Rect (0, 0,
							m_nWidth,
							m_nHeight);
	// draw blue square in mouseover mode
	if (m_cBackground != CLR_INVALID)
	{
		SolidBrush solBrushBackground (Color (GetRValue (m_cBackground),
												GetGValue (m_cBackground),
												GetBValue (m_cBackground)));
		graphics.FillRectangle (&solBrushBackground, rcGdiButton);
	}
	rcGdiButton.Width--;
	rcGdiButton.Height--;

	if (m_bDoubleState)
	{
		// test if mouse is over the current button
		if (m_eButtonState == IGBUTTON_MOUSEOVER)
		{
			// draw blue square in mouseover mode
			SolidBrush solBrushButton (IGBUTTON_SELECTCOLOR_FILL);
			graphics.FillRectangle (&solBrushButton, rcGdiButton);
			Pen penButtonBorder (IGBUTTON_SELECTCOLOR_BORDER);
			graphics.DrawRectangle (&penButtonBorder, rcGdiButton);		
		}
		if (m_bPushed)
		{
			if (m_pCxIconPushed)
				m_pCxIconPushed->LayerDrawAll (hdc, m_nIconLeft, m_nIconTop);
			return;
		}
	}
	if (m_pCxIconNormal)
	{
		if (m_eButtonState == IGBUTTON_MOUSEOVER)
		{
			if (m_bDoubleState)
			{
				SolidBrush solBrushButton (IGBUTTON_SELECTCOLOR_FILL);
				graphics.FillRectangle (&solBrushButton, rcGdiButton);
				Pen penButtonBorder (IGBUTTON_SELECTCOLOR_BORDER);
				graphics.DrawRectangle (&penButtonBorder, rcGdiButton);	
				m_pCxIconNormal->LayerDrawAll (hdc, m_nIconLeft, m_nIconTop);
			}
			else
			{
				// Draw a shadow
				CxImage cxIconGray;
				cxIconGray.Copy (*m_pCxIconNormal->GetLayer(0));
				cxIconGray.GrayScale();
				cxIconGray.Draw (hdc, m_nIconLeft + 1, m_nIconTop + 1);

				SolidBrush solBrushButton (IGBUTTON_SELECTCOLOR_FILL);
				graphics.FillRectangle (&solBrushButton, rcGdiButton);
				Pen penButtonBorder (IGBUTTON_SELECTCOLOR_BORDER);
				graphics.DrawRectangle (&penButtonBorder, rcGdiButton);	
				m_pCxIconNormal->LayerDrawAll (hdc, m_nIconLeft - 1, m_nIconTop - 1);
			}
			
		}
		else if (m_eButtonState == IGBUTTON_MOUSEDOWN)
		{
			m_pCxIconNormal->LayerDrawAll (hdc, m_nIconLeft + 1, m_nIconTop + 1);
		}
		else
		{
			m_pCxIconNormal->LayerDrawAll (hdc, m_nIconLeft, m_nIconTop);
		}
	}
}

LRESULT CIGSimpleButton::WndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			if (m_bDoubleState && !m_pCxIconPushed)
			{
				// toolbox style
				drawToolBoxButton (hDC);
			}
			else
				drawButton (hDC);
			::EndPaint(hWnd, &ps);	
			return 0L;
		}

	case WM_ERASEBKGND:
		return 1L;

	case WM_MOUSEMOVE:
		if (m_eButtonState != IGBUTTON_MOUSEOVER)
		{
			m_eButtonState = IGBUTTON_MOUSEOVER;
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = hWnd;
			trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
			::TrackMouseEvent (&trackMouseEvent);
			Redraw (VARIANT_FALSE);
		}
		break;

	case WM_MOUSELEAVE:
		m_eButtonState = IGBUTTON_MOUSEOUT;
		Redraw (VARIANT_FALSE);
		break;

	case WM_LBUTTONUP:
		if (m_bDoubleState && !m_pCxIconPushed)
		{
			// toolbox style			
			if (!m_bPushed)
			{
				m_bPushed = !m_bPushed;
				::SendMessageW (m_hParent, WM_COMMAND, m_nButtonId, m_nCode);
			}
		}
		else
			::SendMessageW (m_hParent, WM_COMMAND, m_nButtonId, m_nCode);
		Redraw (VARIANT_FALSE);
		return 0L;

	case WM_LBUTTONDOWN:
		m_eButtonState = IGBUTTON_MOUSEDOWN;
		Redraw (VARIANT_FALSE);
		return 0L;
	}
	return 0L;
}

LRESULT CALLBACK CIGSimpleButton::StaticWndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_CREATE:
		{
			CREATESTRUCT *pcs = (CREATESTRUCT *)lParam;
			if (pcs)
				::SetWindowLongPtrW (hWnd, GWLP_USERDATA, (LONG)pcs->lpCreateParams);
		}
		break;
	default:
		{
			CIGSimpleButton *pThis = (CIGSimpleButton *)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);
			if (pThis)
				pThis->WndProc (hWnd, nMsg, wParam, lParam);
		}
		break;
	}
	return ::DefWindowProcW (hWnd, nMsg, wParam, lParam);
}