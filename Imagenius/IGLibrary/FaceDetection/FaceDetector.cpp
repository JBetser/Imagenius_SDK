#include "stdafx.h"
#include "facedetector.h"
#include "cv.h"
#include "ximage.h"
#include <stdio.h>

CvHaarClassifierCascade *cascade_f = NULL;
CvHaarClassifierCascade *cascade_el = NULL;
CvHaarClassifierCascade *cascade_er = NULL;
CvHaarClassifierCascade *cascade_m = NULL;
CvHaarClassifierCascade *cascade_n = NULL;

using namespace std;

void init()
{
#ifdef DEBUG
	char *file1 = "C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\ANN\\haarcascade_frontalface_alt.xml";
	char *file2 = "C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\ANN\\haarcascade_mcs_lefteye.xml";
	char *file3 = "C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\ANN\\haarcascade_mcs_righteye.xml";
	char *file4 = "C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\ANN\\Mouth.xml";
	char *file5 = "C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\ANN\\Nariz.xml";
#else
	char *file1 = "C:\\Imagenius\\IGServer\\IGFaceDetector\\ANN\\haarcascade_frontalface_alt.xml";
	char *file2 = "C:\\Imagenius\\IGServer\\IGFaceDetector\\ANN\\haarcascade_mcs_lefteye.xml";
	char *file3 = "C:\\Imagenius\\IGServer\\IGFaceDetector\\ANN\\haarcascade_mcs_righteye.xml";
	char *file4 = "C:\\Imagenius\\IGServer\\IGFaceDetector\\ANN\\Mouth.xml";
	char *file5 = "C:\\Imagenius\\IGServer\\IGFaceDetector\\ANN\\Nariz.xml";
#endif
	/* load the face classifier */
	cascade_f = (CvHaarClassifierCascade*)cvLoad(file1, 0, 0, 0);

	/* load the eye classifier */
	cascade_el = (CvHaarClassifierCascade*)cvLoad(file2, 0, 0, 0);
	cascade_er = (CvHaarClassifierCascade*)cvLoad(file3, 0, 0, 0);

	/* load the mouth classifier */
	cascade_m = (CvHaarClassifierCascade*)cvLoad(file4, 0, 0, 0);

	/* load the noze classifier */
	cascade_n = (CvHaarClassifierCascade*)cvLoad(file5, 0, 0, 0);
}

bool eye_match(CvRect* pLeft, CvRect* pRight)
{
	if (pRight->x > pLeft->x + pLeft->width){
		if (pRight->x < pLeft->x + 3 * pLeft->width){
			if (abs (pRight->y - pLeft->y) < min (pLeft->height, pRight->height) / 2 &&
				abs (pLeft->width - pRight->width) < max (pLeft->width, pRight->width) / 2 &&
				abs (pLeft->height - pRight->height) < max (pLeft->height, pRight->height) / 2){
				return true;
			}
		}
	}
	return false;
}

