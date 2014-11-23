#include "stdafx.h"
#include "IGFaceDescriptor.h"
#include "IGConfigManager.h"
#include "IGLog.h"
#include <IGFaceDetectorClient.h>
#include <ShlObj.h>
#include <iostream>
#include <fstream>

#ifdef DEBUG
#define FACEDETECTION_DIRECTORY	L"C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\"
#else
#define FACEDETECTION_DIRECTORY	L"C:\\Imagenius\\IGServer\\IGFaceDetector\\"
#endif
#define FACEDETECTION_COMMAND	L"IGFaceDetector.exe"
#define FACEDETECTOR_OUTPUT_FILE L"Result"
#define FACEDETECTOR_ROTATION_INTERVAL	15

using namespace std;
using namespace IGLibrary;
using namespace ClipperLib;

IGFaceDescriptor::IGFaceDescriptor() : 
	m_nLayerWidth (-1),
	m_nLayerHeight (-1),
	m_nWidth (-1),
	m_nHeight (-1)
{
}

IGFaceDescriptor::IGFaceDescriptor(const IGSmartLayer& smartLayer, const std::wstring& wsDescriptor) : 
	m_nLayerWidth (smartLayer.GetWidth()),
	m_nLayerHeight (smartLayer.GetHeight()),
	m_nWidth (-1),
	m_nHeight (-1)
{
	wstring wsCurDescriptor (wsDescriptor);
	int nIdx = wsCurDescriptor.find (L"&");
	m_nWidth = ::_wtoi (wsCurDescriptor.substr (0, nIdx).c_str());
	wsCurDescriptor = wsCurDescriptor.substr (nIdx + 1);
	nIdx = wsCurDescriptor.find (L"&");
	m_nHeight = ::_wtoi (wsCurDescriptor.substr (0, nIdx).c_str());
	wsCurDescriptor = wsCurDescriptor.substr (nIdx + 1);
	int nPrevIdx = 0;
	do
	{
		nIdx = wsCurDescriptor.find (L"#", nPrevIdx);
		m_lFaces.push_back (Face (m_nWidth, m_nHeight, smartLayer.GetWidth(), smartLayer.GetHeight(), wsCurDescriptor.substr (nPrevIdx, nIdx - nPrevIdx)));
		nPrevIdx = nIdx + 1;
	} while (nIdx != wstring::npos);
}

IGFaceDescriptor& IGFaceDescriptor::operator = (const IGFaceDescriptor& descr)
{
	m_lFaces = descr.m_lFaces;
	m_nLayerWidth = descr.m_nLayerWidth;
	m_nLayerHeight = descr.m_nLayerHeight;
	m_nWidth = descr.m_nWidth;
	m_nHeight = descr.m_nHeight;
	return *this;
}


PROCESS_INFORMATION IGFaceDescriptor::launchDetectProcess(const wstring& wsPictureId, const wstring& wsPictureName, int nRot)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	// we want its console window to be invisible to the user.
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	wstring wsArgs (L" ");
	wsArgs += wsPictureId + L" ";
	wsArgs += wsPictureName + L" ";
	wchar_t twNum [32];
	::_itow_s (nRot, twNum, 32, 10);	
	wsArgs += twNum;
	wstring wsCmd (FACEDETECTION_DIRECTORY);
	wsCmd += FACEDETECTION_COMMAND;
	wsCmd += wsArgs;
	if (wsCmd.length() < _MAX_PATH) {
		wchar_t twCmd[_MAX_PATH];
		ZeroMemory (twCmd, _MAX_PATH * sizeof (wchar_t));
		::wcscpy_s (twCmd, _MAX_PATH, wsCmd.c_str());
		if (::CreateProcessW (NULL, twCmd, NULL, NULL, FALSE,
			 NORMAL_PRIORITY_CLASS, NULL, FACEDETECTION_DIRECTORY, &si, &pi) == 0){
			IGLog log;
			DWORD dw = GetLastError ();
			wchar_t twErrNum [32];
			::_itow_s (dw, twErrNum, 32, 10);
			wstring wsMess (L"IGFaceDescriptor::launchDetectProcess Failed - CreateProcess returned an error: ");
			wsMess += twErrNum;
			wsMess += L" - Command was: ";
			wsMess += twCmd;
			log.Add (wsMess.c_str());
		}
	}
	return pi;
}

