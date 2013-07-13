#include "stdafx.h"
#include "resource.h"
#include "IGScrollBar.h"
#include "IGColor.h"
#include "IGTrace.h"
#include "IGMemDC.h"
#include <commctrl.h>
#include <WindowsX.h>
#include <shlobj.h>
#include <gdiplus.h>

using namespace Gdiplus;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// timer defines
#define TIMER_MOUSE_OVER_BUTTON		1	// mouse is over an arrow button, and
										// left button is down
#define TIMER_LBUTTON_PRESSED		2	// mouse is over an arrow button, and
										// left button has just been pressed
#define TIMER_MOUSE_OVER_THUMB		3	// mouse is over thumb
#define TIMER_MOUSE_HOVER_BUTTON	4	// mouse is over an arrow button
#define MOUSE_OVER_BUTTON_TIME		50

// color defines
#define THUMB_MASK_COLOR					RGB(0,0,1)
#define THUMB_GRIPPER_MASK_COLOR			RGB(0,0,2)
#define THUMB_LEFT_TRANSPARENT_MASK_COLOR	RGB(0,0,3)
#define THUMB_RIGHT_TRANSPARENT_MASK_COLOR	RGB(0,0,4)
#define THUMB_UP_TRANSPARENT_MASK_COLOR		THUMB_LEFT_TRANSPARENT_MASK_COLOR
#define THUMB_DOWN_TRANSPARENT_MASK_COLOR	THUMB_RIGHT_TRANSPARENT_MASK_COLOR
#define THUMB_GRIPPER_COLOR					RGB(91,91,91)	// dark gray
#define THUMB_HOVER_COLOR					RGB(52,148,214)	// blue
#define FRAME_COLOR							RGB(76,85,118)	// dark gray
#define THUMB_COLOR_ITEM					Color(255, 191, 193, 195)	// Medium Gray
#define THUMB_COLOR_ITEMBORDER				Color(255, 76,85,118)	// Medium Gray
#define THUMB_COLOR_SELECTEDITEM			Color(100, 52, 148, 214)	// Blue
#define THUMB_COLOR_SELECTEDITEMBORDER		Color(255, 52, 148, 214)	// Blue
#define FRAME_COLOR_BACKGROUND				Color(255, 195, 207, 200)	// Green
#define TICKHEIGHT_OFFSET					5
#define THUMBHEIGHT_MIN						10

using namespace IGLibrary;

LPCWSTR IGScrollBar::s_windowClassName = L"IGScrollBarClass";

static BOOL IsVista()
{
	BOOL rc = FALSE;
	OSVERSIONINFO osvi = { 0 };
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (::GetVersionEx (&osvi))
	{
		if ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
			(osvi.dwMajorVersion >= 6))
		{
			rc = TRUE;
		}
	}
	return rc;
}

IGScrollBar::IGScrollBar()
{
	m_hWnd				   = NULL;
	m_hParent              = NULL;
	m_bHorizontal          = TRUE;
	m_hCursor              = NULL;
	m_bMouseDown           = FALSE;
	m_bMouseOverArrowRight = FALSE;
	m_bMouseOverArrowLeft  = FALSE;
	m_bMouseOverArrowUp    = FALSE;
	m_bMouseOverArrowDown  = FALSE;
	m_bMouseDownArrowLeft  = FALSE;
	m_bMouseDownArrowRight = FALSE;
	m_bMouseDownArrowUp    = FALSE;
	m_bMouseDownArrowDown  = FALSE;
	m_bDragging            = FALSE;
	m_nPos                 = 0;
//	m_nLastScrollPos       = 0;
	m_nMinPos              = 0;
	m_nMaxPos              = 0;
	m_nRange               = 0;
	m_bChannelColor        = TRUE;
	m_bThumbColor          = FALSE;
	m_bThumbGripper        = TRUE;
	m_bThumbHover          = FALSE;
	m_nThumbHeight		   = 0;
	m_nIconBorderHeight	   = 0;
	ZeroMemory (&m_rectThumb, sizeof (RECT));
	ZeroMemory (&m_rectClient, sizeof (RECT));
	ZeroMemory (&m_rectLeftArrow, sizeof (RECT));
	ZeroMemory (&m_rectRightArrow, sizeof (RECT));
	ZeroMemory (&m_rectUpArrow, sizeof (RECT));
	ZeroMemory (&m_rectDownArrow, sizeof (RECT));
	m_nThumbLeft           = 25;
	m_nThumbTop            = 25;
	m_nBitmapWidth         = 25;
	m_nBitmapHeight        = 12;
	m_bmpThumb			   = NULL;
	m_bmpThumbHot		   = NULL;
	m_ThumbColor           = RGB(0,0,0);
	m_ThumbHoverColor      = THUMB_HOVER_COLOR;
	m_ArrowHotColor        = THUMB_HOVER_COLOR;
	if (IsVista())
		m_ArrowHotColor    = THUMB_HOVER_COLOR;
}


IGScrollBar::~IGScrollBar()
{
	if (m_hCursor)
		DestroyCursor(m_hCursor);

	if (m_bmpThumb)
		::DeleteObject (m_bmpThumb);

	if (m_bmpThumbHot)
		::DeleteObject (m_bmpThumbHot);

	if (m_bmpUpLeftArrow)
		::DeleteObject (m_bmpUpLeftArrow);

	if (m_bmpDownRightArrow)
		::DeleteObject (m_bmpThumb);

	::RemoveWindowSubclass (m_hParent, StaticHookScrolledWndProc, 0);
}

BOOL IGScrollBar::initialize (int nXPos, int nYPos, int nWidth, int nHeight)
{
    WNDCLASSEXW l_windowClassInformation = {0};
    l_windowClassInformation.cbSize = sizeof(WNDCLASSEXW);
    l_windowClassInformation.lpfnWndProc = &IGScrollBar::WndProc;
    l_windowClassInformation.hInstance = GetInstance();
    l_windowClassInformation.lpszClassName = s_windowClassName;
    l_windowClassInformation.hCursor = ::LoadCursor (NULL, IDC_ARROW);
    l_windowClassInformation.hbrBackground = reinterpret_cast<HBRUSH> (COLOR_WINDOW+1);
	l_windowClassInformation.style = CS_VREDRAW | CS_HREDRAW | CS_PARENTDC;

    if (RegisterClassExW (&l_windowClassInformation) == 0)
    {
		DWORD dw = GetLastError ();
		if (dw != ERROR_CLASS_ALREADY_EXISTS)
			return FALSE;        
    }

    LPVOID l_parameter = reinterpret_cast<LPVOID>(this);
	m_hWnd = ::CreateWindowExW (0L, s_windowClassName, L"IGScrollBar",
								WS_CHILD | WS_VISIBLE,
								nXPos, nYPos, nWidth, nHeight, m_hParent,
								NULL, GetInstance(), l_parameter);
    if (!m_hWnd)
    {
        UnregisterClassW (s_windowClassName, GetInstance());
        return FALSE;
    }
	::SetWindowSubclass (m_hParent, StaticHookScrolledWndProc, 0, (DWORD_PTR)this);
	return TRUE;
}


