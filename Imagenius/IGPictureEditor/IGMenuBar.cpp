// IGMenuBar.cpp : Implementation of CIGMenuBar
#include "stdafx.h"
#include "IGMenuBar.h"
#include "commandids.h"
#include <msxml2.h>
#include <WindowsX.h>
#include <gdiplus.h>
using namespace Gdiplus;

#define IGMENUBAR_DROP_WIDTH			200
#define IGMENUBAR_DROP_BUTTONHEIGHT		25
#define IGMENUBAR_COLOR_GRADIENTLEFT	Color(255, 97, 190, 132)//Color(255, 173, 239, 225)
#define IGMENUBAR_COLOR_GRADIENTRIGHT	Color(255, 52, 148, 214)//Color(255, 223, 230, 238)
#define IGMENUBAR_COLOR_DROPGRADIENTLEFT	Color(255, 97, 190, 132)//Color(255, 173, 239, 225)
#define IGMENUBAR_COLOR_DROPGRADIENTMIDDLE	Color(255, 223, 230, 238)//Color(255, 52, 148, 214)//Color(255, 223, 230, 238)
#define IGMENUBAR_COLOR_DROPRIGHT			Color(255, 223, 230, 238)//Color(255, 52, 148, 214)//Color(255, 223, 230, 238)
#define IGMENUBAR_COLOR_SEPARATOR		Color(255, 132, 129, 115)
#define IGMENUBAR_COLOR_CENTERLIGHT		Color(255, 255, 255, 255)
#define IGMENUBAR_COLOR_OUTLIGHT			Color(50, 255, 255, 255)

// CIGMenuBar
CIGMenuBar::CIGMenuBar()	: m_nNbButtons(0)
							, m_idxSubMenu (-1)
							, m_idxButton (-1)
							, m_hMenuBarParent (NULL)
							, m_bVertical (VARIANT_FALSE)
							, m_nButtonSize (-1)
							, m_bTransparent (false)
							, m_hFont (NULL)
							, m_nMenuBarInitWidth (0)
							, m_pButtons (NULL)
{
	::ZeroMemory (&m_ptRefPos, sizeof (POINT));
}

CIGMenuBar::~CIGMenuBar()
{
}

