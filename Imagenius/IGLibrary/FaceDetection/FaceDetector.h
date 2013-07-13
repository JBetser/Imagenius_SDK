#ifndef FaceDetector_h
#define FaceDetector_h

#include "IGConvertible.h"
#include <list>
#include <iostream>

class CxImage;

struct POINTD
{
    double  x;
    double  y;
};

struct RECTD {
	double    left;
    double    top;
    double    right;
    double    bottom;
	RECTD(){
		left = 0.0;
		right = -1.0;
		top = 0.0;
		bottom = -1.0;
	}
	RECTD (const RECT& rect){
		*this = rect;
	}
	RECTD& operator =(const RECT& rect){
		left = rect.left;
		right = rect.right;
		top = rect.top;
		bottom = rect.bottom;
		return *this;
	}
	operator RECT() const {		
		RECT rc;
		rc.left = (LONG)left;
		rc.right = (LONG)right;
		rc.top = (LONG)top;
		rc.bottom = (LONG)bottom;
		return rc;
	}
};

class Face {
public:
	Face (){
		m_rcFace.left = -1; m_rcFace.top = -1; m_rcFace.right = -1; m_rcFace.bottom = -1; 
		m_pairEyes.first.left = -1; m_pairEyes.first.top = -1; m_pairEyes.first.right = -1; m_pairEyes.first.bottom = -1; 
		m_pairEyes.second.left = -1; m_pairEyes.second.top = -1; m_pairEyes.second.right = -1; m_pairEyes.second.bottom = -1; 
		m_rcMouth.left = -1; m_rcMouth.top = -1; m_rcMouth.right = -1; m_rcMouth.bottom = -1; 
		m_rcNoze.left = -1; m_rcNoze.top = -1; m_rcNoze.right = -1; m_rcNoze.bottom = -1; 
		m_nRot = 0;
		m_nScore = 0;
	}

