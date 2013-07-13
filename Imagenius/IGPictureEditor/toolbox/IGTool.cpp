#include "stdafx.h"

#include "IGTool.h"

using namespace IGLibrary;

IGTool::IGTool (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, int nToolId, RECT rcButton, int nIconId)	: m_nToolId (nToolId)
																										, m_bMouseDown (false)
																										, m_bToolDownStarted (false)
																										, m_spToolBox (spToolBox)
																										, m_pFrame (NULL)
{	
	m_spButton.CoCreateInstance (CLSID_IGSimpleButton);
	m_spButton->CreateFrame ((OLE_HANDLE)hOwner, nToolId, 0, &rcButton, MAKEINTRESOURCEW(nIconId), NULL, VARIANT_TRUE);
}

IGTool::~IGTool()
{
}

bool IGTool::IsSelected()
{
	VARIANT_BOOL bPushed;
	m_spButton->get_Pushed (&bPushed);
	return (bPushed == VARIANT_TRUE);
}

int IGTool::GetId()
{
	return m_nToolId;
}

void IGTool::SetSelected (bool bSelected)
{
	m_spButton->put_Pushed (bSelected ? VARIANT_TRUE : VARIANT_FALSE);
	IDispatch *pDispFrame = NULL;
	m_spToolBox->get_Frame (&pDispFrame);
	if (!pDispFrame)
		return;
	m_pFrame = CComQIPtr <IIGFrame> (pDispFrame);
	CComQIPtr <IOleWindow> spWin (m_pFrame);
	if (!spWin)
		return;
	HWND hWnd;
	spWin->GetWindow (&hWnd);
	if (bSelected)
		::SetClassLong (hWnd, GCL_HCURSOR, (LONG)IDC_ARROW);	
}

LRESULT IGTool::ProcessMsg (UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = FALSE;
	BOOL bHandled = TRUE;
	if (m_spToolBox)
	{		
		if (!m_pFrame)
			return 0L;
		CComQIPtr <IOleWindow> spWin (m_pFrame);
		if (!spWin)
			return 0L;
		HWND hWnd;
		spWin->GetWindow (&hWnd);

		if (uMsg == WM_LBUTTONDOWN)
		{
			if (!m_bMouseDown)
			{
				m_bMouseDown = true;
				::SetCapture (hWnd);
				OnMouseDown (uMsg, wParam, lParam, bHandled);
				m_bToolDownStarted = true;
				if (bHandled)
					return TRUE;
			}
		}
		else if (uMsg == WM_LBUTTONUP)
		{			
			if (m_bMouseDown)
			{
				m_bMouseDown = false;
				::ReleaseCapture();
			}
			if (m_bToolDownStarted)
			{
				OnMouseUp (uMsg, wParam, lParam, bHandled);
				m_bToolDownStarted = false;
				if (bHandled)
					return TRUE;
			}
		}
		else if (uMsg == WM_MOUSEMOVE)
		{
			OnMouseMove (uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		else if (uMsg == WM_KEYDOWN)
		{
			OnKeyDown (uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
		else if (uMsg == WM_KEYUP)
		{
			OnKeyUp (uMsg, wParam, lParam, bHandled);
			return FALSE;
		}
	}
	return lResult;
}

bool IGTool::IsMouseDown()
{
	return m_bMouseDown;
}

LRESULT IGTool::OnKeyDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;		
	return 0L;
}

LRESULT IGTool::OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;		
	return 0L;
}

IGToolSelector::IGToolSelector (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton) : IGTool (hOwner, spToolBox, IGTOOLID_SELECTOR, rcButton, IDB_ARROW)
{	
}

IGToolSelector::~IGToolSelector()
{
}

LRESULT IGToolSelector::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;		
	return 0L;
}

LRESULT IGToolSelector::OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

LRESULT IGToolSelector::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}