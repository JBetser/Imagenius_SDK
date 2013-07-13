#pragma once
#include <string>
#include "ximage.h"
#include "IGSmartPtr.h"
#include <atlbase.h>
//#include <atlcom.h>
//#include <atlctl.h>

namespace IGLibrary
{
class IGFrame;
class IGSplashWindow;
// contains mini picture library helper

class IGLibPicture
{
public:
	IGLibPicture (	std::wstring wsPath,
					int nWidth,
					int nHeight,
					IGSmartPtr <IGSplashWindow> spProgress);
	IGLibPicture (	std::wstring wsPath,
					CComBSTR bstrCompressedBmpBase64);
	IGLibPicture (	const IGLibPicture &picture);
	~IGLibPicture ();

	std::wstring	m_wsPath;
	HBITMAP			m_hBitmap;
	CComBSTR	m_bstrMiniPictureBase64;	// PNG compressed

	bool operator < (const IGLibPicture &comparedPicture)
	{
		return m_wsPath < comparedPicture.m_wsPath;
	}

	bool operator == (const IGLibPicture &comparedPicture)
	{
		return m_wsPath == comparedPicture.m_wsPath;
	}

private:	
	bool createHBITMAP (int nWidth, int nHeight, IGSmartPtr <IGSplashWindow> spProgress);
	bool createHBITMAP (HINSTANCE hInstance, LPCWSTR lpResName, int nWidth, int nHeight, HWND hProgress);
	bool encodeBase64 ();
	bool decodeBase64 ();
	bool convertImageToBitmap ();

	IGSmartPtr <IGFrame> m_spSizedImage;
};

}