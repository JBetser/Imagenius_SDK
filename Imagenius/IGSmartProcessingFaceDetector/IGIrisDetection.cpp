#include "IGIrisDetection.h"
#include "ximage.h"
#include "IrisDetector.h"

using namespace std;
using namespace IGLibrary;

extern "C"
{
	void detectIris (const CxImage& faceImg, RECT eye1, RECT eye2, POINT & ptEye1, int& ray1, POINT & ptEye2, int& ray2)
	{
		detect_iris (faceImg, eye1, eye2, ptEye1, ray1, ptEye2, ray2);
	}
}