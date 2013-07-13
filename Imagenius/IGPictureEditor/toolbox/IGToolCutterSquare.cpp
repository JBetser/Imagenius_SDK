#include "stdafx.h"
#include "IGTool.h"
#include <IGFrame.h>

#include <atlsafe.h>

using namespace IGLibrary;

HCURSOR IGToolCutterSquare::g_hCursorCutter = NULL;
HCURSOR IGToolCutterSquare::g_hCursorCutterPlus = NULL;
HCURSOR IGToolCutterSquare::g_hCursorCutterMinus = NULL;
int IGToolCutterSquare::g_nNbRefs = 0;

IGToolCutterSquare::IGToolCutterSquare (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton) : IGTool (hOwner, spToolBox, IGTOOLID_CUTTERSQUARE, rcButton, IDB_SELECTORSQUARE)
										, m_xPos1 (0)
										, m_yPos1 (0)
{
	if (g_nNbRefs++ == 0)
	{
		g_hCursorCutter = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_CUTTER), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
		g_hCursorCutterPlus = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_CUTTERPLUS), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
		g_hCursorCutterMinus = (HCURSOR)::LoadImageW (GetInstance(), MAKEINTRESOURCEW(IDC_CUTTERMINUS), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
	}
}

IGToolCutterSquare::~IGToolCutterSquare()
{
	if (--g_nNbRefs == 0)
	{
		::DestroyCursor (g_hCursorCutter);
		::DestroyCursor (g_hCursorCutterPlus);
		::DestroyCursor (g_hCursorCutterMinus);
	}
}

void IGToolCutterSquare::SetSelected (bool bSelected)
{
	IGTool::SetSelected (bSelected);
	CComQIPtr <IOleWindow> spWin (m_pFrame);
	if (!spWin)
		return;
	HWND hWnd;
	spWin->GetWindow (&hWnd);	
	if (bSelected)
		::SetClassLong (hWnd, GCL_HCURSOR, (LONG)g_hCursorCutter);
}

LRESULT IGToolCutterSquare::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComSafeArray <int> spSafeArrayPts;
	spSafeArrayPts.Add (m_xPos1);
	spSafeArrayPts.Add (m_yPos1);
	m_xPos1 = GET_X_LPARAM(lParam); 
	m_yPos1 = GET_Y_LPARAM(lParam); 
	m_pFrame->ConvertCoordFrameToPixel ((LONG*)&m_xPos1, (LONG*)&m_yPos1);
	spSafeArrayPts.Add (m_xPos1);
	spSafeArrayPts.Add (m_yPos1);
	CComVariant spVarArray (spSafeArrayPts);
	int nTypeSelection = IGSELECTION_REPLACE;
	if (::GetKeyState (VK_SHIFT) < 0)
		nTypeSelection = IGSELECTION_ADD;
	else if (::GetKeyState (VK_CONTROL) < 0)
		nTypeSelection = IGSELECTION_REMOVE;
	m_pFrame->ManageSelection (IGSELECTION_SQUARE | nTypeSelection, &spVarArray);
	return 0L;
}

LRESULT IGToolCutterSquare::OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_xPos1 = GET_X_LPARAM(lParam); 
	m_yPos1 = GET_Y_LPARAM(lParam); 
	m_pFrame->ConvertCoordFrameToPixel ((LONG*)&m_xPos1, (LONG*)&m_yPos1);
	return 0L;
}

LRESULT IGToolCutterSquare::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (IsMouseDown())
	{
	}
	else
	{
		bHandled = FALSE;
	}
	return 0L;
}

LRESULT IGToolCutterSquare::OnKeyDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComQIPtr <IOleWindow> spWin (m_pFrame);
	if (!spWin)
		return 0L;
	HWND hWnd;
	spWin->GetWindow (&hWnd);	
	if (::GetKeyState (VK_SHIFT) < 0)
		::SetClassLong (hWnd, GCL_HCURSOR, (LONG)g_hCursorCutterPlus);
	else if (::GetKeyState (VK_CONTROL) < 0)
		::SetClassLong (hWnd, GCL_HCURSOR, (LONG)g_hCursorCutterMinus);
	return 0L;
}

LRESULT IGToolCutterSquare::OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComQIPtr <IOleWindow> spWin (m_pFrame);
	if (!spWin)
		return 0L;
	HWND hWnd;
	spWin->GetWindow (&hWnd);	
	if (::GetKeyState (VK_SHIFT) >= 0 && ::GetKeyState (VK_CONTROL) >= 0)
		::SetClassLong (hWnd, GCL_HCURSOR, (LONG)g_hCursorCutter);
	return 0L;
}