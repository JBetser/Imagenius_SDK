// IGPropertyManager.cpp : Implementation of CIGPropertyManager
#include "stdafx.h"
#include "IGPropertyManager.h"

#define IGPROPERTYMANAGER_ITEMHEIGHT				15
#define IGPROPERTYMANAGER_COLOR_BACKGROUND			Color(255, 195, 207, 200)	// Green
#define IGPROPERTYMANAGER_COLOR_BORDER				Color(255, 76, 85, 118)		// Black
#define IGPROPERTYMANAGER_COLOR_FONT				Color(255, 34, 98, 141)	// Blue

using namespace IGLibrary;
using namespace Gdiplus;

// CIGPropertyManager
CIGPropertyManager::CIGPropertyManager()	: m_spScrollBar (new IGScrollBar ())
{
	ZeroMemory (&m_rcWnd, sizeof (RECT));
	m_bWindowOnly = TRUE;
}

CIGPropertyManager::~CIGPropertyManager()
{
}

STDMETHODIMP  CIGPropertyManager::CreateFrame (OLE_HANDLE hParent, IDispatch *pDispDockPanel)
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

	// create the list scrollbar
	m_spScrollBar->CreateFromRect (SBS_VERT, m_hListBox, m_rcWnd, 0); 
	m_spScrollBar->EnableThumbGripper (TRUE);
	m_spScrollBar->Show (SW_HIDE);

	// Connect workspace to DockPanel events
	if (pDispDockPanel)
		IDispEventSimpleImpl <1, CIGPropertyManager, &__uuidof(_IIGDockPanelEvents)>::DispEventAdvise (pDispDockPanel);

	return S_OK;
}

STDMETHODIMP  CIGPropertyManager::DestroyFrame()
{
	::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
	::DestroyWindow (m_hListBox);
	if (m_hWnd)
		DestroyWindow();
	return S_OK;
}

LRESULT CIGPropertyManager::OnMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

void CIGPropertyManager::OnPanelOpening ()
{
	populateListBox();	
	::InvalidateRect (m_hListBox, NULL, FALSE);
}

void CIGPropertyManager::OnPanelOpened ()
{
}

void CIGPropertyManager::OnPanelClosing ()
{	
	::SendMessageW (m_hListBox, LB_RESETCONTENT, 0, 0); 
	::InvalidateRect (m_hListBox, NULL, FALSE);
}

void CIGPropertyManager::populateListBox()
{
	if (!m_hListBox || !m_spFrameMgr)
		return;	
	int nNbItems = m_spFrameMgr->GetNbProperties();
	while (nNbItems--)
		::SendMessage (m_hListBox, LB_INSERTSTRING, -1, (LPARAM)(L"")); 
}

void CIGPropertyManager::DrawBackground (const RECT& rcWnd, Graphics& graphics)
{
	static SolidBrush backgroundBrush (IGPROPERTYMANAGER_COLOR_BACKGROUND);
	Rect rectBackgroundItem (0, 0,
		rcWnd.right - rcWnd.left - 1, rcWnd.bottom - rcWnd.top - 1);
	graphics.FillRectangle (&backgroundBrush, rectBackgroundItem);
	Pen penBorder (IGPROPERTYMANAGER_COLOR_BORDER);
	graphics.DrawRectangle (&penBorder, rectBackgroundItem);
}

void CIGPropertyManager::DrawProperty (const pair<wstring,wstring>& prop, const RectF& rcfNameProp, const RectF& rcfValueProp, Graphics& graphics)
{
	static FontFamily fontFamily (L"Times New Roman");
	static Font fontRegular (&fontFamily, 12, FontStyleRegular, UnitPixel);
	static Font fontItalic (&fontFamily, 12, FontStyleItalic, UnitPixel);
	static SolidBrush backgroundBrush (IGPROPERTYMANAGER_COLOR_BACKGROUND);
	static SolidBrush  solidFontBrush (IGPROPERTYMANAGER_COLOR_FONT);
	static StringFormat	format (StringFormat::GenericDefault());
	graphics.FillRectangle (&backgroundBrush, Rect (Point ((int)rcfNameProp.X, (int)rcfNameProp.Y), Size ((int)(rcfNameProp.Width + rcfValueProp.Width), (int)rcfNameProp.Height)));
	graphics.DrawString (prop.first.c_str(), -1, &fontItalic, rcfNameProp, &format, &solidFontBrush);
	graphics.DrawString (prop.second.c_str(), -1, &fontRegular, rcfValueProp, &format, &solidFontBrush);
}