void detect_eyes(CvMemStorage	*storage, IplImage* img, std::list <std::pair <RECT, RECT>>& lpairEyes)
{
	/* reset buffer for the next object detection */
	cvClearMemStorage (storage);

	/* Detect objects */
	CvSeq* pEyesLeft = cvHaarDetectObjects(
		img,
		cascade_el,
		storage,
		1.2,
		2,
		CV_HAAR_DO_CANNY_PRUNING,
		cvSize(40, 20)
		);
	CvSeq cvSeqEyeLeft (*pEyesLeft);
	cvClearMemStorage (storage);

	CvSeq* pEyesRight = cvHaarDetectObjects(
		img,
		cascade_er,
		storage,
		1.2,
		2,
		CV_HAAR_DO_CANNY_PRUNING,
		cvSize(40, 20)
		);
	CvSeq cvSeqEyeRight (*pEyesRight);

	std::list <CvRect*> lRectsEyeLeft;
	for(int i = 0; i < (pEyesLeft ? pEyesLeft->total : 0); i++ ) {
		bool bFound = false;
		CvRect* pRc = (CvRect*)cvGetSeqElem(&cvSeqEyeLeft, i);
		if (!pRc)
			continue;
		for(std::list <CvRect*>::iterator itEye = lRectsEyeLeft.begin(); itEye != lRectsEyeLeft.end(); ++itEye){
			RECT rcInt;
			rcInt.left = max ((int)(*itEye)->x, (int)pRc->x);
			rcInt.top = max ((int)(*itEye)->y, (int)pRc->y);
			rcInt.right = min ((int)((*itEye)->x + (*itEye)->width), (int)(pRc->x + pRc->width));
			rcInt.bottom = min ((int)((*itEye)->y + (*itEye)->height), (int)(pRc->y + pRc->height));			
			if ((rcInt.left < rcInt.right) && (rcInt.top < rcInt.bottom)){
				bFound = true;
				break;
			}
		}
		if (!bFound)
			lRectsEyeLeft.push_back (pRc);
	}
	std::list <CvRect*> lRectsEyeRight;
	for(int i = 0; i < (pEyesRight ? pEyesRight->total : 0); i++ ) {
		bool bFound = false;
		CvRect* pRc = (CvRect*)cvGetSeqElem(&cvSeqEyeRight, i);
		if (!pRc)
			continue;
		for(std::list <CvRect*>::iterator itEye = lRectsEyeRight.begin(); itEye != lRectsEyeRight.end(); ++itEye){
			RECT rcInt;
			rcInt.left = max ((int)(*itEye)->x, (int)pRc->x);
			rcInt.top = max ((int)(*itEye)->y, (int)pRc->y);
			rcInt.right = min ((int)((*itEye)->x + (*itEye)->width), (int)(pRc->x + pRc->width));
			rcInt.bottom = min ((int)((*itEye)->y + (*itEye)->height), (int)(pRc->y + pRc->height));			
			if ((rcInt.left < rcInt.right) && (rcInt.top < rcInt.bottom)){
				bFound = true;
				break;
			}
		}
		if (!bFound)
			lRectsEyeRight.push_back (pRc);
	}

	RECT rcNo = {-1,-1,-1,-1};
	for(std::list <CvRect*>::iterator itEyeLeft = lRectsEyeLeft.begin(); itEyeLeft != lRectsEyeLeft.end(); ++itEyeLeft){
		CvRect* rl = *itEyeLeft;
		RECT rcEyeLeft = rcNo;
		RECT rcEyeRight = rcNo;
		for(std::list <CvRect*>::iterator itEyeRight = lRectsEyeRight.begin(); itEyeRight != lRectsEyeRight.end(); ++itEyeRight){
			CvRect* rr = *itEyeRight;
			if (eye_match(rl,rr)){
				rcEyeLeft.left = img->width - rl->x - rl->width; rcEyeLeft.right = img->width - rl->x;
				rcEyeLeft.top = img->height - rl->y - rl->height; rcEyeLeft.bottom = img->height - rl->y;
				rcEyeRight.left = img->width - rr->x - rr->width; rcEyeRight.right = img->width - rr->x;
				rcEyeRight.top = img->height - rr->y - rr->height; rcEyeRight.bottom = img->height - rr->y;
				lpairEyes.push_back(pair<RECT,RECT> (rcEyeLeft, rcEyeRight));
			}
			else if (eye_match(rr,rl)){
				rcEyeLeft.left = img->width - rl->x - rl->width; rcEyeLeft.right = img->width - rl->x;
				rcEyeLeft.top = img->height - rl->y - rl->height; rcEyeLeft.bottom = img->height - rl->y;
				rcEyeRight.left = img->width - rr->x - rr->width; rcEyeRight.right = img->width - rr->x;
				rcEyeRight.top = img->height - rr->y - rr->height; rcEyeRight.bottom = img->height - rr->y;
				lpairEyes.push_back(pair<RECT,RECT> (rcEyeLeft, rcEyeRight));
			}
			else{
				rcEyeLeft.left = img->width - rl->x - rl->width; rcEyeLeft.right = img->width - rl->x;
				rcEyeLeft.top = img->height - rl->y - rl->height; rcEyeLeft.bottom = img->height - rl->y;
				rcEyeRight = rcNo;
			}
		}
		if ((rcEyeRight.left == rcNo.left) && (rcEyeRight.right == rcNo.right) && (rcEyeRight.top == rcNo.top) && (rcEyeRight.bottom == rcNo.bottom))
			lpairEyes.push_back(pair<RECT,RECT> (rcEyeLeft, rcEyeRight));
	}
	for(std::list <CvRect*>::iterator itEye = lRectsEyeRight.begin(); itEye != lRectsEyeRight.end(); ++itEye){
		RECT rcEyeRight; rcEyeRight.left = img->width - (*itEye)->x - (*itEye)->width; rcEyeRight.right = img->width - (*itEye)->x;
		rcEyeRight.top = img->height - (*itEye)->y - (*itEye)->height; rcEyeRight.bottom = img->height - (*itEye)->y;
		lpairEyes.push_back(pair<RECT,RECT> (rcNo, rcEyeRight));
	}
}

