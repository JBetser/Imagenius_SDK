// IGToolBox.cpp : Implementation of CIGToolBox

#include "stdafx.h"
#include "IGToolBox.h"
#include "IGDockPanel.h"

#define IGTOOLBOX_TOOLSIZE		25
#define IGTOOLBOX_CLASSNAME		L"IGToolBox"

using namespace IGLibrary;

// CIGToolBox
CIGToolBox::CIGToolBox()	: m_nRowSize (0)
							, m_nNbToolsByRow (0)
							, m_hWnd (NULL)
							, m_hSubclassedFrame (NULL)
							, m_pSelectedFrame (NULL)
{
}

STDMETHODIMP CIGToolBox::Create (OLE_HANDLE hOwner, LPRECT p_rc)
{
	// create toolbox window
	WNDCLASSEX wndClass;
    ZeroMemory (&wndClass, sizeof(wndClass));
    wndClass.cbSize        = sizeof(wndClass);
	wndClass.lpfnWndProc   = &CIGToolBox::StaticToolBoxProc;
    wndClass.hInstance     = getInstance();
	wndClass.hCursor       = ::LoadCursor (0, IDC_ARROW);  
	wndClass.hbrBackground = ::CreateSolidBrush (IGDOCKPANEL_COLORRGB_BACKGROUND);
    wndClass.lpszClassName = IGTOOLBOX_CLASSNAME;
	::RegisterClassEx (&wndClass);
	m_hWnd = ::CreateWindowExW (WS_EX_LEFT, IGTOOLBOX_CLASSNAME, L"",
									WS_CHILD | WS_VISIBLE,
									0, 0,
									p_rc->right - p_rc->left, 
									p_rc->bottom - p_rc->top,
									(HWND)hOwner, NULL, getInstance(), (LPVOID)this);
	m_nRowSize = (p_rc->right - p_rc->left);
	m_nNbToolsByRow = m_nRowSize / IGTOOLBOX_TOOLSIZE;

	// instanciate tools
	int nInterstice = (m_nRowSize - m_nNbToolsByRow * IGTOOLBOX_TOOLSIZE) / 3;
	RECT rcTool;
	ZeroMemory (&rcTool, sizeof (RECT));	
	rcTool.top = nInterstice;
	rcTool.left = nInterstice;
	rcTool.right = nInterstice + IGTOOLBOX_TOOLSIZE;
	rcTool.bottom = nInterstice + IGTOOLBOX_TOOLSIZE;
	IGSmartPtr <IGTool> spTool (new IGToolSelector (m_hWnd, CComQIPtr <IIGToolBox> (this), rcTool));
	spTool->SetSelected();
	m_spCurrentTool = spTool;
	m_lspTools.push_back (spTool);
	rcTool.left += nInterstice + IGTOOLBOX_TOOLSIZE;
	rcTool.right += nInterstice + IGTOOLBOX_TOOLSIZE;
	spTool.reset (new IGToolCutterSquare (m_hWnd, CComQIPtr <IIGToolBox> (this), rcTool));
	m_lspTools.push_back (spTool);
	rcTool.left -= nInterstice + IGTOOLBOX_TOOLSIZE;
	rcTool.right -= nInterstice + IGTOOLBOX_TOOLSIZE;
	rcTool.top += nInterstice + IGTOOLBOX_TOOLSIZE;
	rcTool.bottom += nInterstice + IGTOOLBOX_TOOLSIZE;
	spTool.reset (new IGToolPencil (m_hWnd, CComQIPtr <IIGToolBox> (this), rcTool));
	m_lspTools.push_back (spTool);
	rcTool.left += nInterstice + IGTOOLBOX_TOOLSIZE;
	rcTool.right += nInterstice + IGTOOLBOX_TOOLSIZE;
	spTool.reset (new IGToolRubber (m_hWnd, CComQIPtr <IIGToolBox> (this), rcTool));
	m_lspTools.push_back (spTool);
	return S_OK;
}

STDMETHODIMP CIGToolBox::Destroy()
{
	if (m_hSubclassedFrame)
		::RemoveWindowSubclass (m_hSubclassedFrame, StaticHookFrameProc, 0);
	::UnregisterClassW (IGTOOLBOX_CLASSNAME, getInstance());
	return S_OK;
}

