// dllmain.h : Declaration of module class.

class CIGPictureEditorModule : public CAtlDllModuleT< CIGPictureEditorModule >
{
public :
	DECLARE_LIBID(LIBID_IGPictureEditorLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_IGPICTUREEDITOR, "{CEE772AB-DF79-485F-BD82-6FDC1237CD13}")
};

extern class CIGPictureEditorModule _AtlModule;

extern HINSTANCE getInstance();