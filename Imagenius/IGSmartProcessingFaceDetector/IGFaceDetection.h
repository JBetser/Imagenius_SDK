#ifdef DLLEXPORT
#define DLLEXPORT_API __declspec(dllexport)
#else
#define DLLEXPORT_API __declspec(dllimport)
#endif

extern "C"
{
	// python stub
	unsigned char execute_python_script(const char *pcFileName, const char *pcFuncName, int nNbArgs, const char **pcArgs) {return 0;}
	
	// API
	void DLLEXPORT_API detectFaces(const wchar_t* pcwPictureId, const wchar_t* pcwPictureName, int rotationIndex, int *pNbFaces, int *pScore);
}