STDMETHODIMP CIGToolBox::put_Frame (IDispatch *pDispFrame)
{
	if (!pDispFrame)
		return E_INVALIDARG;
	m_pSelectedFrame = CComQIPtr <IIGFrame> (pDispFrame);
	if (!m_pSelectedFrame)
		return E_INVALIDARG;
	CComQIPtr <IOleWindow> spWin (pDispFrame);
	if (!spWin)
		return E_INVALIDARG;
	HWND hFrame = NULL;
	if (FAILED (spWin->GetWindow (&hFrame)))
		return E_INVALIDARG;
	if (!hFrame)
		return E_INVALIDARG;
	if (m_hSubclassedFrame)
		::RemoveWindowSubclass (m_hSubclassedFrame, StaticHookFrameProc, 0);
	m_hSubclassedFrame = hFrame;
	m_spCurrentTool->SetSelected();
	return ::SetWindowSubclass (m_hSubclassedFrame, StaticHookFrameProc, 0, (DWORD_PTR)this) ? S_OK : E_FAIL;
}

STDMETHODIMP CIGToolBox::get_Frame (IDispatch **pDispFrame)
{
	*pDispFrame = m_pSelectedFrame;
	return S_OK;
}

STDMETHODIMP CIGToolBox::put_SelectedToolId (LONG nToolId)
{
	if (nToolId < 0 || nToolId >= (int)m_lspTools.size())
		return E_INVALIDARG;
	std::list <IGLibrary::IGSmartPtr <IGLibrary::IGTool>>::const_iterator itItem = m_lspTools.begin();
	for (int idxItem = 0;
			 (itItem != m_lspTools.end()) && (idxItem < nToolId); idxItem++)
		 ++itItem;
	m_spCurrentTool->SetSelected (false);
	m_spCurrentTool = (*itItem);
	m_spCurrentTool->SetSelected();
	return S_OK;
}

STDMETHODIMP CIGToolBox::get_SelectedToolId (LONG *p_nToolId)
{
	if (!p_nToolId)
		return E_INVALIDARG;
	if (!m_spCurrentTool)
		return E_FAIL;
	*p_nToolId = m_spCurrentTool->GetId();
	return S_OK;
}

LRESULT CIGToolBox::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}

LRESULT CIGToolBox::OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}

LRESULT CIGToolBox::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}

//------------------------------------------------------------------------------
//static
LRESULT CALLBACK CIGToolBox::StaticToolBoxProc (HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
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
			CIGToolBox *pThis = (CIGToolBox *)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);
			if (pThis)
				pThis->ToolBoxProc (hWnd, nMsg, wParam, lParam);
		}
		break;
	}
	return ::DefWindowProcW (hWnd, nMsg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGToolBox::ToolBoxProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_COMMAND)
	{
		for (std::list <IGLibrary::IGSmartPtr <IGLibrary::IGTool>>::const_iterator itItem = m_lspTools.begin();
			 itItem != m_lspTools.end(); ++itItem)
		{
			if ((*itItem)->IsSelected())
			{
				if ((*itItem)->GetId() == LOWORD(wParam))
				{
					m_spCurrentTool = (*itItem);
					m_spCurrentTool->SetSelected();
				}
				else
				{
					(*itItem)->SetSelected (false);
				}

			}
		}
		::InvalidateRect (hWnd, NULL, FALSE);
	}
	else if (uMsg == UM_CUSTOMMOVE)
	{
		RECT rcDockPanelWnd;
		::GetWindowRect (::GetParent (m_hWnd), &rcDockPanelWnd);
		RECT rcWnd;
		::GetWindowRect (m_hWnd, &rcWnd);
		::MoveWindow (m_hWnd, rcDockPanelWnd.right - rcDockPanelWnd.left - m_nRowSize - IGDOCKPANEL_DOCKEDSIZE - 1, 0,
						m_nRowSize, rcDockPanelWnd.bottom - rcDockPanelWnd.top, TRUE);
	}
	return 0L;
}

LRESULT APIENTRY CIGToolBox::StaticHookFrameProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
														UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGToolBox* pThis = (CIGToolBox*)dwRefData;
	if( pThis != NULL )
		return pThis->HookFrameProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGToolBox::HookFrameProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = FALSE;

	HWND hWndSel = NULL;
	CComQIPtr <IOleWindow> spWin (m_pSelectedFrame);
	if (spWin)
		spWin->GetWindow (&hWndSel);

	if (m_spCurrentTool && (	(msg == WM_LBUTTONDOWN) ||
								(msg == WM_MOUSEMOVE) ||
								(msg == WM_LBUTTONUP) ||
								(msg == WM_KEYDOWN) ||
								(msg == WM_KEYUP) ) && (hWndSel == hWnd))
	{
		lRes = m_spCurrentTool->ProcessMsg (msg, wParam, lParam);
		if (lRes)
			return lRes;
	}
	return ::DefSubclassProc (hWnd, msg, wParam, lParam);
}