// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "IGLibrary.h"

using namespace IGLibrary;

HINSTANCE g_hInstance = NULL;

#ifdef DLLEXPORT
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#else

extern "C" void Init (HINSTANCE hInstance)
{
	g_hInstance = hInstance;
}

extern "C" void Term ()
{
	g_hInstance = NULL;
}
	
extern "C" HINSTANCE GetInstance()
{
	return g_hInstance;
}

#endif