#include "StdAfx.h"
#include "IGLog.h"
#include "IGConfigManager.h"

using namespace std;
using namespace IGLibrary;

IGThreadSafeData <int> IGLog::mg_lock;
HWND	IGLog::mg_hWndStatus;

IGLog::IGLog(HWND hWnd)
{
	if (hWnd != NULL)
		mg_hWndStatus = hWnd;
}


IGLog::~IGLog(void)
{
}

bool IGLog::Add (const char *pcLogStr, int nLogLevel)
{
	if (!pcLogStr)
		return false;
	if (nLogLevel > 0){
		int nCurLogLevel = 0;
		m_spConfigMgr->GetLogLevel (nCurLogLevel);
		if (nCurLogLevel < nLogLevel)
			return true; // the log is not enough important to be processed
	}
	size_t szBuf = ::strlen(pcLogStr) + 1;
	wchar_t *pwConv = new wchar_t[szBuf]; size_t szNbChConv = 0;
	::mbstowcs_s (&szNbChConv, pwConv, szBuf, pcLogStr, szBuf);
	bool bRes = Add (pwConv, nLogLevel);
	delete [] pwConv;
	return bRes;
}

bool IGLog::AddWithHeader (const char *pcLogStr, const char *pcHeader, int nLogLevel)
{
	if (nLogLevel > 0){
		int nCurLogLevel = 0;
		m_spConfigMgr->GetLogLevel (nCurLogLevel);
		if (nCurLogLevel < nLogLevel)
			return true; // the log is not enough important to be processed
	}
	string sLog (pcHeader);
	sLog += pcLogStr;
	return Add (sLog.c_str(), nLogLevel);
}

bool IGLog::Add (const wchar_t *pwLogStr, int nLogLevel)
{
	if (!pwLogStr)
		return false;
	if (nLogLevel > 0){
		int nCurLogLevel = 0;
		m_spConfigMgr->GetLogLevel (nCurLogLevel);
		if (nCurLogLevel < nLogLevel)
			return true; // the log is not enough important to be processed
	}
	mg_lock.Lock();
	int nPort = m_spConfigMgr->GetServerPortId();
	if (nPort >= 1000){
		wstring wsLogPath;
		m_spConfigMgr->GetLogPath (wsLogPath);
		SYSTEMTIME st;
		::GetSystemTime (&st);
		wchar_t wYear[32];
		_itow (st.wYear, wYear, 10);
		wchar_t wMonth[32];
		_itow (st.wMonth, wMonth, 10);
		wchar_t wDay[32];
		_itow (st.wDay, wDay, 10);
		wsLogPath += L"\\imageniusLocal_trace";
		wsLogPath += wYear;
		if (st.wMonth < 10)
			wsLogPath += L"0";
		wsLogPath += wMonth;
		if (st.wDay < 10)
			wsLogPath += L"0";
		wsLogPath += wDay;
		wsLogPath += L"_";
		wstring::size_type szPos = wsLogPath.length();
		wsLogPath += m_spConfigMgr->GetServerIP();
		do {		
			szPos = wsLogPath.find (L'.', szPos);
			if (szPos != wstring::npos)
				wsLogPath.replace (szPos, 1, L"_");
		}while (szPos != wstring::npos);
		wsLogPath += L"_port";
		wchar_t wInputPort[32];
		_itow (nPort, wInputPort, 10);
		wsLogPath += wInputPort;
		wsLogPath += L".txt";
		FILE *pFile = NULL;
		if (::_wfopen_s (&pFile, wsLogPath.c_str(), L"a") != 0){
			mg_lock.UnLock();
			return false;
		}
		if (!pFile){
			mg_lock.UnLock();
			return false;
		}
		::fwprintf (pFile, L"%dh %dmn %ds %3dms - %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, pwLogStr);
		::fclose (pFile);
	}
	mg_lock.UnLock();
	if (nLogLevel == 0)
		::SetWindowText (mg_hWndStatus, pwLogStr);
	return true;
}

bool IGLog::AddWithHeader (const wchar_t *pwLogStr, const wchar_t *pwHeader, int nLogLevel)
{
	if (nLogLevel > 0){
		int nCurLogLevel = 0;
		m_spConfigMgr->GetLogLevel (nCurLogLevel);
		if (nCurLogLevel < nLogLevel)
			return true; // the log is not enough important to be processed
	}
	wstring sLog (pwHeader);
	sLog += pwLogStr;
	return Add (sLog.c_str(), nLogLevel);
}