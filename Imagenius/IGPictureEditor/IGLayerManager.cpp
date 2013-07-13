// IGLayerManager.cpp : Implementation of CIGLayerManager

#include "stdafx.h"
#include "IGLayerManager.h"
#include "IGSimpleButton.h"
#include <IGImageProcessing.h>
#include <IGImageProcMessage.h>
#include <IGIPMergeLayers.h>

#include <gdiplus.h>

#define IGLAYERMANAGER_LISTBOX_X				29
#define IGLAYERMANAGER_LISTBOX_Y				25
#define IGLAYERMANAGER_ICON_Y					3
#define IGLAYERMANAGER_PANEL_X					22
#define IGLAYERMANAGER_PANELWIDTH				140
#define IGLAYERMANAGER_FRAMEWIDTH				32
#define IGLAYERMANAGER_ITEMWIDTH				124
#define IGLAYERMANAGER_ITEMHEIGHT				40
#define IGLAYERMANAGER_ITEMICON_OFFSET_XY		5
#define IGLAYERMANAGER_BUTTONSIZE				20
#define IGLAYERMANAGER_BUTTONID_ADD				1
#define IGLAYERMANAGER_BUTTONID_REMOVE			2
#define IGLAYERMANAGER_BUTTONID_MERGE			3
#define IGLAYERMANAGER_BUTTONID_UP				4
#define IGLAYERMANAGER_BUTTONID_DOWN			5
#define IGLAYERMANAGER_COLOR_BACKGROUND			Color(255, 169, 207, 185)	// Green
#define IGLAYERMANAGER_COLOR_BORDER				Color(255, 76, 85, 118)		// Black

using namespace Gdiplus;
using namespace std;
using namespace IGLibrary;

// CIGLayerManager
CIGLayerManager::CIGLayerManager()	: m_hWnd (NULL)
									, m_hListBox (NULL)
									, m_spScrollBar (new IGScrollBar ())
{
	::ZeroMemory (&m_rcListBox, sizeof (RECT));
}

CIGLayerManager::~CIGLayerManager()
{
}

