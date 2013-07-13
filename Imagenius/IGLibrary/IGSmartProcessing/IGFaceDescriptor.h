#pragma once
#include "IGSmartLayer.h"
#include "FaceDetector.h"
#include "clipper.hpp"
#include "area.h"
#include <list>

#define IGFACEDESCRIPTOR_MINSCORE	7

namespace IGLibrary
{
	typedef void (__cdecl *PDETECTFACES)(const wchar_t* pcwPictureId, const wchar_t* pcwPictureName, int rotationIndex);	

	class FaceList{
public:
		std::list <Face> _lFaces;

		int getScore() const {
			int nScore = 0;
			for (std::list <Face>::const_iterator it = _lFaces.begin(); it != _lFaces.end(); ++it){
				nScore += (*it).getScore();
			}
			return nScore;
		}

		void remove(const FaceList& faceList) {
			for (std::list <Face>::const_iterator it = faceList._lFaces.begin(); it != faceList._lFaces.end(); ++it)
				_lFaces.remove(*it);
		}

		bool faceExists (int nWidth, int nHeight, const Face& faceCmp, FaceList& facesExisting) const {
			RECT rcFaceCmp = faceCmp.getFaceCoords();
			rcFaceCmp.left = min (faceCmp.getEyeLeftCoords().left, faceCmp.getEyeRightCoords().left);
			rcFaceCmp.right = max (faceCmp.getEyeLeftCoords().right, faceCmp.getEyeRightCoords().right);
			int nBottom = rcFaceCmp.bottom;
			rcFaceCmp.bottom = rcFaceCmp.top;
			int nCmpRot = faceCmp.getRotation();
			RECT rcRotTopLeftTopRightCmp = Face::Rotate2Points (nWidth / 2, nHeight / 2, rcFaceCmp, -nCmpRot);
			rcFaceCmp.top = nBottom;
			rcFaceCmp.bottom = nBottom;
			RECT rcRotBottomLeftBottomRightCmp = Face::Rotate2Points (nWidth / 2, nHeight / 2, rcFaceCmp, -nCmpRot);

			ClipperLib::Polygons subj(1);
			subj[0].push_back (ClipperLib::IntPoint(rcRotTopLeftTopRightCmp.left,rcRotTopLeftTopRightCmp.top));
			subj[0].push_back (ClipperLib::IntPoint(rcRotTopLeftTopRightCmp.right,rcRotTopLeftTopRightCmp.bottom));
			subj[0].push_back (ClipperLib::IntPoint(rcRotBottomLeftBottomRightCmp.right,rcRotBottomLeftBottomRightCmp.bottom));
			subj[0].push_back (ClipperLib::IntPoint(rcRotBottomLeftBottomRightCmp.left,rcRotBottomLeftBottomRightCmp.top));

			double* pSubjX = new double[4];
			double* pSubjY = new double[4];
			int nIdxPt = 0;
			for (vector <ClipperLib::IntPoint>::const_iterator itPt = subj[0].cbegin(); itPt != subj[0].cend(); ++itPt, nIdxPt++){
				pSubjX [nIdxPt] = (double)(*itPt).X;
				pSubjY [nIdxPt] = (double)(*itPt).Y;
			}
			bool bRet = false;
			for (list <Face>::const_iterator itFaceTest = _lFaces.begin(); itFaceTest != _lFaces.end(); ++itFaceTest){
				RECT rcFace = (*itFaceTest).getFaceCoords();
				rcFace.left = min ((*itFaceTest).getEyeLeftCoords().left, (*itFaceTest).getEyeRightCoords().left);
				rcFace.right = max ((*itFaceTest).getEyeLeftCoords().right, (*itFaceTest).getEyeRightCoords().right);
				nBottom = rcFace.bottom;
				rcFace.bottom = rcFace.top;
				int nRot = (*itFaceTest).getRotation();
				RECT rcRotTopLeftTopRightCmpIntersect = Face::Rotate2Points (nWidth / 2, nHeight / 2, rcFace, -nRot);
				rcFace.top = nBottom;
				rcFace.bottom = nBottom;
				RECT rcRotBottomLeftBottomRightCmpIntersect = Face::Rotate2Points (nWidth / 2, nHeight / 2, rcFace, -nRot);		

				ClipperLib::Polygons clip(1), solution;
				clip[0].push_back (ClipperLib::IntPoint(rcRotTopLeftTopRightCmpIntersect.left,rcRotTopLeftTopRightCmpIntersect.top));
				clip[0].push_back (ClipperLib::IntPoint(rcRotTopLeftTopRightCmpIntersect.right,rcRotTopLeftTopRightCmpIntersect.bottom));
				clip[0].push_back (ClipperLib::IntPoint(rcRotBottomLeftBottomRightCmpIntersect.right,rcRotBottomLeftBottomRightCmpIntersect.bottom));
				clip[0].push_back (ClipperLib::IntPoint(rcRotBottomLeftBottomRightCmpIntersect.left,rcRotBottomLeftBottomRightCmpIntersect.top));
				ClipperLib::Clipper c;
				c.AddPolygons (subj, ClipperLib::ptSubject);
				c.AddPolygons (clip, ClipperLib::ptClip);
				c.Execute (ClipperLib::ctIntersection, solution, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
				if (solution.size() == 0)
					continue;	
				double* pClipX = new double[4];
				double* pClipY = new double[4];
				nIdxPt = 0;
				for (vector <ClipperLib::IntPoint>::const_iterator itPt = clip[0].cbegin(); itPt != clip[0].cend(); ++itPt, nIdxPt++){
					pClipX [nIdxPt] = (double)(*itPt).X;
					pClipY [nIdxPt] = (double)(*itPt).Y;
				}
				double* pSolutionPolygonX = new double[solution[0].size()];
				double* pSolutionPolygonY = new double[solution[0].size()];
				nIdxPt = 0;
				for (vector <ClipperLib::IntPoint>::const_iterator itPt = solution[0].cbegin(); itPt != solution[0].cend(); ++itPt, nIdxPt++){
					pSolutionPolygonX [nIdxPt] = (double)(*itPt).X;
					pSolutionPolygonY [nIdxPt] = (double)(*itPt).Y;
				}
				double dArea = abs (polygonArea (pSolutionPolygonX, pSolutionPolygonY, solution[0].size()));
				double dSubjArea = abs (polygonArea (pSubjX, pSubjY, subj[0].size()));
				double dClipArea = abs (polygonArea (pClipX, pClipY, clip[0].size()));
				delete [] pClipX;
				delete [] pClipY;
				delete [] pSolutionPolygonX;
				delete [] pSolutionPolygonY;		
				if ((dArea > dSubjArea / 2)
					|| (dArea > dClipArea / 2)){
					bRet = true;
					facesExisting._lFaces.push_back (*itFaceTest);
				}
		
			}
			delete [] pSubjX;
			delete [] pSubjY;
			return bRet;
		}
	};

