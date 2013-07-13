#include "stdafx.h"
/*
#ifdef IGSERVER
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
#endif*/

void* calloc(size_t szType, int nNum)
{
	void *pMem = new BYTE [nNum * szType];
	ZeroMemory (pMem, nNum * szType);
	return pMem;
}