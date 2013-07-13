#include "StdAfx.h"
#include "IGTabBar.h"
#include <WindowsX.h>
#include <shlobj.h>
#include <gdiplus.h>

#define IGTABBAR_BUTTONWIDTH		40
#define TABBAR_HEIGHT				25
#define BUTTON_HEIGHT				20
#define TABBAR_SEPARATOR_HEIGHT		(TABBAR_HEIGHT - BUTTON_HEIGHT)
#define IGTABBAR_BUTTONSPACE		5
#define IGTABBAR_FIRSTTABX			5
#define IGTAB_CORNERDIAM			12
#define IGTAB_OFFSETBOTTOM			15
#define IGTAB_COLORBACKGND			Color(255, 169, 207, 185)	// Green
#define IGTAB_COLORBORDER			Color(255, 76, 85, 118)		// Black
#define IGTAB_COLOR_FRAMEIN			Color(150, 255, 255, 255)	// White
#define IGTAB_COLOR_FRAMEOUT		Color(0, 169, 207, 185)		// Green
#define IGTAB_COLOR_SELECTED		Color(255, 52, 148, 214)	// Blue
#define IGTAB_COLOR_UNSELECTED		Color(255, 195, 207, 200)	// Gray
#define IGTAB_COLOR_FONTENABLED		Color(255, 34, 98, 141)
#define IGTAB_COLOR_FONTDISABLED	Color(255, 52, 148, 214)

extern HINSTANCE	g_hInstance;

using namespace IGLibrary;
using namespace Gdiplus;

#define IGTAB_CLASSNAME			L"IGTab"
#define IGTABBAR_CLASSNAME		L"IGTabBar"

IGTabBar::IGTabBar (HWND hParent, int nPosX, int nPosY, int nWidth, int nHeight, COLORREF cBackGround)	: m_hWnd (NULL)
																										, m_hSelectedTab (NULL)
																										, m_hOver (NULL)
																										, m_nNbTabs (0)
																										, m_cBackGround (cBackGround)
																										, m_eButtonState (IGBUTTON_MOUSEOUT)
																										, m_nIdxSelectedTab (0)
{
	ZeroMemory (&m_rcWnd, sizeof (RECT));
	ZeroMemory (m_phTabs, MAXTABS * sizeof (HWND));
	ZeroMemory (m_phTabBars, MAXTABS * sizeof (HWND));
	ZeroMemory (m_phTabButtons, MAXTABS * sizeof (HWND));
	ZeroMemory (m_pnDesiredSizes, MAXTABS * sizeof (UINT));
	WNDCLASSEX wndClass;	
    ZeroMemory (&wndClass, sizeof(wndClass));
    wndClass.cbSize        = sizeof(wndClass);
	wndClass.lpfnWndProc   = &IGTabBar::StaticWndTabProc;
    wndClass.hInstance     = g_hInstance;
    wndClass.hCursor       = LoadCursor (0, IDC_ARROW);  
    wndClass.hbrBackground = (HBRUSH)::CreateSolidBrush (m_cBackGround);
    wndClass.lpszClassName = IGTAB_CLASSNAME;
	if (::RegisterClassEx (&wndClass))
	{	
		static const WCHAR winName[]   = L"Tab Container";
		ZeroMemory (&wndClass, sizeof(wndClass));
		wndClass.cbSize        = sizeof(wndClass);
		wndClass.lpfnWndProc   = &IGTabBar::StaticWndProc;
		wndClass.hInstance     = g_hInstance;
		wndClass.hCursor       = LoadCursor (0, IDC_ARROW);  
		wndClass.hbrBackground = (HBRUSH)::CreateSolidBrush (m_cBackGround);
		wndClass.lpszClassName = IGTABBAR_CLASSNAME;
		if (::RegisterClassEx (&wndClass))
		{
		/* Setup windows */
			m_hWnd = ::CreateWindow (IGTABBAR_CLASSNAME, winName, WS_CHILD | WS_CLIPCHILDREN,
									 nPosX, nPosY, nWidth, nHeight - IGTAB_OFFSETBOTTOM,
										hParent, NULL, g_hInstance, (LPVOID)this);
			m_rcWnd.left = nPosX;
			m_rcWnd.right = nPosX + nWidth;
			m_rcWnd.top = nPosY;
			m_rcWnd.bottom = nPosY + nHeight - IGTAB_OFFSETBOTTOM;
		}
	}
}

