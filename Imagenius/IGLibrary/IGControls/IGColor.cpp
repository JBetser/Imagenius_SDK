#include "StdAfx.h"
#include "IGColor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable : 4996)	// disable bogus deprecation warning

using namespace IGLibrary;

const IGColor::DNamedColor IGColor::m_namedColor[IGColor::numNamedColors] =
{
	{aliceblue            , L"aliceblue"},
	{antiquewhite         , L"antiquewhite"},
	{aqua                 , L"aqua"},
	{aquamarine           , L"aquamarine"},
	{azure                , L"azure"},
	{beige                , L"beige"},
	{bisque               , L"bisque"},
	{black                , L"black"},
	{blanchedalmond       , L"blanchedalmond"},
	{blue                 , L"blue"},
	{blueviolet           , L"blueviolet"},
	{brown                , L"brown"},
	{burlywood            , L"burlywood"},
	{cadetblue            , L"cadetblue"},
	{chartreuse           , L"chartreuse"},
	{chocolate            , L"chocolate"},
	{coral                , L"coral"},
	{cornflower           , L"cornflower"},
	{cornsilk             , L"cornsilk"},
	{crimson              , L"crimson"},
	{cyan                 , L"cyan"},
	{darkblue             , L"darkblue"},
	{darkcyan             , L"darkcyan"},
	{darkgoldenrod        , L"darkgoldenrod"},
	{darkgray             , L"darkgray"},
	{darkgreen            , L"darkgreen"},
	{darkkhaki            , L"darkkhaki"},
	{darkmagenta          , L"darkmagenta"},
	{darkolivegreen       , L"darkolivegreen"},
	{darkorange           , L"darkorange"},
	{darkorchid           , L"darkorchid"},
	{darkred              , L"darkred"},
	{darksalmon           , L"darksalmon"},
	{darkseagreen         , L"darkseagreen"},
	{darkslateblue        , L"darkslateblue"},
	{darkslategray        , L"darkslategray"},
	{darkturquoise        , L"darkturquoise"},
	{darkviolet           , L"darkviolet"},
	{deeppink             , L"deeppink"},
	{deepskyblue          , L"deepskyblue"},
	{dimgray              , L"dimgray"},
	{dodgerblue           , L"dodgerblue"},
	{firebrick            , L"firebrick"},
	{floralwhite          , L"floralwhite"},
	{forestgreen          , L"forestgreen"},
	{fuchsia              , L"fuchsia"},
	{gainsboro            , L"gainsboro"},
	{ghostwhite           , L"ghostwhite"},
	{gold                 , L"gold"},
	{goldenrod            , L"goldenrod"},
	{gray                 , L"gray"},
	{green                , L"green"},
	{greenyellow          , L"greenyellow"},
	{honeydew             , L"honeydew"},
	{hotpink              , L"hotpink"},
	{indianred            , L"indianred"},
	{indigo               , L"indigo"},
	{ivory                , L"ivory"},
	{khaki                , L"khaki"},
	{lavender             , L"lavender"},
	{lavenderblush        , L"lavenderblush"},
	{lawngreen            , L"lawngreen"},
	{lemonchiffon         , L"lemonchiffon"},
	{lightblue            , L"lightblue"},
	{lightcoral           , L"lightcoral"},
	{lightcyan            , L"lightcyan"},
	{lightgoldenrodyellow , L"lightgoldenrodyellow"},
	{lightgreen           , L"lightgreen"},
	{lightgrey            , L"lightgrey"},
	{lightpink            , L"lightpink"},
	{lightsalmon          , L"lightsalmon"},
	{lightseagreen        , L"lightseagreen"},
	{lightskyblue         , L"lightskyblue"},
	{lightslategray       , L"lightslategray"},
	{lightsteelblue       , L"lightsteelblue"},
	{lightyellow          , L"lightyellow"},
	{lime                 , L"lime"},
	{limegreen            , L"limegreen"},
	{linen                , L"linen"},
	{magenta              , L"magenta"},
	{maroon               , L"maroon"},
	{mediumaquamarine     , L"mediumaquamarine"},
	{mediumblue           , L"mediumblue"},
	{mediumorchid         , L"mediumorchid"},
	{mediumpurple         , L"mediumpurple"},
	{mediumseagreen       , L"mediumseagreen"},
	{mediumslateblue      , L"mediumslateblue"},
	{mediumspringgreen    , L"mediumspringgreen"},
	{mediumturquoise      , L"mediumturquoise"},
	{mediumvioletred      , L"mediumvioletred"},
	{midnightblue         , L"midnightblue"},
	{mintcream            , L"mintcream"},
	{mistyrose            , L"mistyrose"},
	{moccasin             , L"moccasin"},
	{navajowhite          , L"navajowhite"},
	{navy                 , L"navy"},
	{oldlace              , L"oldlace"},
	{olive                , L"olive"},
	{olivedrab            , L"olivedrab"},
	{orange               , L"orange"},
	{orangered            , L"orangered"},
	{orchid               , L"orchid"},
	{palegoldenrod        , L"palegoldenrod"},
	{palegreen            , L"palegreen"},
	{paleturquoise        , L"paleturquoise"},
	{palevioletred        , L"palevioletred"},
	{papayawhip           , L"papayawhip"},
	{peachpuff            , L"peachpuff"},
	{peru                 , L"peru"},
	{pink                 , L"pink"},
	{plum                 , L"plum"},
	{powderblue           , L"powderblue"},
	{purple               , L"purple"},
	{red                  , L"red"},
	{rosybrown            , L"rosybrown"},
	{royalblue            , L"royalblue"},
	{saddlebrown          , L"saddlebrown"},
	{salmon               , L"salmon"},
	{sandybrown           , L"sandybrown"},
	{seagreen             , L"seagreen"},
	{seashell             , L"seashell"},
	{sienna               , L"sienna"},
	{silver               , L"silver"},
	{skyblue              , L"skyblue"},
	{slateblue            , L"slateblue"},
	{slategray            , L"slategray"},
	{snow                 , L"snow"},
	{springgreen          , L"springgreen"},
	{steelblue            , L"steelblue"},
	{tan                  , L"tan"},
	{teal                 , L"teal"},
	{thistle              , L"thistle"},
	{tomato               , L"tomato"},
	{turquoise            , L"turquoise"},
	{violet               , L"violet"},
	{wheat                , L"wheat"},
	{white                , L"white"},
	{whitesmoke           , L"whitesmoke"},
	{yellow               , L"yellow"},
	{yellowgreen          , L"yellowgreen"}
};