//
// CreateFromWindow
//
// Purpose:     Create the IGScrollBar control from placeholder window
//
// Parameters:  dwStyle    - the scroll bar’s style. Typically this will be
//                           SBS_HORZ | WS_CHILD | WS_VISIBLE | WS_TABSTOP.
//              hParentWnd - the scroll bar’s parent window, usually a CDialog
//                           object. It must not be NULL.
//              hWnd       - HWND of placeholder window (must already exist)
//              nId        - the resource id of the IGScrollBar control
//
// Returns:     BOOL       - TRUE = success
//
BOOL IGScrollBar::CreateFromWindow(DWORD dwStyle,
								   HWND hParentWnd,
								   HWND hWnd,
								   UINT nId)
{
	TRACE(L"in IGScrollBar::CreateFromWindow\n");

	_ASSERT(hParentWnd);
	_ASSERT(::IsWindow(hParentWnd));
	_ASSERT(::IsWindow(hWnd));

	RECT rect;
	::GetWindowRect (hWnd, &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.bottom = rect.top = 0;

	// hide placeholder window
	::ShowWindow (hWnd, SW_HIDE);

	return CreateFromRect (dwStyle, hParentWnd, rect, nId);
}


//
// CreateFromRect
//
// Purpose:     Create the IGScrollBar control from rect
//
// Parameters:  dwStyle    - the scroll bar’s style. Typically this will be
//                           SBS_HORZ | WS_CHILD | WS_VISIBLE | WS_TABSTOP.
//              hParentWnd - the scroll bar’s parent window, usually a CDialog
//                           object. It must not be NULL.
//              rect       - the size and position of the window, in client 
//                           coordinates of hParentWnd
//              nId        - the resource id of the IGScrollBar control
//
// Returns:     BOOL       - TRUE = success
//
BOOL IGScrollBar::CreateFromRect(DWORD dwStyle,
								 HWND hParentWnd,
								 RECT& rect,
								 UINT nId)
{
	TRACE(L"in IGScrollBar::CreateFromRect\n");

	_ASSERT(hParentWnd);
	_ASSERT(::IsWindow(hParentWnd));

	m_hParent = hParentWnd;

	// load hand cursor
	SetDefaultCursor();

	m_bHorizontal = (dwStyle & SBS_VERT) ? FALSE : TRUE;

	BOOL bResult = 	initialize (rect.left, rect.top,
								rect.right - rect.left,
								rect.bottom - rect.top);

	if (bResult)
	{
		// we assume that width of thumb is same as width of arrows
		LoadThumbBitmap();

		if (m_bmpThumb)
		{
			BITMAP bm;
			::GetObject (m_bmpThumb, sizeof (BITMAP), &bm);
			m_nBitmapWidth = bm.bmWidth;
			m_nBitmapHeight = bm.bmHeight;
			m_nThumbHeight = m_nBitmapHeight;

			Move (rect);
		}
		else
		{
			TRACE(L"ERROR - failed to load thumb bitmap\n");
			_ASSERT(FALSE);
		}

		// load arrow bitmaps
		m_bmpUpLeftArrow = (HBITMAP)::LoadImageW (GetInstance(), m_bHorizontal ? MAKEINTRESOURCEW (IDB_HORIZONTAL_SCROLLBAR_LEFTARROW)
												: MAKEINTRESOURCEW (IDB_VERTICAL_SCROLLBAR_UPARROW), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);

		m_bmpDownRightArrow = (HBITMAP)::LoadImageW (GetInstance(), m_bHorizontal ? MAKEINTRESOURCEW (IDB_HORIZONTAL_SCROLLBAR_RIGHTARROW)
													: MAKEINTRESOURCEW (IDB_VERTICAL_SCROLLBAR_DOWNARROW), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);

		BITMAP bm;
		::GetObject (m_bmpUpLeftArrow, sizeof (BITMAP), &bm);
		m_nIconBorderHeight = bm.bmHeight;
	}
	else
	{
		TRACE(L"ERROR - failed to create IGScrollBar\n");
		_ASSERT(FALSE);
	}

	return bResult;
}

void IGScrollBar::Move (RECT& rect)
{
	RECT rcScroll;
	::memcpy_s (&rcScroll, sizeof (RECT), &rect, sizeof (RECT));
	rcScroll.right -= rcScroll.left;
	rcScroll.bottom -= rcScroll.top;
	rcScroll.top = 0;
	rcScroll.left = rcScroll.right - IGSCROLLBAR_WIDTH;
		
	::MoveWindow (m_hWnd, rcScroll.left, rcScroll.top,
					rcScroll.right - rcScroll.left,
					rcScroll.bottom - rcScroll.top, FALSE);

	::GetClientRect (m_hWnd, &m_rectClient);

	::memcpy_s (&m_rectLeftArrow, sizeof (RECT), &m_rectClient, sizeof (RECT));
	m_rectLeftArrow.right = m_rectClient.left + m_nBitmapWidth;

	::memcpy_s (&m_rectRightArrow, sizeof (RECT), &m_rectClient, sizeof (RECT));
	m_rectRightArrow.left = m_rectClient.right - m_nBitmapWidth;

	::memcpy_s (&m_rectUpArrow, sizeof (RECT), &m_rectClient, sizeof (RECT));
	m_rectUpArrow.bottom = m_rectClient.top + m_nIconBorderHeight;

	::memcpy_s (&m_rectDownArrow, sizeof (RECT), &m_rectClient, sizeof (RECT));
	m_rectDownArrow.top = m_rectClient.bottom - m_nIconBorderHeight;
}

void IGScrollBar::Draw (HDC hdc /*= NULL*/)
{
	if (m_bHorizontal)
		DrawHorizontal(hdc);
	else
		DrawVertical(hdc);
}

void IGScrollBar::DrawHorizontal (HDC hdc)
{
	IGMemDC memDC (hdc, &m_rectClient);

	HBRUSH brushFrame = ::CreateSolidBrush (FRAME_COLOR);
	HBRUSH brushHot = ::CreateSolidBrush (m_ArrowHotColor);

	HDC bitmapDC = ::CreateCompatibleDC (hdc);
	HGDIOBJ hOldBitmap = NULL;

	// =====  draw left arrow  =====
	if (!m_bDragging)
	{
		hOldBitmap = ::SelectObject (bitmapDC, m_bmpUpLeftArrow);

		RECT rectLeft;
		::memcpy_s (&rectLeft, sizeof (RECT), &m_rectLeftArrow, sizeof (RECT));
		if (m_bMouseDownArrowLeft)
		{
			rectLeft.left++;rectLeft.right++;
			rectLeft.top++;rectLeft.bottom++;
		}

		memDC.StretchBlt (rectLeft.left, rectLeft.top + 1,
							rectLeft.right - rectLeft.left,
							rectLeft.bottom - rectLeft.top,
							bitmapDC, 0, 0, m_nBitmapWidth, m_nIconBorderHeight, SRCCOPY);

		if (m_bMouseOverArrowLeft)
		{
			RECT rect;
			::memcpy_s (&rect, sizeof (RECT), &m_rectLeftArrow, sizeof (RECT));
			rect.left++;rect.right--;
			rect.top++;rect.bottom--;

			memDC.FrameRect (&rect, brushHot);
			rect.left++;rect.right++;
			rect.top++;rect.bottom++;
			memDC.FrameRect (&rect, brushHot);
		}
		else
		{
			memDC.FrameRect (&m_rectLeftArrow, brushFrame);
		}

		if (hOldBitmap)
			::SelectObject (bitmapDC, hOldBitmap);
		hOldBitmap = NULL;
	}

	int nChannelStart = m_rectClient.left + m_nBitmapWidth;
	int nChannelWidth = m_rectClient.right - m_rectClient.left - 2 * m_nBitmapWidth;

	// =====  draw channel  =====
	// save new thumb position
	m_rectThumb.left   = m_rectClient.left + m_nThumbLeft;
	m_rectThumb.right  = m_rectThumb.left + m_nBitmapWidth;
	m_rectThumb.top    = m_rectClient.top;
	m_rectThumb.bottom = m_rectThumb.top + m_rectClient.bottom - m_rectClient.top;

	HBITMAP bmpChannel;
	bmpChannel = (HBITMAP)::LoadImageW (GetInstance(), MAKEINTRESOURCEW (IDB_HORIZONTAL_SCROLLBAR_CHANNEL),
									IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);

	hOldBitmap = ::SelectObject (bitmapDC, bmpChannel);

	RECT rectChannelRight = {m_rectThumb.left + m_nBitmapWidth / 2, m_rectClient.top,
		nChannelStart + nChannelWidth, m_rectClient.bottom};

	memDC.StretchBlt (rectChannelRight.left, rectChannelRight.top + 1,
		rectChannelRight.right - rectChannelRight.left,
		rectChannelRight.bottom - rectChannelRight.top - 1,
		bitmapDC, 0, 0, 1, m_nBitmapHeight, SRCCOPY);

	if (m_bChannelColor && m_bThumbColor)
	{
		COLORREF rgb1, rgb2, rgb3;
		GetChannelColors(rgb1, rgb2, rgb3);

		BITMAP bm;
		::GetObject (bmpChannel, sizeof (BITMAP), &bm);

		// set highlight colors
		::SetPixel (bitmapDC, 0, 0, rgb1);
		::SetPixel (bitmapDC, 0, 1, rgb2);

		// set main color
		for (int y = 2; y < (bm.bmHeight); y++)
			::SetPixel (bitmapDC, 0, y, rgb3);
	}

	RECT rectChannelLeft = {nChannelStart, m_rectClient.top,
		m_rectThumb.left + m_nBitmapWidth / 2, m_rectClient.bottom};

	memDC.StretchBlt (rectChannelLeft.left, rectChannelLeft.top + 1,
		rectChannelLeft.right - rectChannelLeft.left, 
		rectChannelLeft.bottom - rectChannelLeft.top - 1,
		bitmapDC, 0, 0, 1, m_nBitmapHeight, SRCCOPY);

	if (hOldBitmap)
		::SelectObject (bitmapDC, hOldBitmap);
	if (bmpChannel)
		::DeleteObject (bmpChannel);
	hOldBitmap = NULL;

	// =====  draw right arrow  =====
	if (!m_bDragging)
	{
		hOldBitmap = ::SelectObject (bitmapDC, m_bmpDownRightArrow);

		RECT rectRight;
		::memcpy_s (&rectRight, sizeof (RECT), &m_rectRightArrow, sizeof (RECT));

		if (m_bMouseDownArrowRight)
		{
			rectRight.left++;rectRight.right++;
			rectRight.top++;rectRight.bottom++;
		}

		memDC.StretchBlt (rectRight.left, rectRight.top+1,
			rectRight.right - rectRight.right, 
			rectRight.bottom - rectRight.top - 1,
			bitmapDC, 0, 0, m_nBitmapWidth, m_nIconBorderHeight, SRCCOPY);

		if (m_bMouseOverArrowRight)
		{
			RECT rect;
			::memcpy_s (&rect, sizeof (RECT), &m_rectRightArrow, sizeof (RECT));
			rect.right--;
			rect.top++;rect.bottom--;

			memDC.FrameRect (&rect, brushHot);
			rect.left++;rect.right++;
			rect.top++;rect.bottom++;
			memDC.FrameRect (&rect, brushHot);
		}
		else
		{
			memDC.FrameRect (&m_rectRightArrow, brushFrame);
		}

		if (hOldBitmap)
			::SelectObject (bitmapDC, hOldBitmap);
		hOldBitmap = NULL;
	}

	// If there is nothing to scroll then don't show the thumb
	if (m_nRange)
	{
		// =====  draw thumb  =====
		if (m_bThumbHover || (::GetFocus() == m_hWnd))
			hOldBitmap = ::SelectObject (bitmapDC, m_bmpThumbHot);
		else
			hOldBitmap = ::SelectObject (bitmapDC, m_bmpThumb);

		// fill in transparent channel color
		for (int x = 0; x < m_nBitmapWidth; x++)
		{
			for (int y = 0; y < m_nBitmapHeight; y++)
			{
				COLORREF rgb = ::GetPixel (bitmapDC, x, y);

				if (rgb == THUMB_LEFT_TRANSPARENT_MASK_COLOR)
					::SetPixel (bitmapDC, x, y, memDC.GetPixel (nChannelStart, y));
				else if (rgb == THUMB_RIGHT_TRANSPARENT_MASK_COLOR)
					::SetPixel (bitmapDC, x, y, memDC.GetPixel (nChannelStart + nChannelWidth - 1, y));
			}
		}

		memDC.StretchBlt (m_rectThumb.left, m_rectThumb.top,
			m_rectThumb.right - m_rectThumb.left,
			m_rectThumb.bottom - m_rectThumb.top,
			bitmapDC, 0, 0, m_nBitmapWidth, m_nBitmapHeight, SRCCOPY);

		if (!m_bThumbColor)
		{
			if (m_bThumbHover || (::GetFocus() == m_hWnd))
			{
				RECT rect;
				::memcpy_s (&rect, sizeof (RECT), &m_rectThumb, sizeof (RECT));
				rect.left++;rect.top++;
				rect.right--;rect.bottom-=2;				
				memDC.FrameRect (&rect, brushHot);
				rect.left++;rect.top++;
				rect.right--;rect.bottom--;
				memDC.FrameRect (&rect, brushHot);
			}
		}

		if (hOldBitmap)
			::SelectObject (bitmapDC, hOldBitmap);
		hOldBitmap = NULL;
	}
	else
	{
		m_rectThumb.left=-1; m_rectThumb.right=-1;
		m_rectThumb.top=-1; m_rectThumb.bottom=-1;
	}

	memDC.FrameRect (&m_rectClient, brushFrame);
	::DeleteObject (brushFrame);
	::DeleteObject (brushHot);
}

void IGScrollBar::DrawVertical (HDC hdc)
{
	Graphics graphics (hdc);	
	graphics.SetSmoothingMode (SmoothingModeAntiAlias);
	SolidBrush backgroundBrush (FRAME_COLOR_BACKGROUND);
	Pen pen (THUMB_COLOR_ITEMBORDER);
	SolidBrush blackBrush (Color (255, 0, 0, 0));	
	SolidBrush backgroundSelectedBrush (THUMB_COLOR_SELECTEDITEM);
	Pen penSelected (THUMB_COLOR_SELECTEDITEMBORDER);
	GraphicsPath path;
				
	// =====  draw Up arrow  =====
	if (!m_bDragging)
	{
		RECT rectUp;
		::memcpy_s (&rectUp, sizeof (RECT), &m_rectUpArrow, sizeof (RECT));
		
		graphics.FillRectangle (&backgroundBrush, Rect (rectUp.left, rectUp.top, 
											rectUp.right - rectUp.left, rectUp.bottom - rectUp.top));

		if (m_bMouseDownArrowUp)
		{
			rectUp.left++;rectUp.top++;
			rectUp.right++;rectUp.bottom++;
		}

		path.StartFigure();
		path.AddEllipse (Rect (rectUp.left + 1, rectUp.top + 1, 
			rectUp.right - rectUp.left - 2, rectUp.bottom - rectUp.top - 2));
		graphics.FillPath (&backgroundBrush, &path);
		graphics.DrawPath (&pen, &path);

		if (m_bMouseOverArrowUp)
		{
			graphics.FillPath (&backgroundSelectedBrush, &path);
			graphics.DrawPath (&penSelected, &path);
		}
		
		PointF trg[3];
		trg[0] = PointF (1.0f + (float)(rectUp.right - rectUp.left - 2) / 5.0f, 1.0f + 3.5f * (float)(rectUp.bottom - rectUp.top - 2) / 5.0f);
		trg[1] = PointF (1.0f + (float)(rectUp.right - rectUp.left - 2) / 2.0f, 1.0f + (float)(rectUp.bottom - rectUp.top - 2) / 5.0f);
		trg[2] = PointF (1.0f + (4.0f * (float)(rectUp.right - rectUp.left - 2)) / 5.0f, 1.0f + 3.5f * (float)(rectUp.bottom - rectUp.top - 2) / 5.0f);
		path.Reset();
		path.StartFigure();
		path.AddLine (trg[0], trg[1]);
		path.AddLine (trg[2], trg[0]);			
		graphics.FillPath (&blackBrush, &path);
	}

	int nChannelStart = m_rectClient.top + m_nIconBorderHeight;
	int nChannelHeight = m_rectClient.bottom - m_rectClient.top - 2 * m_nIconBorderHeight;

	// =====  draw channel  =====
	// save new thumb position
	m_rectThumb.left   = m_rectClient.left;
	m_rectThumb.right  = m_rectThumb.left + m_rectClient.right - m_rectClient.left;
	m_rectThumb.top    = m_rectClient.top + m_nThumbTop;
	m_rectThumb.bottom = m_rectThumb.top + m_nThumbHeight;//m_nBitmapHeight;

	RECT rectChannelDown = {m_rectClient.left, m_rectThumb.top + m_nThumbHeight, 
		m_rectClient.right, nChannelStart + nChannelHeight};

	graphics.FillRectangle (&backgroundBrush, Rect (rectChannelDown.left, rectChannelDown.top, 
											rectChannelDown.right - rectChannelDown.left, rectChannelDown.bottom - rectChannelDown.top));
	graphics.FillRectangle (&blackBrush, RectF ((float)rectChannelDown.left + 4.5f, (float)rectChannelDown.top, 
											3.0f, (float)(rectChannelDown.bottom - rectChannelDown.top)));

	RECT rectChannelUp = {m_rectClient.left, nChannelStart, 
		m_rectClient.right, m_rectThumb.top};

	graphics.FillRectangle (&backgroundBrush, Rect (rectChannelUp.left, rectChannelUp.top, 
											rectChannelUp.right - rectChannelUp.left, rectChannelUp.bottom - rectChannelUp.top));
	graphics.FillRectangle (&blackBrush, RectF ((float)rectChannelUp.left + 4.5f, (float)rectChannelUp.top, 
											3.0f, (float)(rectChannelUp.bottom - rectChannelUp.top)));

	// =====  draw down arrow  =====
	if (!m_bDragging)
	{
		RECT rectDown;
		::memcpy_s (&rectDown, sizeof (RECT), &m_rectDownArrow, sizeof (RECT));
		
		graphics.FillRectangle (&backgroundBrush, Rect (rectDown.left, rectDown.top, 
											rectDown.right - rectDown.left, rectDown.bottom - rectDown.top));

		if (m_bMouseDownArrowDown)
		{
			rectDown.left++;rectDown.top++;
			rectDown.right++;rectDown.bottom++;
		}

		path.Reset();
		path.StartFigure();
		path.AddEllipse (Rect (rectDown.left + 1, rectDown.top + 1, 
			rectDown.right - rectDown.left - 2, rectDown.bottom - rectDown.top - 2));
		graphics.FillPath (&backgroundBrush, &path);
		graphics.DrawPath (&pen, &path);

		if (m_bMouseOverArrowDown)
		{
			graphics.FillPath (&backgroundSelectedBrush, &path);
			graphics.DrawPath (&penSelected, &path);
		}

		PointF trg[3];
		trg[0] = PointF (1.0f + (float)(rectDown.right - rectDown.left - 2) / 5.0f, 1.0f + (float)rectDown.top + (float)(rectDown.bottom - rectDown.top - 2) / 5.0f);
		trg[1] = PointF (1.0f + (float)(rectDown.right - rectDown.left - 2) / 2.0f, 1.0f + (float)rectDown.top + 3.5f * (float)(rectDown.bottom - rectDown.top - 2) / 5.0f);
		trg[2] = PointF (1.0f + (4.0f * (float)(rectDown.right - rectDown.left - 2)) / 5.0f, 1.0f + (float)rectDown.top + (float)(rectDown.bottom - rectDown.top - 2) / 5.0f);
		path.Reset();
		path.StartFigure();
		path.AddLine (trg[0], trg[1]);
		path.AddLine (trg[2], trg[0]);	
		graphics.FillPath (&blackBrush, &path);
	}

	// if there is nothing to scroll then don't show the thumb
	if (m_nRange)
	{
		graphics.FillRectangle (&backgroundBrush, Rect (m_rectThumb.left, m_rectThumb.top, 
											m_rectThumb.right - m_rectThumb.left, m_rectThumb.bottom - m_rectThumb.top));
	
		// =====  draw thumb  =====
		float fDiam = (float)(m_rectThumb.right - m_rectThumb.left - 2);
		float fRay = fDiam / 2.0f;
		path.Reset();
		path.StartFigure();
		path.AddArc (RectF ((float)m_rectThumb.left + 1.0f, (float)m_rectThumb.top, 
							fDiam, fDiam), 180.0f, 180.0f);
		path.AddLine (1.0f + fDiam, (float)m_rectThumb.top + fRay, 1.0f + fDiam, (float)m_rectThumb.bottom - fRay);
		path.AddArc (RectF ((float)m_rectThumb.left + 1.0f, (float)m_rectThumb.bottom - fDiam, 
							fDiam, fDiam), 0.0f, 180.0f);
		path.AddLine ((float)m_rectThumb.left + 1.0f, (float)m_rectThumb.bottom - fRay, (float)m_rectThumb.left + 1.0f, (float)m_rectThumb.top + fRay);
		graphics.FillPath (&backgroundBrush, &path);
		graphics.DrawPath (&pen, &path);

		int nGrips = (m_rectThumb.bottom - m_rectThumb.top) / 6;
		int nOffsetY = 0;
		if (nGrips > 5)
		{
			nOffsetY = ((nGrips - 4) * 3);
			nGrips = 5;
		}
		if ((m_rectThumb.bottom - m_rectThumb.top) % 6 != 0)
			nGrips++;
		Pen blackPen (Color (255, 0, 0, 0));
		for (int idxGrip = 1; idxGrip <= nGrips; idxGrip++)
		{
			bool bFirstOrLast = (idxGrip == 1 || idxGrip == nGrips);
			graphics.DrawLine (&blackPen, bFirstOrLast ? 4.5f : 3.0f, (float)nOffsetY + (float)m_rectThumb.top + (float)idxGrip * (float)(m_rectThumb.bottom - m_rectThumb.top - 2 * nOffsetY) / (float)(nGrips + 1), 
								(float)(m_rectThumb.right - m_rectThumb.left) - (bFirstOrLast ? 5.5f : 4.0f), (float)nOffsetY + (float)m_rectThumb.top + (float)idxGrip * (float)(m_rectThumb.bottom - m_rectThumb.top - 2 * nOffsetY) / (float)(nGrips + 1));
		}

		if (!m_bThumbColor)
		{
			if (m_bThumbHover || (::GetFocus() == m_hWnd))
			{
				graphics.FillPath (&backgroundSelectedBrush, &path);
				graphics.DrawPath (&penSelected, &path);
			}
		}
	}
	else
	{
		m_rectThumb.left=-1; m_rectThumb.right=-1;
		m_rectThumb.top=-1; m_rectThumb.bottom=-1;
	}
}


void IGScrollBar::GetChannelColors(COLORREF& rgb1, 
								   COLORREF& rgb2, 
								   COLORREF& rgb3)
{
	IGColor color;
	color.SetRGB(GetRValue(m_ThumbColor),
				 GetGValue(m_ThumbColor), 
				 GetBValue(m_ThumbColor));
	color.ToHLS();
	float fLuminance = color.GetLuminance();

	// use 80% L, 50% S for main color
	fLuminance = 0.8f;
	float fSaturation = color.GetSaturation();
	fSaturation = 0.5f * fSaturation;
	float fHue = color.GetHue();
	color.SetHLS(fHue, fLuminance, fSaturation);
	color.ToRGB();
	rgb3 = RGB(color.GetRed(), color.GetGreen(), color.GetBlue());

	// use .87 L for second highlight color
	fLuminance = .87f;
	color.SetHLS(fHue, fLuminance, fSaturation);
	color.ToRGB();
	rgb2 = RGB(color.GetRed(), color.GetGreen(), color.GetBlue());

	// use .92 L for first highlight color
	fLuminance = .92f;
	color.SetHLS(fHue, fLuminance, fSaturation);
	color.ToRGB();
	rgb1 = RGB(color.GetRed(), color.GetGreen(), color.GetBlue());
}

void IGScrollBar::OnLButtonDown (UINT nFlags, LPPOINT point)
{
	::KillTimer (m_hWnd, TIMER_MOUSE_OVER_BUTTON);
	::KillTimer (m_hWnd, TIMER_LBUTTON_PRESSED);

	if (m_bHorizontal)
	{
		m_bMouseDownArrowLeft = FALSE;
		m_bMouseDownArrowRight = FALSE;
		RECT rectThumb = {m_nThumbLeft, 0, m_nThumbLeft + m_nBitmapWidth, 
			m_rectClient.bottom - m_rectClient.top};

		if (::PtInRect (&rectThumb, *point))
		{
			m_bMouseDown = TRUE;
			::SetCapture (m_hWnd);
		}
		else if (::PtInRect (&m_rectRightArrow, *point))
		{
			m_bMouseDownArrowRight = TRUE;
			::SetTimer (m_hWnd, TIMER_LBUTTON_PRESSED, 200, NULL);
		}
		else if (::PtInRect (&m_rectLeftArrow, *point))
		{
			m_bMouseDownArrowLeft = TRUE;
			::SetTimer (m_hWnd, TIMER_LBUTTON_PRESSED, 200, NULL);
		}
		else	// button down in channel
		{
			m_nThumbLeft = point->x - m_nBitmapWidth / 2;
			SetPositionFromThumb();

			if (m_hParent && ::IsWindow(m_hParent))
				::SendMessageW (m_hParent, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos),
				(LPARAM)m_hWnd);
		}
		::InvalidateRect (m_hWnd, NULL, FALSE);
	}
	else
	{
		RECT rectThumb = {0, m_nThumbTop, m_rectClient.right - m_rectClient.left,
							m_nThumbTop + m_nThumbHeight};

		if (::PtInRect (&rectThumb, *point))
		{
			m_bMouseDown = TRUE;
			::SetCapture (m_hWnd);
		}
		else if (::PtInRect (&m_rectDownArrow, *point))
		{
			m_bMouseDownArrowDown = TRUE;
			::SetTimer (m_hWnd, TIMER_LBUTTON_PRESSED, 150, NULL);
		}
		else if (::PtInRect (&m_rectUpArrow, *point))
		{
			m_bMouseDownArrowUp = TRUE;
			::SetTimer (m_hWnd, TIMER_LBUTTON_PRESSED, 150, NULL);
		}
		else	// button down in channel
		{
			m_nThumbTop = point->y - m_nThumbHeight / 2;
			SetPositionFromThumb();

			if (m_hParent && ::IsWindow(m_hParent))
				::SendMessageW (m_hParent, WM_VSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), 
				(LPARAM)m_hWnd);
		}
		::InvalidateRect (m_hWnd, NULL, FALSE);
	}
}

