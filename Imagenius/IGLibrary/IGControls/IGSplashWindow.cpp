#include "StdAfx.h"
#include "IGThread.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGUserMessages.h"
#include "resource.h"
#include <process.h>
#include <winuser.h>
#include <windowsx.h>
#include <atlbase.h>
#include <atlctl.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace IGLibrary;

#define IGSPLASHWINDOW_COLOR_PROGRESS		Color(255, 52, 148, 214)	// Blue
#define PROGRESSBAR_BK_COLOR				RGB(255, 255, 255)
#define ROUND_RADIUS						10
#define IGSPLASHWINDOW_PROGRESSPNG_WIDTH	109
#define IGSPLASHWINDOW_PROGRESSPNG_HEIGHT	109
#define IGSPLASH_TIMERRENDER_INTERVAL		50	

extern HINSTANCE	g_hInstance;

IGSplashWindow::IGSplashWindow(HWND hParent): m_hDlg(NULL)
											, m_hParent(hParent)
											, m_nWidth (0)
											, m_nHeight (0)
											, m_nPos (0)
											, m_nStep (1)
											, m_nRange (100)
											, m_bLockRange (false)
											, m_nCurStartRange (0)
											, m_nNbSubRanges (0)
											, m_bRefresh (true)
											, m_bSynchroneUpdate (false)
											, m_fOldAngle (0.0f)
											, m_dwCompleteRefreshCount (0)
{
	::ZeroMemory (&m_rcSplashWindow, sizeof(RECT));
	::ZeroMemory (&m_rcMessage, sizeof(RECT));
	::ZeroMemory (&m_rcProgress, sizeof(RECT));
	HRSRC hLogo = ::FindResourceW (g_hInstance, MAKEINTRESOURCE (IDB_PROGRESS), L"PNG");
	if (hLogo)
	{
		LayerCreate(0);
		GetLayer(0)->LoadResource (hLogo, CXIMAGE_FORMAT_PNG, g_hInstance);
		createWindow();
	}
}

IGSplashWindow::~IGSplashWindow(void)
{
	::KillTimer (m_hDlg, IGSPLASH_TIMEREVENT_RENDER);
	destroyWindow ();
}

bool IGSplashWindow::createWindow()
{
	if( m_hDlg != NULL ) 
		return true;	// already created

	if (!g_hInstance)
		return false; // Wrong initialization of the class i.e init method takes an HINTANCE as pram
	
	// Create the main window
	m_hDlg = ::CreateDialogParamW (g_hInstance,
									MAKEINTRESOURCEW(IDD_SPLASH),
									m_hParent,
									staticDialogProc,
									(LPARAM)this);

	if( m_hDlg == NULL ) 
		return false;
	
	::SetTimer (m_hDlg, IGSPLASH_TIMEREVENT_RENDER, IGSPLASH_TIMERRENDER_INTERVAL, NULL);
	return true;
}

void IGSplashWindow::destroyWindow()
{
	if (m_hDlg)
	{
		::DestroyWindow(m_hDlg);
		m_hDlg = NULL;
	}
}

void IGSplashWindow::eraseBkgnd (HDC hdc)
{
	LayerDrawAllInClearedCache (hdc, 0, 0,
								m_nWidth, m_nHeight, NULL,
								false, false, true);
	LayerDrawAllFromCache (hdc);
}