void IGFaceDescriptor::launchDetectIce(const wstring& wsPictureId, const wstring& wsPictureName, int nRot)
{
	iceClientDetectFaces (wsPictureId.c_str(), wsPictureName.c_str(), nRot);
}

void IGFaceDescriptor::getPermutations (int offset, vector <Face>& combinations, list <FaceList>& lFl) {
	if (offset == combinations.size() - 1){
		FaceList fl;
		Face f;
		for (vector <Face>::iterator itFaceDetect = combinations.begin(); itFaceDetect != combinations.end(); ++itFaceDetect) {		
			FaceList fl2;
			if (!fl.faceExists (m_nWidth, m_nHeight, *itFaceDetect, fl2))
				fl._lFaces.push_back (*itFaceDetect);
		}
		lFl.push_back (fl);
	}
	else{
		for (int j = offset; j < (int)combinations.size(); j++)
		{
			Face tmp = combinations[offset];
			combinations[offset] = combinations[j];
			combinations[j] = tmp;   

		    getPermutations (offset + 1, combinations, lFl);

		    tmp = combinations[offset];
			combinations[offset] = combinations[j];
			combinations[j] = tmp;  
		}
	}
}

int IGFaceDescriptor::Detect(int nLayerWidth, int nLayerHeight, IGSmartLayer& sampledLayer, bool bRetry){
	m_lFaces.clear();
	
	UUID uuid;
	::UuidCreate (&uuid);
	RPC_WSTR pwUuid;
	::UuidToStringW (&uuid, &pwUuid);
	wstring wsGuid ((wchar_t*)pwUuid);
	::RpcStringFreeW (&pwUuid);
	wstring wsPictureDir (FACEDETECTION_DIRECTORY);
	wsPictureDir += wsGuid;
	wstring wsPicturePath (wsPictureDir);
	::SHCreateDirectory (NULL, wsPicturePath.c_str());
	wsPicturePath += L"\\";
	wsPicturePath += wsGuid + L".jpg";
	sampledLayer.Save (wsPicturePath.c_str(), CXIMAGE_FORMAT_JPG);

	m_nLayerWidth = nLayerWidth;
	m_nLayerHeight = nLayerHeight;
	float fScale;
	if (m_nLayerHeight < m_nLayerWidth)
		fScale = (float)IGSMARTLAYER_INDEXSAMPLE_SIZE / (float)m_nLayerWidth;
	else
		fScale = (float)IGSMARTLAYER_INDEXSAMPLE_SIZE / (float)m_nLayerHeight;
	fScale = min (1.0f, fScale);
	m_nWidth = (int)(fScale * (float)m_nLayerWidth);
	m_nHeight = (int)(fScale * (float)m_nLayerHeight);

	// Launch ANN face detections
	int nScore = 0;
	int nRot = 0;

	SECURITY_DESCRIPTOR sd;
	::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;
	wstring wsEvent (L"Global\\");
	wsEvent += wsGuid;
	IGLog log;
	wstring wsLog (L"IGFaceDescriptor::Detect - Starting face detection ");
	wsLog += wsEvent;
	log.Add (wsLog.c_str());
	HANDLE hEndProcessingEvent = ::CreateEvent (&sa, false, false, wsEvent.c_str());
	for (int idxRot = 0; idxRot <= 45; idxRot += FACEDETECTOR_ROTATION_INTERVAL){
		// launch face detections on Ice Grid
		launchDetectIce (wsGuid.c_str(), wsGuid + L".jpg", idxRot);
		if (idxRot == 0 || idxRot == 180)
			continue;
		launchDetectIce (wsGuid.c_str(), wsGuid + L".jpg", -idxRot);
	}
	// Wait for processing done	
	log.Add (L"IGFaceDescriptor::Detect - Face detection in progress");
#ifdef DEBUG
	DWORD dwWaitRet = ::WaitForSingleObject (hEndProcessingEvent, 120000); // 2mns timeout
#else
	DWORD dwWaitRet = ::WaitForSingleObject (hEndProcessingEvent, 10000); // 10s timeout
#endif
	::CloseHandle (hEndProcessingEvent);
	log.Add (L"IGFaceDescriptor::Detect - Face detection ended");
	// Read and evaluate the results
	for (int idxRot = 0; idxRot <= 45; idxRot += FACEDETECTOR_ROTATION_INTERVAL){
		int nCurScore = evalDetectFaces (wsGuid, idxRot);
		if (nCurScore > nScore){
			nScore = nCurScore;
			nRot = idxRot;
		}
		if (idxRot == 0 || idxRot == 180)
			continue;
		nCurScore = evalDetectFaces (wsGuid, -idxRot);
		if (nCurScore > nScore){
			nScore = nCurScore;
			nRot = -idxRot;
		}
	}
	IGConfigManager::DeleteDirectory (wsPictureDir.c_str());
		
	correctFaces();
	if (filterSharedEyes())
		correctFaces();
	filterPoorScores();	
	filterSmallAndRotatedFaces();	

	FaceList fl;
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
		FaceList facesExisting;
		if (fl.faceExists (m_nWidth, m_nHeight, *itFaceDetect, facesExisting)){
			if ((*itFaceDetect).getScore() > facesExisting.getScore()){
				fl.remove (facesExisting);
				fl._lFaces.push_back (*itFaceDetect);
			}
		}
		else{
			fl._lFaces.push_back (*itFaceDetect);
		}
	}
	m_lFaces = fl._lFaces;
	nScore = fl.getScore();

	if (nScore < IGFACEDESCRIPTOR_MINSCORE){
		if (!bRetry){
			// retry with scale factor if detection is too poor
			float fRatio = 2.0f;
			IGSmartLayer sampledLayerScale (sampledLayer);
			sampledLayerScale.Clear(255);
			int nSampledScaleWidth = sampledLayerScale.GetWidth();
			int nSampledScaleHeight = (int)(fRatio * (float)sampledLayerScale.GetHeight());
			if (sampledLayerScale.Resample (nSampledScaleWidth, nSampledScaleHeight)){

				sampledLayerScale.Mix (sampledLayer);
				
				sampledLayerScale.Resample();
												
				int nScaledScore = Detect (nLayerWidth, (int)(fRatio * (float)nLayerHeight), sampledLayerScale, true);
				if (nScaledScore >= IGFACEDESCRIPTOR_MINSCORE){		
					Scale (sampledLayerScale.GetWidth(), sampledLayerScale.GetHeight(), nSampledScaleWidth, nSampledScaleHeight / 2);
					m_nWidth = sampledLayer.GetWidth();
					m_nHeight = sampledLayer.GetHeight();
					m_nLayerWidth = nLayerWidth;
					m_nLayerHeight = nLayerHeight;
				}
				else {
					m_lFaces.clear();
				}
			}
		}
	}
	return nScore;
}

