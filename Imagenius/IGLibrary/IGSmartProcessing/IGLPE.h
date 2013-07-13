#pragma once
#include "IGSmartPixel.h"
#include <vector>
#include <set>
#include <algorithm>

class CxImage;

#define IGMARKER_BACKGROUND	0
#define IGMARKER_REGION1	1
#define IGMARKER_EYES		666

namespace IGLibrary
{
	class IGSmartLayer;

	class IGRegion
	{
		friend class IGSmartLayer;
		friend class IGMarkerSubdivided;
	public:
		IGRegion (int nLabel, bool bIsFace = false);
		IGRegion (const IGRegion& region);
		virtual ~IGRegion();

		inline static bool IsValidRect (const RECT& rc);
		inline int GetLabel() const;
		void AddPixel (IGSmartPixel *p24bPixel);
		void AddSimplePixel (IGSmartPixel *p24bPixel);
		inline void RemovePixel (IGSmartPixel *p24bPixel);
		inline void Clear();
		inline bool IsFace() const { return m_bIsFace; }
		void SetLabel ();
		void SetLabel (int nLabel, bool bMarkDone = false);
		void Merge (IGRegion& region);
		void ApplyMedianColor();
		virtual void Select (BYTE level = 255);
		const std::vector <IGSmartPixel*>& GetBoundaries();
		bool Erode(int nSize);
		bool Dilate(int nSize);
		int GetSaddleRegionLabel(const std::vector <IGRegion*>& vRegions) const;
		static int GetSaddleRegionLabel(const std::vector <IGRegion*>& vRegions, const std::vector <IGSmartPixel*>& vBoundaries, int nRegionLabel);
		void SetEyes();
		void SetEyes (const RECT& rectEye);
		void SetEyes (const RECT& rectEye1, const RECT& rectEye2);
		void SetMouth (const RECT& rectMouth);
		void SetNoze (const RECT& rectMouth);
		void GetEyes (RECT& rectEyeLeft, RECT& rectEyeRight){
			rectEyeLeft = m_rcEyeLeft;
			rectEyeRight = m_rcEyeRight;
		}
		void GetMouth (RECT& rectMouth){
			rectMouth = m_rcMouth;
		}
		void GetNoze (RECT& rectNoze){
			rectNoze = m_rcNoze;
		}
		void SetGenericEyes();
		void CalcRect();
		RECT GetRect();
		void Rotate (int nPtCtrX, int nPtCtrY, int nRotate);

	protected:
		int m_nLabel;
		BYTE m_minGradient;
		RECT m_rcReg;
		RECT m_rcEyes;
		RECT m_rcEyeLeft;
		RECT m_rcEyeRight;
		RECT m_rcInnerEyes;
		RECT m_rcMouth;
		RECT m_rcNoze;
		bool m_bIsFace;
		bool m_bDone;
		std::vector <IGSmartPixel*> m_vPixels;
		std::vector <IGSmartPixel*> m_vBoundaries;
		std::vector <IGSmartPixel*> m_vNeighbourSaddles;
		std::vector <IGSmartPixel*> m_vNeighbourSaddlesReconstruct;
	};

	inline bool IGRegion::IsValidRect (const RECT& rc)
	{
		return (rc.top != -1 && rc.left != -1 && rc.right != -1 && rc.bottom != -1);
	}

	inline int IGRegion::GetLabel() const
	{
		return m_nLabel;
	}

	inline void IGRegion::RemovePixel (IGSmartPixel *p24bPixel)
	{
		std::vector<IGSmartPixel*>::const_iterator itPixel = std::find (m_vPixels.cbegin(), m_vPixels.cend(), p24bPixel);
		if (itPixel == m_vPixels.cend())
			return;
		m_vPixels.erase (itPixel);
	}

	inline void IGRegion::Clear()
	{
		m_vPixels.clear();
	}