STDMETHODIMP CIGLayerManager::CreateFrame (IDispatch *pDispWindowOwner)
{
	m_spDockPanelOwner = pDispWindowOwner;
	if (!m_spDockPanelOwner)
		return E_INVALIDARG;
	m_spDockPanelOwner->GetHWND ((OLE_HANDLE*)&m_hWnd);
	::SetWindowSubclass (m_hWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	RECT rcPanel;
	::GetWindowRect (m_hWnd, &rcPanel);
	rcPanel.right -= rcPanel.left;
	rcPanel.bottom -= rcPanel.top;
	rcPanel.left = rcPanel.top = 0;

	// listbox
	m_hListBox = ::CreateWindowExW (WS_EX_LEFT, L"LISTBOX", L"",
									WS_CHILD | WS_VISIBLE |
									LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_NOREDRAW | LBS_NOINTEGRALHEIGHT,
									m_rcListBox.left, m_rcListBox.top,
									m_rcListBox.right - m_rcListBox.left, m_rcListBox.bottom - m_rcListBox.top,
									m_hWnd,	NULL, getInstance(), NULL);
	::SetWindowSubclass (m_hListBox, StaticHookListBoxProc, 0, (DWORD_PTR)this);
	
	// create the add, remove and merge buttons
	m_spButtonAdd.CoCreateInstance (CLSID_IGSimpleButton);
	m_spButtonRemove.CoCreateInstance (CLSID_IGSimpleButton);
	m_spButtonMerge.CoCreateInstance (CLSID_IGSimpleButton);
	m_spButtonUp.CoCreateInstance (CLSID_IGSimpleButton);
	m_spButtonDown.CoCreateInstance (CLSID_IGSimpleButton);
	RECT rcButton;
	rcButton.left = IGLAYERMANAGER_LISTBOX_X;
	rcButton.top = (rcPanel.bottom - rcPanel.top) / 2;
	rcButton.right = rcButton.left + IGLAYERMANAGER_BUTTONSIZE;
	rcButton.bottom = rcButton.top + IGLAYERMANAGER_BUTTONSIZE;
	m_spButtonAdd->CreateFrame ((OLE_HANDLE)m_hWnd, IGLAYERMANAGER_BUTTONID_ADD, 0, &rcButton, MAKEINTRESOURCEW(IDB_ADDLAYER), NULL, VARIANT_FALSE);
	m_spButtonAdd->SetBackground ((LONG)IGLAYERMANAGER_COLOR_BACKGROUND.ToCOLORREF());
	rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
	rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
	m_spButtonRemove->CreateFrame ((OLE_HANDLE)m_hWnd, IGLAYERMANAGER_BUTTONID_REMOVE, 0, &rcButton, MAKEINTRESOURCEW(IDB_REMOVELAYER), NULL, VARIANT_FALSE);
	m_spButtonRemove->SetBackground ((LONG)IGLAYERMANAGER_COLOR_BACKGROUND.ToCOLORREF());
	rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
	rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
	m_spButtonMerge->CreateFrame ((OLE_HANDLE)m_hWnd, IGLAYERMANAGER_BUTTONID_MERGE, 0, &rcButton, MAKEINTRESOURCEW(IDB_MERGELAYERS), NULL, VARIANT_FALSE);
	m_spButtonMerge->SetBackground ((LONG)IGLAYERMANAGER_COLOR_BACKGROUND.ToCOLORREF());
	rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
	rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
	m_spButtonUp->CreateFrame ((OLE_HANDLE)m_hWnd, IGLAYERMANAGER_BUTTONID_UP, 0, &rcButton, MAKEINTRESOURCEW(IDB_UP), NULL, VARIANT_FALSE);
	m_spButtonUp->SetBackground ((LONG)IGLAYERMANAGER_COLOR_BACKGROUND.ToCOLORREF());
	rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
	rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
	m_spButtonDown->CreateFrame ((OLE_HANDLE)m_hWnd, IGLAYERMANAGER_BUTTONID_DOWN, 0, &rcButton, MAKEINTRESOURCEW(IDB_DOWN), NULL, VARIANT_FALSE);
	m_spButtonDown->SetBackground ((LONG)IGLAYERMANAGER_COLOR_BACKGROUND.ToCOLORREF());

	// create the layer list scrollbar
	m_spScrollBar->CreateFromRect (SBS_VERT, m_hListBox, m_rcListBox, 0); 
	m_spScrollBar->EnableThumbGripper (TRUE);
	m_spScrollBar->Show (SW_HIDE);

	// Connect workspace to DockPanel events
	IDispEventSimpleImpl <1, CIGLayerManager, &__uuidof(_IIGDockPanelEvents)>::DispEventAdvise (pDispWindowOwner);

	return S_OK;
}

STDMETHODIMP CIGLayerManager::DestroyFrame ()
{
	for (list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin(); itItem != m_lpItems.end(); ++itItem)
		(*itItem)->Release();
	m_lpItems.clear();
	m_spButtonAdd->DestroyFrame();
	m_spButtonRemove->DestroyFrame();
	m_spButtonMerge->DestroyFrame();
	m_spButtonUp->DestroyFrame();
	m_spButtonDown->DestroyFrame();
	::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
	::DestroyWindow (m_hListBox);
	::RemoveWindowSubclass (m_hWnd, StaticHookPanelProc, 0);
	return S_OK;
}

STDMETHODIMP CIGLayerManager::Move (LPRECT p_rc)
{
	if (m_hListBox)
	{
		int nWidth = p_rc->right - p_rc->left - 2;
		int nHeight = p_rc->bottom - p_rc->top;		
		m_rcListBox.left = IGLAYERMANAGER_LISTBOX_X;
		m_rcListBox.right = m_rcListBox.left + IGLAYERMANAGER_ITEMWIDTH;
		m_rcListBox.top = nHeight / 2 + IGLAYERMANAGER_LISTBOX_Y;
		m_rcListBox.bottom = nHeight;
		::MoveWindow (m_hListBox, m_rcListBox.left, m_rcListBox.top,
								m_rcListBox.right - m_rcListBox.left, m_rcListBox.bottom - m_rcListBox.top, TRUE);
	}
	if (m_spButtonAdd && m_spButtonRemove && m_spButtonMerge && m_spButtonUp && m_spButtonDown)
	{
		RECT rcButton;
		rcButton.left = IGLAYERMANAGER_LISTBOX_X;
		rcButton.top = (p_rc->bottom - p_rc->top) / 2 + IGLAYERMANAGER_ICON_Y;
		rcButton.right = rcButton.left + IGLAYERMANAGER_BUTTONSIZE;
		rcButton.bottom = rcButton.top + IGLAYERMANAGER_BUTTONSIZE;
		m_spButtonAdd->Move (&rcButton);
		rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
		rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
		m_spButtonRemove->Move (&rcButton);
		rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
		rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
		m_spButtonMerge->Move (&rcButton);
		rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
		rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
		m_spButtonUp->Move (&rcButton);
		rcButton.left += IGLAYERMANAGER_BUTTONSIZE;
		rcButton.right += IGLAYERMANAGER_BUTTONSIZE;
		m_spButtonDown->Move (&rcButton);
	}
	if (m_spScrollBar)
		m_spScrollBar->Move (m_rcListBox);
	return S_OK;
}

STDMETHODIMP CIGLayerManager::SetWorking (LONG nId)
{
	if ((nId < 0) || ((ULONG)nId >= m_lpItems.size()))
		return E_FAIL;
	IGLayer *pLayer = m_spFrame->GetLayer (nId);
	if (!pLayer)
		return E_FAIL;
	setWorking (pLayer);
	return S_OK;
}

void CIGLayerManager::OnPanelOpening ()
{
	if (!m_spFrameMgr->GetSelectedFrame (m_spFrame))
	{
		m_spFrame.reset();	
		return;
	}
	PopulateListBox ();
	::InvalidateRect (m_hListBox, NULL, FALSE);
}

void CIGLayerManager::OnPanelOpened ()
{
}

void CIGLayerManager::OnPanelClosing ()
{	
	for (list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin(); itItem != m_lpItems.end(); ++itItem)
		(*itItem)->Release();
	m_lpItems.clear();
	m_spFrame.reset();
	::SendMessage (m_hListBox, LB_RESETCONTENT, 0, 0); 
}

LRESULT CIGLayerManager::OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
	if (lpDrawItem)
	{
		drawListBoxItem (lpDrawItem->hDC, lpDrawItem->itemID);
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;
}

LRESULT CIGLayerManager::OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	if (lpMeasureItem)
	{
		lpMeasureItem->itemWidth = IGLAYERMANAGER_ITEMWIDTH;
		lpMeasureItem->itemHeight = IGLAYERMANAGER_ITEMHEIGHT;
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;	
}

void CIGLayerManager::updateScrollBar ()
{
	if (m_lpItems.empty())
		return;
	RECT rcItem;
	::ZeroMemory (&rcItem, sizeof (RECT));
	list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
	int nNbLayers = m_spFrame->GetNbLayers ();
	for (int idxItem = 0; 
		(idxItem < nNbLayers) && (itItem != m_lpItems.end());
		idxItem++, ++itItem)
	{
		::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)idxItem, (LPARAM)&rcItem);  
		(*itItem)->Move (&rcItem);
	}
}

