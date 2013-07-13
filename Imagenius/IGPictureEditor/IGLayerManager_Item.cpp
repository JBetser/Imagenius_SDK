#include "stdafx.h"
#include "IGLayerManager.h"
#include <gdiplus.h>

using namespace Gdiplus;

#define IGLAYERMANAGER_COLOR_ITEM				Color(255, 191, 193, 195)	// Medium Gray
#define IGLAYERMANAGER_COLOR_BACKGROUNDICON		Color(255, 212, 214, 216)	// Light Gray
#define IGLAYERMANAGER_COLOR_BORDERICON			Color(255, 151, 153, 155)	// Dark Gray
#define IGLAYERMANAGER_COLOR_SELECTITEM			Color(100, 52, 148, 214)	// Blue Mask
#define IGLAYERMANAGER_COLOR_SELECTITEMBORDER	Color(255, 52, 148, 214)	// Blue
#define IGLAYERMANAGER_COLOR_BORDERITEM			Color(255, 76, 85, 118)		// Black
#define IGLAYERMANAGER_ITEM_BUTTON_X			5
#define IGLAYERMANAGER_ITEM_BUTTONSELECT_X		25
#define IGLAYERMANAGER_ITEM_BUTTON_Y			5
#define IGLAYERMANAGER_ITEM_BUTTON_OFFSET		4
#define IGLAYERMANAGER_ITEM_BUTTON_SIZE			15
#define IGLAYERMANAGER_ITEM_FRAME_X				55
#define IGLAYERMANAGER_ITEM_FRAME_Y				4
#define IGLAYERMANAGER_ITEM_FRAME_HEIGHT		33

CIGLayerManager_Item::CIGLayerManager_Item (HWND hListBox, IGLibrary::IGLayer *pLayer,
											IIGDockPanel *pDockPanel)	: m_pLayer (pLayer)
																		, m_hListBox (hListBox)																		
{
	_ASSERTE (m_pLayer && L"A frame layer is NULL");
	if (m_pLayer)
	{
		m_spButtonVisible.CoCreateInstance (CLSID_IGSimpleButton);
		m_spButtonTool.CoCreateInstance (CLSID_IGSimpleButton);
		RECT rcButton;
		rcButton.left = 0;
		rcButton.top = 0;
		rcButton.right = IGLAYERMANAGER_ITEM_BUTTON_SIZE;
		rcButton.bottom = IGLAYERMANAGER_ITEM_BUTTON_SIZE;
		m_spButtonVisible->CreateFrame ((OLE_HANDLE)hListBox, IGLAYERMANAGER_BUTTONID_VISIBLE, (LONG)pLayer, &rcButton, MAKEINTRESOURCEW(IDB_VISIBLE), MAKEINTRESOURCEW(IDB_INVISIBLE), VARIANT_TRUE);
		m_spButtonTool->CreateFrame ((OLE_HANDLE)hListBox, IGLAYERMANAGER_BUTTONID_TOOL, (LONG)pLayer, &rcButton, NULL, MAKEINTRESOURCEW(IDB_TOOL), VARIANT_TRUE);
		m_spButtonVisible->put_Pushed (pLayer->GetVisible() ? VARIANT_FALSE : VARIANT_TRUE);
		m_spButtonTool->put_Pushed (pLayer->GetWorking() ? VARIANT_TRUE : VARIANT_FALSE);
		OLE_HANDLE hButton;
		if (SUCCEEDED (m_spButtonVisible->GetHWND (&hButton)))
			pDockPanel->RegisterControls (hButton);
		if (SUCCEEDED (m_spButtonTool->GetHWND (&hButton)))
			pDockPanel->RegisterControls (hButton);		

		initializeController (hListBox);
		registerFormat (Format::F_IGLAYER, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT);
		registerFormat (Format::F_IGFRAME_DIB, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT);
		registerFormat (Format::F_IGFRAME_BMP, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT);
	}
}

CIGLayerManager_Item::~CIGLayerManager_Item ()
{
	if (m_spButtonVisible)
		m_spButtonVisible->DestroyFrame();
	if (m_spButtonTool)
		m_spButtonTool->DestroyFrame();
}

