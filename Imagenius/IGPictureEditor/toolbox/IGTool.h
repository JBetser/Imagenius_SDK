#pragma once

#include "IGSimpleButton.h"

#define IGTOOLID_SELECTOR		1
#define IGTOOLID_CUTTERSQUARE	2
#define IGTOOLID_PENCIL			3
#define IGTOOLID_RUBBER			4

namespace IGLibrary
{

class IGTool
{
public:
	IGTool (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, int nToolId, RECT rcButton, int nIconId);
	virtual ~IGTool();

	bool IsSelected();
	int GetId();
	virtual void SetSelected (bool bSelected = true);

	LRESULT ProcessMsg (UINT uMsg, WPARAM wParam, LPARAM lParam);
		
protected:
	virtual LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
	virtual LRESULT OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
	virtual LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
	virtual LRESULT OnKeyDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	bool IsMouseDown();

	IIGFrame					*m_pFrame;
	
private:
	CComPtr <IIGToolBox>		m_spToolBox;
	CComPtr <IIGSimpleButton>	m_spButton;
	int							m_nToolId;
	bool						m_bMouseDown;
	bool						m_bToolDownStarted;	
};

class IGToolSelector : public IGTool
{
public:
	IGToolSelector (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton);
	virtual ~IGToolSelector();

protected:
	virtual LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

};

class IGToolCutterSquare : public IGTool
{
public:
	IGToolCutterSquare (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton);
	virtual ~IGToolCutterSquare();

	virtual void SetSelected (bool bSelected = true);

protected:
	virtual LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	int m_xPos1;
	int m_yPos1;

	static int			g_nNbRefs;
	static HCURSOR		g_hCursorCutter;
	static HCURSOR		g_hCursorCutterPlus;
	static HCURSOR		g_hCursorCutterMinus;
};

class IGToolPencil : public IGTool
{
public:
	IGToolPencil (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton);
	virtual ~IGToolPencil();

protected:
	virtual LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

class IGToolRubber : public IGTool
{
public:
	IGToolRubber (HWND hOwner, CComPtr <IIGToolBox> &spToolBox, RECT rcButton);
	virtual ~IGToolRubber();

protected:
	virtual LRESULT OnMouseUp (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseDown (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

}