void match_eyes_with_face(const std::list <std::pair <RECT, RECT>>& lpairEyes, const CvRect& rcFace, std::pair <RECT, RECT>& pairEyes)
{	
	RECT rcNo = {-1,-1,-1,-1};
	pairEyes = pair<RECT,RECT> (rcNo, rcNo);
	pair<RECT,RECT> pairEyesFullMatch = pair<RECT,RECT> (rcNo, rcNo);
	bool bFullMatch = false;
	for(std::list <std::pair <RECT, RECT>>::const_iterator itPairEyes = lpairEyes.begin(); itPairEyes != lpairEyes.end(); ++itPairEyes){
		RECT rcIntersecLeft,rcIntersecRight;
		int nLeftEyeWidth = (*itPairEyes).first.right - (*itPairEyes).first.left;		
		int nRightEyeWidth = (*itPairEyes).second.right - (*itPairEyes).second.left;
		if ((nLeftEyeWidth > rcFace.width / 2) || (nRightEyeWidth > rcFace.width / 2))
			continue;
		rcIntersecLeft.left = max ((int)(*itPairEyes).first.left, (int)rcFace.x);
		rcIntersecLeft.top = max ((int)(*itPairEyes).first.top, (int)rcFace.y);
		rcIntersecLeft.right = min ((int)(*itPairEyes).first.right, (int)(rcFace.x + rcFace.width));
		rcIntersecLeft.bottom = min ((int)(*itPairEyes).first.bottom, (int)(rcFace.y + rcFace.height));
		rcIntersecRight.left = max ((int)(*itPairEyes).second.left, (int)rcFace.x);
		rcIntersecRight.top = max ((int)(*itPairEyes).second.top, (int)rcFace.y);
		rcIntersecRight.right = min ((int)(*itPairEyes).second.right, (int)(rcFace.x + rcFace.width));
		rcIntersecRight.bottom = min ((int)(*itPairEyes).second.bottom, (int)(rcFace.y + rcFace.height));			
		if ((rcIntersecLeft.left < rcIntersecLeft.right) && (rcIntersecLeft.top < rcIntersecLeft.bottom) && (rcIntersecLeft.bottom > (2*rcFace.y + rcFace.height) / 2)){
			pairEyes = *itPairEyes;
			if ((rcIntersecRight.left < rcIntersecRight.right) && (rcIntersecRight.top < rcIntersecRight.bottom) && (rcIntersecRight.bottom > (2*rcFace.y + rcFace.height) / 2)){
				pairEyesFullMatch = *itPairEyes;
				bFullMatch = true;
				if ((rcIntersecLeft.right - rcIntersecLeft.left > nLeftEyeWidth / 2) &&
					(rcIntersecRight.right - rcIntersecRight.left > nRightEyeWidth / 2))
					break;
			}
		}
		else if ((rcIntersecRight.left < rcIntersecRight.right) && (rcIntersecRight.top < rcIntersecRight.bottom) && (rcIntersecRight.bottom > (2*rcFace.y + rcFace.height) / 2)){
			pairEyes = *itPairEyes;
		}
	}
	if (bFullMatch)
		pairEyes = pairEyesFullMatch;
}