void IGScrollBar::OnLButtonUp (UINT nFlags, LPPOINT point)
{
	UpdateThumbPosition();
	::KillTimer (m_hWnd, TIMER_MOUSE_OVER_BUTTON);
	::KillTimer (m_hWnd, TIMER_LBUTTON_PRESSED);

	if (m_bHorizontal)
	{
		RECT rectThumb = {m_nThumbLeft, 0, m_nThumbLeft + m_nBitmapWidth, 
			m_rectClient.bottom - m_rectClient.top};

		m_bMouseDownArrowLeft = FALSE;
		m_bMouseDownArrowRight = FALSE;

		if (::PtInRect (&m_rectLeftArrow, *point))
		{
			ScrollLeft();
		}
		else if (::PtInRect (&m_rectRightArrow, *point))
		{
			ScrollRight();
		}
		else if (::PtInRect (&rectThumb, *point))
		{
			m_bThumbHover = TRUE;
			::SetTimer (m_hWnd, TIMER_MOUSE_OVER_THUMB, 50, NULL);
		}		
	}
	else
	{
		RECT rectThumb = {0, m_nThumbTop, m_rectClient.right - m_rectClient.left, 
			m_nThumbTop + m_nThumbHeight};

		m_bMouseDownArrowUp = FALSE;
		m_bMouseDownArrowDown = FALSE;

		if (::PtInRect (&m_rectUpArrow, *point))
		{
			ScrollUp();
		}
		else if (::PtInRect (&m_rectDownArrow, *point))
		{
			ScrollDown();
		}
		else if (::PtInRect (&rectThumb, *point))
		{
			m_bThumbHover = TRUE;
			::SetTimer (m_hWnd, TIMER_MOUSE_OVER_THUMB, 50, NULL);
		}
	}

	m_bMouseDown = FALSE;
	m_bDragging = FALSE;
	::InvalidateRect (m_hWnd, NULL, FALSE);
	::UpdateWindow (m_hWnd);
	::ReleaseCapture();
}