void CIGLayerManager_Item::Draw (HDC hdc, RECT *p_rcItem)
{
	//item background
	Graphics graphics (hdc);	
	SolidBrush backgroundBrush (IGLAYERMANAGER_COLOR_ITEM);
	Rect rectBackgroundItem (p_rcItem->left, p_rcItem->top
							,p_rcItem->right - p_rcItem->left - 1, p_rcItem->bottom - p_rcItem->top);
	graphics.FillRectangle (&backgroundBrush, rectBackgroundItem);

	//visible icon background
	SolidBrush iconBrush (IGLAYERMANAGER_COLOR_BACKGROUNDICON);
	RECT rcButton;
	rcButton.left = p_rcItem->left + IGLAYERMANAGER_ITEM_BUTTON_X;
	rcButton.top = p_rcItem->top + IGLAYERMANAGER_ITEM_BUTTON_Y;
	rcButton.right = rcButton.left + IGLAYERMANAGER_ITEM_BUTTON_SIZE;
	rcButton.bottom = rcButton.top + IGLAYERMANAGER_ITEM_BUTTON_SIZE;
	Rect rectIcon (rcButton.left, rcButton.top
					,rcButton.right - rcButton.left - 1, rcButton.bottom - rcButton.top);
	graphics.FillRectangle (&iconBrush, rectIcon);
	Pen iconBorderPen (IGLAYERMANAGER_COLOR_BORDERICON);
	graphics.DrawLine (&iconBorderPen, rcButton.left - 2, rcButton.top - 2, 
						rcButton.right - 2, rcButton.top - 2);
	graphics.DrawLine (&iconBorderPen, rcButton.left - 2, rcButton.top - 2, 
						rcButton.left - 2, rcButton.bottom - 1);
	
	//tool icon background
	rcButton.top = rcButton.bottom + IGLAYERMANAGER_ITEM_BUTTON_OFFSET;
	rcButton.bottom = rcButton.top + IGLAYERMANAGER_ITEM_BUTTON_SIZE;
	rectIcon = Rect (rcButton.left, rcButton.top
					,rcButton.right - rcButton.left - 1, rcButton.bottom - rcButton.top);
	graphics.FillRectangle (&iconBrush, rectIcon);
	graphics.DrawLine (&iconBorderPen, rcButton.left - 2, rcButton.top - 2, 
						rcButton.right - 2, rcButton.top - 2);
	graphics.DrawLine (&iconBorderPen, rcButton.left - 2, rcButton.top - 2, 
						rcButton.left - 2, rcButton.bottom - 1);

	Rect rectItem	(p_rcItem->left, p_rcItem->top
					,p_rcItem->right - p_rcItem->left - 1, p_rcItem->bottom - p_rcItem->top);
	Pen penBorder (IGLAYERMANAGER_COLOR_BORDERITEM);
	graphics.DrawRectangle (&penBorder, rectItem);
	if (m_pLayer->GetSelected())
	{
		rectItem.X++;
		rectItem.Y++;
		rectItem.Width -= 2;
		rectItem.Height -= 2;
		SolidBrush selectItemBrush (IGLAYERMANAGER_COLOR_SELECTITEM);
		graphics.FillRectangle (&selectItemBrush, rectItem);
		penBorder.SetColor (IGLAYERMANAGER_COLOR_SELECTITEMBORDER);
		graphics.DrawRectangle (&penBorder, rectItem);
	}

	rectItem.X += IGLAYERMANAGER_ITEM_FRAME_X;
	rectItem.Y += IGLAYERMANAGER_ITEM_FRAME_Y;
	int nWidth = IGLAYERMANAGER_ITEM_FRAME_HEIGHT;
	int nHeight = IGLAYERMANAGER_ITEM_FRAME_HEIGHT;
	int nItemWidth = m_pLayer->GetWidth();
	int nItemHeight = m_pLayer->GetHeight();
	float fRatio = 1.0f;
	if ((float)nWidth / (float)nHeight > (float)nItemWidth / (float)nItemHeight)
	{
		fRatio = (float)nHeight / (float)nItemHeight;
		nItemWidth = (int)((float)nItemWidth * fRatio);			
		nItemHeight = nHeight;
	}
	else
	{
		fRatio = (float)nWidth / (float)nItemWidth;
		nItemWidth = nWidth;
		nItemHeight = (int)((float)nItemHeight * fRatio);
	}
	rectItem.X += (nWidth - nItemWidth) / 2;
	rectItem.Y += (nHeight - nItemHeight) / 2;
	rectItem.Width = nItemWidth;
	rectItem.Height = nItemHeight;
	m_pLayer->Render (hdc, rectItem.X, rectItem.Y,
					nItemWidth, nItemHeight);
	graphics.DrawRectangle (&penBorder, rectItem);
}

void CIGLayerManager_Item::Move (LPRECT p_rcItem)
{
	if (!m_spButtonVisible || !m_spButtonTool)
		return;
	RECT rcButton;
	rcButton.left = p_rcItem->left + IGLAYERMANAGER_ITEM_BUTTON_X;
	rcButton.top = p_rcItem->top + IGLAYERMANAGER_ITEM_BUTTON_Y;
	rcButton.right = rcButton.left + IGLAYERMANAGER_ITEM_BUTTON_SIZE;
	rcButton.bottom = rcButton.top + IGLAYERMANAGER_ITEM_BUTTON_SIZE;
	m_spButtonVisible->Move (&rcButton);
	rcButton.top = rcButton.bottom + IGLAYERMANAGER_ITEM_BUTTON_OFFSET;
	rcButton.bottom = rcButton.top + IGLAYERMANAGER_ITEM_BUTTON_SIZE;
	m_spButtonTool->Move (&rcButton);
}

void CIGLayerManager_Item::Redraw()
{
	m_spButtonVisible->Redraw();
	m_spButtonTool->Redraw();
}

int CIGLayerManager_Item::exportData (std::ostream& os, int fid, int cid, int x, int y, HWND hWnd)
{
	switch (fid)
	{
	case Format::F_IGFRAME_DIB:
		{
			HANDLE hCopy = m_pLayer->CopyToHandle ();
			if (hCopy)
			{
				os.write ((char *)&hCopy, sizeof(HANDLE));
				return TRUE;
			}
		}
		break;
/*
	case Format::F_IGFRAME_BMP:
		{			
			HBITMAP hCopy = m_pLayer->MakeBitmap ();
			if (hCopy)
			{
				os.write ((char *)&hCopy, sizeof(HBITMAP));
				return TRUE;
			}			
		}
		break;*/

	case Format::F_IGLAYER:
		{			
			HANDLE hCopy = m_pLayer->CopyToHandle (true);
			if (hCopy)
			{
				os.write ((char *)&hCopy, sizeof(HANDLE));
				return TRUE;
			}			
		}
		break;
	}
	return FALSE;
}

LRESULT CIGLayerManager_Item::processLeftMouseButtonDown (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MSG msg = {hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	return FALSE;
}

LRESULT CIGLayerManager_Item::processMouseMove (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MSG msg = {hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	return FALSE;
}

LRESULT CIGLayerManager_Item::processMouseUp (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MSG msg = {hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	return FALSE;
}