std::wstring IGColor::GetNameFromIndex(int i)
{
	_ASSERT(0 <= i && i < numNamedColors);
	return m_namedColor[i].name;
}

IGColor IGColor::GetColorFromIndex(int i)
{
	_ASSERT(0 <= i && i < numNamedColors);
	return m_namedColor[i].color;
}

IGColor IGColor::FromString(LPCTSTR pcColor)
{
	IGColor t;
	t.SetString(pcColor);
	return t;
}

IGColor::IGColor(COLORREF cr, BYTE alpha)
  : m_bIsRGB(true), m_bIsHLS(false), m_colorref(cr), m_transparency(alpha)
{
}

IGColor::operator COLORREF() const
{
	const_cast<IGColor*>(this)->ToRGB();
	return m_colorref;
}

// RGB

void IGColor::SetRed(int red)
{
	_ASSERT(0 <= red && red <= 255);
	ToRGB();
	m_color[c_red] = static_cast<unsigned char>(red);
	m_bIsHLS = false;
}

void IGColor::SetGreen(int green)
{
	_ASSERT(0 <= green && green <= 255);
	ToRGB();
	m_color[c_green] = static_cast<unsigned char>(green);
	m_bIsHLS = false;
}

void IGColor::SetBlue(int blue)
{
	_ASSERT(0 <= blue && blue <= 255);
	ToRGB();
	m_color[c_blue] = static_cast<unsigned char>(blue);
	m_bIsHLS = false;
}

void IGColor::SetRGB(int red, int green, int blue)
{
	_ASSERT(0 <= red && red <= 255);
	_ASSERT(0 <= green && green <= 255);
	_ASSERT(0 <= blue && blue <= 255);
	
	m_color[c_red]   = static_cast<unsigned char>(red);
	m_color[c_green] = static_cast<unsigned char>(green);
	m_color[c_blue]  = static_cast<unsigned char>(blue);
	m_bIsHLS = false;
	m_bIsRGB = true;
}

int IGColor::GetRed() const
{
	const_cast<IGColor*>(this)->ToRGB();
	return m_color[c_red];
}

int IGColor::GetGreen() const
{
	const_cast<IGColor*>(this)->ToRGB();
	return m_color[c_green];
}

int IGColor::GetBlue() const
{
	const_cast<IGColor*>(this)->ToRGB();
	return m_color[c_blue];
}

RGBQUAD IGColor::GetRGBQUAD () const
{
	RGBQUAD rgbquad;
	rgbquad.rgbBlue = GetBlue();
	rgbquad.rgbGreen = GetGreen();
	rgbquad.rgbRed = GetRed();
	rgbquad.rgbReserved = m_transparency;
	return rgbquad;
}

// HSL

void IGColor::SetHue(float hue)
{
	_ASSERT(hue >= 0.0 && hue <= 360.0);
	
	ToHLS();
	m_hue = hue;
	m_bIsRGB = false;
}

void IGColor::SetSaturation(float saturation)
{
	_ASSERT(saturation >= 0.0 && saturation <= 1.0); // 0.0 ist undefiniert
	
	ToHLS();
	m_saturation = saturation;
	m_bIsRGB = false;
}

void IGColor::SetLuminance(float luminance)
{
	_ASSERT(luminance >= 0.0 && luminance <= 1.0);
	
	ToHLS();
	m_luminance = luminance;
	m_bIsRGB = false;
}

void IGColor::SetHLS(float hue, float luminance, float saturation)
{
	_ASSERT(hue >= 0.0 && hue <= 360.0);
	_ASSERT(luminance >= 0.0 && luminance <= 1.0);
	_ASSERT(saturation >= 0.0 && saturation <= 1.0); // 0.0 ist undefiniert
	
	m_hue = hue;
	m_luminance = luminance;
	m_saturation = saturation;
	m_bIsRGB = false;
	m_bIsHLS = true;
}

