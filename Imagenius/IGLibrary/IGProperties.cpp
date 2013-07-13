#include "StdAfx.h"
#include "IGProperties.h"
#include "IGBrush.h"

using namespace IGLibrary;

#define IGPROPERTIES_DEFAULT_BRUSH_TRANSPARENCY		100
#define IGPROPERTIES_DEFAULT_BRUSH_SIZE				25.0

LPCWSTR IGProperties::IGPROPERTY_CURTEXTURE	=			L"CurTexture";
LPCWSTR IGProperties::IGPROPERTY_BKGNDTEXTURE	=		L"BkgndTexture";
LPCWSTR IGProperties::IGPROPERTY_TEXTURE1	=			L"Texture1";
LPCWSTR IGProperties::IGPROPERTY_TEXTURE2	=			L"Texture2";
LPCWSTR IGProperties::IGPROPERTY_BRUSHSIZE	=			L"BrushSize";
LPCWSTR IGProperties::IGPROPERTY_BRUSHTYPE	=			L"BrushType";
LPCWSTR IGProperties::IGPROPERTY_TRANSPARENCY	=		L"Transparency";
LPCWSTR IGProperties::IGPROPERTY_SELECT_TEXTURE =		L"SelectTexture";

IGProperties::IGProperties(void)
{
	initialize();
}

IGProperties::~IGProperties(void)
{
}

void IGProperties::initialize()
{
	SetProperty (IGPROPERTY_TEXTURE1, IGTexture (IGColor (IGColor::black, (BYTE)((float)IGPROPERTIES_DEFAULT_BRUSH_TRANSPARENCY * 2.55f))));
	SetProperty (IGPROPERTY_TEXTURE2, IGTexture (IGColor (IGColor::white, (BYTE)((float)IGPROPERTIES_DEFAULT_BRUSH_TRANSPARENCY * 2.55f))));
	SetProperty (IGPROPERTY_TRANSPARENCY, IGPROPERTIES_DEFAULT_BRUSH_TRANSPARENCY);
	SetProperty (IGPROPERTY_SELECT_TEXTURE, IGSELECTTEXTURE_TYPE_ENUM_TEXTURE1);
	SetProperty (IGPROPERTY_BRUSHSIZE, IGPROPERTIES_DEFAULT_BRUSH_SIZE);
	SetProperty (IGPROPERTY_BRUSHTYPE, IGBRUSH_TYPE_ENUM_DEFAULT);	
}

void IGProperties::Reset()
{
	Clear();
	initialize();	
}
