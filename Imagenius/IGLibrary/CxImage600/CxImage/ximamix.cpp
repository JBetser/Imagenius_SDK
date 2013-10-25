#include "stdafx.h"
#include "ximamix.h"
#include "ximage.h"

RGBQUAD CXIMAGEMIXING_DARKENING (RGBQUAD rgbq, BYTE val)
{
	rgbq = CxImage::RGBtoHSL (rgbq);
	rgbq.rgbBlue = (BYTE)(255.0f * (((float)val / 255.0f) * ((float)rgbq.rgbBlue / 255.0f)));
	return CxImage::HSLtoRGB (rgbq);
}