IGTabBar::~IGTabBar()
{
	for (int idxTab = 0; idxTab < m_nNbTabs; idxTab++)
	{
		::DestroyWindow (m_phTabs [idxTab]);
		::DestroyWindow (m_phTabButtons [idxTab]);
	}
	::DestroyWindow (m_hWnd);
	::UnregisterClassW (IGTAB_CLASSNAME, g_hInstance);
	::UnregisterClassW (IGTABBAR_CLASSNAME, g_hInstance);
}

void IGTabBar::SetBkColor (COLORREF cBackGround)
{
	m_cBackGround = cBackGround;
}

bool IGTabBar::AddTab (HWND hTab, LPCWSTR lpcwTabTitle, UINT nDesiredSize)
{
	if (!m_hWnd || m_nNbTabs >= MAXTABS)
		return false;
	m_phTabBars [m_nNbTabs] = ::CreateWindowExW (WS_EX_LEFT, L"IGTabBar", lpcwTabTitle,
													WS_CHILD | WS_VISIBLE,
													m_rcWnd.right + IGTABBAR_BUTTONSPACE, BUTTON_HEIGHT + 1,
													m_rcWnd.right - m_rcWnd.left - IGTABBAR_BUTTONSPACE, m_rcWnd.bottom - m_rcWnd.top - BUTTON_HEIGHT - 1,
													m_hWnd, NULL, g_hInstance, (LPVOID)this);
	if (hTab)
	{
		m_phTabs [m_nNbTabs] = hTab;
		::SetParent (hTab, m_phTabBars [m_nNbTabs]);
	}
	int nButtonPosX = m_nNbTabs * IGTABBAR_BUTTONWIDTH;
	RECT rcLastTab;
	ZeroMemory (&rcLastTab, sizeof (RECT));
	if (m_nNbTabs > 0)
	{
		::GetClientRect (m_phTabButtons [m_nNbTabs - 1], &rcLastTab);
		::MapWindowRect (m_phTabButtons [m_nNbTabs - 1], m_hWnd, &rcLastTab);
	}
	m_phTabButtons [m_nNbTabs] = ::CreateWindowExW (WS_EX_LEFT, IGTAB_CLASSNAME, lpcwTabTitle,
													WS_CHILD | WS_VISIBLE,
													rcLastTab.right + IGTABBAR_BUTTONSPACE, 0,
													((nDesiredSize > 0) && (nDesiredSize < IGTABBAR_BUTTONWIDTH)) ? nDesiredSize : IGTABBAR_BUTTONWIDTH, BUTTON_HEIGHT + 1,
													(HWND)m_hWnd, NULL, g_hInstance, (LPVOID)this);
	if (m_nNbTabs == 0)
	{
		m_hSelectedTab = m_phTabBars [m_nNbTabs];
	}
	else
	{
		::ShowWindow (m_phTabBars [m_nNbTabs], SW_HIDE);
	}
	::SetWindowPos (hTab, HWND_TOP, IGTABBAR_BUTTONSPACE, BUTTON_HEIGHT + IGTABBAR_BUTTONSPACE, m_rcWnd.right - m_rcWnd.left - 2 * IGTABBAR_BUTTONSPACE, m_rcWnd.bottom - m_rcWnd.top - BUTTON_HEIGHT - 2 * IGTABBAR_BUTTONSPACE, SWP_FRAMECHANGED);
	m_pnDesiredSizes [m_nNbTabs] = (nDesiredSize > 0) ? nDesiredSize : IGTABBAR_BUTTONWIDTH;
	m_nNbTabs++;
	return true;
}

