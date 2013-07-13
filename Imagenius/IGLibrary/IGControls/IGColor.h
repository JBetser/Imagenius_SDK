#pragma once
#include "ximadef.h"
#include <string>

namespace IGLibrary
{

class IGColor
{
public:

	// COLORREF Konvertierung
	// ----------------------
	IGColor(COLORREF cr = black, BYTE alpha = 255);
	operator COLORREF() const;

	// RGB
	// --------------
	void SetRed(int red);     // 0..255  
	void SetGreen(int green); // 0..255
	void SetBlue(int blue);   // 0..255
	void SetRGB(int red, int green, int blue);

	int GetRed() const;   // 0..255
	int GetGreen() const; // 0..255
	int GetBlue() const;  // 0..255

	RGBQUAD GetRGBQUAD () const;

	// HLS
	// --------------
	void SetHue(float hue);               // 0.0 .. 360.0
	void SetLuminance(float luminance);   // 0.0 .. 1.0
	void SetSaturation(float saturation); // 0.0 .. 1.0
	void SetHLS(float hue, float luminance, float saturation);

	float GetHue() const;        // 0.0 .. 360.0
	float GetLuminance() const;  // 0.0 .. 1.0
	float GetSaturation() const; // 0.0 .. 1.0

	// String
	// ---------------------
	std::wstring GetString() const;
	BOOL    SetString(LPCTSTR pcColor);

	std::wstring GetName() const; 

	static IGColor FromString(LPCTSTR pcColor); 
	static std::wstring GetNameFromIndex(int i);
	static IGColor GetColorFromIndex(int i);
	static int    GetNumNames() {return numNamedColors;}

