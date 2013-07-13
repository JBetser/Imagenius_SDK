// IGImageLibrary.cpp : Implementation of CIGImageLibrary
#include "stdafx.h"
#include "IGImageLibrary.h"
#include <shlobj.h>
#include <IGLibPicture.h>
#include <IGSplashProxy.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace IGLibrary;

#define IGIMAGELIBRARY_WIDTH			480
#define IGIMAGELIBRARY_HEIGHT			440
#define IGIMAGELIBRARY_MENUBARHEIGHT	25
#define IGIMAGELIBRARY_BORDERWIDTH		15
#define IGIMAGELIBRARY_CORNERRADIUS		(IGIMAGELIBRARY_BORDERWIDTH * 2)
#define IGIMAGELIBRARY_CORNERDIAM		(IGIMAGELIBRARY_CORNERRADIUS * 2)

// ListBox parameters
#define IGIMAGELIBRARY_LISTBOXWIDTH			(IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH * 2)
#define IGIMAGELIBRARY_LISTBOXHEIGHT		(IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_MENUBARHEIGHT - IGIMAGELIBRARY_BORDERWIDTH)
#define IGIMAGELIBRARY_ITEMWIDTH			90
#define IGIMAGELIBRARY_ITEMHEIGHT			90
#define IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR	(IGIMAGELIBRARY_ITEMHEIGHT - IGIMAGELIBRARY_MINIPICTUREHEIGHT)
#define IGIMAGELIBRARY_LISTBOXCORNERRADIUS	10

// Scrollbar parameters
#define IGIMAGELIBRARY_SCROLLBARWIDTH		120
#define IGIMAGELIBRARY_SCROLLBARHEIGHT		20
#define IGIMAGELIBRARY_SCROLLBARBUTTONWIDTH	(IGIMAGELIBRARY_SCROLLBARWIDTH / 6)

// Color parameters
#define IGIMAGELIBRARY_COLOR_BORDERLEFT			Color(255, 52, 148, 214)	// Blue
#define IGIMAGELIBRARY_COLOR_BORDERRIGHT		Color(255, 97, 190, 132)	// Green
#define IGIMAGELIBRARY_COLOR_FRAMEIN			Color(255, 221, 239, 251)	// White
#define IGIMAGELIBRARY_COLOR_FRAMEOUT			Color(255, 157, 209, 244)	// Light Blue
#define IGIMAGELIBRARY_COLOR_SELECTITEM			Color(150, 97, 190, 132)	// Green Mask
#define IGIMAGELIBRARY_COLOR_BORDERSELECTITEM	Color(255, 97, 190, 132)	// Green

int CIGImageLibrary::g_nNbItemsPerColumn = ((IGIMAGELIBRARY_LISTBOXHEIGHT) / (IGIMAGELIBRARY_ITEMHEIGHT));
int CIGImageLibrary::g_nNbColumnsPerPage = ((IGIMAGELIBRARY_LISTBOXWIDTH) / (IGIMAGELIBRARY_ITEMWIDTH));

// _IIGImageLibraryEvents
_ATL_FUNC_INFO EventPanelOpen = {CC_CDECL, VT_EMPTY, 0, 0};

// CIGImageLibrary
CIGImageLibrary::CIGImageLibrary() : m_hListBox (NULL)
									, m_hRegion (NULL)
									, m_nCurrentScrollPos (0)
									, m_nMaxPos (0)
									, m_pnSelectedItems (NULL)
									, m_nNbSelectedItems (0)
									, m_nIdToSendBack (-1)
									, m_hParent (NULL)
									, m_hFont (NULL)
{
	m_pThreadAddPicture = new IGAddPictures();	
	m_pThreadAddPicture->Start ();	
}

CIGImageLibrary::~CIGImageLibrary()
{
	m_pThreadAddPicture->Exit ();
	delete m_pThreadAddPicture;
	if (m_pnSelectedItems)
		delete [] m_pnSelectedItems;
}

