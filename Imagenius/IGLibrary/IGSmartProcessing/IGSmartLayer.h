#pragma once
#include "IGLPE.h"
#include "IGSmartPixel.h"
#include "IGFaceEffect.h"
//#include "ximage.h"
#include <vector>
#include <list>
#include <map>

#define ABS(x) (x >= 0 ? x : -x)

#define IGSMARTLAYER_INDEXSAMPLE_SIZE	640

namespace IGLibrary
{
	class IGFaceDescriptor;

	struct IGSTRUCTELEM_CIRCLE {
		IGSTRUCTELEM_CIRCLE(void*){};
		bool operator()(int nX, int nY, int nRay) {
			return sqrt ((float)(nX*nX + nY*nY)) <= nRay;
		}
	};

	struct IGSTRUCTELEM_ELLIPSE {
		IGSTRUCTELEM_ELLIPSE(void *pCoeffYonX): m_pCoeffYonX(static_cast<float*>(pCoeffYonX)){};
		bool operator()(int nX, int nY, int nRay) {
			return sqrt (((*m_pCoeffYonX)*(*m_pCoeffYonX)*(float)(nX*nX) + (float)(nY*nY))) <= nRay;
		}
		float operator()(int nX, int nY) {
			return sqrt (((*m_pCoeffYonX)*(*m_pCoeffYonX)*(float)(nX*nX) + (float)(nY*nY)));
		}
	private:
		float *m_pCoeffYonX;
	};

	class IGSmartLayer : public CxImage
	{
		friend class IGFaceEffectMorphing;
		friend class IGFaceEffectRedEyeRemove;
		friend class IGFaceEffectChangeEyeColor;
		friend class IGFaceEffectColorize;
	public:
		IGSmartLayer();
		IGSmartLayer(const CxImage& cpy);
		IGSmartLayer(const IGSmartLayer& cpy);
		~IGSmartLayer();

		// basic features
		static bool Init (HRSRC hPaper);
		void UpdateSize (int nWidth, int nHeight);
		inline void AddRegion (IGRegion *pNewRegion);
		inline IGSmartPixel* GetPixel (int nCol, int nRow);		
		bool Dilate(int nSize, bool bSkin = false);
		bool Erode(int nSize, bool bSkin = false);	
		bool Construct4(bool bForce = false);
		bool Construct8(bool bForce = false);
		bool ConstructSaddle();
		IGRegion* GetRegion(int nLabel);
		int GetNbRegions() const  { return m_vRegions.size(); }
		void SelectIndexedRegions (int nIdxReg, BYTE level = 255);
		static bool MergeToSaddleRegion (IGRegion *pBackground, const std::vector <IGRegion*>& vRegions, IGRegion& region, bool bExcludeBackground = false);
		bool MergeToSaddleRegion (IGRegion *pBackground, IGRegion& region, bool bExcludeBackground = false);
		bool GrayScale();
		void CopyFaceDetection(const IGSmartLayer& cpy);
		virtual bool SelectionCopyFromScaledLayer(CxImage& layerScaled);

		// filters
		bool FilterSkin();
		bool FilterSkinUnnoised (bool bSelect = false);
		bool FilterFaceWithEyes (const std::list <std::pair <BYTE, RECT>>& lpairValRects, bool bFinalStep = false);
		static bool FilterSmallRegions (IGRegion *pBackground, std::vector <IGRegion*> vSubdivisions);
		bool FilterSmallRegions(IGRegion *pBackground);
		bool FilterSmallRegions(std::vector <IGRegion*> vRegions);
		void FilterMorphoBackgroundRegions();
		bool ProcessFaceEffect (IGIPFaceEffectMessage *pEffectMessage);
		bool Filter2();
		
		// indexers
		virtual bool IndexLPE (IGMarker **ppMarkers, int& nNbMarkers, bool bShowMarkerMode = false);
		bool IndexFaces (int nDescriptorIdx = -1);
		bool IndexFacenIris (int nDescriptorIdx = -1); // added by TQ
		//bool detectIris(const CxImage& faceImg, RECT eye1, RECT eye2 ); // added by TQ
		//cvPoint getIrisCoor(Mat inputEyeImg); // added by TQ