void detect_mouths(CvMemStorage	*storage, IplImage* img, std::list <RECT>& lRectsMouth)
{
	cvClearMemStorage (storage);
	/* Detect objects */
	CvSeq* pMouth = cvHaarDetectObjects(
		img,
		cascade_m,
		storage,
		1.2,
		2,
		CV_HAAR_DO_CANNY_PRUNING,
		cvSize(25, 15)
		);
	CvSeq cvSeqMouth (*pMouth);
	
	std::list <CvRect*> lCvRectsMouth;
	for(int i = 0; i < (pMouth ? pMouth->total : 0); i++ ) {
		bool bFound = false;
		CvRect* pRectMouth = (CvRect*)cvGetSeqElem(&cvSeqMouth, i);
		if (!pRectMouth)
			continue;
		for(std::list <CvRect*>::iterator itMouth = lCvRectsMouth.begin(); itMouth != lCvRectsMouth.end(); ++itMouth){
			RECT rcInt;
			rcInt.left = max ((int)(*itMouth)->x, (int)pRectMouth->x);
			rcInt.top = max ((int)(*itMouth)->y, (int)pRectMouth->y);
			rcInt.right = min ((int)((*itMouth)->x + (*itMouth)->width), (int)(pRectMouth->x + pRectMouth->width));
			rcInt.bottom = min ((int)((*itMouth)->y + (*itMouth)->height), (int)(pRectMouth->y + pRectMouth->height));			
			if ((rcInt.left < rcInt.right) && (rcInt.top < rcInt.bottom)){
				bFound = true;
				break;
			}
		}
		if (!bFound){
			RECT rcMouthToAdd;
			rcMouthToAdd.left = img->width - pRectMouth->x - pRectMouth->width; rcMouthToAdd.right = img->width - pRectMouth->x;
			rcMouthToAdd.top = img->height - pRectMouth->y - pRectMouth->height; rcMouthToAdd.bottom = img->height - pRectMouth->y;
			lRectsMouth.push_back (rcMouthToAdd);
			lCvRectsMouth.push_back (pRectMouth);
		}
	}
}

void match_mouths_with_face(const std::list <RECT>& lRectsMouth, const CvRect& rcFace, pair <RECT, RECT> pairEyes, RECT& rcMouth)
{
	RECT rcNo = {-1,-1,-1,-1};
	rcMouth = rcNo;
	if (pairEyes.first.left != -1 && pairEyes.first.right != -1){	
		int nHigherEyeBottom = min (pairEyes.first.top, pairEyes.second.top);
		int nHighestMouthTop = nHigherEyeBottom - (int)(0.75f * max (pairEyes.first.bottom - pairEyes.first.top, pairEyes.second.bottom - pairEyes.second.top));
		int nLowestMouthBottom = nHigherEyeBottom - 3 * max (pairEyes.first.bottom - pairEyes.first.top, pairEyes.second.bottom - pairEyes.second.top);
		int nMaxEyeSize = max (pairEyes.first.right - pairEyes.first.left, pairEyes.second.right - pairEyes.second.left);
		for(std::list <RECT>::const_iterator itMouth = lRectsMouth.begin(); itMouth != lRectsMouth.end(); ++itMouth){
			RECT rcIntersecMouth;
			rcIntersecMouth.left = max ((int)(*itMouth).left, (int)rcFace.x);
			rcIntersecMouth.top = max ((int)(*itMouth).top, (int)rcFace.y);
			rcIntersecMouth.right = min ((int)(*itMouth).right, (int)(rcFace.x + rcFace.width));
			rcIntersecMouth.bottom = min ((int)(*itMouth).bottom, (int)(rcFace.y + rcFace.height));	
			int nMidMouth = ((int)(*itMouth).left + (int)(*itMouth).right) / 2;
			if ((rcIntersecMouth.left < rcIntersecMouth.right) && (rcIntersecMouth.top < rcIntersecMouth.bottom) 
				&& (rcIntersecMouth.bottom < (2*rcFace.y + rcFace.height) / 2) && (nLowestMouthBottom < (int)(*itMouth).top) && (nHighestMouthTop > (int)(*itMouth).bottom)
				&& (nMidMouth > rcFace.x) && (nMidMouth < rcFace.x + rcFace.width) && (((int)(*itMouth).right - (int)(*itMouth).left) > nMaxEyeSize) 
				&& ((*itMouth).top > rcFace.y) && ((*itMouth).bottom < nHigherEyeBottom)){
				rcMouth = *itMouth;
				break;
			}
		}	
	}
	else{
		for(std::list <RECT>::const_iterator itMouth = lRectsMouth.begin(); itMouth != lRectsMouth.end(); ++itMouth){
			RECT rcIntersecMouth;
			rcIntersecMouth.left = max ((int)(*itMouth).left, (int)rcFace.x);
			rcIntersecMouth.top = max ((int)(*itMouth).top, (int)rcFace.y);
			rcIntersecMouth.right = min ((int)(*itMouth).right, (int)(rcFace.x + rcFace.width));
			rcIntersecMouth.bottom = min ((int)(*itMouth).bottom, (int)(rcFace.y + rcFace.height));	
			int nMidMouth = ((int)(*itMouth).left + (int)(*itMouth).right) / 2;
			if ((rcIntersecMouth.left < rcIntersecMouth.right) 
				&& (rcIntersecMouth.bottom < (2*rcFace.y + rcFace.height) / 2)
				&& (nMidMouth > rcFace.x) && (nMidMouth < rcFace.x + rcFace.width)  && ((*itMouth).top > rcFace.y)
				&& (((int)(*itMouth).right - (int)(*itMouth).left) > rcFace.width / 8)){
				rcMouth = *itMouth;
				break;
			}
		}
	}
}