STDMETHODIMP CIGMenuBar::CreateFrame(OLE_HANDLE hWnd, LPRECT prc, BSTR xmlConfig, LONG idxButton, LONG nRefPosX, LONG nRefPosY, VARIANT_BOOL bVertical, LONG idxSubMenu)
{
	_U_RECT urc (prc);
	HWND hMenuBar = CComControl<CIGMenuBar>::Create ((HWND)hWnd, urc, L"MenuBar",  (bVertical ? WS_POPUP : WS_CHILD | WS_VISIBLE));
	if (!hMenuBar)
		return E_FAIL;
	m_idxSubMenu = idxSubMenu;
	m_hMenuBarParent = (HWND)hWnd;
	m_bVertical = bVertical;
	m_idxButton = idxButton;
	m_nMenuBarInitWidth = prc->right - prc->left;
	RECT rcParent;
	::GetWindowRect (m_hMenuBarParent, &rcParent);
	if (bVertical)
	{
		m_ptRefPos.x = nRefPosX;
		m_ptRefPos.y = nRefPosY;
	}
	else
	{		
		m_ptRefPos.x = rcParent.left;
		m_ptRefPos.y = rcParent.top;
	}	

	CComPtr <IOleClientSite> spOleClientSite;
	IOleObjectImpl <CIGMenuBar>::GetClientSite (&spOleClientSite);

	CComPtr <IXMLDOMDocument> spXMLDoc;
	HRESULT hr = ::CoCreateInstance(CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
									IID_IXMLDOMDocument, (void**)&spXMLDoc);
	if (FAILED(hr))
		return E_FAIL;

	VARIANT_BOOL bSucceeded;
	hr = spXMLDoc->loadXML (xmlConfig, &bSucceeded);
	if ((hr != S_OK) || !bSucceeded)
		return E_FAIL;
 
	// 2 possibilities : this is the main menubar and there is a menubarconfig node
	// or this is a submenubar and the root is a button node
	CComPtr <IXMLDOMNode> spNodeMenuBarConfig;
	CComPtr <IXMLDOMNodeList> spNodeMenuButtons;
	spXMLDoc->selectSingleNode (CComBSTR("./menubarconfig"), &spNodeMenuBarConfig);
	if (spNodeMenuBarConfig)
	{
		CComPtr <IXMLDOMNamedNodeMap> spMenuBarConfigAttributes;
		spNodeMenuBarConfig->get_attributes (&spMenuBarConfigAttributes);
		if (spMenuBarConfigAttributes)
		{
			CComPtr <IXMLDOMNode> spNodeButtonSize;
			spMenuBarConfigAttributes->getNamedItem (CComBSTR (L"buttonsize"), &spNodeButtonSize);
			if (spNodeButtonSize)
			{
				CComVariant spvButtonSize;	
				spNodeButtonSize->get_nodeValue (&spvButtonSize);
				::swscanf_s (spvButtonSize.bstrVal, L"%d", &m_nButtonSize);
			}
		}
		hr = spNodeMenuBarConfig->selectNodes (CComBSTR("./button"), &spNodeMenuButtons);
	}
	else
	{
		CComPtr <IXMLDOMNode> spNodeMenuButtonRoot;
		hr = spXMLDoc->selectSingleNode (CComBSTR("./button"), &spNodeMenuButtonRoot);
		if ((hr != S_OK) || !spNodeMenuButtonRoot)
			return E_FAIL;
		hr = spNodeMenuButtonRoot->selectNodes (CComBSTR("./button"), &spNodeMenuButtons);
	}
	if ((hr != S_OK) || !spNodeMenuButtons)
		return E_FAIL;

	// create button array
	spNodeMenuButtons->get_length ((long*)&m_nNbButtons);		
	m_pButtons = new CComPtr <IIGMenuBarDropButton> [m_nNbButtons];

	// compute button dimensions
	RECT rc;
	GetWindowRect (&rc);
	int nButtonSizeX, nButtonSizeY;
	if (bVertical)
	{
		nButtonSizeX = IGMENUBAR_DROP_WIDTH;
		nButtonSizeY = IGMENUBAR_DROP_BUTTONHEIGHT;		
	}
	else
	{
		nButtonSizeX = (m_nButtonSize > 0) ? m_nButtonSize : (int)((float)(rc.right - rc.left) / (float)m_nNbButtons);
		nButtonSizeY = (int)((float)(rc.bottom - rc.top));
	}
	rc.top = rc.left = 0;
	rc.right = nButtonSizeX;
	rc.bottom = nButtonSizeY;

	// Other Buttons
	int idxNewButton = 0;
	int idxNewSubMenu = 0;
	IXMLDOMNode *pNodeMenuButton;	
	while (1)
	{
		spNodeMenuButtons->nextNode (&pNodeMenuButton);
		if (!pNodeMenuButton)
			break;
		// check if there is a separator behind this button
		CComPtr <IXMLDOMNamedNodeMap> spButtonAttributes;
		pNodeMenuButton->get_attributes (&spButtonAttributes);
		if (spButtonAttributes)
		{
			CComPtr <IXMLDOMNode> spNodeSeparator;
			spButtonAttributes->getNamedItem (CComBSTR (L"separator"), &spNodeSeparator);
			if (spNodeSeparator)
			{
				int nSeparator = 0;
				CComVariant spvButtonSeparator;	
				spNodeSeparator->get_nodeValue (&spvButtonSeparator);
				::swscanf_s (spvButtonSeparator.bstrVal, L"%d", &nSeparator);
				if (nSeparator == 1)	// add the current button idx to the list of separators
					m_vSeparators.push_back (idxNewButton);
			}
		}
		
		CComBSTR spButtonXml;
		pNodeMenuButton->get_xml (&spButtonXml);

		// create the button		
		m_pButtons [idxNewButton].CoCreateInstance (CLSID_IGMenuBarDropButton);
		CComQIPtr <IOleObject> spOleObject (m_pButtons [idxNewButton]);
		spOleObject->SetClientSite (spOleClientSite);
		m_pButtons [idxNewButton]->put_Font ((OLE_HANDLE)m_hFont);
		m_pButtons [idxNewButton]->CreateFrame ((OLE_HANDLE)hMenuBar, &rc, spButtonXml, (m_idxSubMenu == -1) ? VARIANT_TRUE : VARIANT_FALSE, idxNewButton, idxNewSubMenu);

		// if current button have children, create a submenu
		CComPtr <IXMLDOMNodeList> spNodeSubMenus;
		hr = pNodeMenuButton->selectNodes (CComBSTR("./button"), &spNodeSubMenus);
		if (!spNodeSubMenus)
			break;
		long nSubMenu = 0;
		spNodeSubMenus->get_length (&nSubMenu);
		if ((hr == S_OK) &&  nSubMenu)
		{
			CComPtr <IIGMenuBar> spMenuBar;
			spMenuBar.CoCreateInstance (CLSID_IGMenuBar);
			CComQIPtr <IOleObject> spMenuBarOleObject (spMenuBar);
			spMenuBarOleObject->SetClientSite (spOleClientSite);
			RECT rcWin;
			GetWindowRect (&rcWin);
			// if the current menubar is vertical dock the submenubar on the right
			// else dock the submenubar on the bottom
			if (bVertical)
			{
				rcWin.left += IGMENUBAR_DROP_WIDTH;
				rcWin.right = rcWin.left + IGMENUBAR_DROP_WIDTH;
				rcWin.top += rc.top;
				rcWin.bottom = rcWin.top + nSubMenu * IGMENUBAR_DROP_BUTTONHEIGHT;
			}
			else
			{
				rcWin.left += rc.left;
				rcWin.right = rcWin.left + IGMENUBAR_DROP_WIDTH;
				rcWin.top += nButtonSizeY;
				rcWin.bottom = rcWin.top + nSubMenu * IGMENUBAR_DROP_BUTTONHEIGHT;
			}
			spMenuBar->put_Font ((OLE_HANDLE)m_hFont);
			spMenuBar->CreateFrame ((OLE_HANDLE)hMenuBar, &rcWin, spButtonXml, idxNewButton, m_ptRefPos.x, m_ptRefPos.y, VARIANT_TRUE, idxNewSubMenu);
			m_vSubMenus.push_back (spMenuBar.Detach());
			idxNewSubMenu++;
		}

		// next button
		if (bVertical == VARIANT_TRUE)
		{
			rc.top += nButtonSizeY;
			rc.bottom += nButtonSizeY;
		}
		else
		{
			rc.left += nButtonSizeX;
			rc.right += nButtonSizeX;
		}
		pNodeMenuButton->Release();
		idxNewButton++;
	}

	// initialize the submenu bar position
	if (!spNodeMenuBarConfig)
	{
		RECT rc;
		::GetWindowRect (m_hWnd, &rc);
		rc.left -= m_ptRefPos.x;
		rc.right -= m_ptRefPos.x;
		::SetWindowPos (m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_HIDEWINDOW);
	}

	return S_OK;
}

