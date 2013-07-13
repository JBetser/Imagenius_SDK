#include "stdafx.h"
#include "IGHyperLink.h"
#include "IGTrace.h"

#include <commctrl.h>
#include <atlbase.h>
#include <atlconv.h>		// for Unicode conversion
#include <shellapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4996)		// This function or variable may be unsafe

#define TOOLTIP_ID 1

// Uncomment following line to enable error message box for URL navigation
#define XHYPERLINK_REPORT_ERROR

using namespace IGLibrary;

///////////////////////////////////////////////////////////////////////////////
IGHyperlink::IGHyperlink()
{
	m_hWnd			  = NULL;
	m_hLinkCursor     = NULL;			// No cursor as yet
	m_crLinkColour    = RGB(0,0,238);	// Blue
	m_crVisitedColour = RGB(85,26,139);	// Purple
	m_crHoverColour	  = RGB(255,0,0);	// Red
	m_bOverControl    = FALSE;			// Cursor not yet over control
	m_bVisited        = FALSE;			// Hasn't been visited yet.
	m_nUnderline      = ulHover;		// Underline the link?
	m_bAdjustToFit    = TRUE;			// Resize the window to fit the text?
	m_strURL          = _T("");
	m_nTimerID        = 100;
	m_bNotifyParent   = FALSE;			// TRUE = notify parent
	m_bIsURLEnabled   = TRUE;			// TRUE = navigate to url
	m_bToolTip        = TRUE;			// TRUE = display tooltip
	m_crBackground    = (UINT) -1;		// set to default (no bg color)
	m_bAlwaysOpenNew  = FALSE;			// TRUE = always open new browser window
	m_currentFont	  = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
IGHyperlink::~IGHyperlink()
{
	TRACE(_T("in IGHyperlink::~IGHyperlink\n"));

	if (m_hLinkCursor)
		::DestroyCursor(m_hLinkCursor);
	::DeleteObject (m_UnderlineFont);
	::DeleteObject (m_Brush);
}

BOOL IGHyperlink::Create (const RECT& rect, HWND hParent)
{
	m_hWnd = CreateWindowW (L"Static", L"IGStatic", WS_CHILD | WS_VISIBLE, rect.left, rect.top,
							rect.right - rect.left, rect.bottom - rect.top, hParent, NULL, GetInstance(), 0);
	if (!m_hWnd)
		return FALSE;

	// We want to get mouse clicks via STN_CLICKED
	DWORD dwStyle = ::GetWindowLong (m_hWnd, GWL_STYLE);
	::SetWindowLong (m_hWnd, GWL_STYLE, dwStyle | SS_NOTIFY);
	
	// Set the URL as the window text
	if (m_strURL.empty())
	{
		int nLengthURL = ::GetWindowTextLengthW (m_hWnd);
		wchar_t *pwStrURL = new wchar_t [nLengthURL + 1];
		::GetWindowText (m_hWnd, pwStrURL, nLengthURL + 1);
		m_strURL = pwStrURL;
		delete [] pwStrURL;
	}

	// Check that the window text isn't empty. If it is, set it as the URL.
	std::wstring strWndText;
	int nLength = ::GetWindowTextLengthW (m_hWnd);
	wchar_t *pwStr = new wchar_t [nLength + 1];
	::GetWindowText (m_hWnd, pwStr, nLength + 1);
	strWndText = pwStr;
	delete [] pwStr;

	if (strWndText.empty()) 
	{
		_ASSERT(!m_strURL.empty());	// Window and URL both NULL. DUH!
		if (m_strURL.empty())
			return FALSE;
		::SetWindowText (m_hWnd, m_strURL.c_str());
	}

	if (!m_currentFont)
	{
		m_currentFont = (HFONT)::GetStockObject (DEFAULT_GUI_FONT);
		if (!m_currentFont)
			m_currentFont = (HFONT) ::GetStockObject (ANSI_VAR_FONT);
	}
	_ASSERT (m_currentFont);
	if (!m_currentFont)
		return FALSE;

	// Create the std and underline fonts
	LOGFONT lf = {0};
	lf.lfWidth           = 0;
	lf.lfEscapement      = 0;
	lf.lfOrientation     = 0;
	lf.lfItalic          = FALSE;
	lf.lfUnderline       = FALSE;
	lf.lfStrikeOut       = FALSE;
	lf.lfCharSet         = ANSI_CHARSET;
	lf.lfClipPrecision   = CLIP_DEFAULT_PRECIS;
	lf.lfOutPrecision    = OUT_DEFAULT_PRECIS;
	lf.lfQuality         = DEFAULT_QUALITY;
	lf.lfPitchAndFamily  = DEFAULT_PITCH;
	lf.lfWeight			 = FW_NORMAL;
	::lstrcpyW (lf.lfFaceName, L"Tahoma");
	lf.lfHeight = -12;
	m_StdFont = ::CreateFontIndirectW (&lf);

	lf.lfUnderline       = TRUE;
	m_UnderlineFont = ::CreateFontIndirectW (&lf);

	if (!m_StdFont || !m_UnderlineFont)
		return FALSE;

	PositionWindow();		// Adjust size of window to fit URL if necessary
	SetDefaultCursor();		// Try and load up a "hand" cursor
	SetUnderline();

	// Create the tooltip
	if (m_bToolTip)
	{
		RECT rect; 
		::GetClientRect (m_hWnd, &rect);
//		m_ToolTip.Create(this);
//		m_ToolTip.AddTool(this, m_strURL, rect, TOOLTIP_ID);
	}

	::SetWindowSubclass (m_hWnd, StaticHookWndProc, 0, (DWORD_PTR)this);

	return TRUE;
}

BOOL IGHyperlink::Destroy () 
{
	::KillTimer (m_hWnd, m_nTimerID);
	::RemoveWindowSubclass (m_hWnd, StaticHookWndProc, 0);
	return ::DestroyWindow (m_hWnd);
}

/*
///////////////////////////////////////////////////////////////////////////////
// PreTranslateMessage
BOOL IGHyperlink::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.RelayEvent(pMsg);
	return CStatic::PreTranslateMessage(pMsg);
}*/

///////////////////////////////////////////////////////////////////////////////
// OnClicked
void IGHyperlink::OnClicked()
{
	m_bOverControl = FALSE;
	int result = HINSTANCE_ERROR + 1;
	if (m_bIsURLEnabled)
		result = (int)(INT_PTR)GotoURL(m_strURL.c_str(), SW_SHOW, m_bAlwaysOpenNew);
	m_bVisited = (result > HINSTANCE_ERROR);
	if (!m_bVisited)
	{
		MessageBeep(MB_ICONEXCLAMATION);	 // Unable to follow link
		ReportError(result);
	}
	else 
		SetVisited();						// Repaint to show visited colour

	NotifyParent();
}

///////////////////////////////////////////////////////////////////////////////
// CtlColor
#ifdef _DEBUG
HBRUSH IGHyperlink::CtlColor(HDC hdc, UINT nCtlColor) 
#else
HBRUSH IGHyperlink::CtlColor(HDC hdc, UINT /*nCtlColor*/) 
#endif
{
	_ASSERT(nCtlColor == CTLCOLOR_STATIC);

	if (m_bOverControl)
		::SetTextColor (hdc, m_crHoverColour);
	else if (m_bVisited)
		::SetTextColor (hdc, m_crVisitedColour);
	else
		::SetTextColor (hdc, m_crLinkColour);

	// transparent text.
	::SetBkMode (hdc, TRANSPARENT);

	if (m_Brush)
	{
		::SetBkColor(hdc, m_crBackground);
		return (HBRUSH) m_Brush;
	}
	else
	{
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void IGHyperlink::OnMouseMove(UINT /*nFlags*/, LPPOINT /*point*/) 
{
	if (!m_bOverControl)		// Cursor has just moved over control
	{
		m_bOverControl = TRUE;

		if (m_nUnderline == ulHover)
			m_currentFont = m_UnderlineFont;
		::InvalidateRect (m_hWnd, NULL, FALSE);

		::SetTimer (m_hWnd, m_nTimerID, 100, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void IGHyperlink::OnTimer(UINT /*nIDEvent*/) 
{
	DWORD dwPt = ::GetMessagePos();
	POINTS pts = MAKEPOINTS (dwPt);
	POINT pt = {pts.x, pts.y};
	::ScreenToClient (m_hWnd, &pt);

	RECT rect;
	::GetClientRect (m_hWnd, &rect);
	if (::PtInRect (&rect, pt))
	{
		m_bOverControl = FALSE;
		::KillTimer (m_hWnd, m_nTimerID);

		if (m_nUnderline != ulAlways)
			m_currentFont = m_StdFont;
		rect.bottom+=10;
		::InvalidateRect (m_hWnd, &rect, FALSE);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnSetCursor
BOOL IGHyperlink::OnSetCursor() 
{
	if (m_hLinkCursor)
	{
		::SetCursor(m_hLinkCursor);
		return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL IGHyperlink::OnEraseBkgnd(HDC hdc) 
{
	RECT rect;
	::GetClientRect (m_hWnd, &rect);
	if (m_crBackground != (UINT)-1)
		::FillRect (hdc, &rect, (HBRUSH)m_crBackground);
	else
		::FillRect (hdc, &rect, (HBRUSH)::GetSysColor(COLOR_3DFACE));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// IGHyperlink operations

///////////////////////////////////////////////////////////////////////////////
// SetURL
void IGHyperlink::SetURL(std::wstring strURL)
{
	m_strURL = strURL;

	if (::IsWindow (m_hWnd)) 
	{
		PositionWindow();
//		m_ToolTip.UpdateTipText(strURL, this, TOOLTIP_ID);
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetColours
void IGHyperlink::SetColours(COLORREF crLinkColour, 
							 COLORREF crVisitedColour,	
							 COLORREF crHoverColour /* = -1 */) 
{ 
	m_crLinkColour	= crLinkColour; 
	m_crVisitedColour = crVisitedColour;

	if (crHoverColour == -1)
		m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
	else
		m_crHoverColour = crHoverColour;

	if (::IsWindow(m_hWnd))
		::InvalidateRect (m_hWnd, NULL, FALSE); 
}

///////////////////////////////////////////////////////////////////////////////
// SetBackgroundColour
void IGHyperlink::SetBackgroundColour(COLORREF crBackground)
{
	m_crBackground = crBackground;
	if (m_Brush)
		::DeleteObject (m_Brush);
	m_Brush = ::CreateSolidBrush (m_crBackground);
}

///////////////////////////////////////////////////////////////////////////////
// SetVisited
void IGHyperlink::SetVisited(BOOL bVisited /* = TRUE */) 
{ 
	m_bVisited = bVisited; 

	if (::IsWindow(m_hWnd))
		::InvalidateRect (m_hWnd, NULL, FALSE); 
}

///////////////////////////////////////////////////////////////////////////////
// SetLinkCursor
void IGHyperlink::SetLinkCursor(HCURSOR hCursor)
{ 
	m_hLinkCursor = hCursor;
	if (m_hLinkCursor == NULL)
		SetDefaultCursor();
}

///////////////////////////////////////////////////////////////////////////////
// SetUnderline
void IGHyperlink::SetUnderline(int nUnderline /*=ulHover*/)
{
	if (m_nUnderline == nUnderline)
		return;

	if (::IsWindow(m_hWnd))
	{
		if (nUnderline == ulAlways)
			m_currentFont = m_UnderlineFont;
		else
			m_currentFont = m_StdFont;

		::InvalidateRect (m_hWnd, NULL, FALSE); 
	}

	m_nUnderline = nUnderline;
}

///////////////////////////////////////////////////////////////////////////////
// SetAutoSize
void IGHyperlink::SetAutoSize(BOOL bAutoSize /* = TRUE */)
{
	m_bAdjustToFit = bAutoSize;

	if (::IsWindow(m_hWnd))
		PositionWindow();
}

///////////////////////////////////////////////////////////////////////////////
// SetWindowText
void IGHyperlink::SetWindowText(LPCTSTR lpszString)
{
	_ASSERT(lpszString);
	if (!lpszString)
		return;
	::SetWindowTextW (m_hWnd, _T(""));
	::RedrawWindow (m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	::SetWindowTextW (m_hWnd, lpszString);
	PositionWindow();
}

///////////////////////////////////////////////////////////////////////////////
// PositionWindow

// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.
//
// Suggested by Pål K. Tønder 
//
void IGHyperlink::PositionWindow()
{
	if (!::IsWindow(m_hWnd) || !m_bAdjustToFit) 
		return;

	// Get the current window position
	RECT rcWnd, rcClient;
	::GetWindowRect (m_hWnd, &rcWnd);
	::GetClientRect (m_hWnd, &rcClient);

	POINT ptWndTopLeft = {rcWnd.left, rcWnd.top};
	POINT ptWndBottomRight = {rcWnd.right, rcWnd.bottom};
	POINT ptClientTopLeft = {rcClient.left, rcClient.top};
	POINT ptClientBottomRight = {rcClient.right, rcClient.bottom};
	::ClientToScreen (m_hWnd, &ptClientTopLeft);
	::ClientToScreen (m_hWnd, &ptClientBottomRight);

	HWND hParent = ::GetParent (m_hWnd);
	if (hParent)
	{
		::ScreenToClient (hParent, &ptWndTopLeft);
		::ScreenToClient (hParent, &ptWndBottomRight);
		::ScreenToClient (hParent, &ptClientTopLeft);
		::ScreenToClient (hParent, &ptClientBottomRight);
	}

	rcWnd.left = ptClientTopLeft.x;
	rcWnd.top = ptClientTopLeft.y;
	rcWnd.right = ptWndBottomRight.x;
	rcWnd.bottom = ptWndBottomRight.y;
	rcClient.left = ptClientTopLeft.x;
	rcClient.top = ptClientTopLeft.y;
	rcClient.right = ptWndBottomRight.x;
	rcClient.bottom = ptWndBottomRight.y;

	// Get the size of the window text
	int nWndTextSize = ::GetWindowTextLengthW (m_hWnd);
	wchar_t *pwWndText = new wchar_t [nWndTextSize + 1];
	::GetWindowTextW (m_hWnd, pwWndText, nWndTextSize + 1);
	std::wstring wsWndText (pwWndText);
	delete [] pwWndText;

	HDC hdc = GetDC (m_hWnd);
	HGDIOBJ hOldFont = ::SelectObject (hdc, m_UnderlineFont);
	SIZE szFont;
	::GetTextExtentPoint (hdc, wsWndText.c_str(), wsWndText.length(), &szFont);
	::SelectObject (hdc, hOldFont);
	::ReleaseDC (m_hWnd, hdc);

	// Adjust for window borders
	szFont.cx += (rcWnd.right - rcWnd.left) - (rcClient.right - rcClient.left); 
	szFont.cy += (rcWnd.bottom - rcWnd.top) - (rcClient.bottom - rcClient.top);

	// Get the text justification via the window style
	DWORD dwStyle = ::GetWindowLongW (m_hWnd, GWL_STYLE);

	// Recalc the window size and position based on the text justification
	if (dwStyle & SS_CENTERIMAGE)
	{
		int nDeflate = (rcWnd.bottom - rcWnd.top - szFont.cy) / 2;
		rcWnd.top += nDeflate;
		rcWnd.bottom += nDeflate;
	}
	else
		rcWnd.bottom = rcWnd.top + szFont.cy;

	if (dwStyle & SS_CENTER)
	{
		int nDeflate = (rcWnd.right - rcWnd.left - szFont.cx) / 2;
		rcWnd.left += nDeflate;
		rcWnd.right += nDeflate;
	}
	else if (dwStyle & SS_RIGHT) 
		rcWnd.left = rcWnd.right - szFont.cx;
	else // SS_LEFT = 0, so we can't test for it explicitly 
		rcWnd.right = rcWnd.left + szFont.cx;

	// Move the window
	::SetWindowPos (m_hWnd, NULL, 
					 rcWnd.left, rcWnd.top, 
					 rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, 
					 SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
// IGHyperlink implementation

///////////////////////////////////////////////////////////////////////////////
// SetDefaultCursor
void IGHyperlink::SetDefaultCursor()
{
	if (m_hLinkCursor == NULL)	// No cursor handle - try to load one
	{
		TRACE(_T("loading from IDC_HAND\n"));
		m_hLinkCursor =  ::LoadCursorW (GetInstance(), IDC_HAND);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetRegKey
LONG IGHyperlink::GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = ::RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

	if (retval == ERROR_SUCCESS) 
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		::RegQueryValue (hkey, NULL, data, &datasize);
		_tcscpy (retdata, data);
		::RegCloseKey(hkey);
	}

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
// ReportError
void IGHyperlink::ReportError(int nError)
{
#ifdef XHYPERLINK_REPORT_ERROR
	std::wstring str;
	switch (nError) 
	{
		case 0:							str = L"The operating system is out\nof memory or resources."; break;
		case SE_ERR_PNF:				str = L"The specified path was not found."; break;
		case SE_ERR_FNF:				str = L"The specified file was not found."; break;
		case ERROR_BAD_FORMAT:			str = L"The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image)."; break;
		case SE_ERR_ACCESSDENIED:		str = L"The operating system denied\naccess to the specified file."; break;
		case SE_ERR_ASSOCINCOMPLETE:	str = L"The filename association is\nincomplete or invalid."; break;
		case SE_ERR_DDEBUSY:			str = L"The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed."; break;
		case SE_ERR_DDEFAIL:			str = L"The DDE transaction failed."; break;
		case SE_ERR_DDETIMEOUT:			str = L"The DDE transaction could not\nbe completed because the request timed out."; break;
		case SE_ERR_DLLNOTFOUND:		str = L"The specified dynamic-link library was not found."; break;
		case SE_ERR_NOASSOC:			str = L"There is no application associated\nwith the given filename extension."; break;
		case SE_ERR_OOM:				str = L"There was not enough memory to complete the operation."; break;
		case SE_ERR_SHARE:				str = L"A sharing violation occurred. ";
		default:						str = L"Unknown Error occurred."; break;
	}
	str = L"Unable to open hyperlink:\n\n" + str;
	::MessageBoxW (m_hWnd, str.c_str(), L"Hyperlink Error", MB_ICONEXCLAMATION | MB_OK);
#endif	// XHYPERLINK_REPORT_ERROR
}

///////////////////////////////////////////////////////////////////////////////
// NotifyParent
void IGHyperlink::NotifyParent()
{
	if (m_bNotifyParent)
	{
		HWND hParent = ::GetParent (m_hWnd);
		if (hParent && ::IsWindow (hParent))
		{
			// wParam will contain control id
			::SendMessageW (hParent, UM_XHYPERLINK_CLICKED, ::GetDlgCtrlID (m_hWnd), 0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// GotoURL
HINSTANCE IGHyperlink::GotoURL(LPCTSTR url, int showcmd, BOOL bAlwaysOpenNew /*= FALSE*/)
{
	// if no url then this is not an internet link
	if (!url || url[0] == _T('\0'))
		return (HINSTANCE) HINSTANCE_ERROR + 1;

	TCHAR key[MAX_PATH*2];

	// First try ShellExecute()
	TCHAR *verb = _T("open");
	if (bAlwaysOpenNew)
		verb = _T("new");
	HINSTANCE result = ::ShellExecute (NULL, verb, url, NULL,NULL, showcmd);

	// If it failed, get the .htm regkey and lookup the program
	if (result <= (HINSTANCE)HINSTANCE_ERROR) 
	{
		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) 
		{
			_tcscat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) 
			{
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) 
				{					// No quotes found
					pos = _tcsstr(key, _T("%1"));	// Check for %1, without quotes 
					if (pos == NULL)				// No parameter at all...
						pos = key + _tcslen(key)-1;
					else
						*pos = _T('\0');				// Remove the parameter
				}
				else
				{
					*pos = _T('\0');					// Remove the parameter
				}

				_tcscat(pos, _T(" "));
				_tcscat(pos, url);

				//USES_CONVERSION;
				result = (HINSTANCE) (UINT_PTR) ::WinExec (CW2A(key), showcmd);
			}
		}
	}

	return result;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY IGHyperlink::StaticHookWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
														UINT_PTR /*uIdSubclass*/, DWORD_PTR dwRefData)
{
	IGHyperlink* pThis = (IGHyperlink*)dwRefData;
	if( pThis != NULL )
		return pThis->HookWndProc (hwnd, msg, wParam, lParam);

	return ::DefSubclassProc (hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT IGHyperlink::HookWndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CTLCOLORSTATIC:
			return (LRESULT)CtlColor ((HDC)wParam, (UINT)lParam);

		case WM_SETCURSOR:
			return OnSetCursor();

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

		case WM_LBUTTONUP:
			OnClicked ();
			break;
	}
	// Default behavior using the old window proc
	return ::DefSubclassProc (hWnd, msg, wParam, lParam);
}