bool CIGLayerManager::drawListBoxItem (HDC hdc, int nIdxItem)
{
	if (!m_lpItems.empty())
	{
		RECT rcItem;
		::ZeroMemory (&rcItem, sizeof (RECT));
		::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)nIdxItem, (LPARAM)&rcItem); 
		if (m_spScrollBar->IsVisible())
			rcItem.right -= IGSCROLLBAR_WIDTH;
		list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
		for (int idxItem = 0; idxItem < nIdxItem; ++idxItem, ++itItem)
		{
			if (itItem == m_lpItems.end())
				return false;
		}
		if ((rcItem.top >= m_rcListBox.bottom - m_rcListBox.top) || 
			(rcItem.bottom <= 0))
			return false;
		(*itItem)->Draw (hdc, &rcItem);
	}
	return true;
}

HRESULT CIGLayerManager::PopulateListBox ()
{
	::SendMessage (m_hListBox, LB_RESETCONTENT, 0, 0); 
	for (list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin(); itItem != m_lpItems.end(); ++itItem)
		(*itItem)->Release();
	m_lpItems.clear();
	if (m_spFrame)
	{
		int nNbLayers = m_spFrame->GetNbLayers();
		if (nNbLayers <= 0)
			return E_FAIL;
		int nItemId = 0;
		for (int nIterItem = 0; nIterItem < nNbLayers; nIterItem++)
		{
			nItemId = ::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)L"Layer"); 
			m_spFrame->GetLayer (nIterItem)->SetSelected (false);
			CIGLayerManager_Item *pItem = new CIGLayerManager_Item (m_hListBox, m_spFrame->GetLayer (nIterItem), 
																	m_spDockPanelOwner);
			m_lpItems.push_front (pItem);
		}
		m_spFrame->UnselectCalcs();
		m_spFrame->SelectCalc (nNbLayers - 1);
		::SendMessage (m_hListBox, LB_SETSEL, (WPARAM)TRUE, 0); 
		updateScrollBar();
		::InvalidateRect (m_hListBox, NULL, FALSE);
	}
	return S_OK;
}

