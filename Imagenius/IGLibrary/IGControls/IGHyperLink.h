#pragma once

#include "../IGUserMessages.h"
#include "../IGLibrary.h"
#include <string>

namespace IGLibrary
{

// IGHyperlink static window
class IGHyperlink
{
public:
	 enum UnderLineOptions { ulHover = -1, ulNone = 0, ulAlways = 1};

	 // Construction/destruction
    IGHyperlink();
    virtual ~IGHyperlink();

	static HINSTANCE GotoURL (LPCTSTR url, int showcmd,  BOOL bAlwaysOpenNew = FALSE);

	BOOL Create (const RECT& rect, HWND hParent);
	BOOL Destroy ();

	void SetWindowText(LPCTSTR lpszString);
    void SetURL(std::wstring strURL);
	std::wstring GetURL() const;
	void EnableURL(BOOL bFlag);
	BOOL IsURLEnabled() const;
    void SetColours(COLORREF crLinkColour, 
					COLORREF crVisitedColour, 
                    COLORREF crHoverColour = -1);
	COLORREF GetLinkColour() const;
	COLORREF GetVisitedColour() const;
	COLORREF GetHoverColour() const;
	void SetBackgroundColour(COLORREF crBackground);
	COLORREF GetBackgroundColour() const;
    void SetVisited(BOOL bVisited = TRUE);
	BOOL GetVisited() const;
    void SetLinkCursor(HCURSOR hCursor);
	HCURSOR IGHyperlink::GetLinkCursor() const;
    void SetUnderline(int nUnderline = ulHover);
	int GetUnderline() const;
    void SetAutoSize(BOOL bAutoSize = TRUE);
	BOOL GetAutoSize() const;
	void SetNotifyParent(BOOL bFlag);
	BOOL GetNotifyParent() const;
	void EnableTooltip(BOOL bFlag);
	BOOL IsTooltipEmabled() const;
	void SetAlwaysOpenNew(BOOL bFlag); 
	BOOL GetAlwaysOpenNew() const; 

//    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	static LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
	void NotifyParent();
	void PositionWindow();
	void ReportError(int nError);
	void SetDefaultCursor();

    HBRUSH CtlColor (HDC hdc, UINT nCtlColor);
    BOOL OnSetCursor ();
    void OnMouseMove (UINT nFlags, LPPOINT point);
	BOOL OnEraseBkgnd (HDC hdc);
	void OnTimer (UINT nIDEvent);
    void OnClicked ();

private:
	static LRESULT APIENTRY StaticHookWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
												UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	LRESULT HookWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND			m_hWnd;
    COLORREF		m_crLinkColour;		// Hyperlink colours
	COLORREF		m_crVisitedColour;
    COLORREF		m_crHoverColour;	// Hover colour
	COLORREF		m_crBackground;		// background color
	HBRUSH			m_Brush;			// background brush
    BOOL			m_bOverControl;		// cursor over control?
    BOOL			m_bVisited;			// Has it been visited?
    int				m_nUnderline;		// underline hyperlink?
    BOOL			m_bAdjustToFit;		// Adjust window size to fit text?
    std::wstring	m_strURL;			// hyperlink URL
    HFONT			m_UnderlineFont;	// Font for underline display
    HFONT			m_StdFont;			// Standard font
	HFONT			m_currentFont;
    HCURSOR			m_hLinkCursor;		// Cursor for hyperlink
    //CToolTipCtrl	m_ToolTip;			// The tooltip
    UINT			m_nTimerID;
	BOOL			m_bIsURLEnabled;	// TRUE = navigate to url
	BOOL			m_bNotifyParent;	// TRUE = notify parent
	BOOL			m_bToolTip;			// TRUE = display tooltip
	BOOL			m_bAlwaysOpenNew;	// TRUE = always open new browser window
};

inline std::wstring IGHyperlink::GetURL() const
{ 
	return m_strURL; 
}

inline void IGHyperlink::EnableURL(BOOL bFlag) { m_bIsURLEnabled = bFlag; }
inline BOOL IGHyperlink::IsURLEnabled() const { return m_bIsURLEnabled; }

inline COLORREF IGHyperlink::GetLinkColour() const
{ 
	return m_crLinkColour; 
}

inline COLORREF IGHyperlink::GetVisitedColour() const
{
	return m_crVisitedColour; 
}

inline COLORREF IGHyperlink::GetHoverColour() const
{
	return m_crHoverColour;
}

inline COLORREF IGHyperlink::GetBackgroundColour() const
{
	return m_crBackground;
}

inline BOOL IGHyperlink::GetVisited() const
{ 
	return m_bVisited; 
}

inline HCURSOR IGHyperlink::GetLinkCursor() const
{
	return m_hLinkCursor;
}

int IGHyperlink::GetUnderline() const
{ 
	return m_nUnderline; 
}

inline BOOL IGHyperlink::GetAutoSize() const
{ 
	return m_bAdjustToFit; 
}

inline void IGHyperlink::SetNotifyParent(BOOL bFlag) { m_bNotifyParent = bFlag; }
inline BOOL IGHyperlink::GetNotifyParent() const { return m_bNotifyParent; }

inline void IGHyperlink::EnableTooltip(BOOL bFlag) 
{ 
	m_bToolTip = bFlag; 
//	m_ToolTip.Activate(m_bToolTip);
}

inline BOOL IGHyperlink::IsTooltipEmabled() const 
{ 
	return m_bToolTip; 
}

inline void IGHyperlink::SetAlwaysOpenNew(BOOL bFlag) 
{ 
	m_bAlwaysOpenNew = bFlag; 
}

inline BOOL IGHyperlink::GetAlwaysOpenNew() const 
{ 
	return m_bAlwaysOpenNew; 
}

}	// namespace IGLibrary