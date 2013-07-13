#pragma once

#include "IGThread.h"
#include "IGUserMessages.h"
#include "ximadef.h"	// defines which formats are supported
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CxImage formats enumerator
enum ENUM_CXIMAGE_FORMATS{
CXIMAGE_FORMAT_UNKNOWN = 0,
#if CXIMAGE_SUPPORT_BMP
CXIMAGE_FORMAT_BMP = 1,
#endif
#if CXIMAGE_SUPPORT_GIF
CXIMAGE_FORMAT_GIF = 2,
#endif
#if CXIMAGE_SUPPORT_JPG
CXIMAGE_FORMAT_JPG = 3,
#endif
#if CXIMAGE_SUPPORT_PNG
CXIMAGE_FORMAT_PNG = 4,
#endif
#if CXIMAGE_SUPPORT_ICO
CXIMAGE_FORMAT_ICO = 5,
#endif
#if CXIMAGE_SUPPORT_TIF
CXIMAGE_FORMAT_TIF = 6,
#endif
#if CXIMAGE_SUPPORT_TGA
CXIMAGE_FORMAT_TGA = 7,
#endif
#if CXIMAGE_SUPPORT_PCX
CXIMAGE_FORMAT_PCX = 8,
#endif
#if CXIMAGE_SUPPORT_WBMP
CXIMAGE_FORMAT_WBMP = 9,
#endif
#if CXIMAGE_SUPPORT_WMF
CXIMAGE_FORMAT_WMF = 10,
#endif
#if CXIMAGE_SUPPORT_JP2
CXIMAGE_FORMAT_JP2 = 11,
#endif
#if CXIMAGE_SUPPORT_JPC
CXIMAGE_FORMAT_JPC = 12,
#endif
#if CXIMAGE_SUPPORT_PGX
CXIMAGE_FORMAT_PGX = 13,
#endif
#if CXIMAGE_SUPPORT_PNM
CXIMAGE_FORMAT_PNM = 14,
#endif
#if CXIMAGE_SUPPORT_RAS
CXIMAGE_FORMAT_RAS = 15,
#endif
#if CXIMAGE_SUPPORT_JBG
CXIMAGE_FORMAT_JBG = 16,
#endif
#if CXIMAGE_SUPPORT_MNG
CXIMAGE_FORMAT_MNG = 17,
#endif
#if CXIMAGE_SUPPORT_SKA
CXIMAGE_FORMAT_SKA = 18,
#endif
#if CXIMAGE_SUPPORT_RAW
CXIMAGE_FORMAT_RAW = 19,
#endif
#if CXIMAGE_SUPPORT_IG
CXIMAGE_FORMAT_IG = 20,
#endif
#if CXIMAGE_SUPPORT_DZ
CXIMAGE_FORMAT_DZ = 21,
#endif
CMAX_IMAGE_FORMATS = CXIMAGE_SUPPORT_BMP + CXIMAGE_SUPPORT_GIF + CXIMAGE_SUPPORT_JPG +
					 CXIMAGE_SUPPORT_PNG + CXIMAGE_SUPPORT_MNG + CXIMAGE_SUPPORT_ICO +
					 CXIMAGE_SUPPORT_TIF + CXIMAGE_SUPPORT_TGA + CXIMAGE_SUPPORT_PCX +
					 CXIMAGE_SUPPORT_WBMP+ CXIMAGE_SUPPORT_WMF +
					 CXIMAGE_SUPPORT_JBG + CXIMAGE_SUPPORT_JP2 + CXIMAGE_SUPPORT_JPC +
					 CXIMAGE_SUPPORT_PGX + CXIMAGE_SUPPORT_PNM + CXIMAGE_SUPPORT_RAS +
					 CXIMAGE_SUPPORT_SKA + CXIMAGE_SUPPORT_RAW + CXIMAGE_SUPPORT_IG + 
					 CXIMAGE_SUPPORT_DZ + 1
};

class CxImageThread;
class CvLib;