void detect_noses (CvMemStorage	*storage, IplImage* img, std::list <RECT>& lRectsNoze)
{
	cvClearMemStorage (storage);
	/* Detect objects */
	CvSeq* pNoze = cvHaarDetectObjects(
		img,
		cascade_n,
		storage,
		1.2,
		2,
		CV_HAAR_DO_CANNY_PRUNING,
		cvSize(25, 15)
		);
	CvSeq cvSeqNoze (*pNoze);

	std::list <CvRect*> lCvRectsNoze;
	for(int i = 0; i < (pNoze ? pNoze->total : 0); i++ ) {
		bool bFound = false;
		CvRect* pRectNoze = (CvRect*)cvGetSeqElem(&cvSeqNoze, i);
		if (!pRectNoze)
			continue;
		for(std::list <CvRect*>::iterator itNoze = lCvRectsNoze.begin(); itNoze != lCvRectsNoze.end(); ++itNoze){
			RECT rcInt;
			rcInt.left = max ((int)(*itNoze)->x, (int)pRectNoze->x);
			rcInt.top = max ((int)(*itNoze)->y, (int)pRectNoze->y);
			rcInt.right = min ((int)((*itNoze)->x + (*itNoze)->width), (int)(pRectNoze->x + pRectNoze->width));
			rcInt.bottom = min ((int)((*itNoze)->y + (*itNoze)->height), (int)(pRectNoze->y + pRectNoze->height));			
			if ((rcInt.left < rcInt.right) && (rcInt.top < rcInt.bottom)){
				bFound = true;
				break;
			}
		}
		if (!bFound){
			RECT rcNozeToAdd;
			rcNozeToAdd.left = img->width - pRectNoze->x - pRectNoze->width; rcNozeToAdd.right = img->width - pRectNoze->x;
			rcNozeToAdd.top = img->height - pRectNoze->y - pRectNoze->height; rcNozeToAdd.bottom = img->height - pRectNoze->y;
			lRectsNoze.push_back (rcNozeToAdd);
			lCvRectsNoze.push_back (pRectNoze);
		}
	}
}

void match_noses_with_face (const std::list <RECT>& lRectsNoze, const CvRect& rcFace, pair <RECT, RECT> pairEyes, const RECT& rcMouth, RECT& rcNoze)
{
	RECT rcNo = {-1,-1,-1,-1};
	rcNoze = rcNo;
	int nHighestNozeTop = max (pairEyes.first.bottom + pairEyes.first.top, pairEyes.second.bottom + pairEyes.second.top) / 2;
	for(std::list <RECT>::const_iterator itNoze = lRectsNoze.begin(); itNoze != lRectsNoze.end(); ++itNoze){
		RECT rcIntersecNoze;
		rcIntersecNoze.left = max ((int)(*itNoze).left, (int)rcFace.x);
		rcIntersecNoze.top = max ((int)(*itNoze).top, (int)rcFace.y);
		rcIntersecNoze.right = min ((int)(*itNoze).right, (int)(rcFace.x + rcFace.width));
		rcIntersecNoze.bottom = min ((int)(*itNoze).bottom, (int)(rcFace.y + rcFace.height));		
		if ((rcIntersecNoze.left < rcIntersecNoze.right) && (rcIntersecNoze.top < rcIntersecNoze.bottom) 
			&& (rcIntersecNoze.top > (rcMouth.top + rcMouth.bottom) / 2) && (nHighestNozeTop > (int)(*itNoze).bottom) 
			&& (min (pairEyes.first.left, pairEyes.second.left) < (*itNoze).left) && (max (pairEyes.first.right, pairEyes.second.right) > (*itNoze).right)){
			rcNoze = *itNoze;
			break;
		}
	}
}

