#pragma once

namespace IGLibrary
{

class IGLayout
{
	friend class IGFrameManager;
public:
	IGLayout ();
	virtual ~IGLayout();

	// get frame coordinates, in pixels
	int GetFrameWidth() const;
	int GetFrameHeight() const;
	int GetFrameLeft() const;
	int GetFrameTop() const;

	// Layout dimensions (independent from multiframe dimensions) 
	void SetLayout (float fLeft, float fTop, float fWidth, float fHeight, bool bTry = false);
	void GetLayout (float &fLeft, float &fTop, float &fWidth, float &fHeight, bool bTry = false) const;

	// Compute frame position and dimension from current layout and view
	void UpdateFrameSize();

protected:
	// Set the size in pixels of the multiframe panel
	void SetMultiFrameSize (int nWidth, int nHeight);

	// Adding order in the frame tree
	int GetOrder () const;
	void SetOrder (int nOrder);

	// set Try values equal to layout values
	void ResetTry ();

	// get frame coordinates, in frame ratio (1.0f = whole multiframe)
	float GetFrameWidthF() const;
	float GetFrameHeightF() const;
	float GetFrameLeftF() const;
	float GetFrameTopF() const;

	void GetLayoutSize (int &nWidth, int &nHeight) const;

	// set frame coordinates, in frame ratio (1.0f = whole multiframe)
	// this computes corresponding frame coordinates in pixels
	void SetFrameWidthF (float fWidth);
	void SetFrameHeightF (float fHeight);
	void SetFrameLeftF (float fLeft);	
	void SetFrameTopF (float fTop);

	int									m_nTop;
	int									m_nLeft;
	int									m_nWidth;
	int									m_nHeight;
	int									m_nZoomWidth;
	int									m_nZoomHeight;
	int									m_nZoomPosX;
	int									m_nZoomPosY;
	DWORD								m_dwRenderTickCount;

private:
	// Compute layout dimension from frame dimensions
	// Layout dimension, by definition must have their surface S = 1.0f
	void computeLayoutSize (int nWidth, int nHeight);

	// get closest int value
	int approxFloat (float fValue);
	
	int									m_nOrder;	
	float								m_fTop;
	float								m_fLeft;
	float								m_fWidth;
	float								m_fHeight;
	int									m_nMultiFrameWidth;
	int									m_nMultiFrameHeight;
	// Layout dimensions (independent from multiframe dimensions)
	float								m_fLayoutTop;
	float								m_fLayoutLeft;
	float								m_fLayoutWidth;
	float								m_fLayoutHeight;
	// Layout "Try" coordinates (used to compute best frame site) 
	float								m_fLayoutTryTop;
	float								m_fLayoutTryLeft;
};

inline int IGLayout::GetFrameLeft() const
{
	return m_nLeft;
}

inline int IGLayout::GetFrameTop() const
{
	return m_nTop;
}

inline int IGLayout::GetFrameWidth() const
{
	return m_nWidth;
}

inline int IGLayout::GetFrameHeight() const
{
	return m_nHeight;
}

}
