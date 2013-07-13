// IGImageLibrary.cpp : Implementation of CIGImageLibrary
#include "stdafx.h"
#include "IGImageLibrary.h"
#include <shlobj.h>
#include "../IGLibrary/IGLibPicture.h"
#include <gdiplus.h>
using namespace Gdiplus;

#define IGIMAGELIBRARY_WIDTH			510
#define IGIMAGELIBRARY_HEIGHT			445
#define IGIMAGELIBRARY_MENUBARHEIGHT	30
#define IGIMAGELIBRARY_BORDERWIDTH		30
#define IGIMAGELIBRARY_CORNERRADIUS		IGIMAGELIBRARY_BORDERWIDTH * 2
#define IGIMAGELIBRARY_CORNERDIAM		IGIMAGELIBRARY_CORNERRADIUS * 2
#define IGIMAGELIBRARY_MINIPICTUREWIDTH		80
#define IGIMAGELIBRARY_MINIPICTUREHEIGHT	80

// ListBox parameters
#define IGIMAGELIBRARY_LISTBOXWIDTH			IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH * 2
#define IGIMAGELIBRARY_LISTBOXHEIGHT		IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_MENUBARHEIGHT - IGIMAGELIBRARY_BORDERWIDTH
#define IGIMAGELIBRARY_ITEMWIDTH			90
#define IGIMAGELIBRARY_ITEMHEIGHT			90
#define IGIMAGELIBRARY_ITEMHEIGHTSEPARATOR	IGIMAGELIBRARY_ITEMHEIGHT - IGIMAGELIBRARY_MINIPICTUREHEIGHT
#define IGIMAGELIBRARY_LISTBOXCORNERRADIUS	10

// Scrollbar parameters
#define IGIMAGELIBRARY_SCROLLBARWIDTH		120
#define IGIMAGELIBRARY_SCROLLBARHEIGHT		20
#define IGIMAGELIBRARY_SCROLLBARBUTTONWIDTH	IGIMAGELIBRARY_SCROLLBARWIDTH / 6

// Color parameters
#define IGIMAGELIBRARY_COLOR_BORDERIN		Color(255, 173, 200, 203)	// Blue
#define IGIMAGELIBRARY_COLOR_BORDERMIDDLE	Color(255, 255, 255, 255)	// White
#define IGIMAGELIBRARY_COLOR_BORDEROUT		Color(255, 223, 230, 238)	// Light Blue
#define IGIMAGELIBRARY_COLOR_FRAMEIN		Color(255, 255, 255, 255)	// White
#define IGIMAGELIBRARY_COLOR_FRAMEOUT		Color(255, 222, 233, 234)	// Light Blue
#define IGIMAGELIBRARY_COLOR_FRAMEBORDER	Color(255, 227, 236, 237)	// Very Light Blue
#define IGIMAGELIBRARY_COLOR_SELECTITEM		Color(150, 240, 247, 255)	// Bluish White Mask
#define IGIMAGELIBRARY_COLOR_BORDERSELECTITEM	Color(255, 173, 200, 203)	// Blue

int CIGImageLibrary::g_nNbItemsPerColumn = ((IGIMAGELIBRARY_LISTBOXHEIGHT) / (IGIMAGELIBRARY_ITEMHEIGHT));
int CIGImageLibrary::g_nNbColumnsPerPage = ((IGIMAGELIBRARY_LISTBOXWIDTH) / (IGIMAGELIBRARY_ITEMWIDTH));

// CIGImageLibrary
CIGImageLibrary::CIGImageLibrary() : m_hListBox (NULL)
									, m_hRegion (NULL)
									, m_nCurrentScrollPos (0)
									, m_nMaxPos (0)
									, m_pnSelectedItems (NULL)
									, m_nNbSelectedItems (0)
{
	m_pConfigMgr = IGConfigManager::getInstance();	
}

