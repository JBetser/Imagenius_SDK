// IGServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windowsx.h>
#include <commctrl.h>
#include <string>
#include <IGPictureEditor_i.h>
#include "IGServer.h"
#include <fstream>

MIDL_DEFINE_GUID(CLSID, CLSID_IGMainWindow,0xDF95F65E,0xE9E9,0x4B63,0x89,0x3D,0xD4,0x84,0xC7,0x7C,0xE0,0x9E);

#define MAX_LOADSTRING 100
#define WINDOW_WIDTH	640
#define WINDOW_HEIGHT	480
#define STATUS_HEIGHT	20

#define IGMAINWINDOW_TITLE	L"IGMainWindow"
#define IGMAINWINDOW_CLASS	L"IGMainWindowClass"
#define IGSERVER_CLASS	L"IGServerClass"
#define IGSERVER_OUTPUTFILES_FOLDERHEADER L"IGOutputFiles"

using namespace std;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text

// Forward declarations of functions included in this code module:
void				RegisterClasses(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int, HWND*, BOOL);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProcIGMain(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

HWND		g_hIGMainWindow = NULL;
HWND		g_hStatus = NULL;
SHORT		g_nPortInputId = 0;
wstring		g_wsServerIP;
wstring		g_wsOutputIP;
wstring		g_wsOutputDrive;
wstring		g_wsDoMount;
wstring		g_wsSynchroEventReady = L"IGSMServerReady";
UINT		g_threadMessageReady = ::RegisterWindowMessageW (L"WakeUp");

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//_ASSERT(0);
	//int *p = NULL;
	//*p = 0;
	try{
		// read arguments
		if (::wcslen (lpCmdLine) == 0)
		{
			MessageBox (NULL, L"Missing argument: Input port Id.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Missing argument: Input port Id." << endl;
			return FALSE;
		}
		wstring wsCmdLine (lpCmdLine);
		size_t szOffset = 0;
		szOffset = wsCmdLine.find (L" ");
		if (szOffset == wstring::npos)
		{
			MessageBox (NULL, L"Missing argument: Server IP.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Missing argument: Server IP." << endl;
			return FALSE;
		}
		wstring wsPortInputId (wsCmdLine.substr (0, szOffset));
		g_nPortInputId = ::_wtoi (wsPortInputId.c_str());
		wsCmdLine = wsCmdLine.substr (szOffset + 1);
		szOffset = wsCmdLine.find (L" ");
		if (szOffset == wstring::npos)
		{
			MessageBox (NULL, L"Missing argument: Output web server IP.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Missing argument: Output web server IP." << endl;
			return FALSE;
		}
		g_wsServerIP = wsCmdLine.substr (0, szOffset);
		wsCmdLine = wsCmdLine.substr (szOffset + 1);
		szOffset = wsCmdLine.find (L" ");
		if (szOffset == wstring::npos)
		{
			MessageBox (NULL, L"Missing argument: Output Drive letter.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Missing argument: Output Drive letter." << endl;
			return FALSE;
		}
		g_wsOutputIP = wsCmdLine.substr (0, szOffset);
		wsCmdLine = wsCmdLine.substr (szOffset + 1);
		szOffset = wsCmdLine.find (L" ");
		if (szOffset == wstring::npos)
		{
			MessageBox (NULL, L"Missing argument: Output Drive letter.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Missing argument: Output Drive letter." << endl;
			return FALSE;
		}
		g_wsOutputDrive = wsCmdLine.substr (0, szOffset);
		wsCmdLine = wsCmdLine.substr (szOffset + 1);
		g_wsDoMount = wsCmdLine;

		// synchronization event with server manager	
		g_wsSynchroEventReady += wsPortInputId;
		HANDLE hEventReady = ::CreateEventW(0, TRUE, FALSE, g_wsSynchroEventReady.c_str());

		MSG msg;
		HACCEL hAccelTable;
		HWND hWnd;

		INITCOMMONCONTROLSEX initCC;
		initCC.dwSize = sizeof (INITCOMMONCONTROLSEX);
		initCC.dwICC = ICC_WIN95_CLASSES;
		InitCommonControlsEx (&initCC);

		// Initialize global strings
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		RegisterClasses(hInstance);

		// Perform application initialization:
		if (!InitInstance (hInstance, nCmdShow, &hWnd, g_wsDoMount == L"Yes")){
			MessageBox (NULL, L"Failed to instanciate the main application window.", L"IGServer error", 0);	
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Failed to instanciate the main application window." << endl;
			return FALSE;
		}

		::OleInitialize(NULL);

		g_hStatus = ::CreateWindow (L"Static", L"", WS_CHILD | WS_VISIBLE,
										CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWnd, NULL, hInstance, NULL);

		g_hIGMainWindow = ::CreateWindow (IGMAINWINDOW_CLASS, IGMAINWINDOW_TITLE, WS_CHILD | WS_VISIBLE,
											CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hWnd, NULL, hInstance, NULL);

		CComPtr <IIGMainWindow>	spMainWindow;
		spMainWindow.CoCreateInstance (CLSID_IGMainWindow);
		if (!spMainWindow)
		{
			MessageBox (NULL, L"Failed to instanciate IGPictureEditor.\nThe ActiveX is probably not registered.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Failed to instanciate IGPictureEditor" << endl;
			return FALSE;
		}

		wstring wsOutputDrivePath = g_wsOutputDrive + L"\\";
		if (FAILED (spMainWindow->CreateFrame ((OLE_HANDLE)g_hIGMainWindow, VARIANT_TRUE, CComBSTR (g_wsServerIP.c_str()), CComBSTR (wsOutputDrivePath.c_str()))))
		{
			MessageBox (NULL, L"Failed to initialize IGPictureEditor.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Failed to initialize IGPictureEditor." << endl;
			return FALSE;
		}

		if (FAILED (spMainWindow->Connect (g_nPortInputId, CComBSTR (g_wsServerIP.c_str()), (OLE_HANDLE)g_hStatus)))
		{
			MessageBox (NULL, L"Failed to setup socket communication.", L"IGServer error", 0);
			ofstream outfile;
			outfile.open (L"CriticalError.txt", ios_base::app);
			outfile << "Failed to setup socket communication." << endl;
			return FALSE;
		}

		// Synchronize the application now
		::SetEvent (hEventReady);

		// Move + Resize the main window
		HMONITOR hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize=sizeof(MONITORINFO);
		::GetMonitorInfo(hMonitor,&mi);

		RECT wrk = mi.rcWork;
		int wrkw = wrk.right - wrk.left;
		int wrkh = wrk.bottom - wrk.top;

		int x = wrk.left + (wrkw - WINDOW_WIDTH) / 2;
		int y = wrk.top  + (wrkh - WINDOW_HEIGHT) / 2;

		if (x < wrk.left)     
			x = wrk.left;
		if (y < wrk.top)      
			y = wrk.top;
		if (x + WINDOW_WIDTH > wrk.right)  
			x = wrk.right - WINDOW_WIDTH;
		if (y + WINDOW_HEIGHT > wrk.bottom) 
			y = wrk.bottom - WINDOW_HEIGHT;

	#ifdef NDEBUG
		// in release, hide the application window
		::SetWindowPos (hWnd, HWND_TOP, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_HIDEWINDOW);
	#else
		::SetWindowPos (hWnd, HWND_TOP, x, y, WINDOW_WIDTH, WINDOW_HEIGHT, SWP_SHOWWINDOW);
	#endif

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IGSERVER));

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		
		spMainWindow->DestroyFrame();
		spMainWindow.Detach()->Release();

		::OleUninitialize();

		return (int) msg.wParam;
	}
	catch(...){
		int nError = ::GetLastError();
		ofstream outfile;
		outfile.open (L"CriticalError.txt", ios_base::app);
		outfile << "An unhandled exception has been thrown. Last error code: " << endl;
		outfile << nError;
	}

	return 1;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
void RegisterClasses(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	if (::GetClassInfoEx(hInstance, IGSERVER_CLASS, &wcex) && ::GetClassInfoEx(hInstance, IGMAINWINDOW_CLASS, &wcex))
		return;
	
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IGSERVER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= IGSERVER_CLASS;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!::RegisterClassEx(&wcex)){
		int nError = ::GetLastError();
	    ofstream outfile;
		outfile.open (L"CriticalError.txt", ios_base::app);
		outfile << "RegisterClassEx of IGServer failed with error code: ";
		outfile << nError << endl;
		outfile << "wcex.hInstance: " << wcex.hInstance << endl;
		outfile << "wcex.hIcon: " << wcex.hIcon << endl;
		outfile << "wcex.hCursor: " << wcex.hCursor << endl;
		outfile << "wcex.hbrBackground: " << wcex.hbrBackground << endl;
		char tStr[128];
		size_t nbC;
		::wcstombs_s (&nbC, tStr, wcex.lpszClassName, ::wcslen(wcex.lpszClassName) + 1);
		outfile << "wcex.lpszClassName: " << tStr << endl;
		outfile << "wcex.hIconSm: " << wcex.hIconSm << endl;
	}

	wcex.lpfnWndProc	= WndProcIGMain;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= IGMAINWINDOW_CLASS;
	if (!::RegisterClassEx(&wcex)){
		int nError = ::GetLastError();
	    ofstream outfile;
		outfile.open (L"CriticalError.txt", ios_base::app);
		outfile << "RegisterClassEx of IGMainWindow failed with error code: ";
		outfile << nError << endl;
		outfile << "wcex.hInstance: " << wcex.hInstance << endl;
		outfile << "wcex.hIcon: " << wcex.hIcon << endl;
		outfile << "wcex.hCursor: " << wcex.hCursor << endl;
		outfile << "wcex.hbrBackground: " << wcex.hbrBackground << endl;
		char tStr[128];
		size_t nbC;
		::wcstombs_s (&nbC, tStr, wcex.lpszClassName, ::wcslen(wcex.lpszClassName) + 1);
		outfile << "wcex.lpszClassName: " << tStr << endl;
		outfile << "wcex.hIconSm: " << wcex.hIconSm << endl;
	}
}

BOOL MountDrive()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	// we want its console window to be invisible to the user.
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	wstring wsArgs (L"use ");
	wsArgs += g_wsOutputDrive;
	wsArgs += L" \\\\";
	wsArgs += g_wsOutputIP + L"\\";
	wsArgs += IGSERVER_OUTPUTFILES_FOLDERHEADER;
	wsArgs += g_wsServerIP;	
	
	wstring wsCmd (L"net ");	
	wsCmd += wsArgs;
	if (wsCmd.length() >= _MAX_PATH)
		return FALSE;
	wchar_t twCmd[_MAX_PATH];
	ZeroMemory (twCmd, _MAX_PATH * sizeof (wchar_t));
	::wcscpy_s (twCmd, _MAX_PATH, wsCmd.c_str());
	return ::CreateProcessW (NULL, twCmd, NULL, NULL, FALSE,
		 NORMAL_PRIORITY_CLASS, NULL, L"\\", &si, &pi);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND *p_hWnd, BOOL bMountDrive)
{
   hInst = hInstance; // Store instance handle in our global variable

   *p_hWnd = CreateWindow(IGSERVER_CLASS, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!(*p_hWnd))
   {
	    int nError = ::GetLastError();
	    ofstream outfile;
		outfile.open (L"CriticalError.txt", ios_base::app);
		outfile << "CreateWindow failed with error code: ";
		outfile << nError << endl;
		return FALSE;
   }

#ifdef _DEBUG
	// in release, hide the application window
   ShowWindow(*p_hWnd, nCmdShow);
   UpdateWindow(*p_hWnd);
#endif

	return bMountDrive ? MountDrive() : TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		{
			WINDOWINFO wInfo;
			::GetWindowInfo (hWnd, &wInfo);
			RECT rcWin;
			rcWin.left = 0;//(wInfo.rcClient.left - wInfo.rcWindow.left);
			rcWin.right = rcWin.left + (wInfo.rcClient.right - wInfo.rcClient.left);
			rcWin.top = 0;//(wInfo.rcClient.top - wInfo.rcWindow.top);
			rcWin.bottom = rcWin.top + (wInfo.rcClient.bottom - wInfo.rcClient.top);
			::SetWindowPos (g_hIGMainWindow, HWND_TOP, rcWin.left, rcWin.top,
				rcWin.right - rcWin.left, rcWin.bottom - rcWin.top - STATUS_HEIGHT, SWP_SHOWWINDOW);
			::SetWindowPos (g_hStatus, HWND_TOP, rcWin.left, rcWin.bottom -  STATUS_HEIGHT,
				rcWin.right - rcWin.left, STATUS_HEIGHT, SWP_SHOWWINDOW);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK WndProcIGMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