STDMETHODIMP CIGImageLibrary::CreateFrame(OLE_HANDLE hWnd, LPRECT prc)
{
	if (!prc || !hWnd)
		return E_INVALIDARG;
	RECT rcImageLibrary;
	rcImageLibrary.left = prc->left;
	rcImageLibrary.top = prc->top;
	rcImageLibrary.right = rcImageLibrary.left + IGIMAGELIBRARY_WIDTH;
	rcImageLibrary.bottom = rcImageLibrary.top + IGIMAGELIBRARY_HEIGHT;
	_U_RECT urc (&rcImageLibrary);
	HWND hImageLibrary = CComControl<CIGImageLibrary>::Create ((HWND)hWnd, urc, L"ImageLibrary", WS_POPUP);
	if (!hImageLibrary)
		return E_FAIL;
	m_hParent = (HWND)hWnd;

	HRGN hTopRgn = ::CreateRectRgn (0, 0, IGIMAGELIBRARY_WIDTH, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS);
	HRGN hBottomRgn = ::CreateRectRgn (IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS
									, IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_HEIGHT);
	HRGN hLeftCorner = ::CreateRoundRectRgn (0, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM + 1,
											IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_HEIGHT + 1, 
											IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_CORNERDIAM);
	HRGN hRightCorner = ::CreateRoundRectRgn (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERDIAM + 1, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM + 1,
											IGIMAGELIBRARY_WIDTH + 1, IGIMAGELIBRARY_HEIGHT + 1, 
											IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_CORNERDIAM);
	m_hRegion = hTopRgn;
	::CombineRgn (m_hRegion, hTopRgn, hBottomRgn, RGN_OR);
	::CombineRgn (m_hRegion, m_hRegion, hLeftCorner, RGN_OR);
	::CombineRgn (m_hRegion, m_hRegion, hRightCorner, RGN_OR);
	::SetWindowRgn (hImageLibrary, m_hRegion, TRUE);
	::DeleteObject (hBottomRgn);
	::DeleteObject (hLeftCorner);
	::DeleteObject (hRightCorner);

	// menu bar
	m_spMenuBar.CoCreateInstance (CLSID_IGMenuBar);
	CComPtr <IOleClientSite> spOleClientSite;
	IOleObjectImpl <CIGImageLibrary>::GetClientSite (&spOleClientSite);
	CComQIPtr <IOleObject> spOleObject (m_spMenuBar);
	spOleObject->SetClientSite (spOleClientSite);
	RECT rcMenuBar;
	rcMenuBar.top = 0;
	rcMenuBar.left = IGIMAGELIBRARY_BORDERWIDTH;
	rcMenuBar.right = IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH;
	rcMenuBar.bottom = IGIMAGELIBRARY_MENUBARHEIGHT;
	char *pValidXml = IGConfigManager::CreateBufferFromRes (getInstance(), MAKEINTRESOURCE (IDR_IMAGELIBRARYCONFIG), L"Configuration");
	m_spMenuBar->put_Font ((OLE_HANDLE)m_hFont);
	m_spMenuBar->CreateFrame ((OLE_HANDLE)m_hWnd, &rcMenuBar, CComBSTR (pValidXml));
	delete [] pValidXml;

	// scroll bar
	m_spScrollBar.CoCreateInstance (CLSID_IGMenuBar);
	CComQIPtr <IOleObject> spScrollOleObject (m_spScrollBar);
	spScrollOleObject->SetClientSite (spOleClientSite);
	RECT rcScrollBar;
	rcScrollBar.top = IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_BORDERWIDTH - IGIMAGELIBRARY_SCROLLBARHEIGHT;
	rcScrollBar.left = IGIMAGELIBRARY_WIDTH / 2 - IGIMAGELIBRARY_SCROLLBARWIDTH / 2;
	rcScrollBar.right = rcScrollBar.left + IGIMAGELIBRARY_SCROLLBARWIDTH;
	rcScrollBar.bottom = rcScrollBar.top + IGIMAGELIBRARY_SCROLLBARHEIGHT;
	pValidXml = IGConfigManager::CreateBufferFromRes (getInstance(), MAKEINTRESOURCE (IDR_IMAGELIBRARYSCROLLCONFIG), L"Configuration");
	m_spScrollBar->put_Font ((OLE_HANDLE)m_hFont);
	m_spScrollBar->CreateFrame ((OLE_HANDLE)m_hWnd, &rcScrollBar, CComBSTR (pValidXml));
	delete [] pValidXml;

	m_hListBox = ::CreateWindowExW (WS_EX_LEFT, L"LISTBOX", L"",
									WS_CHILD | WS_VISIBLE |
									LBS_MULTICOLUMN  | LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_NOREDRAW | LBS_NOINTEGRALHEIGHT,
									IGIMAGELIBRARY_BORDERWIDTH, IGIMAGELIBRARY_MENUBARHEIGHT,
									IGIMAGELIBRARY_LISTBOXWIDTH, IGIMAGELIBRARY_LISTBOXHEIGHT,
									m_hWnd,	NULL, getInstance(), NULL);
	::SendMessageW (m_hListBox, (UINT) LB_SETCOLUMNWIDTH, (WPARAM) IGIMAGELIBRARY_ITEMWIDTH, 0 );
	::SetWindowSubclass (m_hListBox, StaticHookListBoxProc, 0, (DWORD_PTR)this);
	populateListBox ();

	// Progress bar
	m_spProgress = new IGSplashWindow (m_hWnd);
	m_spProgress->ProgressSetMode (IGLibrary::SPLASHSCREEN_PROGRESSBAR_PROGRESSION);

	return S_OK;
}

