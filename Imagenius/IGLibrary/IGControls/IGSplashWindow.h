#pragma once
#include <string>
#include <commctrl.h>

#define	IGSPLASH_TIMEREVENT_RENDER			100

namespace IGLibrary
{
// This define all the possible behavior of a command (cf : Command Manager)
typedef enum SPLASHSCREEN_PROGRESSBAR_MODE 
{
	// This mode doent show a progression but ask the user to wait
	SPLASHSCREEN_PROGRESSBAR_WAITING = 0,
	// This mode show a progression
	SPLASHSCREEN_PROGRESSBAR_PROGRESSION = 1,
	// This mode does not show a progress bar
	SPLASHSCREEN_PROGRESSBAR_NOTVISIBLE = 2
}SPLASHSCREEN_PROGRESSBAR_MODE;

class IGSplashWindow	: protected CxImage
{
public:
	IGSplashWindow (HWND);
	virtual ~IGSplashWindow (void);	

	void Show (bool bUpdate = false);
	void Hide ();
	void UpdatePosition();
	void ProgressSetMode (SPLASHSCREEN_PROGRESSBAR_MODE nProgressOption);
	void ProgressSetRange (UINT nMax, bool bLock = false);
	void ProgressSetSubRange(int nCurStartRange, int nNbSubRanges);
	void ProgressSetStep (UINT nStep);
	void ProgressSetPos (UINT nPos);
	void ProgressSetMessage (const wchar_t *pwMessage);
	void ProgressStepIt (UINT nNbSteps = 0);
	int ProgressGetProgress();
	HWND GetHWND ();

	static void SendSetRange (HWND hProgress, UINT nMax, bool bLock = false);
	static void SendSetSubRange (HWND hProgress, int nCurStartRange, int nNbSubRanges);
	static void SendStepIt (HWND hProgress);
	static void SendSetMessage (HWND hProgress, wchar_t *pwMessage);

protected:
	// Window Messages
	void OnPaint();
	// Splash screen dialog initialization
	INT_PTR OnInitDialog(HWND hWnd);

private:
	IGSplashWindow (const IGSplashWindow&);
	void invalidateProgress();

	// Creation
	bool createWindow();

	// Release window handles
	void destroyWindow();

	void centerWindow (HWND hParent);
	void redraw ();	
	void eraseBkgnd (HDC hdc);

	// Window Proc
	static INT_PTR CALLBACK staticDialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Data for the window
	int								m_nWidth;
	int								m_nHeight;
	RECT							m_rcSplashWindow;
	RECT							m_rcProgress;
	RECT							m_rcMessage;		
	HWND							m_hDlg;
	HWND							m_hParent;
	SPLASHSCREEN_PROGRESSBAR_MODE	m_nProgressOption;
	UINT							m_nRange;
	UINT							m_nStep;
	UINT							m_nPos;
	bool							m_bLockRange;
	int								m_nCurStartRange;
	int								m_nNbSubRanges;
	std::wstring					m_wsMessage;
	bool							m_bRefresh;
	bool							m_bSynchroneUpdate;
	float							m_fOldAngle;
	DWORD							m_dwCompleteRefreshCount;
};

}