#include "stdafx.h"

#include "IGTool.h"

using namespace IGLibrary;

IGToolRubber::IGToolRubber (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton) : IGTool (hOwner, spToolBox, IGTOOLID_RUBBER, rcButton, IDB_RUBBER)
{	
}

IGToolRubber::~IGToolRubber()
{
}

LRESULT IGToolRubber::OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}

LRESULT IGToolRubber::OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}

LRESULT IGToolRubber::OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0L;
}
