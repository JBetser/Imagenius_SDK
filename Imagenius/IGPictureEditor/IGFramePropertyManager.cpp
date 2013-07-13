// IGFramePropertyManager.cpp : Implementation of CIGFramePropertyManager
#include "stdafx.h"
#include "IGFramePropertyManager.h"

#include <gdiplus.h>

#define IGPROPERTYMANAGER_ITEMHEIGHT				15
#define IGPROPERTYMANAGER_COLOR_BACKGROUND			Color(255, 195, 207, 200)	// Green
#define IGPROPERTYMANAGER_COLOR_BORDER				Color(255, 76, 85, 118)	// Black
#define IGPROPERTYMANAGER_COLOR_FONT				Color(255, 34, 98, 141)	// Blue

using namespace IGLibrary;
using namespace Gdiplus;

// CIGFramePropertyManager
CIGFramePropertyManager::CIGFramePropertyManager()	: m_spScrollBar (new IGScrollBar ())
													, m_hListBox (NULL)
{
	ZeroMemory (&m_rcWnd, sizeof (RECT));
	m_bWindowOnly = TRUE;
}

CIGFramePropertyManager::~CIGFramePropertyManager()
{
}

STDMETHODIMP  CIGFramePropertyManager::CreateFrame (OLE_HANDLE hParent, IDispatch *pDispDockPanel)
{
	::GetWindowRect ((HWND)hParent, &m_rcWnd);
	m_rcWnd.right -= m_rcWnd.left;
	m_rcWnd.bottom -= m_rcWnd.top;
	m_rcWnd.left = m_rcWnd.top = 0;

	if (!Create ((HWND)hParent, _U_RECT (&m_rcWnd), L"History", WS_CHILD | WS_VISIBLE))
		return E_FAIL;

	m_rcWnd.right -= 2; m_rcWnd.bottom -= 2;
	m_hListBox = ::CreateWindowW (L"LISTBOX", L"Steps", WS_CHILD | WS_VISIBLE |
		LBS_OWNERDRAWFIXED | LBS_HASSTRINGS | LBS_NOREDRAW |
		LBS_NOINTEGRALHEIGHT,
		1, 1, m_rcWnd.right, m_rcWnd.bottom,
		(HWND)m_hWnd, NULL, GetInstance(), NULL);
	::SetWindowSubclass (m_hListBox, StaticHookListBoxProc, 0, (DWORD_PTR)this);
	
	// create list scrollbar
	m_spScrollBar->CreateFromRect (SBS_VERT, m_hListBox, m_rcWnd, 0); 
	m_spScrollBar->EnableThumbGripper (TRUE);
	m_spScrollBar->Show (SW_HIDE);

	// Connect workspace to DockPanel events
	if (pDispDockPanel)
		IDispEventSimpleImpl <1, CIGFramePropertyManager, &__uuidof(_IIGDockPanelEvents)>::DispEventAdvise (pDispDockPanel);

	return S_OK;
}

STDMETHODIMP  CIGFramePropertyManager::DestroyFrame()
{
	::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
	::DestroyWindow (m_hListBox);
	if (m_hWnd)
		DestroyWindow();
	return S_OK;
}

LRESULT CIGFramePropertyManager::OnMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WINDOWPOS *pWinPos = (WINDOWPOS *)lParam;
	m_rcWnd.left = 0; m_rcWnd.top = 0;	
	m_rcWnd.right = pWinPos->cx - 2;
	m_rcWnd.bottom = pWinPos->cy - 2;
	if (m_spScrollBar)
		m_spScrollBar->Move (m_rcWnd);
	::SetWindowPos (m_hListBox, HWND_TOP, 1, 1, m_rcWnd.right, m_rcWnd.bottom, SWP_FRAMECHANGED);
	bHandled = TRUE;
	return 0L;
}

void CIGFramePropertyManager::OnPanelOpening ()
{
	if (!m_spFrameMgr->GetSelectedFrame (m_spFrame))
	{
		m_spFrame.reset();	
		return;
	}
	populateListBox();	
	::InvalidateRect (m_hListBox, NULL, FALSE);
}

void CIGFramePropertyManager::OnPanelOpened ()
{
}

void CIGFramePropertyManager::OnPanelClosing ()
{	
	m_spFrame.reset();
	::SendMessageW (m_hListBox, LB_RESETCONTENT, 0, 0); 
	::InvalidateRect (m_hListBox, NULL, FALSE);
}

void CIGFramePropertyManager::populateListBox()
{
	if (!m_hListBox || !m_spFrame)
		return;	
	int nNbItems = m_spFrame->GetNbProperties();
	while (nNbItems--)
		::SendMessage (m_hListBox, LB_INSERTSTRING, -1, (LPARAM)(L"")); 
}

LRESULT CIGFramePropertyManager::OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rcWnd;
	GetWindowRect (&rcWnd);
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint (m_hWnd, &ps);
	Graphics graphics (hDC);	
	CIGPropertyManager::DrawBackground (rcWnd, graphics);
	::EndPaint (m_hWnd, &ps);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGFramePropertyManager::OnPaintList (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hListBox)
		return 0L;
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint (m_hListBox, &ps);
	Graphics graphics (hDC);
	if (m_spFrame)
		CIGPropertyManager::DrawProperties (*m_spFrame, m_hListBox,  m_spScrollBar->IsVisible() ? IGSCROLLBAR_WIDTH : 0, graphics);
	::EndPaint (m_hListBox, &ps);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGFramePropertyManager::OnEraseListBackground (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 1;
}

LRESULT CIGFramePropertyManager::OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	if (lpMeasureItem)
	{
		lpMeasureItem->itemHeight = IGPROPERTYMANAGER_ITEMHEIGHT;
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;	
}

LRESULT CIGFramePropertyManager::OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hListBox)
		return 0L;	
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
	if (lpDrawItem)
	{
		Graphics graphics (lpDrawItem->hDC);
		CIGPropertyManager::DrawProperty (*m_spFrame, m_hListBox, m_spScrollBar->IsVisible() ? IGSCROLLBAR_WIDTH : 0, lpDrawItem->itemID, graphics);
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGFramePropertyManager::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGFramePropertyManager* pThis = (CIGFramePropertyManager*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);;
}

//------------------------------------------------------------------------------
LRESULT CIGFramePropertyManager::HookListBoxProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;
	BOOL bHandled = false;
	MESSAGE_HANDLER(WM_PAINT, OnPaintList)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseListBackground)
	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}