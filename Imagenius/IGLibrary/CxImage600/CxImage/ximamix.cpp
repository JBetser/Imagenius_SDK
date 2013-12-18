#include "stdafx.h"
#include "ximamix.h"
#include "ximage.h"

#define ChannelBlend_Normal(A,B)     ((BYTE)(A))
#define ChannelBlend_Lighten(A,B)    ((BYTE)((B > A) ? B:A))
#define ChannelBlend_Darken(A,B)     ((BYTE)((B > A) ? A:B))
#define ChannelBlend_Multiply(A,B)   ((BYTE)((A * B) / 255))
#define ChannelBlend_Average(A,B)    ((BYTE)((A + B) / 2))
#define ChannelBlend_Add(A,B)        ((BYTE)(min(255, (A + B))))
#define ChannelBlend_Subtract(A,B)   ((BYTE)((A + B < 255) ? 0:(A + B - 255)))
#define ChannelBlend_Difference(A,B) ((BYTE)(abs(A - B)))
#define ChannelBlend_Negation(A,B)   ((BYTE)(255 - abs(255 - A - B)))
#define ChannelBlend_Screen(A,B)     ((BYTE)(255 - (((255 - A) * (255 - B)) >> 8)))
#define ChannelBlend_Exclusion(A,B)  ((BYTE)(A + B - 2 * A * B / 255))
#define ChannelBlend_Overlay(A,B)    ((BYTE)((B < 128) ? (2 * A * B / 255):(255 - 2 * (255 - A) * (255 - B) / 255)))
#define ChannelBlend_SoftLight(A,B)  ((BYTE)((B < 128)?(2*((A>>1)+64))*((float)B/255):(255-(2*(255-((A>>1)+64))*(float)(255-B)/255))))
#define ChannelBlend_HardLight(A,B)  (ChannelBlend_Overlay(B,A))
#define ChannelBlend_ColorDodge(A,B) ((BYTE)((B == 255) ? B:min(255, ((A << 8 ) / (255 - B)))))
#define ChannelBlend_ColorBurn(A,B)  ((BYTE)((B == 0) ? B:max(0, (255 - ((255 - A) << 8 ) / B))))
#define ChannelBlend_LinearDodge(A,B)(ChannelBlend_Add(A,B))
#define ChannelBlend_LinearBurn(A,B) (ChannelBlend_Subtract(A,B))
#define ChannelBlend_LinearLight(A,B)((BYTE)(B < 128)?ChannelBlend_LinearBurn(A,(2 * B)):ChannelBlend_LinearDodge(A,(2 * (B - 128))))
#define ChannelBlend_VividLight(A,B) ((BYTE)(B < 128)?ChannelBlend_ColorBurn(A,(2 * B)):ChannelBlend_ColorDodge(A,(2 * (B - 128))))
#define ChannelBlend_PinLight(A,B)   ((BYTE)(B < 128)?ChannelBlend_Darken(A,(2 * B)):ChannelBlend_Lighten(A,(2 * (B - 128))))
#define ChannelBlend_HardMix(A,B)    ((BYTE)((ChannelBlend_VividLight(A,B) < 128) ? 0:255))
#define ChannelBlend_Reflect(A,B)    ((BYTE)((B == 255) ? B:min(255, (A * A / (255 - B)))))
#define ChannelBlend_Glow(A,B)       (ChannelBlend_Reflect(B,A))
#define ChannelBlend_Phoenix(A,B)    ((BYTE)(min(A,B) - max(A,B) + 255))
#define ChannelBlend_Alpha(A,B,O)    ((BYTE)(O * A + (1.0f - O) * B))
#define ChannelBlend_AlphaF(A,B,F,O) (ChannelBlend_Alpha(F(A,B),A,O))

#define MultiChannelAlphaBlend(rgbqSrc,rgbqDst,blendFunc){\
	float fAlphaSrc = (float)rgbqSrc.rgbReserved / 255.0f;\
	rgbqDst.rgbRed = ChannelBlend_Alpha (blendFunc (rgbqSrc.rgbRed, rgbqDst.rgbRed), rgbqDst.rgbRed, fAlphaSrc);\
	rgbqDst.rgbGreen = ChannelBlend_Alpha (blendFunc (rgbqSrc.rgbGreen, rgbqDst.rgbGreen), rgbqDst.rgbGreen, fAlphaSrc);\
	rgbqDst.rgbBlue = ChannelBlend_Alpha (blendFunc (rgbqSrc.rgbBlue, rgbqDst.rgbBlue), rgbqDst.rgbBlue, fAlphaSrc);\
	return rgbqDst;}

#define MultiChannelBlend(rgbqSrc,rgbqDst,blendFunc){\
	rgbqDst.rgbRed = blendFunc (rgbqSrc.rgbRed, rgbqDst.rgbRed);\
	rgbqDst.rgbGreen = blendFunc (rgbqSrc.rgbGreen, rgbqDst.rgbGreen);\
	rgbqDst.rgbBlue = blendFunc (rgbqSrc.rgbBlue, rgbqDst.rgbBlue);\
	return rgbqDst;}

RGBQUAD CXIMAGEMIXING_ALPHA (RGBQUAD rgbqSrc, RGBQUAD rgbqDst)		MultiChannelAlphaBlend (rgbqSrc, rgbqDst, ChannelBlend_Normal)
RGBQUAD CXIMAGEMIXING_DARKENING (RGBQUAD rgbqSrc, RGBQUAD rgbqDst)	MultiChannelAlphaBlend (rgbqSrc, rgbqDst, ChannelBlend_Darken)
RGBQUAD CXIMAGEMIXING_MULTIPLY (RGBQUAD rgbqSrc, RGBQUAD rgbqDst)	MultiChannelAlphaBlend (rgbqSrc, rgbqDst, ChannelBlend_Multiply)
RGBQUAD CXIMAGEMIXING_SCREEN (RGBQUAD rgbqSrc, RGBQUAD rgbqDst)		MultiChannelAlphaBlend (rgbqSrc, rgbqDst, ChannelBlend_Screen)
RGBQUAD CXIMAGEMIXING_OVERLAY (RGBQUAD rgbqSrc, RGBQUAD rgbqDst)	MultiChannelBlend (rgbqSrc, rgbqDst, ChannelBlend_Overlay)
