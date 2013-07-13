// IGLibrary.cpp : implements image library helpers

#include "stdafx.h"
#include "IGLibPicture.h"
#include "IGUserMessages.h"
#include "IGPacker.h"
#include "IGFrame.h"
#include "IGLayer.h"
#include <string>

using namespace IGLibrary;

IGLibPicture::IGLibPicture (std::wstring wsPath,
							int nWidth,
							int nHeight,
							IGSmartPtr <IGSplashWindow> spProgress)	: m_wsPath (wsPath)
																	, m_hBitmap (NULL)


{
	if (createHBITMAP (nWidth, nHeight, spProgress))
	{
		encodeBase64();
	}
}

IGLibPicture::IGLibPicture (std::wstring wsPath,
							CComBSTR bstrCompressedBmpBase64)	: m_wsPath (wsPath)
																, m_bstrMiniPictureBase64 (bstrCompressedBmpBase64)
																, m_hBitmap (NULL)
																, m_spSizedImage (new IGFrame (IGFrame::GetImageType (wsPath.c_str())))
{
	if (m_spSizedImage)
	{
		decodeBase64();
	}
}

IGLibPicture::~IGLibPicture ()
{
	if (m_hBitmap)
		::DeleteObject (m_hBitmap);
}

IGLibPicture::IGLibPicture (const IGLibPicture &picture) 
{
	m_wsPath = picture.m_wsPath;
	if (picture.m_hBitmap && picture.m_bstrMiniPictureBase64 && picture.m_spSizedImage)
	{
		m_spSizedImage = new IGFrame (*picture.m_spSizedImage, false);
		IGLayer *pLayer = m_spSizedImage->GetLayer (0);
		if (pLayer)
			m_hBitmap = pLayer->MakeBitmap (NULL, -1, -1, true, false);
		m_bstrMiniPictureBase64 = picture.m_bstrMiniPictureBase64;
	}
	else
	{
		m_hBitmap = NULL;
	}
}

bool IGLibPicture::createHBITMAP (int nWidth, int nHeight, IGSmartPtr <IGSplashWindow> spProgress)
{
	if (m_hBitmap)
		return false;
	DWORD dwType = IGFrame::GetImageType (m_wsPath.c_str());
	if (dwType == CXIMAGE_FORMAT_UNKNOWN)
		return false;
	if (!m_spSizedImage)
		m_spSizedImage = new IGFrame (spProgress, m_wsPath.c_str(), false);
	int nItemWidth = m_spSizedImage->GetWidth();
	int nItemHeight = m_spSizedImage->GetHeight();
	float fRatio = 1.0f;
	if ((float)nWidth / (float)nHeight > (float)nItemWidth / (float)nItemHeight)
	{
		fRatio = (float)nHeight / (float)nItemHeight;
		nItemWidth = (int)((float)nItemWidth * fRatio);			
		nItemHeight = nHeight;
	}
	else
	{
		fRatio = (float)nWidth / (float)nItemWidth;
		nItemWidth = nWidth;
		nItemHeight = (int)((float)nItemHeight * fRatio);		
		// increment progress step by number of missing lines
		spProgress->ProgressSetStep (nHeight-nItemHeight);
		spProgress->ProgressStepIt();
		spProgress->ProgressSetStep (1);
	}
	m_spSizedImage->Resample2 (nItemWidth, nItemHeight);
	return convertImageToBitmap ();
}

bool IGLibPicture::createHBITMAP (HINSTANCE hInstance, LPCWSTR lpResName, int nWidth, int nHeight, HWND hProgress)
{
	if (m_hBitmap)
		return false;
	DWORD dwType = IGFrame::GetImageType (lpResName);
	if (dwType == CXIMAGE_FORMAT_UNKNOWN)
		return false;
	HRSRC hRsrc = ::FindResource (  hInstance,
									lpResName,
									L"Library"	); 
	if (!hRsrc)
		return false;
	if (!m_spSizedImage)
		m_spSizedImage = new IGFrame (hRsrc, dwType, hInstance, hProgress, false);
	m_spSizedImage->Resample2 (nWidth, nHeight);
	return convertImageToBitmap ();
}

bool IGLibPicture::convertImageToBitmap ()
{
	if (!m_spSizedImage)
		return false;
	int nImageWidth = m_spSizedImage->GetWidth ();
	int nImageHeight = m_spSizedImage->GetHeight ();
	if (nImageWidth <= 0 || nImageHeight <= 0)
		return false;
	// convert the image to a bitmap
	m_hBitmap = m_spSizedImage->MakeBitmap (NULL, -1, -1, true, false);
	return (m_hBitmap != NULL);
}

bool IGLibPicture::encodeBase64 ()
{
	if (!m_hBitmap || m_bstrMiniPictureBase64 || !m_spSizedImage)
		return false;
	BYTE *pBuffer = NULL;
	LONG szBuffer = 0;
	if (!m_spSizedImage->Encode (pBuffer, szBuffer, CXIMAGE_FORMAT_IG))
		return false;
	HRESULT hr = IGPacker::encode64 (pBuffer, szBuffer, m_bstrMiniPictureBase64);
	delete [] pBuffer;
	if (FAILED (hr))
		return false;
	return true;
}

bool IGLibPicture::decodeBase64 ()
{
	if (m_hBitmap || !m_bstrMiniPictureBase64 || !m_spSizedImage)
		return false;
	char *pDecodedBuffer = NULL;
	int decodedBufferLen = IGPacker::decode64 (m_bstrMiniPictureBase64, pDecodedBuffer);
	if (decodedBufferLen <= 0)
		return false;
	if (!m_spSizedImage->Decode ((BYTE*)pDecodedBuffer, decodedBufferLen, CXIMAGE_FORMAT_IG))
		return false;
	delete [] pDecodedBuffer;
	m_hBitmap = m_spSizedImage->MakeBitmap (NULL, -1, -1, true, false);
	if (!m_hBitmap)
		return false;
	return true;
}