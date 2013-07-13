#include "StdAfx.h"
#include "IGTexture.h"

using namespace IGLibrary;

RECT	IGTexture::IGTEXTURE_DEFAULTRECT = {0, 0, -1, -1};

IGTexture::IGTexture()
{
}


IGTexture::~IGTexture()
{
}

IGTexture::IGTexture(COLORREF color) : m_colorMajor (color)
{
}

IGTexture::IGTexture(const IGColor& color) : m_colorMajor (color)
{
}

IGTexture::IGTexture(const IGTexture& texture): m_colorMajor (texture)
{
}

IGTexture::IGTexture(const IGFrame& frame, const RECT& rect)
{
}