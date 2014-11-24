#include "StdAfx.h"
#include "IGAutoKill.h"
#include <IGLog.h>
#include <Windows.h>

using namespace IGLibrary;

IGAutoKill::IGAutoKill()
{
	::GetSystemTime(&_startTime);
}

void IGAutoKill::Check()
{
	SYSTEMTIME checkTime;
	::GetSystemTime(&checkTime);
	ULARGE_INTEGER timeDiff = checkTime - _startTime;
	if ((timeDiff.QuadPart / 10000000) > MAX_LIVE_TIME_SEC)
		Kill();
}

void IGAutoKill::Kill()
{
	if (IsDead())
		return;
	IsDead() = true;
	try{
		IGLog log;
		log.Add (L"IGAutoKill - exiting the process...");
	}
	catch(...){}
#ifndef DEBUG
	exit(0);
#endif
}

bool& IGAutoKill::IsDead()
{
	static bool bIsDead = false;
	return bIsDead;
}

void IGAutoKill::Snooze()
{
	IsDead() = false;
	::GetSystemTime(&_startTime);
}

ULARGE_INTEGER operator-(const SYSTEMTIME& pSr,const SYSTEMTIME& pSl)
{
	FILETIME v_ftime;
	ULARGE_INTEGER v_ui;
	__int64 v_right,v_left,v_res;
	SystemTimeToFileTime(&pSr,&v_ftime);
	v_ui.LowPart=v_ftime.dwLowDateTime;
	v_ui.HighPart=v_ftime.dwHighDateTime;
	v_right=v_ui.QuadPart;

	SystemTimeToFileTime(&pSl,&v_ftime);
	v_ui.LowPart=v_ftime.dwLowDateTime;
	v_ui.HighPart=v_ftime.dwHighDateTime;
	v_left=v_ui.QuadPart;

	v_res=v_right-v_left;

	v_ui.QuadPart=v_res;
	return v_ui;
}