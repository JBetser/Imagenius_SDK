// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#ifdef IGSERVER
#define MessageBoxA __MessageBoxA
#define MessageBoxW __MessageBoxW
#include <windows.h>
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
#else
#include <windows.h>
#endif