	class IGMarker : public IGRegion
	{
		friend class IGMarkerLayer;
	public:
		typedef enum {IGMARKER_CIRCLE = 0,
						IGMARKER_DISC = 1,
						IGMARKER_CROWN = 2,
						IGMARKER_HALFCIRCLE = 3, 
						IGMARKER_HALO = 4,
						IGMARKER_RECTANGLE = 5,
						IGMARKER_SKIN = 6, 
						IGMARKER_FILLEDRECTANGLE = 7 } ENUM_IGMARKER;
		static IGMarker* Create (ENUM_IGMARKER eType, int nLabel, const POINT& ptCenter, int nSize1, int nSize2 = -1, bool bSubdivided = false);

		IGMarker (int nLabel);
		IGMarker (int nLabel, CxImage& cxMarker, int nRot = 0);
		virtual ~IGMarker();

		virtual bool Mark (IGSmartLayer& layer);
		virtual void Odd();
		virtual int GetNbMarkers() {return 1;}
		virtual IGMarker* GetMarker (int nIdx) {return nIdx ? NULL : this;}
		virtual bool FilterBackground() {return false;}
		CxImage* GetLayer() {return m_pCxMarker;}

	protected:
		CxImage *m_pCxMarker;
		int m_nRot;
	};

	class IGMarkerSubdivided : public IGMarker
	{
		friend class IGSmartLayer;
	public:
		IGMarkerSubdivided(int nLabel, CxImage& cxMarker);
		IGMarkerSubdivided(int nLabel, const std::vector <IGSmartPixel*>& m_vPixels);
		virtual ~IGMarkerSubdivided();

		virtual bool Mark (IGSmartLayer& layer);
		virtual void Odd();
		virtual void Select (BYTE level = 255);
		virtual int GetNbMarkers() {return m_vSubdivisions.size();}
		virtual IGMarker* GetMarker (int nIdx);

	protected:
		void SubDivide (IGSmartLayer& layer);

	private:
		std::vector <IGMarker*> m_vSubdivisions;
	};

	class IGMarkerLayer : public IGMarker
	{
	public:
		IGMarkerLayer (int nLabel, CxImage& cxMarker) : IGMarker (nLabel, cxMarker){}
		virtual ~IGMarkerLayer(){}

		virtual bool Mark (IGSmartLayer& layer);
	};

	class IGMarkerCircle : public IGMarker
	{
	public:
		IGMarkerCircle(int nLabel, const POINT& ptCenter, int nRadius, bool bDisc = false);
		virtual ~IGMarkerCircle();

		virtual bool Mark (IGSmartLayer& layer);
	protected:
		bool	m_bDisc;
		int		m_nRadius;
		POINT	m_ptCenter;
	};

	class IGMarkerHalfCircle : public IGMarkerCircle
	{
	public:
		IGMarkerHalfCircle(int nLabel, const POINT& ptCenter, int nRadius, bool bDisc = false);
		virtual ~IGMarkerHalfCircle();

		virtual bool Mark (IGSmartLayer& layer);
	};

	class IGMarkerDisc : public IGMarkerCircle
	{
	public:
		IGMarkerDisc(int nLabel, const POINT& ptCenter, int nRadius);
		virtual ~IGMarkerDisc();
	};

	class IGMarkerCrown : public IGMarkerCircle
	{
	public:
		IGMarkerCrown(int nLabel, const POINT& ptCenter, int nRadius, int nSize);
		virtual ~IGMarkerCrown();

		virtual bool Mark (IGSmartLayer& layer);
	protected:
		int m_nRadiusMax;
	};

	class IGMarkerHalo : public IGMarkerCrown
	{
	public:
		IGMarkerHalo(int nLabel, const POINT& ptCenter, int nRadius, int nSize);
		virtual ~IGMarkerHalo();

		virtual bool Mark (IGSmartLayer& layer);
	};

	class IGMarkerRectangle : public IGMarker
	{
	public:
		IGMarkerRectangle(int nLabel, const POINT& ptCenter, int nWidth, int nHeight, bool bFilled = false);
		virtual ~IGMarkerRectangle();

		virtual bool Mark (IGSmartLayer& layer);
	protected:
		POINT	m_ptCenter;
		int		m_nWidth;
		int		m_nHeight;
		bool	m_bFilled;
	};
}