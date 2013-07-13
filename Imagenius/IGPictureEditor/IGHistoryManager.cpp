// IGHistoryManager.cpp : Implementation of CIGHistoryManager
#include "stdafx.h"
#include "IGHistoryManager.h"

#include <gdiplus.h>

#define IGHISTORYMANAGER_ITEMHEIGHT					15
#define IGHISTORYMANAGER_COLOR_BACKGROUND			Color(255, 195, 207, 200)	// Green
#define IGHISTORYMANAGER_COLOR_BORDER				Color(255, 76, 85, 118)		// Black

using namespace IGLibrary;
using namespace Gdiplus;

// CIGHistoryManager
CIGHistoryManager::CIGHistoryManager()	: m_spScrollBar (new IGScrollBar ())
										, m_hListBox (NULL)
{
	ZeroMemory (&m_rcWnd, sizeof (RECT));
	m_bWindowOnly = TRUE;
}

CIGHistoryManager::~CIGHistoryManager()
{
}

STDMETHODIMP  CIGHistoryManager::CreateFrame (OLE_HANDLE hParent, IDispatch *pDispDockPanel)
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
	
	// create the layer list scrollbar
	m_spScrollBar->CreateFromRect (SBS_VERT, m_hListBox, m_rcWnd, 0); 
	m_spScrollBar->EnableThumbGripper (TRUE);
	m_spScrollBar->Show (SW_HIDE);

	// Connect workspace to DockPanel events
	if (pDispDockPanel)
		IDispEventSimpleImpl <1, CIGHistoryManager, &__uuidof(_IIGDockPanelEvents)>::DispEventAdvise (pDispDockPanel);

	return S_OK;
}

STDMETHODIMP  CIGHistoryManager::DestroyFrame()
{
	::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
	::DestroyWindow (m_hListBox);
	if (m_hWnd)
		DestroyWindow();
	return S_OK;
}

LRESULT CIGHistoryManager::OnMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

void CIGHistoryManager::OnPanelOpening ()
{
	if (!m_spFrameMgr->GetSelectedFrame (m_spFrame))
	{
		m_spFrame.reset();	
		return;
	}
	populateListBox();	
	::InvalidateRect (m_hListBox, NULL, FALSE);
}

void CIGHistoryManager::OnPanelOpened ()
{
}

void CIGHistoryManager::OnPanelClosing ()
{	
	m_spFrame.reset();
	::SendMessageW (m_hListBox, LB_RESETCONTENT, 0, 0); 
}

void CIGHistoryManager::populateListBox()
{
	if (!m_spFrame || !m_hListBox)
		return;	
	m_spFrame->PopulateListBox (m_hListBox);
}

LRESULT CIGHistoryManager::OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hListBox)
		return 0L;
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint (m_hListBox, &ps);
	if (m_spFrame)
		m_spFrame->HistoryDraw (m_hListBox, m_spScrollBar->IsVisible() ? IGSCROLLBAR_WIDTH : 0, hDC);
	::EndPaint (m_hListBox, &ps);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGHistoryManager::OnBackgroundPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rcWnd;
	GetWindowRect (&rcWnd);
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint (m_hWnd, &ps);
	Graphics graphics (hDC);	
	static SolidBrush backgroundBrush (IGHISTORYMANAGER_COLOR_BACKGROUND);
	Rect rectBackgroundItem (0, 0,
							rcWnd.right - rcWnd.left - 1, rcWnd.bottom - rcWnd.top - 1);
	graphics.FillRectangle (&backgroundBrush, rectBackgroundItem);
	static Pen penBorder (IGHISTORYMANAGER_COLOR_BORDER);
	graphics.DrawRectangle (&penBorder, rectBackgroundItem);
	::EndPaint (m_hWnd, &ps);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGHistoryManager::OnEraseListBackground (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 1;
}

LRESULT CIGHistoryManager::OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	if (lpMeasureItem)
	{
		lpMeasureItem->itemHeight = IGHISTORYMANAGER_ITEMHEIGHT;
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;	
}

LRESULT CIGHistoryManager::OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_spFrame || !m_hListBox)
		return 0L;	
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
	if (lpDrawItem)
	{
		m_spFrame->HistoryDraw (m_hListBox, m_spScrollBar->IsVisible() ? IGSCROLLBAR_WIDTH : 0, lpDrawItem->hDC, lpDrawItem->itemID);
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;
}

LRESULT CIGHistoryManager::OnSelChanged (WPARAM wParam, LPARAM lParam, HWND hWnd, BOOL& bHandled)
{
	m_spFrame->RestoreStep (::SendMessageW (m_hListBox, LB_GETCOUNT, 0, 0) - ::SendMessageW (m_hListBox, LB_GETCURSEL, 0, 0) - 1);
	m_spFrame->GetWorkingLayer()->SelectionRebuildBox();
	bHandled = FALSE;
	return 1L;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGHistoryManager::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGHistoryManager* pThis = (CIGHistoryManager*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);;
}

//------------------------------------------------------------------------------
LRESULT CIGHistoryManager::HookListBoxProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;
	BOOL bHandled = false;
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseListBackground)
	lResult = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
	if (uMsg == WM_LBUTTONUP)
	{
		int nLastStep = ::SendMessageW (m_hListBox, LB_GETCOUNT, 0, 0) - 1;
		if (nLastStep < 0)
			return 0L;
		int nCurStep = ::SendMessageW (m_hListBox, LB_GETCURSEL, 0, 0);
		if (nCurStep > nLastStep)
			return 0L;
		m_spFrame->RestoreStep (nLastStep - nCurStep);
		m_spFrame->GetWorkingLayer()->SelectionRebuildBox();
	}
	return lResult;
}