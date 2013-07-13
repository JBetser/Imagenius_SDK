#pragma once

typedef struct _CxLayerCache
{
	_CxLayerCache::_CxLayerCache (struct _CxLayerCache *pCopy = NULL) : m_bLayerDrawAllInCache (true)
	{
		if (!pCopy || !pCopy->m_hCacheMemBmp || !pCopy->m_hCacheMemDC)
		{
			m_hCacheMemDC = NULL;
			m_hCacheMemBmp = NULL;
			m_hCacheOldMemBmp = NULL;
		}
		else
		{
			m_hCacheMemDC = ::CreateCompatibleDC (pCopy->m_hCacheMemDC);
			BITMAP bitmapInfo;
			::GetObject (pCopy->m_hCacheMemBmp, sizeof (BITMAP), &bitmapInfo);
			m_hCacheMemBmp = (HBITMAP)::CopyImage (pCopy->m_hCacheMemBmp, IMAGE_BITMAP, bitmapInfo.bmWidth, bitmapInfo.bmHeight, LR_CREATEDIBSECTION);
			m_hCacheOldMemBmp = ::SelectObject (m_hCacheMemDC, m_hCacheMemBmp);
		}
	}

	_CxLayerCache::~_CxLayerCache()
	{
		if (m_hCacheMemDC && m_hCacheMemBmp && m_hCacheOldMemBmp)
		{
			::SelectObject (m_hCacheMemDC, m_hCacheOldMemBmp);
			::DeleteDC (m_hCacheMemDC);
			::DeleteObject (m_hCacheMemBmp);
		}
	}

	HDC					m_hCacheMemDC;
	HBITMAP				m_hCacheMemBmp;
	HGDIOBJ				m_hCacheOldMemBmp;
	bool				m_bLayerDrawAllInCache;
}	CxLayerCache;