/*
 * File:	ximage.h
 * Purpose:	General Purpose Image Class 
 */
/*
  --------------------------------------------------------------------------------

	COPYRIGHT NOTICE, DISCLAIMER, and LICENSE:

	CxImage version 6.0.0 02/Feb/2008

	CxImage : Copyright (C) 2001 - 2008, Davide Pizzolato

	Original CImage and CImageIterator implementation are:
	Copyright (C) 1995, Alejandro Aguilar Sierra (asierra(at)servidor(dot)unam(dot)mx)

	Covered code is provided under this license on an "as is" basis, without warranty
	of any kind, either expressed or implied, including, without limitation, warranties
	that the covered code is free of defects, merchantable, fit for a particular purpose
	or non-infringing. The entire risk as to the quality and performance of the covered
	code is with you. Should any covered code prove defective in any respect, you (not
	the initial developer or any other contributor) assume the cost of any necessary
	servicing, repair or correction. This disclaimer of warranty constitutes an essential
	part of this license. No use of any covered code is authorized hereunder except under
	this disclaimer.

	Permission is hereby granted to use, copy, modify, and distribute this
	source code, or portions hereof, for any purpose, including commercial applications,
	freely and without fee, subject to the following restrictions: 

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.

  --------------------------------------------------------------------------------

	Other information about CxImage, and the latest version, can be found at the
	CxImage home page: http://www.xdp.it/cximage/

  --------------------------------------------------------------------------------
 */
#if !defined(__CXIMAGE_H)
#define __CXIMAGE_H

#if _MSC_VER > 1000
#pragma once
#endif 

#include <stdlib.h>
#include "CxAlloc.h"

/////////////////////////////////////////////////////////////////////////////
#include "xfile.h"
#include "xiofile.h"
#include "xmemfile.h"
#include "ximathread.h"
#include "ximalyrcache.h"
#include "xbrush.h"

#include <list>

#include "IGUserMessages.h"
#include <IGSmartPixel.h>
#include <IGLPE.h>

enum ENUM_CXIMAGE_TRANSMODE{
	CXIMAGE_TRANSMODE_NORMAL = 0,
	CXIMAGE_TRANSMODE_SMOOTH = 1};

/////////////////////////////////////////////////////////////////////////////
// CxImage class
/////////////////////////////////////////////////////////////////////////////
class DLL_EXP CxImage : public CxImageThread
{
public:
	//public structures
struct rgb_color { BYTE r,g,b; };

#if CXIMAGE_SUPPORT_WINDOWS
// <VATI> text placement data
// members must be initialized with the InitTextInfo(&this) function.
typedef struct tagCxTextInfo
{
#if defined (_WIN32_WCE)
	TCHAR    text[256];  ///< text for windows CE
#else
	TCHAR    text[4096]; ///< text (char -> TCHAR for UNICODE [Cesar M])
#endif
	LOGFONT  lfont;      ///< font and codepage data
    COLORREF fcolor;     ///< foreground color
    long     align;      ///< DT_CENTER, DT_RIGHT, DT_LEFT aligment for multiline text
    BYTE     smooth;     ///< text smoothing option. Default is false.
    BYTE     opaque;     ///< text has background or hasn't. Default is true.
						 ///< data for background (ignored if .opaque==FALSE) 
    COLORREF bcolor;     ///< background color
    float    b_opacity;  ///< opacity value for background between 0.0-1.0 Default is 0. (opaque)
    BYTE     b_outline;  ///< outline width for background (zero: no outline)
    BYTE     b_round;    ///< rounding radius for background rectangle. % of the height, between 0-50. Default is 10.
                         ///< (backgr. always has a frame: width = 3 pixel + 10% of height by default.)
} CXTEXTINFO;
#endif

public:
/** \addtogroup Constructors */ //@{
	CxImage(DWORD imagetype = 0);
	CxImage(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype = 0, BYTE crInit = CXIMAGE_GRAY);
	CxImage(const CxImage &src, bool copypixels = true, bool copyselection = true, bool copyalpha = true);
	CxImage(const TCHAR * filename, DWORD imagetype);	// For UNICODE support: char -> TCHAR
	CxImage(FILE * stream, DWORD imagetype);
	CxImage(CxFile * stream, DWORD imagetype);
	CxImage(BYTE * buffer, DWORD size, DWORD imagetype);
	virtual ~CxImage() { DestroyFrames(); Destroy(); };
	CxImage& operator = (const CxImage&);
	CxImage& operator += (CxImage&);
//@}

/** \addtogroup Initialization */ //@{
	void*	Create(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype = 0, BYTE crInit = CXIMAGE_GRAY);
	bool	Destroy();
	bool	DestroyFrames();
	void	Clear(BYTE bval=0);
	void	Copy(const CxImage &src, bool copypixels = true, bool copyselection = true, bool copyalpha = true, bool copylayers = true, int nOffsetX = -1, int nOffsetY = -1);
	bool	Transfer(CxImage &from, bool bTransferFrames = true);
	bool	CreateFromArray(BYTE* pArray,DWORD dwWidth,DWORD dwHeight,DWORD dwBitsperpixel, DWORD dwBytesperline, bool bFlipImage);
	bool	CreateFromMatrix(BYTE** ppMatrix,DWORD dwWidth,DWORD dwHeight,DWORD dwBitsperpixel, DWORD dwBytesperline, bool bFlipImage);
	void	FreeMemory(void* memblock);
	virtual LPCWSTR GetId() const;

