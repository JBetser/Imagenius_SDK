#include "stdafx.h"
#include "IGMainWindow.h"
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

HWND CIGMainWindow::getHWNDfromOLE (IDispatch* pDisp)
{
	CComQIPtr <IOleWindow> spWindow (pDisp);
	if (!spWindow)
		return false;
	HWND hWndHistory;
	spWindow->GetWindow (&hWndHistory);
	spWindow.Detach();
	return hWndHistory;
}

bool CIGMainWindow::initAll ()
{
	// this ActiveX must be a singleton, if an instance exists, leave !
	if (IGLibrary::GetInstance())
		return false;

	// This initializes IGLibrary and splashscreen
	IGLibrary::Init (getInstance());

	if (m_spConfigMgr->IsServerConfig())	// in server mode configuration is managed dynamically
	{
		if (FAILED (m_spConfigMgr->LoadServerConfiguration (getInstance(),
															MAKEINTRESOURCE(IDR_DEEPZOOMCONFIG))))
			return false;	// stop the application if configuration could not start
	}
	else
	{
		if (FAILED (m_spConfigMgr->LoadConfiguration (getInstance(),
											MAKEINTRESOURCE(IDR_IGPICTUREEDITORCONFIG))))
			return false;	// stop the application if configuration could not start
	}
	
	Format::RegisterFormats();

	// initialize gdiplus
	GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup (&m_gdiplusToken, &gdiplusStartupInput, NULL);
	initFonts ();

	// load face effect textures
	IGFaceEffect::Init (::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_BLOOD1), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_BLOOD2), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_BLOOD3), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_BLOOD4), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_TEARS1), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_TEARS2), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_TEARS3), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_POX1), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_POX2), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_STAR1), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_STAR2), L"PNG"));

	// load filter effect textures
	IGSmartLayer::Init (::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_PAPER), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_FILTER2_HALO), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_FILTER3_BRUSH), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_FILTER4_SCREEN), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_FILTER4_OVERLAY), L"PNG"),
						::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_FILTER4_BRUSH), L"PNG"));

	// new site coordinates
	RECT rc;
	::ZeroMemory (&rc, sizeof (RECT));
	::GetWindowRect (m_hWnd, &rc);
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
	IDispEventSimpleImpl <1, CIGMainWindow, &__uuidof(_IIGImageLibraryEvents)>::DispEventAdvise (m_spImageLibrary);

	// following object are children so coordinates are window-relative
	rc.right -= rc.left;
	rc.bottom -= rc.top;

	// load main menu bar configuration
	rc.top = 0;
	rc.bottom = IGMENUBAR_HEIGHT;
	char *pValidXml = IGConfigManager::CreateBufferFromRes (getInstance(), MAKEINTRESOURCE (IDR_MENUBARCONFIG), L"Configuration");

#ifndef IGPICTUREEDITOR_SERVER
	// create main menu bar
	m_spMenuBar.CoCreateInstance (CLSID_IGMenuBar);
	CComQIPtr <IOleObject> spMenuBarOleObject (m_spMenuBar);
	spMenuBarOleObject->SetClientSite ((IOleClientSite *)this);
	m_spMenuBar->put_Font ((OLE_HANDLE)m_hFontUI);
	hr = m_spMenuBar->CreateFrame ((OLE_HANDLE)m_hWnd, &rc, CComBSTR (pValidXml));
	if (FAILED (hr))
		return false;
#endif
	delete [] pValidXml;

	// add dockpanels to main window	
	rc.top = 0;	
	rc.bottom = nMainWindowHeight;
#ifndef IGPICTUREEDITOR_SERVER
	rc.top = IGMENUBAR_HEIGHT;	
	m_spDockPanelLeft.CoCreateInstance (CLSID_IGDockPanel);
	CComQIPtr <IOleObject> spDockPanelLeftOleObject (m_spDockPanelLeft);
	spDockPanelLeftOleObject->SetClientSite ((IOleClientSite *)this);
	hr = m_spDockPanelLeft->CreateFrame  ((OLE_HANDLE)m_hWnd, &rc, IGDOCKPANELSIDE_LEFT, 60);
	if (FAILED (hr))
		return false;
