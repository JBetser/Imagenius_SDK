#pragma once

#define MAXTABS		10

namespace IGLibrary
{
typedef enum {
	IGBUTTON_MOUSEOUT	=	1,
	IGBUTTON_MOUSEOVER	=	2,
	IGBUTTON_MOUSEDOWN	=	3	} IGBUTTONSTATE;

class IGTabBar
{
public:
	IGTabBar (HWND hParent, int nPosX, int nPosY, int nWidth, int nHeight, COLORREF cBackGround = 0x00FFFFFF);
	~IGTabBar ();

	void SetBkColor (COLORREF cBackGround);
	void Show (bool bShow = true);
	bool AddTab (HWND hTab, LPCWSTR lpcwTabTitle, UINT nDesiredSize = 0);
	void Move (int nHeight);
	void getHWND (HWND *phWnd);

protected:
	void OnPaint();

	LRESULT WndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	LRESULT WndTabProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT APIENTRY StaticWndTabProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	
private:
	void drawTab (HDC hdc, UINT nSize, bool bSelected, bool bOver, const wchar_t *pcwTitle);
	void redraw (bool bRedrawParent = false);
	void resizeTabs();
	void selectTabButton (HWND hWnd);

	int					m_nNbTabs;
	HWND				m_phTabs [MAXTABS];
	HWND				m_phTabBars [MAXTABS];
	HWND				m_phTabButtons [MAXTABS];
	UINT				m_pnDesiredSizes [MAXTABS];
	HWND				m_hWnd;
	HWND				m_hSelectedTab;
	int					m_nIdxSelectedTab;
	HWND				m_hOver;
	RECT				m_rcWnd;
	COLORREF			m_cBackGround;
	IGBUTTONSTATE		m_eButtonState;
};

inline void IGTabBar::getHWND (HWND *phWnd)
{
	*phWnd = m_hWnd;
}

}