void Face::adjustRectFaces(std::list <Face>& lFaces)
{
	RECT rcNo = {-1,-1,-1,-1};
	for (list <Face>::iterator itFaceDetect = lFaces.begin(); itFaceDetect != lFaces.end(); ++itFaceDetect){
		if ((*itFaceDetect).m_rcFace.left != -1) {
			if (((*itFaceDetect).m_pairEyes.first.left != -1) && ((*itFaceDetect).m_pairEyes.second.left != -1)){
				double dWidthFace = max ((*itFaceDetect).m_pairEyes.first.right, (*itFaceDetect).m_pairEyes.second.right) - 
									min ((*itFaceDetect).m_pairEyes.first.left, (*itFaceDetect).m_pairEyes.second.left);
				dWidthFace = (dWidthFace * 8.0) / 7.0;
				double dCenterX = min ((*itFaceDetect).m_pairEyes.first.right, (*itFaceDetect).m_pairEyes.second.right) + (max ((*itFaceDetect).m_pairEyes.first.left, (*itFaceDetect).m_pairEyes.second.left) - 
									min ((*itFaceDetect).m_pairEyes.first.right, (*itFaceDetect).m_pairEyes.second.right)) / 2;
				(*itFaceDetect).m_rcFace.left = dCenterX - dWidthFace / 2.0;
				(*itFaceDetect).m_rcFace.right = dCenterX + dWidthFace / 2.0;

				if ((*itFaceDetect).m_rcMouth.left != -1){
					int nMidMouth = ((int)(*itFaceDetect).m_rcMouth.left + (int)(*itFaceDetect).m_rcMouth.right) / 2;
					if ((nMidMouth <= (*itFaceDetect).m_rcFace.left) || (nMidMouth >= (*itFaceDetect).m_rcFace.right))
						(*itFaceDetect).m_rcMouth = rcNo;
				}			

				if ((*itFaceDetect).m_rcMouth.left != -1){
					double dHeightFace = max ((*itFaceDetect).m_pairEyes.first.bottom, (*itFaceDetect).m_pairEyes.second.bottom) - 
											(*itFaceDetect).m_rcMouth.top;
					dHeightFace = (dHeightFace * 5.0) / 3.0;
					double dCenterY = min ((*itFaceDetect).m_pairEyes.first.top, (*itFaceDetect).m_pairEyes.second.top);
					(*itFaceDetect).m_rcFace.top = dCenterY - dHeightFace / 2.0;
					(*itFaceDetect).m_rcFace.bottom = dCenterY + dHeightFace / 2.0;
					(*itFaceDetect).m_rcFace.top = min ((*itFaceDetect).m_rcFace.top, (*itFaceDetect).m_rcMouth.top - ((*itFaceDetect).m_rcMouth.bottom - (*itFaceDetect).m_rcMouth.top) / 2);
				}
			}
			else{
				RECT rcMouth = ((*itFaceDetect).m_rcMouth.left == -1 ? (*itFaceDetect).getDefaultMouth() : (*itFaceDetect).getMouthCoords());
				int nMidMouth = ((int)rcMouth.left + (int)rcMouth.right) / 2;
				int nHigherEyeBottom = (int)(min ((*itFaceDetect).m_pairEyes.first.top, (*itFaceDetect).m_pairEyes.second.top));
				int nHighestMouthTop = nHigherEyeBottom - (int)(0.75f * max ((*itFaceDetect).m_pairEyes.first.bottom - (*itFaceDetect).m_pairEyes.first.top, (*itFaceDetect).m_pairEyes.second.bottom - (*itFaceDetect).m_pairEyes.second.top));
				int nLowestMouthBottom = (int)(nHigherEyeBottom - 3 * max ((*itFaceDetect).m_pairEyes.first.bottom - (*itFaceDetect).m_pairEyes.first.top, (*itFaceDetect).m_pairEyes.second.bottom - (*itFaceDetect).m_pairEyes.second.top));
				int nMaxEyeSize = (int)(max ((*itFaceDetect).m_pairEyes.first.right - (*itFaceDetect).m_pairEyes.first.left, (*itFaceDetect).m_pairEyes.second.right - (*itFaceDetect).m_pairEyes.second.left));
				RECT rcIntersecMouth;
				rcIntersecMouth.left = max ((int)rcMouth.left, (int)(*itFaceDetect).m_rcFace.left);
				rcIntersecMouth.top = max ((int)rcMouth.top, (int)(*itFaceDetect).m_rcFace.top);
				rcIntersecMouth.right = min ((int)rcMouth.right, (int)((*itFaceDetect).m_rcFace.right));
				rcIntersecMouth.bottom = min ((int)rcMouth.bottom, (int)((*itFaceDetect).m_rcFace.bottom));	
				if (!((rcIntersecMouth.left < rcIntersecMouth.right) && (rcIntersecMouth.top < rcIntersecMouth.bottom) 
					&& (rcIntersecMouth.bottom < ((*itFaceDetect).m_rcFace.top + (*itFaceDetect).m_rcFace.bottom) / 2) && (nLowestMouthBottom < (int)rcMouth.top) && (nHighestMouthTop > (int)rcMouth.bottom)
					&& (nMidMouth > (*itFaceDetect).m_rcFace.left) && (nMidMouth < (*itFaceDetect).m_rcFace.right) && (((int)rcMouth.right - (int)rcMouth.left) > nMaxEyeSize) 
					&& (rcMouth.top > (*itFaceDetect).m_rcFace.top) && (rcMouth.bottom < nHigherEyeBottom))){
					(*itFaceDetect).m_pairEyes.first = rcNo;
					(*itFaceDetect).m_pairEyes.second = rcNo;
				}			
			}
		}
	}
}

