#pragma once
#include "IGProperty.h"

#define IGFRAMEPROPERTY_RATIO_CM_INCH	0.393700787;

namespace IGLibrary
{
	class IGFrameProperties :
		public IGHasProperties
	{
	public:
		static LPCWSTR IGFRAMEPROPERTY_WIDTH;
		static LPCWSTR IGFRAMEPROPERTY_HEIGHT;
		static LPCWSTR IGFRAMEPROPERTY_UNIT;
		static LPCWSTR IGFRAMEPROPERTY_RESOLUTION;
		static LPCWSTR IGFRAMEPROPERTY_STRETCHING;
		static LPCWSTR IGFRAMEPROPERTY_SHOW_SEGMENTATION;
		static LPCWSTR IGFRAMEPROPERTY_SELECTIONRECT;
		static LPCWSTR IGFRAMEPROPERTY_HISTORYSTEP;
		static LPCWSTR IGFRAMEPROPERTY_FACEDESCRIPTOR;

		IGFrameProperties(void);
		virtual ~IGFrameProperties(void);
		
		bool UpdateSize (int nCurrentWidth, int nCurrentHeight);

	protected:
		virtual bool OnFrameSizeChanged (int nNewWidth, int nNewHeight, IGFRAMEPROPERTY_STRETCHING_ENUM stretchMode) = 0;

	private:
		void initialize();
	};	
}