float IGColor::GetHue() const
{
	const_cast<IGColor*>(this)->ToHLS();
	return m_hue;
}

float IGColor::GetSaturation() const
{
	const_cast<IGColor*>(this)->ToHLS();
	return m_saturation;
}

float IGColor::GetLuminance() const
{
	const_cast<IGColor*>(this)->ToHLS();
	return m_luminance;
}

// Konvertierung

void IGColor::ToHLS() 
{
	if (!m_bIsHLS)
	{
		// Konvertierung
		unsigned char minval = min(m_color[c_red], min(m_color[c_green], m_color[c_blue]));
		unsigned char maxval = max(m_color[c_red], max(m_color[c_green], m_color[c_blue]));
		float mdiff  = float(maxval) - float(minval);
		float msum   = float(maxval) + float(minval);
		
		m_luminance = msum / 510.0f;
		
		if (maxval == minval) 
		{
			m_saturation = 0.0f;
			m_hue = 0.0f; 
		}   
		else 
		{ 
			float rnorm = (maxval - m_color[c_red]  ) / mdiff;      
			float gnorm = (maxval - m_color[c_green]) / mdiff;
			float bnorm = (maxval - m_color[c_blue] ) / mdiff;   
			
			m_saturation = (m_luminance <= 0.5f) ? 
				(mdiff / msum) : (mdiff / (510.0f - msum));
			
			if (m_color[c_red]   == maxval) m_hue = 60.0f * (6.0f + bnorm - gnorm);
			if (m_color[c_green] == maxval) m_hue = 60.0f * (2.0f + rnorm - bnorm);
			if (m_color[c_blue]  == maxval) m_hue = 60.0f * (4.0f + gnorm - rnorm);
			if (m_hue > 360.0f) m_hue = m_hue - 360.0f;
		}
		m_bIsHLS = true;
	}
}

void IGColor::ToRGB() 
{
	if (!m_bIsRGB)
	{
		if (m_saturation == 0.0) // Grauton, einfacher Fall
		{
			m_color[c_red] = m_color[c_green] = m_color[c_blue] = 
				unsigned char(m_luminance * 255.0);
		}
		else
		{
			float rm1, rm2;
			
			if (m_luminance <= 0.5f) 
				rm2 = m_luminance + m_luminance * m_saturation;  
			else
				rm2 = m_luminance + m_saturation - m_luminance * m_saturation;
			rm1 = 2.0f * m_luminance - rm2;   
			m_color[c_red]   = ToRGB1(rm1, rm2, m_hue + 120.0f);   
			m_color[c_green] = ToRGB1(rm1, rm2, m_hue);
			m_color[c_blue]  = ToRGB1(rm1, rm2, m_hue - 120.0f);
		}
		m_bIsRGB = true;
	}
}

unsigned char IGColor::ToRGB1(float rm1, float rm2, float rh)
{
	if      (rh > 360.0f) rh -= 360.0f;
	else if (rh <   0.0f) rh += 360.0f;
	
	if      (rh <  60.0f) rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
	else if (rh < 180.0f) rm1 = rm2;
	else if (rh < 240.0f) rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;
	
#if 0  // -----------------------------------------------------------
	rm1 = rm1 * 255.;
	rm1 += 0.5;
	if (rm1 > 255.0)
		rm1 = 255.0;
#endif // -----------------------------------------------------------
	
	return static_cast<unsigned char>(rm1 * 255);
}

// Stringkonvertierung im Format RRGGBB

std::wstring IGColor::GetString() const 
{
	std::wstring wFormat (L"%02X%02X%02X");
	std::wstring wColor;

	wchar_t *pwFormatedColor = new wchar_t [wFormat.length() + 1];
	::wsprintfW (pwFormatedColor, wFormat.c_str(), GetRed(), GetGreen(), GetBlue());
	wColor = pwFormatedColor;
	delete [] pwFormatedColor;

	return wColor;
}

BOOL IGColor::SetString(LPCTSTR pcColor) 
{
	_ASSERT (pcColor);
	int r, g, b;
	if (::swscanf_s (pcColor, L"%2x%2x%2x", &r, &g, &b) != 3) 
	{
		m_color[c_red] = m_color[c_green] = m_color[c_blue] = 0;
		return FALSE;
	}
	else
	{
		m_color[c_red]   = static_cast<unsigned char>(r);
		m_color[c_green] = static_cast<unsigned char>(g);
		m_color[c_blue]  = static_cast<unsigned char>(b);
		m_bIsRGB = true;
		m_bIsHLS = false;
		return true;
	}
}

std::wstring IGColor::GetName() const
{
	const_cast<IGColor*>(this)->ToRGB();
	int i = numNamedColors; 
	while (i-- && m_colorref != m_namedColor[i].color);
	if (i < 0) 
	{
		return L"#" + GetString();
	}
	else return m_namedColor[i].name;
}