void IGSplashWindow::OnPaint()
{
	PAINTSTRUCT ps; 
	HDC hdc =  ::BeginPaint (m_hDlg, &ps); 	
	eraseBkgnd (hdc);
	Graphics graphics (hdc);	
	Status st = graphics.SetSmoothingMode (SmoothingModeAntiAlias);
	if (m_wsMessage.length() > 0)
	{
		FontFamily fontFamily(L"Times New Roman");
		Font font(&fontFamily, 16, FontStyleRegular, UnitPixel);
		PointF      pointF(20.0f, 5.0f);
		SolidBrush  solidBrush(Color(255, 0, 0, 255));
		StringFormat	format(StringFormat::GenericDefault());
		format.SetAlignment (StringAlignmentCenter);
		graphics.DrawString (m_wsMessage.c_str(), -1, &font, RectF ((float)m_rcMessage.left, (float)m_rcMessage.top, 
																(float)(m_rcMessage.right - m_rcMessage.left), (float)(m_rcMessage.bottom - m_rcMessage.top)), &format, &solidBrush);
	}
	float fAngle = 360.0f * (float)m_nPos / (float)m_nRange;
	GraphicsPath pathProgress;
	pathProgress.StartFigure();
	pathProgress.AddArc (Rect (2, 2, IGSPLASHWINDOW_PROGRESSPNG_WIDTH - 4, IGSPLASHWINDOW_PROGRESSPNG_HEIGHT - 4), -90.0f + fAngle, 360.0f - fAngle);
	pathProgress.AddArc (RectF (6, 6, IGSPLASHWINDOW_PROGRESSPNG_WIDTH - 12, IGSPLASHWINDOW_PROGRESSPNG_HEIGHT - 12), -90.0f, fAngle - 360.0f);
	pathProgress.CloseFigure();
	SolidBrush brushWhite (Color::White);	
	graphics.FillPath (&brushWhite, &pathProgress);
	pathProgress.Reset();
	pathProgress.StartFigure();
	pathProgress.AddArc (Rect (2, 2, IGSPLASHWINDOW_PROGRESSPNG_WIDTH - 4, IGSPLASHWINDOW_PROGRESSPNG_HEIGHT - 4), -90.0f, fAngle);
	pathProgress.AddArc (RectF (6, 6, IGSPLASHWINDOW_PROGRESSPNG_WIDTH - 12, IGSPLASHWINDOW_PROGRESSPNG_HEIGHT - 12), -90.0f + fAngle, -fAngle);
	pathProgress.CloseFigure();
	SolidBrush brushProgress (IGSPLASHWINDOW_COLOR_PROGRESS);	
	graphics.FillPath (&brushProgress, &pathProgress);	
	::EndPaint(m_hDlg, &ps);
}

INT_PTR IGSplashWindow::DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_PAINT:			
			OnPaint();
			return 0L;

		case WM_ERASEBKGND:
			return 1L;

		case WM_TIMER:
			if (m_bRefresh)
			{
				m_bRefresh = false;
				// 500 ms global refresh
				if (m_dwCompleteRefreshCount++ % 10 == 0)
					UpdatePosition();
				else
					invalidateProgress();
			}
			break;

		case UM_PROGRESS_SETRANGE:
			ProgressSetRange ((UINT)wParam, (lParam != 0));
			return 1L;

		case UM_PROGRESS_SETSTEP:
			ProgressSetStep ((UINT)wParam);
			return 1L;

		case UM_PROGRESS_STEPIT:
			ProgressStepIt ((UINT)wParam);
			return 1L;

		case UM_PROGRESS_GETPROGRESS:
		{
			int *pProgress = (int*)wParam;
			if (pProgress)
				*pProgress = ProgressGetProgress();
			return 1L;
		}

		case UM_PROGRESS_SETMESSAGE:
			if (wParam == 0)
				ProgressSetMessage(L"");
			else
				ProgressSetMessage((const wchar_t*)wParam);
			return 1L;		

		case UM_PROGRESS_SETSUBRANGE:
			ProgressSetSubRange((int)wParam, (int)lParam);
			return 1L;
	}

	return -1;
}

INT_PTR CALLBACK IGSplashWindow::staticDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	IGSplashWindow* pThis = (IGSplashWindow*)::GetWindowLongPtrW (hWnd, GWLP_USERDATA);
	
	if( pThis != NULL )
	{
		return pThis->DialogProc(hWnd, msg, wParam, lParam);
	}
	else if (msg == WM_INITDIALOG)
	{
		pThis = (IGSplashWindow*)lParam;
		::SetWindowLongPtrW (hWnd, GWLP_USERDATA, (LONG)pThis);
		return pThis->OnInitDialog (hWnd);		
	}
    return FALSE;
}