void CIGLayerManager::setWorking (IGLayer* pLayer)
{
	pLayer->SetWorking();
	for (list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
		 itItem != m_lpItems.end(); ++itItem)
	{
		if ((*itItem)->m_pLayer == pLayer)
		{
			(*itItem)->m_spButtonTool->put_Pushed (VARIANT_TRUE);
		}
		else
		{
			(*itItem)->m_spButtonTool->put_Pushed (VARIANT_FALSE);
			(*itItem)->m_spButtonTool->Redraw (VARIANT_FALSE);
		}
	}
}

LRESULT CIGLayerManager::OnAddLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (!m_spFrame)
		return 0L;
	if (!m_spFrame->RequestAccess ())
		return 0L;
	IIGFrame *pFrame = (IIGFrame *)m_spFrame->getFramePtr();
	if (FAILED (pFrame->AddLayer()))
		return 0L;
	IGLayer *pLayer = const_cast <IGLayer*> (m_spFrame->GetLayer (m_spFrame->GetNbLayers () - 1));
	if (!pLayer)
		return 0L;	
	::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)L"Layer"); 
	CIGLayerManager_Item *pItem = new CIGLayerManager_Item (m_hListBox, pLayer, m_spDockPanelOwner);
	m_lpItems.push_front (pItem);
	setWorking (pLayer);
	m_spFrame->UnselectCalcs();
	m_spFrame->SelectCalc (m_spFrame->GetNbLayers () - 1);
	::SendMessage (m_hListBox, LB_SETSEL, (WPARAM)TRUE, 0); 
	updateScrollBar ();
	::InvalidateRect (m_hListBox, NULL, FALSE);
	m_spFrame->Redraw (true);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGLayerManager::OnRemoveLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (!m_spFrame || (m_spFrame->GetNbLayers () <= 1))
		return 0L;
	if (!m_spFrame->RequestAccess ())
		return 0L;
	std::list <const IGLayer*> lpLayers = m_spFrame->GetSelectedCalcs ();
	if (lpLayers.empty() || lpLayers.size() == m_spFrame->GetNbLayers ())
		return 0L;			

	bool bResetWorking = false;
	for (list <const IGLayer*>::const_iterator itLayItem = lpLayers.begin();
		 itLayItem != lpLayers.end(); ++itLayItem)
	{		
		CIGLayerManager_Item *pItemToDelete;
		for (list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
			 itItem != m_lpItems.end(); ++itItem)
		{
			if ((*itItem)->m_pLayer == (*itLayItem))
			{
				pItemToDelete = *itItem;
				break;
			}
		}

		removeLayer (*itLayItem);
		m_lpItems.remove (pItemToDelete);
		pItemToDelete->Release();
	}

	updateScrollBar ();
	::InvalidateRect (m_hListBox, NULL, FALSE);
	m_spFrame->Redraw (true);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGLayerManager::OnMergeLayers(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (!m_spFrame || (m_spFrame->GetNbLayers () <= 1))
		return 0L;
	if (!m_spFrame->RequestAccess ())
		return 0L;

	list <const IGLayer*> lpSelLayers = m_spFrame->GetSelectedCalcs();
	lpSelLayers.reverse();
	
	// layer conversion
	list <CxImage*> lpLayers;
	for (list <const IGLayer*>::const_iterator itItem = lpSelLayers.begin();
			 itItem != lpSelLayers.end(); ++itItem)
		lpLayers.push_back ((CxImage*)(*itItem));

	IGIPMergeLayers *pMergeLayerImageProcessing = new IGIPMergeLayers (m_spFrame, 
																  new IGIPMergeLayersMessage (lpLayers));
	pMergeLayerImageProcessing->Start();
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGLayerManager::OnMoveUpLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (!m_spFrame || (m_spFrame->GetNbLayers () <= 1))
		return 0L;
	if (!m_spFrame->RequestAccess ())
		return 0L;
	list <const IGLayer*> lpSelLayers = m_spFrame->GetSelectedCalcs();
	if (lpSelLayers.size() != 1)
		return 0L;
	int nIdxLayer = m_spFrame->GetLayerPos (*lpSelLayers.begin());
	if (nIdxLayer == m_spFrame->GetNbLayers () - 1)
		return 0L;
	IIGFrame *pFrame = (IIGFrame *)m_spFrame->getFramePtr();
	if (FAILED (pFrame->MoveLayer (nIdxLayer, nIdxLayer + 1)))
		return 0L;

	// update list box
	PopulateListBox();

	m_spFrame->UnselectCalcs();
	m_spFrame->SelectCalc (nIdxLayer + 1);
	::SendMessage (m_hListBox, LB_SETSEL, (WPARAM)TRUE, nIdxLayer + 1); 

	m_spFrameMgr->RedrawFrame (m_spFrame);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGLayerManager::OnMoveDownLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	if (!m_spFrame || (m_spFrame->GetNbLayers () <= 1))
		return 0L;
	if (!m_spFrame->RequestAccess ())
		return 0L;
	list <const IGLayer*> lpSelLayers = m_spFrame->GetSelectedCalcs();
	if (lpSelLayers.size() != 1)
		return 0L;
	int nIdxLayer = m_spFrame->GetLayerPos (*lpSelLayers.begin());
	if (nIdxLayer == 0)
		return 0L;
	IIGFrame *pFrame = (IIGFrame *)m_spFrame->getFramePtr();
	if (FAILED (pFrame->MoveLayer (nIdxLayer, nIdxLayer - 1)))
		return 0L;

	// update list box
	PopulateListBox();

	m_spFrame->UnselectCalcs();
	m_spFrame->SelectCalc (nIdxLayer - 1);
	::SendMessage (m_hListBox, LB_SETSEL, (WPARAM)TRUE, nIdxLayer - 1); 

	m_spFrameMgr->RedrawFrame (m_spFrame);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGLayerManager::OnVisibleLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	IGLayer *pLayer = (IGLayer*)hWnd;
	bool bVisible = pLayer->GetVisible();
	pLayer->SetVisible (!bVisible);	
	for (list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
		 itItem != m_lpItems.end(); ++itItem)
	{
		if ((*itItem)->m_pLayer == pLayer)
		{
			(*itItem)->m_spButtonVisible->put_Pushed (bVisible ? VARIANT_TRUE : VARIANT_FALSE);
		}
	}
	m_spFrame->Redraw (true);
	bHandled = TRUE;
	return 0L;
}

