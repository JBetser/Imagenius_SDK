// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "IGPictureEditor_i.h"
#include "dllmain.h"

CIGPictureEditorModule _AtlModule;
static HINSTANCE _Instance = NULL;

HINSTANCE getInstance() {
	return _Instance;}

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	_Instance = hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}