void IGFaceDescriptor::GetFaces (list <Face>& lFaces)
{
	lFaces = m_lFaces;
}

Face IGFaceDescriptor::GetFace (int idxFace)
{
	Face ret;
	int idxFaceIter = 0;
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect, ++idxFaceIter){
		if (idxFaceIter == idxFace)
			return (*itFaceDetect);
	}
	return ret;
}

void IGFaceDescriptor::Rotate (int nRotate)
{	
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect)
		(*itFaceDetect).Rotate (m_nWidth / 2, m_nHeight / 2, nRotate);	
	if ((abs(nRotate) % 180) != 0){
		int nWidth = m_nWidth;
		m_nWidth = m_nHeight;
		m_nHeight = nWidth;
	}
}

void IGFaceDescriptor::Rotate()
{	
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect)
		(*itFaceDetect).Rotate (m_nWidth / 2, m_nHeight / 2, (*itFaceDetect).getRotation());	
}

void IGFaceDescriptor::UnRotate()
{	
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect)
		(*itFaceDetect).Rotate (m_nWidth / 2, m_nHeight / 2, -(*itFaceDetect).getRotation());	
}

void IGFaceDescriptor::Scale (int nWidth, int nHeight, int nLayerWidth, int nLayerHeight)
{
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
		(*itFaceDetect).Rotate (nWidth / 2, nHeight / 2, -(*itFaceDetect).getRotation());

		(*itFaceDetect).Offset (0, - (int)(0.5f * (float)nHeight));

		float fScale;
		if (nLayerHeight < nLayerWidth)
			fScale = (float)nLayerWidth / (float)nWidth;
		else
			fScale = (float)nLayerHeight / (float)nHeight;

		int nNewWidth = (int)((float)nWidth * fScale);
		int nNewHeight = (int)(0.5f * (float)nHeight * fScale);
		(*itFaceDetect).Scale (nWidth, (int)(0.5f * (float)nHeight), nNewWidth, nNewHeight);	

		(*itFaceDetect).Rotate ((int)((float)nNewWidth / 2.0f), (int)((float)nNewHeight / 2.0f), (*itFaceDetect).getRotation());	
	}
}