CIGImageLibrary::~CIGImageLibrary()
{
	if (m_pnSelectedItems)
		delete [] m_pnSelectedItems;
	m_pConfigMgr->Release();
}

STDMETHODIMP CIGImageLibrary::CreateFrame(OLE_HANDLE hWnd, LPRECT prc)
{
	if (!prc || !hWnd)
		return E_FAIL;
	RECT rcImageLibrary;
	rcImageLibrary.left = prc->left;
	rcImageLibrary.top = prc->top;
	rcImageLibrary.right = rcImageLibrary.left + IGIMAGELIBRARY_WIDTH;
	rcImageLibrary.bottom = rcImageLibrary.top + IGIMAGELIBRARY_HEIGHT;
	_U_RECT urc (&rcImageLibrary);
	HWND hImageLibrary = CComControl<CIGImageLibrary>::Create ((HWND)hWnd, urc, L"ImageLibrary", WS_POPUP);
	if (!hImageLibrary)
		return E_FAIL;

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
	HRSRC hRsrc = ::FindResource (  getInstance(),
									MAKEINTRESOURCE (IDR_IMAGELIBRARYCONFIG),
									L"Configuration"	); 
	// add a zero terminal to resource buffer
	DWORD dwNbConfigBytes = ::SizeofResource (getInstance(), hRsrc);
	HGLOBAL hConfig = ::LoadResource (getInstance(), hRsrc);	
	char *pXml = (char *)::LockResource (hConfig);
	char *pValidXml = new char[dwNbConfigBytes + 1];
	::ZeroMemory (pValidXml, dwNbConfigBytes + 1);
	::memcpy_s (pValidXml, dwNbConfigBytes + 1, pXml, dwNbConfigBytes);
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
	hRsrc = ::FindResource (  getInstance(),
									MAKEINTRESOURCE (IDR_IMAGELIBRARYSCROLLCONFIG),
									L"Configuration"	); 
	// add a zero terminal to resource buffer
	dwNbConfigBytes = ::SizeofResource (getInstance(), hRsrc);
	hConfig = ::LoadResource (getInstance(), hRsrc);	
	pXml = (char *)::LockResource (hConfig);
	pValidXml = new char[dwNbConfigBytes + 1];
	::ZeroMemory (pValidXml, dwNbConfigBytes + 1);
	::memcpy_s (pValidXml, dwNbConfigBytes + 1, pXml, dwNbConfigBytes);
	m_spScrollBar->CreateFrame ((OLE_HANDLE)m_hWnd, &rcScrollBar, CComBSTR (pValidXml));
	delete [] pValidXml;

	m_hListBox = ::CreateWindowExW (WS_EX_LEFT, L"LISTBOX", L"",
									WS_CHILD | WS_VISIBLE |
									LBS_MULTICOLUMN  | LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_SORT | LBS_NOREDRAW | LBS_NOINTEGRALHEIGHT,
									IGIMAGELIBRARY_BORDERWIDTH, IGIMAGELIBRARY_MENUBARHEIGHT,
									IGIMAGELIBRARY_LISTBOXWIDTH, IGIMAGELIBRARY_LISTBOXHEIGHT,
									m_hWnd,	NULL, getInstance(), NULL);
	::SendMessageW (m_hListBox, (UINT) LB_SETCOLUMNWIDTH, (WPARAM) IGIMAGELIBRARY_ITEMWIDTH, 0 );
	::SetWindowSubclass (m_hListBox, StaticHookListBoxProc, 0, (DWORD_PTR)this);
	populateListBox ();

	return S_OK;
}

STDMETHODIMP CIGImageLibrary::ShowFrame (LPRECT prc)
{
	if (!prc)
		return E_FAIL;
	::MoveWindow (m_hWnd, prc->left, prc->bottom, IGIMAGELIBRARY_WIDTH, IGIMAGELIBRARY_HEIGHT, TRUE);
	::ShowWindow (m_hWnd, SW_SHOW);
	return S_OK;
}