void IGTabBar::Show (bool bShow)
{
	if (!m_hWnd)
		return;
	::ShowWindow (m_hWnd, bShow ? SW_SHOW : SW_HIDE);
	selectTabButton (m_phTabButtons [m_nIdxSelectedTab]);			
}

void IGTabBar::OnPaint()
{
	if (!m_hWnd)
		return;
	PAINTSTRUCT ps;
	HDC hdc = ::BeginPaint (m_hWnd, &ps);	

	// draw background
	HBRUSH hBrush = ::CreateSolidBrush (RGB (IGTAB_COLORBACKGND.GetR(), IGTAB_COLORBACKGND.GetG(), IGTAB_COLORBACKGND.GetB()));
	HPEN hPen = ::CreatePen (PS_SOLID, 1, RGB (IGTAB_COLORBORDER.GetR(), IGTAB_COLORBORDER.GetG(), IGTAB_COLORBORDER.GetB()));
	HPEN hOldPen = SelectPen (hdc, hPen);
	HBRUSH hOldBrush = SelectBrush (hdc, hBrush);
	::Rectangle (hdc, 0, BUTTON_HEIGHT, m_rcWnd.right - m_rcWnd.left, m_rcWnd.bottom - m_rcWnd.top);
	SelectBrush (hdc, hOldBrush);
	SelectPen (hdc, hOldPen);
	DeleteBrush (hBrush);
	DeletePen (hPen);

	::EndPaint (m_hWnd, &ps);
}

void IGTabBar::Move (int nHeight)
{
	nHeight -= IGTAB_OFFSETBOTTOM;
	::SetWindowPos (m_hWnd, HWND_TOP, m_rcWnd.left, m_rcWnd.top, m_rcWnd.right - m_rcWnd.left, nHeight, SWP_FRAMECHANGED);
	m_rcWnd.bottom = m_rcWnd.top + nHeight;
	for (int idxTab = 0; idxTab < m_nNbTabs; idxTab++)
	{
		::SetWindowPos (m_phTabBars [idxTab], HWND_TOP, IGTABBAR_BUTTONSPACE, BUTTON_HEIGHT + IGTABBAR_BUTTONSPACE, m_rcWnd.right - m_rcWnd.left - 2 * IGTABBAR_BUTTONSPACE, m_rcWnd.bottom - m_rcWnd.top - BUTTON_HEIGHT - 2 * IGTABBAR_BUTTONSPACE, SWP_FRAMECHANGED);
		::SetWindowPos (m_phTabs [idxTab], HWND_TOP, 0, 0, m_rcWnd.right - m_rcWnd.left - 2 * IGTABBAR_BUTTONSPACE, m_rcWnd.bottom - m_rcWnd.top - BUTTON_HEIGHT - 2 * IGTABBAR_BUTTONSPACE, SWP_FRAMECHANGED);
	}
	resizeTabs();
}

LRESULT IGTabBar::WndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT:
		OnPaint();
		return 0L;

	case WM_ERASEBKGND:
		return 1L;
	}
	return 0L;
}

