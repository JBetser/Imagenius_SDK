#include "stdafx.h"
#include "MainWindow.h"
#include "Format.h"
#include "IGHistoryManager.h"
#include <IGLibrary.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace IGLibrary;

#define FONT_ATHENA_NAME					L"Athena Unicode"
#define FONT_IGUI_SIZE						16
#define FONT_CURSIF_NAME					L"Cursif"
#define FONT_MESSAGE_SIZE					20
#define TABBAR_POSX							22
#define TABBAR_POSY							3
#define TABBAR_WIDTH						140
#define TAB_HISTORY_WIDTH					60

bool CMainWindow::initAll ()
{
	// this ActiveX must be a singleton, if an instance exists, leave !
	if (IGLibrary::GetInstance())
		return false;

	// This initializes IGLibrary and splashscreen
	IGLibrary::Init (getInstance());

	if (!initConfiguration())
		return false;	// stop the application if configuration could not start
	
	Format::RegisterFormats();

	// initialize gdiplus
	GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup (&m_gdiplusToken, &gdiplusStartupInput, NULL);

	initFonts ();

	// new site coordinates
	RECT rc;
	::ZeroMemory (&rc, sizeof (RECT));
	GetWindowRect(&rc);
	int nMainWindowHeight = rc.bottom - rc.top;

	// image library, it is a popup so coordinates are absolute
	rc.top += IGMENUBAR_HEIGHT;
	rc.bottom += IGMENUBAR_HEIGHT;
	m_spImageLibrary.CoCreateInstance (CLSID_IGImageLibrary);
	CComQIPtr <IOleObject> spImageLibOleObject (m_spImageLibrary);
	spImageLibOleObject->SetClientSite ((IOleClientSite *)this);
	m_spImageLibrary->put_Font ((OLE_HANDLE)m_hFontUI);
	HRESULT hr = m_spImageLibrary->CreateFrame  ((OLE_HANDLE)m_hWnd, &rc);
	if (FAILED (hr))
		return false;
	IDispEventSimpleImpl <1, CMainWindow, &__uuidof(_IIGImageLibraryEvents)>::DispEventAdvise (m_spImageLibrary);

	// following object are children so coordinates are window-relative
	rc.right -= rc.left;
	rc.bottom -= rc.top;

	// load main menu bar configuration
	rc.top = 0;
	rc.bottom = IGMENUBAR_HEIGHT;
	char *pValidXml = IGConfigManager::CreateBufferFromRes (getInstance(), MAKEINTRESOURCE (IDR_MENUBARCONFIG), L"Configuration");
	// create main menu bar
	m_spMenuBar.CoCreateInstance (CLSID_IGMenuBar);
	CComQIPtr <IOleObject> spMenuBarOleObject (m_spMenuBar);
	spMenuBarOleObject->SetClientSite ((IOleClientSite *)this);
	m_spMenuBar->put_Font ((OLE_HANDLE)m_hFontUI);
	hr = m_spMenuBar->CreateFrame ((OLE_HANDLE)m_hWnd, &rc, CComBSTR (pValidXml));
	if (FAILED (hr))
		return false;	
	delete [] pValidXml;

	// add dockpanels to main window	
	rc.top = IGMENUBAR_HEIGHT;	
	rc.bottom = nMainWindowHeight;
	m_spDockPanelLeft.CoCreateInstance (CLSID_IGDockPanel);
	CComQIPtr <IOleObject> spDockPanelLeftOleObject (m_spDockPanelLeft);
	spDockPanelLeftOleObject->SetClientSite ((IOleClientSite *)this);
	hr = m_spDockPanelLeft->CreateFrame  ((OLE_HANDLE)m_hWnd, &rc, IGDOCKPANELSIDE_LEFT, 60);
	if (FAILED (hr))
		return false;
	m_spDockPanelRight.CoCreateInstance (CLSID_IGDockPanel);
	CComQIPtr <IOleObject> spDockPanelRightOleObject (m_spDockPanelRight);
	spDockPanelRightOleObject->SetClientSite ((IOleClientSite *)this);
	hr = m_spDockPanelRight->CreateFrame  ((OLE_HANDLE)m_hWnd, &rc, IGDOCKPANELSIDE_RIGHT, 150, WS_CLIPCHILDREN);
	if (FAILED (hr))
		return false;
	m_spDockPanelBottom.CoCreateInstance (CLSID_IGDockPanel);
	CComQIPtr <IOleObject> spDockPanelBottomOleObject (m_spDockPanelBottom);
	spDockPanelBottomOleObject->SetClientSite ((IOleClientSite *)this);
	hr = m_spDockPanelBottom->CreateFrame  ((OLE_HANDLE)m_hWnd, &rc, IGDOCKPANELSIDE_BOTTOM, 90);
	if (FAILED (hr))
		return false;
	m_spDockPanelLeft->SetString (L"TOOLBOX");
	m_spDockPanelRight->SetString (L"LAYERS");
	m_spDockPanelBottom->SetString (L"WORKSPACE");

	OLE_HANDLE hLeftPanel;
	RECT rcLeftPanel;
	m_spDockPanelLeft->GetHWND (&hLeftPanel);
	m_spDockPanelLeft->GetClientRECT (&rcLeftPanel);
	m_spToolBox.CoCreateInstance (CLSID_IGToolBox);
	m_spToolBox->Create (hLeftPanel, &rcLeftPanel);

	// layer manager
	m_spLayerManager.CoCreateInstance (CLSID_IGLayerManager);
	m_spLayerManager->CreateFrame (m_spDockPanelRight);

	// tab bar container (history)
	HWND hWndRightPanel;
	m_spDockPanelRight->GetHWND ((OLE_HANDLE *)&hWndRightPanel);
	m_spTabBar = new IGLibrary::IGTabBar (hWndRightPanel, TABBAR_POSX, TABBAR_POSY, TABBAR_WIDTH, (nMainWindowHeight - IGMENUBAR_HEIGHT - IGDOCKPANEL_DOCKEDSIZE + TABBAR_HEIGHTOFFSET) / 2,
											RGB (IGDOCKPANEL_COLOR_BACKGROUND.GetR(), IGDOCKPANEL_COLOR_BACKGROUND.GetG(), IGDOCKPANEL_COLOR_BACKGROUND.GetB()));

	// history manager
	HWND hTabBar;
	m_spTabBar->getHWND (&hTabBar);
	m_spHistoryManager.CoCreateInstance (CLSID_IGHistoryManager);
	CComQIPtr <IOleObject> spHistoryManagerOleObject (m_spHistoryManager);
	spHistoryManagerOleObject->SetClientSite ((IOleClientSite *)this);
	m_spHistoryManager->CreateFrame ((OLE_HANDLE)hTabBar, m_spDockPanelRight);	
	CComQIPtr <IOleWindow> spWindow (m_spHistoryManager);
	if (!spWindow)
		return false;
	HWND hWndHistory;
	spWindow->GetWindow (&hWndHistory);

	// create tabs
	m_spTabBar->AddTab (hWndHistory, L"History", TAB_HISTORY_WIDTH);
	m_spTabBar->AddTab (NULL, L"Tab2");
	m_spTabBar->AddTab (NULL, L"Tab3");
	m_spTabBar->Show();

	// multi frame object
	rc.left = 0;
	rc.top = IGMENUBAR_HEIGHT;
	rc.bottom -= rc.top;
	m_spMultiFrame.CoCreateInstance (CLSID_IGMultiFrame);
	m_spWorkspace.CoCreateInstance (CLSID_IGWorkspace);
	CComQIPtr <IOleObject> spMultiFrameOleObject (m_spMultiFrame);
	CComQIPtr <IOleObject> spMainWindowOleObject (this);
	spMultiFrameOleObject->SetClientSite ((IOleClientSite *)this);
	hr = m_spMultiFrame->CreateFrame ((OLE_HANDLE)m_hWnd, &rc, m_spLayerManager, m_spWorkspace, m_spToolBox);
	if (FAILED (hr))
		return false;

	// transfer drop management from mainmenu and dock panel to multiframe
	IDropTarget *pMultiFrameTarget;
	if (SUCCEEDED (m_spMultiFrame->GetDropTarget (&pMultiFrameTarget)) && pMultiFrameTarget)
	{
		m_spMenuBar->AddDropTarget (pMultiFrameTarget);
		m_spDockPanelLeft->AddDropTarget (pMultiFrameTarget);
		m_spDockPanelRight->AddDropTarget (pMultiFrameTarget);
		m_spDockPanelBottom->AddDropTarget (pMultiFrameTarget);
	}

	// workspace	
	m_spWorkspace->CreateFrame (m_spDockPanelBottom, m_spMultiFrame);

	return true;
}

