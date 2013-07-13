#include "stdafx.h"
#include "ximage.h"

#if CXIMAGE_SUPPORT_LAYERS
#if CXIMAGE_SUPPORT_WINDOWS

long CxImage::LayerDrawAllClearCache()
{
	
	

	if (!pSelectedLayerCache)
		return 0;
	pSelectedLayerCache->m_bLayerDrawAllInCache = true;
	return 1;
}

long CxImage::LayerDrawAllInClearedCache(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bShowCurrentBackground, COLORREF cBackgroundColor, COLORREF cTranspMask)
{
	
	

	if (!pSelectedLayerCache)
		return 0;
	if (pSelectedLayerCache->m_bLayerDrawAllInCache)
	{
		pSelectedLayerCache->m_bLayerDrawAllInCache = false;
		return LayerDrawAllInCache (hdc, x, y, cx, cy, pClipRect, bSmooth, bShowChessBackground, bShowCurrentBackground, cBackgroundColor, cTranspMask);
	}
	return 1;
}

long CxImage::LayerDrawAllInCache(HDC hdc, long x, long y, long cx, long cy, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bShowCurrentBackground, COLORREF cBackgroundColor, COLORREF cTranspMask)
{
	GdiFlush ();
	if (!pSelectedLayerCache)
		return 0;
	if (pSelectedLayerCache->m_hCacheMemDC)
	{
		::SelectObject (pSelectedLayerCache->m_hCacheMemDC, pSelectedLayerCache->m_hCacheOldMemBmp);
		::DeleteDC (pSelectedLayerCache->m_hCacheMemDC);
	}
	if (pSelectedLayerCache->m_hCacheMemBmp)
		::DeleteObject ((HGDIOBJ)pSelectedLayerCache->m_hCacheMemBmp);
	pSelectedLayerCache->m_hCacheMemDC = ::CreateCompatibleDC (hdc);
	int nBmpWidth = cx;
	int nBmpHeight = cy;

	// manage clipping
	if (pClipRect)
	{
		nBmpWidth = pClipRect->right - pClipRect->left;
		nBmpHeight = pClipRect->bottom - pClipRect->top;
	}
	else
	{
		if (nBmpWidth <= 0) 
			nBmpWidth = head.biWidth;
		if (nBmpHeight <= 0) 
			nBmpHeight = head.biHeight;
	}
	pSelectedLayerCache->m_hCacheMemBmp = ::CreateCompatibleBitmap (hdc, nBmpWidth, nBmpHeight);
	::SetBitmapDimensionEx (pSelectedLayerCache->m_hCacheMemBmp, nBmpWidth, nBmpHeight, NULL);
	pSelectedLayerCache->m_hCacheOldMemBmp = ::SelectObject (pSelectedLayerCache->m_hCacheMemDC, pSelectedLayerCache->m_hCacheMemBmp);

	// copy current DC background to cache
	if (bShowCurrentBackground)
	{
		::BitBlt (pSelectedLayerCache->m_hCacheMemDC, 0, 0, nBmpWidth, nBmpHeight,
					hdc, x, y, SRCCOPY);
	}
	else if (cBackgroundColor != CLR_INVALID)
	{
		// apply background color
		HBRUSH hBackgroundBrush = ::CreateSolidBrush (cBackgroundColor);
		HPEN hNullPen = ::CreatePen (PS_NULL, 1, RGB (0,0,0));
		HGDIOBJ hOldBrush = ::SelectObject (pSelectedLayerCache->m_hCacheMemDC, hBackgroundBrush);
		HGDIOBJ hOldPen = ::SelectObject (pSelectedLayerCache->m_hCacheMemDC, hNullPen);
		::Rectangle (pSelectedLayerCache->m_hCacheMemDC, 0, 0, nBmpWidth + 1, nBmpHeight + 1);
		::SelectObject (pSelectedLayerCache->m_hCacheMemDC, hOldBrush);
		::SelectObject (pSelectedLayerCache->m_hCacheMemDC, hOldPen);
		::DeleteObject (hBackgroundBrush);	
		::DeleteObject (hNullPen);	
	}

	// manage transparent color
	if ((cTranspMask != CLR_INVALID) || bShowChessBackground)
	{
		HDC hTmpDC = ::CreateCompatibleDC (hdc);
		HBITMAP hTmpBmp = ::CreateCompatibleBitmap (hdc, nBmpWidth, nBmpHeight);
		HGDIOBJ hOldTmpBmp = ::SelectObject (hTmpDC, hTmpBmp);

		if (bShowChessBackground)
		{
			HBRUSH hWhiteBrush = ::CreateSolidBrush (RGB (230, 230, 230));
			HPEN hNullPen = ::CreatePen (PS_NULL, 1, RGB (0,0,0));
			HGDIOBJ hOldPen = ::SelectObject (hTmpDC, hNullPen);
			HGDIOBJ hOldBrush = ::SelectObject (hTmpDC, hWhiteBrush);
			::Rectangle (hTmpDC, -1, -1, nBmpWidth + 1, nBmpHeight + 1);
			::SelectObject (hTmpDC, hOldBrush);
			::DeleteObject (hWhiteBrush);
			HBRUSH hGreyBrush = ::CreateSolidBrush (RGB (200, 200, 200));	
			hOldBrush = ::SelectObject (hTmpDC, hGreyBrush);
			int nDrawSizeX = 0;
			int nDrawSizeY = 0;
			int nMaxDrawSizeX = nBmpWidth + 8;
			int nMaxDrawSizeY = nBmpHeight + 8;
			bool bDrawX = true;
			bool bDrawY = true;
			while (nDrawSizeX < nMaxDrawSizeX)
			{
				while (nDrawSizeY < nMaxDrawSizeY)
				{
					if (bDrawY)	
						::Rectangle (hTmpDC, nDrawSizeX - 1, nDrawSizeY - 1, nDrawSizeX + 8, nDrawSizeY + 8);
					bDrawY = !bDrawY;
					nDrawSizeY += 8;
				}
				bDrawX = !bDrawX;
				bDrawY = bDrawX;
				nDrawSizeX += 8;
				nDrawSizeY = 0;
			}
			::SelectObject (hTmpDC, hOldPen);
			::DeleteObject (hNullPen);
			::SelectObject (hTmpDC, hOldBrush);
			::DeleteObject (hGreyBrush);
		}

		if (GetNumLayers() > 0)
		{
			if (LayerDrawAll (hTmpDC, x, y, cx, cy, pClipRect, bSmooth, false) == 0)
				return 0;
		}
		else
		{
			if (Draw (hTmpDC, x, y, cx, cy, pClipRect, bSmooth, false) == 0)
				return 0;
		}

		if (cTranspMask == CLR_INVALID)
			::BitBlt (pSelectedLayerCache->m_hCacheMemDC, 0, 0, nBmpWidth, nBmpHeight,
						hTmpDC, 0, 0, SRCCOPY);
		else
			::TransparentBlt (pSelectedLayerCache->m_hCacheMemDC, 0, 0, nBmpWidth, nBmpHeight,
						hTmpDC, 0, 0, nBmpWidth, nBmpHeight, (COLORREF)cTranspMask);

		::SelectObject (hTmpDC, hOldTmpBmp);
		::DeleteObject (hOldTmpBmp);
		::DeleteDC (hTmpDC);
		return 1;
	}
	return LayerDrawAll (pSelectedLayerCache->m_hCacheMemDC, x, y, cx, cy, pClipRect, bSmooth, false);
}

