#include "stdafx.h"

#ifdef IGSERVER_APP
#ifndef _DEBUG
#pragma warning(push)
#pragma warning(disable: 4273)
int WINAPI
MessageBoxA(
    HWND hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
	UINT uType){return 0;}

int WINAPI
MessageBoxW(
    HWND hWnd,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
	UINT uType){return 0;}
#pragma warning(pop)
#endif
#endif