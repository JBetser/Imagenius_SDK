#pragma once
#include "IGLibPicture.h"
#include "IGSmartPtr.h"
#include "IGThreadSafeData.h"
#include <string>
#include <list>
#include <msxml2.h>
#include <shlobj.h>

#define IGCONFIGMANAGER_DEMOACCOUNTHEADER	L"Demo@"

namespace IGLibrary
{

class IGConfigManager
{
public:
	IGConfigManager();
	virtual ~IGConfigManager();

	// static helpers
	static char *CreateBufferFromRes (HINSTANCE hInstance, LPCWSTR pwResName, LPCWSTR pwResType);
	static bool DeleteDirectory (LPCTSTR lpszDir, bool noRecycleBin = true);
	static bool CopyFile (LPCTSTR lpszFrom, LPCTSTR lpszTo);
	HRESULT LoadUserConfiguration (HINSTANCE hInstance, LPCWSTR pwConfigRes);
	HRESULT LoadServerConfiguration (HINSTANCE hInstance, LPCWSTR pcwDeepZoomConfigRes);
	HRESULT LoadConfiguration (HINSTANCE hInstance, LPCWSTR pwConfigRes);
	HRESULT SaveConfiguration();
	HRESULT CleanDirectories() const;
	void GetUserPicturesPath (std::wstring &wsUserPicturesPath) const; 
	void GetLogPath (std::wstring &wsLogPath) const;
	void SetLogPath (const std::wstring &wsLogPath);
	inline void GetHistoryPath (std::wstring &wsHistoryPath) const; 
	inline void GetLogLevel (int &nLogLevel) const;
	inline void GetUserOutputPath (std::wstring &wsOutputPath) const;
	inline void GetOutputPath (std::wstring &wsOutputPath) const;
	inline void SetOutputPath (const std::wstring &wsOutputPath);
	inline void GetUserTempPath (std::wstring &wsTempPath) const;
	inline void GetUserMiniPicturesPath (wstring &wsUserMiniPicturesPath) const;
	inline void GetUserMiniPicturesOutputPath (wstring &wsUserMiniPicturesOutputPath) const;
	HRESULT SetUserPicturesPath (const std::wstring &wUserPicturesPath); 
	void GetPictureList (std::list<IGSmartPtr <IGLibPicture>> &lspPictures) const;
	void SetPictureList (std::list<IGSmartPtr <IGLibPicture>> lspPictures);
	bool AddPicture (const std::wstring &wPicturePath, const CComBSTR &bstrMiniPictureBase64);
	bool AddPicture (IGSmartPtr <IGLibPicture> const &spLibPicture);
	bool GetPicture (const std::wstring &wPicturePath, IGSmartPtr <IGLibPicture> &spPictureFound) const;
	void RemovePicture (const std::wstring &wPicturePath);	
	bool Contains (const std::wstring &wPicturePath);
	void SetServerConfig(wchar_t *pwServerIP, int nInputPortId = -1);
	inline bool IsServerConfig() const;
	inline void SetIsServerConfig(bool bServer);
	void SetUserLogin (LPCWSTR pwUserLogin);
	inline LPCWSTR GetUserLogin (bool bOutput = false) const;
	inline LPCWSTR GetServerIP() const;
	inline int GetServerPortId() const;
	inline bool IsUserLogged() const;
	inline int GetNbPictures () const;
	bool DestroyUserAccount() const;
	HRESULT GetFileList (const wstring wsFolderPath, IEnumIDList **ppFileList, IShellFolder **ppFolder) const;
	void AddMiniPicture(wstring wsMiniPicPath, int nWidth, int nHeight);
	void GetMiniPictures(wstring& wsMiniPictures);
	void DisconnectUser();
	HRESULT InterlockSaveDeepZoomConfig (int nWidth, int nHeight, wstring wsDeepZoomOutputPath);

private:
	HRESULT readConfiguration (HINSTANCE hInstance, LPCWSTR pwConfigRes, BOOL bOverwrite);
	void initializeListPicturesPath();
	HRESULT saveAndClearListPicturesPath();
	void deleteHistory() const;
	void garbageFolder (const wchar_t *pcwFolder) const;
	HRESULT loadDeepZoomConfig (HINSTANCE hInstance, LPCWSTR pcwDeepZoomConfigRes);