void IGFaceDescriptor::Offset (int nX, int nY)
{
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect)
		(*itFaceDetect).Offset (nX, nY);
}

bool IGFaceDescriptor::ConvertTo(std::wstring& wsDescriptor) const {
	if (m_lFaces.size() > 0){
		wchar_t wInt [32];
		if (::_itow_s (m_nWidth, wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		if (::_itow_s (m_nHeight, wInt, 10) != 0)
			return false;
		wsDescriptor += wInt; wsDescriptor += L"&";
		for (list <Face>::const_iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
			if (itFaceDetect != m_lFaces.begin())
				wsDescriptor += L"#";
			wsDescriptor += (*itFaceDetect).ToString(m_nWidth, m_nHeight, m_nLayerWidth, m_nLayerHeight);
		}
	}
	return true;
}

void IGFaceDescriptor::correctFaces()
{
	// complete missing information on eyes, mouth & noze base on generic proportions
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
		RECT rcNo = {-1,-1,-1,-1}; RECT rcEye1 = {-1,-1,-1,-1};		
		RECT rcFace = (*itFaceDetect).getFaceCoords();
		std::pair <RECT, RECT> pairEyes  = std::pair <RECT, RECT> ((*itFaceDetect).getEyeLeftCoords(), (*itFaceDetect).getEyeRightCoords());
		// deduce missing eyes
		if (pairEyes.first.left == -1 && pairEyes.second.left == -1){
			int nAvgX = (rcFace.left + rcFace.right) / 2;
			int nAvgY = (rcFace.top + rcFace.bottom) / 2;
			int nHighX = rcFace.right;
			int nHighY = rcFace.bottom;
			int nLowX = rcFace.left;
			int nLowY = rcFace.top;
			int nStdDevX = (nHighX - nLowX) / 3;
			int nStdDevY = (nHighY - nLowY) / 4;
			int nSizeEyeX = nStdDevX;	
			int nSizeEyeY = (nSizeEyeX * 3) / 4;
			RECT rcLeftEye, rcRightEye, rcIntersectEyeFace;
			rcLeftEye.left = nAvgX - (nStdDevX * 3) / 4 - nSizeEyeX / 2;
			rcLeftEye.right = rcLeftEye.left + nSizeEyeX;
			rcLeftEye.top = nAvgY + nStdDevY / 2 - nSizeEyeY / 2;
			rcLeftEye.bottom = rcLeftEye.top + nSizeEyeY;
			rcIntersectEyeFace.left = max ((int)rcLeftEye.left, (int)rcFace.left + nSizeEyeX / 3);
			rcIntersectEyeFace.top = max ((int)rcLeftEye.top, (int)rcFace.top);
			rcIntersectEyeFace.right = min ((int)rcLeftEye.right, (int)rcFace.right);
			rcIntersectEyeFace.bottom = min ((int)rcLeftEye.bottom, (int)rcFace.bottom);
			rcLeftEye = rcIntersectEyeFace;
			pairEyes.first = rcLeftEye;
			rcRightEye = rcLeftEye;
			rcRightEye.left = nAvgX + nStdDevX / 4;
			rcRightEye.right = rcRightEye.left + nSizeEyeX;
			rcIntersectEyeFace.left = max ((int)rcRightEye.left, (int)rcFace.left);
			rcIntersectEyeFace.top = max ((int)rcRightEye.top, (int)rcFace.top);
			rcIntersectEyeFace.right = min ((int)rcRightEye.right, (int)rcFace.right - nSizeEyeX / 3);
			rcIntersectEyeFace.bottom = min ((int)rcRightEye.bottom, (int)rcFace.bottom);
			rcRightEye = rcIntersectEyeFace;
			pairEyes.second = rcRightEye;
		}		
		else if (pairEyes.first.left == -1 || pairEyes.second.left == -1){
			RECT rcEye2; 
			rcEye1 = pairEyes.first;  rcEye2 = pairEyes.second;
			int nXmiddle = (rcFace.right + rcFace.left) / 2;
			if (rcEye1.left == -1){
				if (nXmiddle > (rcEye2.right + rcEye2.left) / 2){
					rcEye1 = rcEye2;
					rcEye2 = rcNo;
				}
			}
			if (rcEye2.left == -1){
				if (nXmiddle < (rcEye1.right + rcEye1.left) / 2){
					rcEye2 = rcEye1;
					rcEye1 = rcNo;
				}
			}
			pairEyes.first = rcEye1;
			pairEyes.second = rcEye2;
			if (rcEye1.left == -1){
				rcEye1.left = rcEye2.left - (int)(1.2f*(float)(rcEye2.right - rcEye2.left));
				rcEye1.right = rcEye1.left + rcEye2.right - rcEye2.left;
				rcEye1.top = rcEye2.top;
				rcEye1.bottom = rcEye2.bottom;
				RECT rcIntersectEyeFace;
				rcIntersectEyeFace.left = min (max ((int)rcEye1.left, (int)rcFace.left + (rcFace.right - rcEye2.right)), (int)(rcEye2.right - (rcEye2.bottom - rcEye2.top)));
				rcIntersectEyeFace.top = max ((int)rcEye1.top, (int)rcFace.top);
				rcIntersectEyeFace.right = max (min ((int)rcEye1.right, (int)rcFace.right), (int)(rcIntersectEyeFace.left + (rcEye1.bottom - rcEye1.top)));
				rcIntersectEyeFace.bottom = min ((int)rcEye1.bottom, (int)rcFace.bottom);
				pairEyes.first = rcIntersectEyeFace;
			}
			else if (rcEye2.left == -1){
				rcEye2.right = rcEye1.right + (int)(1.2f*(float)(rcEye1.right - rcEye1.left));
				rcEye2.left = rcEye2.right - (rcEye1.right - rcEye1.left);
				rcEye2.top = rcEye1.top;
				rcEye2.bottom = rcEye1.bottom;
				RECT rcIntersectEyeFace;
				rcIntersectEyeFace.left = max ((int)rcEye2.left, (int)rcFace.left);
				rcIntersectEyeFace.top = max ((int)rcEye2.top, (int)rcFace.top);
				rcIntersectEyeFace.right = max (min ((int)rcEye2.right, (int)rcFace.right - (rcEye1.left - rcFace.left)), (int)(rcIntersectEyeFace.left + rcEye2.bottom - rcEye2.top));
				rcIntersectEyeFace.bottom = min ((int)rcEye2.bottom, (int)rcFace.bottom);
				pairEyes.second = rcIntersectEyeFace;
			}
		}
		else{
			if (pairEyes.second.left < pairEyes.first.left){
				rcEye1 = pairEyes.second;
				pairEyes.second = pairEyes.first;
				pairEyes.first = rcEye1;
			}
		}
		// resolve eye intersections
		if (pairEyes.first.right > pairEyes.second.left){
			int nEyeWidth = (pairEyes.second.right - pairEyes.first.left) / 2;
			pairEyes.first.right = pairEyes.first.left + nEyeWidth;
			pairEyes.second.left = pairEyes.second.right - nEyeWidth;
		}
		(*itFaceDetect).setEyeLeftCoords (pairEyes.first);
		(*itFaceDetect).setEyeRightCoords (pairEyes.second);

		RECT rcMouth = (*itFaceDetect).getMouthCoords();
		if (rcMouth.left == -1 || rcMouth.top == -1 || rcMouth.right == -1 || rcMouth.bottom == -1)
			(*itFaceDetect).setMouthCoords ((*itFaceDetect).getDefaultMouth());	
	}
}

void IGFaceDescriptor::filterSmallAndRotatedFaces()
{
	int nBiggestFaceArea = 0;
	int nHighestScore = 0;
	int nHighestScoreRot = 0;	
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
		RECT rcFace = (*itFaceDetect).getFaceCoords();
		int nFaceArea = (rcFace.right - rcFace.left) * (rcFace.bottom - rcFace.top);
		if (nBiggestFaceArea < nFaceArea)
			nBiggestFaceArea = nFaceArea;
		int nScore = (*itFaceDetect).getScore();
		if (nScore > nHighestScore){
			nHighestScoreRot = (*itFaceDetect).getRotation();
		}
	}
	list <Face> lFilteredFaces;
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
		RECT rcFace = (*itFaceDetect).getFaceCoords();
		int nFaceArea = (rcFace.right - rcFace.left) * (rcFace.bottom - rcFace.top);
		if (((float)nFaceArea > (float)nBiggestFaceArea * 0.33f) &&
			(abs (nHighestScoreRot - (*itFaceDetect).getRotation()) < 90))
			lFilteredFaces.push_back(*itFaceDetect);
	}
	m_lFaces = lFilteredFaces;
}

