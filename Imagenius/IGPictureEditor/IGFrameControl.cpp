// IGFrameControl.cpp : Implementation of CIGFrameControl
#include "stdafx.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrameControl.h"
#include "IGRequest.h"
#include <IGImageProcessing.h>
#include <IGImageProcMessage.h>
#include <gdiplus.h>

using namespace Gdiplus;
using namespace IGLibrary;

#define IGFRAME_BORDER_INCOLOR		Color (255, 97, 190, 132)	// Green
#define IGFRAME_BORDER_OUTCOLOR		Color (255, 157, 213, 184)	// Light Green
#define IGFRAME_BORDER_SELECTED		Color (255, 52, 148, 214)	// Blue
#define IGFRAME_FRAMEOUT_SELECTED	Color (255, 157, 209, 244)	// Light Blue
#define IGFRAME_FRAMEOUT			Color (255, 212, 214, 216)	// Gray
#define IGFRAME_FRAMEBORDER			Color (255, 0, 0, 0)		// Black

// CIGFrameControl
CIGFrameControl::CIGFrameControl()	: m_nFramePosX (0)
									, m_nFramePosY (0)
									, m_nFrameWidth (0)
									, m_nFrameHeight (0)
									, m_bMinimized (false)
									, m_bMaximized (false)
{
}

CIGFrameControl::~CIGFrameControl()
{
}

