// ximage.cpp : main implementation file
/* 07/08/2001 v1.00 - Davide Pizzolato - www.xdp.it
 * CxImage version 6.0.0 02/Feb/2008
 */
#include "stdafx.h"
#include "ximage.h"

////////////////////////////////////////////////////////////////////////////////
// CxImage 
////////////////////////////////////////////////////////////////////////////////
/**
 * Initialize the internal structures
 */
void CxImage::Startup(DWORD imagetype)
{
	//init pointers
	pDib = pSelection = pAlpha = NULL;
	AddLayerCache(2);
	SelectLayerCache (0);
	ppFrames = NULL;
	//init structures
	memset(&head,0,sizeof(BITMAPINFOHEADER));
	memset(&info,0,sizeof(CXIMAGEINFO));
	//init default attributes
    info.dwType = imagetype;
	info.fQuality = 90.0f;
	info.nAlphaMax = 255;
	info.nBkgndIndex = -1;
	info.bEnabled = true;
	info.fZoomCurrent = 1.0f;
	info.bFullDZUpdate = false;
	info.rcUpdateBox.right = -1;info.rcUpdateBox.bottom = -1;
	info.rcLastUpdateBox.right = -1;info.rcLastUpdateBox.bottom = -1;
	info.rSelectionBox.right = -1;info.rSelectionBox.bottom = -1;
	SetXDPI(CXIMAGE_DEFAULT_DPI);
	SetYDPI(CXIMAGE_DEFAULT_DPI);

	short test = 1;
	info.bLittleEndianHost = (*((char *) &test) == 1);

	pWorkingLayer = NULL;

//	m_nSelStep = 0; 
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Empty image constructor
 * \param imagetype: (optional) set the image format, see ENUM_CXIMAGE_FORMATS
 */
CxImage::CxImage(DWORD imagetype)
{
	Startup(imagetype);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Call this function to destroy image pixels, alpha channel, selection and sub layers.
 * - Attributes are not erased, but IsValid returns false.
 *
 * \return true if everything is freed, false if the image is a Ghost
 */
bool CxImage::Destroy()
{
	//free this only if it's valid and it's not a ghost
	if (info.pGhost==NULL){
		// deleting layer caches
		if (!lpLayerCaches.empty())
		{
			for (std::list<CxLayerCache*>::const_iterator iterCache = lpLayerCaches.begin(); 
				iterCache != lpLayerCaches.end(); ++iterCache)
				delete *iterCache;
			lpLayerCaches.clear();
		}		
		// deleting layers
		LayerDeleteAll();
		// deleting selection, alpha, dib
		if (pSelection) {free(pSelection); pSelection = NULL;}
		if (pAlpha) {free(pAlpha); pAlpha = NULL;}
		if (pDib) {free(pDib); pDib = NULL;}
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImage::DestroyFrames()
{
	if (info.pGhost==NULL) {
		if (ppFrames) {
			for (long n=0; n<info.nNumFrames; n++) { delete ppFrames[n]; }
			delete [] ppFrames; ppFrames = NULL; info.nNumFrames = 0;
		}
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sized image constructor
 * \param dwWidth: width
 * \param dwHeight: height
 * \param wBpp: bit per pixel, can be 1, 4, 8, 24
 * \param imagetype: (optional) set the image format, see ENUM_CXIMAGE_FORMATS
 */
CxImage::CxImage(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype, BYTE crInit)
{
	Startup(imagetype);
	Create(dwWidth,dwHeight,wBpp,imagetype,crInit);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * image constructor from existing source
 * \param src: source image.
 * \param copypixels: copy the pixels from the source image into the new image.
 * \param copyselection: copy the selection from source
 * \param copyalpha: copy the alpha channel from source
 * \sa Copy
 */
CxImage::CxImage(const CxImage &src, bool copypixels, bool copyselection, bool copyalpha)
{
	Startup(src.GetType());
	Copy(src,copypixels,copyselection,copyalpha);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Copies the image from an exsisting source
 * \param src: source image.
 * \param copypixels: copy the pixels from the source image into the new image.
 * \param copyselection: copy the selection from source
 * \param copyalpha: copy the alpha channel from source
 */
void CxImage::Copy(const CxImage &src, bool copypixels, bool copyselection, bool copyalpha, bool copylayers, int nOffsetX, int nOffsetY)
{
	
	

	// if the source is a ghost, the copy is still a ghost
	if (src.info.pGhost){
		Ghost(&src);
		return;
	}

	//copy the attributes
	if (nOffsetX == -1 && nOffsetY == -1){
		memcpy(&info,&src.info,sizeof(CXIMAGEINFO));
		memcpy(&head,&src.head,sizeof(BITMAPINFOHEADER)); // [andy] - fix for bitmap header DPI
		//rebuild the image
		Create(src.GetWidth(),src.GetHeight(),src.GetBpp(),src.GetType());
	}
	//copy the pixels and the palette, or at least copy the palette only.
	if (copypixels && pDib && src.pDib) memcpy(pDib,src.pDib,GetSize());
	else SetPalette(src.GetPalette());
	long nSize = head.biWidth * head.biHeight;
	//copy the selection
	if (copyselection && src.pSelection){
		if (pSelection) free(pSelection);
		pSelection = (BYTE*)malloc(nSize);
		::memset (pSelection, 0, nSize);
		if (nOffsetX == -1)
			nOffsetX = 0;
		if (nOffsetY == -1)
			nOffsetY = 0;
		BYTE *pCurSelection = pSelection + nOffsetY * head.biWidth + nOffsetX;
		BYTE *pCurSrcSelection = src.pSelection;
		for (int idxRow = 0; idxRow < src.head.biHeight; idxRow++){
			memcpy(pCurSelection, pCurSrcSelection, src.head.biWidth);
			pCurSelection += head.biWidth;
			pCurSrcSelection += src.head.biWidth;
		}
	}
	//copy the alpha channel
	if (copyalpha && src.pAlpha){
		if (pAlpha) free(pAlpha);
		pAlpha = (BYTE*)malloc(nSize);
		memcpy(pAlpha,src.pAlpha,nSize);
	}
	//copy the sublayers
	if (copylayers && !src.lpLayers.empty()){
		info.nNumLayers = src.info.nNumLayers;
		
		for (std::list<CxImage*>::const_iterator iterPict = src.lpLayers.begin(); 
			iterPict != lpLayers.end(); ++iterPict)
		{
			LayerCreate (0);
			CxImage *pNewLayer = GetLayer (0);
			pNewLayer->Copy (*(*iterPict), copypixels, copyselection, copyalpha, true);
		}
	}
	//copy the cache
	if (copylayers)
	{
		for (std::list<CxLayerCache*>::const_iterator iterCache = src.lpLayerCaches.begin(); 
			iterCache != src.lpLayerCaches.end(); ++iterCache)
		{
			lpLayerCaches.push_back (new CxLayerCache (*iterCache));
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Copies the image attributes from an existing image.
 * - Works only on an empty image, and the image will be still empty.
 * - <b> Use it before Create() </b>
 */
void CxImage::CopyInfo(const CxImage &src)
{
	
	

	if (pDib==NULL) {
		memcpy(&info,&src.info,sizeof(CXIMAGEINFO));
	}
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa Copy
 */
CxImage& CxImage::operator = (const CxImage& isrc)
{
	if (this != &isrc) Copy(isrc);
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Initializes or rebuilds the image.
 * \param dwWidth: width
 * \param dwHeight: height
 * \param wBpp: bit per pixel, can be 1, 4, 8, 24
 * \param imagetype: (optional) set the image format, see ENUM_CXIMAGE_FORMATS
 * \return pointer to the internal pDib object; NULL if an error occurs.
 */
void* CxImage::Create(DWORD dwWidth, DWORD dwHeight, DWORD wBpp, DWORD imagetype, BYTE crInit)
{
	// destroy the existing image (if any)
	if (!Destroy())
		return NULL;

	AddLayerCache(2);
	SelectLayerCache (0);

	// prevent further actions if width or height are not vaild <Balabasnia>
	if ((dwWidth == 0) || (dwHeight == 0)){
		strcpy(info.tcLastError,"CxImage::Create : width and height must be greater than zero");
		return NULL;
	}

    // Make sure bits per pixel is valid
    if		(wBpp <= 1)	wBpp = 1;
    else if (wBpp <= 4)	wBpp = 4;
    else if (wBpp <= 8)	wBpp = 8;
    else				wBpp = 24;

	// limit memory requirements (and also a check for bad parameters)
	if (((dwWidth*dwHeight*wBpp)>>3) > CXIMAGE_MAX_MEMORY ||
		((dwWidth*dwHeight*wBpp)/wBpp) != (dwWidth*dwHeight))
	{
		strcpy(info.tcLastError,"CXIMAGE_MAX_MEMORY exceeded");
		return NULL;
	}

	// set the correct bpp value
    switch (wBpp){
        case 1:
            head.biClrUsed = 2;	break;
        case 4:
            head.biClrUsed = 16; break;
        case 8:
            head.biClrUsed = 256; break;
        default:
            head.biClrUsed = 0;
    }

	//set the common image informations
	info.dwEffWidth = ((((wBpp * dwWidth) + 31) / 32) * 4);
    info.dwType = imagetype;

    // initialize BITMAPINFOHEADER
	head.biSize = sizeof(BITMAPINFOHEADER); //<ralphw>
    head.biWidth = dwWidth;		// fill in width from parameter
    head.biHeight = dwHeight;	// fill in height from parameter
    head.biPlanes = 1;			// must be 1
    head.biBitCount = (WORD)wBpp;		// from parameter
    head.biCompression = BI_RGB;    
    head.biSizeImage = info.dwEffWidth * dwHeight;
//    head.biXPelsPerMeter = 0; See SetXDPI
//    head.biYPelsPerMeter = 0; See SetYDPI
//    head.biClrImportant = 0;  See SetClrImportant

	pDib = malloc(GetSize()); // alloc memory block to store our bitmap
    if (!pDib){
		strcpy(info.tcLastError,"CxImage::Create can't allocate memory");
		return NULL;
	}

	//clear the palette
	RGBQUAD* pal=GetPalette();
	if (pal) memset(pal,0,GetPaletteSize());
	//Destroy the existing selection
#if CXIMAGE_SUPPORT_SELECTION
	if (pSelection) SelectionDelete();
#endif //CXIMAGE_SUPPORT_SELECTION
	//Destroy the existing alpha channel
#if CXIMAGE_SUPPORT_ALPHA
	if (pAlpha) AlphaDelete();
#endif //CXIMAGE_SUPPORT_ALPHA

    // use our bitmap info structure to fill in first part of
    // our DIB with the BITMAPINFOHEADER
    BITMAPINFOHEADER*  lpbi;
	lpbi = (BITMAPINFOHEADER*)(pDib);
    *lpbi = head;

	// Initialize bitmap bits to light grey
	info.pImage = GetBits();
	memset (info.pImage, crInit, head.biSizeImage);

	m_cSelection = 0xFF; // red is the default selection color
    return pDib; //return handle to the DIB
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return pointer to the image pixels. <b> USE CAREFULLY </b>
 */
BYTE* CxImage::GetBits(DWORD row)
{
	
	

	if (pDib){
		if (row) {
			if (row<(DWORD)head.biHeight){
				return ((BYTE*)pDib + *(DWORD*)pDib + GetPaletteSize() + (info.dwEffWidth * row));
			} else {
				return NULL;
			}
		} else {
			return ((BYTE*)pDib + *(DWORD*)pDib + GetPaletteSize());
		}
	}
	return NULL;
}

long CxImage::GetBitmapBits (HDC hMemDC, HBITMAP hMemBmp, int nSrcX, int nSrcY, LPBITMAPINFO &pBitmapInfo)
{
	SIZE szBitmap;
	if (!::GetBitmapDimensionEx (hMemBmp, &szBitmap))
		return 0;
	if (szBitmap.cx < 1 || szBitmap.cy < 1)
		return 0;
	BITMAPINFO bitmapInfo;
	::ZeroMemory (&bitmapInfo, sizeof (BITMAPINFOHEADER));
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// retrieve bitmap information header
	::GetDIBits(  hMemDC,				// handle to DC
				  hMemBmp,			// handle to bitmap
				  0,				// first scan line to set
				  szBitmap.cy,		// number of scan lines to copy
				  NULL,				// array for bitmap bits
				  &bitmapInfo,		// bitmap data buffer
				  DIB_RGB_COLORS );			// RGB or palette index	
	if ((bitmapInfo.bmiHeader.biCompression != BI_RGB) && 
				(bitmapInfo.bmiHeader.biCompression != BI_BITFIELDS))
				return 0;
	// allocate bitmap buffer
	bitmapInfo.bmiHeader.biSize = (bitmapInfo.bmiHeader.biCompression == BI_BITFIELDS) ? sizeof(BITMAPINFOHEADER) + 3 * sizeof (DWORD)
																							: sizeof(BITMAPINFOHEADER);
	int nSizeBitmapSrcBuffer = bitmapInfo.bmiHeader.biSize + bitmapInfo.bmiHeader.biSizeImage;
	LPBYTE pBitmapSrcBuffer = new BYTE [nSizeBitmapSrcBuffer];	
	::memcpy_s (pBitmapSrcBuffer, nSizeBitmapSrcBuffer,
				&bitmapInfo, sizeof (BITMAPINFOHEADER));
	if (bitmapInfo.bmiHeader.biCompression == BI_BITFIELDS)
	{
		LPBYTE pBitmapMask = pBitmapSrcBuffer + sizeof (BITMAPINFOHEADER);
		pBitmapMask [0] = 255; pBitmapMask [1] = 0; pBitmapMask [2] = 0;
		pBitmapMask [4+0] = 0; pBitmapMask [4+1] = 255; pBitmapMask [4+2] = 0;
		pBitmapMask [8+0] = 0; pBitmapMask [8+1] = 0; pBitmapMask [8+2] = 255;
	}	
	LPBITMAPINFO pBitmapSrcInfo = (LPBITMAPINFO)pBitmapSrcBuffer;
	
	// retrieve bitmap bits
	::GetDIBits(  hMemDC,				// handle to DC
				  hMemBmp,			// handle to bitmap
				  0,				// first scan line to set
				  szBitmap.cy,		// number of scan lines to copy
				  (bitmapInfo.bmiHeader.biCompression == BI_BITFIELDS) ? pBitmapSrcInfo->bmiColors + 3 
																		: pBitmapSrcInfo->bmiColors,	// array for bitmap bits
				  pBitmapSrcInfo,		// bitmap data buffer
				  DIB_RGB_COLORS );			// RGB or palette index	
	if (pBitmapSrcInfo->bmiHeader.biCompression == BI_BITFIELDS)
	{
		pBitmapInfo = pBitmapSrcInfo;
		CxImage::Bitfield2RGB32 ((LPBYTE)pBitmapSrcInfo->bmiColors, &pBitmapSrcInfo->bmiHeader,
									szBitmap.cx, szBitmap.cy, pBitmapInfo);
		delete [] pBitmapSrcBuffer;
	}
	else
	{
		pBitmapInfo = pBitmapSrcInfo;
	}
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return the size in bytes of the internal pDib object
 */
long CxImage::GetSize()
{
	return head.biSize + head.biSizeImage + GetPaletteSize();
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the image bits to the specified value
 * - for indexed images, the output color is set by the palette entries.
 * - for RGB images, the output color is a shade of gray.
 */
void CxImage::Clear(BYTE bval)
{
	
	

	if (pDib == 0) return;

	if (GetBpp() == 1){
		if (bval > 0) bval = 255;
	}
	if (GetBpp() == 4){
		bval = (BYTE)(17*(0x0F & bval));
	}

	memset(info.pImage,bval,head.biSizeImage);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Transfers the image from an existing source image. The source becomes empty.
 * \return true if everything is ok
 */
bool CxImage::Transfer(CxImage &from, bool bTransferFrames /*=true*/)
{
	if (!Destroy())
		return false;

	AddLayerCache((int)from.lpLayerCaches.size());
	SelectLayerCache(0);

	memcpy(&head,&from.head,sizeof(BITMAPINFOHEADER));
	memcpy(&info,&from.info,sizeof(CXIMAGEINFO));

	pDib = from.pDib;
	pSelection = from.pSelection;
	pAlpha = from.pAlpha;
	lpLayers = from.lpLayers;	
	 
	for (std::list<CxImage*>::const_iterator iterPict = lpLayers.begin();
		iterPict != lpLayers.end(); ++iterPict)
	{
		(*iterPict)->info.pParent = this;
	}

	memset(&from.head,0,sizeof(BITMAPINFOHEADER));
	memset(&from.info,0,sizeof(CXIMAGEINFO));
	from.pDib = from.pSelection = from.pAlpha = NULL;
	from.lpLayers.clear();

	if (bTransferFrames){
		DestroyFrames();
		ppFrames = from.ppFrames;
		from.ppFrames = NULL;
	}	
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * (this) points to the same pDib owned by (*from), the image remains in (*from)
 * but (this) has the access to the pixels. <b>Use carefully !!!</b>
 */
void CxImage::Ghost(const CxImage *from)
{
	if (from){		
		memcpy(&head,&from->head,sizeof(BITMAPINFOHEADER));
		memcpy(&info,&from->info,sizeof(CXIMAGEINFO));
		pDib = from->pDib;
		pSelection = from->pSelection;
		pAlpha = from->pAlpha;
		lpLayers = from->lpLayers;
		ppFrames = from->ppFrames;
		pWorkingLayer = from->GetWorkingLayer();	
		info.pGhost=(CxImage *)from;
		vDZThreads.insert (vDZThreads.end(), from->vDZThreads.begin(), from->vDZThreads.end());
	}
}

void CxImage::Bitfield2RGB32(BYTE *pSrcBits, LPBITMAPINFOHEADER pSrcHeader, int nWidth, int nHeight, 
								LPBITMAPINFO &pDst)
{	
	DWORD dwEffWidth = ((((32 * nWidth) + 31) / 32) * 4);
	DWORD ns[3]={0,0,0};
	DWORD *pdwMask = (DWORD *)pSrcBits;
	// compute the number of shift for each mask
	for (int i=8;i<32;i+=8){
		if (pdwMask[0]>>i) ns[0]++;
		if (pdwMask[1]>>i) ns[1]++;
		if (pdwMask[2]>>i) ns[2]++;
	}
	pSrcBits += 3 * sizeof (DWORD);
	// dword aligned width for 32 bit image
	long effwidth4 = nWidth * 4;
	long y4,x4;
	int nSizeBitmapDst = dwEffWidth * nHeight + sizeof(BITMAPINFOHEADER);
	pDst = (LPBITMAPINFO)new BYTE[nSizeBitmapDst];
	::memcpy_s (pDst, nSizeBitmapDst,
				pSrcHeader, sizeof (BITMAPINFOHEADER));
	pDst->bmiHeader.biCompression = BI_RGB;
	pDst->bmiHeader.biWidth = dwEffWidth / 4;
	LPBYTE p = (LPBYTE)pDst->bmiColors;
	// scan the buffer in reverse direction to avoid reallocations
	for (long y = nHeight - 1; y >= 0; y--){
		y4=effwidth4*y;
		for (long x = nWidth - 1; x >= 0; x--){
			x4 = 4*x+y4;
			p[  x4] = pSrcBits[ns[2]+x4];
			p[1+x4] = pSrcBits[ns[1]+x4];
			p[2+x4] = pSrcBits[ns[0]+x4];
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
/**
 * turns a 16 or 32 bit bitfield image into a RGB image
 */
void CxImage::Bitfield2RGB(BYTE *src, DWORD redmask, DWORD greenmask, DWORD bluemask, BYTE bpp)
{
	switch (bpp){
	case 16:
	{
		DWORD ns[3]={0,0,0};
		// compute the number of shift for each mask
		for (int i=0;i<16;i++){
			if ((redmask>>i)&0x01) ns[0]++;
			if ((greenmask>>i)&0x01) ns[1]++;
			if ((bluemask>>i)&0x01) ns[2]++;
		}
		ns[1]+=ns[0]; ns[2]+=ns[1];	ns[0]=8-ns[0]; ns[1]-=8; ns[2]-=8;
		// dword aligned width for 16 bit image
		long effwidth2=(((head.biWidth + 1) / 2) * 4);
		WORD w;
		long y2,y3,x2,x3;
		BYTE *p=info.pImage;
		// scan the buffer in reverse direction to avoid reallocations
		for (long y=head.biHeight-1; y>=0; y--){
			y2=effwidth2*y;
			y3=info.dwEffWidth*y;
			for (long x=head.biWidth-1; x>=0; x--){
				x2 = 2*x+y2;
				x3 = 3*x+y3;
				w = (WORD)(src[x2]+256*src[1+x2]);
				p[  x3]=(BYTE)((w & bluemask)<<ns[0]);
				p[1+x3]=(BYTE)((w & greenmask)>>ns[1]);
				p[2+x3]=(BYTE)((w & redmask)>>ns[2]);
			}
		}
		break;
	}
	case 32:
	{
		DWORD ns[3]={0,0,0};
		// compute the number of shift for each mask
		for (int i=8;i<32;i+=8){
			if (redmask>>i) ns[0]++;
			if (greenmask>>i) ns[1]++;
			if (bluemask>>i) ns[2]++;
		}
		// dword aligned width for 32 bit image
		long effwidth4 = head.biWidth * 4;
		long y4,y3,x4,x3;
		BYTE *p=info.pImage;
		// scan the buffer in reverse direction to avoid reallocations
		for (long y=head.biHeight-1; y>=0; y--){
			y4=effwidth4*y;
			y3=info.dwEffWidth*y;
			for (long x=head.biWidth-1; x>=0; x--){
				x4 = 4*x+y4;
				x3 = 3*x+y3;
				p[  x3]=src[ns[2]+x4];
				p[1+x3]=src[ns[1]+x4];
				p[2+x3]=src[ns[0]+x4];
			}
		}
	}

	}
	return;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Creates an image from a generic buffer
 * \param pArray: source memory buffer
 * \param dwWidth: image width
 * \param dwHeight: image height
 * \param dwBitsperpixel: can be 1,4,8,24,32
 * \param dwBytesperline: line alignment, in bytes, for a single row stored in pArray
 * \param bFlipImage: tune this parameter if the image is upsidedown
 * \return true if everything is ok
 */
bool CxImage::CreateFromArray(BYTE* pArray,DWORD dwWidth,DWORD dwHeight,DWORD dwBitsperpixel, DWORD dwBytesperline, bool bFlipImage)
{
	if (pArray==NULL) return false;
	if (!((dwBitsperpixel==1)||(dwBitsperpixel==4)||(dwBitsperpixel==8)||
		(dwBitsperpixel==24)||(dwBitsperpixel==32))) return false;

	if (!Create(dwWidth,dwHeight,dwBitsperpixel)) return false;

	if (dwBitsperpixel<24) SetGrayPalette();

#if CXIMAGE_SUPPORT_ALPHA
	if (dwBitsperpixel==32) AlphaCreate();
#endif //CXIMAGE_SUPPORT_ALPHA

	BYTE *dst,*src;

	for (DWORD y = 0; y<dwHeight; y++) {
		dst = info.pImage + (bFlipImage?(dwHeight-1-y):y) * info.dwEffWidth;
		src = pArray + y * dwBytesperline;
		if (dwBitsperpixel==32){
			for(DWORD x=0;x<dwWidth;x++){
				*dst++=src[0];
				*dst++=src[1];
				*dst++=src[2];
#if CXIMAGE_SUPPORT_ALPHA
				AlphaSet(x,(bFlipImage?(dwHeight-1-y):y),src[3]);
#endif //CXIMAGE_SUPPORT_ALPHA
				src+=4;
			}
		} else {
			memcpy(dst,src,min(info.dwEffWidth,dwBytesperline));
		}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \sa CreateFromArray
 */
bool CxImage::CreateFromMatrix(BYTE** ppMatrix,DWORD dwWidth,DWORD dwHeight,DWORD dwBitsperpixel, DWORD dwBytesperline, bool bFlipImage)
{
	if (ppMatrix==NULL) return false;
	if (!((dwBitsperpixel==1)||(dwBitsperpixel==4)||(dwBitsperpixel==8)||
		(dwBitsperpixel==24)||(dwBitsperpixel==32))) return false;

	if (!Create(dwWidth,dwHeight,dwBitsperpixel)) return false;

	if (dwBitsperpixel<24) SetGrayPalette();

#if CXIMAGE_SUPPORT_ALPHA
	if (dwBitsperpixel==32) AlphaCreate();
#endif //CXIMAGE_SUPPORT_ALPHA

	BYTE *dst,*src;

	for (DWORD y = 0; y<dwHeight; y++) {
		dst = info.pImage + (bFlipImage?(dwHeight-1-y):y) * info.dwEffWidth;
		src = ppMatrix[y];
		if (src){
			if (dwBitsperpixel==32){
				for(DWORD x=0;x<dwWidth;x++){
					*dst++=src[0];
					*dst++=src[1];
					*dst++=src[2];
#if CXIMAGE_SUPPORT_ALPHA
					AlphaSet(x,(bFlipImage?(dwHeight-1-y):y),src[3]);
#endif //CXIMAGE_SUPPORT_ALPHA
					src+=4;
				}
			} else {
				memcpy(dst,src,min(info.dwEffWidth,dwBytesperline));
			}
		}
	}
	UpdateImageParameters();
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * \return lightness difference between elem1 and elem2
 */
int CxImage::CompareColors(const void *elem1, const void *elem2)
{
	RGBQUAD* c1 = (RGBQUAD*)elem1;
	RGBQUAD* c2 = (RGBQUAD*)elem2;

	int g1 = (int)RGB2GRAY(c1->rgbRed,c1->rgbGreen,c1->rgbBlue);
	int g2 = (int)RGB2GRAY(c2->rgbRed,c2->rgbGreen,c2->rgbBlue);
	
	return (g1-g2);
}
////////////////////////////////////////////////////////////////////////////////
/**
 * simply calls "if (memblock) free(memblock);".
 * Useful when calling Encode for a memory buffer,
 * from a DLL compiled with different memory management options.
 * CxImage::FreeMemory will use the same memory environment used by Encode. 
 * \author [livecn]
 */
void CxImage::FreeMemory(void* memblock)
{
	if (memblock)
		free(memblock);
}

LPCWSTR CxImage::GetId() const
{
	return m_wsGuid.c_str(); 
}
////////////////////////////////////////////////////////////////////////////////
//EOF