	class IGFaceDescriptor : public IGLibrary::IGConvertible {
	public:
		IGFaceDescriptor();
		IGFaceDescriptor(const IGSmartLayer& smartLayer, const std::wstring& wsDescriptor);

		virtual bool ConvertTo(std::wstring& wsDescriptor) const;

		operator const int() const
		{
			return -1;
		}

		operator std::wstring() const
		{
			std::wstring wsDescriptor;
			ConvertTo(wsDescriptor);
			return wsDescriptor;
		}

		IGFaceDescriptor& operator = (const IGFaceDescriptor& descr);

		void CopyFrom (const IGConvertible& conv)
		{
			const IGFaceDescriptor& descr = dynamic_cast <const IGFaceDescriptor&> (conv);
			operator = (descr);
		}

		int Detect(int nLayerWidth, int nLayerHeight, IGSmartLayer& sampledLayer, bool bRetry = false);
		void GetFaces (std::list <Face>& lFaces);
		Face GetFace (int idxFace);
		void Rotate (int nRotate);
		void Rotate();
		void UnRotate();
		void Scale (int nWidth, int nHeight, int nLayerWidth, int nLayerHeight);
		void Offset (int nX, int nY);
		int GetWidth(){
			return m_nWidth;
		}
		int GetHeight(){
			return m_nHeight;
		}

	private:
		void filterSmallAndRotatedFaces();
		bool filterSharedEyes();
		void filterPoorScores();
		void correctFaces();
		int evalDetectFaces (const std::wstring& wsPictureId, int idxRot);
		bool faceExists (const Face& faceCmp) const;
		PROCESS_INFORMATION launchDetectProcess(const std::wstring& wsPictureId, const std::wstring& wsPictureName, int nRot);
		void launchDetectIce(const std::wstring& wsPictureId, const std::wstring& wsPictureName, int nRot);
		void getPermutations (int offset, std::vector <Face>& combinations, std::list <FaceList>& lFl);

		std::list <Face> m_lFaces;
		std::vector <Face> m_vFaces;
		int m_nWidth;
		int m_nHeight;
		int m_nLayerWidth;
		int m_nLayerHeight;
		PDETECTFACES m_pDetectFaces;
	};	
}