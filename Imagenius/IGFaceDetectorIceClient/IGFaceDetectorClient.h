#ifdef DLLEXPORT
#define DLLEXPORT_API __declspec(dllexport)
#else
#define DLLEXPORT_API __declspec(dllimport)
#endif

extern "C"
{
	void DLLEXPORT_API iceClientDetectFaces(const wchar_t* pcwPictureId, const wchar_t* pcwPictureName, int rotationIndex);
}