// return number of faces and score
void detectFaces(const CxImage& img, std::list <Face>& lFaces, int nRot)
{
	if (cascade_el == NULL || cascade_er == NULL || cascade_f == NULL || cascade_m == NULL || cascade_n == NULL)
		init();
	CxImage imgRotated (img);
	imgRotated.Rotate180();
	IplImage *pIplImage = cvCreateImageHeader (cvSize (img.GetWidth(), img.GetHeight()), 8, img.GetBpp() / 8);
	pIplImage->imageDataOrigin = (char*)imgRotated.GetDIB();
	pIplImage->imageData = (char*)imgRotated.GetBits();

	CvMemStorage			*storage;

	/* setup memory storage, needed by the object detector */
	storage = cvCreateMemStorage(0);

	assert(cascade_f && cascade_el && cascade_er && cascade_m && cascade_n && storage);

	/* haar detection for faces */
	CvSeq *pFaces = cvHaarDetectObjects(
		pIplImage, cascade_f, storage,
		1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize( 40, 40 ) );
	if (pFaces->total == 0)
		return;

	CvSeq seqFaces (*pFaces);
	list <CvRect> lrFaces;
	for(int idxFace = 0; idxFace < seqFaces.total; idxFace++ ) {
		CvRect *r = (CvRect*)cvGetSeqElem(&seqFaces, idxFace);
		if (r)
			lrFaces.push_back (*r);
	}

	/* haar detection for eyes */
	list<pair <RECT, RECT>> lpairEyes;
	detect_eyes (storage, pIplImage, lpairEyes);
	
	/* haar detection for mouths */
	list<RECT> lMouths;
	detect_mouths (storage, pIplImage, lMouths);
	
	/* haar detection for noses */
	list<RECT> lNoses;
	detect_noses (storage, pIplImage, lNoses);
	
	for(list <CvRect>::iterator itFace = lrFaces.begin(); itFace != lrFaces.end(); ++itFace) {
		CvRect r (*itFace);
		CvRect rFace (*itFace);
		rFace.x = img.GetWidth() - r.x - r.width;
		rFace.y = img.GetHeight() - r.y - r.height;
		RECT rcFace; 
		rcFace.left = img.GetWidth() - r.x - r.width - 10; rcFace.right = img.GetWidth() - r.x + 10;
		rcFace.top = img.GetHeight() - r.y - r.height - 10; rcFace.bottom = img.GetHeight() - r.y + 10;

		/* detect current eyes */
		pair <RECT, RECT> pairEyes;
		match_eyes_with_face (lpairEyes, rFace, pairEyes);

		/* detect current mouth */
		RECT rcMouth;
		match_mouths_with_face (lMouths, rFace, pairEyes, rcMouth);

		/* detect current nose */
		RECT rcNose;
		match_noses_with_face (lNoses, rFace, pairEyes, rcMouth, rcNose);

		lFaces.push_back (Face (rcFace, pairEyes, rcMouth, rcNose, nRot));
	}
	Face::adjustRectFaces(lFaces);

	cvReleaseMemStorage (&storage);

	cvReleaseImageHeader (&pIplImage);
}