	DWORD Dump(BYTE * dst);
	DWORD UnDump(const BYTE * src);
	DWORD DumpSize();

//@}

/** \addtogroup Attributes */ //@{
	long	GetSize();
	virtual BYTE*	GetBits(DWORD row = 0);
	static long GetBitmapBits (HDC hMemDC, HBITMAP hMemBmp, int nSrcX, int nSrcY, LPBITMAPINFO &pBitmapInfo);
	BYTE	GetColorType();
	void*	GetDIB() const;
	DWORD	GetHeight() const;
	DWORD	GetWidth() const;
	void	SetHeight(DWORD dwHeight);
	void	SetWidth(DWORD dwWidth);
	DWORD	GetEffWidth() const;
	DWORD	GetNumColors() const;
	WORD	GetBpp() const;
	DWORD	GetType() const;
	const char*	GetLastError();
	static const TCHAR* GetVersion();
	static const float GetVersionNumber();

	DWORD	GetFrameDelay() const;
	void	SetFrameDelay(DWORD d);

	void	GetOffset(long *x,long *y);
	void	SetOffset(long x,long y);

	BYTE	GetJpegQuality() const;
	void	SetJpegQuality(BYTE q);
	float	GetJpegQualityF() const;
	void	SetJpegQualityF(float q);

	BYTE	GetJpegScale() const;
	void	SetJpegScale(BYTE q);

	long	GetXDPI() const;
	long	GetYDPI() const;
	void	SetXDPI(long dpi);
	void	SetYDPI(long dpi);

	DWORD	GetClrImportant() const;
	void	SetClrImportant(DWORD ncolors = 0);

	long	GetProgress() const;
	long	GetEscape() const;
	void	SetProgress(long p);
	void	SetEscape(long i);

	long	GetTransIndex() const;
	RGBQUAD	GetTransColor();
	void	SetTransIndex(long idx);
	void	SetTransColor(RGBQUAD rgb);
	void	SetTransMode(ENUM_CXIMAGE_TRANSMODE transMode);
	bool	IsTransparent() const;

	DWORD	GetCodecOption(DWORD imagetype = 0);
	bool	SetCodecOption(DWORD opt, DWORD imagetype = 0);

	DWORD	GetFlags() const;
	void	SetFlags(DWORD flags, bool bLockReservedFlags = true);

	BYTE	GetDisposalMethod() const;
	void	SetDisposalMethod(BYTE dm);

	bool	SetType(DWORD type);

	static DWORD GetNumTypes();
	static DWORD GetTypeIdFromName(const TCHAR* ext);
	static DWORD GetTypeIdFromIndex(const DWORD index);
	static DWORD GetTypeIndexFromId(const DWORD id);

