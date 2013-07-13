#include "stdafx.h"
#include "IGLPE.h"

using namespace std;
using namespace IGLibrary;

int IGSmartPixel::less (const void* p1, const void* p2)
{
	return ((*static_cast <const IGSmartPixel* const *> (p1))->m_gradient - (*static_cast <const IGSmartPixel* const *> (p2))->m_gradient);
}

IGSmartPixel::IGSmartPixel (BYTE *pPixel) : m_pPixel (pPixel)
												, m_pRegion (NULL)
												, m_pChildRegion (NULL)
												, m_pChildRegion2 (NULL)
												, m_nLabel (0)
												, m_nNeighbourLabel (0)
												, m_gradient (0)
												, m_nbNeighbourSkinPxls (0)
												, m_vNeighbours (0)
												, m_nX (-1)
												, m_nY (-1)
												, m_bEyes (false)
{
}

IGSmartPixel::IGSmartPixel (IGRegion *pRegion) : m_pPixel (NULL)
	, m_pRegion (pRegion)
	, m_pChildRegion (NULL)
	, m_pChildRegion2 (NULL)
	, m_nLabel (0)
	, m_nNeighbourLabel (0)
	, m_gradient (0)
	, m_nbNeighbourSkinPxls (0)
	, m_vNeighbours (0)
	, m_nX (-1)
	, m_nY (-1)
	, m_bEyes (false)
{
}

IGSmartPixel::~IGSmartPixel()
{
}

BYTE* IGSmartPixel::operator *()
{
	return m_pPixel;
}

IGSmartPixel& IGSmartPixel::operator =(const IGSmartPixel& copy)
{
	m_pPixel = copy.m_pPixel;
	m_pSelection = copy.m_pSelection;
	m_nLabel = copy.m_nLabel;
	m_nNeighbourLabel = copy.m_nNeighbourLabel;
	m_gradient = copy.m_gradient;
	m_vNeighbours = copy.m_vNeighbours;
	m_nX = copy.m_nX;
	m_nY = copy.m_nY;
	m_pRegion = copy.m_pRegion;
	m_pChildRegion = copy.m_pChildRegion;
	m_pChildRegion2 = copy.m_pChildRegion2;
	m_nbNeighbourSkinPxls = copy.m_nbNeighbourSkinPxls;
	m_bEyes = copy.m_bEyes;
	return *this;
}

bool IGSmartPixel::operator < (const IGSmartPixel& pix) const{
	return m_gradient < pix.m_gradient;
}