#endif
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
#ifndef IGPICTUREEDITOR_SERVER
	m_spDockPanelLeft->SetString (L"TOOLBOX");
#endif
	m_spDockPanelRight->SetString (L"LAYERS");
	m_spDockPanelBottom->SetString (L"WORKSPACE");

#ifndef IGPICTUREEDITOR_SERVER
	OLE_HANDLE hLeftPanel;
	RECT rcLeftPanel;
	m_spDockPanelLeft->GetHWND (&hLeftPanel);
	m_spDockPanelLeft->GetClientRECT (&rcLeftPanel);
	m_spToolBox.CoCreateInstance (CLSID_IGToolBox);
	m_spToolBox->Create (hLeftPanel, &rcLeftPanel);
#endif

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
	HWND hWndHistory = getHWNDfromOLE(m_spHistoryManager);

	// frame property manager
	m_spFramePropertyMgr.CoCreateInstance (CLSID_IGFramePropertyManager);
	CComQIPtr <IOleObject> spFramePropertyMgrOleObject (m_spFramePropertyMgr);
	spFramePropertyMgrOleObject->SetClientSite ((IOleClientSite *)this);
	m_spFramePropertyMgr->CreateFrame ((OLE_HANDLE)hTabBar, m_spDockPanelRight);
	HWND hWndFrameProperties = getHWNDfromOLE(m_spFramePropertyMgr);

	// property manager
	m_spPropertyMgr.CoCreateInstance (CLSID_IGPropertyManager);
	CComQIPtr <IOleObject> spPropertyMgrOleObject (m_spPropertyMgr);
	spPropertyMgrOleObject->SetClientSite ((IOleClientSite *)this);
	m_spPropertyMgr->CreateFrame ((OLE_HANDLE)hTabBar, m_spDockPanelRight);
	HWND hWndProperties = getHWNDfromOLE(m_spPropertyMgr);

	// create tabs
	m_spTabBar->AddTab (hWndHistory, L"History", TAB_HISTORY_WIDTH);
	m_spTabBar->AddTab (hWndFrameProperties, L"Frame", TAB_HISTORY_WIDTH);
	m_spTabBar->AddTab (hWndProperties, L"General", TAB_HISTORY_WIDTH);
	m_spTabBar->Show();

	// multi frame object
	rc.left = 0;
	rc.top = IGMENUBAR_HEIGHT;
	rc.bottom -= rc.top;
	m_spMultiFrame.CoCreateInstance (CLSID_IGMultiFrame);
	m_spWorkspace.CoCreateInstance (CLSID_IGWorkspace);
	CComQIPtr <IOleObject> spMultiFrameOleObject (m_spMultiFrame);
	spMultiFrameOleObject->SetClientSite ((IOleClientSite *)this);
	hr = m_spMultiFrame->CreateFrame ((OLE_HANDLE)m_hWnd, &rc, m_spLayerManager, m_spWorkspace, m_spToolBox, m_spPropertyMgr);
	if (FAILED (hr))
		return false;

	// transfer drop management from mainmenu and dock panel to multiframe
	IDropTarget *pMultiFrameTarget;
	if (SUCCEEDED (m_spMultiFrame->GetDropTarget (&pMultiFrameTarget)) && pMultiFrameTarget)
	{
#ifndef IGPICTUREEDITOR_SERVER
		m_spMenuBar->AddDropTarget (pMultiFrameTarget);
		m_spDockPanelLeft->AddDropTarget (pMultiFrameTarget);
#endif
		m_spDockPanelRight->AddDropTarget (pMultiFrameTarget);
		m_spDockPanelBottom->AddDropTarget (pMultiFrameTarget);
	}

	// workspace	
	m_spWorkspace->CreateFrame (m_spDockPanelBottom, m_spMultiFrame);

	return true;
}

void CIGMainWindow::initFonts ()
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

HANDLE CIGMainWindow::installFont (HINSTANCE hInstance, int numResFont)
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