LRESULT CIGLayerManager::OnWorkingLayer(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	if (m_lpItems.size() <= 1)
		return 0L;
	setWorking ((IGLayer*)hWnd);
	return 0L;
}

void CIGLayerManager::removeLayer (const IGLayer *pLayer, IGLayer **ppDeletedLayer, bool bRemoveLayers)
{
	if (!pLayer)
		return;
	bool bResetWorking = false;
	if (pLayer->GetWorking())
		bResetWorking = true;	
	::SendMessage (m_hListBox, LB_DELETESTRING, m_spFrame->GetLayerPos (pLayer), 0); 

	// false when using threaded actions such as MergeLayers
	if (bRemoveLayers)
	{
		int nIdxLayer = m_spFrame->GetLayerPos (pLayer);
		if (ppDeletedLayer)
		{
			// layer merging, avoid history registering
			m_spFrame->RemoveCalc (nIdxLayer, ppDeletedLayer);
		}
		else
		{
			IIGFrame *pFrame = (IIGFrame *)m_spFrame->getFramePtr();
			pFrame->RemoveLayer (nIdxLayer);
		}
	}

	if (bResetWorking)
		setWorking (m_spFrame->GetLayer (m_spFrame->GetNbLayers () - 1));
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGLayerManager::StaticHookPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGLayerManager* pThis = (CIGLayerManager*)dwRefData;
	if( pThis != NULL )
		return pThis->HookPanelProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGLayerManager::HookPanelProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	LRESULT lResult = FALSE;
	BOOL bHandled = false;
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_ADD, OnAddLayer)
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_REMOVE, OnRemoveLayer)
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_MERGE, OnMergeLayers)
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_UP, OnMoveUpLayer)
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_DOWN, OnMoveDownLayer)
	MESSAGE_HANDLER (WM_MEASUREITEM, OnMeasureListBoxItem)
	MESSAGE_HANDLER (WM_DRAWITEM, OnDrawListBoxItem)
	
	LRESULT lRes = ::DefSubclassProc (hWnd, uMsg, wParam, lParam);
	
	if (uMsg == WM_PAINT)
	{
		RECT rcPanel;
		rcPanel.left = IGLAYERMANAGER_PANEL_X;
		rcPanel.right = IGLAYERMANAGER_PANEL_X + IGLAYERMANAGER_PANELWIDTH;
		rcPanel.bottom = m_rcListBox.bottom;
		rcPanel.top = m_rcListBox.top - IGLAYERMANAGER_LISTBOX_Y;
		::InvalidateRect (hWnd, &rcPanel, FALSE);
		PAINTSTRUCT ps;
		HDC hDC = ::BeginPaint (hWnd, &ps);
		Graphics graphics (hDC);	
		graphics.SetClip (Rect (ps.rcPaint.left, ps.rcPaint.top,
							ps.rcPaint.right - ps.rcPaint.left,
							ps.rcPaint.bottom - ps.rcPaint.top));
		SolidBrush backgroundBrush (IGLAYERMANAGER_COLOR_BACKGROUND);
		Rect rectBackgroundItem (	rcPanel.left, rcPanel.top, 
									rcPanel.right - rcPanel.left - 1, rcPanel.bottom - rcPanel.top	);
		graphics.FillRectangle (&backgroundBrush, rectBackgroundItem);
		Pen penBorder (IGLAYERMANAGER_COLOR_BORDER);
		graphics.DrawRectangle (&penBorder, rectBackgroundItem);
		::EndPaint (hWnd, &ps);
		return 0L;
	}
	return lRes;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGLayerManager::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGLayerManager* pThis = (CIGLayerManager*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGLayerManager::HookListBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	LRESULT lResult = FALSE;
	BOOL bHandled = false;
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_VISIBLE, OnVisibleLayer)
	COMMAND_ID_HANDLER (IGLAYERMANAGER_BUTTONID_TOOL, OnWorkingLayer)
	MESSAGE_HANDLER (WM_LBUTTONDOWN, OnLeftMouseButtonDown)
	MESSAGE_HANDLER (WM_MOUSEMOVE, OnMouseMove)
	MESSAGE_HANDLER (WM_LBUTTONUP, OnMouseUp)

	switch(uMsg)
	{
	case WM_PAINT:
		{			
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			Graphics graphics (hDC);	
			SolidBrush backgroundBrush (IGLAYERMANAGER_LISTBOXCOLOR_BACKGROUND);
			Rect rectBackgroundItem (0, 0,
									m_rcListBox.right - m_rcListBox.left - 1, m_rcListBox.bottom - m_rcListBox.top);
			graphics.FillRectangle (&backgroundBrush, rectBackgroundItem);
			Pen penBorder (IGLAYERMANAGER_COLOR_BORDER);
			graphics.DrawRectangle (&penBorder, rectBackgroundItem);
			if (m_spFrame)
			{
				int nNbLayers = m_spFrame->GetNbLayers();
				if (nNbLayers > 0)
				{
					for (int nIdxItem = 0; nIdxItem < nNbLayers; nIdxItem++)
					{
						drawListBoxItem (hDC, nIdxItem);
					}
				}
			}
			::EndPaint(hWnd, &ps);
			updateScrollBar ();
		}
		break;
	// hacked messages
	case WM_ERASEBKGND:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
		return 0L;
	}

	LRESULT lRes = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);

	switch(uMsg)
	{
	case WM_LBUTTONUP:
		{
			int nNbSelectedItems = ::SendMessageW (m_hListBox, (UINT)LB_GETSELCOUNT, 0, 0); 
			if (nNbSelectedItems <= 0)
				return 0L;
			int *pnSelectedItems = new int[nNbSelectedItems];
			::SendMessage (hWnd, (UINT)LB_GETSELITEMS, (WPARAM)nNbSelectedItems, (LPARAM)pnSelectedItems); 
			if (pnSelectedItems)
			{	
				m_spFrame->UnselectCalcs();
				for (int nIdxItem = 0; nIdxItem < nNbSelectedItems; nIdxItem++)
				{
					m_spFrame->SelectCalc (m_spFrame->GetNbLayers() - 1 - pnSelectedItems[nIdxItem]);
				}
			}
		}
		break;
	}
	return lRes;
}