STDMETHODIMP CIGImageLibrary::ShowFrame (LPRECT prc, LONG nId)
{
	if (!prc)
		return E_FAIL;
	m_nIdToSendBack = nId;	
	::MoveWindow (m_hWnd, prc->left, prc->bottom, IGIMAGELIBRARY_WIDTH, IGIMAGELIBRARY_HEIGHT, TRUE);
	::ShowWindow (m_hWnd, SW_SHOW);
	bool bIsSplashWorking = false;
	m_pThreadAddPicture->IsWorking (&bIsSplashWorking);
	if (bIsSplashWorking)
		m_spProgress->Show (true);
	return S_OK;
}

STDMETHODIMP CIGImageLibrary::DestroyFrame()
{	
	if (!m_hWnd)
		return S_OK;

	::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
	::DestroyWindow (m_hListBox);

	if (m_spMenuBar)
	{
		m_spMenuBar->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_spMenuBar);
		spOleObject->SetClientSite (NULL);
	}

	if (m_spScrollBar)
	{
		m_spScrollBar->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_spScrollBar);
		spOleObject->SetClientSite (NULL);
	}

	::DeleteObject (m_hRegion);
	DestroyWindow();
	m_hWnd = NULL;
	return S_OK;
}

STDMETHODIMP CIGImageLibrary::put_Font(OLE_HANDLE hFont)
{
	m_hFont = (HFONT)hFont;
	return S_OK;
}

