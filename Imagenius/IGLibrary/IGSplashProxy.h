// IGSplashProxy.h : Declaration of the singleton splash screen object
#pragma once
#include "IGSplashWindow.h"

namespace IGLibrary
{

class IGSplashProxy
{
public:	
	IGSplashProxy(HWND);
	~IGSplashProxy();

	static void Init (HINSTANCE, int);
	static IGSplashProxy	*getInstance();

	//void ShowProgress ();
	//void ShowAlert (const wchar_t *pwMessage, const wchar_t *pwTitle, UINT nMask);
	void Show (HWND hParent);
	void Hide();
	void ProgressSetMode (SPLASHSCREEN_PROGRESSBAR_MODE nProgressOption);
	void ProgressSetRange (UINT nMax);
	void ProgressSetStep (UINT nStep);
	void ProgressSetPos (UINT nPos);
	void ProgressSetMessage (const wchar_t *pwMessage);
	void ProgressStepIt ();
	HWND getHWND ();

private:
	static HINSTANCE		g_hSplashInstance;
	static const UINT		g_nMaxProgressMessageSize;
	static int				g_nResIdLogo;

	SPLASHSCREEN_PROGRESSBAR_MODE	m_nProgressOption;
	UINT							m_nRange;
	UINT							m_nStep;
	UINT							m_nPos;
	std::wstring					m_wMessage;
	IGSplashWindow					*m_pSplashScreenWindow;
};

}