LRESULT CIGLayerManager::OnLeftMouseButtonDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	RECT rcListBox;
	::GetWindowRect (m_hListBox, &rcListBox);
	unsigned int nIdxItem = pt.y / IGLAYERMANAGER_ITEMHEIGHT + m_spScrollBar->GetScrollPos();
	if (nIdxItem < m_lpItems.size())
	{
		list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
		for (unsigned int nIterItem = 0; nIterItem < nIdxItem; nIterItem++)
			++itItem;
		(*itItem)->processLeftMouseButtonDown (m_hListBox, uMsg, wParam, lParam);
	}
	bHandled = FALSE;
	return FALSE;
}

LRESULT CIGLayerManager::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	RECT rcListBox;
	::GetWindowRect (m_hListBox, &rcListBox);
	unsigned int nIdxItem = pt.y / IGLAYERMANAGER_ITEMHEIGHT + m_spScrollBar->GetScrollPos();
	if (nIdxItem < m_lpItems.size())
	{
		list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
		for (unsigned int nIterItem = 0; nIterItem < nIdxItem; nIterItem++)
			++itItem;
		(*itItem)->processMouseMove (m_hListBox, uMsg, wParam, lParam);
	}
	bHandled = FALSE;
	return FALSE;
}

LRESULT CIGLayerManager::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	RECT rcListBox;
	::GetWindowRect (m_hListBox, &rcListBox);
	unsigned int nIdxItem = pt.y / IGLAYERMANAGER_ITEMHEIGHT + m_spScrollBar->GetScrollPos();
	if (nIdxItem < m_lpItems.size())
	{
		list <CIGLayerManager_Item*>::const_iterator itItem = m_lpItems.begin();
		for (unsigned int nIterItem = 0; nIterItem < nIdxItem; nIterItem++)
			++itItem;
		(*itItem)->processMouseUp (m_hListBox, uMsg, wParam, lParam);
	}
	bHandled = FALSE;
	return FALSE;
}
