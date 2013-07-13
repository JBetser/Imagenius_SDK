#pragma once
#include "IGConfigManager.h"
#include "IGSingleton.h"
#include "IGSmartPtr.h"
#include "IGSmartLayer.h"
#include <set>

namespace IGLibrary
{
class IGFrame;

class IGLayer : public IGSmartLayer
{
	friend class IGFrame;
	friend class CxImageIG;

public:
	IGLayer (HANDLE hMem, IGFrame *pFrame);
	IGLayer (HBITMAP hBmp, IGFrame *pFrame);
	IGLayer (LPWSTR pwPath, IGFrame *pFrame);
	IGLayer (IGFrame *pFrame, bool bAlpha = true, bool bSelection = true);
	virtual ~IGLayer (void);

	bool RequestThreadAccess();
	bool Render (HDC hdc, int nLeft, int nTop, int nWidth, int nHeight);

	void SetSelected (bool bSelect = true);	
	virtual bool GetSelected() const;
	void SetVisible (bool bVisible = true);
	bool GetVisible() const;
	void SetWorking();
	bool GetWorking() const;

	virtual RECT GetUpdateRect() const;

	// get Image size, in pixels
	int GetWidth() const;
	int GetHeight() const;

	// get layer offset from parent frame origin
	int GetXOffset() const;
	int GetYOffset() const;

	HANDLE CopyToHandle (bool bCopyAlpha = false);
	bool Load (const TCHAR* filename, bool bAutoRotate = false);
	
	bool SetId (int nId);
	bool HasBeen (int nId);
	void RemoveFromHistory (int nId);
	virtual LPCWSTR GetId() const {return (LPCWSTR)m_nId;} // LPCWSTR for compliance with Cx, actually it is int
	virtual bool Save(const TCHAR* filename, DWORD imagetype);

	bool Delete();
	bool SmartDelete();
	virtual bool AutoRotate();
	
	operator const CxImage*() const;
	operator CxImage*();

protected:
	// CxImage
	virtual long Draw (HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bDrawChessBackGround = false, bool bMixAlpha = true);
	virtual IGSmartPixel*	GetSegmentationLayer (IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM& eShowSegmType);

private:
	void setFrameOwner (IGFrame *pFrame, bool bAddCalc = true);
	inline void setProgressOwner (HWND hProgress);

	bool									m_bSelected;
	bool									m_bVisible;
	int										m_nId;
	IGSingleton <IGConfigManager>			m_spConfigMgr;
	std::set <int>							m_setHistoryLayerIds;
};

inline void IGLayer::SetSelected (bool bSelect)
{
	m_bSelected = bSelect;
}

inline bool IGLayer::GetSelected() const
{
	return m_bSelected;
}

inline void IGLayer::SetVisible (bool bVisible)
{
	m_bVisible = bVisible;
}

inline bool IGLayer::GetVisible() const
{
	return m_bVisible;
}

inline int IGLayer::GetWidth() const
{
	return CxImage::GetWidth();
}

inline int IGLayer::GetHeight() const
{
	return CxImage::GetHeight();
}

inline IGLayer::operator const CxImage*() const
{
	return this;
}

inline IGLayer::operator CxImage*()
{
	return this;
}

inline void IGLayer::setProgressOwner (HWND hProgress)
{
	info.hProgress = hProgress;
}

inline int IGLayer::GetXOffset() const
{
	return info.xOffset;
}

inline int IGLayer::GetYOffset() const
{
	return info.yOffset;
}

}