void IGTabBar::drawTab (HDC hdc, UINT nSize, bool bSelected, bool bOver, const wchar_t *pcwTitle)
{
	Graphics graphics (hdc);
	Color colBackground (Color (GetRValue (m_cBackGround), GetGValue (m_cBackGround), GetBValue (m_cBackGround)));
	SolidBrush solBrushBackground (colBackground);
	graphics.FillRectangle (&solBrushBackground, Rect (0, 0, nSize, BUTTON_HEIGHT - 1));

	SolidBrush solBrushTab (bSelected ? IGTAB_COLORBACKGND : IGTAB_COLOR_UNSELECTED);
	GraphicsPath pathBorder;
	pathBorder.StartFigure();
	pathBorder.AddArc (Rect (0, 0, IGTAB_CORNERDIAM, IGTAB_CORNERDIAM), 180.0f, 90.0f);
	pathBorder.AddArc (Rect (nSize - IGTAB_CORNERDIAM, 0, IGTAB_CORNERDIAM, IGTAB_CORNERDIAM), -90.0f, 90.0f);
	pathBorder.AddLine (Point (nSize, BUTTON_HEIGHT + (bSelected ? 1 : 0)), Point (0, BUTTON_HEIGHT + (bSelected ? 1 : 0)));
	graphics.FillPath (&solBrushTab, &pathBorder);

	if (bOver)
	{
		PathGradientBrush pthGrBrush (&pathBorder);
		pthGrBrush.SetCenterPoint (PointF (0.7f * (float)nSize,
										  0.3f * (float)BUTTON_HEIGHT));
		pthGrBrush.SetCenterColor (IGTAB_COLOR_FRAMEIN);
		Color colors[] = {IGTAB_COLOR_FRAMEOUT};
		int count = 1;
		pthGrBrush.SetSurroundColors (colors, &count);
		graphics.FillPath (&pthGrBrush, &pathBorder);
	}

	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 16, FontStyleRegular, UnitPixel);
	PointF      pointF(20.0f, 5.0f);
	SolidBrush  solidFontBrush (bSelected ? IGTAB_COLOR_FONTENABLED : IGTAB_COLOR_FONTDISABLED);
	StringFormat	format(StringFormat::GenericDefault());
	format.SetAlignment (StringAlignmentCenter);
	graphics.DrawString (pcwTitle, -1, &font, RectF (0.0f, 0.0f, (float)nSize, (float)BUTTON_HEIGHT),
							&format, &solidFontBrush);

	Pen penBorder (IGTAB_COLORBORDER, 1);
	GraphicsPath pathBorderOut;
	pathBorderOut.StartFigure();
	pathBorderOut.AddArc (Rect (0, 0, IGTAB_CORNERDIAM + 1, IGTAB_CORNERDIAM + 1), 180.0f, 90.0f);
	pathBorderOut.AddArc (Rect (nSize - IGTAB_CORNERDIAM - 2, 0, IGTAB_CORNERDIAM + 1, IGTAB_CORNERDIAM + 1), -90.0f, 90.0f);
	pathBorderOut.AddLine (Point (nSize - 1, BUTTON_HEIGHT + (bSelected ? 1 : 0)), Point (0, BUTTON_HEIGHT + (bSelected ? 1 : 0)));
	pathBorderOut.CloseFigure();
	graphics.DrawPath (&penBorder, &pathBorderOut);
}

void IGTabBar::redraw (bool bRedrawParent)
{
	if (bRedrawParent)
	{
		::InvalidateRect (m_hWnd, NULL, FALSE);
		::UpdateWindow (m_hWnd);
	}
	for (int idxTab = 0; idxTab < m_nNbTabs; idxTab++)
		::InvalidateRect (m_phTabButtons [idxTab], NULL, FALSE);
}