INT_PTR IGSplashWindow::OnInitDialog(HWND hWnd)
{
	::GetWindowRect (hWnd, &m_rcSplashWindow);
	m_nWidth = m_rcSplashWindow.right - m_rcSplashWindow.left;
	m_nHeight = m_rcSplashWindow.bottom - m_rcSplashWindow.top;

	HWND hStcProgress = ::GetDlgItem (hWnd, IDC_PROGRESS);
	::GetWindowRect (hStcProgress, &m_rcProgress);
	::DestroyWindow (hStcProgress);
	m_rcProgress.left -= m_rcSplashWindow.left;
	m_rcProgress.top -= m_rcSplashWindow.top;
	m_rcProgress.right -= m_rcSplashWindow.left;
	m_rcProgress.bottom -= m_rcSplashWindow.top;

	HWND hStcMessage = ::GetDlgItem (hWnd, IDC_MESSAGE);
	::GetWindowRect (hStcMessage, &m_rcMessage);
	::DestroyWindow (hStcMessage);
	m_rcMessage.left -= m_rcSplashWindow.left;
	m_rcMessage.top -= m_rcSplashWindow.top;
	m_rcMessage.right -= m_rcSplashWindow.left;
	m_rcMessage.bottom -= m_rcSplashWindow.top;

	m_rcSplashWindow.right -= m_rcSplashWindow.left;
	m_rcSplashWindow.bottom -= m_rcSplashWindow.top;
	m_rcSplashWindow.top = m_rcSplashWindow.left = 0;	
		
	return FALSE;
}