bool IGFaceDescriptor::filterSharedEyes()
{
	bool bRet = false;
	for (list <Face>::iterator itFaceDetect = m_lFaces.begin(); itFaceDetect != m_lFaces.end(); ++itFaceDetect){
		list <Face>::iterator itFaceComp = itFaceDetect;
		++itFaceComp;
		for (; itFaceComp != m_lFaces.end(); ++itFaceComp){
			Face faceDetect (*itFaceDetect);
			Face faceComp (*itFaceComp);
			faceDetect.Rotate (m_nWidth / 2, m_nHeight / 2, -(*itFaceDetect).getRotation());
			faceComp.Rotate (m_nWidth / 2, m_nHeight / 2, -(*itFaceComp).getRotation());
			std::pair <RECT, RECT> pairEyes  = std::pair <RECT, RECT> ((*itFaceDetect).getEyeLeftCoords(), (*itFaceDetect).getEyeRightCoords());
			std::pair <RECT, RECT> pairEyesComp  = std::pair <RECT, RECT> (faceComp.getEyeLeftCoords(), faceComp.getEyeRightCoords());
			int nMidEyeDetectLeftX = (pairEyes.first.left + pairEyes.first.right) / 2;
			int nMidEyeDetectLeftY = (pairEyes.first.top + pairEyes.first.bottom) / 2;
			int nMidEyeDetectRightX = (pairEyes.second.left + pairEyes.second.right) / 2;
			int nMidEyeDetectRightY = (pairEyes.second.top + pairEyes.second.bottom) / 2;
			int nMidEyeCompRightX = (pairEyesComp.first.left + pairEyesComp.first.right) / 2;
			int nMidEyeCompRightY = (pairEyesComp.first.top + pairEyesComp.first.bottom) / 2;
			int nMidEyeCompLeftX = (pairEyesComp.second.left + pairEyesComp.second.right) / 2;
			int nMidEyeCompLeftY = (pairEyesComp.second.top + pairEyesComp.second.bottom) / 2;			

			int nMaxSizeCompLeft = max(((pairEyes.first.right - pairEyes.first.left) + (pairEyes.first.bottom - pairEyes.first.top)) / 2,
									((pairEyesComp.second.right - pairEyesComp.second.left) + (pairEyesComp.first.bottom - pairEyesComp.second.top)) / 2);
			int nMaxSizeCompRight = max(((pairEyes.second.right - pairEyes.second.left) + (pairEyes.second.bottom - pairEyes.second.top)) / 2,
									((pairEyesComp.first.right - pairEyesComp.first.left) + (pairEyesComp.first.bottom - pairEyesComp.first.top)) / 2);

			if ((sqrtf((float)((nMidEyeDetectLeftX - nMidEyeCompLeftX) * (nMidEyeDetectLeftX - nMidEyeCompLeftX) + (nMidEyeDetectLeftY - nMidEyeCompLeftY) * (nMidEyeDetectLeftY - nMidEyeCompLeftY))) < nMaxSizeCompLeft) ||
				(sqrtf((float)((nMidEyeDetectRightX - nMidEyeCompRightX) * (nMidEyeDetectRightX - nMidEyeCompRightX) + (nMidEyeDetectRightY - nMidEyeCompRightY) * (nMidEyeDetectRightY - nMidEyeCompRightY))) < nMaxSizeCompRight)) {
				RECT rcNo = {-1,-1,-1,-1}; 
				if (faceDetect.getScore() > faceComp.getScore()){
					(*itFaceComp).setEyeLeftCoords (rcNo);
					(*itFaceComp).setEyeRightCoords (rcNo);
					(*itFaceComp).CalculateScore();
				}
				else{
					(*itFaceDetect).setEyeLeftCoords (rcNo);
					(*itFaceDetect).setEyeRightCoords (rcNo);
					(*itFaceDetect).CalculateScore();
				}
				bRet = true;
			}
		}
	}
	return bRet;
}

