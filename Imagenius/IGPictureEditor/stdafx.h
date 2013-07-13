// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <comsvcs.h>

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#ifdef IGPICTUREEDITOR_SERVER
#define MessageBoxA __MessageBoxA
#define MessageBoxW __MessageBoxW
#endif
#include <windows.h>
#ifdef IGPICTUREEDITOR_SERVER
#undef MessageBoxA
#undef MessageBoxW
#pragma warning(push)
#pragma warning(disable: 4273)
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
#pragma warning(pop)
#endif

using namespace ATL;

extern HINSTANCE getInstance();