	bool			m_bLoaded;
	bool			m_bIsServer;
	std::wstring	m_wsLogPath;
	std::wstring	m_wsConfigPath;
	std::wstring	m_wsOutputPath;
	std::wstring	m_wsUserHistoryPath;
	std::wstring	m_wsUserPath;
	std::wstring	m_wsUserTempPath;
	std::wstring	m_wsUserRootPath;
	std::wstring	m_wsUserLogin;
	std::wstring	m_wsUserOutputLogin;	// for demo it is different than m_wsUserLogin
	std::wstring	m_wsUserPicturesPath;
	std::wstring	m_wsUserPicturesBMPath;
	std::wstring	m_wsUserMiniPicturesPath;	
	std::wstring	m_wsUserOutputPath;
	std::wstring	m_wsUserMiniPicturesOutputPath;
	std::wstring	m_wsServerIP;
	int				m_nInputPortId;
	int				m_nLogLevel;
	//std::wstring	m_wsDeepZoomPath;
	CComPtr <IXMLDOMDocument> m_spXMLDoc;
	CComPtr <IXMLDOMNode> m_spNodeCommonConfig;
	CComPtr <IXMLDOMNode> m_spNodeUserPicturesPath;
	CComPtr <IXMLDOMNode> m_spNodeUserPicturesPathValue;
	CComPtr <IXMLDOMNode> m_spNodeListPicturesPath;
	IGThreadSafeData <std::pair <CComPtr <IXMLDOMDocument>, CComPtr <IXMLDOMNamedNodeMap>>> m_threadDataDeepZoomConfig;
	std::list <IGSmartPtr <IGLibPicture>> m_lspPictures;
	std::list <std::wstring> m_lwsMiniPicturePath;
	int m_nLastDeepZoomWidth;	// DZ perf - do not set width/height node if equal
	int m_nLastDeepZoomHeight;
};

inline void IGConfigManager::GetLogLevel (int &nLogLevel) const
{
	nLogLevel = m_nLogLevel;
}

inline void IGConfigManager::GetUserMiniPicturesOutputPath (wstring &wsUserMiniPicturesOutputPath) const
{
	wsUserMiniPicturesOutputPath = m_wsUserMiniPicturesOutputPath;
}

inline void IGConfigManager::GetUserOutputPath (wstring &wsOutputPath) const
{
	wsOutputPath = m_wsUserOutputPath;
}

inline void IGConfigManager::GetOutputPath (wstring &wsOutputPath) const
{
	wsOutputPath = m_wsOutputPath;
}

inline void IGConfigManager::SetOutputPath (const wstring &wsOutputPath)
{
	m_wsOutputPath = wsOutputPath;
}

inline void IGConfigManager::GetUserTempPath (wstring &wsTempPath) const
{
	wsTempPath = m_wsUserTempPath;
}

inline void IGConfigManager::GetPictureList (list<IGSmartPtr <IGLibPicture>> &lspPictures) const
{
	lspPictures = m_lspPictures;
}

inline void IGConfigManager::SetPictureList (list<IGSmartPtr <IGLibPicture>> lspPictures)
{
	m_lspPictures = lspPictures;
}

inline int IGConfigManager::GetNbPictures() const
{
	return m_lspPictures.size();
}

inline bool IGConfigManager::IsServerConfig() const
{
	return m_bIsServer;
}

inline void IGConfigManager::SetIsServerConfig(bool bServer)
{
	m_bIsServer = bServer;
}

inline LPCWSTR IGConfigManager::GetServerIP() const
{
	return m_wsServerIP.c_str();
}

inline int IGConfigManager::GetServerPortId() const
{
	return m_nInputPortId;
}

inline LPCWSTR IGConfigManager::GetUserLogin (bool bOutput) const
{
	return bOutput ? m_wsUserOutputLogin.c_str() : m_wsUserLogin.c_str();
}

inline bool IGConfigManager::IsUserLogged() const
{
	return !m_wsUserLogin.empty();
}

inline void IGConfigManager::GetHistoryPath (wstring &wsHistoryPath) const
{
	wsHistoryPath = m_wsUserHistoryPath;
}

inline void IGConfigManager::GetUserMiniPicturesPath (wstring &wsUserMiniPicturesPath) const
{
	wsUserMiniPicturesPath = m_wsUserMiniPicturesPath;
}

}