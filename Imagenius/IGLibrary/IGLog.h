#pragma once
#include "IGThreadSafeData.h"
#include "IGSingleton.h"
#include "IGConfigManager.h"

namespace IGLibrary
{

class IGLog
{
public:
	IGLog(HWND hWnd = NULL);
	~IGLog(void);

	bool Add (const char *pcLogStr, int nLogLevel = 0);
	bool AddWithHeader (const char *pcLogStr, const char *pcHeader, int nLogLevel = 0);
	bool Add (const wchar_t *pwLogStr, int nLogLevel = 0);
	bool AddWithHeader (const wchar_t *pwLogStr, const wchar_t *pwHeader, int nLogLevel = 0);

private:
	static IGThreadSafeData <int> mg_lock;
	static HWND	mg_hWndStatus;
	IGSingleton <IGConfigManager> m_spConfigMgr;
};

}

