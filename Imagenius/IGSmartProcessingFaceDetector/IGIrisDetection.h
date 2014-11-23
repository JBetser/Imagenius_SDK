#include <Windows.h>

#ifdef DLLEXPORT
#define DLLEXPORT_API __declspec(dllexport)
#else
#define DLLEXPORT_API __declspec(dllimport)
#endif

class CxImage;

extern "C"
{
	void DLLEXPORT_API detectIris (const CxImage& faceImg, RECT eye1, RECT eye2, POINT & ptEye1, int& ray1, POINT & ptEye2, int& ray2);
}