HRESULT CIGImageLibrary::OnDraw(ATL_DRAWINFO& di)
{
	Graphics graphics (di.hdcDraw);
	// Middle part
	GraphicsPath path;
	path.AddRectangle (RectF (0, 0,
							IGIMAGELIBRARY_WIDTH, IGIMAGELIBRARY_HEIGHT));
	PathGradientBrush pthGrBrush (&path);
	pthGrBrush.SetCenterPoint (PointF (0.7f * (float)IGIMAGELIBRARY_WIDTH,
									  0.3f * (float)IGIMAGELIBRARY_HEIGHT));
	pthGrBrush.SetCenterColor (IGIMAGELIBRARY_COLOR_FRAMEIN);
	Color colors[] = {IGIMAGELIBRARY_COLOR_FRAMEOUT};
	int count = 1;
	pthGrBrush.SetSurroundColors(colors, &count);
	graphics.FillPath (&pthGrBrush, &path);

	// Border part
	GraphicsPath pathBorder;
	pathBorder.StartFigure();
	pathBorder.AddLine (Point (0,-50), Point (0,IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS));
	pathBorder.AddArc(Rect (0, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM,
					IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_CORNERDIAM), 180.0f, -90.0f);
	pathBorder.AddLine (Point (IGIMAGELIBRARY_CORNERRADIUS,IGIMAGELIBRARY_HEIGHT), 
						Point (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_HEIGHT));
	pathBorder.AddArc(Rect (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM,
					IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_CORNERDIAM), 90.0f, -90.0f);
	pathBorder.AddLine (Point (IGIMAGELIBRARY_WIDTH,IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS), Point (IGIMAGELIBRARY_WIDTH,-50));

	pathBorder.AddLine (Point (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH,-50), Point (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH,IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS));
	pathBorder.AddArc(Rect (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERDIAM + IGIMAGELIBRARY_BORDERWIDTH, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM + IGIMAGELIBRARY_BORDERWIDTH,
					IGIMAGELIBRARY_CORNERDIAM - 2 * IGIMAGELIBRARY_BORDERWIDTH, IGIMAGELIBRARY_CORNERDIAM - 2 * IGIMAGELIBRARY_BORDERWIDTH), 0.0f, 90.0f);
	pathBorder.AddLine (Point (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_BORDERWIDTH),
						Point (IGIMAGELIBRARY_CORNERRADIUS,IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_BORDERWIDTH));
	pathBorder.AddArc(Rect (IGIMAGELIBRARY_BORDERWIDTH, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM + IGIMAGELIBRARY_BORDERWIDTH,
					IGIMAGELIBRARY_CORNERDIAM - 2 * IGIMAGELIBRARY_BORDERWIDTH, IGIMAGELIBRARY_CORNERDIAM - 2 * IGIMAGELIBRARY_BORDERWIDTH), 180.0f, -90.0f);
	pathBorder.AddLine (Point (IGIMAGELIBRARY_BORDERWIDTH,IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS), Point (IGIMAGELIBRARY_BORDERWIDTH,-50));
		
	SolidBrush solLeftBrush (IGIMAGELIBRARY_COLOR_BORDERRIGHT);	
	graphics.FillPath (&solLeftBrush, &pathBorder);

	PathGradientBrush pthGrBorderBrush (&pathBorder);
	pthGrBorderBrush.SetCenterPoint (Point (IGIMAGELIBRARY_WIDTH / 2, IGIMAGELIBRARY_HEIGHT / 2));
	Color colorsBottom[] = {Color (0, 255, 255, 255), 
								Color (150, 255, 255, 255),
								Color (0, 255, 255, 255)};
	REAL interpPositionsBottom[] = {   0.0f,
									   0.05f,
									   1.0f};
	pthGrBorderBrush.SetInterpolationColors (colorsBottom, interpPositionsBottom, 3);
	graphics.FillPath (&pthGrBorderBrush, &pathBorder);

	::RedrawWindow (m_hListBox, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	return S_OK;
}

LRESULT CIGImageLibrary::OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	if (lpMeasureItem)
	{
		lpMeasureItem->itemWidth = IGIMAGELIBRARY_ITEMWIDTH;
		lpMeasureItem->itemHeight = IGIMAGELIBRARY_ITEMHEIGHT;
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;	
}

LRESULT CIGImageLibrary::OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
	if (lpDrawItem && lpDrawItem->itemData)
	{
		drawListBoxItem (lpDrawItem->hDC, lpDrawItem->itemID);
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;
}

void CIGImageLibrary::drawListBoxItem (HDC hDC, int nItemId)
{
	RECT rcItem;
	::ZeroMemory (&rcItem, sizeof (RECT));
	::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)nItemId, (LPARAM)&rcItem);  
	if ((rcItem.right > 0) && (rcItem.left < IGIMAGELIBRARY_LISTBOXWIDTH))
	{
		HBITMAP hBmp = (HBITMAP)::SendMessageW (m_hListBox, (UINT)LB_GETITEMDATA, (WPARAM)nItemId, 0);  
		if (!hBmp)
			return;
		BITMAP bitmapInfo;
		::ZeroMemory (&bitmapInfo, sizeof (BITMAP));
		::GetObject (hBmp, sizeof (BITMAP), &bitmapInfo);
		HBITMAP hBmpItem = ::CreateCompatibleBitmap (hDC, 
														bitmapInfo.bmWidth, 
														bitmapInfo.bmHeight); 
		HBITMAP hOldBmp = (HBITMAP)::SelectObject (hDC, hBmpItem);

		HDC hdcCompatible = ::CreateCompatibleDC (hDC); 
		HGDIOBJ hOldCompatibleBmp = ::SelectObject (hdcCompatible, hBmp);
		int nItemPosX = (IGIMAGELIBRARY_ITEMWIDTH - bitmapInfo.bmWidth) / 2;
		int nItemPosY = (IGIMAGELIBRARY_ITEMHEIGHT - bitmapInfo.bmHeight) / 2;

		::BitBlt (hDC, 
				   rcItem.left + nItemPosX, rcItem.top + nItemPosY, 
				   bitmapInfo.bmWidth, bitmapInfo.bmHeight, 
				   hdcCompatible, 
				   0, 0, 
				   SRCCOPY);
		::SelectObject (hDC, hOldBmp);
		::SelectObject (hdcCompatible, hOldCompatibleBmp);
		::DeleteDC (hdcCompatible);
		::DeleteObject (hBmpItem);

		int nIsSelected = (int)::SendMessageW (m_hListBox, (UINT)LB_GETSEL, (WPARAM)nItemId, 0);  
		if (nIsSelected > 0)
		{
			Graphics graphics (hDC);
			SolidBrush selectItemBrush (IGIMAGELIBRARY_COLOR_SELECTITEM);
			int nBmpMaxSize = (bitmapInfo.bmWidth > bitmapInfo.bmHeight ? bitmapInfo.bmWidth : bitmapInfo.bmHeight) - 1;
			Rect rectItem (rcItem.left + (IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR) / 2
							, rcItem.top + (IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR) / 2
							, nBmpMaxSize, nBmpMaxSize);
			graphics.FillRectangle (&selectItemBrush, rectItem);
			Pen penBorder (IGIMAGELIBRARY_COLOR_BORDERSELECTITEM);
			graphics.DrawRectangle (&penBorder, rectItem);			
		}
	}
}

