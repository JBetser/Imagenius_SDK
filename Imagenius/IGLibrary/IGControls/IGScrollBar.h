#pragma once

#include "../IGLibrary.h"

#define IGSCROLLBAR_WIDTH					12

namespace IGLibrary
{

class IGScrollBar
{
// Construction and creation
public:
	IGScrollBar();
	virtual ~IGScrollBar();

	BOOL CreateFromWindow(DWORD dwStyle,
						  HWND pParentWnd,
						  HWND hStatic,
						  UINT nId);
	BOOL CreateFromRect(DWORD dwStyle,
						HWND pParentWnd,
						RECT& rect,
						UINT nId);
	void Move (RECT& rect);

// Attributes
public:
	IGScrollBar&	EnableChannelColor(BOOL bEnable) 
					{ m_bChannelColor = bEnable; return *this; }
	IGScrollBar&	EnableThumbColor(BOOL bEnable);
	IGScrollBar&	EnableThumbGripper(BOOL bGripper) 
					{ m_bThumbGripper = bGripper; return *this; }
	int				GetScrollPos() const 
					{ return m_nPos; }
	void			GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const
					{ *lpMinPos = m_nMinPos; *lpMaxPos = m_nMaxPos; }
	IGScrollBar&	SetArrowHotColor(COLORREF cr)	
					{ m_ArrowHotColor = cr; return *this; }
	int				SetScrollPos(int nPos, BOOL bRedraw = TRUE);
	IGScrollBar&	SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw = TRUE);
	IGScrollBar&	SetThumbColor(COLORREF rgb);
	IGScrollBar&	SetThumbHoverColor(COLORREF rgb) 
					{ m_ThumbHoverColor = rgb; return *this; }
	void			Show (int nCmdShow);
	void			UpdateScrollPos (int nPos);
	HWND			GetHWND()
	{	return m_hWnd;}
	BOOL			IsVisible();

// Operations
	void	ScrollLeft();
	void	ScrollRight();
	void	ScrollUp();
	void	ScrollDown();

protected:
	void OnMouseWheel(int nDelta);
	void OnLButtonDown(UINT nFlags, LPPOINT point);
	void OnLButtonUp(UINT nFlags, LPPOINT point);
	void OnMouseMove(UINT nFlags, LPPOINT point);
	void OnTimer(UINT nIDEvent);
    BOOL OnSetCursor(HWND pWnd, UINT nHitTest, UINT message);
	void OnLButtonDblClk(UINT nFlags, LPPOINT point);
	BOOL OnEraseBkgnd(HDC hdc);
	void OnSize(UINT nType, int cx, int cy);
	void OnSetFocus(HWND pOldWnd);
	void OnKillFocus(HWND pNewWnd);	

private:
	void updateScrollBar ();

	LRESULT WindowProc (HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK WndProc (HWND window, UINT message, WPARAM wParam, LPARAM lParam);

	static LRESULT APIENTRY StaticHookScrolledWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookScrolledWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	BOOL initialize (int nXPos, int nYPos, int nWidth, int nHeight);

	static LPCWSTR s_windowClassName;

	HWND		m_hWnd;
	HWND		m_hParent;				// control parent 
	int			m_nThumbLeft;			// left margin of thumb
	int			m_nThumbTop;			// top margin of thumb
	int			m_nBitmapHeight;		// height of arrow and thumb bitmaps
	int			m_nBitmapWidth;			// width of arrow and thumb bitmaps
	int			m_nPos;					// current thumb position in scroll units
	int			m_nMinPos;				// minimum scrolling position
	int			m_nMaxPos;				// maximum scrolling position
	int			m_nRange;				// absolute value of max - min pos
	RECT		m_rectThumb;			// current rect for thumb
	RECT		m_rectClient;			// control client rect
	RECT		m_rectLeftArrow;		// left arrow rect
	RECT		m_rectRightArrow;		// right arrow rect
	RECT		m_rectUpArrow;			// up arrow rect
	RECT		m_rectDownArrow;		// down arrow rect
	HBITMAP		m_bmpThumb;				// bitmap for thumb
	HBITMAP		m_bmpThumbHot;			// bitmap for hot thumb
	HBITMAP		m_bmpUpLeftArrow;		// bitmap for up/left arrow
	HBITMAP		m_bmpDownRightArrow;	// bitmap for down/right arrow
    HCURSOR		m_hCursor;				// hand cursor
	COLORREF	m_ThumbColor;			// thumb color
	COLORREF	m_ThumbHoverColor;		// thumb color when mouse hovers
	COLORREF	m_ArrowHotColor;		// hover color
	BOOL		m_bThumbColor;			// TRUE = display thumb with color
	BOOL		m_bChannelColor;		// TRUE = display color in channel
	BOOL		m_bThumbGripper;		// TRUE = display thumb gripper
	BOOL		m_bThumbHover;			// TRUE = mouse is over thumb
	BOOL		m_bMouseOverArrowRight;	// TRUE = mouse over right arrow
	BOOL		m_bMouseOverArrowLeft;	// TRUE = mouse over left arrow
	BOOL		m_bMouseOverArrowUp;	// TRUE = mouse over up arrow
	BOOL		m_bMouseOverArrowDown;	// TRUE = mouse over down arrow
	BOOL		m_bMouseDownArrowRight;	// TRUE = mouse over right arrow & left button down
	BOOL		m_bMouseDownArrowLeft;	// TRUE = mouse over left arrow & left button down
	BOOL		m_bMouseDownArrowUp;	// TRUE = mouse over up arrow & left button down
	BOOL		m_bMouseDownArrowDown;	// TRUE = mouse over down arrow & left button down
	BOOL		m_bMouseDown;			// TRUE = mouse over thumb & left button down
	BOOL		m_bDragging;			// TRUE = thumb is being dragged
	BOOL		m_bHorizontal;			// TRUE = horizontal scroll bar
	BOOL		m_bThemed;				// TRUE = use visual themes
	int			m_nThumbHeight;
	int			m_nIconBorderHeight;

	void		ColorThumb(HDC hdc, COLORREF rgbThumb);
	void		Draw(HDC hdc = NULL);
	void		DrawHorizontal(HDC hdc);
	void		DrawVertical(HDC hdc);
	void		GetChannelColors(COLORREF& rgb1, COLORREF& rgb2, COLORREF& rgb3);
	void		LimitThumbPosition();
	void		LoadThumbBitmap();
	void		SetDefaultCursor();
	int			SetPositionFromThumb (bool bUpdate = true);
	void		UpdateThumbPosition();
};

}	// namespace IGLibrary