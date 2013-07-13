#include "stdafx.h"
#include "IGWorkspace.h"
#include <IGUserMessages.h>
#include <gdiplus.h>

using namespace Gdiplus;

#define IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE		13
#define IGWORKSPACEBUTTONPANEL_WRKSPACE_OFFSET_X	2
#define IGWORKSPACEBUTTONPANEL_WRKSPACE_OFFSET_Y	5//12
#define IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE	26
#define IGWORKSPACEBUTTONPANEL_ALPHANORMAL			70
#define IGWORKSPACEBUTTONPANEL_ALPHAOVER			255
#define IGWORKSPACEBUTTONPANEL_FORCEMOUSELEAVE_WPARAM	111

IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconClose = NULL;
IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconMaximize = NULL;
IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconUnMaximize = NULL;
IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconMinimize = NULL;
IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconSave = NULL;
IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconSaveAs = NULL;
IGLibrary::IGFrame *CIGWorkspaceButtonPanel::g_pCxIconUpload = NULL;
HWND CIGWorkspaceButtonPanel::g_hOverButton = NULL;
bool CIGWorkspaceButtonPanel::g_bChanged = false;

// CIGWorkspaceButtonPanel
CIGWorkspaceButtonPanel::CIGWorkspaceButtonPanel (HWND hBackGround, HWND hListBox, RECT *p_rcPanel
												  , IDispatch *pFrame
												  , IDispatch *pDispMultiFrame
												  , IDispatch *pDispDockPanel
												  , IDispatch *pWorkspace)
														: m_hBackGround (hBackGround)
														, m_hListBox (hListBox)
														, m_hOverButton (NULL)
														, m_eButtonState (IGBUTTON_MOUSEOUT)
														, m_hBackgroundDC (NULL)
														, m_hBackgroundBmp (NULL)
														, m_bMinimized (false)
														, m_bMaximized (false)
														, m_bButtonChanged (false)
														, m_pFrame ((IIGFrame*)pFrame)
														, m_pMultiFrame ((IIGMultiFrame*)pDispMultiFrame)
														, m_pDockPanel ((IIGDockPanel*)pDispDockPanel)
														, m_pWorkspace ((IIGWorkspace*)pWorkspace)
{
	// remember panel offsets for further drawing
	::memcpy_s (&m_rcPanel, sizeof (RECT), p_rcPanel, sizeof (RECT));
	::GetWindowRect (m_hListBox, &m_rcParent);

	VARIANT_BOOL bMinimized;
	m_pMultiFrame->get_isMinimized ((OLE_HANDLE)(IIGFrame *)m_pFrame, &bMinimized);
	m_bMinimized = (bMinimized == VARIANT_TRUE);

	VARIANT_BOOL bMaximized;
	m_pMultiFrame->get_isMaximized ((OLE_HANDLE)(IIGFrame *)m_pFrame, &bMaximized);
	m_bMaximized = (bMaximized == VARIANT_TRUE);

	// create the button icons
	createButtonIcon (g_pCxIconClose, MAKEINTRESOURCEW(IDB_CLOSE));
	createButtonIcon (g_pCxIconMinimize, MAKEINTRESOURCEW(IDB_MINIMIZE));
	createButtonIcon (g_pCxIconMaximize, MAKEINTRESOURCEW(IDB_MAXIMIZE));
	createButtonIcon (g_pCxIconUnMaximize, MAKEINTRESOURCEW(IDB_UNMAXIMIZE));
	createButtonIcon (g_pCxIconSave, MAKEINTRESOURCEW(IDB_SAVE));
	createButtonIcon (g_pCxIconSaveAs, MAKEINTRESOURCEW(IDB_SAVEAS));
	createButtonIcon (g_pCxIconUpload, MAKEINTRESOURCEW(IDB_UPLOAD));	

	// create the minimize, maximize and close buttons
	int nLeft = m_rcParent.left + p_rcPanel->right - 3 * IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE;
	int nTop = m_rcParent.top + p_rcPanel->top;
	m_hMinimizeWnd = createButtonWindow (nLeft, nTop, IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE);
	nLeft += IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE;
	m_hMaximizeWnd = createButtonWindow (nLeft, nTop, IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE);
	nLeft += IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE;
	m_hCloseWnd = createButtonWindow (nLeft, nTop, IGWORKSPACEBUTTONPANEL_WIN_BUTTONSIZE);

	// create the save, save as and upload buttons
	nLeft = m_rcParent.left + p_rcPanel->left + IGWORKSPACEBUTTONPANEL_WRKSPACE_OFFSET_X;
	nTop = m_rcParent.top + p_rcPanel->bottom - IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE - IGWORKSPACEBUTTONPANEL_WRKSPACE_OFFSET_Y;
	m_hSaveWnd = createButtonWindow (nLeft, nTop, IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE);
	nLeft += IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE;
	m_hSaveAsWnd = createButtonWindow (nLeft, nTop, IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE);
	nLeft += IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE;
	m_hUploadWnd = createButtonWindow (nLeft, nTop, IGWORKSPACEBUTTONPANEL_WRKSPACE_BUTTONSIZE);

	// subclass button window procs
	::SetWindowSubclass (m_hMinimizeWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	::SetWindowSubclass (m_hMaximizeWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	::SetWindowSubclass (m_hCloseWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	::SetWindowSubclass (m_hSaveWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	::SetWindowSubclass (m_hSaveAsWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	::SetWindowSubclass (m_hUploadWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
}

CIGWorkspaceButtonPanel::~CIGWorkspaceButtonPanel ()
{	
	::RemoveWindowSubclass (m_hCloseWnd, StaticHookPanelProc, 0);
	::RemoveWindowSubclass (m_hMaximizeWnd, StaticHookPanelProc, 0);
	::RemoveWindowSubclass (m_hMinimizeWnd, StaticHookPanelProc, 0);
	::RemoveWindowSubclass (m_hSaveWnd, StaticHookPanelProc, 0);
	::RemoveWindowSubclass (m_hSaveAsWnd, StaticHookPanelProc, 0);
	::RemoveWindowSubclass (m_hUploadWnd, StaticHookPanelProc, 0);
	::DestroyWindow (m_hCloseWnd);
	::DestroyWindow (m_hMaximizeWnd);
	::DestroyWindow (m_hMinimizeWnd);
	::DestroyWindow (m_hSaveWnd);
	::DestroyWindow (m_hSaveAsWnd);
	::DestroyWindow (m_hUploadWnd);

	if (m_hBackgroundBmp)
	{
		::DeleteObject ((HGDIOBJ)m_hBackgroundBmp);
		::DeleteDC (m_hBackgroundDC);
	}
}

void CIGWorkspaceButtonPanel::MoveLeft ()
{
	m_rcPanel.left -= IGWORKSPACE_COLUMNWIDTH;
	m_rcPanel.right -= IGWORKSPACE_COLUMNWIDTH;
	bool bIsInside = true;
	bIsInside = bIsInside && isInside (m_hMinimizeWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hMaximizeWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hCloseWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hSaveWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hSaveAsWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hUploadWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	if (bIsInside)
	{
		// redraw dockpanel button background
		Show (SW_HIDE);
		::InvalidateRect (m_hBackGround, NULL, TRUE);
		::UpdateWindow (m_hBackGround);		
		saveBackground();
	}
	moveButton (m_hMinimizeWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hMaximizeWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hCloseWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hSaveWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hSaveAsWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hUploadWnd, -1 * IGWORKSPACE_COLUMNWIDTH);
	Show (bIsInside ? SW_SHOW : SW_HIDE);
	redrawButtons();
}

void CIGWorkspaceButtonPanel::MoveRight ()
{
	m_rcPanel.left += IGWORKSPACE_COLUMNWIDTH;
	m_rcPanel.right += IGWORKSPACE_COLUMNWIDTH;
	bool bIsInside = true;
	bIsInside = bIsInside && isInside (m_hMinimizeWnd, IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hMaximizeWnd, IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hCloseWnd, IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hSaveWnd, IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hSaveAsWnd, IGWORKSPACE_COLUMNWIDTH);
	bIsInside = bIsInside && isInside (m_hUploadWnd, IGWORKSPACE_COLUMNWIDTH);
	if (bIsInside)
	{
		// redraw dockpanel button background
		Show (SW_HIDE);
		::InvalidateRect (m_hBackGround, NULL, TRUE);
		::UpdateWindow (m_hBackGround);
		saveBackground();
	}
	moveButton (m_hMinimizeWnd, IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hMaximizeWnd, IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hCloseWnd, IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hSaveWnd, IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hSaveAsWnd, IGWORKSPACE_COLUMNWIDTH);
	moveButton (m_hUploadWnd, IGWORKSPACE_COLUMNWIDTH);
	Show (bIsInside ? SW_SHOW : SW_HIDE);
	redrawButtons();
}

void CIGWorkspaceButtonPanel::Show (int nCmdShow)
{
	::ShowWindow (m_hMinimizeWnd, nCmdShow);
	::ShowWindow (m_hMaximizeWnd, nCmdShow);
	::ShowWindow (m_hCloseWnd, nCmdShow);
	::ShowWindow (m_hSaveWnd, nCmdShow);
	::ShowWindow (m_hSaveAsWnd, nCmdShow);
	::ShowWindow (m_hUploadWnd, nCmdShow);
}

bool CIGWorkspaceButtonPanel::isInside (HWND hButton)
{
	RECT rcParent;
	::GetWindowRect (m_hListBox, &rcParent);
	RECT rcButton;
	::GetWindowRect (hButton, &rcButton);
	return ((rcButton.left >= rcParent.left) 
		&&  (rcButton.right <= rcParent.right));
}

bool CIGWorkspaceButtonPanel::isInside (HWND hButton, int nOffset)
{
	RECT rcParent;
	::GetWindowRect (m_hListBox, &rcParent);
	RECT rcButton;
	::GetWindowRect (hButton, &rcButton);
	rcButton.left += nOffset;
	rcButton.right += nOffset;
	return ((rcButton.left >= rcParent.left) 
		&&  (rcButton.right <= rcParent.right));
}

void CIGWorkspaceButtonPanel::moveButton (HWND hButton, int nOffset)
{
	RECT rcButton;
	::GetWindowRect (hButton, &rcButton);
	rcButton.left += nOffset;
	rcButton.right += nOffset;
	::MoveWindow (hButton, rcButton.left, rcButton.top,
				rcButton.right - rcButton.left, rcButton.bottom - rcButton.top, TRUE);
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGWorkspaceButtonPanel::StaticHookPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGWorkspaceButtonPanel* pThis = (CIGWorkspaceButtonPanel*)dwRefData;
	if( pThis != NULL )
		return pThis->HookPanelProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGWorkspaceButtonPanel::HookPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch(msg)
	{
	case WM_ERASEBKGND:
		OnEraseBkgnd (hWnd, (HDC)wParam);
		return 1L;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		return 0L;

	case WM_MOUSEMOVE:
		{
			g_bChanged = g_hOverButton	? ((g_hOverButton != m_hCloseWnd) &&
											(g_hOverButton != m_hMaximizeWnd) &&
											(g_hOverButton != m_hMinimizeWnd) &&
											(g_hOverButton != m_hSaveWnd) &&
											(g_hOverButton != m_hSaveAsWnd) &&
											(g_hOverButton != m_hUploadWnd))
										: false;
			if (m_hOverButton)
				m_bButtonChanged = (m_hOverButton != hWnd);
			HWND hOldOverButton = m_hOverButton;
			m_hOverButton = hWnd;			
			if (m_bButtonChanged && hOldOverButton)
			{
				redrawButton (hOldOverButton);
				redrawButton (hWnd);
			}
			if (m_eButtonState != IGBUTTON_MOUSEOVER)
			{
				// test if last buttonpanel had time to unselecect
				// if not, send a special message to force mouse leave
				if (g_bChanged && !m_bButtonChanged)
					::SendMessageW (g_hOverButton, WM_MOUSELEAVE, IGWORKSPACEBUTTONPANEL_FORCEMOUSELEAVE_WPARAM, 0);
				m_eButtonState = IGBUTTON_MOUSEOVER;
				TRACKMOUSEEVENT trackMouseEvent;
				trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
				trackMouseEvent.dwFlags = TME_LEAVE;
				trackMouseEvent.hwndTrack = hWnd;
				trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
				::TrackMouseEvent (&trackMouseEvent);
				redrawButtons ();
			}
			g_hOverButton = hWnd;			
		}
		break;

	case WM_MOUSELEAVE:
		if (wParam == IGWORKSPACEBUTTONPANEL_FORCEMOUSELEAVE_WPARAM)
		{
			m_eButtonState = IGBUTTON_MOUSEOUT;
			redrawButtons ();
			m_hOverButton = NULL;
			return 0;
		}
		if (!m_bButtonChanged || g_bChanged)
		{
			m_eButtonState = IGBUTTON_MOUSEOUT;
			redrawButtons ();
			m_hOverButton = NULL;
			g_hOverButton = NULL;
			m_bButtonChanged = false;
			g_bChanged = false;
		}
		break;

	case WM_LBUTTONUP:
		::DefSubclassProc (hWnd, WM_LBUTTONDOWN, wParam, lParam);
		if (hWnd == m_hCloseWnd)
			OnButtonClose ();
		else if (hWnd == m_hMaximizeWnd)
			OnButtonMaximize ();
		else if (hWnd == m_hMinimizeWnd)
			OnButtonMinimize ();
		else if (hWnd == m_hSaveWnd)
			OnButtonSave ();
		else if (hWnd == m_hSaveAsWnd)
			OnButtonSaveAs ();
		else if (hWnd == m_hUploadWnd)
			OnButtonUpload ();
		break;

	case WM_PAINT:
		{
			VARIANT_BOOL vBool;
			m_pMultiFrame->get_isMinimized ((OLE_HANDLE)(IIGFrame *)m_pFrame, &vBool);
			m_bMinimized = (vBool == VARIANT_TRUE);
			m_pMultiFrame->get_isMaximized ((OLE_HANDLE)(IIGFrame *)m_pFrame, &vBool);
			m_bMaximized = (vBool == VARIANT_TRUE);
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			OnEraseBkgnd (hWnd, hDC);
			if (hWnd == m_hCloseWnd)
				drawButton (hWnd, hDC, g_pCxIconClose);
			else if (hWnd == m_hMaximizeWnd)
				drawButton (hWnd, hDC, m_bMaximized ? g_pCxIconUnMaximize : g_pCxIconMaximize);
			else if (hWnd == m_hMinimizeWnd)
				drawButton (hWnd, hDC, m_bMinimized ? g_pCxIconUnMaximize : g_pCxIconMinimize);
			else if (hWnd == m_hSaveWnd)
				drawButton (hWnd, hDC, g_pCxIconSave);
			else if (hWnd == m_hSaveAsWnd)
				drawButton (hWnd, hDC, g_pCxIconSaveAs);
			else if (hWnd == m_hUploadWnd)
				drawButton (hWnd, hDC, g_pCxIconUpload);
			::EndPaint(hWnd, &ps);
			return 0L;
		}
	}
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc(hWnd, msg, wParam, lParam);	

	switch(msg)
	{
		case WM_KILLFOCUS:
		case WM_LBUTTONUP:
			redrawButton (hWnd);
			break;
	}
	return lRes;
}

void CIGWorkspaceButtonPanel::redrawButton (HWND hWnd)
{
	::InvalidateRect (hWnd, NULL, TRUE);
	::UpdateWindow (hWnd);
}

void CIGWorkspaceButtonPanel::redrawButtons ()
{
	g_pCxIconClose->LayerDrawAllClearCache();
	g_pCxIconMaximize->LayerDrawAllClearCache();
	g_pCxIconMinimize->LayerDrawAllClearCache();
	g_pCxIconSave->LayerDrawAllClearCache();
	g_pCxIconSaveAs->LayerDrawAllClearCache();
	g_pCxIconUpload->LayerDrawAllClearCache();
	redrawButton (m_hMinimizeWnd);
	redrawButton (m_hMaximizeWnd);
	redrawButton (m_hCloseWnd);
	redrawButton (m_hSaveWnd);
	redrawButton (m_hSaveAsWnd);
	redrawButton (m_hUploadWnd);
}

int CIGWorkspaceButtonPanel::getFrameId ()
{
	// the most secure way to retrieve a frame id is to pass by multiframe panel
	// instead of the frame manager
	LONG nFrameId = -1;
	HRESULT hr = m_pMultiFrame->get_frameId ((OLE_HANDLE)((IIGFrame *)m_pFrame),
												&nFrameId);
	if (FAILED(hr))
		return -1;
	return (int)nFrameId;
}

void CIGWorkspaceButtonPanel::OnButtonClose ()
{
	int nFrameId = getFrameId ();
	if (nFrameId < 0)
		return;	
	m_pWorkspace->RemoveFrame (nFrameId);
}

void CIGWorkspaceButtonPanel::OnButtonMaximize ()
{
	if (m_bMaximized)
	{
		m_pMultiFrame->put_isMaximized ((OLE_HANDLE)(IIGFrame *)m_pFrame, VARIANT_FALSE);
		::InvalidateRect (m_hMaximizeWnd, NULL, FALSE);
		::InvalidateRect (m_hMinimizeWnd, NULL, FALSE);		
	}
	else
	{
		m_pMultiFrame->put_isMaximized ((OLE_HANDLE)(IIGFrame *)m_pFrame, VARIANT_TRUE);
		m_pDockPanel->RedrawControls();
	}
}

void CIGWorkspaceButtonPanel::OnButtonMinimize ()
{
	if (m_bMinimized)
	{
		m_pMultiFrame->put_isMinimized ((OLE_HANDLE)(IIGFrame *)m_pFrame, VARIANT_FALSE);
		m_pDockPanel->RedrawControls();
	}
	else
	{
		m_pMultiFrame->put_isMinimized ((OLE_HANDLE)(IIGFrame *)m_pFrame, VARIANT_TRUE);
		::InvalidateRect (m_hMaximizeWnd, NULL, FALSE);
		::InvalidateRect (m_hMinimizeWnd, NULL, FALSE);
	}	
}

void CIGWorkspaceButtonPanel::OnButtonSave ()
{
}

void CIGWorkspaceButtonPanel::OnButtonSaveAs ()
{
	LONG nFrameId = -1;
	m_pMultiFrame->get_frameId ((OLE_HANDLE)(IIGFrame *)m_pFrame, &nFrameId);
	m_pWorkspace->SaveFrame (nFrameId);
}

void CIGWorkspaceButtonPanel::OnButtonUpload ()
{
}

void CIGWorkspaceButtonPanel::OnEraseBkgnd (HWND hWnd, HDC hdc)
{		
	if (!m_hBackgroundBmp)
		saveBackground ();

	RECT rcButton;
	::GetWindowRect (hWnd, &rcButton);
	rcButton.left -= m_rcParent.left;
	rcButton.right -= m_rcParent.left;
	rcButton.top -= m_rcParent.top;
	rcButton.bottom -= m_rcParent.top;
	HGDIOBJ hOldMemBmp = ::SelectObject (m_hBackgroundDC, m_hBackgroundBmp);
	::BitBlt (hdc, 0, 0, rcButton.right - rcButton.left, rcButton.bottom - rcButton.top,
			m_hBackgroundDC, rcButton.left - m_rcPanel.left,
			rcButton.top,
			SRCCOPY);
	::SelectObject (m_hBackgroundDC, hOldMemBmp);	
}

void CIGWorkspaceButtonPanel::drawButton (HWND hWnd, HDC hDC, IGLibrary::IGFrame *pCxButtonIcon)
{
	if (pCxButtonIcon)
	{
		pCxButtonIcon->LayerDrawAllInClearedCache (hDC, 0, 0, -1, -1, 0, false, false, false, RGB (0, 0, 0));

		// test if mouse is over the current button
		if ((m_eButtonState == IGBUTTON_MOUSEOVER)
			&& (hWnd == m_hOverButton))
		{
			Graphics graphics (hDC);
			int nButtonSize = pCxButtonIcon->GetWidth();
			Rect rcGdiButton = Rect (0, 0,
									nButtonSize - 1,
									nButtonSize - 1);
			// draw blue square in mouseover mode
			SolidBrush solBrushButton (IGWORKSPACE_COLOR_BUTTON);
			graphics.FillRectangle (&solBrushButton, rcGdiButton);
			Pen penButtonBorder (IGWORKSPACE_COLOR_BUTTONBORDER);
			graphics.DrawRectangle (&penButtonBorder, rcGdiButton);
		}

		// draw button icon
		if (m_eButtonState == IGBUTTON_MOUSEOVER)
			pCxButtonIcon->LayerDrawAllFromCache (hDC, 0, 0, IGWORKSPACEBUTTONPANEL_ALPHAOVER, RGB (0, 0, 0));
		else
			pCxButtonIcon->LayerDrawAllFromCache (hDC, 0, 0, IGWORKSPACEBUTTONPANEL_ALPHANORMAL, RGB (0, 0, 0));
	}
}

void CIGWorkspaceButtonPanel::saveBackground ()
{
	int nLeft = m_rcPanel.left;
	int nTop = m_rcPanel.top;
	int nWidth = m_rcPanel.right - m_rcPanel.left;
	int nHeight = m_rcPanel.bottom - m_rcPanel.top;
	if (m_hBackgroundBmp)
		::DeleteObject ((HGDIOBJ)m_hBackgroundBmp);
	if (m_hBackgroundDC)
		::DeleteDC (m_hBackgroundDC);
	HDC hParentDC = ::GetDC (m_hListBox);
	m_hBackgroundDC = ::CreateCompatibleDC (hParentDC);
	m_hBackgroundBmp = ::CreateCompatibleBitmap (hParentDC, nWidth, nHeight);
	HGDIOBJ hOldMemBmp = ::SelectObject (m_hBackgroundDC, m_hBackgroundBmp);
	::BitBlt (m_hBackgroundDC, 0, 0, nWidth, nHeight,
			hParentDC, nLeft, nTop, SRCCOPY);
	::SelectObject (m_hBackgroundDC, hOldMemBmp);
	::ReleaseDC (m_hListBox, hParentDC);
}

void CIGWorkspaceButtonPanel::createButtonIcon (IGLibrary::IGFrame *&pCxButtonIcon, LPCWSTR pwIcon)
{
	if (!pCxButtonIcon)
	{
		HRSRC hResIcon = ::FindResourceW (getInstance(), pwIcon, L"PNG");
		pCxButtonIcon = new IGLibrary::IGFrame (hResIcon, CXIMAGE_FORMAT_PNG, getInstance());
	}
}

HWND CIGWorkspaceButtonPanel::createButtonWindow (int nLeft, int nTop, int nSize)
{
	return ::CreateWindowExW (WS_EX_LEFT, L"Button", L"",
										WS_POPUP | WS_VISIBLE,
										nLeft, nTop,
										nSize,
										nSize,
										m_hBackGround,	NULL, getInstance(), NULL);
}