LRESULT CIGImageLibrary::OnDeleteItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDELETEITEMSTRUCT lpDeleteItem = (LPDELETEITEMSTRUCT) lParam;
	if (lpDeleteItem && lpDeleteItem->itemData)
	{
		::DeleteObject ((HBITMAP)lpDeleteItem->itemData);
		bHandled = TRUE;
		return 0;
	}
	bHandled = FALSE;
	return 1L;	
}

void CIGImageLibrary::updateCurrentScrollPos()
{
	int nNbColumns = m_spConfigMgr->GetNbPictures() / g_nNbItemsPerColumn + (m_spConfigMgr->GetNbPictures() % g_nNbItemsPerColumn ? 1 : 0);
	m_nMaxPos = nNbColumns - 5;
	m_nMaxPos = m_nMaxPos > 0 ? m_nMaxPos : 0;
	int nFirstItemVisible = ::SendMessage (m_hListBox, LB_GETTOPINDEX, 0, 0);
	m_nCurrentScrollPos = nFirstItemVisible / g_nNbItemsPerColumn;
	m_nCurrentScrollPos = m_nCurrentScrollPos > 0 ? m_nCurrentScrollPos : 0;
}

void CIGImageLibrary::updateListBoxScrollPos()
{
	::SendMessage (m_hListBox, WM_HSCROLL, MAKEWPARAM (SB_THUMBPOSITION, m_nCurrentScrollPos), NULL);
	redraw();
}

void CIGImageLibrary::redraw()
{
	RECT rcLibPictures;
	rcLibPictures.left = IGIMAGELIBRARY_BORDERWIDTH + (IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR) / 2;
	rcLibPictures.top = IGIMAGELIBRARY_MENUBARHEIGHT + (IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR) / 2;
	rcLibPictures.right = IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH - (IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR) / 2;
	rcLibPictures.bottom = IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_BORDERWIDTH - (IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR) / 2;
	::InvalidateRect (m_hWnd, &rcLibPictures, FALSE);
	::InvalidateRect (m_hListBox, NULL, FALSE);

}