void IGFaceDescriptor::filterPoorScores()
{
	list <Face> cpyFaces (m_lFaces);
	for (list <Face>::iterator itFaceDetect = cpyFaces.begin(); itFaceDetect != cpyFaces.end(); ++itFaceDetect){
		int nScore = (*itFaceDetect).getScore();
		if (nScore < 10) // filter worst-case face detection
			m_lFaces.remove (*itFaceDetect);
	}
}

int IGFaceDescriptor::evalDetectFaces(const wstring& wsPictureId, int idxRot)
{
	// read ANN face detection results
	list <Face> lFaces;
	wstring wsResultFile (FACEDETECTION_DIRECTORY);
	wsResultFile += wsPictureId + L"\\";
	wsResultFile += FACEDETECTOR_OUTPUT_FILE;
	wchar_t twNum [32];
	::_itow_s (idxRot, twNum, 32, 10);	
	wsResultFile += twNum;
	wsResultFile += L".txt";
	HANDLE hFile = ::CreateFile (wsResultFile.c_str(), GENERIC_READ, FILE_SHARE_READ,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;
	ifstream resfile;
	resfile.open (wsResultFile.c_str());
	while (!resfile.eof()){
		Face newFace;
		newFace << resfile;
		if (newFace.getScore() > 0)
			lFaces.push_back (newFace);	
	}
	resfile.close();
	::CloseHandle(hFile);
	int nScore = 0;	
	for (list <Face>::iterator itFaceDetect = lFaces.begin(); itFaceDetect != lFaces.end(); ++itFaceDetect){
		m_lFaces.push_back (*itFaceDetect);
		nScore += (*itFaceDetect).getScore();
	}	
	if (lFaces.size() > 0){
		if (nScore / lFaces.size() <= 1) // filter worst-case face detection
			nScore = 0;
	}
	return nScore;
}

void IGFaceDescriptor::rotatePt (int nRot, int nPtCtrX, int nPtCtrY, int nPtX, int nPtY, int& nRotatedX, int& nRotatedY)
{
	// change of vectorial space, set the origin to the center
	int nX = nPtX - nPtCtrX;
	int nY = nPtY - nPtCtrY;
	// rotate
	float fXTr = cosf((float)nRot * PI / 180.0f);
	float fYTr = sinf((float)nRot * PI / 180.0f);
	nRotatedX = (int)(nX * fXTr + nY * fYTr);
	nRotatedY = (int)(-nX * fYTr + nY * fXTr);
	// change of vectorial space, back to the origin
	nRotatedX = nRotatedX + nPtCtrX;
	nRotatedY = nRotatedY + nPtCtrY;
}