	Face (int nScaledWidth, int nScaledHeight, int nLayerWidth, int nLayerHeight, const std::wstring& wsDescriptor)
	{
		std::wstring wsCurDescriptor (wsDescriptor);
		int nIdx = wsDescriptor.find (L"&");
		m_nRot = ::_wtoi (wsDescriptor.substr (0, nIdx).c_str());
		wsCurDescriptor = wsCurDescriptor.substr (nIdx + 1);
		double fScaleWidth = (double)nScaledWidth / (double)nLayerWidth;
		double fScaleHeight = (double)nScaledHeight / (double)nLayerHeight;
		double fPtCtrX = (double)nLayerWidth / 2.0f;
		double fPtCtrY = (double)nLayerHeight / 2.0f;
		ConvertDescriptorTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, -m_nRot, wsCurDescriptor, m_rcFace);
		ConvertDescriptorTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, -m_nRot, wsCurDescriptor, m_pairEyes.first);
		ConvertDescriptorTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, -m_nRot, wsCurDescriptor, m_pairEyes.second);
		ConvertDescriptorTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, -m_nRot, wsCurDescriptor, m_rcMouth);
		ConvertDescriptorTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, -m_nRot, wsCurDescriptor, m_rcNoze);
	}

	Face (RECT rcFace, std::pair <RECT, RECT> rcEyes, RECT rcMouth, RECT rcNoze, int nRot){
		m_rcFace = rcFace;
		m_pairEyes = rcEyes;
		m_rcMouth = rcMouth;
		m_rcNoze = rcNoze;
		m_nRot = nRot;
		m_nScore = 0;
	}

	int CalculateScore()
	{
		m_nScore = 100;
		if (m_pairEyes.first.left == -1)
			m_nScore -= 37;
		if (m_pairEyes.second.left == -1)
			m_nScore -= 37;
		if (m_rcMouth.left == -1)
			m_nScore -= 18;
		if (m_rcNoze.left == -1)
			m_nScore -= 7;
		return m_nScore;
	}
	
	void Rotate (int nPtCtrX, int nPtCtrY, int nRotate)
	{
		m_rcFace = Rotate2Points (nPtCtrX, nPtCtrY, m_rcFace, nRotate);
		m_pairEyes.first = Rotate2Points (nPtCtrX, nPtCtrY, m_pairEyes.first, nRotate);
		m_pairEyes.second = Rotate2Points (nPtCtrX, nPtCtrY, m_pairEyes.second, nRotate);
		m_rcMouth = Rotate2Points (nPtCtrX, nPtCtrY, m_rcMouth, nRotate);
		m_rcNoze = Rotate2Points (nPtCtrX, nPtCtrY, m_rcNoze, nRotate);
	}

	void Scale (int nScaledWidth, int nScaledHeight, int nLayerWidth, int nLayerHeight)
	{
		double fScaleWidth = (double)nLayerWidth / (double)nScaledWidth; 
		double fScaleHeight = (double)nLayerHeight / (double)nScaledHeight; 
		m_rcFace = ScaleRect (fScaleWidth, fScaleHeight, m_rcFace);
		m_pairEyes.first = ScaleRect (fScaleWidth, fScaleHeight, m_pairEyes.first);
		m_pairEyes.second = ScaleRect (fScaleWidth, fScaleHeight, m_pairEyes.second);
		m_rcMouth = ScaleRect (fScaleWidth, fScaleHeight, m_rcMouth);
		m_rcNoze = ScaleRect (fScaleWidth, fScaleHeight, m_rcNoze);
	}

	void Offset (int nX, int nY)
	{
		m_rcFace = OffsetRect (nX, nY, m_rcFace);
		m_pairEyes.first = OffsetRect (nX, nY, m_pairEyes.first);
		m_pairEyes.second = OffsetRect (nX, nY, m_pairEyes.second);
		m_rcMouth = OffsetRect (nX, nY, m_rcMouth);
		m_rcNoze = OffsetRect (nX, nY, m_rcNoze);
	}

	std::wstring ToString (int nScaledWidth, int nScaledHeight, int nLayerWidth, int nLayerHeight) const
	{
		double fPtCtrX = (double)nLayerWidth / 2.0;
		double fPtCtrY = (double)nLayerHeight / 2.0;
		double fScaleWidth = (double)nLayerWidth / (double)nScaledWidth;
		double fScaleHeight = (double)nLayerHeight / (double)nScaledHeight;
		std::wstring wsDescriptor;
		wchar_t wInt [32];
		if (::_itow_s (m_nRot, wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (!ConvertRectTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, m_rcFace, m_nRot, wsDescriptor))
			return L"";
		wsDescriptor += L"&";
		if (!ConvertRectTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, m_pairEyes.first, m_nRot, wsDescriptor))
			return L"";
		wsDescriptor += L"&";
		if (!ConvertRectTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, m_pairEyes.second, m_nRot, wsDescriptor))
			return L"";
		wsDescriptor += L"&";
		if (!ConvertRectTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, m_rcMouth, m_nRot, wsDescriptor))
			return L"";
		wsDescriptor += L"&";
		if (!ConvertRectTo (nLayerWidth, nLayerHeight, fScaleWidth, fScaleHeight, fPtCtrX, fPtCtrY, m_rcNoze, m_nRot, wsDescriptor))
			return L"";	

		std::wstring wsCurDescriptor;
		if (::_itow_s (m_nRot, wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		RECTD  rcToConv = m_rcFace;
		int nRotate = m_nRot;
		double ptPosTopLeftX = rcToConv.left; 
		double ptPosTopRightX = rcToConv.right;
		double ptPosTopLeftY = rcToConv.top; 
		double ptPosTopRightY = rcToConv.top;
		double ptPosBottomLeftX = rcToConv.left; 
		double ptPosBottomRightX = rcToConv.right;
		double ptPosBottomLeftY = rcToConv.bottom; 
		double ptPosBottomRightY = rcToConv.bottom;

		if (rcToConv.left != -1 && rcToConv.right != -1 && rcToConv.top != -1 && rcToConv.bottom != -1){
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopLeftX, ptPosTopLeftY, ptPosTopLeftX, ptPosTopLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopRightX, ptPosTopRightY, ptPosTopRightX, ptPosTopRightY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomLeftX, ptPosBottomLeftY, ptPosBottomLeftX, ptPosBottomLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomRightX, ptPosBottomRightY, ptPosBottomRightX, ptPosBottomRightY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosTopLeftX, ptPosTopLeftY, nRotate, ptPosTopLeftX, ptPosTopLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosTopRightX, ptPosTopRightY, nRotate, ptPosTopRightX, ptPosTopRightY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomLeftX, ptPosBottomLeftY, nRotate, ptPosBottomLeftX, ptPosBottomLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomRightX, ptPosBottomRightY, nRotate, ptPosBottomRightX, ptPosBottomRightY);	

			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosTopLeftX, ptPosTopLeftY, nLayerHeight, ptPosTopLeftX, ptPosTopLeftY);
			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosTopRightX, ptPosTopRightY, nLayerHeight, ptPosTopRightX, ptPosTopRightY);
			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosBottomLeftX, ptPosBottomLeftY, nLayerHeight, ptPosBottomLeftX, ptPosBottomLeftY);
			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosBottomRightX, ptPosBottomRightY, nLayerHeight, ptPosBottomRightX, ptPosBottomRightY);

			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGLibrary::IGConvertible::FromIGtoDZRatios (nLayerWidth, nLayerHeight, dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			ptPosTopLeftX *= dSeadragonToPixelRatioX; 
			ptPosTopRightX *= dSeadragonToPixelRatioX;
			ptPosTopLeftY *= dSeadragonToPixelRatioY; 
			ptPosTopRightY *= dSeadragonToPixelRatioY;
			ptPosBottomLeftX *= dSeadragonToPixelRatioX; 
			ptPosBottomRightX *= dSeadragonToPixelRatioX;
			ptPosBottomLeftY *= dSeadragonToPixelRatioY; 
			ptPosBottomRightY *= dSeadragonToPixelRatioY;	
		}

		if (::_itow_s ((int)round (ptPosTopLeftX), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosTopRightX), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosTopLeftY), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosTopRightY), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomLeftX), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomRightX), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomLeftY), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt; wsCurDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomRightY), wInt, 10) != 0)
			return false;
		wsCurDescriptor += wInt;
		
		int nIdx = wsCurDescriptor.find (L"&");
		nRotate = ::_wtoi (wsCurDescriptor.substr (0, nIdx).c_str());
		wsCurDescriptor = wsCurDescriptor.substr (nIdx + 1);
		fScaleWidth = (double)nScaledWidth / (double)nLayerWidth;
		fScaleHeight = (double)nScaledHeight / (double)nLayerHeight;
		fPtCtrX = (double)nLayerWidth / 2.0f;
		fPtCtrY = (double)nLayerHeight / 2.0f;
		POINTD ptPosTopLeft;
		POINTD ptPosTopRight;
		POINTD ptPosBottomLeft; 
		POINTD ptPosBottomRight;		
		int nPrevIdx = 0;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosTopLeft.x = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosTopRight.x = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosTopLeft.y = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosTopRight.y = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosBottomLeft.x = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosBottomRight.x = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		ptPosBottomLeft.y = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsCurDescriptor.find (L"&", nPrevIdx);
		if (nIdx == -1)
			ptPosBottomRight.y = ::_wtoi (wsCurDescriptor.substr (nPrevIdx).c_str());
		else{
			ptPosBottomRight.y = ::_wtoi (wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
			wsCurDescriptor = wsCurDescriptor.substr (nIdx + 1);
		}
		
		RECTD rcConv;
		if (ptPosTopLeft.x != -1 && ptPosTopLeft.y != -1 && ptPosTopLeft.x != -1 && ptPosTopLeft.y != -1 &&
			ptPosBottomLeft.x != -1 && ptPosBottomLeft.y != -1 && ptPosBottomRight.x != -1 && ptPosBottomRight.y != -1){
			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGLibrary::IGConvertible::FromDZtoIGRatios (nLayerWidth, nLayerHeight, dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			double ptPosTopLeftX = ptPosTopLeft.x * dSeadragonToPixelRatioX; 
			double ptPosTopRightX = ptPosTopRight.x * dSeadragonToPixelRatioX;
			double ptPosTopLeftY = ptPosTopLeft.y * dSeadragonToPixelRatioY; 
			double ptPosTopRightY = ptPosTopRight.y * dSeadragonToPixelRatioY;
			double ptPosBottomLeftX = ptPosBottomLeft.x * dSeadragonToPixelRatioX; 
			double ptPosBottomRightX = ptPosBottomRight.x * dSeadragonToPixelRatioX;
			double ptPosBottomLeftY = ptPosBottomLeft.y * dSeadragonToPixelRatioY; 
			double ptPosBottomRightY = ptPosBottomRight.y * dSeadragonToPixelRatioY;
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosTopLeftX, ptPosTopLeftY, nLayerHeight, ptPosTopLeftX, ptPosTopLeftY);
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosTopRightX, ptPosTopRightY, nLayerHeight, ptPosTopRightX, ptPosTopRightY);
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosBottomLeftX, ptPosBottomLeftY, nLayerHeight, ptPosBottomLeftX, ptPosBottomLeftY);
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosBottomRightX, ptPosBottomRightY, nLayerHeight, ptPosBottomRightX, ptPosBottomRightY);

			RotatePt (fPtCtrX, fPtCtrY, ptPosTopLeftX, ptPosTopLeftY, -nRotate, ptPosTopLeftX, ptPosTopLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosTopRightX, ptPosTopRightY, -nRotate, ptPosTopRightX, ptPosTopRightY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomLeftX, ptPosBottomLeftY, -nRotate, ptPosBottomLeftX, ptPosBottomLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomRightX, ptPosBottomRightY, -nRotate, ptPosBottomRightX, ptPosBottomRightY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopLeftX, ptPosTopLeftY, ptPosTopLeftX, ptPosTopLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopRightX, ptPosTopRightY, ptPosTopRightX, ptPosTopRightY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomLeftX, ptPosBottomLeftY, ptPosBottomLeftX, ptPosBottomLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomRightX, ptPosBottomRightY, ptPosBottomRightX, ptPosBottomRightY);

			rcConv.top = ptPosTopLeftY;
			rcConv.left = ptPosTopLeftX;
			rcConv.bottom = ptPosBottomRightY;
			rcConv.right = ptPosBottomRightX;
		}	

		return wsDescriptor;
	}

	bool ConvertRectTo(int nLayerWidth, int nLayerHeight, double fScaleWidth, double fScaleHeight, double fPtCtrX, double fPtCtrY, RECTD rcToConv, int nRotate, std::wstring& wsDescriptor) const{
		double ptPosTopLeftX = rcToConv.left; 
		double ptPosTopRightX = rcToConv.right;
		double ptPosTopLeftY = rcToConv.top; 
		double ptPosTopRightY = rcToConv.top;
		double ptPosBottomLeftX = rcToConv.left; 
		double ptPosBottomRightX = rcToConv.right;
		double ptPosBottomLeftY = rcToConv.bottom; 
		double ptPosBottomRightY = rcToConv.bottom;

		if (rcToConv.left != -1 && rcToConv.right != -1 && rcToConv.top != -1 && rcToConv.bottom != -1){
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopLeftX, ptPosTopLeftY, ptPosTopLeftX, ptPosTopLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopRightX, ptPosTopRightY, ptPosTopRightX, ptPosTopRightY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomLeftX, ptPosBottomLeftY, ptPosBottomLeftX, ptPosBottomLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomRightX, ptPosBottomRightY, ptPosBottomRightX, ptPosBottomRightY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosTopLeftX, ptPosTopLeftY, nRotate, ptPosTopLeftX, ptPosTopLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosTopRightX, ptPosTopRightY, nRotate, ptPosTopRightX, ptPosTopRightY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomLeftX, ptPosBottomLeftY, nRotate, ptPosBottomLeftX, ptPosBottomLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomRightX, ptPosBottomRightY, nRotate, ptPosBottomRightX, ptPosBottomRightY);	

			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosTopLeftX, ptPosTopLeftY, nLayerHeight, ptPosTopLeftX, ptPosTopLeftY);
			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosTopRightX, ptPosTopRightY, nLayerHeight, ptPosTopRightX, ptPosTopRightY);
			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosBottomLeftX, ptPosBottomLeftY, nLayerHeight, ptPosBottomLeftX, ptPosBottomLeftY);
			IGLibrary::IGConvertible::FromCxtoIGCoords (ptPosBottomRightX, ptPosBottomRightY, nLayerHeight, ptPosBottomRightX, ptPosBottomRightY);

			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGLibrary::IGConvertible::FromIGtoDZRatios (nLayerWidth, nLayerHeight, dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			ptPosTopLeftX *= dSeadragonToPixelRatioX; 
			ptPosTopRightX *= dSeadragonToPixelRatioX;
			ptPosTopLeftY *= dSeadragonToPixelRatioY; 
			ptPosTopRightY *= dSeadragonToPixelRatioY;
			ptPosBottomLeftX *= dSeadragonToPixelRatioX; 
			ptPosBottomRightX *= dSeadragonToPixelRatioX;
			ptPosBottomLeftY *= dSeadragonToPixelRatioY; 
			ptPosBottomRightY *= dSeadragonToPixelRatioY;	
		}

		wchar_t wInt [32];
		if (::_itow_s ((int)round (ptPosTopLeftX), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosTopRightX), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosTopLeftY), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosTopRightY), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomLeftX), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomRightX), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomLeftY), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s ((int)round (ptPosBottomRightY), wInt, 10) != 0)
			return false;
		wsDescriptor += wInt;
		return true;
	}

	void ConvertDescriptorTo(int nLayerWidth, int nLayerHeight, double fScaleWidth, double fScaleHeight, double fPtCtrX, double fPtCtrY, int nRotate, std::wstring& wsDescriptor, RECTD& rcConv) const{
		POINTD ptPosTopLeft;
		POINTD ptPosTopRight;
		POINTD ptPosBottomLeft; 
		POINTD ptPosBottomRight;		
		int nPrevIdx = 0;
		int nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosTopLeft.x = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosTopRight.x = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosTopLeft.y = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosTopRight.y = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosBottomLeft.x = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosBottomRight.x = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		ptPosBottomLeft.y = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
		nPrevIdx = nIdx + 1;
		nIdx = wsDescriptor.find (L"&", nPrevIdx);
		if (nIdx == -1)
			ptPosBottomRight.y = ::_wtoi (wsDescriptor.substr (nPrevIdx).c_str());
		else{
			ptPosBottomRight.y = ::_wtoi (wsDescriptor.substr (nPrevIdx, nIdx - nPrevIdx).c_str());
			wsDescriptor = wsDescriptor.substr (nIdx + 1);
		}
		
		if (ptPosTopLeft.x != -1 && ptPosTopLeft.y != -1 && ptPosTopLeft.x != -1 && ptPosTopLeft.y != -1 &&
			ptPosBottomLeft.x != -1 && ptPosBottomLeft.y != -1 && ptPosBottomRight.x != -1 && ptPosBottomRight.y != -1){
			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGLibrary::IGConvertible::FromDZtoIGRatios (nLayerWidth, nLayerHeight, dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			double ptPosTopLeftX = ptPosTopLeft.x * dSeadragonToPixelRatioX; 
			double ptPosTopRightX = ptPosTopRight.x * dSeadragonToPixelRatioX;
			double ptPosTopLeftY = ptPosTopLeft.y * dSeadragonToPixelRatioY; 
			double ptPosTopRightY = ptPosTopRight.y * dSeadragonToPixelRatioY;
			double ptPosBottomLeftX = ptPosBottomLeft.x * dSeadragonToPixelRatioX; 
			double ptPosBottomRightX = ptPosBottomRight.x * dSeadragonToPixelRatioX;
			double ptPosBottomLeftY = ptPosBottomLeft.y * dSeadragonToPixelRatioY; 
			double ptPosBottomRightY = ptPosBottomRight.y * dSeadragonToPixelRatioY;
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosTopLeftX, ptPosTopLeftY, nLayerHeight, ptPosTopLeftX, ptPosTopLeftY);
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosTopRightX, ptPosTopRightY, nLayerHeight, ptPosTopRightX, ptPosTopRightY);
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosBottomLeftX, ptPosBottomLeftY, nLayerHeight, ptPosBottomLeftX, ptPosBottomLeftY);
			IGLibrary::IGConvertible::FromIGtoCxCoords (ptPosBottomRightX, ptPosBottomRightY, nLayerHeight, ptPosBottomRightX, ptPosBottomRightY);

			RotatePt (fPtCtrX, fPtCtrY, ptPosTopLeftX, ptPosTopLeftY, nRotate, ptPosTopLeftX, ptPosTopLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosTopRightX, ptPosTopRightY, nRotate, ptPosTopRightX, ptPosTopRightY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomLeftX, ptPosBottomLeftY, nRotate, ptPosBottomLeftX, ptPosBottomLeftY);
			RotatePt (fPtCtrX, fPtCtrY, ptPosBottomRightX, ptPosBottomRightY, nRotate, ptPosBottomRightX, ptPosBottomRightY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopLeftX, ptPosTopLeftY, ptPosTopLeftX, ptPosTopLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosTopRightX, ptPosTopRightY, ptPosTopRightX, ptPosTopRightY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomLeftX, ptPosBottomLeftY, ptPosBottomLeftX, ptPosBottomLeftY);
			ScalePt (fScaleWidth, fScaleHeight, ptPosBottomRightX, ptPosBottomRightY, ptPosBottomRightX, ptPosBottomRightY);

			rcConv.top = ptPosTopLeftY;
			rcConv.left = ptPosTopLeftX;
			rcConv.bottom = ptPosBottomRightY;
			rcConv.right = ptPosBottomRightX;
		}		
	}

	void FlipY (int nHeight)
	{
		double dTop = m_rcFace.top;
		m_rcFace.top = nHeight - m_rcFace.bottom;
		m_rcFace.bottom = nHeight - dTop;
		dTop = m_pairEyes.first.top;
		m_pairEyes.first.top = nHeight - m_pairEyes.first.bottom;
		m_pairEyes.first.bottom = nHeight - dTop;
		dTop = m_pairEyes.second.top;
		m_pairEyes.second.top = nHeight - m_pairEyes.second.bottom;
		m_pairEyes.second.bottom = nHeight - dTop;
		dTop = m_rcMouth.top;
		m_rcMouth.top = nHeight - m_rcMouth.bottom;
		m_rcMouth.bottom = nHeight - dTop;
		dTop = m_rcNoze.top;
		m_rcNoze.top = nHeight - m_rcNoze.bottom;
		m_rcNoze.bottom = nHeight - dTop;
	}

	static void RotatePt (double dPtCtrX, double dPtCtrY, double dX, double dY, int nRotate, double& drX, double& drY)
	{
		// change of vectorial space, set the origin to the center
		dX = dX - dPtCtrX;
		dY = dY - dPtCtrY;
		// rotate
		double dXTr = cos((double)nRotate * PI / 180.0f);
		double dYTr = sin((double)nRotate * PI / 180.0f);
		drX = dX * dXTr + dY * dYTr;
		drY = -dX * dYTr + dY * dXTr;
		// change of vectorial space, back to the origin
		drX = drX + dPtCtrX;
		drY = drY + dPtCtrY;
	}

	static void ScalePt (double fScaleWidth, double fScaleHeight, double dX, double dY, double& drX, double& drY)
	{
		drX = dX * fScaleWidth;
		drY = dY * fScaleHeight;
	}

	static RECTD Rotate2Points (int nPtCtrX, int nPtCtrY, RECTD rcToRotate, int nRotate)
	{
		// change of vectorial space, set the origin to the center
		double nTopLeftX = rcToRotate.left - nPtCtrX;
		double nTopLeftY = rcToRotate.top - nPtCtrY;
		double nBottomRightX = rcToRotate.right - nPtCtrX;
		double nBottomRightY = rcToRotate.bottom - nPtCtrY;
		// rotate
		double fXTr = cos((double)nRotate * PI / 180.0);
		double fYTr = sin((double)nRotate * PI / 180.0);
		RECTD rcRot;
		rcRot.left = nTopLeftX * fXTr - nTopLeftY * fYTr;
		rcRot.top = nTopLeftX * fYTr + nTopLeftY * fXTr;
		rcRot.right = nBottomRightX * fXTr - nBottomRightY * fYTr;
		rcRot.bottom = nBottomRightX * fYTr + nBottomRightY * fXTr;
		// change of vectorial space, back to the origin
		rcRot.left = rcRot.left + nPtCtrX;
		rcRot.top = rcRot.top + nPtCtrY;
		rcRot.right = rcRot.right + nPtCtrX;
		rcRot.bottom = rcRot.bottom + nPtCtrY;
		return rcRot;
	}

	static RECTD RotateRect (int nPtCtrX, int nPtCtrY, RECTD rcToRotate, int nRotate)
	{
		double dBottom = rcToRotate.bottom;
		rcToRotate.bottom = rcToRotate.top;
		RECTD rcRotTopLeftTopRightCmpIntersect = Face::Rotate2Points (nPtCtrX, nPtCtrY, rcToRotate, nRotate);
		rcToRotate.top = dBottom;
		rcToRotate.bottom = dBottom;
		RECTD rcRotBottomLeftBottomRightCmpIntersect = Face::Rotate2Points (nPtCtrX, nPtCtrY, rcToRotate, nRotate);		
		RECTD rcRot;
		rcRot.left = min (min (rcRotTopLeftTopRightCmpIntersect.left, rcRotBottomLeftBottomRightCmpIntersect.left),
						min (rcRotTopLeftTopRightCmpIntersect.right, rcRotBottomLeftBottomRightCmpIntersect.right));
		rcRot.right = max (max (rcRotTopLeftTopRightCmpIntersect.left, rcRotBottomLeftBottomRightCmpIntersect.left),
						max (rcRotTopLeftTopRightCmpIntersect.right, rcRotBottomLeftBottomRightCmpIntersect.right));
		rcRot.top = min (min (rcRotTopLeftTopRightCmpIntersect.top, rcRotBottomLeftBottomRightCmpIntersect.top),
						min (rcRotTopLeftTopRightCmpIntersect.bottom, rcRotBottomLeftBottomRightCmpIntersect.bottom));
		rcRot.bottom = max (max (rcRotTopLeftTopRightCmpIntersect.top, rcRotBottomLeftBottomRightCmpIntersect.top),
						max (rcRotTopLeftTopRightCmpIntersect.bottom, rcRotBottomLeftBottomRightCmpIntersect.bottom));
		return rcRot;
	}

	static RECTD ScaleRect (double fScaleWidth, double fScaleHeight, const RECTD& rcToScale)
	{
		RECTD rcScale = rcToScale;
		rcScale.left = rcScale.left * fScaleWidth;
		rcScale.top = rcScale.top * fScaleHeight;
		rcScale.right = rcScale.right * fScaleWidth;
		rcScale.bottom = rcScale.bottom * fScaleHeight;
		return rcScale;
	}

	static RECTD OffsetRect (int nX, int nY, const RECTD& rcToOffset)
	{
		RECTD rcOffset = rcToOffset;
		rcOffset.left += nX;
		rcOffset.top += nY;
		rcOffset.right += nX;
		rcOffset.bottom += nY;
		return rcOffset;
	}

	void operator >> (std::ostream& st)
	{
		writeRect (st, m_rcFace);
		writeRect (st, m_pairEyes.first);
		writeRect (st, m_pairEyes.second);
		writeRect (st, m_rcMouth);
		writeRect (st, m_rcNoze);
		st << m_nRot;
		st << ";";
		st << m_nScore;
	}

	void operator << (std::istream& st)
	{
		readRect (st, m_rcFace);
		readRect (st, m_pairEyes.first);
		readRect (st, m_pairEyes.second);
		readRect (st, m_rcMouth);
		readRect (st, m_rcNoze);
		st >> m_nRot;
		char sep;
		st >> sep;
		st >> m_nScore;
	}

	bool operator == (Face face)
	{
		return (m_rcFace.left == face.m_rcFace.left &&
			m_rcFace.right == face.m_rcFace.right &&
			m_rcFace.top == face.m_rcFace.top &&
			m_rcFace.bottom == face.m_rcFace.bottom);
	}

	bool operator < (const Face& face) const {
		return getScore() < face.getScore();
	}

	RECT getFaceCoords() const {
		return m_rcFace;
	}

	void setFaceCoords(const RECT& rc){
		m_rcFace = rc;
	}

	RECT getEyeLeftCoords() const {
		return m_pairEyes.first;
	}

	void setEyeLeftCoords(const RECT& rc){
		m_pairEyes.first = rc;
	}

	RECT getEyeRightCoords() const {
		return m_pairEyes.second;
	}

	void setEyeRightCoords(const RECT& rc){
		m_pairEyes.second = rc;
	}

	RECT getMouthCoords() const {
		return m_rcMouth;
	}

	void setMouthCoords(const RECT& rc){
		m_rcMouth = rc;
	}

	RECT getNozeCoords() const {
		return m_rcNoze;
	}

	void setNozeCoords(const RECT& rc){
		m_rcNoze = rc;
	}

	inline int getRotation() const {
		return m_nRot;
	}

	inline void setRotation(int nRot) {
		m_nRot = nRot;
	}

	inline int getScore() const {
		return m_nScore;
	}
	RECT getDefaultMouth() const{
		RECT rcMouth = {-1,-1,-1,-1};
		if (m_pairEyes.first.left != -1 && m_pairEyes.second.left != -1){
			int nMidEyesY = (int)((m_pairEyes.first.top + m_pairEyes.first.bottom) / 2 + (m_pairEyes.second.top + m_pairEyes.second.bottom) / 2) / 2;
			int nMidEyesX = (int)(min (m_pairEyes.first.right, m_pairEyes.second.right) + max (m_pairEyes.first.left, m_pairEyes.second.left)) / 2;
			int nMidEyeSizeX = (int)(m_pairEyes.first.right - m_pairEyes.first.left + m_pairEyes.second.right - m_pairEyes.second.left) / 2;
			int nMidEyeSizeY = (int)(m_pairEyes.first.bottom - m_pairEyes.first.top + m_pairEyes.second.bottom - m_pairEyes.second.top) / 2;
			rcMouth.bottom = (int)(nMidEyesY - 2 * max (m_pairEyes.first.bottom - m_pairEyes.first.top, m_pairEyes.second.bottom - m_pairEyes.second.top));
			rcMouth.top = (int)(rcMouth.bottom - nMidEyeSizeY);
	
			int nMouthSizeX = (int)((4 * nMidEyeSizeX) / 3);
			int nMouthSizeY = rcMouth.bottom - rcMouth.top;
			rcMouth.left = nMidEyesX - nMouthSizeX / 2;
			rcMouth.right = nMidEyesX + nMouthSizeX / 2;			
			rcMouth.top = (int)(max ((double)rcMouth.top, m_rcFace.top + nMouthSizeY / 2));
			rcMouth.bottom = rcMouth.top + nMouthSizeY;
		}
		return rcMouth;
	}

	static void adjustRectFaces(std::list <Face>& lFaces);

private:
	void writeRect (std::ostream& st, const RECTD& rect)
	{
		st << (int)(round(rect.left));
		st << ";";
		st << (int)(round(rect.top));
		st << ";";
		st << (int)(round(rect.right));
		st << ";";
		st << (int)(round(rect.bottom));
		st << ";";
	}

	void writeRect (std::ostream& st, const RECT& rect)
	{
		st << rect.left;
		st << ";";
		st << rect.top;
		st << ";";
		st << rect.right;
		st << ";";
		st << rect.bottom;
		st << ";";
	}

	void readRect (std::istream& st, RECTD& rect)
	{
		char sep;
		st >> rect.left;
		st >> sep;
		st >> rect.top;
		st >> sep;
		st >> rect.right;
		st >> sep;
		st >> rect.bottom;
		st >> sep;
	}

	static double round(double number)
	{
		return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
	}

	RECTD m_rcFace;
	std::pair <RECTD, RECTD> m_pairEyes;
	RECTD m_rcMouth;
	RECTD m_rcNoze;
	int m_nRot;
	int m_nScore;
};

extern void adjustRectFaces(std::list <Face>& lFaces);
extern void detectFaces(const CxImage& pImg, std::list <Face>& lFaces, int nRot);

#endif FaceDetector_h