void CMainWindow::initFonts ()
{
	m_hFontAthena =	installFont (getInstance(), IDR_FONTS_ATHENA);
	m_hFontCursif =	installFont (getInstance(), IDR_FONTS_CURSIF);

	// Create base font
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

	// Create imagenius user-interface font	
	::lstrcpyW (lf.lfFaceName, FONT_ATHENA_NAME);
	lf.lfHeight = - FONT_IGUI_SIZE;
	lf.lfWeight = FW_NORMAL;
	m_hFontUI = ::CreateFontIndirectW (&lf);

	// Create imagenius message font
	::lstrcpyW (lf.lfFaceName, FONT_CURSIF_NAME);
	lf.lfHeight = - FONT_MESSAGE_SIZE;
	lf.lfWeight = FW_NORMAL;
	m_hFontMessage = ::CreateFontIndirectW (&lf);
}

HANDLE CMainWindow::installFont (HINSTANCE hInstance, int numResFont)
{
	HRSRC hResFontBold = ::FindResourceW (hInstance, MAKEINTRESOURCE (numResFont), L"FONTS");
	HGLOBAL hGlbFontBold = ::LoadResource (hInstance, hResFontBold);
	size_t szFontData = ::SizeofResource (hInstance, hResFontBold);
	LPVOID pvFontData = ::LockResource (hGlbFontBold);
	DWORD nbFonts = 0;
	HANDLE hLoadedFont = ::AddFontMemResourceEx (pvFontData, szFontData, 0, &nbFonts);
	if (nbFonts == 0)
		return NULL;
	return hLoadedFont;
}