	bool	GetRetreiveAllFrames() const;
	void	SetRetreiveAllFrames(bool flag);
	CxImage * GetFrame(long nFrame) const;
//@}

/** \addtogroup Palette
 * These functions have no effects on RGB images and in this case the returned value is always 0.
 * @{ */
	bool	IsGrayScale();
	bool	IsIndexed() const;
	bool	IsSamePalette(CxImage &img, bool bCheckAlpha = true);
	DWORD	GetPaletteSize();
	RGBQUAD* GetPalette() const;
	RGBQUAD GetPaletteColor(BYTE idx);
	bool	GetPaletteColor(BYTE i, BYTE* r, BYTE* g, BYTE* b);
	BYTE	GetNearestIndex(RGBQUAD c);
	void	BlendPalette(COLORREF cr,long perc);
	void	SetGrayPalette();
	void	SetPalette(DWORD n, BYTE *r, BYTE *g, BYTE *b);
	void	SetPalette(RGBQUAD* pPal,DWORD nColors=256);
	void	SetPalette(rgb_color *rgb,DWORD nColors=256);
	void	SetPaletteColor(BYTE idx, BYTE r, BYTE g, BYTE b, BYTE alpha=0);
	void	SetPaletteColor(BYTE idx, RGBQUAD c);
	void	SetPaletteColor(BYTE idx, COLORREF cr);
	void	SwapIndex(BYTE idx1, BYTE idx2);
	void	SwapRGB2BGR();
	void	SetStdPalette();
//@}

/** \addtogroup Pixel */ //@{
	inline bool	IsInside(long x, long y);
	inline bool IsInsideSel(long x, long y);
	bool	IsTransparent(long x,long y);
	bool	GetTransparentMask(CxImage* iDst = 0);
	RGBQUAD GetPixelColor(long x,long y, bool bGetAlpha = true);
	inline BYTE* GetPixelColor24(const POINT&);
	inline void GetPixelNeighbours(POINT pt, POINT *pNeighbours, int& nNbNeighbours);
	inline void GetPixelNeighbours(POINT pt, POINT *pNeighbours, bool *pbIsDone, int& nNbNeighbours);
	BYTE	GetPixelIndex(long x,long y);
	BYTE	GetPixelGray(long x, long y);
	void	SetPixelColor(long x,long y,RGBQUAD c, bool bSetAlpha = false);
	void	SetPixelColor(long x,long y,COLORREF cr);
	void	SetPixelIndex(long x,long y,BYTE i);
	void	DrawLine(int StartX, int EndX, int StartY, int EndY, const CxBrush& brush);
	void	BlendPixelColor(long x,long y,RGBQUAD c, float blend, bool bSetAlpha = false);
	inline RGBQUAD BlindGetPixelColor(const long x,const long y, bool bGetAlpha = true);
	inline void BlindSetPixelColor(long x,long y,RGBQUAD c, bool bSetAlpha = false);
//@}

protected:
/** \addtogroup Protected */ //@{
	BYTE BlindGetPixelIndex(const long x,const long y);
	void *BlindGetPixelPointer(const long x,const  long y);
	void BlindSetPixelIndex(long x,long y,BYTE i);
//@}

public:

#if CXIMAGE_SUPPORT_INTERPOLATION
/** \addtogroup Interpolation */ //@{
	//overflow methods:
	enum OverflowMethod {
		OM_COLOR=1,
		OM_BACKGROUND=2,
		OM_TRANSPARENT=3,
		OM_WRAP=4,
		OM_REPEAT=5,
		OM_MIRROR=6
	};
	void OverflowCoordinates(float &x, float &y, OverflowMethod const ofMethod);
	void OverflowCoordinates(long  &x, long &y, OverflowMethod const ofMethod);
	RGBQUAD GetPixelColorWithOverflow(long x, long y, OverflowMethod const ofMethod=OM_BACKGROUND, RGBQUAD* const rplColor=0);
	//interpolation methods:
	enum InterpolationMethod {
		IM_NEAREST_NEIGHBOUR=1,
		IM_BILINEAR		=2,
		IM_BSPLINE		=3,
		IM_BICUBIC		=4,
		IM_BICUBIC2		=5,
		IM_LANCZOS		=6,
		IM_BOX			=7,
		IM_HERMITE		=8,
		IM_HAMMING		=9,
		IM_SINC			=10,
		IM_BLACKMAN		=11,
		IM_BESSEL		=12,
		IM_GAUSSIAN		=13,
		IM_QUADRATIC	=14,
		IM_MITCHELL		=15,
		IM_CATROM		=16,
		IM_HANNING		=17,
		IM_POWER		=18
	};
	RGBQUAD GetPixelColorInterpolated(float x,float y, InterpolationMethod const inMethod=IM_BILINEAR, OverflowMethod const ofMethod=OM_BACKGROUND, RGBQUAD* const rplColor=0);
	RGBQUAD GetAreaColorInterpolated(float const xc, float const yc, float const w, float const h, InterpolationMethod const inMethod, OverflowMethod const ofMethod=OM_BACKGROUND, RGBQUAD* const rplColor=0);
//@}

protected:
/** \addtogroup Protected */ //@{
	void  AddAveragingCont(RGBQUAD const &color, float const surf, float &rr, float &gg, float &bb, float &aa);
//@}

/** \addtogroup Kernels */ //@{
public:
	static float KernelBSpline(const float x);
	static float KernelLinear(const float t);
	static float KernelCubic(const float t);
	static float KernelGeneralizedCubic(const float t, const float a=-1);
	static float KernelLanczosSinc(const float t, const float r = 3);
	static float KernelBox(const float x);
	static float KernelHermite(const float x);
	static float KernelHamming(const float x);
	static float KernelSinc(const float x);
	static float KernelBlackman(const float x);
	static float KernelBessel_J1(const float x);
	static float KernelBessel_P1(const float x);
	static float KernelBessel_Q1(const float x);
	static float KernelBessel_Order1(float x);
	static float KernelBessel(const float x);
	static float KernelGaussian(const float x);
	static float KernelQuadratic(const float x);
	static float KernelMitchell(const float x);
	static float KernelCatrom(const float x);
	static float KernelHanning(const float x);
	static float KernelPower(const float x, const float a = 2);
//@}
#endif //CXIMAGE_SUPPORT_INTERPOLATION
	
/** \addtogroup Painting */ //@{
#if CXIMAGE_SUPPORT_WINDOWS
	long	Blt (HDC pDC, long x=0, long y=0);
	HBITMAP MakeBitmap (HDC hdc = NULL, int nImageWidth = -1, int nImageHeight = -1, bool bDrawChessBackground = false, bool bDrawWhiteBackground = true, bool bMixAlpha = true);
	HANDLE	CopyToHandle();
	bool	CreateFromHANDLE(HANDLE hMem);		//Windows objects (clipboard)
	bool	CreateFromHBITMAP(HBITMAP hbmp, HPALETTE hpal=0, bool bUpdateParams = true);	//Windows resource
	bool	CreateFromHICON(HICON hico);
	virtual long	Draw(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bDrawChessBackGround = false, bool bMixAlpha = true);
	long	DrawWithSelection (HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bDrawChessBackGround = false, bool bMixAlpha = true);
	long	DrawWithSegmentation (HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bDrawChessBackGround = false, bool bMixAlpha = true);
	long	Draw(HDC hdc, const RECT& rect, RECT* pClipRect=NULL, bool bSmooth = false, bool bMixAlpha = true);
	long	Stretch(HDC hdc, long xoffset, long yoffset, long xsize, long ysize, DWORD dwRop = SRCCOPY);
	long	Stretch(HDC hdc, const RECT& rect, DWORD dwRop = SRCCOPY);
	long	Tile(HDC hdc, RECT *rc);
	long	Draw2(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1);
	long	Draw2(HDC hdc, const RECT& rect);
	long	DrawString(HDC hdc, long x, long y, const TCHAR* text, RGBQUAD color, const TCHAR* font, long lSize=0, long lWeight=400, BYTE bItalic=0, BYTE bUnderline=0, bool bSetAlpha=false);
	// <VATI> extensions
	long    DrawStringEx(HDC hdc, long x, long y, CXTEXTINFO *pTextType, bool bSetAlpha=false );
	void    InitTextInfo( CXTEXTINFO *txt );
	virtual IGLibrary::IGSmartPixel*	GetSegmentationLayer (IGLibrary::IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM& eShowSegmType);

