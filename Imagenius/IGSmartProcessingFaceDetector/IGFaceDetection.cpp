#include "IGFaceDetection.h"
#include "ximage.h"
#include "FaceDetector.h"
#include "IGLog.h"
#include "IGSmartLayer.h"
#include <fstream>

#define OUTPUT_FILE L"Result"
#ifdef DEBUG
#define FACEDETECTOR_PATH	L"C:\\Imagenius\\Testing\\IGServer\\IGFaceDetector\\"
#else
#define FACEDETECTOR_PATH	L"C:\\Imagenius\\IGServer\\IGFaceDetector\\"
#endif

using namespace std;
using namespace IGLibrary;

extern "C"
{

void detectFaces(const wchar_t* pcwPictureId, const wchar_t* pcwPictureName, int rotationIndex, int *pNbFaces, int *pScore)
{
	wstring wsPictureId (pcwPictureId);
	wstring wsPictureName (pcwPictureName);
	wstring wsMess;
	wstring wsFaceDetectorPath (FACEDETECTOR_PATH);

	wchar_t twRot[32];
	::_itow_s(rotationIndex, twRot, 10);
	wstring wsRot (twRot);

	IGLog log;
	try{
		wstring wsPath (wsFaceDetectorPath);
		wsPath += wsPictureId;
		wsPath += L"\\";
		wsPath += pcwPictureName;
		CxImage sampledLayer (wsPath.c_str(), CXIMAGE_FORMAT_JPG);

		// Face detection		
		float cosRot = cosf((float)rotationIndex * PI / 180.0f);
		float sinRot = sinf((float)rotationIndex * PI / 180.0f);
		list <Face> lFaces;
		if (rotationIndex != 0) {
			IGSmartLayer rotLayer (sampledLayer);
			rotLayer.RotateAndResize (cosRot, -sinRot, 1.0f);	// detection rotation is inverted as cv space is opposite Y as cx
			detectFaces (rotLayer, lFaces, 0);
		}
		else
			detectFaces (sampledLayer, lFaces, 0);
		
		wstring wsOutputFile (wsFaceDetectorPath);
		wsOutputFile += wsPictureId;
		wsOutputFile += L"\\";
		wsOutputFile += OUTPUT_FILE;
		wsOutputFile += wsRot;
		wsOutputFile += L".txt";
		ofstream outfile;
		outfile.open (wsOutputFile.c_str());
		if ((outfile.rdstate() & ios_base::failbit) == ios_base::failbit){
			wsMess = L"IGFaceDetectorIceI::faceDetection Failed - Cannot open file: ";
			wsMess += wsOutputFile;
			log.Add (wsMess.c_str());
		}
		int nScore = 0;
		for (list <Face>::iterator itFaceDetect = lFaces.begin(); itFaceDetect != lFaces.end(); ++itFaceDetect){
			nScore += (*itFaceDetect).CalculateScore();
			(*itFaceDetect).setRotation (rotationIndex);
			(*itFaceDetect) >> outfile;
			outfile << "\r\n";
		}	
		outfile.close();		
		*pNbFaces = lFaces.size();
		*pScore = nScore;
	}
	catch(...){
		wsMess = L"IGFaceDetectorIceI::faceDetection Failed - Exception was caught running arguments: ";
		wsMess += pcwPictureId;
		wsMess += L" - ";
		wsMess += pcwPictureName;
		wsMess += L" - ";
		wsMess += wsRot;
		log.Add (wsMess.c_str());
	}
}

}