STDMETHODIMP CIGFrameControl::CreateFrame(OLE_HANDLE hWnd, LPRECT prc, BSTR bstrPath, IDispatch *pDispLayerManager, IDispatch *pDispPropertyManager, VARIANT_BOOL bAutoRotate)
{	
	if (!m_spFrame)
	{
		if (::PathFileExistsW (bstrPath) != 1)
		{
			if (!m_spConfigMgr->IsServerConfig())
				::MessageBoxW ((HWND)hWnd, L"This file does not exist", L"Image loading error", MB_OK | MB_ICONEXCLAMATION);
			return E_FAIL;
		}
		_U_RECT urc (prc);
		HWND hThisWnd = CComControl<CIGFrameControl>::Create ((HWND)hWnd, urc);
		if (!hThisWnd)
			return E_FAIL;
		m_spFrame = new IGLibrary::IGFrame (hThisWnd, bstrPath, bAutoRotate == VARIANT_TRUE, true);
		if (!m_spFrame)
		{
			DestroyFrame();
			return E_FAIL;
		}
		m_spFrame->setFramePtr ((OLE_HANDLE)this);
		if (!m_spFrameMgr->AddFrame (m_spFrame))
		{
			DestroyFrame();
			return E_FAIL;
		}
		if (pDispLayerManager)
			m_spLayerMgr = pDispLayerManager;

		// drag&drop, copy/paste
		if (initializeController (hThisWnd) == CONTROLLER_ERROR)
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CIGFrameControl::CreateNewFrame(OLE_HANDLE hWnd, LPRECT prc, LONG nWidth, LONG nHeight, LONG nColorMode, LONG nBackgroundMode, IDispatch *pDispLayerManager, IDispatch *pDispPropertyManager)
{
	if (m_spFrame)
		return S_OK;
	_U_RECT urc (prc);
	HWND hThisWnd = CComControl<CIGFrameControl>::Create ((HWND)hWnd, urc);
	if (!hThisWnd)
		return E_FAIL;
	m_spFrame = new IGLibrary::IGFrame (hThisWnd, nWidth, nHeight, nColorMode, nBackgroundMode, true);
	if (!m_spFrame)
	{
		DestroyFrame();
		return E_FAIL;
	}
	m_spFrame->setFramePtr ((OLE_HANDLE)this);
	if (!m_spFrameMgr->AddFrame (m_spFrame))
	{
		DestroyFrame();
		return E_FAIL;
	}
	if (pDispLayerManager)
		m_spLayerMgr = pDispLayerManager;
	// drag&drop, copy/paste
	if (initializeController (hThisWnd) == CONTROLLER_ERROR)
		return E_FAIL;

	if (!m_spFrame->CreateHistory (getInstance(), MAKEINTRESOURCE(IDR_FRAMEHISTORY)))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGFrameControl::CreateFrameFromHandle(OLE_HANDLE hWnd, LPRECT prc, OLE_HANDLE hMem, IDispatch *pDispLayerManager, IDispatch *pDispPropertyManager)
{
	if (!m_spFrame)
	{
		_U_RECT urc (prc);
		HWND hThisWnd = CComControl<CIGFrameControl>::Create ((HWND)hWnd, urc);
		if (!hThisWnd)
			return E_FAIL;
		m_spFrame = new IGLibrary::IGFrame (hThisWnd, (HANDLE)hMem, true);
		if (!m_spFrame)
		{
			DestroyFrame();
			return E_FAIL;
		}
		m_spFrame->setFramePtr ((OLE_HANDLE)this);
		if (!m_spFrameMgr->AddFrame (m_spFrame))
		{
			DestroyFrame();
			return E_FAIL;
		}
		if (pDispLayerManager)
			m_spLayerMgr = pDispLayerManager;

		// drag&drop, copy/paste
		if (initializeController (hThisWnd) == CONTROLLER_ERROR)
			return E_FAIL;

		if (!m_spFrame->CreateHistory (getInstance(), MAKEINTRESOURCE(IDR_FRAMEHISTORY)))
			return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CIGFrameControl::UpdatePosition ()
{
	BOOL res = FALSE;
	if (m_spFrame)
	{
		res = CComControl<CIGFrameControl>::MoveWindow (m_spFrame->GetFrameLeft() - IGFRAME_BORDERWIDTH, 
														m_spFrame->GetFrameTop() - IGFRAME_BORDERWIDTH, 
														m_spFrame->GetFrameWidth() + 2 * IGFRAME_BORDERWIDTH, 
														m_spFrame->GetFrameHeight() + 2 * IGFRAME_BORDERWIDTH);
		m_spFrame->UpdatePosition();
	}
	return (res == TRUE) ? S_OK : E_FAIL;
}

STDMETHODIMP CIGFrameControl::AddLayer (LONG nPos)
{
	if (!m_spFrame)
		return E_FAIL;
	HRESULT hr = m_spFrame->CreateNewCalc (nPos) ? S_OK : E_FAIL;
	if (FAILED (hr))
		return E_FAIL;
	// add a new history step
	if (nPos < 0)
		nPos = m_spFrame->GetNbLayers() - 1;
	return S_OK;
}

STDMETHODIMP CIGFrameControl::RemoveLayer (LONG nPos)
{
	if (!m_spFrame)
		return E_FAIL;
	bool bWorkingLast = (m_spFrame->GetWorkingLayer() == m_spFrame->GetLayer (nPos));
	HRESULT hr = m_spFrame->RemoveCalc (nPos);
	if (FAILED (hr))
		return E_FAIL;
	if (bWorkingLast)
		m_spFrame->SetWorkingLayer (m_spFrame->GetNbLayers() - 1);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::MoveLayer (LONG nPosFrom, LONG nPosDest)
{
	if (!m_spFrame)
		return E_FAIL;
	if (FAILED (m_spFrame->MoveCalc (nPosFrom, nPosDest)))
		return E_FAIL;	
	return S_OK;
}

STDMETHODIMP CIGFrameControl::SetToolBox (IDispatch *pDispToolbox)
{
	if (!pDispToolbox)
		return E_INVALIDARG;
	m_spToolBox = pDispToolbox;
	return S_OK;
}

STDMETHODIMP CIGFrameControl::ManageSelection(LONG nSelectionActionId, VARIANT *pArrayParams)
{
	std::list<POINT> lPts;
	if (pArrayParams)
	{
		LPSAFEARRAY pSafeArray = pArrayParams->parray;
		LPPOINT pPts = (LPPOINT)pSafeArray->pvData;		
		for (int idxElem = 0; idxElem < (int)(pSafeArray->cbElements / 2); idxElem++)
			lPts.push_back (pPts [idxElem]);
	}
	m_spFrame->ManageSelection ((IGSELECTIONENUM)nSelectionActionId, lPts);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::ConvertCoordFrameToPixel (LONG *p_nPosX, LONG *p_nPosY)
{
	if (!p_nPosX || !p_nPosY)
		return E_INVALIDARG;
	if (!m_spFrame)
		return E_FAIL;
	m_spFrame->ConvertCoordFrameToPixel ((int*)p_nPosX, (int*)p_nPosY);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::ProcessImage (LONG nImageProcessingId, OLE_HANDLE hParams)
{
	if (!m_spFrame->RequestAccess())
		return E_ACCESSDENIED;
	return S_OK;
}

STDMETHODIMP CIGFrameControl::put_isMinimized (VARIANT_BOOL bMinimized)
{
	m_bMinimized = (bMinimized == VARIANT_TRUE);
	if (m_bMinimized)
		m_bMaximized = false;
	::ShowWindow (m_hWnd, m_bMinimized ? SW_HIDE : SW_SHOW);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::get_isMinimized (VARIANT_BOOL *p_bMinimized)
{
	*p_bMinimized = (m_bMinimized ? VARIANT_TRUE : VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::get_isMaximized (VARIANT_BOOL *p_bMaximized)
{
	*p_bMaximized = (m_bMaximized ? VARIANT_TRUE : VARIANT_FALSE);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::put_isMaximized (VARIANT_BOOL bMaximized)
{
	m_bMaximized = (bMaximized == VARIANT_TRUE);
	if (m_bMaximized && m_bMinimized)
	{
		m_bMinimized = false;
		::ShowWindow (m_hWnd, SW_SHOW);
	}
	m_spFrame->Redraw (true);
	return S_OK;
}

STDMETHODIMP CIGFrameControl::put_isVisible (VARIANT_BOOL bVisible)
{
	if (!m_bMinimized)
		::ShowWindow (m_hWnd, bVisible ? SW_SHOW : SW_HIDE);
	return S_OK;
}

HRESULT CIGFrameControl::OnDraw(ATL_DRAWINFO& di)
{
	if (!m_spFrame)
		return S_OK;
	m_spFrame->Render (di.hdcDraw);
	RECT& rc = *(RECT*)di.prcBounds;
	Graphics graphics (di.hdcDraw);
	int nFramePosX = 0;
	int nFramePosY = 0;
	int nFramePosWidth = 0;
	int nFramePosHeight = 0;
	m_spFrame->GetFramePos (nFramePosX, nFramePosY, nFramePosWidth, nFramePosHeight);
	SolidBrush brushFrameOut (m_spFrame->GetSelected() ? IGFRAME_FRAMEOUT_SELECTED : IGFRAME_FRAMEOUT);
	graphics.FillRectangle (&brushFrameOut, Rect (2, 2,
											rc.right - rc.left - 4, nFramePosY - 2));
	graphics.FillRectangle (&brushFrameOut, Rect (2, nFramePosY,
											nFramePosX - 2, rc.bottom - rc.top - nFramePosY - 2));
	graphics.FillRectangle (&brushFrameOut, Rect (nFramePosX, nFramePosY + nFramePosHeight,
											nFramePosWidth, rc.bottom - rc.top - (nFramePosY + nFramePosHeight) - 2));
	graphics.FillRectangle (&brushFrameOut, Rect (nFramePosX + nFramePosWidth, nFramePosY,
											rc.right - rc.left - (nFramePosX + nFramePosWidth) - 2, rc.bottom - rc.top - nFramePosY - 2));
	Pen penFrameBorder (IGFRAME_FRAMEBORDER, 1);	
	graphics.DrawRectangle (&penFrameBorder, Rect (nFramePosX - 1, nFramePosY - 1,
											nFramePosWidth + 1, nFramePosHeight + 1));
	Pen penIn (m_spFrame->GetSelected() ? IGFRAME_BORDER_SELECTED : IGFRAME_BORDER_INCOLOR, 1);
	Pen penOut (m_spFrame->GetSelected() ? IGFRAME_BORDER_SELECTED : IGFRAME_BORDER_OUTCOLOR, 1);	
	graphics.DrawRectangle (&penOut, Rect (0, 0,
											rc.right - rc.left - 1, rc.bottom - rc.top - 1));
	graphics.DrawRectangle (&penIn, Rect (1, 1,
											rc.right - rc.left - 3, rc.bottom - rc.top - 3));
	return S_OK;
}

LRESULT CIGFrameControl::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGFrameControl::OnLeftMouseButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SetFocus (m_hWnd);
	if (!m_spFrame->GetSelected() && m_spToolBox)
	{
		// attach the toolbox to the selected frame
		m_spToolBox->put_Frame (this);
		m_spFrameMgr->SelectFrame (m_spFrame);		
	}
	else
	{
		m_spFrameMgr->SelectFrame (m_spFrame);
		MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
		preProcessingInput (msg);
	}
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGFrameControl::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGFrameControl::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	bHandled = TRUE;
	return TRUE;
}

int CIGFrameControl::exportData (std::ostream& os, int fid, int cid, int x, int y, HWND hWnd)
{
	switch (fid)
	{
	case Format::F_IGFRAME_DIB:
		{
			HANDLE hCopy = m_spFrame->CopyToHandle ();
			if (hCopy)
			{
				os.write ((char *)&hCopy, sizeof(HANDLE));
				return TRUE;
			}
		}
		break;

	case Format::F_IGFRAME:
		{
			HANDLE hCopy = m_spFrame->CopyToHandle (true);
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

int CIGFrameControl::importData (std::istream& is, int fid, int bufsize, int cid, int x, int y, HWND hWnd)
{
	switch (fid)
	{
	case Format::F_IGLAYER:
	case Format::F_IGFRAME:
	case Format::F_IGFRAME_DIB:	
		{
			if (bufsize == sizeof(HANDLE) + 1)
			{
				HANDLE hBitmapTemp;		
				is.read ((char *)&hBitmapTemp, sizeof (HANDLE));
				m_spFrame->PasteFromHandle (hBitmapTemp, x, y);
				HGLOBAL hg = ::GlobalFree (hBitmapTemp);
				m_spFrame->Redraw (true);
				return TRUE;
			}
		}
		break;	

	case Format::F_IGFRAME_BMP:
		{
			if (bufsize == sizeof(HBITMAP) + 1)
			{
				HBITMAP hBitmapTemp;		
				is.read ((char *)&hBitmapTemp, sizeof (HBITMAP));
				IGLayer *pNewLayer = new IGLayer (hBitmapTemp, m_spFrame);
				::DeleteObject (hBitmapTemp);
				m_spFrame->AddCalc (pNewLayer);
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

LRESULT CIGFrameControl::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return MA_ACTIVATE;
}

LRESULT CIGFrameControl::OnImageProcessingEnded(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	IGImageProcMessage *pMessage = (IGImageProcMessage*)wParam;
	_ASSERTE (pMessage && L"NULL image processing message");	
	if (!pMessage)
		return FALSE;
	LRESULT lRes = 0L;
	if (lParam == IGREQUEST_FAILED)
	{
		m_spFrame->GrantAccess();
		lRes = ::SendMessageW (::GetAncestor (m_hWnd, GA_ROOTOWNER), uMsg, wParam, lParam);
		if (lRes == IGREQUEST_TOBEPOSTED)
		{
			pMessage->SetPosted (true);	
			::PostMessageW (::GetAncestor (m_hWnd, GA_ROOTOWNER), uMsg, wParam, lParam);
		}
		bHandled = TRUE;
		return TRUE;
	}
	if (lParam == IGREQUEST_SUCCEEDED)
	{
		// read image processing id
		IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcId = pMessage->GetImageProcessingId();
		
		// access to frame protected resource can be granted
		// except for load frame because it will induce a saving thread
		if (nImageProcId != IGREQUEST_WORKSPACE_LOADIMAGE)
			m_spFrame->GrantAccess();	

		// transmit message to parent, image processing may be handled by server main window
		lRes = ::SendMessageW (::GetAncestor (m_hWnd, GA_ROOTOWNER), uMsg, wParam, lParam);
		if (lRes == IGREQUEST_TOBEPOSTED)
		{
			pMessage->SetPosted (true);	
			::PostMessageW (::GetAncestor (m_hWnd, GA_ROOTOWNER), uMsg, wParam, lParam);
			bHandled = TRUE;
			return TRUE;
		}
		else if (lRes != IGREQUEST_SUCCEEDED)
		{
			bHandled = TRUE;
			return TRUE;
		}
		delete pMessage;
		
		// update frame
		m_spFrame->OnImageProcessingEnded (nImageProcId);

		if (m_spLayerMgr)
		{
			switch(nImageProcId)
			{
			case IGREQUEST_FRAME_MERGELAYER:
				m_spLayerMgr->PopulateListBox ();
				m_spLayerMgr->SetWorking (m_spFrame->GetNbLayers() - 1);
				break;
			}
		}
	}
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGFrameControl::OnLayerChanged (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spLayerMgr)
		m_spLayerMgr->PopulateListBox();
	if (m_spToolBox)
		m_spToolBox->put_Frame (this);
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGFrameControl::OnTimer (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0L;
}

STDMETHODIMP CIGFrameControl::DestroyFrame()
{
	if (!m_hWnd)
		return S_FALSE;	// already destroyed
	if (m_spFrame->RequestAccess())
	{
		DestroyWindow();
		m_hWnd = NULL;		
		return S_OK;
	}
	return E_ACCESSDENIED;	
}

/*
STDMETHODIMP CIGFrameControl::GetFramePtr(OLE_HANDLE *pFrame)
{
	*pFrame = (OLE_HANDLE)(IGLibrary::IGFrame *)m_spFrame;
	return S_OK;
}*/

int CIGFrameControl::initializeController (HWND hwnd)
{
	// drag&drop, copy/paste
	if (IGCommunicable::initializeController (hwnd) == CONTROLLER_ERROR)
		return CONTROLLER_ERROR;
	if (registerFormat (Format::F_IGSELECTION, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return CONTROLLER_ERROR;
	if (registerFormat (Format::F_IGLAYER, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return CONTROLLER_ERROR;
	if (registerFormat (Format::F_IGFRAME, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return CONTROLLER_ERROR;
	if (registerFormat (Format::F_IGFRAME_DIB, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return CONTROLLER_ERROR;
	if (registerFormat (Format::F_IGFRAME_BMP, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return CONTROLLER_ERROR;
	return 0;
}