	// Progress bar
	virtual void ProgressStepIt();
	virtual void ProgressSetRange (UINT nMax, bool bLock);
	virtual void ProgressSetSubRange (int nCurStartRange, int nNbSubRanges);
	virtual void ProgressSetMessage (wchar_t *pwMessage);
#endif //CXIMAGE_SUPPORT_WINDOWS
//@}

	// file operations
#if CXIMAGE_SUPPORT_DECODE
/** \addtogroup Decode */ //@{
#ifdef WIN32
	bool LoadResource(HRSRC hRes, DWORD imagetype, HMODULE hModule=NULL);
#endif
	// For UNICODE support: char -> TCHAR
	virtual bool Load(const TCHAR* filename, DWORD imagetype);
	virtual void SetPath(const TCHAR* filename) {}
	virtual const TCHAR* GetPath() {return NULL;}
	bool Decode(FILE * hFile, DWORD imagetype);
	bool Decode(CxFile * hFile, DWORD imagetype);
	bool Decode(BYTE * buffer, DWORD size, DWORD imagetype);

	bool CheckFormat(CxFile * hFile, DWORD imagetype = 0);
	bool CheckFormat(BYTE * buffer, DWORD size, DWORD imagetype = 0);
//@}
#endif //CXIMAGE_SUPPORT_DECODE

#if CXIMAGE_SUPPORT_ENCODE
protected:
/** \addtogroup Protected */ //@{
	bool EncodeSafeCheck(CxFile *hFile);
//@}

public:
/** \addtogroup Encode */ //@{
	// For UNICODE support: char -> TCHAR
	virtual bool Save(const TCHAR* filename, DWORD imagetype);
	void SetLastOutputPath(const TCHAR *filename) {::wcscpy_s (info.twLastOutputPath, filename);}
	std::wstring GetLastOutputPath() const { return std::wstring (info.twLastOutputPath);}
	void SetLastReqGuid(const TCHAR *reqGuid) {::wcscpy_s (info.twLastReqGuid, reqGuid);}
	std::wstring GetLastReqGuid() const { return std::wstring (info.twLastReqGuid);}
	bool Encode(FILE * hFile, DWORD imagetype);
	bool Encode(CxFile * hFile, DWORD imagetype);
	bool Encode(BYTE *&buffer, long &size, DWORD imagetype);
	bool Encode(CxFile * hFile, CxImage ** pImages, int pagecount, DWORD imagetype);
	bool Encode(FILE *hFile, CxImage ** pImages, int pagecount, DWORD imagetype);
	
	bool Encode2RGBA(CxFile *hFile, bool bFlipY = false);
	bool Encode2RGBA(BYTE * &buffer, long &size, bool bFlipY = false);
//@}
#endif //CXIMAGE_SUPPORT_ENCODE

/** \addtogroup Attributes */ //@{
	//misc.
	bool IsValid() const;
	bool IsEnabled() const;
	void Enable(bool enable=true);

	// frame operations
	long GetNumFrames() const;
	long GetFrame() const;
	void SetFrame(long nFrame);

	// zoom attribute
	float GetCurrentZoom() const;
	void SetCurrentZoom (float fCurrentZoom);