STDMETHODIMP CIGImageLibrary::DestroyFrame()
{	
	if (!m_hWnd)
	{
		return S_OK;
	}
	else
	{
		::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
		::DestroyWindow (m_hListBox);
	}
	if (m_spMenuBar)
	{
		m_spMenuBar->DestroyFrame();
		m_spMenuBar.Release();
	}
	if (m_hWnd)
	{
		::DeleteObject (m_hRegion);
		DestroyWindow();
		m_hWnd = NULL;
	}
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

	// Left part
	LinearGradientBrush linLeftGrBrush (PointF(0,0), PointF((float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f, 0),
							IGIMAGELIBRARY_COLOR_BORDEROUT, IGIMAGELIBRARY_COLOR_BORDERMIDDLE);	
	graphics.FillRectangle (&linLeftGrBrush, RectF (0, 0,
							(float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f,
							IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS));
	LinearGradientBrush linLeft2GrBrush (PointF ((float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f - 1.0f, 0), PointF (IGIMAGELIBRARY_BORDERWIDTH, 0),
							IGIMAGELIBRARY_COLOR_BORDERMIDDLE, IGIMAGELIBRARY_COLOR_BORDERIN);	
	graphics.FillRectangle (&linLeft2GrBrush, RectF ((float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f, 0,
							(float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f,
							IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS));
	
	// Right part
	LinearGradientBrush linRightGrBrush (PointF(IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH - 1, 0),
										PointF(IGIMAGELIBRARY_WIDTH - (float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f, 0),
										IGIMAGELIBRARY_COLOR_BORDERIN, IGIMAGELIBRARY_COLOR_BORDERMIDDLE);
	graphics.FillRectangle (&linRightGrBrush, RectF (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_BORDERWIDTH, 0,
							(float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f,
							IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS));
	LinearGradientBrush linRight2GrBrush (PointF(IGIMAGELIBRARY_WIDTH - (float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f - 1.0f, 0),
										PointF(IGIMAGELIBRARY_WIDTH, 0),
										IGIMAGELIBRARY_COLOR_BORDERMIDDLE, IGIMAGELIBRARY_COLOR_BORDEROUT);
	graphics.FillRectangle (&linRight2GrBrush, RectF (IGIMAGELIBRARY_WIDTH - (float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f, 0,
							(float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f,
							IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS));
	
	// Bottom part
	LinearGradientBrush linBottomGrBrush (PointF (0, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_BORDERWIDTH - 1.0f),
											PointF (0, IGIMAGELIBRARY_HEIGHT - (float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f),
							IGIMAGELIBRARY_COLOR_BORDERIN, IGIMAGELIBRARY_COLOR_BORDERMIDDLE);
	graphics.FillRectangle (&linBottomGrBrush, RectF (IGIMAGELIBRARY_CORNERRADIUS, 
												IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_BORDERWIDTH,
												IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERDIAM,
												(float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f));
	LinearGradientBrush linBottom2GrBrush (PointF(0,IGIMAGELIBRARY_HEIGHT - (float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f - 1.0f),
											PointF(0,IGIMAGELIBRARY_HEIGHT),
							IGIMAGELIBRARY_COLOR_BORDERMIDDLE, IGIMAGELIBRARY_COLOR_BORDEROUT);
	graphics.FillRectangle (&linBottom2GrBrush, RectF (IGIMAGELIBRARY_CORNERRADIUS, 
												IGIMAGELIBRARY_HEIGHT - (float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f,
												IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERDIAM,
												(float)IGIMAGELIBRARY_BORDERWIDTH / 2.0f));

	// Bottom left part
	GraphicsPath pathBottomLeft;
	pathBottomLeft.AddEllipse (RectF (-5, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM,
										IGIMAGELIBRARY_CORNERDIAM + 10.0f, IGIMAGELIBRARY_CORNERDIAM));
	PathGradientBrush pthGrBrushBottomLeft (&pathBottomLeft);
	Color colorsBottom[] = {IGIMAGELIBRARY_COLOR_BORDEROUT,
								IGIMAGELIBRARY_COLOR_BORDERMIDDLE,
								IGIMAGELIBRARY_COLOR_BORDERIN,
								IGIMAGELIBRARY_COLOR_FRAMEBORDER,
								IGIMAGELIBRARY_COLOR_FRAMEBORDER};
	REAL interpPositionsBottom[] = {   0.0f,
									   0.28f,
									   0.49f,
									   0.50f,
									   1.0f};
	pthGrBrushBottomLeft.SetInterpolationColors (colorsBottom, interpPositionsBottom, 5);
	graphics.FillRectangle (&pthGrBrushBottomLeft, RectF (0, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS,
															IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_CORNERRADIUS));

	// Bottom right part
	REAL interpPositionsBottomRight[] = {   0.0f,
									   		0.30f,
									   		0.54f,
									   		0.55f,
									   		1.0f};
	GraphicsPath pathBottomRight;
	pathBottomRight.AddEllipse (RectF (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERDIAM - 5,
										IGIMAGELIBRARY_CORNERDIAM, IGIMAGELIBRARY_CORNERDIAM + 10));
	PathGradientBrush pthGrBrushBottomRight (&pathBottomRight);
	pthGrBrushBottomRight.SetInterpolationColors (colorsBottom, interpPositionsBottomRight, 5);
	graphics.FillRectangle (&pthGrBrushBottomRight, RectF (IGIMAGELIBRARY_WIDTH - IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_HEIGHT - IGIMAGELIBRARY_CORNERRADIUS,
															IGIMAGELIBRARY_CORNERRADIUS, IGIMAGELIBRARY_CORNERRADIUS));
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
		::SelectObject (hdcCompatible, hBmp);
		int nItemPosX = (IGIMAGELIBRARY_ITEMWIDTH - bitmapInfo.bmWidth) / 2;
		int nItemPosY = (IGIMAGELIBRARY_ITEMHEIGHT - bitmapInfo.bmHeight) / 2;

		::BitBlt (hDC, 
				   rcItem.left + nItemPosX, rcItem.top + nItemPosY, 
				   bitmapInfo.bmWidth, bitmapInfo.bmHeight, 
				   hdcCompatible, 
				   0, 0, 
				   SRCCOPY);
		::SelectObject (hDC, hOldBmp);
		::DeleteDC (hdcCompatible);
		::DeleteObject (hBmpItem);

		int nIsSelected = (int)::SendMessageW (m_hListBox, (UINT)LB_GETSEL, (WPARAM)nItemId, 0);  
		if (nIsSelected > 0)
		{
			Graphics graphics (hDC);
			SolidBrush selectItemBrush (IGIMAGELIBRARY_COLOR_SELECTITEM);
			int nBmpMaxSize = bitmapInfo.bmWidth > bitmapInfo.bmHeight ? bitmapInfo.bmWidth : bitmapInfo.bmHeight;
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
	int nNbColumns = m_pConfigMgr->GetNbPictures() / g_nNbItemsPerColumn + (m_pConfigMgr->GetNbPictures() % g_nNbItemsPerColumn ? 1 : 0);
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
	::InvalidateRect (m_hWnd, NULL, FALSE);
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
		::ShowWindow (m_hWnd, SW_HIDE);
		bHandled = TRUE;
		return 0;		
	}
	bHandled = FALSE;
	return 1;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGImageLibrary::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGImageLibrary* pThis = (CIGImageLibrary*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);;
}

//------------------------------------------------------------------------------
LRESULT CIGImageLibrary::HookListBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	bool bRedrawFrame = false;
	switch(msg)
	{
	case WM_PAINT:
		{
			if (m_pConfigMgr)
			{
				int nbItems = m_pConfigMgr->GetNbPictures();
				if (!nbItems)
					return 0L;
				PAINTSTRUCT ps;
				HDC hDC = ::BeginPaint(hWnd, &ps);
				for (int nIdxItem = 0; nIdxItem < nbItems; nIdxItem++)
					drawListBoxItem (hDC, nIdxItem);
				::EndPaint(hWnd, &ps);
				return 0L;
			}
		}
		break;
	case WM_KILLFOCUS:
		::PostMessage (m_hWnd, msg, wParam, lParam);
		break;
	// hacked messages
	case WM_ERASEBKGND:
	case WM_MOUSEMOVE:
		return 0L;
		
	}
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc(hWnd, msg, wParam, lParam);	

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
	ofn.lpstrFilter = L"All Image Files (*.jpg;*.png;*.bmp;*.gif;*.ico;*.tiff)\0*.jpg;*.png;*.bmp;*.gif;*.ico;*.tiff\0All Files\0*.*\0\0";
	ofn.nFilterIndex = 1;	
	DWORD nUserProfilePathLenght = 0;

	wchar_t pwUserPicturesPath [MAX_PATH];
	std::wstring wUserPicturesPath;
	m_pConfigMgr->GetUserPicturesPath (wUserPicturesPath);
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
	if (!bRes)
	{
		// open file cancelled by user
		bHandled = TRUE;
		return 0;
	}

	std::wstring wsNewPicturePath = pwFileSelected;
	wchar_t *pwNextFile = pwFileSelected + wsNewPicturePath.length() + 1;
	if (*pwNextFile)
	{
		// multi-file selection
		// set user picture path
		std::wstring wsFolderPath (wsNewPicturePath);
		m_pConfigMgr->SetUserPicturesPath (wsFolderPath);	
		// add pictures
		std::wstring wsFileTitle;
		while (*pwNextFile)
		{
			wsFileTitle = pwNextFile;
			wsNewPicturePath = wsFolderPath + L"\\" + wsFileTitle;
			bool bAdded = m_pConfigMgr->AddPicture (wsNewPicturePath,
												IGIMAGELIBRARY_MINIPICTUREWIDTH,
												IGIMAGELIBRARY_MINIPICTUREHEIGHT);
			if (bAdded) // check if file already exists
			{
				IGLibPicture *pPictureAdded;
				if (m_pConfigMgr->GetPicture (wsNewPicturePath, pPictureAdded))
					addListBoxItem (pPictureAdded);
			}
			pwNextFile += wsFileTitle.length() + 1;
		}	
	}
	else
	{
		// single file selection
		// set user picture path
		std::wstring wsFileTitle (pwFileTitle);
		std::wstring wsNewPictureFolder = wsNewPicturePath.substr (0, wsNewPicturePath.length() - (wsFileTitle.length() + 1));
		m_pConfigMgr->SetUserPicturesPath (wsNewPictureFolder);
		// add picture
		bool bAdded = m_pConfigMgr->AddPicture (wsNewPicturePath,
												IGIMAGELIBRARY_MINIPICTUREWIDTH,
												IGIMAGELIBRARY_MINIPICTUREHEIGHT);
		if (bAdded) // check if file already exists
		{
			IGLibPicture *pPictureAdded;
			if (m_pConfigMgr->GetPicture (wsNewPicturePath, pPictureAdded))
				addListBoxItem (pPictureAdded);
		}
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
		//IGLibPicture removePicture (wsFileToBeRemoved, 0, 0);
		m_pConfigMgr->RemovePicture (wsFileToBeRemoved);
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
	::ShowWindow (m_hWnd, SW_HIDE);
	bHandled = TRUE;
	return 0;
}

void CIGImageLibrary::populateListBox ()
{
	// TODO : updateconfig and GetListPictures
	std::list <IGLibPicture*> spPictures;
	m_pConfigMgr->GetPictureList (spPictures);
	for (std::list<IGLibPicture*>::const_iterator iterPict = spPictures.begin(); 
		iterPict != spPictures.end();
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