	enum ENamedColor // Named Colors, could be used as COLORREF
	{
		none                 = 0x7FFFFFFF,   
		aliceblue            = 0x00FFF8F0,    
		antiquewhite         = 0x00D7EBFA,    
		aqua                 = 0x00FFFF00,    
		aquamarine           = 0x00D4FF7F,    
		azure                = 0x00FFFFF0,    
		beige                = 0x00DCF5F5,    
		bisque               = 0x00C4E4FF,    
		black                = 0x00000000,    
		blanchedalmond       = 0x00CDEBFF,    
		blue                 = 0x00FF0000,    
		blueviolet           = 0x00E22B8A,    
		brown                = 0x002A2AA5,    
		burlywood            = 0x0087B8DE,    
		cadetblue            = 0x00A09E5F,    
		chartreuse           = 0x0000FF7F,    
		chocolate            = 0x001E69D2,    
		coral                = 0x00507FFF,    
		cornflower           = 0x00ED9564,    
		cornsilk             = 0x00DCF8FF,    
		crimson              = 0x003C14DC,    
		cyan                 = 0x00FFFF00,    
		darkblue             = 0x008B0000,    
		darkcyan             = 0x008B8B00,    
		darkgoldenrod        = 0x000B86B8,    
		darkgray             = 0x00A9A9A9,    
		darkgreen            = 0x00006400,    
		darkkhaki            = 0x006BB7BD,    
		darkmagenta          = 0x008B008B,    
		darkolivegreen       = 0x002F6B55,    
		darkorange           = 0x00008CFF,    
		darkorchid           = 0x00CC3299,    
		darkred              = 0x0000008B,    
		darksalmon           = 0x007A96E9,    
		darkseagreen         = 0x008BBC8F,    
		darkslateblue        = 0x008B3D48,    
		darkslategray        = 0x004F4F2F,    
		darkturquoise        = 0x00D1CE00,    
		darkviolet           = 0x00D30094,    
		deeppink             = 0x009314FF,    
		deepskyblue          = 0x00FFBF00,    
		dimgray              = 0x00696969,    
		dodgerblue           = 0x00FF901E,    
		firebrick            = 0x002222B2,    
		floralwhite          = 0x00F0FAFF,    
		forestgreen          = 0x00228B22,    
		fuchsia              = 0x00FF00FF,    
		gainsboro            = 0x00DCDCDC,    
		ghostwhite           = 0x00FFF8F8,    
		gold                 = 0x0000D7FF,    
		goldenrod            = 0x0020A5DA,    
		gray                 = 0x00808080,    
		green                = 0x00008000,    
		greenyellow          = 0x002FFFAD,    
		honeydew             = 0x00F0FFF0,    
		hotpink              = 0x00B469FF,    
		indianred            = 0x005C5CCD,    
		indigo               = 0x0082004B,    
		ivory                = 0x00F0FFFF,    
		khaki                = 0x008CE6F0,    
		lavender             = 0x00FAE6E6,    
		lavenderblush        = 0x00F5F0FF,    
		lawngreen            = 0x0000FC7C,    
		lemonchiffon         = 0x00CDFAFF,    
		lightblue            = 0x00E6D8AD,    
		lightcoral           = 0x008080F0,    
		lightcyan            = 0x00FFFFE0,    
		lightgoldenrodyellow = 0x00D2FAFA,    
		lightgreen           = 0x0090EE90,    
		lightgrey            = 0x00D3D3D3,    
		lightpink            = 0x00C1B6FF,    
		lightsalmon          = 0x007AA0FF,    
		lightseagreen        = 0x00AAB220,    
		lightskyblue         = 0x00FACE87,    
		lightslategray       = 0x00998877,    
		lightsteelblue       = 0x00DEC4B0,    
		lightyellow          = 0x00E0FFFF,    
		lime                 = 0x0000FF00,    
		limegreen            = 0x0032CD32,    
		linen                = 0x00E6F0FA,    
		magenta              = 0x00FF00FF,    
		maroon               = 0x00000080,    
		mediumaquamarine     = 0x00AACD66,    
		mediumblue           = 0x00CD0000,    
		mediumorchid         = 0x00D355BA,    
		mediumpurple         = 0x00DB7093,    
		mediumseagreen       = 0x0071B33C,    
		mediumslateblue      = 0x00EE687B,    
		mediumspringgreen    = 0x009AFA00,    
		mediumturquoise      = 0x00CCD148,    
		mediumvioletred      = 0x008515C7,    
		midnightblue         = 0x00701919,    
		mintcream            = 0x00FAFFF5,    
		mistyrose            = 0x00E1E4FF,    
		moccasin             = 0x00B5E4FF,    
		navajowhite          = 0x00ADDEFF,    
		navy                 = 0x00800000,    
		oldlace              = 0x00E6F5FD,    
		olive                = 0x00008080,    
		olivedrab            = 0x00238E6B,    
		orange               = 0x0000A5FF,    
		orangered            = 0x000045FF,    
		orchid               = 0x00D670DA,    
		palegoldenrod        = 0x00AAE8EE,    
		palegreen            = 0x0098FB98,    
		paleturquoise        = 0x00EEEEAF,    
		palevioletred        = 0x009370DB,    
		papayawhip           = 0x00D5EFFF,    
		peachpuff            = 0x00B9DAFF,    
		peru                 = 0x003F85CD,    
		pink                 = 0x00CBC0FF,    
		plum                 = 0x00DDA0DD,    
		powderblue           = 0x00E6E0B0,    
		purple               = 0x00800080,    
		red                  = 0x000000FF,    
		rosybrown            = 0x008F8FBC,    
		royalblue            = 0x00E16941,    
		saddlebrown          = 0x0013458B,    
		salmon               = 0x007280FA,    
		sandybrown           = 0x0060A4F4,    
		seagreen             = 0x00578B2E,    
		seashell             = 0x00EEF5FF,    
		sienna               = 0x002D52A0,    
		silver               = 0x00C0C0C0,    
		skyblue              = 0x00EBCE87,    
		slateblue            = 0x00CD5A6A,    
		slategray            = 0x00908070,    
		snow                 = 0x00FAFAFF,    
		springgreen          = 0x007FFF00,    
		steelblue            = 0x00B48246,    
		tan                  = 0x008CB4D2,    
		teal                 = 0x00808000,    
		thistle              = 0x00D8BFD8,    
		tomato               = 0x004763FF,    
		turquoise            = 0x00D0E040,    
		violet               = 0x00EE82EE,    
		wheat                = 0x00B3DEF5,    
		white                = 0x00FFFFFF,    
		whitesmoke           = 0x00F5F5F5,    
		yellow               = 0x0000FFFF,    
		yellowgreen          = 0x0032CD9A,    
	};