	// DeepZoom config - To be called before Thread::Start
	void SetFullUpdate();
	void SetUpdateRect(const RECT& rcUpdate);
	void SetLastUpdateRect(const RECT& rcUpdate);
//@}

#if CXIMAGE_SUPPORT_BASICTRANSFORMATIONS
/** \addtogroup BasicTransformations */ //@{
	bool GrayScale(bool bTo8bits = false);
	bool GrayScaleRed();
	bool Flip(bool bFlipSelection = false, bool bFlipAlpha = true);
	bool Mirror(bool bMirrorSelection = false, bool bMirrorAlpha = true);
	bool Negative();
	bool RotateLeft(CxImage* iDst = NULL);
	bool RotateRight(CxImage* iDst = NULL);
//@}
#endif //CXIMAGE_SUPPORT_BASICTRANSFORMATIONS

#if CXIMAGE_SUPPORT_TRANSFORMATION
/** \addtogroup Transformations */ //@{
	// image operations
	bool Rotate (int angle);
	bool Rotate (float angle, CxImage *iDst = NULL, InterpolationMethod inMethod=IM_BILINEAR,
                OverflowMethod ofMethod=OM_BACKGROUND, RGBQUAD *replColor=0,
                bool const optimizeRightAngles=true, bool const bKeepOriginalSize=false);
	bool Rotate180(CxImage* iDst = NULL);
	virtual bool AutoRotate() { return false; }
	bool Resample(const CxImage& iDst);
	bool Resample(long newx, long newy, InterpolationMethod const inMethod=IM_BICUBIC2,
				OverflowMethod const ofMethod=OM_REPEAT, CxImage* const iDst = NULL,
				bool const disableAveraging=false);
	bool DecreaseBpp(DWORD nbit, bool errordiffusion, RGBQUAD* ppal = 0, DWORD clrimportant = 0);
	bool IncreaseBpp(DWORD nbit);
	bool Dither(long method = 0);
	bool Crop(long left, long top, long right, long bottom, CxImage* iDst = NULL);
	bool Crop(const RECT& rect, CxImage* iDst = NULL);
	bool CropRotatedRectangle( long topx, long topy, long width, long height, float angle, CxImage* iDst = NULL);
	bool Skew(float xgain, float ygain, long xpivot=0, long ypivot=0, bool bEnableInterpolation = false);
	bool RotateAndResize(float xTransform, float yTransform, float fRate, bool bFullSelect = false, CxImage *pSrc = NULL);
	bool Expand(long left, long top, long right, long bottom, RGBQUAD canvascolor, CxImage* iDst = 0);
	bool Expand(long newx, long newy, RGBQUAD canvascolor, CxImage* iDst = 0);
	bool Thumbnail(long newx, long newy, RGBQUAD canvascolor, CxImage* iDst = 0);
	bool CircleTransform(int type,long rmax=0,float Koeff=1.0f);
	bool RedEyeRemove(RECT rcLeft, RECT rcRight, float strength = 0.8f);
	bool ChangeEyeColor (RECT rcLeft, RECT rcRight, BYTE hue, BYTE sat, float fStrength);
	bool GetEyeCenters (RECT rcLeft, RECT rcRight, POINT& ptCenterLeft, POINT& ptCenterRight);
	POINT GetEyeCenter();
	bool QIShrink(long newx, long newy, CxImage* const iDst = NULL, bool bChangeBpp = false);
	bool Smile(float fSmileSizeL, float fSmileSizeR, const RECT& rcReg, const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth);
	bool Morphing(int nPosX, int nPosY, float fMorphingDirectionX, float fMorphingDirectionY, float fRadius, int nType, bool bDisableSel = false);
	bool ColorizeTurb(BYTE red, BYTE green, BYTE blue, float blend);
	bool FaceColorize(BYTE red, BYTE green, BYTE blue, float blend, RECT rcEyeLeft, RECT rcEyeRight, RECT rcMouth, bool bNoTurbulence = false, bool bFun = false);
	bool SetEyeHueSat (BYTE hue, BYTE sat, RECT rcEye, BYTE valThreshold, bool bMix, bool bUseSelect = false);
	bool AddEyeRGB (RGBQUAD rgbColRGB, RECT rcEye, bool bUseSelect = false);
	bool AddEyeSaturation (short sat, RECT rcEye, bool bUseSelect = false);
	bool AddEyeValue (short val, RECT rcEye, bool bUseSelect = false);
	bool ScaleEyeValue (float fScale, RECT rcEye, bool bUseSelect = false);
	// advanced transformations
	bool Cartoon(float fDiffusion = 5.0f);
	bool Sketch(int nDiffusion = 4);
	bool OilPainting(float fDiffusion = 5.0f);
	bool WaterPainting(float fDiffusion = 5.0f);
	bool Quantize(int colorCount = 256);
	bool Clay();
	bool ExecutePythonScript (const std::wstring& wsPythonScript);
	virtual bool Update() {return true;}
	virtual void SetSize(int nWidth, int nHeight);
//@}
#endif //CXIMAGE_SUPPORT_TRANSFORMATION

#if CXIMAGE_SUPPORT_DSP
/** \addtogroup DSP */ //@{
	bool Contour();
	bool HistogramStretch(long method = 0, double threshold = 0);
	bool HistogramEqualize();
	bool HistogramNormalize();
	bool HistogramRoot();
	bool HistogramLog();
	long Histogram(long* red, long* green = 0, long* blue = 0, long* gray = 0, long colorspace = 0);
	bool Jitter(long radius=2);
	bool Repair(float radius = 0.25f, long niterations = 1, long colorspace = 0);
	bool Combine(CxImage* r,CxImage* g,CxImage* b,CxImage* a, long colorspace = 0);
	bool FFT2(CxImage* srcReal, CxImage* srcImag, CxImage* dstReal, CxImage* dstImag, long direction = 1, bool bForceFFT = true, bool bMagnitude = true);
	bool Noise(long level);
	bool Median(long Ksize=3);
	bool Gamma(float gamma);
	bool GammaRGB(float gammaR, float gammaG, float gammaB);
	bool ShiftRGB(long r, long g, long b);
	bool Threshold(BYTE level);
	bool ThresholdAnd(CxImage* pThresholdMask, BYTE filtLevel);
	bool Threshold(CxImage* pThresholdMask);
	bool Threshold2(BYTE level, bool bDirection, RGBQUAD nBkgndColor, bool bSetAlpha = false);
	bool Colorize(BYTE hue, BYTE sat, BYTE val, float blend, bool bMixVal = false);
	bool Light(long brightness, long contrast = 0);
	bool Vibrance(long strength);
	float Mean();
	bool Filter(long* kernel, long Ksize, long Kfactor, long Koffset);
	bool GradientMorpho(long nDiam, bool bHSV = false);
	bool Erode(long Ksize=2);
	bool Dilate(long Ksize=2);
	bool Edge(long Ksize=2);
	void HuePalette(float correction=1);
	enum ImageOpType {OpAdd, OpAnd, OpXor, OpOr, OpMask, OpSrcCopy, OpDstCopy, OpSub, OpSrcBlend, OpScreen, OpAvg, OpAlphaBlend};
	void Mix(CxImage & imgsrc2);
	void MixFrom(CxImage & imagesrc2, long lXOffset, long lYOffset);
	bool UnsharpMask(float radius = 5.0f, float amount = 0.5f, int threshold = 0);
	bool Lut(BYTE* pLut);
	bool Lut(BYTE* pLutR, BYTE* pLutG, BYTE* pLutB, BYTE* pLutA = 0);
	bool GaussianBlur(float radius = 1.0f, CxImage* iDst = 0);
	bool TextBlur(BYTE threshold = 100, BYTE decay = 2, BYTE max_depth = 5, bool bBlurHorizontal = true, bool bBlurVertical = true, CxImage* iDst = 0);
	bool SelectiveBlur(float radius = 1.0f, BYTE threshold = 25, CxImage* iDst = 0);
	bool Solarize(BYTE level = 128, bool bLinkedChannels = true);
	bool FloodFill(const long xStart, const long yStart, const RGBQUAD cFillColor, const BYTE tolerance = 0,
					BYTE nOpacity = 255, const bool bSelectFilledArea = false, const BYTE nSelectionLevel = 255);
	bool FillGradient(const long xStart, const long yStart, const long nDirectionX, const long nDirectionY, const RGBQUAD cFillColor, const RGBQUAD cFillColor2);
	bool Saturate(const long saturation, const long colorspace = 1);
	bool ConvertColorSpace(const long dstColorSpace, const long srcColorSpace);
	int  OptimalThreshold(long method = 0, RECT * pBox = 0, CxImage* pContrastMask = 0);
	bool AdaptiveThreshold(long method = 0, long nBoxSize = 64, CxImage* pContrastMask = 0, long nBias = 0, float fGlobalLocalBalance = 0.5f);
	bool DeletePixels(bool bSmart);
	bool MovePixels(int nVectorX, int nVectorY);
	bool kmeanClustering();
	bool Multiply(const CxImage & imageSrc); 
	bool Duotone(COLORREF col1, COLORREF col2);
	bool Overlay(const CxImage & imageSrc);
	inline void SetBackgroundColor (const RGBQUAD& rgbq) { info.nBkgndColor.rgbRed = rgbq.rgbRed; info.nBkgndColor.rgbGreen = rgbq.rgbGreen; info.nBkgndColor.rgbBlue = rgbq.rgbBlue;}