LRESULT CIGImageLibrary::OnImageLibScrollBegin (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (m_nCurrentScrollPos)
	{
		m_nCurrentScrollPos = 0;
		updateListBoxScrollPos();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibScrollPageLeft (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	updateCurrentScrollPos();
	if (m_nCurrentScrollPos)
	{
		m_nCurrentScrollPos -= g_nNbColumnsPerPage;
		m_nCurrentScrollPos = (m_nCurrentScrollPos > 0 ? m_nCurrentScrollPos : 0);
		updateListBoxScrollPos();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibScrollLeft (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	updateCurrentScrollPos();
	if (m_nCurrentScrollPos)
	{
		m_nCurrentScrollPos --;
		m_nCurrentScrollPos = (m_nCurrentScrollPos > 0 ? m_nCurrentScrollPos : 0);
		updateListBoxScrollPos();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibScrollRight (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	updateCurrentScrollPos();
	if (m_nCurrentScrollPos < m_nMaxPos)
	{
		m_nCurrentScrollPos ++;
		m_nCurrentScrollPos = (m_nCurrentScrollPos < m_nMaxPos ? m_nCurrentScrollPos : m_nMaxPos);
		updateListBoxScrollPos();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibScrollPageRight (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	updateCurrentScrollPos();
	if (m_nCurrentScrollPos < m_nMaxPos)
	{
		m_nCurrentScrollPos += g_nNbColumnsPerPage;
		m_nCurrentScrollPos = (m_nCurrentScrollPos < m_nMaxPos ? m_nCurrentScrollPos : m_nMaxPos);
		updateListBoxScrollPos();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibScrollEnd (WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	updateCurrentScrollPos();
	if (m_nCurrentScrollPos < m_nMaxPos)
	{
		m_nCurrentScrollPos = m_nMaxPos;
		updateListBoxScrollPos();
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnKillFocus (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hParent = NULL;
	HWND hGreatParent = NULL;
	HWND hGreatGreatParent = NULL;
	if (wParam)
	{		
		hParent = ::GetParent ((HWND)wParam);
		hGreatParent = ::GetParent (hParent);
		hGreatGreatParent = ::GetParent (hGreatParent);
	}
	if (hParent != m_hWnd && hGreatParent != m_hWnd && hGreatGreatParent != m_hWnd)
	{
		hide ();
		bHandled = TRUE;
		return 0;		
	}	
	bHandled = FALSE;
	return 1;
}

LRESULT CIGImageLibrary::OnAddPictures (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::list <IGSmartPtr <IGLibPicture>> *plspPictures = (std::list <IGSmartPtr <IGLibPicture>> *)wParam;
	if (plspPictures)
	{
		for (std::list<IGSmartPtr <IGLibPicture>>::const_iterator iterPict = plspPictures->begin(); 
			iterPict != plspPictures->end();
			iterPict++)
		{
			m_spConfigMgr->AddPicture (*iterPict);
			addListBoxItem (*iterPict);
		}
		delete plspPictures;
	}
	m_spProgress->Hide();
	redraw();
	// enable buttons
	m_spMenuBar->EnableButton (IGM_IMAGELIB_ADD);
	m_spMenuBar->EnableButton (IGM_IMAGELIB_REMOVE);
	bHandled = TRUE;
	return 0;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGImageLibrary::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGImageLibrary* pThis = (CIGImageLibrary*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGImageLibrary::HookListBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool bRedrawFrame = false;
	switch(msg)
	{
	case WM_PAINT:
		{
			int nbItems = m_spConfigMgr->GetNbPictures();
			if (!nbItems)
				break;
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			for (int nIdxItem = 0; nIdxItem < nbItems; nIdxItem++)
				drawListBoxItem (hDC, nIdxItem);
			::EndPaint(hWnd, &ps);
			break;			
		}
		break;
	case WM_KILLFOCUS:
		::PostMessage (m_hWnd, msg, wParam, lParam);
		break;
	// hacked messages
	case WM_ERASEBKGND:
	case WM_MOUSEMOVE:
		return 1L;

		// avoid item selection while adding pictures
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:	
		{
			bool bIsAddingPictures = false;
			m_pThreadAddPicture->IsWorking (&bIsAddingPictures);
			if (bIsAddingPictures)
				return 1L;
			break;
		}

	case WM_LBUTTONDBLCLK:	
		{
			if (m_pnSelectedItems)
			{
				LONG nPathLen = 0;
				wchar_t *pwPathBuffer = NULL;
				for (int nIdxItem = 0; nIdxItem < m_nNbSelectedItems; nIdxItem++)
				{
					nPathLen = ::SendMessageW (m_hListBox, (UINT)LB_GETTEXTLEN, (WPARAM)m_pnSelectedItems[nIdxItem], 0);
					if (nPathLen > 0)
					{
						pwPathBuffer = new wchar_t [nPathLen + 1];	
						::ZeroMemory (pwPathBuffer, (nPathLen + 1) * sizeof(wchar_t));
						if (nPathLen == ::SendMessageW (m_hListBox, (UINT)LB_GETTEXT, (WPARAM)m_pnSelectedItems[nIdxItem], (LPARAM)pwPathBuffer))
							Fire_OnOpenImage (CComBSTR (pwPathBuffer));
						delete [] pwPathBuffer;
					}
				}
			}
			return 1L;
		}
	}
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc (hWnd, msg, wParam, lParam);	

	switch(msg)
	{
		case WM_LBUTTONUP:
		{
			// invalidate unselect previous selected item areas
			int nIdxItem;
			RECT rcItem;
			RECT rcImageLib;
			if (m_pnSelectedItems)
			{
				for (nIdxItem = 0; nIdxItem < m_nNbSelectedItems; nIdxItem++)
				{					
					::ZeroMemory (&rcItem, sizeof (RECT));
					::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)m_pnSelectedItems[nIdxItem], (LPARAM)&rcItem);  
					rcImageLib.left = rcItem.left + IGIMAGELIBRARY_BORDERWIDTH;
					rcImageLib.right = rcItem.right + IGIMAGELIBRARY_BORDERWIDTH;
					rcImageLib.top = rcItem.top + IGIMAGELIBRARY_MENUBARHEIGHT;
					rcImageLib.bottom = rcItem.bottom + IGIMAGELIBRARY_MENUBARHEIGHT;
					::InvalidateRect (m_hWnd, &rcImageLib, FALSE);
					::InvalidateRect (m_hListBox, &rcItem, FALSE);
				}
				delete [] m_pnSelectedItems;
				m_pnSelectedItems = NULL;
			}
			// invalidate new selected item areas
			m_nNbSelectedItems = ::SendMessageW (m_hListBox, (UINT)LB_GETSELCOUNT, 0, 0); 
			if (m_nNbSelectedItems)
			{				
				m_pnSelectedItems = new int[m_nNbSelectedItems];
				::SendMessageW (m_hListBox, (UINT)LB_GETSELITEMS, (WPARAM)m_nNbSelectedItems, (LPARAM)m_pnSelectedItems); 
			}
			if (m_pnSelectedItems)
			{				
				for (nIdxItem = 0; nIdxItem < m_nNbSelectedItems; nIdxItem++)
				{					
					::ZeroMemory (&rcItem, sizeof (RECT));
					::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)m_pnSelectedItems[nIdxItem], (LPARAM)&rcItem);  
					rcImageLib.left = rcItem.left + IGIMAGELIBRARY_BORDERWIDTH;
					rcImageLib.right = rcItem.right + IGIMAGELIBRARY_BORDERWIDTH;
					rcImageLib.top = rcItem.top + IGIMAGELIBRARY_MENUBARHEIGHT;
					rcImageLib.bottom = rcItem.bottom + IGIMAGELIBRARY_MENUBARHEIGHT;
					::InvalidateRect (m_hWnd, &rcImageLib, FALSE);
					::InvalidateRect (m_hListBox, &rcItem, FALSE);
				}
			}
		}
		break;
	}

	return lRes;
}

LRESULT CIGImageLibrary::OnImageLibAdd(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	OPENFILENAME ofn;
	::ZeroMemory (&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof (OPENFILENAME);
	ofn.hwndOwner = m_hWnd;
	ofn.hInstance = getInstance();
	ofn.lpstrFilter = L"All Image Files (*.ig;*.jpg;*.png;*.bmp;*.gif;*.ico;*.tiff)\0*.ig;*.jpg;*.png;*.bmp;*.gif;*.ico;*.tiff\0All Files\0*.*\0\0";
	ofn.nFilterIndex = 1;

	wchar_t pwUserPicturesPath [MAX_PATH];
	std::wstring wUserPicturesPath;
	m_spConfigMgr->GetUserPicturesPath (wUserPicturesPath);
	if (!wUserPicturesPath.empty())
		::wcscpy_s (pwUserPicturesPath, MAX_PATH, wUserPicturesPath.c_str());
	else
		::SHGetFolderPath (NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, pwUserPicturesPath);
    ofn.lpstrInitialDir = pwUserPicturesPath;

	wchar_t pwFileSelected [65536];
	::ZeroMemory (pwFileSelected, 65536);
	ofn.nMaxFile = 65536;
	ofn.lpstrFile = pwFileSelected;
	wchar_t pwFileTitle [MAX_PATH];
	::ZeroMemory (pwFileTitle, MAX_PATH);
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrFileTitle = pwFileTitle;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_EXPLORER;
	BOOL bRes = ::GetOpenFileName (&ofn);
	::InvalidateRect (m_hParent, NULL, FALSE);
	::UpdateWindow (m_hParent);
	redraw();
	if (!bRes)
	{
		// open file cancelled by user
		bHandled = TRUE;
		return 0;
	}
	// disable buttons
	m_spMenuBar->DisableButton (IGM_IMAGELIB_ADD);
	m_spMenuBar->DisableButton (IGM_IMAGELIB_REMOVE);

	// give a vector of path picture already loaded to the add thread
	std::list <IGSmartPtr <IGLibPicture>> lspPictures;
	m_spConfigMgr->GetPictureList (lspPictures);
	std::vector <std::wstring> vLoadedFiles;		
	for (std::list <IGSmartPtr <IGLibPicture>>::const_iterator iterPict = lspPictures.begin(); 
		iterPict != lspPictures.end();
		iterPict++)
	{
		vLoadedFiles.push_back ((*iterPict)->m_wsPath);
	}

	std::wstring wsNewPicturePath = pwFileSelected;
	wchar_t *pwNextFile = pwFileSelected + wsNewPicturePath.length() + 1;
	int nNbPictures = 0;
	size_t szBuffer = wsNewPicturePath.length() + 1;
	if (*pwNextFile)
	{
		// multi-file selection
		// set user picture path
		std::wstring wsFolderPath (wsNewPicturePath);
		m_spConfigMgr->SetUserPicturesPath (wsFolderPath);	
		// compute nb pictures (for the progress bar) and size of buffer for the path string
		std::wstring wsFileTitle;
		while (*pwNextFile)
		{
			wsFileTitle = pwNextFile;
			szBuffer += wsFileTitle.length() + 1;
			pwNextFile += wsFileTitle.length() + 1;
			nNbPictures++;
		}
		// add zero terminal
		szBuffer++;	
	}
	else
	{
		// single file selection
		nNbPictures = 1;
		// set user picture path
		std::wstring wsFileTitle (pwFileTitle);
		std::wstring wsNewPictureFolder = wsNewPicturePath.substr (0, wsNewPicturePath.length() - (wsFileTitle.length() + 1));
		m_spConfigMgr->SetUserPicturesPath (wsNewPictureFolder);
	}

	// launch add picture thread
	m_spProgress->ProgressSetRange (nNbPictures * IGIMAGELIBRARY_MINIPICTUREHEIGHT*2);
	IGThreadMessage_AddPictures *pIGThreadLoadPictures = new IGThreadMessage_AddPictures (pwFileSelected,
																							szBuffer,
																							nNbPictures,
																							vLoadedFiles,
																							m_hWnd,
																							m_spProgress);
	m_pThreadAddPicture->PostMsg (pIGThreadLoadPictures);
	m_spProgress->Show (true);
	
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibOpen(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	int nNbSelectedItems = ::SendMessage (m_hListBox, (UINT) LB_GETSELCOUNT, 0, 0); 
	int *pSelectedItems = new int [nNbSelectedItems];
	::ZeroMemory (pSelectedItems, nNbSelectedItems * sizeof (int));
	::SendMessage (m_hListBox, (UINT)LB_GETSELITEMS, (WPARAM)nNbSelectedItems, (LPARAM)pSelectedItems);  
	wchar_t pBuf [65536];
	::ZeroMemory (pBuf, 65536 * sizeof (wchar_t));
	for (int nIdxStr = 0; nIdxStr < nNbSelectedItems; nIdxStr++)
	{
		::SendMessage (m_hListBox, (UINT)LB_GETTEXT, (WPARAM)pSelectedItems [nIdxStr], (LPARAM)pBuf);  
		Fire_OnOpenImage (CComBSTR (pBuf));
	}
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibRemove(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	int nNbSelectedItems = ::SendMessage (m_hListBox, (UINT) LB_GETSELCOUNT, 0, 0); 
	int *pSelectedItems = new int [nNbSelectedItems];
	::ZeroMemory (pSelectedItems, nNbSelectedItems * sizeof (int));
	::SendMessage (m_hListBox, (UINT)LB_GETSELITEMS, (WPARAM)nNbSelectedItems, (LPARAM)pSelectedItems);  
	std::wstring wsFileToBeRemoved;
	wchar_t pBuf [65536];
	::ZeroMemory (pBuf, 65536 * sizeof (wchar_t));
	for (int nIdxStr = 0; nIdxStr < nNbSelectedItems; nIdxStr++)
	{
		::SendMessage (m_hListBox, (UINT)LB_GETTEXT, (WPARAM)pSelectedItems [nIdxStr], (LPARAM)pBuf);  
		wsFileToBeRemoved = pBuf;
		m_spConfigMgr->RemovePicture (wsFileToBeRemoved);
		::SendMessage (m_hListBox, (UINT)LB_DELETESTRING, (WPARAM)pSelectedItems [nIdxStr], 0);  
		// Adjust the indices of items after the removed item
		int nIdxRemoved = pSelectedItems [nIdxStr];
		for (int nIdxStrAdjust = nIdxStr + 1; nIdxStrAdjust < nNbSelectedItems; nIdxStrAdjust++)
		{
			if (pSelectedItems [nIdxStrAdjust] > nIdxRemoved)
				pSelectedItems [nIdxStrAdjust]--;
		}
	}
	redraw();
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibDefault(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnImageLibClose(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	hide ();
	bHandled = TRUE;
	return 0;
}

LRESULT CIGImageLibrary::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return MA_ACTIVATE;
}

void CIGImageLibrary::populateListBox ()
{
	std::list <IGSmartPtr <IGLibPicture>> lspPictures;
	m_spConfigMgr->GetPictureList (lspPictures);
	for (std::list <IGSmartPtr <IGLibPicture>>::const_iterator iterPict = lspPictures.begin(); 
		iterPict != lspPictures.end();
		iterPict++)
	{
		// send a list of small images to the listbox
		addListBoxItem (*iterPict);
	}	
}

void CIGImageLibrary::addListBoxItem (const IGLibPicture *picture) 
{
	int nItem = ::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)picture->m_wsPath.c_str()); 
	::SendMessage (m_hListBox, LB_SETITEMDATA, (WPARAM)nItem, (LPARAM)picture->m_hBitmap); 
} 

void CIGImageLibrary::hide () 
{
	::ShowWindow (m_hWnd, SW_HIDE);
	if (m_hParent && m_nIdToSendBack >= 0)
		::PostMessageW (m_hParent, WM_COMMAND, MAKEWPARAM (IGM_IMAGELIB_CLOSE, m_nIdToSendBack), (LPARAM)m_hWnd);
}