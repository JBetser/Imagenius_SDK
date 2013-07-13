#include "stdafx.h"
#include "IGMainWindow.h"
#include <IGIPFilter.h>

using namespace IGLibrary;

LRESULT CIGMainWindow::OnToolsResize(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsRotate(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsBrightness(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsContrast(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsColor(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsRedeye(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsHistogramEqualize(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsFilterErode(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsFilterDilate(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsFilterBlur(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	OLE_HANDLE hFrame = NULL;
	m_spWorkspace->GetActiveFrame (&hFrame);
	if (!hFrame)
		return FALSE;
	IGIPFilter *pFilterImageProcessing = new IGIPFilter ((IGFrame*)hFrame, 
														  new IGIPFilterMessage (IGIPFILTER_BLUR));
	pFilterImageProcessing->Start();
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsFilterGaussian(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsFilterGradient(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

LRESULT CIGMainWindow::OnToolsFilterZerocross(WORD wCode, WORD wId, HWND hWnd, BOOL &bHandled)
{
	bHandled = TRUE;
	return TRUE;
}