	void Move(int nVectorX, int nVectorY) { info.xOffset += nVectorX; info.yOffset += nVectorY; }
//@}

protected:
/** \addtogroup Protected */ //@{
	bool IsPowerof2(long x);
	bool FFT(int dir,int m,double *x,double *y);
	bool DFT(int dir,long m,double *x1,double *y1,double *x2,double *y2);
	bool RepairChannel(CxImage *ch, float radius);
	// <nipper>
	int gen_convolve_matrix (float radius, float **cmatrix_p);
	float* gen_lookup_table (float *cmatrix, int cmatrix_length);
	void blur_line (float *ctable, float *cmatrix, int cmatrix_length, BYTE* cur_col, BYTE* dest_col, int y, long bytes);
	void blur_text (BYTE threshold, BYTE decay, BYTE max_depth, CxImage* iSrc, CxImage* iDst, BYTE bytes);

	HANDLE  convertDDBToDIB (HBITMAP bitmap, DWORD dwCompression, bool bCopyAlpha);
//@}

public:
/** \addtogroup ColorSpace */ //@{
	bool SplitRGB(CxImage* r,CxImage* g,CxImage* b);
	bool SplitYUV(CxImage* y,CxImage* u,CxImage* v);
	bool SplitHSL(CxImage* h,CxImage* s,CxImage* l);
	bool SplitYIQ(CxImage* y,CxImage* i,CxImage* q);
	bool SplitXYZ(CxImage* x,CxImage* y,CxImage* z);
	bool SplitCMYK(CxImage* c,CxImage* m,CxImage* y,CxImage* k);
	static RGBQUAD HSLtoRGB(COLORREF cHSLColor);
	static RGBQUAD RGBtoHSL(RGBQUAD lRGBColor);
	static RGBQUAD HSLtoRGB(RGBQUAD lHSLColor);
	static RGBQUAD YUVtoRGB(RGBQUAD lYUVColor);
	static RGBQUAD RGBtoYUV(RGBQUAD lRGBColor);
	static RGBQUAD YIQtoRGB(RGBQUAD lYIQColor);
	static RGBQUAD RGBtoYIQ(RGBQUAD lRGBColor);
	static RGBQUAD XYZtoRGB(RGBQUAD lXYZColor);
	static RGBQUAD RGBtoXYZ(RGBQUAD lRGBColor);
#endif //CXIMAGE_SUPPORT_DSP
	static RGBQUAD RGBtoRGBQUAD(COLORREF cr);
	static COLORREF RGBQUADtoRGB (RGBQUAD c);
//@}

#if CXIMAGE_SUPPORT_SELECTION
/** \addtogroup Selection */ //@{
	bool SelectionClear(BYTE level = 0);
	bool SelectionCreate();
	bool SelectionDelete();
	bool SelectionInvert();
	bool SelectionMirror();
	bool SelectionFlip();
	bool SelectionAddRect(RECT r, BYTE level = 255);
	bool SelectionAndRect(RECT r, BYTE level = 255);
	bool SelectionAnd (CxImage& src, BYTE level = 255);
	bool SelectionAddEllipse(RECT r, BYTE level = 255);
	bool SelectionAddPolygon(const std::list<POINT>& lPts, BYTE level = 255, bool bFill = true);
	bool SelectionAddMagic(POINT pt, BYTE level, int nTolerance);
	bool SelectionAddColor(RGBQUAD c, BYTE level = 255);
	bool SelectionAddPixel(long x, long y, BYTE level = 255);
	bool SelectionAddFaces(BYTE level = 255);
	bool SelectionAddEyes(BYTE level = 255);
	bool SelectionAddMouth(BYTE level = 255);
	bool SelectionAddNoze(BYTE level = 255);
	bool SelectionAddLPE(const std::list<POINT>& lPts, BYTE level = 255);
	bool SelectionCopy(CxImage &from);
	bool SelectionCopyAdd(CxImage &from);
	virtual bool SelectionCopyFromScaledLayer (CxImage& layerScaled);
	bool SelectionIsInside(long x, long y);
	bool SelectionIsValid();
	void SelectionGetBox(RECT& r);
	bool SelectionToHRGN(HRGN& region);
	bool SelectionSplit(CxImage *dest);
	BYTE SelectionGet(const long x,const long y);
	bool SelectionSet(CxImage &from);
	void SelectionRebuildBox();
	bool SelectionErode(int nSize);
	bool SelectionDilate(int nSize);
	BYTE* SelectionGetPointer(const long x = 0,const long y = 0);
	virtual bool GetSelected() const {return false;}
	virtual RECT GetUpdateRect() const;
	const RECT& GetLastUpdateRect() const;
	void getBoundaries (std::vector <BYTE*>& vBoundaries);
	inline bool BlindSelectionIsInside(long x, long y);
//@}

protected:
/** \addtogroup Protected */ //@{
	BYTE BlindSelectionGet(const long x,const long y);
	void SelectionSet(const long x,const long y,const BYTE level);
	void UpdateImageParameters();
//@}

public:

#endif //CXIMAGE_SUPPORT_SELECTION

#if CXIMAGE_SUPPORT_ALPHA
/** \addtogroup Alpha */ //@{
	void AlphaClear();
	bool AlphaCreate(BYTE level = 255);
	void AlphaAdd (BYTE level);
	void AlphaMultiply (float fFactor);
	void AlphaDelete();
	void AlphaInvert();
	bool AlphaMirror();
	bool AlphaFlip();
	bool AlphaCopy(CxImage &from);
	bool AlphaSplit(CxImage *dest);
	void AlphaStrip();
	inline void AlphaSet(BYTE level);
	bool AlphaSet(CxImage &from);
	void AlphaSet(const long x,const long y,const BYTE level);
	void AlphaSetMixingFunc(MIXING_FUNC func);
	BYTE AlphaGet(const long x,const long y);
	BYTE AlphaGetMax() const;
	void AlphaSetMax(BYTE nAlphaMax);
	bool AlphaIsValid();
	BYTE* AlphaGetPointer(const long x = 0,const long y = 0);
	bool AlphaFromTransparency();