		bool IndexSkinBinary();
		int DetectFaces();	
		IGFaceDescriptor* GetFaceDescriptor();
		void SetFaceDescriptor (const std::wstring& wsDescriptor);

		bool Resample();
		virtual bool AutoRotate();
		bool SelectionEyesMouth (BYTE level = 255);

		bool SelectEyes (BYTE level = 255);
		bool SelectMouth (BYTE level = 255);
		bool SelectNoze (BYTE level = 255);
		
		template<typename IGSTRUCTELEM_FUNC>
		void DilateRegions(int nSize, void *pArgs){
			IGSTRUCTELEM_FUNC func(pArgs);
			vector <IGSmartPixel*> vBoundaries;
			getRegionBoundaries (vBoundaries);
			long ray = nSize / 2;
			long kMax = nSize - ray;
			IGSmartPixel *pFirstPixel = GetPixels();
			IGSmartPixel *pOverLastPixel = pFirstPixel + head.biWidth * head.biHeight;
			for (vector <IGSmartPixel*>::const_iterator itBoundPixel = vBoundaries.cbegin(); itBoundPixel != vBoundaries.cend(); ++itBoundPixel){			
				int iY = -ray * head.biWidth;
				for(long y = -ray; y < kMax; y++)
				{
					if ((pFirstPixel > (*itBoundPixel) + iY) || ((*itBoundPixel) + iY >= pOverLastPixel)
						|| (((*itBoundPixel)->m_nY + y) < 0) || (((*itBoundPixel)->m_nY + y) >= head.biHeight)){
							iY += head.biWidth;
							continue;
					}
					int iX = iY - ray;
					for(long x = -ray; x < kMax; x++)
					{
						if ((pFirstPixel > ((*itBoundPixel) + iX)) || (((*itBoundPixel) + iX) >= pOverLastPixel)
							|| (((*itBoundPixel)->m_nX + x) < 0) || (((*itBoundPixel)->m_nX + x) >= head.biWidth)) {
								iX++;
								continue;
						}
						if (func(x, y, ray)){
							(*itBoundPixel)[iX++].m_nLabel = (*itBoundPixel)->m_nLabel;
						}
						else{
							iX++;
						}
					}
					iY += head.biWidth;
				}
			}
		}

		template<typename IGSTRUCTELEM_FUNC>
		void ErodeRegions(int nSize, void *pArgs){
			IGSTRUCTELEM_FUNC func(pArgs);
			vector <IGSmartPixel*> vBoundaries;
			getRegionBoundaries (vBoundaries);
			long ray = nSize / 2;
			long kMax = nSize - ray;
			IGSmartPixel *pFirstPixel = GetPixels();
			IGSmartPixel *pOverLastPixel = pFirstPixel + head.biWidth * head.biHeight;
			for (vector <IGSmartPixel*>::const_iterator itBoundPixel = vBoundaries.cbegin(); itBoundPixel != vBoundaries.cend(); ++itBoundPixel){			
				int iY = -ray * head.biWidth;
				for(long y = -ray; y < kMax; y++)
				{
					if ((pFirstPixel > (*itBoundPixel) + iY) || ((*itBoundPixel) + iY >= pOverLastPixel)
						|| (((*itBoundPixel)->m_nY + y) < 0) || (((*itBoundPixel)->m_nY + y) >= head.biHeight)){
							iY += head.biWidth;
							continue;
					}
					int iX = iY - ray;
					for(long x = -ray; x < kMax; x++)
					{
						if ((pFirstPixel > ((*itBoundPixel) + iX)) || (((*itBoundPixel) + iX) >= pOverLastPixel)
							|| (((*itBoundPixel)->m_nX + x) < 0) || (((*itBoundPixel)->m_nX + x) >= head.biWidth)) {
								iX++;
								continue;
						}
						if (func(x, y, ray)){
							(*itBoundPixel)[iX++].m_nLabel = IGMARKER_BACKGROUND;
						}
						else{
							iX++;
						}
					}
					iY += head.biWidth;
				}
			}
		}