STDMETHODIMP CIGMenuBar::DestroyFrame()
{	
	freeDropTargets();
	// clean submenus
	for (unsigned int idxSubMenu = 0; idxSubMenu < m_vSubMenus.size(); idxSubMenu++)
	{
		m_vSubMenus[idxSubMenu]->DestroyFrame();
		CComQIPtr <IOleObject> spOleObject (m_vSubMenus[idxSubMenu]);
		spOleObject->SetClientSite (NULL);
	}
	m_vSubMenus.clear();
	// clean subbuttons
	if (m_nNbButtons > 0)
	{
		for (int nIdxButton = 0; nIdxButton < m_nNbButtons; nIdxButton++)
		{
			m_pButtons [nIdxButton]->DestroyFrame();
			CComQIPtr <IOleObject> spOleObject (m_pButtons [nIdxButton]);
			spOleObject->SetClientSite (NULL);
		}
		delete [] m_pButtons;
		m_nNbButtons = 0;
	}
	if (m_hWnd)
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBar::Move (LPRECT p_rc)
{
	CComControl <CIGMenuBar>::MoveWindow (p_rc->left, 
												p_rc->top, 
												p_rc->right - p_rc->left, 
												p_rc->bottom - p_rc->top);	
	return S_OK;
}

STDMETHODIMP CIGMenuBar::get_IdxSubMenu(LONG *p_idxSubMenu)
{
	*p_idxSubMenu = m_idxSubMenu;
	return S_OK;
}

STDMETHODIMP CIGMenuBar::get_IdxButton(LONG *p_idxButton)
{
	*p_idxButton = m_idxButton;
	return S_OK;
}

STDMETHODIMP CIGMenuBar::put_Font(OLE_HANDLE hFont)
{
	m_hFont = (HFONT)hFont;
	return S_OK;
}

STDMETHODIMP CIGMenuBar::ShowPopup(LONG nNewRefPosX, LONG nNewRefPosY)
{
	RECT rc;
	::GetWindowRect (m_hWnd, &rc);
	rc.left += nNewRefPosX - m_ptRefPos.x;
	rc.right += nNewRefPosX - m_ptRefPos.x;
	rc.top += nNewRefPosY - m_ptRefPos.y;
	rc.bottom += nNewRefPosY - m_ptRefPos.y;
	::SetWindowPos (m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
	m_ptRefPos.x = nNewRefPosX;
	m_ptRefPos.y = nNewRefPosY;
	return S_OK;
}

STDMETHODIMP CIGMenuBar::Select(LONG idxButtonClick)
{
	if (!m_pButtons)
		return E_FAIL;
	LONG idxIterButtonClick;
	for (int idxButton = 0; idxButton < m_nNbButtons; idxButton++)
	{
		m_pButtons[idxButton]->get_IdxButtonClick (&idxIterButtonClick);
		if (idxIterButtonClick == idxButtonClick)
		{
			m_pButtons[idxButton]->Select();
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBar::UnSelect(LONG idxButtonClick)
{
	if (!m_pButtons)
		return E_FAIL;
	LONG idxIterButtonClick;
	for (int idxButton = 0; idxButton < m_nNbButtons; idxButton++)
	{
		m_pButtons[idxButton]->get_IdxButtonClick (&idxIterButtonClick);
		if (idxIterButtonClick == idxButtonClick)
		{
			m_pButtons[idxButton]->UnSelect();
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBar::EnableButton(LONG idxButtonClick)
{
	if (!m_pButtons)
		return E_FAIL;
	LONG idxIterButtonClick;
	for (int idxButton = 0; idxButton < m_nNbButtons; idxButton++)
	{
		m_pButtons[idxButton]->get_IdxButtonClick (&idxIterButtonClick);
		if (idxIterButtonClick == idxButtonClick)
		{
			m_pButtons[idxButton]->Enable ();
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBar::DisableButton(LONG idxButtonClick)
{
	if (!m_pButtons)
		return E_FAIL;
	LONG idxIterButtonClick;
	for (int idxButton = 0; idxButton < m_nNbButtons; idxButton++)
	{
		m_pButtons[idxButton]->get_IdxButtonClick (&idxIterButtonClick);
		if (idxIterButtonClick == idxButtonClick)
		{
			m_pButtons[idxButton]->Disable ();
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CIGMenuBar::AddDropTarget (IDropTarget *pDropTarget)
{
	if (!m_hWnd)
		return E_FAIL;
	if (!pDropTarget)
		return E_INVALIDARG;
	if (!pDropTarget)
		return E_INVALIDARG;
	IGDropTarget *pIGTarget = dynamic_cast <IGDropTarget*> (pDropTarget);	
	if (!pIGTarget)
		return E_INVALIDARG;
	initializeController (m_hWnd, pIGTarget);
	return S_OK;
}

LRESULT CIGMenuBar::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	HWND hButton;
	HWND hAxButton;
	HWND hSubMenu;
	LONG nIdxSubMenu = 0;
	// avoid vertical menubar to kill his own children
	if (m_bVertical)
	{
		hAxButton = ::GetParent ((HWND)wParam);
		if (m_hWnd == ::GetParent (hAxButton))
			return 0;
	}

	// check if the kill focus message comes from a submenu
	for (unsigned int idxSubMenu = 0; idxSubMenu < m_vSubMenus.size(); idxSubMenu++)
	{
		m_vSubMenus [idxSubMenu]->get_IdxSubMenu (&nIdxSubMenu);
		CComQIPtr <IOleWindow> spWindow (m_vSubMenus [idxSubMenu]);
		spWindow->GetWindow (&hSubMenu);
		if ((hSubMenu == (HWND)wParam))// && (nIdxSubMenu == (int)lParam))
			return 0;
	}

	if (m_pButtons)
	{
		// check if the kill focus message comes from a subbutton
		for (int nIdxButton = 0; nIdxButton < m_nNbButtons; nIdxButton++)
		{
			m_pButtons [nIdxButton]->get_IdxSubMenu (&nIdxSubMenu);
			CComQIPtr <IOleWindow> spWindow (m_pButtons [nIdxButton]);
			spWindow->GetWindow (&hButton);
			hAxButton = ::GetParent(hButton);
			// if the focus killer is a subbutton of the button that has lost focus
			// then maintain submenu visibility
			if ((hAxButton == (HWND)wParam) && (nIdxSubMenu == (int)lParam))
				return 0;
		}	
	}
	// no submenu is the child of the button that took the focus
	// so hide them
	hideSubMenus();
	// forward the message to the parent menu 'cause the current submenu is
	// maybe to be hidden as well
	if (m_hMenuBarParent)
		::PostMessage (m_hMenuBarParent, uMsg, wParam, (LPARAM)m_idxSubMenu);
	
	return 0;	
}

LRESULT CIGMenuBar::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (HIWORD(wParam))
	{		
	case IGM_COMMAND:
		// post command button message to parent
		hideSubMenus();
		if (m_hMenuBarParent)
			::PostMessageW (m_hMenuBarParent, WM_COMMAND, wParam, lParam);
		break;

	case IGM_SUBMENU:
		// open submenu, hide other submenus
		LONG  nSubMenu = (LONG)lParam;
		HWND hSubMenu;
		for (unsigned int idxSubMenu = 0; idxSubMenu < m_vSubMenus.size(); idxSubMenu++)
		{
			if (nSubMenu == idxSubMenu)
			{
				RECT rc;
				::GetWindowRect (m_hMenuBarParent, &rc);
				m_vSubMenus [idxSubMenu]->ShowPopup (rc.left, rc.top);
			}
			else
			{
				LONG idxButton;
				m_vSubMenus [idxSubMenu]->get_IdxButton (&idxButton);
				m_pButtons [idxButton]->UnSelect();
				CComQIPtr <IOleWindow> spWindow (m_vSubMenus [idxSubMenu]);
				spWindow->GetWindow (&hSubMenu);
				::ShowWindow (hSubMenu, SW_HIDE);
			}
		}
		break;
	}
	bHandled = TRUE;	
	return 0;
}

LRESULT CIGMenuBar::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rc;
	::GetWindowRect (m_hWnd, &rc);
	int nWidth = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;
	rc.left = rc.top = 0;
	Pen penSeparator(IGMENUBAR_COLOR_SEPARATOR, 1);
	Graphics graphics ((HDC)wParam);
	if (m_bVertical)
	{
		LinearGradientBrush linGrBrush (Point(0,0), Point(IGMENUBAR_DROP_BUTTONHEIGHT,0),
										IGMENUBAR_COLOR_DROPGRADIENTLEFT, IGMENUBAR_COLOR_DROPGRADIENTMIDDLE);	
		graphics.FillRectangle (&linGrBrush, Rect (0, 0,
													IGMENUBAR_DROP_BUTTONHEIGHT, nHeight));
		SolidBrush rightBrush (IGMENUBAR_COLOR_DROPRIGHT);
		graphics.FillRectangle (&rightBrush, Rect (IGMENUBAR_DROP_BUTTONHEIGHT, 0,
													nWidth - IGMENUBAR_DROP_BUTTONHEIGHT, nHeight));
		int nCurTop = 0;
		int nCurBottom = 0;
		int nCurHeight = 0;
		int nCurWidth = nWidth - IGMENUBAR_DROP_BUTTONHEIGHT;
		bool bLastPart = (m_vSeparators.size() == 0) ? true : false;
		for (unsigned int idxSeparator = 0; idxSeparator < m_vSeparators.size() ||
											bLastPart	; idxSeparator++)
		{
			nCurBottom = bLastPart ? m_nNbButtons * IGMENUBAR_DROP_BUTTONHEIGHT : (m_vSeparators[idxSeparator] + 1) * IGMENUBAR_DROP_BUTTONHEIGHT;
			nCurHeight = nCurBottom - nCurTop;
			GraphicsPath path;
			path.AddRectangle (Rect (IGMENUBAR_DROP_BUTTONHEIGHT, nCurTop,
									nCurWidth, nCurHeight));
			PathGradientBrush pthGrBrush (&path);
			pthGrBrush.SetCenterPoint (PointF ((float)IGMENUBAR_DROP_BUTTONHEIGHT + 0.7f * (float)nCurWidth,
											  (float)nCurTop + 0.3f * (float)nCurHeight));
			pthGrBrush.SetCenterColor (IGMENUBAR_COLOR_CENTERLIGHT);
			Color colors[] = {IGMENUBAR_COLOR_OUTLIGHT};
			int count = 1;
			pthGrBrush.SetSurroundColors(colors, &count);
			graphics.FillPath (&pthGrBrush, &path);
			graphics.DrawLine (&penSeparator, IGMENUBAR_DROP_BUTTONHEIGHT + 4, nCurBottom,
								nWidth - 5, nCurBottom);
			nCurTop = nCurBottom;
			if (idxSeparator == m_vSeparators.size() - 1 && !bLastPart)
				bLastPart = true;
			else
				bLastPart = false;
		}
	}
	else
	{
		if (!m_bTransparent)
		{
			LinearGradientBrush linGrBrush (Point(0, 0), Point(m_nMenuBarInitWidth, 0),
											IGMENUBAR_COLOR_GRADIENTLEFT, IGMENUBAR_COLOR_GRADIENTRIGHT);	
			graphics.FillRectangle (&linGrBrush, Rect (0, 0,
														m_nMenuBarInitWidth, nHeight));
			SolidBrush solBrush (IGMENUBAR_COLOR_GRADIENTRIGHT);
			graphics.FillRectangle (&solBrush, Rect (m_nMenuBarInitWidth, 0,
															nWidth - m_nMenuBarInitWidth, nHeight));
/*			for (int nIdxButton = 0; nIdxButton < m_nNbButtons; nIdxButton++)
			{
				CComQIPtr <IOleWindow> spWin (m_pButtons [nIdxButton]);
				HWND hWnd;
				spWin->GetWindow (&hWnd);
				::RedrawWindow (hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			}*/
		}
	}
	int nSeparatorPosX = (m_nButtonSize > 0) ? m_nButtonSize : (int)((float)nWidth / (float)m_nNbButtons);
	int nCurrentSeparatorPosX = nSeparatorPosX;
	Pen penSeparatorBlack(Color (255,0,0,0), 1);
	Pen penSeparatorWhite(Color (255,255,255,255), 1);
	if (m_bVertical)
	{
		graphics.DrawLine (&penSeparator, IGMENUBAR_DROP_BUTTONHEIGHT, 2,
								IGMENUBAR_DROP_BUTTONHEIGHT, nHeight - 4);		
		graphics.DrawRectangle (&penSeparatorWhite, Rect (rc.left + 1, rc.top + 1,
												nWidth - 3, nHeight - 3));
		graphics.DrawRectangle (&penSeparatorBlack, Rect (rc.left, rc.top,
												nWidth - 1, nHeight - 1));
	}/*
	else
	{
		for (int nIdxButton = 0; nIdxButton < m_nNbButtons - 1; nIdxButton++)
		{
			graphics.DrawLine (&penSeparator, nCurrentSeparatorPosX, 2,
								nCurrentSeparatorPosX, nHeight - 4);
			nCurrentSeparatorPosX += nSeparatorPosX;
		}
	}*/
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMenuBar::OnShow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if ((BOOL)wParam == FALSE)
		hideSubMenus();
	bHandled = TRUE;
	return 0;
}

HRESULT CIGMenuBar::OnDraw(ATL_DRAWINFO& di)
{	
	return S_OK;
}

LRESULT CIGMenuBar::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMenuBar::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = {m_hWnd, uMsg, wParam, lParam, 0, {0, 0}};
	preProcessingInput (msg);
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMenuBar::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return MA_ACTIVATEANDEAT;
}

void CIGMenuBar::hideSubMenus()
{
	HWND hSubMenu;
	for (unsigned int idxSubMenu = 0; idxSubMenu < m_vSubMenus.size(); idxSubMenu++)
	{
		LONG idxButton;
		m_vSubMenus [idxSubMenu]->get_IdxButton (&idxButton);
		m_pButtons [idxButton]->UnSelect();
		CComQIPtr <IOleWindow> spWindow (m_vSubMenus [idxSubMenu]);
		spWindow->GetWindow (&hSubMenu);
		// no submenu is the child of the button that took the focus
		// so hide them
		::ShowWindow (hSubMenu, SW_HIDE);
	}
}