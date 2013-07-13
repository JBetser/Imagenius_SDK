#pragma once

class IGMemDC
{
public:
	IGMemDC (HDC hdc, const LPRECT p_rcBounds) : m_pRcBounds (NULL)
	{
		if (hdc && p_rcBounds)
		{
			m_hdc = ::CreateCompatibleDC (hdc);
			m_bitmap = ::CreateCompatibleBitmap (hdc, p_rcBounds->right - p_rcBounds->left, 
													p_rcBounds->bottom - p_rcBounds->top);
			m_oldBitmap = ::SelectObject (m_hdc, m_bitmap);
			m_hdc = hdc;
			m_pRcBounds = p_rcBounds;
		}
	}

	BOOL BitBlt (int nLeft, int nTop, int nWidth, int nHeight, 
				HDC hDC, int nScrLeft, int nDestLeft, DWORD op = SRCCOPY)
	{
		return ::BitBlt (m_hdc, nLeft, nTop, nWidth, nHeight, 
							hDC, nScrLeft, nDestLeft, op);
	}

	BOOL StretchBlt (int nLeft, int nTop, int nWidth, int nHeight, 
					 HDC hDC, int nScrLeft, int nDestLeft, int nDestWidth, int nDestHeight, DWORD op = SRCCOPY)
	{
		return ::StretchBlt (m_hdc, nLeft, nTop, nWidth, nHeight, 
							hDC, nScrLeft, nDestLeft, nDestWidth, nDestHeight, op);
	}

	BOOL FrameRect (const RECT *lprc, HBRUSH hbr)
	{
		HGDIOBJ hOldBrush = ::SelectObject (m_hdc, hbr);
		BOOL bRet = ::FrameRect (m_hdc, lprc, hbr);
		::SelectObject (m_hdc, hOldBrush);
		return bRet;
	}

	COLORREF GetPixel (int xPos, int yPos)
	{
		return ::GetPixel (m_hdc, xPos, yPos);
	}

	~IGMemDC() 
	{
		::SelectObject (m_hdc, m_oldBitmap);
		if (m_bitmap) 
			::DeleteObject (m_bitmap);
		if (m_hdc)
			::DeleteDC (m_hdc);
	}

	IGMemDC* operator->() 
	{
		return this;
	}

private:
	HBITMAP			m_bitmap;
	HGDIOBJ			m_oldBitmap;
	HDC				m_hdc;
	LPRECT			m_pRcBounds;
};