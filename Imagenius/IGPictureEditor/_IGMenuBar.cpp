// IGMenuBar.cpp : Implementation of CIGMenuBar
#include "stdafx.h"
#include "IGMenuBar.h"

#include <msxml2.h>


// CIGMenuBar
STDMETHODIMP CIGMenuBar::CreateFrame(OLE_HANDLE hWnd, LPRECT prc, BSTR xmlConfig)
{
	ATLASSERT (0);/*
	_U_RECT urc (prc);
	HRESULT hr = CComControl<CIGMenuBar>::Create ((HWND)hWnd, urc) ? S_OK : E_FAIL;
	if (FAILED(hr))
		return E_FAIL;*/

	CComPtr <IXMLDOMDocument> spXMLDoc;
	HRESULT hr = ::CoCreateInstance(CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
		   IID_IXMLDOMDocument, (void**)&spXMLDoc);
	if (FAILED(hr))
		return E_FAIL;

	VARIANT_BOOL bSucceeded;
	hr = spXMLDoc->loadXML (xmlConfig, &bSucceeded);
	if ((hr != S_OK) || !bSucceeded)
		return E_FAIL;
 
	CComPtr <IXMLDOMNodeList> spNodeMenuButtons;
	hr = spXMLDoc->selectNodes (CComBSTR("//button"), &spNodeMenuButtons);
	if ((hr != S_OK) || !spNodeMenuButtons)
		return E_FAIL;

	spNodeMenuButtons->get_length ((long*)&m_nNbButtons);
	RECT rc;
	GetWindowRect (&rc);
	int nButtonSizeX = (int)((float)(rc.right - rc.left) / (float)m_nNbButtons);
	int nButtonSizeY = (int)((float)(rc.bottom - rc.top));
	int nButtonPos = 0;
	IXMLDOMNode *pNodeMenuButton;
	do
	{
		spNodeMenuButtons->nextNode (&pNodeMenuButton);
		if (pNodeMenuButton)
		{/*
			CComPtr <IXMLDOMNamedNodeMap> spAttributes;
			spNodeMenuButton->get_attributes (&spAttributes);
			spAttributes->getNamedItem (CComBSTR (*/
			CComBSTR spButtonName;
			pNodeMenuButton->get_nodeName (&spButtonName);
			HWND hButton = ::CreateWindow (L"BUTTON", spButtonName, 
											WS_CHILD | WS_VISIBLE, nButtonPos, 0, nButtonSizeX, nButtonSizeY,
											(HWND)hWnd, NULL, getInstance(), NULL);
			::EnableWindow (hButton, TRUE);
		}
		nButtonPos += nButtonSizeX;
	}
	while (pNodeMenuButton);
	return S_OK;
}

/*
HRESULT CIGMenuBar::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	// Set Clip region to the rectangle specified by di.prcBounds
	HRGN hRgnOld = NULL;
	if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
		hRgnOld = NULL;
	bool bSelectOldRgn = false;

	HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

	if (hRgnNew != NULL)
	{
		bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
	}

	if (!m_nNbButtons)
	{
		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ERROR: NO BUTTON IN MENUBAR CONFIG");

		TextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			pszText,
			lstrlen(pszText));
	}

	if (bSelectOldRgn)
		SelectClipRgn(di.hdcDraw, hRgnOld);

	return S_OK;
}*/