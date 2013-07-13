// xImaSel.cpp : Selection functions
/* 07/08/2001 v1.00 - Davide Pizzolato - www.xdp.it
 * CxImage version 6.0.0 02/Feb/2008
 */
#include "stdafx.h"
#include "ximage.h"
#include <IGSmartLayer.h>
#include <list>

using namespace std;
using namespace IGLibrary;

#if CXIMAGE_SUPPORT_SELECTION

////////////////////////////////////////////////////////////////////////////////
/**
 * Checks if the image has a valid selection.
 */
bool CxImage::SelectionIsValid()
{
	if (info.rSelectionBox.right < 0 || info.rSelectionBox.bottom < 0 || info.rSelectionBox.left < 0 || info.rSelectionBox.top < 0 ||
		info.rSelectionBox.right < info.rSelectionBox.left || info.rSelectionBox.top < info.rSelectionBox.bottom)
		return false;
	return pSelection!=0;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the smallest rectangle that contains the selection 
 */
void CxImage::SelectionGetBox(RECT& r)
{
	memcpy(&r,&info.rSelectionBox,sizeof(RECT));
	if ((r.left >= r.right) || (r.bottom >= r.top)){
		r.left = -1;
		r.right = -1;
		r.top = -1;
		r.bottom = -1;
	}
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Empties the selection.
 */
bool CxImage::SelectionClear(BYTE level)
{
	if (!pSelection)
		SelectionCreate();
	
	if (level==0){
		memset(pSelection,0,head.biWidth * head.biHeight);
		info.rSelectionBox.left = 0;
		info.rSelectionBox.right = -1;
		info.rSelectionBox.bottom = 0;
		info.rSelectionBox.top = -1;
	} else {
		memset(pSelection,level,head.biWidth * head.biHeight);
		info.rSelectionBox.left = 0;
		info.rSelectionBox.right = head.biWidth - 1;
		info.rSelectionBox.bottom = 0;
		info.rSelectionBox.top = head.biHeight - 1;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Allocates an empty selection.
 */
bool CxImage::SelectionCreate()
{
	SelectionDelete();
	pSelection = (BYTE*)calloc(1, head.biWidth * head.biHeight);
	return (pSelection!=0);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Deallocates the selction.
 */
bool CxImage::SelectionDelete()
{
	if (pSelection){
		free(pSelection);
		pSelection=NULL;
	}
	info.rSelectionBox.left = head.biWidth - 1;
	info.rSelectionBox.bottom = head.biHeight - 1;
	info.rSelectionBox.right = info.rSelectionBox.top = 0;
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Checks if the coordinates are inside the selection.
 */
bool CxImage::SelectionIsInside(long x, long y)
{
	if (IsInside(x,y)){
		if (pSelection==NULL) return true;
		return pSelection[x+y*head.biWidth]!=0;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////

bool CxImage::SelectionAndRect(RECT r, BYTE level)
{
	if (pSelection==NULL)
		return true;
	int nSize = head.biWidth * head.biHeight;
	BYTE *pCopySelection = new BYTE [nSize];
	::memcpy (pCopySelection, pSelection, nSize);
	SelectionClear (level == 0 ? 255 : 0);
	BYTE *pCurFromSel = pCopySelection + min (r.top, r.bottom) * head.biWidth + min (r.left, r.right);
	BYTE *pCurDstSel = pSelection + min (r.top, r.bottom) * head.biWidth + min (r.left, r.right);
	int nRectHeight = abs (r.top - r.bottom);
	int nRectWidth = abs (r.left - r.right);
	for (long y = 0; y < nRectHeight; y++){
		::memcpy (pCurDstSel, pCurFromSel, nRectWidth);
		pCurFromSel += head.biWidth;
		pCurDstSel += head.biWidth;
	}
	delete [] pCopySelection;
	return true;
}

bool CxImage::SelectionAnd (CxImage& src, BYTE level)
{
	if (pSelection==NULL)
		return true;
	if (src.GetWidth() != head.biWidth || src.GetHeight() != head.biHeight)
		return false;
	int nSize = head.biWidth * head.biHeight;
	BYTE *pCopySelection = new BYTE [nSize];
	::memcpy (pCopySelection, pSelection, nSize);
	BYTE *pCurFromSel = pCopySelection + min (info.rSelectionBox.top, info.rSelectionBox.bottom) * head.biWidth + min (info.rSelectionBox.left, info.rSelectionBox.right);
	BYTE *pCurDstSel = pSelection + min (info.rSelectionBox.top, info.rSelectionBox.bottom) * head.biWidth + min (info.rSelectionBox.left, info.rSelectionBox.right);
	BYTE *pCurAndSel = src.SelectionGetPointer() + min (info.rSelectionBox.top, info.rSelectionBox.bottom) * head.biWidth + min (info.rSelectionBox.left, info.rSelectionBox.right);
	int nRectHeight = abs (info.rSelectionBox.top - info.rSelectionBox.bottom) + 1;
	int nRectWidth = abs (info.rSelectionBox.left - info.rSelectionBox.right) + 1;
	SelectionClear (level == 0 ? 255 : 0);
	for (long y = 0; y < nRectHeight; y++){
		for (long x = 0; x < nRectWidth; x++){
			if (*pCurAndSel == level)
				*pCurDstSel = *pCurFromSel;
			pCurFromSel++;
			pCurDstSel++;
			pCurAndSel++;
		}
		pCurFromSel += head.biWidth - nRectWidth;
		pCurDstSel += head.biWidth - nRectWidth;
		pCurAndSel += head.biWidth - nRectWidth;
	}
	delete [] pCopySelection;
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Adds a rectangle to the existing selection.
 */
bool CxImage::SelectionAddRect(RECT r, BYTE level)
{
	if (pSelection==NULL) SelectionCreate();
	if (pSelection==NULL) return false;

	RECT r2;
	if (r.left<r.right) {r2.left=r.left; r2.right=r.right; } else {r2.left=r.right ; r2.right=r.left; }
	if (r.bottom<r.top) {r2.bottom=r.bottom; r2.top=r.top; } else {r2.bottom=r.top ; r2.top=r.bottom; }

	if (info.rSelectionBox.top < r2.top) info.rSelectionBox.top = max(0L,min(head.biHeight - 1,r2.top));
	if (info.rSelectionBox.left > r2.left) info.rSelectionBox.left = max(0L,min(head.biWidth - 1,r2.left));
	if (info.rSelectionBox.right < r2.right) info.rSelectionBox.right = max(0L,min(head.biWidth - 1,r2.right));
	if (info.rSelectionBox.bottom > r2.bottom) info.rSelectionBox.bottom = max(0L,min(head.biHeight - 1,r2.bottom));

	long ymin = max(0L,min(head.biHeight - 1,r2.bottom));
	long ymax = max(0L,min(head.biHeight - 1,r2.top+1));
	long xmin = max(0L,min(head.biWidth - 1,r2.left));
	long xmax = max(0L,min(head.biWidth - 1,r2.right+1));

	for (long y=ymin; y<=ymax; y++)
		memset(pSelection + xmin + y * head.biWidth, level, xmax-xmin);

	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Adds an ellipse to the existing selection.
 */
bool CxImage::SelectionAddEllipse(RECT r, BYTE level)
{
	
	

	if (pSelection==NULL) SelectionCreate();
	if (pSelection==NULL) return false;

	long xradius = abs(r.right - r.left)/2;
	long yradius = abs(r.top - r.bottom)/2;
	if (xradius==0 || yradius==0) return false;

	long xcenter = (r.right + r.left)/2;
	long ycenter = (r.top + r.bottom)/2;

	if (info.rSelectionBox.left > (xcenter - xradius)) info.rSelectionBox.left = max(0L,min(head.biWidth - 1,(xcenter - xradius)));
	if (info.rSelectionBox.right < (xcenter + xradius)) info.rSelectionBox.right = max(0L,min(head.biWidth - 1,(xcenter + xradius)));
	if (info.rSelectionBox.bottom > (ycenter - yradius)) info.rSelectionBox.bottom = max(0L,min(head.biHeight - 1,(ycenter - yradius)));
	if (info.rSelectionBox.top < (ycenter + yradius)) info.rSelectionBox.top = max(0L,min(head.biHeight - 1,(ycenter + yradius)));

	long xmin = max(0L,min(head.biWidth - 1,xcenter - xradius));
	long xmax = max(0L,min(head.biWidth - 1,xcenter + xradius));
	long ymin = max(0L,min(head.biHeight - 1,ycenter - yradius));
	long ymax = max(0L,min(head.biHeight - 1,ycenter + yradius));

	long y,yo;
	for (y=ymin; y<min(ycenter,ymax+1); y++){
		for (long x=xmin; x<=xmax; x++){
			yo = (long)(ycenter - yradius * sqrt(1-pow((float)(x - xcenter)/(float)xradius,2)));
			if (yo<y) pSelection[x + y * head.biWidth] = level;
		}
	}
	for (y=ycenter; y<=ymax; y++){
		for (long x=xmin; x<=xmax; x++){
			yo = (long)(ycenter + yradius * sqrt(1-pow((float)(x - xcenter)/(float)xradius,2)));
			if (yo>y) pSelection[x + y * head.biWidth] = level;
		}
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Inverts the selection.
 * Note: the SelectionBox is set to "full image", call SelectionGetBox before (if necessary)
 */
bool CxImage::SelectionInvert()
{
	
	

	if (pSelection) {
		BYTE *iSrc=pSelection;
		long n=head.biHeight*head.biWidth;
		for(long i=0; i < n; i++){
			*iSrc=(BYTE)~(*(iSrc));
			iSrc++;
		}		
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Imports an existing region from another image with the same width and height.
 */
bool CxImage::SelectionCopy(CxImage &from)
{
	if (from.pSelection == NULL || head.biWidth != from.head.biWidth || head.biHeight != from.head.biHeight) return false;
	if (pSelection==NULL) pSelection = (BYTE*)malloc(head.biWidth * head.biHeight);
	if (pSelection==NULL) return false;
	memcpy(pSelection,from.pSelection,head.biWidth * head.biHeight);
	memcpy(&info.rSelectionBox,&from.info.rSelectionBox,sizeof(RECT));
	return true;
}

bool CxImage::SelectionCopyAdd(CxImage &from)
{
	if (from.pSelection == NULL || head.biWidth != from.head.biWidth || head.biHeight != from.head.biHeight) return false;
	if (pSelection==NULL) SelectionClear();
	if (pSelection==NULL) return false;
	int nSize = head.biWidth * head.biHeight;
	while (--nSize){
		if (from.pSelection[nSize] != 0)
			pSelection[nSize] = 255;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Adds a polygonal region to the existing selection. points points to an array of POINT structures.
 * Each structure specifies the x-coordinate and y-coordinate of one vertex of the polygon.
 * npoints specifies the number of POINT structures in the array pointed to by points.
 */
bool CxImage::SelectionAddPolygon(const std::list<POINT>& lPts, BYTE level, bool bFill)
{	
	int npoints = lPts.size();
	if (npoints<3) return false;

	if (pSelection==NULL) SelectionCreate();
	if (pSelection==NULL) return false;

	BYTE* plocal = (BYTE*)calloc(1, head.biWidth * head.biHeight);
	RECT localbox = {head.biWidth,0,0,head.biHeight};

	long x,y,i=0;
	std::list<POINT>::const_iterator itCurrent = lPts.begin();
	std::list<POINT>::const_iterator itNext = lPts.begin();
	++itNext;
	std::list<POINT>::const_iterator itPrevious = lPts.begin();
	POINT current = {-1, -1};
	POINT next = {-1, -1};
	POINT start = {-1, -1};
	POINT previous = {-1, -1};

	//trace contour
	while (i < npoints){
		current = *itCurrent;		
		if ((current.x==-1) || (current.y==-1))
			return false;

		if (i == 0)
			start = current;
		else
			previous = *itPrevious;

		if ((i+1) == npoints)
			next = start;
		else
			next = *itNext;

		float beta;
		if (current.x != next.x){
			beta = (float)(next.y - current.y)/(float)(next.x - current.x);
			if (current.x < next.x){
				for (x=current.x; x<=next.x; x++){
					y = (long)(current.y + (x - current.x) * beta);
					if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
				}
			} else {
				for (x=current.x; x>=next.x; x--){
					y = (long)(current.y + (x - current.x) * beta);
					if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
				}
			}
		}
		if (current.y != next.y){
			beta = (float)(next.x - current.x)/(float)(next.y - current.y);
			if (current.y < next.y){
				for (y=current.y; y<=next.y; y++){
					x = (long)(current.x + (y - current.y) * beta);
					if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
				}
			} else {
				for (y=current.y; y>=next.y; y--){
					x = (long)(current.x + (y - current.y) * beta);
					if (IsInside(x,y)) plocal[x + y * head.biWidth] = 255;
				}
			}
		}

		RECT r2;
		if (current.x < next.x) {r2.left=current.x; r2.right=next.x; } else {r2.left=next.x ; r2.right=current.x; }
		if (current.y < next.y) {r2.bottom=current.y; r2.top=next.y; } else {r2.bottom=next.y ; r2.top=current.y; }
		if (localbox.top < r2.top) localbox.top = max(0L,min(head.biHeight-1,r2.top+1));
		if (localbox.left > r2.left) localbox.left = max(0L,min(head.biWidth-1,r2.left-1));
		if (localbox.right < r2.right) localbox.right = max(0L,min(head.biWidth-1,r2.right+1));
		if (localbox.bottom > r2.bottom) localbox.bottom = max(0L,min(head.biHeight-1,r2.bottom-1));

		++itCurrent;		
		if (i > 0)
			++itPrevious;
		if (i < npoints - 1)
			++itNext;
		i++;
	}

	POINT* pix = NULL;
	if (bFill){
		//fill the outer region
		long npix=(localbox.right - localbox.left)*(localbox.top - localbox.bottom);
		pix = (POINT*)calloc(sizeof(POINT), npix);
		BYTE back=0, mark=1;
		long fx, fy, fxx, fyy, first, last;
		long xmin = 0;
		long xmax = 0;
		long ymin = 0;
		long ymax = 0;

		for (int side=0; side<4; side++){
			switch(side){
			case 0:
				xmin=localbox.left; xmax=localbox.right+1; ymin=localbox.bottom; ymax=localbox.bottom+1;
				break;
			case 1:
				xmin=localbox.right; xmax=localbox.right+1; ymin=localbox.bottom; ymax=localbox.top+1;
				break;
			case 2:
				xmin=localbox.left; xmax=localbox.right+1; ymin=localbox.top; ymax=localbox.top+1;
				break;
			case 3:
				xmin=localbox.left; xmax=localbox.left+1; ymin=localbox.bottom; ymax=localbox.top+1;
				break;
			}
			//fill from the border points
			for(y=ymin;y<=ymax;y++){
				for(x=xmin;x<=xmax;x++){
					if (plocal[x+y*head.biWidth]==0){
						// Subject: FLOOD FILL ROUTINE              Date: 12-23-97 (00:57)       
						// Author:  Petter Holmberg                 Code: QB, QBasic, PDS        
						// Origin:  petter.holmberg@usa.net         Packet: GRAPHICS.ABC
						first=0;
						last=1;
						while(first!=last){
							fx = pix[first].x;
							fy = pix[first].y;
							fxx = fx + x;
							fyy = fy + y;
							for(;;)
							{
								if ((plocal[fxx + fyy*head.biWidth] == back) &&
									fxx>=localbox.left && fxx<=localbox.right && fyy>=localbox.bottom && fyy<=localbox.top )
								{
									plocal[fxx + fyy*head.biWidth] = mark;
									if (fyy > 0 && plocal[fxx + (fyy - 1)*head.biWidth] == back){
										pix[last].x = fx;
										pix[last].y = fy - 1;
										last++;
										if (last == npix) last = 0;
									}
									if ((fyy + 1)<head.biHeight && plocal[fxx + (fyy + 1)*head.biWidth] == back){
										pix[last].x = fx;
										pix[last].y = fy + 1;
										last++;
										if (last == npix) last = 0;
									}
								} else {
									break;
								}
								fx++;
								fxx++;
							};

							fx = pix[first].x - 1;
							fy = pix[first].y;
							fxx = fx + x;
							fyy = fy + y;

							for( ;; )
							{
								if ((plocal[fxx + fyy*head.biWidth] == back) &&
									fxx>=localbox.left && fxx<=localbox.right && fyy>=localbox.bottom && fyy<=localbox.top )
								{
									plocal[fxx + (y + fy)*head.biWidth] = mark;
									if (fyy > 0 && plocal[fxx + (fyy - 1)*head.biWidth] == back){
										pix[last].x = fx;
										pix[last].y = fy - 1;
										last++;
										if (last == npix) last = 0;
									}
									if ((fyy + 1)<head.biHeight && plocal[fxx + (fyy + 1)*head.biWidth] == back){
										pix[last].x = fx;
										pix[last].y = fy + 1;
										last++;
										if (last == npix) last = 0;
									}
								} else {
									break;
								}
								fx--;
								fxx--;
							}
						
							first++;
							if (first == npix) first = 0;
						}
					}
				}
			}
		}
	}

	//transfer the region
	long yoffset;
	for (y=localbox.bottom; y<=localbox.top; y++){
		yoffset = y * head.biWidth;
		for (x=localbox.left; x<=localbox.right; x++)
			if (plocal[x + yoffset]!=1) pSelection[x + yoffset]=level;
	}
	if (info.rSelectionBox.top < localbox.top) info.rSelectionBox.top = min(head.biHeight - 1,localbox.top);
	if (info.rSelectionBox.left > localbox.left) info.rSelectionBox.left = min(head.biWidth - 1,localbox.left);
	if (info.rSelectionBox.right < localbox.right) info.rSelectionBox.right = min(head.biWidth - 1,localbox.right);
	if (info.rSelectionBox.bottom > localbox.bottom) info.rSelectionBox.bottom = min(head.biHeight - 1,localbox.bottom);

	free(plocal);
	if (pix)
		free(pix);

	return true;
}

bool CxImage::SelectionAddMagic(POINT pt, BYTE level, int nTolerance)
{
	if (pSelection==NULL) SelectionCreate();
	if (pSelection==NULL) return false;	

	std::list <POINT> lPts;
	POINT	tptNeighbours[8];
	int nSizeTabIsDone = head.biWidth*head.biHeight;
	bool	*pbIsDone = new bool[nSizeTabIsDone];
	ZeroMemory (pbIsDone, nSizeTabIsDone);
	lPts.push_front (pt);
	int nIdxPixel = pt.x + pt.y * head.biWidth;
	pbIsDone[nIdxPixel] = true;
	pSelection[nIdxPixel] = level;
	BYTE *pByte = GetPixelColor24 (pt);
	BYTE cGray = RGB2GRAY(pByte[2],pByte[1],pByte[0]);
	int nNbPixSelected = 0;
	const int nNbPixStep = 200;
	ProgressSetRange (nSizeTabIsDone / nNbPixStep, 0);	
	while (!lPts.empty())
	{
		const POINT& ptCur = lPts.back();
		int nNbNeighbours = 0;
		GetPixelNeighbours (ptCur, tptNeighbours, &pbIsDone[ptCur.x + ptCur.y * head.biWidth], nNbNeighbours);
		while (nNbNeighbours--)
		{
			const POINT& ptCurNeighbour = tptNeighbours[nNbNeighbours];
			BYTE *pByteNeighbour = GetPixelColor24 (ptCurNeighbour);
			if (abs(RGB2GRAY(pByteNeighbour[2],pByteNeighbour[1],pByteNeighbour[0])
				-  cGray) <= nTolerance)
			{
				lPts.push_front (ptCurNeighbour);
				pSelection[ptCurNeighbour.x + ptCurNeighbour.y * head.biWidth] = level;
				info.rSelectionBox.top = max (ptCurNeighbour.y, info.rSelectionBox.top);
				info.rSelectionBox.bottom = min (ptCurNeighbour.y, info.rSelectionBox.bottom);
				info.rSelectionBox.left = min (ptCurNeighbour.x, info.rSelectionBox.left);
				info.rSelectionBox.right = max (ptCurNeighbour.x, info.rSelectionBox.right);				
				nNbPixSelected++;
			}
		}
		lPts.pop_back();
		if (nNbPixSelected%nNbPixStep == 0)
			ProgressStepIt();		 
					
	};
	delete [] pbIsDone;
	return true;
}

bool CxImage::SelectionCopyFromScaledLayer (CxImage& layerScaled)
{
	float fScale = (float)head.biWidth / (float)layerScaled.GetWidth();
	if (ABS((int)(fScale * (float)layerScaled.GetHeight()) - head.biHeight) > 1)
		return false;
	if (SelectionIsValid())
		SelectionCreate();
	BYTE *pCurSel = pSelection;
	for (int y = 0; y < head.biHeight; y++){
		for (int x = 0; x < head.biWidth; x++){
			int xScl = (int)((float)x/fScale);
			int yScl = (int)((float)y/fScale);
			*pCurSel++ = layerScaled.SelectionGet(xScl, yScl);
		}
	}
	return true;
}

bool CxImage::SelectionAddFaces (BYTE level)
{
	// sampling
	ProgressSetSubRange (0, 3);
	IGSmartLayer sampledLayer (*this);
	if (!sampledLayer.Resample())
		return false;
	ProgressSetRange (3, 0);
	ProgressStepIt();

	// face detection	
	int nRot = sampledLayer.DetectFaces();
	ProgressStepIt();

	// rotation	
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f);

	// indexing sampled layer
	ProgressSetSubRange (1, 3);
	if (!sampledLayer.IndexFaces())
		return false;

	ProgressSetSubRange (2, 3);
	ProgressSetRange (3, 0);
	sampledLayer.SelectIndexedRegions (-1, level);
	ProgressStepIt();

	// undo rotation
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)-nRot * PI / 180.0f), 1.0f);
	ProgressStepIt();

	if (!SelectionCopyFromScaledLayer (sampledLayer))
		return false;
	SelectionRebuildBox();
	ProgressStepIt();
	return true;
}

bool CxImage::SelectionAddEyes (BYTE level)
{
	ProgressSetSubRange (0, 3);
	// sampling
	IGSmartLayer sampledLayer (*this);
	if (!sampledLayer.Resample())
		return false;
	ProgressSetRange (3, 0);
	ProgressStepIt();

	// face detection	
	int nRot = sampledLayer.DetectFaces();
	bool bFullSelect = !sampledLayer.SelectionIsValid();
	ProgressStepIt();

	// rotation	
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f);

	// indexing sampled layer
	ProgressSetSubRange (1, 3);
	if (!sampledLayer.IndexFaces())
		return false;
	
	ProgressSetSubRange (2, 3);
	ProgressSetRange (3, 0);
	if (!sampledLayer.SelectEyes (level))
		return false;
	ProgressStepIt();

	// undo rotation
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)-nRot * PI / 180.0f), 1.0f, bFullSelect);
	ProgressStepIt();

	if (!SelectionCopyFromScaledLayer (sampledLayer))
		return false;
	SelectionRebuildBox();
	ProgressStepIt();
	return true;
}

bool CxImage::SelectionAddMouth (BYTE level)
{
	ProgressSetSubRange (0, 3);
	// sampling
	IGSmartLayer sampledLayer (*this);
	if (!sampledLayer.Resample())
		return false;
	ProgressSetRange (3, 0);
	ProgressStepIt();

	// face detection	
	int nRot = sampledLayer.DetectFaces();
	bool bFullSelect = !sampledLayer.SelectionIsValid();
	ProgressStepIt();

	// rotation	
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f);

	// indexing sampled layer
	ProgressSetSubRange (1, 3);
	if (!sampledLayer.IndexFaces())
		return false;

	ProgressSetSubRange (2, 3);
	ProgressSetRange (3, 0);
	if (!sampledLayer.SelectMouth (level))
		return false;
	ProgressStepIt();

	// undo rotation
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)-nRot * PI / 180.0f), 1.0f, bFullSelect);
	ProgressStepIt();

	if (!SelectionCopyFromScaledLayer (sampledLayer))
		return false;
	SelectionRebuildBox();
	ProgressStepIt();
	return true;
}

bool CxImage::SelectionAddNoze (BYTE level)
{
	ProgressSetSubRange (0, 3);
	// sampling
	IGSmartLayer sampledLayer (*this);
	if (!sampledLayer.Resample())
		return false;
	ProgressSetRange (3, 0);
	ProgressStepIt();

	// face detection	
	int nRot = sampledLayer.DetectFaces();
	bool bFullSelect = !sampledLayer.SelectionIsValid();
	ProgressStepIt();

	// rotation	
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)nRot * PI / 180.0f), 1.0f);

	// indexing sampled layer
	ProgressSetSubRange (1, 3);
	if (!sampledLayer.IndexFaces())
		return false;

	ProgressSetSubRange (2, 3);
	ProgressSetRange (3, 0);
	if (!sampledLayer.SelectNoze (level))
		return false;
	ProgressStepIt();

	// undo rotation
	if (nRot != 0)
		sampledLayer.RotateAndResize (cosf((float)nRot * PI / 180.0f), sinf((float)-nRot * PI / 180.0f), 1.0f, bFullSelect);
	ProgressStepIt();

	if (!SelectionCopyFromScaledLayer (sampledLayer))
		return false;
	SelectionRebuildBox();
	ProgressStepIt();
	return true;
}

bool CxImage::SelectionAddLPE(const std::list<POINT>& lPts, BYTE level)
{
	// sampling
	IGSmartLayer sampledLayer;
	sampledLayer.Copy (*this, true, true);
	float fScale;
	if (head.biHeight < head.biWidth){
		fScale = (float)IGSMARTLAYER_INDEXSAMPLE_SIZE / (float)head.biWidth;
		sampledLayer.CxImage::Resample (IGSMARTLAYER_INDEXSAMPLE_SIZE,  (int)((float)head.biHeight * fScale));
	}
	else{
		fScale = (float)IGSMARTLAYER_INDEXSAMPLE_SIZE / (float)head.biHeight;
		sampledLayer.CxImage::Resample ((int)((float)head.biWidth * fScale), IGSMARTLAYER_INDEXSAMPLE_SIZE);
	}
	std::list<POINT> lSampledPts (lPts);
	POINT ptCenter; ptCenter.x = 0; ptCenter.y = 0;
	for (std::list<POINT>::iterator itCurrent = lSampledPts.begin(); itCurrent != lSampledPts.end(); ++itCurrent){
		(*itCurrent).x = (int)((float)(*itCurrent).x * fScale);
		(*itCurrent).y = (int)((float)(*itCurrent).y * fScale);
		ptCenter.x += (*itCurrent).x;
		ptCenter.y += (*itCurrent).y;
	}
	ptCenter.x /= lSampledPts.size();
	ptCenter.y /= lSampledPts.size();
	int nStdDevX = 0; int nStdDevY = 0;
	for (std::list<POINT>::iterator itCurrent = lSampledPts.begin(); itCurrent != lSampledPts.end(); ++itCurrent){
		nStdDevX += abs((*itCurrent).x - ptCenter.x);
		nStdDevY += abs((*itCurrent).y - ptCenter.y);
	}
	int nStdDev = min(nStdDevX, nStdDevY) / lSampledPts.size();
	// segmentation
	IGSmartLayer layerBackground;
	layerBackground.Copy (sampledLayer);
	layerBackground.SelectionAddPolygon (lSampledPts, 255);
	layerBackground.SelectionInvert();
	layerBackground.SelectionRebuildBox();
	IGMarker cxMarkerBackgnd (IGMARKER_BACKGROUND, layerBackground);	
	IGMarkerCircle cxMarkerElem (IGMARKER_REGION1, ptCenter, max (2, nStdDev / 3), true);
	IGMarker* tM [2];
	tM [0] = &cxMarkerBackgnd;
	tM [1] = &cxMarkerElem;

	// indexing sampled layer
	int nNbRegions = 2;
	if (!sampledLayer.IndexLPE (tM, nNbRegions))
		return false;
	sampledLayer.SelectIndexedRegions (level);

	// apply circular structured element
	sampledLayer.StructuredElement <IGSTRUCTELEM_CIRCLE> (max (2, nStdDev / 20), NULL);
	
	// copy selection results
	if (!SelectionCopyFromScaledLayer (sampledLayer))
		return false;
	SelectionRebuildBox();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Adds to the selection all the pixels matching the specified color.
 */
bool CxImage::SelectionAddColor(RGBQUAD c, BYTE level)
{
    if (pSelection==NULL) SelectionCreate();
	if (pSelection==NULL) return false;

	RECT localbox = {head.biWidth,0,0,head.biHeight};

	if (head.biBitCount == 24){
		for (long y = 0; y < head.biHeight; y++){
			for (long x = 0; x < head.biWidth; x++){
				RGBQUAD color = BlindGetPixelColor(x, y);
				if (color.rgbRed   == c.rgbRed &&
					color.rgbGreen == c.rgbGreen &&
					color.rgbBlue  == c.rgbBlue)
				{
					pSelection[x + y * head.biWidth] = level;

					if (localbox.top < y) localbox.top = y;
					if (localbox.left > x) localbox.left = x;
					if (localbox.right < x) localbox.right = x;
					if (localbox.bottom > y) localbox.bottom = y;
				}
			}
		}
	}
	else if (head.biBitCount == 8)
	{
		int nSize = head.biWidth * head.biHeight;
		for (int nRow = 0; nRow < head.biHeight; nRow++){
			BYTE *pCurPixel = GetBits (nRow);
			BYTE *pCurSelection = SelectionGetPointer (0, nRow);
			for (int nCol = 0; nCol < head.biWidth; nCol++){
				if (*pCurPixel++ == c.rgbRed) {
					*pCurSelection = level;

					if (localbox.top < nRow) localbox.top = nRow;
					if (localbox.left > nCol) localbox.left = nCol;
					if (localbox.right < nCol) localbox.right = nCol;
					if (localbox.bottom > nRow) localbox.bottom = nRow;
				}
				pCurSelection++;				
			}
		}
	}
	else{
		return false;
	}

	if (info.rSelectionBox.top < localbox.top) info.rSelectionBox.top = localbox.top;
	if (info.rSelectionBox.left > localbox.left) info.rSelectionBox.left = localbox.left;
	if (info.rSelectionBox.right < localbox.right) info.rSelectionBox.right = localbox.right;
	if (info.rSelectionBox.bottom > localbox.bottom) info.rSelectionBox.bottom = localbox.bottom;

	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Adds a single pixel to the existing selection.
 */
bool CxImage::SelectionAddPixel(long x, long y, BYTE level)
{
	
	

    if (pSelection==NULL) SelectionCreate();
	if (pSelection==NULL) return false;

    if (IsInside(x,y)) {
        pSelection[x + y * head.biWidth] = level; // set the correct mask bit

		if (info.rSelectionBox.top < y) info.rSelectionBox.top = y;
		if (info.rSelectionBox.left > x) info.rSelectionBox.left = x;
		if (info.rSelectionBox.right < x) info.rSelectionBox.right = x;
		if (info.rSelectionBox.bottom > y) info.rSelectionBox.bottom = y;

        return true;
    }

    return false;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Exports the selection channel in a 8bpp grayscale image.
 */
bool CxImage::SelectionSplit(CxImage *dest)
{
	
	

	if (!pSelection || !dest) return false;

	CxImage tmp(head.biWidth,head.biHeight,8);
	if (!tmp.IsValid()){
		strcpy(info.tcLastError,tmp.GetLastError());
		return false;
	}

	for(long y=0; y<head.biHeight; y++){
		for(long x=0; x<head.biWidth; x++){
			tmp.BlindSetPixelIndex(x,y,pSelection[x+y*head.biWidth]);
		}
	}

	tmp.SetGrayPalette();
	dest->Transfer(tmp);

	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Creates the selection channel from a gray scale image.
 * black = unselected
 */
bool CxImage::SelectionSet(CxImage &from)
{
	
	

	if (!from.IsGrayScale() || head.biWidth != from.head.biWidth || head.biHeight != from.head.biHeight){
		strcpy(info.tcLastError,"CxImage::SelectionSet: wrong width or height, or image is not gray scale");
		return false;
	}

	if (pSelection==NULL) pSelection = (BYTE*)malloc(head.biWidth * head.biHeight);

	BYTE* src = from.info.pImage;
	BYTE* dst = pSelection;
	if (src==NULL || dst==NULL){
		strcpy(info.tcLastError,"CxImage::SelectionSet: null pointer");
		return false;
	}

	for (long y=0; y<head.biHeight; y++){
		memcpy(dst,src,head.biWidth);
		dst += head.biWidth;
		src += from.info.dwEffWidth;
	}

	SelectionRebuildBox();

	return true;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Sets the Selection level for a single pixel
 * internal use only: doesn't set SelectionBox. Use SelectionAddPixel
 */
void CxImage::SelectionSet(const long x,const long y,const BYTE level)
{
	
	

	if (pSelection && IsInside(x,y)) pSelection[x+y*head.biWidth]=level;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the Selection level for a single pixel 
 */
BYTE CxImage::SelectionGet(const long x,const long y)
{
	
	

	if (pSelection && IsInside(x,y)) return pSelection[x+y*head.biWidth];
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Rebuilds the SelectionBox 
 */
void CxImage::SelectionRebuildBox()
{
	
	

	info.rSelectionBox.left = head.biWidth - 1;
	info.rSelectionBox.bottom = head.biHeight - 1;
	info.rSelectionBox.right = info.rSelectionBox.top = 0;

	if (!pSelection)
		return;

	long x,y;

	for (y=0; y<head.biHeight; y++){
		for (x=0; x<info.rSelectionBox.left; x++){
			if (pSelection[x+y*head.biWidth]){
				info.rSelectionBox.left = x;
				continue;
			}
		}
	}

	for (y=0; y<head.biHeight; y++){
		for (x=head.biWidth-1; x>=info.rSelectionBox.right; x--){
			if (pSelection[x+y*head.biWidth]){
				info.rSelectionBox.right = x;
				continue;
			}
		}
	}

	for (x=0; x<head.biWidth; x++){
		for (y=0; y<info.rSelectionBox.bottom; y++){
			if (pSelection[x+y*head.biWidth]){
				info.rSelectionBox.bottom = y;
				continue;
			}
		}
	}

	for (x=0; x<head.biWidth; x++){
		for (y=head.biHeight-1; y>=info.rSelectionBox.top; y--){
			if (pSelection[x+y*head.biWidth]){
				info.rSelectionBox.top = y;
				continue;
			}
		}
	}

	if (!SelectionIsValid())
		SelectionDelete();
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Gets the Selection level for a single pixel 
 * "blind" version assumes that (x,y) is inside to the image.
 */
BYTE CxImage::BlindSelectionGet(const long x,const long y)
{
	
	

#ifdef _DEBUG
	if (!IsInside(x,y) || (pSelection==0))
  #if CXIMAGE_SUPPORT_EXCEPTION_HANDLING
		throw 0;
  #else
		return 0;
  #endif
#endif
	return pSelection[x+y*head.biWidth];
}
////////////////////////////////////////////////////////////////////////////////
/**
 * Returns pointer to selection data for pixel (x,y).
 */
BYTE* CxImage::SelectionGetPointer(const long x,const long y)
{
	
	

	if (pSelection) return pSelection+x+y*head.biWidth;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImage::SelectionFlip()
{
	
	

	if (!pSelection) return false;

	BYTE *buff = (BYTE*)malloc(head.biWidth);
	if (!buff) return false;

	BYTE *iSrc,*iDst;
	iSrc = pSelection + (head.biHeight-1)*head.biWidth;
	iDst = pSelection;
	for (long i=0; i<(head.biHeight/2); ++i)
	{
		memcpy(buff, iSrc, head.biWidth);
		memcpy(iSrc, iDst, head.biWidth);
		memcpy(iDst, buff, head.biWidth);
		iSrc-=head.biWidth;
		iDst+=head.biWidth;
	}

	free(buff);

	long top = info.rSelectionBox.top;
	info.rSelectionBox.top = head.biHeight - 1 - info.rSelectionBox.bottom;
	info.rSelectionBox.bottom = head.biHeight - 1 - top;
	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CxImage::SelectionMirror()
{
	
	

	if (!pSelection) return false;
	BYTE* pSelection2 = (BYTE*)malloc(head.biWidth * head.biHeight);
	if (!pSelection2) return false;
	
	BYTE *iSrc,*iDst;
	long wdt=head.biWidth-1;
	iSrc=pSelection + wdt;
	iDst=pSelection2;
	for(long y=0; y < head.biHeight; y++){
		for(long x=0; x <= wdt; x++)
			*(iDst+x)=*(iSrc-x);
		iSrc+=head.biWidth;
		iDst+=head.biWidth;
	}
	free(pSelection);
	pSelection=pSelection2;
	
	long left = info.rSelectionBox.left;
	info.rSelectionBox.left = head.biWidth - 1 - info.rSelectionBox.right;
	info.rSelectionBox.right = head.biWidth - 1 - left;
	return true;
}
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_WINDOWS
/**
 * Converts the selection in a HRGN object.
 */
bool CxImage::SelectionToHRGN(HRGN& region)
{
	
	

	if (pSelection && region){           
        for(int y = 0; y < head.biHeight; y++){
            HRGN hTemp = NULL;
            int iStart = -1;
            int x = 0;
			for(; x < head.biWidth; x++){
                if (pSelection[x + y * head.biWidth] != 0){
					if (iStart == -1) iStart = x;
					continue;
                }else{
                    if (iStart >= 0){
                        hTemp = CreateRectRgn(iStart, y, x, y + 1);
                        CombineRgn(region, hTemp, region, RGN_OR);
                        DeleteObject(hTemp);
                        iStart = -1;
                    }
                }
            }
            if (iStart >= 0){
                hTemp = CreateRectRgn(iStart, y, x, y + 1);
                CombineRgn(region, hTemp, region, RGN_OR);
                DeleteObject(hTemp);
                iStart = -1;
            }
        }
		return true;
    }
	return false;
}

bool CxImage::SelectionErode(int nSize)
{
	std::vector <BYTE*> vBoundaries;
	getBoundaries(vBoundaries);
	long nRay = nSize / 2;
	BYTE *pLastByte = pSelection + head.biWidth * head.biHeight;
	for (std::vector <BYTE*>::iterator it = vBoundaries.begin(); it != vBoundaries.end(); ++it){
		int iY = -nRay * head.biWidth;
		for(long y = -nRay; y < nRay; y++)
		{
			if ((pSelection > (*it) + y * head.biWidth) || ((*it) + y * head.biWidth >= pLastByte)){
				iY += head.biWidth;
				continue;
			}
			int iX = iY - nRay;
			for(long x = -nRay; x < nRay; x++)
			{
				if (pSelection > ((*it) + y * head.biWidth + x) || ((*it)+ y * head.biWidth + x) >= pLastByte){
					iX++;
					continue;
				}
				if (sqrt ((float)(x*x + y*y)) <= nRay){
					(*it)[iX++] = 0;
				}
				else{
					iX++;
				}
			}
			iY += head.biWidth;
		}
	}
	return true;
}

bool CxImage::SelectionDilate(int nSize)
{
	std::vector <BYTE*> vBoundaries;
	getBoundaries(vBoundaries);
	long nRay = nSize / 2;
	BYTE *pLastByte = pSelection + head.biWidth * head.biHeight;
	for (std::vector <BYTE*>::iterator it = vBoundaries.begin(); it != vBoundaries.end(); ++it){
		int iY = -nRay * head.biWidth;
		for(long y = -nRay; y < nRay; y++)
		{
			if ((pSelection > (*it) + y * head.biWidth) || ((*it) + y * head.biWidth >= pLastByte)){
				iY += head.biWidth;
				continue;
			}
			int iX = iY - nRay;
			for(long x = -nRay; x < nRay; x++)
			{
				if (pSelection > ((*it) + y * head.biWidth + x) || ((*it) + y * head.biWidth + x) >= pLastByte){
					iX++;
					continue;
				}
				if (sqrt ((float)(x*x + y*y)) <= nRay){
					(*it)[iX++] = 255;
				}
				else{
					iX++;
				}
			}
			iY += head.biWidth;
		}
	}
	return true;
}

void CxImage::getBoundaries (std::vector <BYTE*>& vBoundaries)
{
	for (int selY = info.rSelectionBox.bottom; selY <= info.rSelectionBox.top; selY++){
		BYTE *pSel = SelectionGetPointer (info.rSelectionBox.left, selY);
		for (int selX = info.rSelectionBox.left; selX <= info.rSelectionBox.right; selX++, pSel++){
			if (*pSel){				
				int nL = max (selX-1,0);
				int nB = max (selY-1,0);
				int nR = min (selX+1,head.biWidth - 1);
				int nT = min (selY+1,head.biHeight - 1);
				bool bIsBound = false;
				for (int neiY = nB; neiY <= nT && !bIsBound; neiY++){
					BYTE *pSelNeighbour = SelectionGetPointer (nL, neiY);
					for (int neiX = nL; neiX <= nR; neiX++){
						if (!*pSelNeighbour++){
							vBoundaries.push_back(pSel);
							bIsBound = true;
							break;
						}
					}
				}
			}
		}
	}
}

#endif //CXIMAGE_SUPPORT_WINDOWS
////////////////////////////////////////////////////////////////////////////////
#endif //CXIMAGE_SUPPORT_SELECTION