void IGTabBar::resizeTabs()
{
	bool bResize = false;
	int nSelectedTabSize = 0;
	int nTabSize = 0;
	for (int idxTab = 0; idxTab < m_nNbTabs; idxTab++)
	{
		if ((idxTab == m_nIdxSelectedTab) &&
			(m_pnDesiredSizes [idxTab] > 0))
		{
			bResize = true;
			nSelectedTabSize = m_pnDesiredSizes [idxTab];
			nTabSize = ((m_nNbTabs <= 1) ? 0 : 
						(int)(((float)(m_rcWnd.right - m_rcWnd.left) - (float)m_nNbTabs * IGTABBAR_BUTTONSPACE - (float)nSelectedTabSize) / (float)(m_nNbTabs - 1)));
			break;
		}
	}
	if (bResize)
	{
		int nNextTabX = IGTABBAR_FIRSTTABX;
		for (int idxTabResize = 0; idxTabResize < m_nNbTabs; idxTabResize++)
		{
			if (idxTabResize == m_nIdxSelectedTab)
			{
				::SetWindowPos (m_phTabButtons [idxTabResize], HWND_TOP, nNextTabX, 0, nSelectedTabSize, BUTTON_HEIGHT + 1, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
				nNextTabX += nSelectedTabSize;
			}
			else
			{
				::SetWindowPos (m_phTabButtons [idxTabResize], HWND_TOP, nNextTabX, 0, nTabSize, BUTTON_HEIGHT + 1, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
				nNextTabX += nTabSize;
			}
			nNextTabX += IGTABBAR_BUTTONSPACE;
		}
	}
}

void IGTabBar::selectTabButton (HWND hWnd)
{
	int idxSelected = 0;
	HWND hOldSelectedTabButton = m_phTabButtons [m_nIdxSelectedTab];	

	for (int idxTab = 0; idxTab < m_nNbTabs; idxTab++)
	{
		if (m_phTabButtons [idxTab] == hWnd)
		{
			m_hSelectedTab = m_phTabBars [idxTab];
			m_nIdxSelectedTab = idxTab;			
		}
		else
		{
			::ShowWindow (m_phTabBars [idxTab], SW_HIDE);
		}
	}

	::ShowWindow (m_hSelectedTab, SW_SHOW);

	if (hOldSelectedTabButton != hWnd)
		resizeTabs();
	redraw (true);
}

LRESULT CALLBACK IGTabBar::StaticWndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
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
			IGTabBar *pThis = (IGTabBar *)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);
			if (pThis)
				pThis->WndProc (hWnd, nMsg, wParam, lParam);
		}
		break;
	}
	return ::DefWindowProcW (hWnd, nMsg, wParam, lParam);
}

LRESULT IGTabBar::WndTabProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT:
		{
			wchar_t pwTitle [MAX_PATH];
			::GetWindowTextW (hWnd, pwTitle, MAX_PATH);	
			RECT rcWnd;
			::GetWindowRect (hWnd, &rcWnd);	
			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint (hWnd, &ps);
			drawTab (hdc, 
					rcWnd.right - rcWnd.left, 
					m_phTabButtons [m_nIdxSelectedTab] == hWnd, 
					(m_eButtonState == IGBUTTON_MOUSEOVER) ? (m_hOver == hWnd) : false,
					pwTitle);
			::EndPaint (hWnd, &ps);
		}
		break;
	case WM_ERASEBKGND:
		return 1L;
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		return 0L;

	case WM_MOUSEMOVE:
		if (m_eButtonState != IGBUTTON_MOUSEOVER)
		{
			m_eButtonState = IGBUTTON_MOUSEOVER;
			m_hOver = hWnd;
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = hWnd;
			trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
			::TrackMouseEvent (&trackMouseEvent);
			redraw();
		}
		break;

	case WM_MOUSELEAVE:
		m_eButtonState = IGBUTTON_MOUSEOUT;
		redraw();
		break;

	case WM_LBUTTONUP:
		selectTabButton (hWnd);
		break;

	case WM_KILLFOCUS:		
		redraw();		
		break;
	}

	// Default behavior using the old window proc	
	return ::DefWindowProcW (hWnd, nMsg, wParam, lParam);
}

LRESULT APIENTRY IGTabBar::StaticWndTabProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
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
		IGTabBar *pThis = (IGTabBar *)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);
		if (pThis)
			return pThis->WndTabProc (hWnd, nMsg, wParam, lParam);
	}
	break;
	}
	return ::DefWindowProcW (hWnd, nMsg, wParam, lParam);
}