void IGScrollBar::UpdateScrollPos (int nPos)
{
	int nOldPos = m_nPos;
	SetScrollPos (nPos, FALSE);
	if (m_nPos != nOldPos)
		::SendMessageW (m_hParent, WM_VSCROLL, MAKELONG(SB_THUMBPOSITION, m_nPos), (LPARAM)m_hWnd);	
}

void IGScrollBar::OnMouseWheel(int nDelta)
{
	UpdateScrollPos (m_nPos - nDelta);
}

void IGScrollBar::OnLButtonDblClk(UINT nFlags, LPPOINT point) 
{
	OnLButtonDown (nFlags, point);
}

void IGScrollBar::OnMouseMove(UINT nFlags, LPPOINT point)
{
	BOOL bOldThumbHover = m_bThumbHover;
	m_bThumbHover = FALSE;

	if (::PtInRect (&m_rectThumb, *point))
		m_bThumbHover = TRUE;

	if (m_bMouseDown)
		m_bDragging = TRUE;

	BOOL bOldHover = m_bMouseOverArrowRight |
					 m_bMouseOverArrowLeft  |
					 m_bMouseOverArrowUp    |
					 m_bMouseOverArrowDown;

	m_bMouseOverArrowRight = FALSE;
	m_bMouseOverArrowLeft  = FALSE;
	m_bMouseOverArrowUp    = FALSE;
	m_bMouseOverArrowDown  = FALSE;

	if (m_bHorizontal)
	{
		if (::PtInRect (&m_rectLeftArrow, *point))
			m_bMouseOverArrowLeft = TRUE;
		else if (::PtInRect (&m_rectRightArrow, *point))
			m_bMouseOverArrowRight = TRUE;
	}
	else
	{
		if (::PtInRect (&m_rectUpArrow, *point))
			m_bMouseOverArrowUp = TRUE;
		else if (::PtInRect (&m_rectDownArrow, *point))
			m_bMouseOverArrowDown = TRUE;
	}

	BOOL bNewHover = m_bMouseOverArrowRight |
					 m_bMouseOverArrowLeft  |
					 m_bMouseOverArrowUp    |
					 m_bMouseOverArrowDown;

	if (bNewHover)
		::SetTimer (m_hWnd, TIMER_MOUSE_HOVER_BUTTON, 80, NULL);

	if (bOldHover != bNewHover)
		::InvalidateRect (m_hWnd, NULL, FALSE);

	if (m_bDragging)
	{
		if (m_bHorizontal)
		{
			m_nThumbLeft = point->x - m_nBitmapWidth / 2;

			SetPositionFromThumb();

			if (m_hParent && ::IsWindow(m_hParent))
				::SendMessageW (m_hParent, WM_HSCROLL, MAKELONG(SB_THUMBTRACK, m_nPos), 
					(LPARAM)m_hWnd);
		}
		else
		{
			m_nThumbTop = point->y - m_nThumbHeight / 2;
			int nNewPos = SetPositionFromThumb (false);
			if (m_hParent && ::IsWindow(m_hParent))
				UpdateScrollPos (nNewPos);
		}
	}

	if (bOldThumbHover != m_bThumbHover)
	{
		::InvalidateRect (m_hWnd, NULL, FALSE);
		::SetTimer (m_hWnd, TIMER_MOUSE_OVER_THUMB, 50, NULL);
	}
}

