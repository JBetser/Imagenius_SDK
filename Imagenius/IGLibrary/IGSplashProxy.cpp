// SplashScreen : Implementation of IGSplashProxy

#include "stdafx.h"
#include "IGSplashProxy.h"

using namespace IGLibrary;

HINSTANCE		IGSplashProxy::g_hSplashInstance = NULL;
const UINT		IGSplashProxy::g_nMaxProgressMessageSize = 256;
int				IGSplashProxy::g_nResIdLogo = 0;

// IGSplashProxy
IGSplashProxy::IGSplashProxy(HWND hParent)	: m_pSplashScreenWindow (NULL)
											, m_nProgressOption (SPLASHSCREEN_PROGRESSBAR_WAITING)
											, m_nRange (100)
											, m_nStep (1)
											, m_nPos (0)
{
	// Create the class that manage the window
	m_pSplashScreenWindow = new IGSplashWindow(hParent);

	if (g_hSplashInstance == NULL || m_pSplashScreenWindow == NULL)
	{
		delete this;
	}
	else
	{
		m_pSplashScreenWindow->CS_SetHINSTANCE (g_hSplashInstance, g_nResIdLogo);
		// Init the class
		m_pSplashScreenWindow->Start();
	}
}

IGSplashProxy::~IGSplashProxy()
{
	if (m_pSplashScreenWindow)
	{
		delete m_pSplashScreenWindow;
		m_pSplashScreenWindow = NULL;
	}
}

// Hide the Splash Screen
void IGSplashProxy::Hide()
{
	if (m_pSplashScreenWindow == NULL) 
		return;

	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_HIDE, NULL, 0);
}

// Show the Splash Screen
void IGSplashProxy::Show (HWND hParent)
{
	if (m_pSplashScreenWindow == NULL) 
		return;

	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_SHOW, (WPARAM)hParent, 0, true);
}

void IGSplashProxy::Init (HINSTANCE hInstance, int nResLogo)
{
	g_hSplashInstance = hInstance;
	g_nResIdLogo = nResLogo;
}

void IGSplashProxy::ProgressSetMode (SPLASHSCREEN_PROGRESSBAR_MODE nProgressOption)
{
	if (m_pSplashScreenWindow == NULL) 
		return;

	BOOL bIsAnimated = TRUE;
	m_nProgressOption = nProgressOption;
	if (m_nProgressOption == SPLASHSCREEN_PROGRESSBAR_PROGRESSION)
	{		
		ProgressSetRange (m_nRange);
		ProgressSetStep (m_nStep);
		ProgressSetPos (m_nPos);
		bIsAnimated = FALSE;
	}
	ProgressSetMessage (m_wMessage.c_str());
	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_PROGRESSMODE, NULL, (WPARAM)bIsAnimated);
}

void IGSplashProxy::ProgressSetRange (UINT nMax)
{	
	if (m_pSplashScreenWindow == NULL) 
		return;

	m_nRange = nMax;
	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_PROGRESSMODE, FALSE, 0);
	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_SETMAXVALUE,  nMax, 0);
}

void IGSplashProxy::ProgressSetStep (UINT nStep)
{
	if (m_pSplashScreenWindow == NULL) 
		return;

	m_nStep = nStep;
	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_SETSTEP, nStep, 0);
}

void IGSplashProxy::ProgressSetPos (UINT nPos)
{	
	if (m_pSplashScreenWindow == NULL) 
		return;

	m_nPos = nPos;
	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_SETPOS, nPos, 0);
}

void IGSplashProxy::ProgressSetMessage (const wchar_t *pwMessage)
{
	if (m_pSplashScreenWindow == NULL) 
		return;

	// Test Input parameter
	if (pwMessage == NULL)
		return;

	if (::wcslen (pwMessage) <= 0)
		return;

	m_wMessage = pwMessage;
	m_pSplashScreenWindow->CS_SetLoadingText (pwMessage);
}

void IGSplashProxy::ProgressStepIt ()
{
	if (m_pSplashScreenWindow == NULL) 
		return;

	m_nPos = (m_nPos + m_nStep > m_nRange) ? m_nRange : m_nPos + m_nStep;
	m_pSplashScreenWindow->PostThreadMessage (IGSplashWindow::UM_SETPOS, m_nPos, 0);
}

HWND IGSplashProxy::getHWND ()
{
	HWND hSplashWin = NULL;
	m_pSplashScreenWindow->CS_GetHWND (&hSplashWin);
	return hSplashWin;
}