//extensible information collector
typedef struct tagCxImageInfo {

	tagCxImageInfo(){}
	~tagCxImageInfo(){
	}

	DWORD	dwEffWidth;			///< DWORD aligned scan line width
	BYTE*	pImage;				///< THE IMAGE BITS
	CxImageThread* pGhost;		///< if this is a ghost, pGhost points to the body
	CxImageThread* pParent;		///< if this is a layer, pParent points to the body
	DWORD	dwType;				///< original image format
	char	tcLastError[256];	///< debugging
	HWND	hProgress;
	long	nProgress;			///< monitor
	long	nEscape;			///< escape
	long	nBkgndIndex;		///< used for GIF, PNG, MNG
	RGBQUAD nBkgndColor;		///< used for RGB transparency
	long	nTransMode;			///< used for RGB transparency
	float	fQuality;			///< used for JPEG, JPEG2000 (0.0f ... 100.0f)
	BYTE	nJpegScale;			///< used for JPEG [ignacio]
	long	nFrame;				///< used for TIF, GIF, MNG : actual frame
	long	nNumFrames;			///< used for TIF, GIF, MNG : total number of frames
	DWORD	dwFrameDelay;		///< used for GIF, MNG
	long	xDPI;				///< horizontal resolution
	long	yDPI;				///< vertical resolution
	RECT	rSelectionBox;		///< bounding rectangle
	BYTE	nAlphaMax;			///< max opacity (fade)
	bool	bAlphaPaletteEnabled; ///< true if alpha values in the palette are enabled.
	bool	bEnabled;			///< enables the painting functions
	long	xOffset;
	long	yOffset;
	DWORD	dwCodecOpt[CMAX_IMAGE_FORMATS];	///< for GIF, TIF : 0=def.1=unc,2=fax3,3=fax4,4=pack,5=jpg
	RGBQUAD last_c;				///< for GetNearestIndex optimization
	BYTE	last_c_index;
	bool	last_c_isvalid;
	long	nNumLayers;
	DWORD	dwFlags;			///< 0x??00000 = reserved, 0x00??0000 = blend mode, 0x0000???? = layer id - user flags
	BYTE	dispmeth;
	bool	bGetAllFrames;
	bool	bLittleEndianHost;
	float	fZoomCurrent;
	int		nNbDZThreads;
	int		nNbDZSubThreads;
	bool	bFullDZUpdate;
	int		nWorkingPos;
	RECT	rcUpdateBox;
	RECT	rcLastUpdateBox;
	TCHAR	twOutputPath[256];
	TCHAR	twLastOutputPath[256];
	TCHAR	twLastReqGuid[256];
	CvLib	*pFaceDetection;
} CXIMAGEINFO;

class CxImageThread : public IGLibrary::IGThread
{
	friend class CxImageIG;
	friend class CxImageDZThreadSampler;
public:
	CxImageThread(void);
	virtual ~CxImageThread(void);

	virtual void SetNbDZThreads (int nNbSamplingThreads, int nNbCroppingAndSavingSubThreads);

	inline void SetProgressBar (HWND hProgress) {info.hProgress = hProgress;}
	inline HWND GetProgressBar() const {return info.hProgress;}
	inline void StepIt (int nSteps) const {
		::SendMessageW (info.hProgress, UM_PROGRESS_STEPIT, (WPARAM)nSteps, 0);
	}

protected:
	virtual bool Start();
	virtual bool Exit();
	virtual void ThreadProc (IGLibrary::IGThreadMessage *pMessage);

	// Process notification and kill message
	void EndThreadProc (IGLibrary::IGThreadMessage *pMessage, bool bSuccess = true);

	////////////////////////////////////
	// Image processing datas - use with care (not thread-safe)
	void				*pDib; //contains the header, the palette, the pixels
	BYTE				*pSelection; //selected region
	BYTE				*pAlpha; //alpha channel
	////////////////////////////////////

	///////////////////////////////////////////////////////////
	// Image thread datas, accessed through the critical section
	BITMAPINFOHEADER    head; //standard header
	CXIMAGEINFO			info; //extended information
	///////////////////////////////////////////////////////////

	// DeepZoom sampler threads
	std::vector <CxImageDZThreadSampler*> vDZThreads;
};