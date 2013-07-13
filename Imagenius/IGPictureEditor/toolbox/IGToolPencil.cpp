#include "stdafx.h"

#include "IGTool.h"

using namespace IGLibrary;

IGToolPencil::IGToolPencil (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton) : IGTool (hOwner, spToolBox, IGTOOLID_PENCIL, rcButton, IDB_PENCIL)
{	
}

IGToolPencil::~IGToolPencil()
{
}

LRESULT IGToolPencil::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}

LRESULT IGToolPencil::OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	return 0L;
}

LRESULT IGToolPencil::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}