long CxImage::LayerDrawAllInCache(HDC hdc, const RECT& rect, RECT* pClipRect, bool bSmooth, bool bShowChessBackground, bool bShowCurrentBackground, COLORREF cBackgroundColor, COLORREF cTranspMask)
{
	return LayerDrawAllInCache (hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, pClipRect, bSmooth, bShowChessBackground, bShowCurrentBackground, cBackgroundColor, cTranspMask);
}

// This method is thread safe and does not need access checking
// Because : - only the UI thread can acces it
//			 - it is not accessing memory shared with image processing thread
long CxImage::LayerDrawAllFromCache (HDC hdc, int nDestX, int nDestY, unsigned char ucAlpha, COLORREF cTranspMask)
{
	if (!ucAlpha || !pSelectedLayerCache || 
		!pSelectedLayerCache->m_hCacheMemDC || !pSelectedLayerCache->m_hCacheMemBmp)
		return 0;
	SIZE szBitmap;
	::GetBitmapDimensionEx (pSelectedLayerCache->m_hCacheMemBmp, &szBitmap);
	// 32 bit transparency
	if (ucAlpha == 255)
	{
		if (cTranspMask == CLR_INVALID)
		{
			::BitBlt (hdc, nDestX, nDestY, szBitmap.cx, szBitmap.cy,
					pSelectedLayerCache->m_hCacheMemDC, 0, 0, SRCCOPY);
		}
		else
		{
			::TransparentBlt (hdc, nDestX, nDestY, szBitmap.cx, szBitmap.cy,
					pSelectedLayerCache->m_hCacheMemDC, 0, 0, szBitmap.cx, szBitmap.cy, (COLORREF)cTranspMask);
		}
	}
	else
	{
		LPBITMAPINFO pBitmapSrcInfo;
		if (!GetBitmapBits (pSelectedLayerCache->m_hCacheMemDC, pSelectedLayerCache->m_hCacheMemBmp, 0, 0, pBitmapSrcInfo))
			return 0;		
		HDC hTmpDC = ::CreateCompatibleDC (hdc);		
		HBITMAP hTmpBmp = ::CreateCompatibleBitmap (hdc, szBitmap.cx, szBitmap.cy);
		::SetBitmapDimensionEx (hTmpBmp, szBitmap.cx, szBitmap.cy, NULL);
		HGDIOBJ hOldTmpBmp = ::SelectObject (hTmpDC, hTmpBmp);
		::BitBlt (hTmpDC, 0, 0, szBitmap.cx, szBitmap.cy,
					hdc, 0, 0, SRCCOPY);	
		LPBITMAPINFO pBmpDst;
		if (GetBitmapBits (hTmpDC, hTmpBmp, 0, 0, pBmpDst))
		{
			RGBQUAD *pBmpSrcBits = pBitmapSrcInfo->bmiColors;
			RGBQUAD *pBmpDstBits = pBmpDst->bmiColors;
			if (ucAlpha == 128)
			{
				for (int idxPixel = 0; idxPixel < pBmpDst->bmiHeader.biWidth * pBmpDst->bmiHeader.biHeight; idxPixel++)
				{
					if ((pBmpSrcBits [idxPixel].rgbBlue == (cTranspMask & 0x00FF0000)) &&
						(pBmpSrcBits [idxPixel].rgbRed == (cTranspMask & 0x000000FF)) &&
						(pBmpSrcBits [idxPixel].rgbGreen == (cTranspMask & 0x0000FF00)))
						continue;
					pBmpDstBits [idxPixel].rgbBlue = (pBmpSrcBits [idxPixel].rgbBlue >> 1)
														+ (pBmpDstBits [idxPixel].rgbBlue >> 1);
					pBmpDstBits [idxPixel].rgbRed = (pBmpSrcBits [idxPixel].rgbRed >> 1)
														+ (pBmpDstBits [idxPixel].rgbRed >> 1);
					pBmpDstBits [idxPixel].rgbGreen = (pBmpSrcBits [idxPixel].rgbGreen >> 1)
														+ (pBmpDstBits [idxPixel].rgbGreen >> 1);
				}
			}
			else
			{
				float fSrcAlpha = (float)ucAlpha / 256.0f;
				float fDstAlpha = 1.0f - fSrcAlpha;
				for (int idxPixel = 0; idxPixel < pBmpDst->bmiHeader.biWidth * pBmpDst->bmiHeader.biHeight; idxPixel++)
				{
					if ((pBmpSrcBits [idxPixel].rgbBlue == (cTranspMask & 0x00FF0000)) &&
						(pBmpSrcBits [idxPixel].rgbRed == (cTranspMask & 0x000000FF)) &&
						(pBmpSrcBits [idxPixel].rgbGreen == (cTranspMask & 0x0000FF00)))
						continue;
					pBmpDstBits [idxPixel].rgbBlue = (BYTE)(fSrcAlpha * (float)pBmpSrcBits [idxPixel].rgbBlue
														+ fDstAlpha * (float)pBmpDstBits [idxPixel].rgbBlue);
					pBmpDstBits [idxPixel].rgbRed = (BYTE)(fSrcAlpha * (float)pBmpSrcBits [idxPixel].rgbRed
														+ fDstAlpha * (float)pBmpDstBits [idxPixel].rgbRed);
					pBmpDstBits [idxPixel].rgbGreen = (BYTE)(fSrcAlpha * (float)pBmpSrcBits [idxPixel].rgbGreen
														+ fDstAlpha * (float)pBmpDstBits [idxPixel].rgbGreen);
				}
			}
			::SetDIBitsToDevice ( hTmpDC,			// handle to DC
								  0,0,	
								  pBmpDst->bmiHeader.biWidth,
								  szBitmap.cy,
								  0,0,0,szBitmap.cy,
								  pBmpDst->bmiColors,	// array for bitmap bits
								  pBmpDst,		// bitmap data buffer
								  DIB_RGB_COLORS );
			delete [] pBmpDst;
		}
		delete [] pBitmapSrcInfo;
		::BitBlt (hdc, 0, 0, szBitmap.cx, szBitmap.cy,
					hTmpDC, 0, 0, SRCCOPY);
		::SelectObject (hTmpDC, hOldTmpBmp);
		::DeleteObject (hTmpBmp);
		::DeleteDC (hTmpDC);
	}
	return 1;
}

void CxImage::AddLayerCache(int nNbCaches)
{
	if (nNbCaches > 0)
	{
		while (nNbCaches--)
			lpLayerCaches.push_back (new CxLayerCache());
	}
}

long CxImage::SelectLayerCache(long position)
{
	
	

	if (position < 0 || position >= (long)lpLayerCaches.size())
		return 0;
	std::list<CxLayerCache*>::const_iterator iterCache = lpLayerCaches.begin();
	for (int nIdxCache = 0; nIdxCache < position; ++nIdxCache, ++iterCache)
	{
		if (iterCache == lpLayerCaches.end())
			return 0;
	}
	pSelectedLayerCache = *iterCache;
	return 1;
}
#endif //CXIMAGE_SUPPORT_WINDOWS
#endif //CXIMAGE_SUPPORT_LAYERS
////////////////////////////////////////////////////////////////////////////////