	void AlphaPaletteClear();
	void AlphaPaletteEnable(bool enable=true);
	bool AlphaPaletteIsEnabled();
	bool AlphaPaletteIsValid();
	bool AlphaPaletteSplit(CxImage *dest);
	void AlphaExport (std::ostream &os);
	void AlphaSetChannel (void *p_alpha);
//@}

protected:
/** \addtogroup Protected */ //@{
	inline BYTE BlindAlphaGet(const long x,const long y);
//@}
#endif //CXIMAGE_SUPPORT_ALPHA

public:
#if CXIMAGE_SUPPORT_LAYERS
/** \addtogroup Layers */ //@{
	virtual bool LayerCreate(long position = -1);
	bool LayerCreate(long position, CxImage *pNewLayer, long x = 0, long y = 0);
	bool LayerDelete(long position = -1, CxImage ** ppDeletedLayer = NULL);
	void LayerDeleteAll();
	CxImage* GetLayer(long position) const;
	void PushAndSortLayers();
	void PullLayers();
	inline void SetWorkingLayer (int nPos) {info.nWorkingPos = nPos;}
	CxImage* GetWorkingLayer() const {return GetLayer(info.nWorkingPos);}
	void GetOriginCoordinates (int &nPixX, int &nPixY) const{
		nPixX = info.xOffset;nPixY = info.yOffset; }
	long GetLayerPos(const CxImage *pLayer) const;
	CxImage* GetParent() const;
	void SetParent (CxImage* pParent);
	long GetNumLayers() const;
	long LayerDrawAll(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bShowChessBackground = false, bool bMixAlpha = true);
	long LayerDrawAll(HDC hdc, const RECT& rect, RECT* pClipRect=NULL, bool bSmooth = false, bool bShowChessBackground = false, bool bMixAlpha = true);
	// layer cache to increase drawing speed
	void AddLayerCache(int nNbCaches);
	long SelectLayerCache(long position);
	long LayerDrawAllClearCache();
	long LayerDrawAllInClearedCache(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bShowChessBackground = false, bool bShowCurrentBackground = false, COLORREF cBackgroundColor = CLR_INVALID, COLORREF cTranspMask = CLR_INVALID);
	long LayerDrawAllInCache(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bShowChessBackground = false, bool bShowCurrentBackground = false, COLORREF cBackgroundColor = CLR_INVALID, COLORREF cTranspMask = CLR_INVALID);
	long LayerDrawAllInCache(HDC hdc, const RECT& rect, RECT* pClipRect=NULL, bool bSmooth = false, bool bShowChessBackground = false, bool bShowCurrentBackground = false, COLORREF cBackgroundColor = CLR_INVALID, COLORREF cTranspMask = CLR_INVALID);
	long LayerDrawAllFromCache (HDC hdc, int nDestX=0, int nDestY=0, unsigned char ucAlpha = 255, COLORREF cTranspMask = CLR_INVALID);
//@}
#endif //CXIMAGE_SUPPORT_LAYERS
	
protected:
/** \addtogroup Protected */ //@{
	void Startup(DWORD imagetype = 0);
	void CopyInfo(const CxImage &src);
	void Ghost(const CxImage *src);
	void RGBtoBGR(BYTE *buffer, int length);
	static float HueToRGB(float n1,float n2, float hue);
	void Bitfield2RGB(BYTE *src, DWORD redmask, DWORD greenmask, DWORD bluemask, BYTE bpp);
	static void Bitfield2RGB32(BYTE *pSrcBits, LPBITMAPINFOHEADER pSrcHeader, int nWidth, int nHeight, 
								LPBITMAPINFO &pDst);
	static int CompareColors(const void *elem1, const void *elem2);
	short ntohs(const short word);
	long ntohl(const long dword);
	void bihtoh(BITMAPINFOHEADER* bih);

	// smart layer
	virtual bool LPE (IGLibrary::IGMarker **ppMarkers = NULL, int nNbMarkers = 0) {return true;}
	
	CxImage*			pWorkingLayer;
	std::list<CxImage*> lpLayers;//generic layers
	std::list<CxImage*> lpPushedLayers;//backup of the layers (for the sort needed for the history)
	std::list<CxLayerCache*> lpLayerCaches;//layer cache to increase drawing speed
	CxLayerCache*		pSelectedLayerCache;
	CxImage**			ppFrames;
	COLORREF			m_cSelection;
	std::wstring		m_wsGuid;
//@}
};

#include "ximasel.h.inline"

////////////////////////////////////////////////////////////////////////////
#endif // !defined(__CXIMAGE_H)
