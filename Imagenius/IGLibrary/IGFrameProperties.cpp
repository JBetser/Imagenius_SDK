#include "StdAfx.h"
#include "IGFrameProperties.h"
#include "IGSmartProcessing/IGFaceDescriptor.h"

using namespace IGLibrary;

LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_WIDTH	=			L"Width";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_HEIGHT	=			L"Height";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_UNIT		=			L"Unit";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_RESOLUTION=			L"Resolution";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_STRETCHING=			L"Stretching";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_SHOW_SEGMENTATION =	L"ShowSegment";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_SELECTIONRECT =		L"SelectionRect";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_HISTORYSTEP =		L"HistoryStep";
LPCWSTR IGFrameProperties::IGFRAMEPROPERTY_FACEDESCRIPTOR =		L"FaceDescriptor";

IGFrameProperties::IGFrameProperties(void)
{
	initialize();
}


IGFrameProperties::~IGFrameProperties(void)
{
}

void IGFrameProperties::initialize()
{
	SetProperty (IGFRAMEPROPERTY_WIDTH, 0L);
	SetProperty (IGFRAMEPROPERTY_HEIGHT, 0L);
	SetProperty (IGFRAMEPROPERTY_UNIT, IGFRAMEPROPERTY_UNIT_PIXEL);
	SetProperty (IGFRAMEPROPERTY_RESOLUTION, 1L);
	SetProperty (IGFRAMEPROPERTY_STRETCHING, IGFRAMEPROPERTY_STRETCHING_FAST);
	SetProperty (IGFRAMEPROPERTY_SHOW_SEGMENTATION, IGFRAMEPROPERTY_SHOWSEGMENTATION_TRANSPARENT);	
	wstring wsDefSelRect (L"0#-1#0#-1");
	SetProperty (IGFRAMEPROPERTY_SELECTIONRECT, wsDefSelRect);	
	SetProperty (IGFRAMEPROPERTY_HISTORYSTEP, 0L);	
}

bool IGFrameProperties::UpdateSize (int nCurrentWidth, int nCurrentHeight)
{
	int nPropWidth = 0;
	int nPropHeight = 0;
	int nPropResolution = 0;
	GetProperty (IGFRAMEPROPERTY_WIDTH, nPropWidth);
	GetProperty (IGFRAMEPROPERTY_HEIGHT, nPropHeight);
	GetProperty (IGFRAMEPROPERTY_RESOLUTION, nPropResolution);
	int nActualWidth = (int)(nPropWidth * nPropResolution);
	int nActualHeight = (int)(nPropHeight * nPropResolution);
	if ((nActualWidth != nCurrentWidth) || (nActualHeight != nCurrentHeight))
	{
		// an update is needed
		IGFRAMEPROPERTY_STRETCHING_ENUM eStretchMode = IGFRAMEPROPERTY_STRETCHING_NO;
		GetProperty (IGFRAMEPROPERTY_STRETCHING, eStretchMode);
		return OnFrameSizeChanged (nActualWidth, nActualHeight, eStretchMode);
	}
	return true;
}