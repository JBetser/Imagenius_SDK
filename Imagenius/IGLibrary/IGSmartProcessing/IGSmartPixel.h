#pragma once
#include <vector>

namespace IGLibrary
{
	class IGRegion;

	typedef enum {
		IGFRAMEPROPERTY_SHOWSEGMENTATION_NO	= 0,
		IGFRAMEPROPERTY_SHOWSEGMENTATION_OPAQUE	= 1,
		IGFRAMEPROPERTY_SHOWSEGMENTATION_TRANSPARENT = 2,
		IGFRAMEPROPERTY_SHOWSEGMENTATION_GRADIENT = 4} IGFRAMEPROPERTY_SHOWSEGMENTATION_ENUM;

	class IGSmartPixel
	{
		friend class IGRegion;
	public:
		IGSmartPixel (BYTE *p24bPixel = NULL);
		IGSmartPixel (IGRegion *pSmartPixel);
		~IGSmartPixel();

		BYTE* operator *();
		IGSmartPixel& operator =(const IGSmartPixel& copy);
		bool operator <(const IGSmartPixel& pix) const;

		static int less (const void* p1, const void* p2);

		inline void LinkToPixel (BYTE *pPixel, BYTE *pSelection, int nX, int nY);
		inline void LinkToChildRegions (IGRegion *pRegion1, IGRegion *pRegion2);
		inline const std::vector<IGSmartPixel*>& GetNeighbours() const;
		inline void SetNeighbours (const std::vector<IGSmartPixel*>& vNeighbours);
		inline void AddNeighbour (IGSmartPixel* pPixel);
		inline void Clean();
		inline IGRegion *GetRegion() const;
		inline IGRegion *GetChildRegion() const;
		inline IGRegion *GetChildRegion2() const;

		int m_nLabel;
		int m_nNeighbourLabel;
		BYTE m_nbNeighbourSkinPxls;
		BYTE m_gradient;
		BYTE *m_pSelection;
		int	m_nX;
		int m_nY;
		bool m_bEyes;

	private:
		IGRegion *m_pRegion;
		IGRegion *m_pChildRegion;
		IGRegion *m_pChildRegion2;
		BYTE *m_pPixel;
		std::vector<IGSmartPixel*> m_vNeighbours;
	};

	inline void IGSmartPixel::LinkToPixel (BYTE *pPixel, BYTE *pSelection, int nX, int nY)
	{
		m_pPixel = pPixel;
		m_pSelection = pSelection;
		m_nX = nX;
		m_nY = nY;
	}

	inline void IGSmartPixel::LinkToChildRegions (IGRegion *pRegion1, IGRegion *pRegion2)
	{
		m_pChildRegion = pRegion1;
		m_pChildRegion2 = pRegion2;
	}

	inline const std::vector<IGSmartPixel*>& IGSmartPixel::GetNeighbours() const
	{
		return m_vNeighbours;
	}

	inline void IGSmartPixel::SetNeighbours (const std::vector<IGSmartPixel*>& vNeighbours)
	{
		m_vNeighbours = vNeighbours;
	}

	inline void IGSmartPixel::AddNeighbour (IGSmartPixel* pPixel)
	{
		m_vNeighbours.push_back (pPixel);
	}

	inline void IGSmartPixel::Clean()
	{
		m_nLabel = 0;
		m_nNeighbourLabel = 0;
		m_nbNeighbourSkinPxls = 0;
		m_gradient = 0;
		m_bEyes = false;
		m_pRegion = NULL;
		m_pChildRegion = NULL;
		m_pChildRegion2 = NULL;
	}

	inline IGRegion *IGSmartPixel::GetRegion() const
	{
		return m_pRegion;
	}

	inline IGRegion *IGSmartPixel::GetChildRegion() const
	{
		return m_pChildRegion;
	}

	inline IGRegion *IGSmartPixel::GetChildRegion2() const
	{
		return m_pChildRegion2;
	}
}