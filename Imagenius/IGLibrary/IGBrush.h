#pragma once
#include "IGTexture.h"

namespace IGLibrary
{
	class IGBrush : public CxBrush
	{
	public:
		IGBrush() {}
		virtual ~IGBrush() {}

		static IGBrush* CreateBrush (const IGHasProperties& hasProps);

		inline float GetSize() const;
		inline virtual bool IsAA() const {return false;}

		operator COLORREF() const
		{
			return m_texture;
		}

		void SetColor (COLORREF col);
		void SetTransparency (BYTE alpha);
		void SetType (IGBRUSH_TYPE_ENUM eType);

	protected:
		IGBrush(const IGTexture&, float fSize, IGBRUSH_TYPE_ENUM eType = IGBRUSH_TYPE_ENUM_DEFAULT);
		
		template <typename T>
		inline double Pythagore2 (T x, T y) const
		{
			return Square (x) + Square (y);
		}
		
		template <typename T>
		inline double Square (T x) const
		{
			return x * x;
		}

		float			m_fBrushSize;
		IGBRUSH_TYPE_ENUM m_eTypeBrush;
		IGTexture		m_texture;
	};

	inline float IGBrush::GetSize() const
	{
		return m_fBrushSize;
	}

	class IGSolidBrushRound : public IGBrush
	{
	public:
		IGSolidBrushRound (const IGColor& color, float fSize)  : IGBrush (IGTexture (color), fSize, IGBRUSH_TYPE_ENUM_SHAPE_ROUND_SOLID){}
		virtual ~IGSolidBrushRound(){}

		virtual bool Apply (float x, float y, const BYTE *pOriginalBits, BYTE *pBits, BYTE *pAlpha, BYTE *pSel, const int& nWidth, const int& nHeight, const DWORD& dwEffWidth) const;
	};

	class IGSolidBrushAARound : public IGSolidBrushRound
	{
	public:
		IGSolidBrushAARound (const IGColor& color, float fSize)  : IGSolidBrushRound (color, fSize){
			m_eTypeBrush = IGBRUSH_TYPE_ENUM_SHAPE_ROUND_AA_SOLID;
			m_nAAStep = 0;
		}
		virtual ~IGSolidBrushAARound(){}

		virtual bool Apply (float x, float y, const BYTE *pOriginalBits, BYTE *pBits, BYTE *pAlpha, BYTE *pSel, const int& nWidth, const int& nHeight, const DWORD& dwEffWidth) const;

		inline virtual bool IsAA() const {return true;}

	private:
		mutable int m_nAAStep;
	};
}