void IGScrollBar::OnTimer(UINT nIDEvent)
{
	POINT point;
	::GetCursorPos (&point);
	::ScreenToClient (m_hWnd, &point);

	RECT rect;
	::GetWindowRect (m_hWnd, &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	rect.bottom = rect.top = 0;

	if (nIDEvent == TIMER_MOUSE_HOVER_BUTTON)
	{
		m_bMouseOverArrowRight = FALSE;
		m_bMouseOverArrowLeft  = FALSE;
		m_bMouseOverArrowUp    = FALSE;
		m_bMouseOverArrowDown  = FALSE;

		if (m_bHorizontal)
		{
			if (::PtInRect (&m_rectLeftArrow, point))
				m_bMouseOverArrowLeft = TRUE;
			else if (::PtInRect (&m_rectRightArrow, point))
				m_bMouseOverArrowRight = TRUE;
		}
		else
		{
			if (::PtInRect (&m_rectUpArrow, point))
				m_bMouseOverArrowUp = TRUE;
			else if (::PtInRect (&m_rectDownArrow, point))
				m_bMouseOverArrowDown = TRUE;
		}

		if (!m_bMouseOverArrowLeft  &&
			!m_bMouseOverArrowRight && 
			!m_bMouseOverArrowUp    &&
			!m_bMouseOverArrowDown)
		{
			::KillTimer (m_hWnd, nIDEvent);
			::InvalidateRect (m_hWnd, NULL, FALSE);
		}
	}
	else if (nIDEvent == TIMER_MOUSE_OVER_BUTTON)	// mouse is in an arrow button,
													// and left button is down
	{
		if (m_bMouseDownArrowLeft)
			ScrollLeft();
		if (m_bMouseDownArrowRight)
			ScrollRight();
		if (m_bMouseDownArrowUp)
			ScrollUp();
		if (m_bMouseDownArrowDown)
			ScrollDown();

		if (!::PtInRect (&rect, point))
		{
			m_bMouseDownArrowLeft  = FALSE;
			m_bMouseDownArrowRight = FALSE;
			m_bMouseDownArrowUp    = FALSE;
			m_bMouseDownArrowDown  = FALSE;
		}
		if (!m_bMouseDownArrowLeft  &&
			!m_bMouseDownArrowRight && 
			!m_bMouseDownArrowUp    &&
			!m_bMouseDownArrowDown)
		{
			::KillTimer (m_hWnd, nIDEvent);
			::InvalidateRect (m_hWnd, NULL, FALSE);
		}
	}
	else if (nIDEvent == TIMER_LBUTTON_PRESSED)	// mouse is in an arrow button,
												// and left button has just been pressed
	{
		::KillTimer (m_hWnd, nIDEvent);

		if (m_bMouseDownArrowLeft  || 
			m_bMouseDownArrowRight || 
			m_bMouseDownArrowUp    || 
			m_bMouseDownArrowDown)
		{
			// debounce left click
			::SetTimer (m_hWnd, TIMER_MOUSE_OVER_BUTTON, MOUSE_OVER_BUTTON_TIME, NULL);
			::InvalidateRect (m_hWnd, NULL, FALSE);
		}
	}
	else if (nIDEvent == TIMER_MOUSE_OVER_THUMB)	// mouse is over thumb
	{
		if (!::PtInRect (&m_rectThumb, point))
		{
			// no longer over thumb, restore thumb color
			m_bThumbHover = FALSE;
			::KillTimer (m_hWnd, nIDEvent);
			::SetCursor (::LoadCursor(NULL, IDC_ARROW));
			::InvalidateRect (m_hWnd, NULL, FALSE);
		}
	}
}

void IGScrollBar::ScrollLeft()
{
	if (m_nPos > 0)
		m_nPos--;
	if (m_hParent && ::IsWindow (m_hParent))
		::SendMessageW (m_hParent, WM_HSCROLL, MAKELONG(SB_LINELEFT,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

void IGScrollBar::ScrollRight()
{
	if (m_nPos < m_nRange)
		m_nPos++;
	if (m_hParent && ::IsWindow (m_hParent))
		::SendMessageW (m_hParent, WM_HSCROLL, MAKELONG(SB_LINERIGHT,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

void IGScrollBar::ScrollUp()
{
	if (m_nPos > 0)
		m_nPos--;
	if (m_hParent && ::IsWindow(m_hParent))
		::SendMessageW (m_hParent, WM_VSCROLL, MAKELONG(SB_LINEUP,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

void IGScrollBar::ScrollDown()
{
	if (m_nPos < m_nRange)
		m_nPos++;
	if (m_hParent && ::IsWindow (m_hParent))
		::SendMessageW (m_hParent, WM_VSCROLL, MAKELONG(SB_LINEDOWN,0), (LPARAM)m_hWnd);
	UpdateThumbPosition();
}

int IGScrollBar::SetPositionFromThumb (bool bUpdate)
{
	double dPixels, dMax, dInterval, dPos;

	LimitThumbPosition();
	dMax = m_nRange;

	if (m_bHorizontal)
	{
		dPixels   = m_rectClient.right - m_rectClient.left - 3 * m_nBitmapWidth;
		dInterval = dMax / dPixels;
		dPos      = dInterval * (m_nThumbLeft - m_nBitmapWidth);
	}
	else
	{
		dPixels   = m_rectClient.bottom - m_rectClient.top - 2 * m_nIconBorderHeight - m_nThumbHeight;
		dInterval = dMax / dPixels;
		dPos      = dInterval * (m_nThumbTop - m_nIconBorderHeight);
	}

	int nPos = (int) (dPos + 0.5);
	if (nPos < 0)
		nPos = 0;
	if (nPos > m_nRange)
		nPos = m_nRange;
	if (bUpdate)
		m_nPos = nPos;
	return nPos;	
}

void IGScrollBar::UpdateThumbPosition()
{
	double dPixels, dMax, dInterval, dPos;

	dMax = m_nRange;
	dPos = m_nPos;
	bool bThumbPositionChanged = false;

	if (m_bHorizontal)
	{
		dPixels   = m_rectClient.right - m_rectClient.left - 3 * m_nBitmapWidth;
		dInterval = dPixels / dMax;
		double dThumbLeft = dPos * dInterval + 0.5;
		if (m_nThumbLeft != m_nBitmapWidth + (int)dThumbLeft)
		{
			bThumbPositionChanged = true;
			m_nThumbLeft = m_nBitmapWidth + (int)dThumbLeft;
		}
	}
	else
	{
		dPixels   = m_rectClient.bottom - m_rectClient.top - 2 * m_nIconBorderHeight - m_nThumbHeight;
		dInterval = dPixels / dMax;
		double dThumbTop = dPos * dInterval + 0.5;

		m_nThumbHeight = (int)((float)(m_rectClient.bottom - m_rectClient.top - 2 * m_nIconBorderHeight - m_nRange * TICKHEIGHT_OFFSET));
		if (m_nThumbHeight < THUMBHEIGHT_MIN)
			m_nThumbHeight = THUMBHEIGHT_MIN;

		if (m_nThumbTop != m_nIconBorderHeight + (int)dThumbTop)
		{
			bThumbPositionChanged = true;
			m_nThumbTop = m_nIconBorderHeight + (int)dThumbTop;
		}
	}

	LimitThumbPosition();
}

void IGScrollBar::LimitThumbPosition()
{
	if (m_bHorizontal)
	{
		if ((m_nThumbLeft + m_nBitmapWidth) > (m_rectClient.right - m_rectClient.left - m_nBitmapWidth))
			m_nThumbLeft = m_rectClient.right - m_rectClient.left - 2 * m_nBitmapWidth;

		if (m_nThumbLeft < (m_rectClient.left + m_nBitmapWidth))
			m_nThumbLeft = m_rectClient.left + m_nBitmapWidth;
	}
	else
	{
		if ((m_nThumbTop + m_nThumbHeight) > (m_rectClient.bottom - m_rectClient.top - m_nIconBorderHeight))
			m_nThumbTop = m_rectClient.bottom - m_rectClient.top - m_nThumbHeight - m_nIconBorderHeight;

		if (m_nThumbTop < (m_rectClient.top + m_nIconBorderHeight))
			m_nThumbTop = m_rectClient.top + m_nIconBorderHeight;
	}
}

IGScrollBar& IGScrollBar::SetScrollRange(int nMinPos,
										 int nMaxPos,
										 BOOL bRedraw /*= TRUE*/)
{
	m_nMinPos = nMinPos;
	m_nMaxPos = nMaxPos;
	if (m_nMinPos < m_nMaxPos)
		m_nRange = m_nMaxPos - m_nMinPos;
	else
		m_nRange = m_nMinPos - m_nMaxPos;

	if (bRedraw)
		::InvalidateRect (m_hWnd, NULL, FALSE);

	return *this;
}

int IGScrollBar::SetScrollPos (int nPos, BOOL bRedraw /*= TRUE*/)
{
	int nOldPos = m_nPos;
	if ((nPos <= m_nRange) && (nPos >= 0))
	{
		m_nPos = nPos;

		UpdateThumbPosition();

		if (bRedraw)
			::InvalidateRect (m_hWnd, NULL, FALSE);
	}
	return nOldPos;
}

BOOL IGScrollBar::OnSetCursor(HWND /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
	if (m_bThumbHover && m_hCursor)
		::SetCursor (m_hCursor);
	else
		::SetCursor (::LoadCursor (NULL, IDC_ARROW));

	return TRUE;
}

void IGScrollBar::SetDefaultCursor()
{
	if (m_hCursor == NULL)				// No cursor handle - try to load one
	{
		m_hCursor =  ::LoadCursorW (GetInstance(), IDC_HAND);
	}
}

IGScrollBar& IGScrollBar::SetThumbColor(COLORREF rgb) 
{ 
	m_ThumbColor = rgb;
	m_bThumbColor = (m_ThumbColor == CLR_INVALID) ? FALSE : TRUE;
	LoadThumbBitmap();
	return *this;
}

IGScrollBar& IGScrollBar::EnableThumbColor(BOOL bEnable) 
{ 
	m_bThumbColor = bEnable; 
	LoadThumbBitmap();
	return *this;
}

void IGScrollBar::ColorThumb(HDC hdc, COLORREF rgbThumb)
{
	COLORREF rgbPrev = 0;

	// add desired hot color to thumb
	for (int x = 0; x < m_nBitmapWidth; x++)
	{
		for (int y = 0; y < m_nThumbHeight; y++)
		{
			COLORREF rgb = ::GetPixel (hdc, x, y);

			if (m_bThumbColor && (rgb == THUMB_MASK_COLOR))
			{
				::SetPixel (hdc, x, y, rgbThumb);
			}
			else if (rgb == THUMB_GRIPPER_MASK_COLOR)
			{
				if (m_bThumbGripper)
					::SetPixel (hdc, x, y, THUMB_GRIPPER_COLOR);
				else
					::SetPixel (hdc, x, y, rgbPrev);
			}

			rgbPrev = rgb;
		}
	}
}

void IGScrollBar::LoadThumbBitmap()
{
	if (m_bmpThumb)
		::DeleteObject (m_bmpThumb);

	if (m_bmpThumbHot)
		::DeleteObject (m_bmpThumbHot);

	if (m_bThumbColor)
	{
		m_bmpThumb = (HBITMAP)::LoadImageW (GetInstance(), MAKEINTRESOURCEW (m_bHorizontal ? 
																			 IDB_HORIZONTAL_SCROLLBAR_THUMB : 
																			 IDB_VERTICAL_SCROLLBAR_THUMB),
											IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
		m_bmpThumbHot = (HBITMAP)::LoadImageW (GetInstance(), MAKEINTRESOURCEW (m_bHorizontal ? 
																			 IDB_HORIZONTAL_SCROLLBAR_THUMB : 
																			 IDB_VERTICAL_SCROLLBAR_THUMB),
											IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
																		 
		HDC bitmapDC = ::CreateCompatibleDC (::GetDC (m_hWnd));

		HGDIOBJ hOldBitmap = ::SelectObject (bitmapDC, m_bmpThumbHot);

		// add desired hot color to thumb
		ColorThumb (bitmapDC, m_ThumbHoverColor);

		::SelectObject (bitmapDC, m_bmpThumb);

		// add desired cold color to thumb
		ColorThumb (bitmapDC, m_ThumbColor);

		::SelectObject (bitmapDC, hOldBitmap);
	}
	else
	{
		m_bmpThumb = (HBITMAP)::LoadImageW (GetInstance(), MAKEINTRESOURCEW (m_bHorizontal ? 
																			 IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR : 
																			 IDB_VERTICAL_SCROLLBAR_THUMB_NO_COLOR),
											IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
		m_bmpThumbHot = (HBITMAP)::LoadImageW (GetInstance(), MAKEINTRESOURCEW (m_bHorizontal ? 
																			 IDB_HORIZONTAL_SCROLLBAR_THUMB_NO_COLOR : 
																			 IDB_VERTICAL_SCROLLBAR_THUMB_NO_COLOR),
											IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_VGACOLOR);
	}
}

void IGScrollBar::Show (int nCmdShow)
{
	::ShowWindow (m_hWnd, nCmdShow);
}

BOOL IGScrollBar::IsVisible()
{
	return ::IsWindowVisible (m_hWnd);
}

BOOL IGScrollBar::OnEraseBkgnd(HDC /*hdc*/) 
{
	return TRUE;
}

void IGScrollBar::OnSize (UINT nType, int cx, int cy) 
{	
	if (m_hWnd)
	{
		::GetClientRect (m_hWnd, &m_rectClient);
		UpdateThumbPosition();
	}
}

void IGScrollBar::OnSetFocus (HWND hOldWnd) 
{
	::InvalidateRect (m_hWnd, NULL, FALSE);	
}

void IGScrollBar::OnKillFocus(HWND pNewWnd) 
{
	::InvalidateRect (m_hWnd, NULL, FALSE);	
}

void IGScrollBar::updateScrollBar ()
{
	RECT rcListBox;
	::GetWindowRect (m_hParent, &rcListBox);
	int nNbItems = ::SendMessageW (m_hParent, (UINT)LB_GETCOUNT, 0, 0); 
	int nItemHeight = ::SendMessageW (m_hParent, (UINT)LB_GETITEMHEIGHT, 0, 0);  
	float fRange = (float)((nNbItems *  nItemHeight) - (rcListBox.bottom - rcListBox.top))
					/ (float)nItemHeight;
	int nRange = (int)fRange;
	if (fRange > (float)nRange)
		nRange++;
	if (nRange > 0)
	{
		Show (SW_SHOW);
		SetScrollRange (0, nRange);
	}
	else
	{
		Show (SW_HIDE);
	}	
}

LRESULT IGScrollBar::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_SETCURSOR:
			return OnSetCursor ((HWND)wParam, LOWORD (lParam), HIWORD (lParam));

		case WM_MOUSEMOVE:
			{
				POINT pt = {LOWORD (lParam), HIWORD (lParam)};
				OnMouseMove ((UINT)wParam, &pt);
			}
			break;

		case WM_ERASEBKGND:
			return OnEraseBkgnd ((HDC)wParam);

		case WM_TIMER:
			OnTimer ((UINT)wParam);
			break;

		case WM_LBUTTONDOWN:
			{
				POINT pt = {LOWORD (lParam), HIWORD (lParam)};
				OnLButtonDown ((UINT)wParam, &pt);
			}
			break;

		case WM_LBUTTONUP:
			{
				POINT pt = {LOWORD (lParam), HIWORD (lParam)};
				OnLButtonUp ((UINT)wParam, &pt);
			}
			break;

		case WM_LBUTTONDBLCLK:
			{
				POINT pt = {LOWORD (lParam), HIWORD (lParam)};
				OnLButtonDblClk ((UINT)wParam, &pt);
			}
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = ::BeginPaint (window, &ps);
				Draw (hdc);	
				::EndPaint (window, &ps);
			}
			break;

		case WM_MOUSEWHEEL:
			OnMouseWheel (GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
			break;

		case WM_SIZE:
			OnSize ((UINT)wParam, LOWORD (lParam), HIWORD (lParam));
			break;

		case WM_SETFOCUS:
			OnSetFocus ((HWND)wParam);
			break;

		case WM_KILLFOCUS:
			OnKillFocus ((HWND)wParam);
			break;
    }
	return ::DefWindowProcW (window, message, wParam, lParam);
}

LRESULT IGScrollBar::WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(message == WM_NCCREATE)
    {
        IGScrollBar* l_self = reinterpret_cast<IGScrollBar*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
#ifdef _WIN64
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(l_self));
#else
#pragma warning(push)
#pragma warning(disable:4311)
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG>(l_self));
#pragma warning(pop)
#endif
        return l_self->WindowProc(window, message, wParam, lParam);
    }
    else
    {
#pragma warning(push)
#pragma warning(disable:4312)
        IGScrollBar* l_self = reinterpret_cast<IGScrollBar*>(GetWindowLongPtrW(window, GWLP_USERDATA));
#pragma warning(pop)
        if(l_self != NULL)
        {
            return l_self->WindowProc(window, message, wParam, lParam);
        }
    }
	return ::DefWindowProcW(window, message, wParam, lParam);
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY IGScrollBar::StaticHookScrolledWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	IGScrollBar* pThis = (IGScrollBar*)dwRefData;
	if( pThis != NULL )
		return pThis->HookScrolledWndProc (hwnd, msg, wParam, lParam);

	return ::DefSubclassProc (hwnd, msg, wParam, lParam);;
}

//------------------------------------------------------------------------------
LRESULT IGScrollBar::HookScrolledWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc (hWnd, msg, wParam, lParam);	

	switch (msg)
	{
	case LB_ADDSTRING:
	case LB_INSERTSTRING:
	case LB_DELETESTRING:
		updateScrollBar();
		break;

	case LB_SETSEL:
		if ((BOOL)wParam)
			SetScrollPos ((int)lParam);
		break;

	case LB_SETCURSEL:
		SetScrollPos ((int)lParam);
		break;

	case WM_VSCROLL:
		{
			RECT rcListBox;
			::GetWindowRect (m_hParent, &rcListBox);
			HWND hContainer = ::GetParent (m_hParent);
			MapWindowRect (NULL, hContainer, &rcListBox);
			rcListBox.right -= m_rectClient.right;
			::InvalidateRect (hContainer, &rcListBox, FALSE);	
			MapWindowRect (hContainer, m_hParent, &rcListBox);
			::InvalidateRect (m_hParent, &rcListBox, FALSE);	
			::InvalidateRect (m_hWnd, &m_rectClient, FALSE);
		}
		break;	

	case WM_MOUSEWHEEL:
		{	
			int nDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			UpdateScrollPos (GetScrollPos() - nDelta);
		}
		break;	

	case WM_LBUTTONUP:
		{
			RECT rcListBox;
			::GetWindowRect (m_hParent, &rcListBox);	
			int nItemHeight = ::SendMessageW (m_hParent, (UINT)LB_GETITEMHEIGHT, 0, 0);  
			POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			RECT rcBottomListBox = rcListBox;
			rcBottomListBox.bottom = rcListBox.bottom - rcBottomListBox.top;
			rcBottomListBox.top = rcBottomListBox.bottom - nItemHeight;
			rcBottomListBox.right = rcBottomListBox.right - rcBottomListBox.left;
			rcBottomListBox.left = 0;
			if (::PtInRect (&rcBottomListBox, pt))
				UpdateScrollPos (GetScrollPos() + 1);
			::InvalidateRect (m_hParent, NULL, FALSE);
		}
		break;	
	}

	return lRes;
}