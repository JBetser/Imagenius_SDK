#include "stdafx.h"
#include "ximamix.h"
#include "ximage.h"

RGBQUAD CXIMAGEMIXING_ALPHA (RGBQUAD rgbqSrc, RGBQUAD rgbqDst)
{
	float fAlphaSrc = (float)rgbqSrc.rgbReserved / 255.0f;
	float fAlphaOpSrc = 1.0f - fAlphaSrc;
	float fAlphaDst = (float)rgbqDst.rgbReserved / 255.0f;

	if (rgbqDst.rgbReserved)
	{
		rgbqDst.rgbRed = (BYTE) (fAlphaOpSrc * ((float)rgbqDst.rgbRed) + fAlphaSrc * ((float)rgbqSrc.rgbRed));
		rgbqDst.rgbGreen = (BYTE) (fAlphaOpSrc * ((float)rgbqDst.rgbGreen) + fAlphaSrc * ((float)rgbqSrc.rgbGreen));
		rgbqDst.rgbBlue = (BYTE) (fAlphaOpSrc * ((float)rgbqDst.rgbBlue) + fAlphaSrc * ((float)rgbqSrc.rgbBlue));
		rgbqDst.rgbReserved = (BYTE)min (255.0f, 255.0f * (fAlphaDst + fAlphaSrc));
	}
	else
	{
		rgbqDst.rgbRed = rgbqSrc.rgbRed;
		rgbqDst.rgbGreen = rgbqSrc.rgbGreen;
		rgbqDst.rgbBlue = rgbqSrc.rgbBlue;
		rgbqDst.rgbReserved = (BYTE)min (255.0f, 255.0f * fAlphaSrc);
	}
	return rgbqDst;
}

RGBQUAD CXIMAGEMIXING_DARKENING (RGBQUAD rgbqSrc, RGBQUAD rgbqVal)
{
	rgbqVal = CxImage::RGBtoHSL (rgbqVal);
	rgbqSrc = CxImage::RGBtoHSL (rgbqSrc);
	rgbqSrc.rgbBlue = (BYTE)(255.0f * (((float)rgbqVal.rgbBlue / 255.0f) * ((float)rgbqSrc.rgbBlue / 255.0f)));
	return CxImage::HSLtoRGB (rgbqSrc);
}

