#pragma once
#include "IGProperty.h"

namespace IGLibrary
{
	class IGProperties :
		public IGHasProperties
	{
	public:
		static LPCWSTR IGPROPERTY_TEXTURE1;
		static LPCWSTR IGPROPERTY_TEXTURE2;
		static LPCWSTR IGPROPERTY_BRUSHSIZE;
		static LPCWSTR IGPROPERTY_BRUSHTYPE;
		static LPCWSTR IGPROPERTY_TRANSPARENCY;
		static LPCWSTR IGPROPERTY_SELECT_TEXTURE;
		static LPCWSTR IGPROPERTY_CURTEXTURE;
		static LPCWSTR IGPROPERTY_BKGNDTEXTURE;

		IGProperties(void);
		virtual ~IGProperties(void);

		void Reset();
	private:
		void initialize();		
	};
}

