// IGMultiFrame.cpp : Implementation of CIGMultiFrame
#include "stdafx.h"
#include "IGMultiFrame.h"
#include "resource.h"
#include <gdiplus.h>

using namespace Gdiplus;

#define IGMULTIFRAME_COLOR_BACKGROUND			Color(255, 157, 209, 244)	// Light Blue

CIGMultiFrame::CIGMultiFrame()	: m_nNbFrames(0)
								, m_cxLogo (::FindResourceW (getInstance(), MAKEINTRESOURCE (IDB_MULTIFRAME), L"PNG")
											, CXIMAGE_FORMAT_PNG, getInstance())
{
	if (m_cxLogo.GetNbLayers() > 0)
	{
		HDC hdcDesktop = ::GetDC (NULL);
		m_cxLogo.LayerDrawAllInCache (hdcDesktop, 0, 0, m_cxLogo.GetWidth(), m_cxLogo.GetHeight(), false, false, false);
		::ReleaseDC (NULL, hdcDesktop);
	}
}

CIGMultiFrame::~CIGMultiFrame()
{
}

// CIGMultiFrame
STDMETHODIMP CIGMultiFrame::CreateFrame(OLE_HANDLE hWnd, LPRECT prc, IDispatch *pDispLayerManager, IDispatch *pDispWorkspace, IDispatch *pDispToolbox, IDispatch *pDispPropertyManager)
{		
	HRESULT hr = CComControl<CIGMultiFrame>::Create ((HWND)hWnd, *prc, L"MultiFrame", WS_CHILD | WS_VISIBLE) ? S_OK : E_FAIL;
	if (FAILED (hr) || !m_hWnd)
		return E_FAIL;	
	if (pDispLayerManager)
		m_spLayerMgr = pDispLayerManager;
	if (pDispWorkspace)
		m_spWorkspace = pDispWorkspace;	
	if (pDispToolbox)
		m_spToolBox = pDispToolbox;
	if (pDispPropertyManager)
		m_spPropertyMgr = pDispPropertyManager;
	// drag&drop, copy/paste
	if (initializeController (m_hWnd) == CONTROLLER_ERROR)
		return E_FAIL;
	if (registerFormat (Format::F_IGLAYER, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return E_FAIL;
	if (registerFormat (Format::F_IGFRAME, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return E_FAIL;
	if (registerFormat (Format::F_IGFRAME_DIB, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return E_FAIL;
	if (registerFormat (Format::F_IGFRAME_BMP, COPY_PASTE_ID | DRAG_DROP_ID, Format::MODE_IN_OUT) != Format::REGISTRATION_OK)
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGMultiFrame::Move (LPRECT p_rc)
{
	m_spFrameMgr->SetMultiFrameSize (p_rc->right - p_rc->left, p_rc->bottom - p_rc->top); 	
	CComControl <CIGMultiFrame>::MoveWindow (p_rc->left, 
												p_rc->top, 
												p_rc->right - p_rc->left, 
												p_rc->bottom - p_rc->top);	
	autoArrange();
	return S_OK;
}

STDMETHODIMP CIGMultiFrame::DrawFrame (LONG IdxBitmap, OLE_HANDLE hDC, UINT_PTR pRect)
{
	if (m_spFrameMgr->DrawFrame (IdxBitmap, (HDC)hDC, (RECT *)pRect, true))
		return S_OK;
	return E_FAIL;
}

STDMETHODIMP CIGMultiFrame::AddNewImage (LONG nWidth, LONG nHeight, LONG nColorMode, LONG nBackgroundMode)
{
	if (m_nNbFrames >= IGFRAMES_MAX)
		return S_FALSE;
	CComPtr <IIGFrame> spNewFrame;
	spNewFrame.CoCreateInstance (CLSID_IGFrameControl);
	RECT rc;
	GetWindowRect (&rc);
	HRESULT hr = spNewFrame->CreateNewFrame ((OLE_HANDLE)m_hWnd, &rc, nWidth, nHeight, nColorMode, nBackgroundMode, m_spLayerMgr);
	if (FAILED(hr))
		return hr;
	addFrame (spNewFrame);		
	return hr;
}

void CIGMultiFrame::unMaximize ()
{
	VARIANT_BOOL bFrmMaximized;
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{		
		m_ppFrames [idxFrame]->get_isMaximized (&bFrmMaximized);
		if (bFrmMaximized)
		{
			put_isMaximized (((OLE_HANDLE)((IIGFrame*)m_ppFrames [idxFrame])), VARIANT_FALSE);
		}
	}
}

STDMETHODIMP CIGMultiFrame::put_isMinimized (OLE_HANDLE hPtrFrame, VARIANT_BOOL bMinimized)
{
	IIGFrame *pFrame = (IIGFrame *)hPtrFrame;
	// if unmimizing frame, unmaximize maximized frame is there is one
	if (!bMinimized)
		unMaximize ();
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{
		if (pFrame == m_ppFrames [idxFrame])
		{
			if (bMinimized)
			{
				VARIANT_BOOL bMaximized;
				pFrame->get_isMaximized (&bMaximized);
				if (bMaximized)
					put_isMaximized ((OLE_HANDLE)pFrame, VARIANT_FALSE);			
				m_spFrameMgr->MinimizeFrame (idxFrame);				
			}
			else
			{
				m_spFrameMgr->UnMinimizeFrame (idxFrame);				
			}
			HRESULT hr = pFrame->put_isMinimized (bMinimized);
			autoArrange();
			return hr;
		}
	}
	return E_FAIL;
}

STDMETHODIMP CIGMultiFrame::get_isMinimized (OLE_HANDLE hPtrFrame, VARIANT_BOOL *p_bMinimized)
{
	IIGFrame *pFrame = (IIGFrame *)hPtrFrame;
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{		
		if (pFrame == m_ppFrames [idxFrame])
		{
			return pFrame->get_isMinimized (p_bMinimized);
		}
	}
	return E_FAIL;
}

STDMETHODIMP CIGMultiFrame::get_isMaximized (OLE_HANDLE hPtrFrame, VARIANT_BOOL *p_bMaximized)
{
	IIGFrame *pFrame = (IIGFrame *)hPtrFrame;
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{		
		if (pFrame == m_ppFrames [idxFrame])
		{
			return pFrame->get_isMaximized (p_bMaximized);
		}
	}
	return E_FAIL;
}

STDMETHODIMP CIGMultiFrame::put_isMaximized (OLE_HANDLE hPtrFrame, VARIANT_BOOL bMaximized)
{
	HRESULT hr = E_FAIL;
	IIGFrame *pFrame = (IIGFrame *)hPtrFrame;
	// if maximizing a frame, unmaximized maximized frame if there is already one
	if (bMaximized)
		unMaximize ();
	// maximize/unmaximize frame
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{		
		if (pFrame == m_ppFrames [idxFrame])
		{
			if (bMaximized)
			{
				VARIANT_BOOL bMinimized;
				pFrame->get_isMinimized (&bMinimized);
				if (bMinimized)
					put_isMinimized ((OLE_HANDLE)pFrame, VARIANT_FALSE);	
				m_spFrameMgr->MaximizeFrame (idxFrame);
			}
			else
			{
				m_spFrameMgr->UnMaximize();
			}
			hr = pFrame->put_isMaximized (bMaximized);
		}
		else 
		{
			// if maximizing frame make other frames invisible
			m_ppFrames [idxFrame]->put_isVisible (bMaximized ? VARIANT_FALSE : VARIANT_TRUE);
		}
	}
	autoArrange();
	return hr;
}

STDMETHODIMP CIGMultiFrame::get_frameSize (OLE_HANDLE hPtrFrame, LONG *p_nWidth, LONG *p_nHeight) // get a frame size
{
	LONG nFrameId = -1;
	if (FAILED (get_frameId (hPtrFrame, &nFrameId)))
		return E_FAIL;
	int nFrameWidth = 0;
	int nFrameHeight = 0;
	if (m_spFrameMgr->GetFrameSize (nFrameId, nFrameWidth, nFrameHeight))
	{
		*p_nWidth = (LONG)nFrameWidth;
		*p_nHeight = (LONG)nFrameHeight;
		return S_OK;
	}
	return E_FAIL;
}

STDMETHODIMP CIGMultiFrame::AddImage (BSTR bstrImagePath, VARIANT_BOOL bAutoRotate)
{
	unMaximize ();
	HRESULT hr = addFrame (bstrImagePath, bAutoRotate);
	if (SUCCEEDED (hr))
		autoArrange();
	return hr;
}

STDMETHODIMP CIGMultiFrame::AddImageFromHandle (OLE_HANDLE hMem)
{
	unMaximize ();
	HRESULT hr = addFrame ((HANDLE)hMem);
	if (SUCCEEDED (hr))
		autoArrange();
	return hr;
}

STDMETHODIMP CIGMultiFrame::DestroyFrame()
{
	bool bOk = true;
	while (m_nNbFrames > 0)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		bOk = m_spFrameMgr->GetFrame (0, spFrame);
		if (bOk)
		{
			m_spFrameMgr->RemoveFrame (spFrame);
			bOk &= removeFrame (m_ppFrames [0]);
		}
		_ASSERTE (bOk && L"Failed destroying frame");
	}
	m_nNbFrames = 0;
	m_spFrameMgr.Release();
	if (m_hWnd)
	{		
		bOk &= (DestroyWindow() == TRUE);
		m_hWnd = NULL;
	}
	return bOk ? S_OK : E_FAIL;
}

STDMETHODIMP CIGMultiFrame::RemoveFrame (LONG nFrameId)
{
	if (nFrameId == -1){
		while (m_nNbFrames > 0)
		{
			IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
			if (!m_spFrameMgr->GetFrame (0, spFrame))
				return E_FAIL;
			if (!spFrame->RequestAccess())
				return E_FAIL;
			m_spFrameMgr->RemoveFrame (spFrame);
			if (!removeFrame (m_ppFrames [0]))
				return E_FAIL;
		}
		return S_OK;
	}
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (!m_spFrameMgr->GetFrame (nFrameId, spFrame))
		return S_FALSE;	
	if (!spFrame->RequestAccess())
		return E_FAIL;	
	if (removeFrame (m_ppFrames [nFrameId]))
	{	
		unMaximize ();
		m_spFrameMgr->RemoveFrame (spFrame);
		autoArrange();
		return S_OK;
	}
	// Frame could not be removed because it is currently busy
	return E_FAIL;
}

STDMETHODIMP CIGMultiFrame::GetDropTarget (IDropTarget **ppDropTarget)
{
	if (dropTargetsList_.empty())
		return E_FAIL;
	*ppDropTarget = dynamic_cast <IDropTarget *> (dropTargetsList_.front());
	return S_OK;
}

void CIGMultiFrame::addFrame (IIGFrame *pFrame)
{
	if (m_spToolBox)
		pFrame->SetToolBox (m_spToolBox);

	CComQIPtr <IOleObject> spMultiFrameOleObject (this);
	CComPtr <IOleClientSite> spOleClientSite;
	spMultiFrameOleObject->GetClientSite (&spOleClientSite);

	CComQIPtr <IOleObject> spFrameOleObject (pFrame);
	spFrameOleObject->SetClientSite (spOleClientSite);	
	spFrameOleObject.Detach();

	if (pFrame && !frameExists (pFrame) && m_nNbFrames < IGFRAMES_MAX)
	{		
		m_ppFrames [m_nNbFrames++] = pFrame;
	}
	autoArrange();
}

bool CIGMultiFrame::removeFrame (IIGFrame *pFrame)
{
	bool bPopFrame = false;	
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{
		IIGFrame *pIGFrame = NULL;
		if (!bPopFrame && (m_ppFrames [idxFrame] == pFrame))
		{
			if (FAILED (m_ppFrames [idxFrame]->DestroyFrame()))
				break;
			bPopFrame = true;
			pIGFrame = m_ppFrames [idxFrame].Detach();
			CComQIPtr <IOleObject> spOleObject (pIGFrame);
			spOleObject->SetClientSite (NULL);			
		}
		if (pIGFrame)
			while (pIGFrame->Release()){}
		// make sure 0 value is spread to removed frames
		if (bPopFrame && idxFrame < m_nNbFrames)
			m_ppFrames [idxFrame] = m_ppFrames [idxFrame + 1];
	}
	if (bPopFrame)
		--m_nNbFrames;
	return bPopFrame;
}

bool CIGMultiFrame::frameExists (IIGFrame *pFrame)
{
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{
		if (m_ppFrames [idxFrame] == pFrame)
			return true;
	}
	return false;
}

// find a frame id
STDMETHODIMP CIGMultiFrame::get_frameId (OLE_HANDLE hPtrFrame, LONG *p_nFrameId) 
{
	IIGFrame *pFrame = (IIGFrame *)hPtrFrame;
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
	{		
		if (pFrame == m_ppFrames [idxFrame])
		{
			*p_nFrameId = idxFrame;
			return S_OK;
		}
	}
	_ASSERTE (0 && L"Frame cannot be found in multiframe panel");
	return E_FAIL;
}

void CIGMultiFrame::autoArrange()
{
	for (int idxFrame = 0; idxFrame < m_nNbFrames; idxFrame++)
		m_ppFrames [idxFrame]->UpdatePosition ();
	if (!m_nNbFrames)
		::InvalidateRect (m_hWnd, NULL, TRUE);
}

HRESULT CIGMultiFrame::addFrame (BSTR bstrImagePath, VARIANT_BOOL bAutoRotate)
{
	if (m_nNbFrames >= IGFRAMES_MAX)
		return S_FALSE;
	CComPtr <IIGFrame> spNewFrame;
	spNewFrame.CoCreateInstance (CLSID_IGFrameControl);
	RECT rc;
	GetWindowRect (&rc);
	HRESULT hr = spNewFrame->CreateFrame ((OLE_HANDLE)m_hWnd, &rc, bstrImagePath, m_spLayerMgr, m_spPropertyMgr, bAutoRotate);
	if (FAILED(hr))
		return hr;
	addFrame (spNewFrame);		
	return hr;
}

HRESULT CIGMultiFrame::addFrame (HANDLE hMem)
{
	if (m_nNbFrames >= IGFRAMES_MAX)
		return S_FALSE;
	CComPtr <IIGFrame> spNewFrame;
	spNewFrame.CoCreateInstance (CLSID_IGFrameControl);
	RECT rc;
	GetWindowRect (&rc);
	HRESULT hr = spNewFrame->CreateFrameFromHandle ((OLE_HANDLE)m_hWnd, &rc, (OLE_HANDLE)hMem, m_spLayerMgr);
	if (FAILED(hr))
		return hr;
	addFrame (spNewFrame);
	return hr;
}

LRESULT CIGMultiFrame::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_spFrameMgr->GetNbVisibleFrames() <= 0)
	{
		RECT rc;
		GetWindowRect (&rc);
		int nRcWidth = rc.right - rc.left;
		int nRcHeight = rc.bottom - rc.top;
		int nLogoWidth = m_cxLogo.GetWidth ();
		int nLogoHeight = m_cxLogo.GetHeight ();
		int nLogoPosX = nRcWidth / 2 - nLogoWidth / 2;
		int nLogoPosY = nRcHeight / 2 - nLogoHeight / 2;
		Graphics graphics ((HDC)wParam);
		SolidBrush solBrush (IGMULTIFRAME_COLOR_BACKGROUND);	
		graphics.FillRectangle (&solBrush, Rect (0, 0, nRcWidth, nLogoPosY));
		graphics.FillRectangle (&solBrush, Rect (0, nLogoPosY, nLogoPosX, nRcHeight - nLogoPosY));
		graphics.FillRectangle (&solBrush, Rect (nLogoPosX, nLogoPosY + nLogoHeight, nLogoWidth, nRcHeight - (nLogoPosY + nLogoHeight)));
		graphics.FillRectangle (&solBrush, Rect (nLogoPosX + nLogoWidth, nLogoPosY, nRcWidth - (nLogoPosX + nLogoWidth), nRcHeight - nLogoPosY));
		m_cxLogo.LayerDrawAllFromCache ((HDC)wParam, nLogoPosX, nLogoPosY);
	}
	bHandled = TRUE;
	return TRUE;
}

HRESULT CIGMultiFrame::OnDraw(ATL_DRAWINFO& di)
{
	return S_OK;
}

LRESULT CIGMultiFrame::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMultiFrame::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMultiFrame::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return MA_ACTIVATE;
}

int CIGMultiFrame::importData (std::istream& is, int fid, int bufsize, int cid, int x, int y, HWND hWnd)
{
	if (!m_spWorkspace)
		return FALSE;
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
				m_spWorkspace->AddFrameFromHandle ((OLE_HANDLE)hBitmapTemp);
				::GlobalFree (hBitmapTemp);
				return TRUE;
			}
		}
		break;	
/*
	case Format::F_IGFRAME_BMP:
		{
			if (bufsize == sizeof(HBITMAP) + 1)
			{
				HBITMAP hBitmapTemp;		
				is.read ((char *)&hBitmapTemp, sizeof (HBITMAP));
				IGLayer *pNewLayer = new IGLayer (m_spFrame);
				pNewLayer->CreateFromHBITMAP (hBitmapTemp);
				m_spFrame->AddCalc (pNewLayer);
				return TRUE;
			}
		}
		break;*/
	}
	return FALSE;
}