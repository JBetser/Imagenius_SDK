#include "StdAfx.h"
#include "IGConvertible.h"

using namespace IGLibrary;
using namespace std;

bad_exception IGToString::igproperty_exception = bad_exception ("IGProperty exception");

BEGIN_TOSTRING_MAP()
	TOSTRING_SUPPORT(int)
	TOSTRING_SUPPORT(double)
	TOSTRING_SUPPORT(wstring)
	TOSTRING_SUPPORT(IGConvertible)
	TOSTRING_SUPPORT(IGFRAMEPROPERTY_UNIT_ENUM)
	TOSTRING_SUPPORT(IGFRAMEPROPERTY_STRETCHING_ENUM)
	TOSTRING_SUPPORT(IGBRUSH_TYPE_ENUM)
	TOSTRING_SUPPORT(IGSELECTTEXTURE_TYPE_ENUM)
	TOSTRING_SUPPORT(IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM)
END_TOSTRING_MAP()

RECT IGConvertible::FromListPtsToRECT (const std::list <POINT>& lPts, int nWidth, int nHeight)
{
	RECT rcSel = {nWidth - 1, nHeight - 1, 0, 0};
	if (!lPts.empty())
	{
		// Conversion from list pts to RECT
		std::list<POINT>::const_iterator it = lPts.begin();
		do {
			if ((*it).x < rcSel.left)
				rcSel.left = (*it).x;
			if ((*it).x > rcSel.right)
				rcSel.right = (*it).x;
			if ((*it).y < rcSel.top)
				rcSel.top = (*it).y;
			if ((*it).y > rcSel.bottom)
				rcSel.bottom = (*it).y;
		}while (++it != lPts.end());
	}
	if ((rcSel.left > rcSel.right) || (rcSel.top > rcSel.bottom)){
		rcSel.left = 0; rcSel.top = 0; rcSel.bottom = nHeight - 1; rcSel.right = nWidth - 1;
	}
	return rcSel;
}

void IGConvertible::FromIGtoCxCoords(std::list<POINT>& lPts, int nHeight)
{
	std::list<POINT>::iterator itCurrent = lPts.begin();
	while (itCurrent != lPts.end()){
		FromIGtoCxCoords (*itCurrent, nHeight);
		++itCurrent;
	}
}

void IGConvertible::FromIGtoCxCoords(POINT& pt, int nHeight)
{
	pt.y = nHeight - 1 - pt.y;
}

void IGConvertible::FromIGtoCxCoords(double dX, double dY, int nHeight, double& dtX, double& dtY)
{
	dtX = dX;
	dtY = nHeight - 1 - dY;
}

void IGConvertible::FromCxtoIGCoords(double dX, double dY, int nHeight, double& dtX, double& dtY)
{
	FromIGtoCxCoords(dX, dY, nHeight, dtX, dtY);
}

void IGConvertible::FromDZtoIGRatios (int nWidth, int nHeight, double& dRatioX, double& dRatioY)
{
	dRatioX = (double)nWidth / PIXEL_SELECTOR_PRECISION;
	dRatioY = (double)nHeight / PIXEL_SELECTOR_PRECISION;
	double dRatioWidthHeight = (double)nWidth / (double)nHeight;
	if (dRatioWidthHeight > 1.0)
		dRatioY *= dRatioWidthHeight;
	else
		dRatioX /= dRatioWidthHeight;
}

void IGConvertible::FromIGtoDZRatios (int nWidth, int nHeight, double& dRatioX, double& dRatioY)
{
	FromDZtoIGRatios (nWidth, nHeight, dRatioX, dRatioY);
	dRatioX = 1.0 / dRatioX;
	dRatioY = 1.0 / dRatioY;
}