		template<typename IGSTRUCTELEM_FUNC>
		void StructuredElement(int nSize, void *pArgs = NULL, bool bInvert = false){
			if (nSize < 0)
				return;
			if (bInvert){
				ErodeRegions <IGSTRUCTELEM_FUNC> (nSize, pArgs);
				DilateRegions <IGSTRUCTELEM_FUNC> (nSize, pArgs);				
			}
			else {
				DilateRegions <IGSTRUCTELEM_FUNC> (nSize, pArgs);
				ErodeRegions <IGSTRUCTELEM_FUNC> (nSize, pArgs);
			}
			// re-index regions
			int nLabel = 0;
			std::map<int,int> mapLabels;
			for (vector <IGRegion*>::const_iterator itReg = m_vRegions.cbegin(); itReg != m_vRegions.cend(); ++itReg){
				mapLabels.insert(pair<int,int>((*itReg)->m_nLabel, nLabel++));
				(*itReg)->Clear();
			}
			IGSmartPixel* pPixels = GetPixels();
			int nNbPixels = head.biWidth * head.biHeight;
			while (nNbPixels--){
				m_vRegions[mapLabels[pPixels->m_nLabel]]->AddPixel(pPixels);
				pPixels++;
			}
		}

		bool ComputeGradient(bool bApplyResultToPixels = false);
		void GetMinGradientPos (int nPosX, int nPosY, int nRadius, int& nResPosX, int& nResPosY);

		virtual void ProgressStepIt();
		virtual void ProgressSetRange (UINT nMax, bool bLock);
		virtual void ProgressSetSubRange (int nCurStartRange, int nNbSubRanges);
		virtual void ProgressSetMessage (wchar_t *pwMessage);
		void ProgressSetRange (UINT nMax) { return ProgressSetRange (nMax, false); }

		IGSmartLayer *m_pLayerSkinFiltered;

	protected:
		inline IGSmartPixel* GetPixels();

		IGFaceDescriptor *m_pFaceDescriptor;

	private:
		void clean();
		void getBoundaries (bool bOver = false, bool bSkin = false);
		void getRegionBoundaries (std::vector <IGSmartPixel*>& vBoundaries);
		bool unnoiseSkin();
		bool LPE_TopDown (int nNbRegions = 10, bool bWaterfall = false);
		bool LPE_FAH (IGMarker **ppMarkers, int& nNbMarkers, bool bShowMarkerMode = false, bool bShowGradient = false);
		int evalDetectFaces(IGFaceDescriptor *pFaceDescriptor);		
		void rotatePt (int nRot, int nPtCtrX, int nPtCtrY, int nPtX, int nPtY, int& nRotatedX, int& nRotatedY);
		void destroyFaceDescriptor();

		bool m_bReconstructionNeeded;
		IGSmartPixel *m_pPixels;
		std::vector <IGRegion*> m_vRegions;
		std::vector <IGSmartPixel*> m_vSkinPixels;
		std::vector <IGSmartPixel*> m_vBoundaries;
		std::vector <std::vector <IGSmartPixel*>> m_vvTreePixels;
		std::vector <std::vector <IGRegion*>> m_vvTreeRegions;
		static IGSmartPtr <IGFrame> mg_spPaper;
	};

	inline void IGSmartLayer::AddRegion (IGRegion *pNewRegion)
	{
		m_vRegions.push_back (pNewRegion);
	}

	inline IGSmartPixel* IGSmartLayer::GetPixels()
	{
		return m_pPixels;
	}

	inline IGSmartPixel* IGSmartLayer::GetPixel (int nCol, int nRow)
	{
		return &m_pPixels [nRow * head.biWidth + nCol];
	}
}