void CIGPropertyManager::DrawProperty (const IGLibrary::IGHasProperties& hasPropObj, HWND hListBox, int nOffsetWidth, int nListboxItemId, Graphics& graphics)
{
	vector <pair<wstring,wstring>> vProp = hasPropObj.GetPropertiesAndValues();
	pair<wstring,wstring> curProp;
	int idxItem = 0;
	for(vector <pair<wstring,wstring>>::const_iterator it = vProp.begin(); it != vProp.end(); ++it, idxItem++){
		if (idxItem == nListboxItemId)
		{
			curProp = *it;
			break;
		}
	}
	RECT rcItem;
	::ZeroMemory (&rcItem, sizeof (RECT));
	::SendMessageW (hListBox, (UINT)LB_GETITEMRECT, (WPARAM)nListboxItemId, (LPARAM)&rcItem);
	RectF rcfNameProp ((float)rcItem.left + 5.0f, (float)rcItem.top, (float)(rcItem.right - rcItem.left) / 2.0f - 5.0f, (float)(rcItem.bottom - rcItem.top));
	RectF rcfValueProp (rcfNameProp);
	rcfValueProp.X = rcfNameProp.GetRight() + 5.0f;
	rcfValueProp.Width -= nOffsetWidth + 5.0f;
	DrawProperty (curProp, rcfNameProp, rcfValueProp, graphics);
}

void CIGPropertyManager::DrawProperties (const IGHasProperties& hasPropObj, HWND hListBox, int nOffsetWidth, Graphics& graphics)
{
	for (int nNumProp = 0; nNumProp < (int)::SendMessageW (hListBox, LB_GETCOUNT, 0, 0); nNumProp++)
		DrawProperty (hasPropObj, hListBox, nOffsetWidth, nNumProp, graphics);
}

LRESULT CIGPropertyManager::OnPaint (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rcWnd;
	GetWindowRect (&rcWnd);
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint (m_hWnd, &ps);
	Graphics graphics (hDC);
	DrawBackground (rcWnd, graphics);
	::EndPaint (m_hWnd, &ps);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGPropertyManager::OnPaintList (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hListBox)
		return 0L;
	RECT rcList;
	::GetWindowRect (m_hListBox, &rcList);
	PAINTSTRUCT ps;
	HDC hDC = ::BeginPaint (m_hListBox, &ps);
	Graphics graphics (hDC);
	DrawProperties (*m_spFrameMgr, m_hListBox,  m_spScrollBar->IsVisible() ? IGSCROLLBAR_WIDTH : 0, graphics);
	::EndPaint (m_hListBox, &ps);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGPropertyManager::OnEraseListBackground (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 1;
}

LRESULT CIGPropertyManager::OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CIGPropertyManager::OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_hListBox)
		return 0L;	
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
	if (lpDrawItem)
	{
		Graphics graphics (lpDrawItem->hDC);
		DrawProperty (*m_spFrameMgr, m_hListBox, m_spScrollBar->IsVisible() ? IGSCROLLBAR_WIDTH : 0, lpDrawItem->itemID, graphics);
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGPropertyManager::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGPropertyManager* pThis = (CIGPropertyManager*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);;
}

//------------------------------------------------------------------------------
LRESULT CIGPropertyManager::HookListBoxProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;
	BOOL bHandled = false;
	MESSAGE_HANDLER(WM_PAINT, OnPaintList)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseListBackground)
	return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}