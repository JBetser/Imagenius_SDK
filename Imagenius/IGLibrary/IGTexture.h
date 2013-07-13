#pragma once
#include "IGConvertible.h"
#include "IGColor.h"
#include "IGFrame.h"

namespace IGLibrary
{
	class IGTexture :
		public IGConvertible
	{
	public:
		static RECT	IGTEXTURE_DEFAULTRECT;
		IGTexture();
		IGTexture(COLORREF);
		IGTexture(const IGColor&);
		IGTexture(const IGTexture&);
		IGTexture(const IGFrame&, const RECT& rect = IGTEXTURE_DEFAULTRECT);
		virtual ~IGTexture();

		virtual bool ConvertTo(int& i) const {
			i = (COLORREF)m_colorMajor;
			return true;
		}
		virtual bool ConvertTo(std::wstring& wsColor) const {
			unsigned int nColValue = *reinterpret_cast <unsigned int*> (&m_colorMajor.GetRGBQUAD());
			wchar_t wColor [32];
			if (::_ultow_s (nColValue, wColor, 32, 10) != 0)
				return false;
			wsColor += wColor;
			return true;
		}
		virtual bool ConvertTo(COLORREF& col) const {
			col = (COLORREF)m_colorMajor;
			return true;
		}
		virtual bool ConvertTo(IGColor& col) const {
			col = IGColor (m_colorMajor);
			return true;
		}

		operator COLORREF() const
		{
			COLORREF col;
			ConvertTo(col);
			return col;
		}

		operator IGColor() const
		{
			IGColor col;
			ConvertTo(col);
			return col;
		}

		operator RGBQUAD() const
		{
			IGColor col;
			ConvertTo(col);
			return col.GetRGBQUAD ();
		}

		IGTexture& operator = (const IGTexture& texture)
		{
			m_colorMajor = (IGColor)texture;
			return *this;
		}

		void CopyFrom (const IGConvertible& conv)
		{
			const IGTexture& text = dynamic_cast <const IGTexture&> (conv);
			operator = (text);
		}

		void CopyFrom (const int& conv)
		{
			m_colorMajor = IGColor (conv);
		}

		void SetTransparency (BYTE alpha)
		{
			m_colorMajor.SetTransparency(alpha);
		}

		inline BYTE GetTransparency() const
		{
			return m_colorMajor.GetTransparency();
		}

		inline bool HasTransparency() const
		{
			return m_colorMajor.HasTransparency();
		}

	private:
		IGColor m_colorMajor;
	};
}