void IGSplashWindow::centerWindow (HWND hParent)
{
	RECT rcParent;
	m_hParent = hParent;
	::GetWindowRect (hParent, &rcParent);
	rcParent.right -= rcParent.left;
	rcParent.bottom -= rcParent.top;
	rcParent.left = rcParent.top = 0;
	int x = rcParent.left + (rcParent.right - rcParent.left - m_nWidth) / 2;
	int y = rcParent.top  + (rcParent.bottom - rcParent.top - m_nHeight) / 2;
	if (x < rcParent.left)     
		x = rcParent.left;
	if (y < rcParent.top)      
		y = rcParent.top;	
	::SetWindowPos (m_hDlg, HWND_TOP, x, y, m_nWidth, m_nHeight, ::IsWindowVisible (m_hDlg) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
	m_rcSplashWindow.left = x;
	m_rcSplashWindow.top = y;
	m_rcSplashWindow.right = x + m_nWidth;
	m_rcSplashWindow.bottom = y + m_nHeight;
}

void IGSplashWindow::redraw ()
{
	::InvalidateRect (m_hParent, &m_rcSplashWindow, FALSE);
	::UpdateWindow (m_hParent);
	::InvalidateRect (m_hDlg, NULL, FALSE);	
}

void IGSplashWindow::Show (bool bUpdate)
{	
	m_bSynchroneUpdate = bUpdate;
	centerWindow (m_hParent);
	LayerDrawAllClearCache();
	::ShowWindow(m_hDlg, SW_SHOW);
	redraw ();
}

void IGSplashWindow::UpdatePosition()
{
	BOOL bShow = ::IsWindowVisible (m_hDlg);
	Hide();
	redraw ();
	centerWindow (m_hParent);
	if (bShow)
	{
		LayerDrawAllClearCache();
		::ShowWindow (m_hDlg, SW_SHOW);
		redraw ();		
	}
}

void IGSplashWindow::Hide()
{
	m_bSynchroneUpdate = false;
	::ShowWindow(m_hDlg, SW_HIDE);
}

void IGSplashWindow::ProgressSetMode (SPLASHSCREEN_PROGRESSBAR_MODE nProgressOption)
{
	m_nProgressOption = nProgressOption;
	redraw();
}

void IGSplashWindow::ProgressSetRange (UINT nMax, bool bLock)
{
	if (bLock){
		m_bLockRange = !m_bLockRange;
	}
	else
	{
		if (m_bLockRange)
			return;
	}
	if ((m_nCurStartRange == 0) && (m_nNbSubRanges == 0))
	{
		m_nRange = nMax;
		m_nPos = 0;
	}
	else
	{
		m_nRange = nMax * m_nNbSubRanges;
		m_nPos = nMax * m_nCurStartRange;	
	}	
	if (m_nRange <= 0)
		m_nRange = 100;
	m_nProgressOption = SPLASHSCREEN_PROGRESSBAR_PROGRESSION;
	m_nStep = 1;
	redraw();
}

void IGSplashWindow::ProgressSetSubRange(int nCurStartRange, int nNbSubRanges)
{
	m_nCurStartRange = nCurStartRange;
	m_nNbSubRanges = nNbSubRanges;
}

void IGSplashWindow::ProgressSetStep (UINT nStep)
{
	m_nStep = nStep;
}

void IGSplashWindow::ProgressSetPos (UINT nPos)
{	
	m_nPos = nPos;
	if (m_nPos == 0)
		m_fOldAngle = 0.0f;
	redraw();
}

void IGSplashWindow::ProgressSetMessage (const wchar_t *pwMessage)
{
	if (m_bLockRange)
		return;
	// Test Input parameter
	if (pwMessage == NULL)
		return;

	if (::wcslen (pwMessage) <= 0)
		return;

	m_wsMessage = pwMessage;
	::SetWindowTextW (m_hDlg, pwMessage);
	::InvalidateRect (m_hDlg, (const RECT*)&m_rcMessage, TRUE);
	// to accelerate message display we paint directly the splash
	::UpdateWindow (m_hDlg);
}

void IGSplashWindow::ProgressStepIt (UINT nNbSteps)
{
	UINT nMultStep = (nNbSteps > 0 ? nNbSteps : 1);
	m_nPos = (m_nPos + (m_nStep * nMultStep) > m_nRange) ? m_nRange : m_nPos + (m_nStep * nMultStep);
	if (m_bSynchroneUpdate)
	{
		// synchrone redraw
		if ((UINT)(100.0f * (float)m_nPos / (float)m_nRange) % 5 == 0)
		{
			invalidateProgress();
			::UpdateWindow (m_hDlg);			
		}
	}
	else
	{
		// asynchrone redraw
		m_bRefresh = true;
	}
}

int IGSplashWindow::ProgressGetProgress()
{
	return (int)(100.0f * (float)m_nPos / (float)m_nRange);
}

HWND IGSplashWindow::GetHWND ()
{
	return m_hDlg;
}

void IGSplashWindow::invalidateProgress()
{
	float fAngle = PI / 2.0f - 2.0f * PI * (float)m_nPos / (float)m_nRange;
	RECT rcUpdateSplash;
	int nRay = IGSPLASHWINDOW_PROGRESSPNG_WIDTH / 2 - 4;
	rcUpdateSplash.left = (int)((float)IGSPLASHWINDOW_PROGRESSPNG_WIDTH / 2.0f + (float)nRay * min (cosf (m_fOldAngle), cosf (fAngle))) - 15;
	rcUpdateSplash.top = (int)((float)IGSPLASHWINDOW_PROGRESSPNG_WIDTH / 2.0f - (float)nRay * max (sinf (m_fOldAngle), sinf (fAngle))) - 15;
	rcUpdateSplash.right = (int)((float)IGSPLASHWINDOW_PROGRESSPNG_WIDTH / 2.0f + (float)nRay * max (cosf (m_fOldAngle), cosf (fAngle))) + 15;
	rcUpdateSplash.bottom = (int)((float)IGSPLASHWINDOW_PROGRESSPNG_WIDTH / 2.0f - (float)nRay * min (sinf (m_fOldAngle), sinf (fAngle))) + 15;
	::InvalidateRect (m_hDlg, &rcUpdateSplash, TRUE);	
	m_fOldAngle = fAngle;
}

void IGSplashWindow::SendSetRange (HWND hProgress, UINT nMax, bool bLock)
{
	::SendMessageW (hProgress, UM_PROGRESS_SETRANGE, nMax, bLock ? 1 : 0);
}

void IGSplashWindow::SendSetSubRange (HWND hProgress, int nCurStartRange, int nNbSubRanges)
{
	::SendMessageW (hProgress, UM_PROGRESS_SETSUBRANGE, nCurStartRange, nNbSubRanges);
}

void IGSplashWindow::SendStepIt (HWND hProgress)
{
	::SendMessageW (hProgress, UM_PROGRESS_STEPIT, 0, 0);
}

void IGSplashWindow::SendSetMessage (HWND hProgress, wchar_t *pwMessage)
{
	::SendMessageW (hProgress, UM_PROGRESS_SETMESSAGE, (WPARAM)pwMessage, 0);
}