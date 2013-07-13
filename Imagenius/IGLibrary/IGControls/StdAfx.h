// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#ifdef IGSERVER
#define MessageBoxA __MessageBoxA
#define MessageBoxW __MessageBoxW
#endif
#include <windows.h>
#ifdef IGSERVER
#undef MessageBoxA
#undef MessageBoxW
extern int WINAPI
MessageBoxA(
    HWND hWnd,
    LPCSTR lpText,
    LPCSTR lpCaption,
	UINT uType);

extern int WINAPI
MessageBoxW(
    HWND hWnd,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
	UINT uType);
#endif