	enum ENamedColorIndex
	{ 
		i_aliceblue, i_antiquewhite, i_aqua, i_aquamarine, i_azure, i_beige, i_bisque, i_black, 
		i_blanchedalmond, i_blue, i_blueviolet, i_brown, i_burlywood, i_cadetblue, i_chartreuse,
		i_chocolate, i_coral, i_cornflower, i_cornsilk, i_crimson, i_cyan, i_darkblue, i_darkcyan, 
		i_darkgoldenrod, i_darkgray, i_darkgreen, i_darkkhaki, i_darkmagenta, i_darkolivegreen, 
		i_darkorange, i_darkorchid, i_darkred, i_darksalmon, i_darkseagreen, i_darkslateblue, 
		i_darkslategray, i_darkturquoise, i_darkviolet, i_deeppink, i_deepskyblue, i_dimgray, 
		i_dodgerblue, i_firebrick, i_floralwhite, i_forestgreen, i_fuchsia, i_gainsboro, 
		i_ghostwhite, i_gold, i_goldenrod, i_gray, i_green, i_greenyellow, i_honeydew, i_hotpink, 
		i_indianred, i_indigo, i_ivory, i_khaki, i_lavender, i_lavenderblush, i_lawngreen, 
		i_lemonchiffon, i_lightblue, i_lightcoral, i_lightcyan, i_lightgoldenrodyellow, 
		i_lightgreen, i_lightgrey, i_lightpink, i_lightsalmon, i_lightseagreen, i_lightskyblue, 
		i_lightslategray, i_lightsteelblue, i_lightyellow, i_lime, i_limegreen, i_linen, 
		i_magenta, i_maroon, i_mediumaquamarine, i_mediumblue, i_mediumorchid, i_mediumpurple, 
		i_mediumseagreen, i_mediumslateblue, i_mediumspringgreen, i_mediumturquoise, 
		i_mediumvioletred, i_midnightblue, i_mintcream, i_mistyrose, i_moccasin, i_navajowhite, 
		i_navy, i_oldlace, i_olive, i_olivedrab, i_orange, i_orangered, i_orchid, i_palegoldenrod, 
		i_palegreen, i_paleturquoise, i_palevioletred, i_papayawhip, i_peachpuff, i_peru, i_pink,
		i_plum, i_powderblue, i_purple, i_red, i_rosybrown, i_royalblue, i_saddlebrown, i_salmon, 
		i_sandybrown, i_seagreen, i_seashell, i_sienna, i_silver, i_skyblue, i_slateblue, 
		i_slategray, i_snow, i_springgreen, i_steelblue, i_tan, i_teal, i_thistle, i_tomato, 
		i_turquoise, i_violet, i_wheat, i_white, i_whitesmoke, i_yellow, i_yellowgreen,
		numNamedColors
	};

	void ToRGB(); 
	void ToHLS(); 

	inline void SetTransparency (BYTE alpha)
	{
		m_transparency = alpha;
	}

	inline BYTE GetTransparency() const
	{
		return m_transparency;
	}

	inline bool HasTransparency() const
	{
		return m_transparency != 255;
	}

private:

	static unsigned char ToRGB1(float rm1, float rm2, float rh);

	union 
	{
		COLORREF      m_colorref;
		unsigned char m_color[4];
	};
	enum {c_red = 0, c_green = 1, c_blue = 2, c_null = 3}; 

	float m_hue;         // 0.0 .. 360.0  
	float m_saturation;  // 0.0 .. 1.0    
	float m_luminance;   // 0.0 .. 1.0    

	BOOL m_bIsRGB; 
	BOOL m_bIsHLS; 

	BYTE m_transparency;

	struct DNamedColor
	{
		COLORREF color; 
		const wchar_t  *name;
	};
	static const DNamedColor m_namedColor[numNamedColors]; 
};

}	// namespace IGLibrary