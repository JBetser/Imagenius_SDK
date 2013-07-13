#pragma once
#include "IGColor.h"
#include "IGPropertyDefs.h"
#include <string>
#include <list>
#include <atlbase.h>
#include <atlcom.h>

#define PIXEL_SELECTOR_PRECISION	1000000.0

namespace IGLibrary
{
	class IGConvertible : public IUnknown
	{
	public:
		IGConvertible() : m_dwRef (1) {}
		virtual ~IGConvertible(){}
		virtual bool ConvertTo(int&) const {return false;}
		virtual bool ConvertTo(double&) const {return false;}
		virtual bool ConvertTo(std::wstring&) const {return false;}
		virtual bool ConvertTo(COLORREF&) const {return false;}
		virtual bool ConvertTo(IGColor&) const {return false;}
		virtual void CopyFrom (const IGConvertible& conv) {}
		virtual void CopyFrom (const int& conv) {}
		virtual ULONG STDMETHODCALLTYPE AddRef () {return ++m_dwRef;}
		virtual ULONG STDMETHODCALLTYPE Release () {
			if (--m_dwRef == 0)
			{
				delete this;
				return 0;
			}
			return m_dwRef;
		}
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,  __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
		{
			if (riid == IID_IUnknown)
			{
				*ppvObject = dynamic_cast <IUnknown*> (this);
				if (*ppvObject)
					return S_OK;
				return E_FAIL;
			}
			return E_NOTIMPL;
		}

		IGConvertible& operator = (const IGConvertible& conv)
		{
			CopyFrom (conv);
			return *this;
		}

		IGConvertible& operator = (const int& conv)
		{
			CopyFrom (conv);
			return *this;
		}

		static RECT FromListPtsToRECT (const std::list <POINT>& lPts, int nWidth, int nHeight);
		static void FromIGtoCxCoords(POINT& pt, int nHeight);
		static void FromIGtoCxCoords(std::list<POINT>& lPts, int nHeight);
		static void FromIGtoCxCoords(double dX, double dY, int nHeight, double& dtX, double& dtY);
		static void FromCxtoIGCoords(double dX, double dY, int nHeight, double& dtX, double& dtY);
		static void FromDZtoIGRatios (int nWidth, int nHeight, double& dRatioX, double& dRatioY);
		static void FromIGtoDZRatios (int nWidth, int nHeight, double& dRatioX, double& dRatioY);

	private:
		DWORD m_dwRef;
	};

	class IGToString
	{
	public:
		static std::bad_exception igproperty_exception;
		IGToString(){}
		virtual ~IGToString(){}
		std::wstring& operator () (std::wstring& wsVal)
		{
			return wsVal;
		}
		std::wstring operator () (const int& nVal)
		{
			wchar_t wInt [32];
			if (::_itow_s (nVal, wInt, 10))
				throw &igproperty_exception;
			return std::wstring (wInt);
		}
		std::wstring operator () (const double& dVal)
		{
			wchar_t wDouble [256];
			if (::swprintf_s (wDouble, L"%.3f", (float)dVal) == -1)
				throw &igproperty_exception;
			return std::wstring (wDouble);
		}
		std::wstring operator () (IGFRAMEPROPERTY_UNIT_ENUM eVal)
		{
			switch (eVal)
			{
			case IGFRAMEPROPERTY_UNIT_CM:
				return std::wstring (L"cm");
			case IGFRAMEPROPERTY_UNIT_PIXEL:
				return std::wstring (L"pixel");
			case IGFRAMEPROPERTY_UNIT_INCH:
				return std::wstring (L"inch");
			}
			return std::wstring (L"Unknown Unit");
		}
		std::wstring operator () (IGFRAMEPROPERTY_STRETCHING_ENUM eVal)
		{
			switch (eVal)
			{
			case IGFRAMEPROPERTY_STRETCHING_NO:
				return std::wstring (L"no");
			case IGFRAMEPROPERTY_STRETCHING_FAST:
				return std::wstring (L"fast");
			case IGFRAMEPROPERTY_STRETCHING_BICUBIC:
				return std::wstring (L"bicubic");
			}
			return std::wstring (L"Unknown Stretch");
		}
		std::wstring operator () (IGBRUSH_TYPE_ENUM eVal)
		{
			std::wstring wsVal;
			if ((eVal & IGBRUSH_TYPE_ENUM_COLOR_SOLID) == IGBRUSH_TYPE_ENUM_COLOR_SOLID)
				wsVal = L"Sol";
			else if ((eVal & IGBRUSH_TYPE_ENUM_COLOR_TEXTURED) == IGBRUSH_TYPE_ENUM_COLOR_TEXTURED)
				wsVal = L"Tex";
			else
				wsVal = L"Trn"; // transparent
			if ((eVal & IGBRUSH_TYPE_ENUM_SHAPE_ROUND) == IGBRUSH_TYPE_ENUM_SHAPE_ROUND)
				wsVal += L"#Rnd";
			else if ((eVal & IGBRUSH_TYPE_ENUM_SHAPE_SQUARE) == IGBRUSH_TYPE_ENUM_SHAPE_SQUARE)
				wsVal += L"#Squ";
			else
				wsVal += L"#Cus";
			if ((eVal & IGBRUSH_TYPE_ENUM_ANTIALIASED) == IGBRUSH_TYPE_ENUM_ANTIALIASED)
				wsVal += L"#AA";
			return wsVal;
		}
		std::wstring operator () (IGSELECTTEXTURE_TYPE_ENUM eVal)
		{
			switch (eVal)
			{
			case IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1:
				return std::wstring (L"Texture1");
			case IGSELECTTEXTURE_TYPE_ENUM_TEXTURE2:
				return std::wstring (L"Texture2");
			}
			return std::wstring (L"Unknown Texture");
		}
		std::wstring operator () (IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM eVal)
		{
			std::wstring wsVal;
			switch (eVal & 3)
			{
			case IGFRAMEPROPERTY_SHOWSEGMENTATION_NO:
				wsVal = L"No";
				break;
			case IGFRAMEPROPERTY_SHOWSEGMENTATION_OPAQUE:
				wsVal = L"Opaq";
				break;
			case IGFRAMEPROPERTY_SHOWSEGMENTATION_TRANSPARENT:
				wsVal = L"Trans";
				break;
			default:
				return wsVal;
			}
			if ((eVal & IGFRAMEPROPERTY_SHOWSEGMENTATION_GRADIENT) == IGFRAMEPROPERTY_SHOWSEGMENTATION_GRADIENT)
				wsVal += L"#Grad";
			else
				wsVal += L"#Orig";
			return wsVal;
		}
		std::wstring operator () (const IGConvertible& val)
		{
			std::wstring wsConv;
			val.ConvertTo(wsConv);
			return wsConv;
		}
		void operator () (std::wstring& wsDest, unsigned int uiPtr, ENUM_IGTYPE ePropType);
	};
}