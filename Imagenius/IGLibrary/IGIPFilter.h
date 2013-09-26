#pragma once

#include "IGImageProcessing.h"
#include "IGImageProcMessage.h"
#include <list>

#define IGIPFILTER_PARAM_STRENGTH L"Strength"
#define IGIPFILTER_PARAM_POSX L"PosX"
#define IGIPFILTER_PARAM_POSY L"PosY"
#define IGIPFILTER_PARAM_DIRECTIONX L"DirectionX"
#define IGIPFILTER_PARAM_DIRECTIONY L"DirectionY"
#define IGIPFILTER_PARAM_TOLERANCE L"Tolerance"
#define IGIPFILTER_PARAM_TYPE L"Type"

namespace IGLibrary
{
typedef enum {IGIPFILTER_BLUR	= 0,
IGIPFILTER_GRADIENT	=	1,
IGIPFILTER_GRADIENT_MORPHO	=	2,
IGIPFILTER_GRAY	=	3,
IGIPFILTER_SKIN	=	4,
IGIPFILTER_SKIN_UNNOISED	=	5,
IGIPFILTER_EYES	=	6,
IGIPFILTER_FFT	=	7,
IGIPFILTER_FACE_EFFECT	=	8,
IGIPFILTER_COLORIZE = 9,
IGIPFILTER_BRIGHTNESS = 10,
IGIPFILTER_CONTRAST = 11,
IGIPFILTER_MORPHING = 12,
IGIPFILTER_NEGATIVE = 13,
IGIPFILTER_TRANSPARENCY = 14,
IGIPFILTER_PAINT = 15,
IGIPFILTER_PAINTGRADIENT = 16,
IGIPFILTER_REPAIR = 17,
IGIPFILTER_EYES_COLOR	=	18,
IGIPFILTER_SATURATE = 19,
IGIPFILTER_SHARPEN = 20,
IGIPFILTER_MATIFY = 21,
IGIPFILTER_THRESHOLD = 22,
IGIPFILTER_AUTOROTATE = 23,
IGIPFILTER_SKETCH = 24,
IGIPFILTER_CARTOON = 25,
IGIPFILTER_SEPIA = 26,
IGIPFILTER_VINTAGE = 27,
IGIPFILTER_OILPAINTING = 28,
IGIPFILTER_WATERPAINTING = 29,
IGIPFILTER_DITHER = 30,
IGIPFILTER_CLAY = 31,
IGIPFILTER_FILTER1 = 32,
IGIPFILTER_FILTER2 = 33,
IGIPFILTER_FILTER3 = 34, 
IGIPFILTER_FILTER4 = 35,
IGIPFILTER_FILTER5 = 36,

IGIPINDEX_LPE = 100,
IGIPINDEX_FACE = 101,
IGIPINDEX_IRIS = 102}	IGIPFILTER; // added by TQ

typedef IGIPFILTER	IGIPINDEX;

#define IGIPFACE_EFFECT_MATIFY_COLORCODE 1986710

class IGBrush;

class IGIPFilterMessage : public IGImageProcMessage
{
friend class IGIPFilter;
public:
IGIPFilterMessage (IGIPFILTER eFilterType)
: IGImageProcMessage (IGREQUEST_FRAME_FILTER)
, m_eFilterType (eFilterType)
{
::ZeroMemory (&m_rcSubLayer, sizeof (RECT));
m_rcSubLayer.right=-1;m_rcSubLayer.bottom=-1;
}

virtual ~IGIPFilterMessage()
{
}

static UINT ComputeNbProgressSteps (CxImage *pLayer)
{
return pLayer->GetHeight();
}

protected:
IGIPFilterMessage (IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcessingId) : IGImageProcMessage (nImageProcessingId)
{
}
virtual bool IsValid ()
{
return InternalIsValid <IGIPFilterMessage> ();
}
virtual UINT ComputeNbProgressSteps (const IGFrame &frame) const
{
return ComputeNbProgressSteps (frame.GetWorkingLayer());
}
IGIPFILTER	m_eFilterType;

private:
RECT	m_rcSubLayer;
};

class IGIPFaceEffectMessage : public IGIPFilterMessage
{
public:
IGIPFaceEffectMessage (IGIPFACE_EFFECT eFaceEffectType, COLORREF col1, COLORREF col2, double dParam1, double dParam2, double dParam3, CxImage *pImage1, CxImage *pImage2)
: IGIPFilterMessage (IGIPFILTER_FACE_EFFECT)
, m_eFaceEffectType (eFaceEffectType)
, m_col1 (col1)
, m_col2 (col2)
, m_dParam1 (dParam1)
, m_dParam2 (dParam2)
, m_dParam3 (dParam3)
, m_pImage1 (pImage1)
, m_pImage2 (pImage2)
{
}
IGIPFACE_EFFECT m_eFaceEffectType;
COLORREF m_col1;
COLORREF m_col2;
double m_dParam1;
double m_dParam2;
double m_dParam3;
CxImage *m_pImage1;
CxImage *m_pImage2;
};

class IGIPDrawMessage : public IGIPFilterMessage
{
friend class IGIPDraw;
public:
IGIPDrawMessage (const std::list<POINT>& lPts, IGBrush *pBrush)
: IGIPFilterMessage (IGREQUEST_FRAME_FILTER)
, m_lPts (lPts)
, m_pBrush (pBrush)
{
}

virtual ~IGIPDrawMessage();

static UINT ComputeNbProgressSteps (CxImage *pLayer)
{
return pLayer->GetHeight();
}

protected:
virtual bool IsValid ()
{
return InternalIsValid <IGIPFilterMessage> ();
}
virtual UINT ComputeNbProgressSteps (const IGFrame &frame) const
{
return ComputeNbProgressSteps (frame.GetWorkingLayer());
}

private:
std::list<POINT> m_lPts;
IGBrush	*m_pBrush;
};

class IGIPFilter : public IGImageProcessing
{
public:
IGIPFilter (IGFrame *pFrame, const wchar_t *pcwXml, HWND hProgress = NULL)
: IGImageProcessing (pFrame, pcwXml, hProgress)
{
}
IGIPFilter (IGFrame *pFrame, IGImageProcMessage *pMessage, HWND hProgress = NULL)
: IGImageProcessing (pFrame, pMessage, hProgress)
{
}
virtual ~IGIPFilter()
{
}

protected:
// Image processing thread callback
virtual bool OnImageProcessing (CxImage& image, IGImageProcMessage& message);
virtual int GetFirstParam (IGImageProcMessage& message, CxImage& image) const;
virtual int GetSecondParam (IGImageProcMessage& message, CxImage& image) const;
};

class IGIPIndexMessage : public IGIPFilterMessage
{
friend class IGIPIndex;
public:
IGIPIndexMessage (IGIPFILTER eFilterType, IGMarker *pMarkerObject, IGMarker *pMarkerBackground)
: IGIPFilterMessage (eFilterType)
, m_pMarkerObject (pMarkerObject)
, m_pMarkerBackground (pMarkerBackground)
{
}

virtual ~IGIPIndexMessage()
{
if (m_pMarkerObject)
delete m_pMarkerObject;
if (m_pMarkerBackground)
delete m_pMarkerBackground;
}

protected:
IGIPIndexMessage (IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcessingId) : IGIPFilterMessage (nImageProcessingId)
{
}
virtual bool IsValid ()
{
return InternalIsValid <IGIPIndexMessage> ();
}

private:
IGMarker *m_pMarkerObject;
IGMarker *m_pMarkerBackground;
};

class IGIPIndex : public IGIPFilter
{
public:
IGIPIndex (IGFrame *pFrame, const wchar_t *pcwXml, HWND hProgress = NULL)
: IGIPFilter (pFrame, pcwXml, hProgress)
{
}
IGIPIndex (IGFrame *pFrame, IGImageProcMessage *pMessage, HWND hProgress = NULL)
: IGIPFilter (pFrame, pMessage, hProgress)
{
}
virtual ~IGIPIndex()
{
}

protected:
// Image processing thread callback
virtual bool OnImageProcessing (CxImage& image, IGImageProcMessage& message);
};

class IGIPDraw : public IGIPFilter
{
public:
IGIPDraw (IGFrame *pFrame, const wchar_t *pcwXml, HWND hProgress = NULL)
: IGIPFilter (pFrame, pcwXml, hProgress)
{
}
IGIPDraw (IGFrame *pFrame, IGImageProcMessage *pMessage, HWND hProgress = NULL)
: IGIPFilter (pFrame, pMessage, hProgress)
{
}
virtual ~IGIPDraw()
{
}

protected:
// Image processing thread callback
virtual bool OnImageProcessing (CxImage& image, IGImageProcMessage& message);
// specialize the layer updating box
virtual int GetSecondParam (IGImageProcMessage& message, CxImage& image) const;
};

}