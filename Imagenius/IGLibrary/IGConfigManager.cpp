#include "StdAfx.h"
#include "IGConfigManager.h"
#include "IGLibPicture.h"
#include "IGFrame.h"
#include "IGPing.h"
#include "IGLog.h"
#include <stdlib.h>
#include <time.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <atlstr.h>

#define IGCONFIGMANAGER_CONFIGPATH			L"\\Imagenius\\Config\\IGPictureEditorConfig.xml"
#define IGCONFIGMANAGER_DEEPZOOMCONFIGPATH	L"./Config/IGDeepZoomConfig.xml"
#ifndef _DEBUG
#define IGCONFIGMANAGER_SERVERCONFIGPATH	L"./Config/IGServerConfig.xml"
#else
#define IGCONFIGMANAGER_SERVERCONFIGPATH	L"./Config/IGServerConfigDebug.xml"
#endif
#define IGCONFIGMANAGER_HISTORYPATH			L"\\Imagenius\\History"
#define IGCONFIGMANAGER_IMAGEPATH			L"\\Imagenius\\Images\\"
#define IGCONFIGMANAGER_UPLOADPATH			L"\\Imagenius\\Upload"
#define IGCONFIGMANAGER_BEETLEMORPH			L"BeetleMorph\\"
#define IGCONFIGMANAGER_IMAGEMINIPATH		L"Mini\\"
#define IGCONFIGMANAGER_TEMPPATH			L"\\Imagenius\\Temp\\"
#define IGCONFIGMANAGER_LOGPATH				L"./LogPath"
#define IGCONFIGMANAGER_LOGLEVEL			L"./LogLevel"
#define IGCONFIGMANAGER_USERPATH			L"./ServerUserPath"
#define IGCONFIGMANAGER_SERVEROUTPUT		L"./ServerOutput"
#define IGCONFIGMANAGER_DEEPZOOMPATH		L"./DeepZoomPath"

using namespace IGLibrary;
using namespace IGPing;
using namespace std;

IGConfigManager::IGConfigManager()	: m_bLoaded (false)
									, m_bIsServer (false)
									, m_nLastDeepZoomWidth (-1)
									, m_nLastDeepZoomHeight (-1)
									, m_nInputPortId (-1)
									, m_nLogLevel (0)
{
	srand ((unsigned int)time (NULL));
}

IGConfigManager::~IGConfigManager()
{
}

char *IGConfigManager::CreateBufferFromRes (HINSTANCE hInstance, LPCWSTR pwResName, LPCWSTR pwResType)
{
	HRSRC hRsrc = ::FindResource (  hInstance,
									pwResName,
									pwResType	); 
	// add a zero terminal to resource buffer
	DWORD dwNbMemBytes = ::SizeofResource (hInstance, hRsrc);
	HGLOBAL hMem = ::LoadResource (hInstance, hRsrc);	
	char *pMem = (char *)::LockResource (hMem);
	char *pMemCopy = new char [dwNbMemBytes + 1];
	::ZeroMemory (pMemCopy, dwNbMemBytes + 1);
	::memcpy_s (pMemCopy, dwNbMemBytes + 1, pMem, dwNbMemBytes);
	return pMemCopy;
}

bool IGConfigManager::DeleteDirectory (LPCTSTR lpszDir, bool noRecycleBin)
{
	int len = _tcslen(lpszDir);
	TCHAR *pszFrom = new TCHAR[len+2];
	_tcscpy(pszFrom, lpszDir);
	pszFrom[len] = 0;
	pszFrom[len+1] = 0;
	if ((pszFrom[len-1] == L'\\') || (pszFrom[len-1] == L'/'))
		pszFrom[len-1] = 0;

	SHFILEOPSTRUCT fileop;
	fileop.hwnd = NULL; // no status display
	fileop.wFunc = FO_DELETE; // delete operation
	fileop.pFrom = pszFrom; // source file name as double null terminated string
	fileop.pTo = NULL; // no destination needed
	fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT; // do not prompt the user

	if(!noRecycleBin)
	fileop.fFlags |= FOF_ALLOWUNDO;

	fileop.fAnyOperationsAborted = FALSE;
	fileop.lpszProgressTitle = NULL;
	fileop.hNameMappings = NULL;

	int ret = ::SHFileOperation(&fileop);

	delete [] pszFrom; 
	return (ret == 0);
}

bool IGConfigManager::CopyFile (LPCTSTR lpszFrom, LPCTSTR lpszTo)
{
	int lenFrom = _tcslen(lpszFrom);
	TCHAR *pszFrom = new TCHAR[lenFrom+2];
	_tcscpy(pszFrom, lpszFrom);
	pszFrom[lenFrom] = 0;
	pszFrom[lenFrom+1] = 0;
	if ((pszFrom[lenFrom-1] == L'\\') || (pszFrom[lenFrom-1] == L'/'))
		pszFrom[lenFrom-1] = 0;
	int lenTo = _tcslen(lpszTo);
	TCHAR *pszTo = new TCHAR[lenTo+2];
	_tcscpy(pszTo, lpszTo);
	pszTo[lenTo] = 0;
	pszTo[lenTo+1] = 0;
	if ((pszTo[lenTo-1] == L'\\') || (pszTo[lenTo-1] == L'/'))
		pszTo[lenTo-1] = 0;
	SHFILEOPSTRUCT fileop;
	fileop.hwnd = NULL; // no status display
	fileop.wFunc = FO_COPY; // delete operation
	fileop.pFrom = pszFrom; // source file name as double null terminated string
	fileop.pTo = lpszTo; // no destination needed
	fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT; // do not prompt the user
	fileop.fAnyOperationsAborted = FALSE;
	fileop.lpszProgressTitle = NULL;
	fileop.hNameMappings = NULL;

	int ret = ::SHFileOperation(&fileop);

	delete [] pszFrom; 
	return (ret == 0);
}

HRESULT IGConfigManager::LoadServerConfiguration (HINSTANCE hInstance, LPCWSTR pcwDeepZoomConfigRes)
{
	// read the server config file
	HANDLE hFile = ::CreateFile (IGCONFIGMANAGER_SERVERCONFIGPATH, GENERIC_READ, FILE_SHARE_READ,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;
	::CloseHandle (hFile);
	HRESULT hr = S_OK;
	CComPtr <IXMLDOMDocument> spXMLDocServerConfig;
	hr = ::CoCreateInstance(CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
									IID_IXMLDOMDocument, (void**)&spXMLDocServerConfig);
	if (FAILED(hr))
		return hr;
	VARIANT_BOOL bSucceeded;
	hr = spXMLDocServerConfig->load (CComVariant(IGCONFIGMANAGER_SERVERCONFIGPATH), &bSucceeded);
	if ((hr != S_OK) || !bSucceeded)
		return E_FAIL;
	CComPtr <IXMLDOMNode> spNodeCommonConfig;
	hr = spXMLDocServerConfig->selectSingleNode (CComBSTR("./Common"), &spNodeCommonConfig);
	if ((hr != S_OK) || !spNodeCommonConfig)
		return E_FAIL;	

	// construct the user root path
	CComPtr <IXMLDOMNode> spNodeServerConfigPath;
	hr = spNodeCommonConfig->selectSingleNode (CComBSTR(IGCONFIGMANAGER_USERPATH), &spNodeServerConfigPath);
	if ((hr != S_OK) || !spNodeServerConfigPath)
		return E_FAIL;
	CComPtr <IXMLDOMNode> spNodeServerConfigPathValue;
	spNodeServerConfigPath->get_firstChild (&spNodeServerConfigPathValue);
	if ((hr != S_OK) || !spNodeServerConfigPathValue)
		return E_FAIL;
	VARIANT vUserPicturesPath;
	spNodeServerConfigPathValue->get_nodeValue (&vUserPicturesPath);
	if (!vUserPicturesPath.bstrVal)
		return E_FAIL;
	m_wsUserRootPath = vUserPicturesPath.bstrVal;
	
	// construct the output path
	IGFrame::SetOutputPath (m_wsOutputPath.c_str());

	// construct the log path
	CComPtr <IXMLDOMNode> spNodeServerLogPath;
	hr = spNodeCommonConfig->selectSingleNode (CComBSTR(IGCONFIGMANAGER_LOGPATH), &spNodeServerLogPath);
	if ((hr != S_OK) || !spNodeServerLogPath)
		return E_FAIL;
	CComPtr <IXMLDOMNode> spNodeServerLogPathValue;
	spNodeServerLogPath->get_firstChild (&spNodeServerLogPathValue);
	if ((hr != S_OK) || !spNodeServerLogPathValue)
		return E_FAIL;
	VARIANT vLogPath;
	spNodeServerLogPathValue->get_nodeValue (&vLogPath);
	if (!vLogPath.bstrVal)
		return E_FAIL;
	m_wsLogPath = vLogPath.bstrVal;

	// log level
	CComPtr <IXMLDOMNode> spNodeServerLogLevel;
	hr = spNodeCommonConfig->selectSingleNode (CComBSTR(IGCONFIGMANAGER_LOGLEVEL), &spNodeServerLogLevel);
	if ((hr != S_OK) || !spNodeServerLogLevel)
		return E_FAIL;
	CComPtr <IXMLDOMNode> spNodeServerLogLevelValue;
	spNodeServerLogLevel->get_firstChild (&spNodeServerLogLevelValue);
	if ((hr != S_OK) || !spNodeServerLogLevelValue)
		return E_FAIL;
	VARIANT vLogLevel;
	vLogLevel.intVal = 0;
	spNodeServerLogLevelValue->get_nodeValue (&vLogLevel);
	if (!vLogLevel.bstrVal)
		return E_FAIL;
	m_nLogLevel = ::_wtoi (vLogLevel.bstrVal);
	
	// load deepzoom config template
	return loadDeepZoomConfig (hInstance, pcwDeepZoomConfigRes);
}

HRESULT IGConfigManager::LoadUserConfiguration (HINSTANCE hInstance, LPCWSTR pwConfigRes)
{	
	if (m_wsUserLogin.empty())
		return E_FAIL;

	// construct the user account path
	m_wsUserPath = m_wsUserRootPath;
	m_wsUserPath += L"\\";
	m_wsUserPath += m_wsUserLogin.c_str();
	if (m_wsUserPath.length() >= MAX_PATH)
		return E_FAIL;

	m_wsUserTempPath = m_wsUserPath;
	m_wsUserTempPath += IGCONFIGMANAGER_TEMPPATH;
	if (m_wsUserTempPath.length() >= MAX_PATH)
		return E_FAIL;	

	// construct the user picture path
	m_wsUserPicturesPath = m_wsUserPath;
	m_wsUserPicturesPath += IGCONFIGMANAGER_IMAGEPATH;
	if (m_wsUserPicturesPath.length() >= MAX_PATH)
		return E_FAIL;	

	// construct the user picture path
	m_wsUserPicturesBMPath = m_wsUserPicturesPath;
	m_wsUserPicturesBMPath += IGCONFIGMANAGER_BEETLEMORPH;
	if (m_wsUserPicturesBMPath.length() >= MAX_PATH)
		return E_FAIL;	

	// construct the user mini picture path
	m_wsUserMiniPicturesPath = m_wsUserPicturesPath;
	m_wsUserMiniPicturesPath += IGCONFIGMANAGER_IMAGEMINIPATH;
	if (m_wsUserMiniPicturesPath.length() >= MAX_PATH)
		return E_FAIL;		
	
	// construct the config path
	m_wsConfigPath = m_wsUserPath;
	m_wsConfigPath += IGCONFIGMANAGER_CONFIGPATH;
	if (m_wsConfigPath.length() >= MAX_PATH)
		return E_FAIL;	

	// construct the history path
	m_wsUserHistoryPath = m_wsUserPath;
	m_wsUserHistoryPath += IGCONFIGMANAGER_HISTORYPATH;	 
	if (m_wsUserHistoryPath.length() >= MAX_PATH)
		return E_FAIL;	

	// construct the upload path
	wstring wsUserUploadPath = m_wsUserPath + IGCONFIGMANAGER_UPLOADPATH;
	if (wsUserUploadPath.length() >= MAX_PATH)
		return E_FAIL;	

	// construct the output path
	m_wsUserOutputPath = m_wsOutputPath + m_wsUserOutputLogin;
	IGFrame::SetOutputPath (m_wsUserOutputPath.c_str());

	// construct the mini picture output path
	m_wsUserMiniPicturesOutputPath = m_wsUserOutputPath + L"\\";
	m_wsUserMiniPicturesOutputPath += IGCONFIGMANAGER_IMAGEMINIPATH;	 
	if (m_wsUserMiniPicturesOutputPath.length() >= MAX_PATH)
		return E_FAIL;	

	if (FAILED (readConfiguration (hInstance, pwConfigRes, VARIANT_FALSE)))
	{
		if (FAILED (readConfiguration (hInstance, pwConfigRes, VARIANT_TRUE)))
			return E_FAIL;
	}
	SetUserPicturesPath (m_wsUserPicturesPath);
	int nRes = ::SHCreateDirectory (NULL, m_wsUserPicturesPath.c_str());
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return E_FAIL;
	nRes = ::SHCreateDirectory (NULL, m_wsUserPicturesBMPath.c_str());
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return E_FAIL;	
	nRes = ::SHCreateDirectory (NULL, m_wsUserMiniPicturesPath.c_str());
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return E_FAIL;	
	nRes = ::SHCreateDirectory (NULL, wsUserUploadPath.c_str());
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return E_FAIL;	
	return S_OK;
}

HRESULT IGConfigManager::LoadConfiguration (HINSTANCE hInstance, LPCWSTR pwConfigRes)
{
	if (m_bLoaded)
		return S_FALSE;
	// construct the config path
	wchar_t pwUserAppDataPath [MAX_PATH];
	HRESULT hr = ::SHGetFolderPath (NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, pwUserAppDataPath);
	if (FAILED(hr))
		return E_FAIL;
	m_wsConfigPath = pwUserAppDataPath;
	m_wsConfigPath += IGCONFIGMANAGER_CONFIGPATH;
	if (m_wsConfigPath.length() >= MAX_PATH)
		return E_FAIL;
	// construct the user path
	m_wsUserPath = pwUserAppDataPath;
	// construct the history path
	m_wsUserHistoryPath = pwUserAppDataPath;
	m_wsUserHistoryPath += IGCONFIGMANAGER_HISTORYPATH;
	if (m_wsUserHistoryPath.length() >= MAX_PATH)
		return E_FAIL;
	if (FAILED (readConfiguration (hInstance, pwConfigRes, VARIANT_FALSE)))
	{
		::MessageBoxW (NULL, L"Configuration file is corrupted, switching back to default settings", L"Imagenius", MB_ICONWARNING);
		if (FAILED (readConfiguration (hInstance, pwConfigRes, VARIANT_TRUE)))
		{
			::MessageBoxW (NULL, L"Could not recover default settings\nCleaning your browser cache and/or restarting your computer may fix the problem", L"Imagenius", MB_ICONERROR);
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT IGConfigManager::SaveConfiguration()
{
	saveAndClearListPicturesPath();
	HRESULT hr = S_OK;
	if (!m_spXMLDoc || m_wsConfigPath.empty())
		hr = E_FAIL;
	if (hr == S_OK)
		hr = m_spXMLDoc->save (CComVariant (m_wsConfigPath.c_str()));	
	return hr;
}

HRESULT IGConfigManager::CleanDirectories() const
{
	//garbage all output files
	if (!m_wsUserLogin.empty())
	{
		int nRes = ::SHCreateDirectory (NULL, m_wsUserOutputPath.c_str());
		if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS)){
			wstring wsErr (L"Could not create user output folder ");
			wsErr += m_wsUserOutputPath;
			wsErr += L", error code ";
			wchar_t twCode[32];
			wsErr += _itow (nRes, twCode, 10);
			IGLog log;
			log.AddWithHeader(wsErr.c_str(), L"IGConfigManager::CleanDirectories error - ");
			return E_FAIL;
		}
		garbageFolder (m_wsUserOutputPath.c_str());
		nRes = ::SHCreateDirectory (NULL, m_wsUserOutputPath.c_str());
		if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
			return E_FAIL;
		nRes = ::SHCreateDirectory (NULL, m_wsUserMiniPicturesOutputPath.c_str());
		if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
			return E_FAIL;	
		return S_OK;
	}
	return S_FALSE;
}

void IGConfigManager::GetUserPicturesPath (wstring &wsUserPicturesPath) const
{
	if (m_spNodeUserPicturesPathValue)
	{
		VARIANT vUserPicturesPath;
		m_spNodeUserPicturesPathValue->get_nodeValue (&vUserPicturesPath);
		wsUserPicturesPath = vUserPicturesPath.bstrVal ? vUserPicturesPath.bstrVal : L"";
	}
}

void IGConfigManager::GetLogPath (wstring &wsLogPath) const
{
	if (IsServerConfig())
	{
		wsLogPath = m_wsLogPath;
	}
	else
	{
		wchar_t pwTempPath [MAX_PATH];
		::GetTempPathW (MAX_PATH, pwTempPath);
		wsLogPath = pwTempPath;
	}
}

void IGConfigManager::SetLogPath (const std::wstring &wsLogPath)
{
	m_wsLogPath = wsLogPath;
}

HRESULT IGConfigManager::GetFileList (const wstring wsFolderPath, IEnumIDList **ppFileList, IShellFolder **ppFolder) const
{
	int nRes = ::SHCreateDirectory (NULL, wsFolderPath.c_str());
	if ((nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return E_FAIL;
	CComPtr <IShellFolder> spDesktopFolder;
	HRESULT hr = ::SHGetDesktopFolder (&spDesktopFolder);
	if (FAILED(hr))
		return hr;
	LPITEMIDLIST pidCurFoler = NULL;
	wchar_t pwFolderPath [_MAX_PATH];
	ZeroMemory (pwFolderPath, _MAX_PATH * sizeof (wchar_t));
	::wcscpy_s (pwFolderPath, wsFolderPath.size() + 1, wsFolderPath.c_str());
	hr = spDesktopFolder->ParseDisplayName (NULL, NULL, pwFolderPath, NULL, &pidCurFoler, NULL);
	if (FAILED(hr))
		return hr;
	if (!pidCurFoler)
		return E_FAIL;
	hr = spDesktopFolder->BindToObject (pidCurFoler, NULL, IID_IShellFolder, (void **)ppFolder);
	if (FAILED(hr))
		return hr;
	hr = (*ppFolder)->EnumObjects (NULL, SHCONTF_NONFOLDERS, ppFileList);
	if (FAILED(hr))
		return hr;
	return S_OK;
}

HRESULT IGConfigManager::SetUserPicturesPath (const wstring &wUserPicturesPath)
{
	VARIANT vUserPicturesPath;	
	vUserPicturesPath.vt = VT_BSTR;
	vUserPicturesPath.bstrVal = CComBSTR (wUserPicturesPath.c_str());
	if (m_spNodeUserPicturesPathValue)
	{
		m_spNodeUserPicturesPathValue->put_nodeValue (vUserPicturesPath);
	}
	else
	{
		CComPtr <IXMLDOMNode> spNodeUserPicturesPath;
		HRESULT hr = m_spXMLDoc->createNode (CComVariant (NODE_TEXT), 
												CComBSTR (L"FolderPath"), 
												NULL,
												&spNodeUserPicturesPath);
		if ((hr != S_OK) || !spNodeUserPicturesPath)
			return E_FAIL;	
		spNodeUserPicturesPath->put_nodeValue (vUserPicturesPath);
		m_spNodeUserPicturesPath->appendChild (spNodeUserPicturesPath, &m_spNodeUserPicturesPathValue);	
	}
	return S_OK;
}

bool IGConfigManager::AddPicture (const wstring &wPicturePath, const CComBSTR &bstrMiniPictureBase64)
{
	if (Contains (wPicturePath))
		return false;
	IGSmartPtr <IGLibPicture> spNewPicture (new IGLibPicture (wPicturePath, bstrMiniPictureBase64));
	m_lspPictures.push_back (spNewPicture);
	return true;
}

bool IGConfigManager::AddPicture (IGSmartPtr <IGLibPicture> const &spLibPicture)
{
	if (Contains (spLibPicture->m_wsPath))
		return false;
	if (!spLibPicture)
		return false;
	m_lspPictures.push_back (spLibPicture);
	return true;
}

void IGConfigManager::RemovePicture (const wstring &wPicturePath)
{
	IGSmartPtr <IGLibPicture> spPicture;
	if (GetPicture (wPicturePath, spPicture))
	{
		m_lspPictures.remove (spPicture);
	}
}

bool IGConfigManager::Contains (const wstring &wPicturePath)
{
	for (list<IGSmartPtr <IGLibPicture>>::const_iterator iterPict = m_lspPictures.begin(); 
		iterPict != m_lspPictures.end();
		++iterPict)
	{
		if ((*iterPict)->m_wsPath == wPicturePath)
			return true;
	}
	return false;
}

bool IGConfigManager::GetPicture (const wstring &wPicturePath, IGSmartPtr <IGLibPicture> &spPictureFound) const
{
	for (list<IGSmartPtr <IGLibPicture>>::const_iterator iterPict = m_lspPictures.begin(); 
		iterPict != m_lspPictures.end();
		++iterPict)
	{
		if ((*iterPict)->m_wsPath == wPicturePath)
		{
			spPictureFound = *iterPict;
			return true;
		}
	}
	return false;
}

void IGConfigManager::SetServerConfig(wchar_t *pwServerIP, int nInputPortId)
{
	m_bIsServer = true;
	if(m_bIsServer && pwServerIP){
		m_wsServerIP = pwServerIP;
		m_nInputPortId = nInputPortId;
	}
}

void IGConfigManager::DisconnectUser()
{
	SaveConfiguration();
	m_wsUserLogin.clear();
	m_lwsMiniPicturePath.clear();
}

void IGConfigManager::AddMiniPicture(wstring wsMiniPicPath, int nWidth, int nHeight)
{
	m_lwsMiniPicturePath.push_back (wsMiniPicPath);
	wchar_t pwSize [32];
	::_itow_s (nWidth, pwSize, 10);
	m_lwsMiniPicturePath.push_back (std::wstring (pwSize));
	::_itow_s (nHeight, pwSize, 10);
	m_lwsMiniPicturePath.push_back (std::wstring (pwSize));
}

void IGConfigManager::GetMiniPictures(wstring& wsMiniPictures)
{
	wsMiniPictures.clear();
	list<wstring>::const_iterator iterMiniPictures;
	for (iterMiniPictures = m_lwsMiniPicturePath.begin(); iterMiniPictures != m_lwsMiniPicturePath.end();)
	{
		wsMiniPictures += (*iterMiniPictures);
		++iterMiniPictures;
		if (iterMiniPictures != m_lwsMiniPicturePath.end())
			wsMiniPictures += L",";
	}
}

HRESULT IGConfigManager::InterlockSaveDeepZoomConfig (int nWidth, int nHeight, wstring wsDeepZoomOutputPath)
{
	std::pair <CComPtr <IXMLDOMDocument>, CComPtr <IXMLDOMNamedNodeMap>> pairXMLDeepZoomConfig = m_threadDataDeepZoomConfig.Lock();
	HRESULT hr = S_OK;
	if (!pairXMLDeepZoomConfig.first || !pairXMLDeepZoomConfig.second || wsDeepZoomOutputPath.empty())
		hr = E_FAIL;
	if (hr == S_OK)
	{
		if (m_nLastDeepZoomWidth != nWidth)
		{
			CComPtr <IXMLDOMAttribute> spAttributeWidth;
			hr = pairXMLDeepZoomConfig.first->createAttribute (CComBSTR (L"Width"), &spAttributeWidth);
			if ((hr == S_OK) && spAttributeWidth)
			{
				wchar_t twWidth [8];
				::_itow_s (nWidth, twWidth, 10);
				hr = spAttributeWidth->put_value (CComVariant (CComBSTR (twWidth))); _ASSERTE ((hr == S_OK) && L"MSXml Error in IGConfigManager::InterlockSaveDeepZoomConfig");
				CComPtr <IXMLDOMNode> spAttributeWidthAdded;
				hr = pairXMLDeepZoomConfig.second->setNamedItem (spAttributeWidth, &spAttributeWidthAdded); _ASSERTE ((hr == S_OK) && L"MSXml Error in IGConfigManager::InterlockSaveDeepZoomConfig");
			}
		}
		if ((hr == S_OK) && (m_nLastDeepZoomHeight != nHeight))
		{
			CComPtr <IXMLDOMAttribute> spAttributeHeight;			
			hr = pairXMLDeepZoomConfig.first->createAttribute (CComBSTR (L"Height"), &spAttributeHeight);
			if ((hr == S_OK) && spAttributeHeight)
			{
				wchar_t twHeight [8];
				::_itow_s (nHeight, twHeight, 10);
				hr = spAttributeHeight->put_value (CComVariant (CComBSTR (twHeight))); _ASSERTE ((hr == S_OK) && L"MSXml Error in IGConfigManager::InterlockSaveDeepZoomConfig");
				CComPtr <IXMLDOMNode> spAttributeHeightAdded;
				hr = pairXMLDeepZoomConfig.second->setNamedItem (spAttributeHeight, &spAttributeHeightAdded); _ASSERTE ((hr == S_OK) && L"MSXml Error in IGConfigManager::InterlockSaveDeepZoomConfig");
			}
		}
		if (hr == S_OK)
			hr = pairXMLDeepZoomConfig.first->save (CComVariant (wsDeepZoomOutputPath.c_str())); _ASSERTE ((hr == S_OK) && L"MSXml Error in IGConfigManager::InterlockSaveDeepZoomConfig");
	}
	m_threadDataDeepZoomConfig.UnLock();
	return hr;	
}

HRESULT IGConfigManager::readConfiguration (HINSTANCE hInstance, LPCWSTR pwConfigRes, BOOL bOverwrite)
{
	// read the config file
	HANDLE hFile = ::CreateFile (m_wsConfigPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (bOverwrite)
	{
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return E_FAIL; // cannot overwrite non existing file
		}
		else
		{
			::CloseHandle (hFile);
			BOOL bSuccess = ::DeleteFile (m_wsConfigPath.c_str());
			if (bSuccess)
				hFile = INVALID_HANDLE_VALUE;
			else
				return E_FAIL; // impossible to delete corrupted file
		}
	}
	int nRes = ::SHCreateDirectory (NULL, m_wsUserHistoryPath.c_str());
	if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
		return E_FAIL;
	
	// delete all history files
	if ((nRes == ERROR_FILE_EXISTS) || (nRes == ERROR_ALREADY_EXISTS))
		deleteHistory();

	HRESULT hr = S_OK;
	if (!m_spXMLDoc)
		hr = ::CoCreateInstance(CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
									IID_IXMLDOMDocument, (void**)&m_spXMLDoc);
	if (FAILED(hr))
		return E_FAIL;

	char *pXmlFileBuffer = NULL;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CAtlString strConfigPath (IGCONFIGMANAGER_CONFIGPATH);
		CAtlString resToken, nextToken, nextNextToken;
		int curPos = 0;
		wstring wsConstructPath (m_wsUserPath);
		nextToken = strConfigPath.Tokenize(_T("\\"), curPos);
		nextNextToken = strConfigPath.Tokenize(_T("\\"), curPos);
		do
		{
			resToken = nextToken;
			nextToken = nextNextToken;
			nextNextToken = strConfigPath.Tokenize(_T("\\"), curPos);
			wsConstructPath += L"\\" + resToken;
			::CreateDirectory (wsConstructPath.c_str(), NULL);			
		}while (nextNextToken != _T(""));

		hFile = ::CreateFile (m_wsConfigPath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,
								 NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return E_FAIL;
		// File does not exist, open the default config file
		char *pValidXml = IGConfigManager::CreateBufferFromRes (hInstance, pwConfigRes, L"Configuration");
		VARIANT_BOOL bSucceeded;
		hr = m_spXMLDoc->loadXML (CComBSTR(pValidXml), &bSucceeded);
		if ((hr != S_OK) || !bSucceeded)
			return E_FAIL;
		delete [] pValidXml;
		::CloseHandle (hFile);
	}
	else
	{
		::CloseHandle (hFile);
		VARIANT_BOOL bSucceeded;
		hr = m_spXMLDoc->load (CComVariant(m_wsConfigPath.c_str()), &bSucceeded);
		if ((hr != S_OK) || !bSucceeded)
			return E_FAIL;
	}
	if (m_spNodeCommonConfig)
		m_spNodeCommonConfig.Release();
	hr = m_spXMLDoc->selectSingleNode (CComBSTR("./Common"), &m_spNodeCommonConfig);
	if ((hr != S_OK) || !m_spNodeCommonConfig)
		return E_FAIL;

	// get user picture folder
	if (m_spNodeUserPicturesPath)
		m_spNodeUserPicturesPath.Release();
	hr = m_spNodeCommonConfig->selectSingleNode (CComBSTR("./UserPicturesPath"), &m_spNodeUserPicturesPath);
	if ((hr != S_OK) || !m_spNodeUserPicturesPath)
		return E_FAIL;

	VARIANT_BOOL bHasChild = VARIANT_FALSE;
	m_spNodeUserPicturesPath->hasChildNodes (&bHasChild);
	if (m_spNodeUserPicturesPathValue)
		m_spNodeUserPicturesPathValue.Release();
	if (bHasChild)
		m_spNodeUserPicturesPath->get_firstChild (&m_spNodeUserPicturesPathValue);

	// get picture list
	if (m_spNodeListPicturesPath)
		m_spNodeListPicturesPath.Release();
	hr = m_spNodeCommonConfig->selectSingleNode (CComBSTR("./Pictures"), &m_spNodeListPicturesPath);
	if ((hr != S_OK) || !m_spNodeListPicturesPath)
		return E_FAIL;

	initializeListPicturesPath ();

	m_bLoaded = true;
	return S_OK;
}

void IGConfigManager::initializeListPicturesPath ()
{
	m_lspPictures.clear();
	VARIANT_BOOL bHasChildNodes;
	m_spNodeListPicturesPath->hasChildNodes (&bHasChildNodes);
	if (bHasChildNodes)
	{
		// read picture list
		CComPtr <IXMLDOMNodeList> spPictureList;
		HRESULT hr = m_spNodeListPicturesPath->selectNodes (CComBSTR("./Picture"), &spPictureList);
		if (hr == S_OK && spPictureList)
		{
			long nNbPictures = 0;
			spPictureList->get_length (&nNbPictures);
			for (int nIdxPicture = 0; nIdxPicture < nNbPictures; nIdxPicture++)
			{
				CComPtr <IXMLDOMNode> spXMLNode;
				hr = spPictureList->get_item (nIdxPicture, &spXMLNode);
				if ((hr != S_OK) || !spXMLNode)
					continue;

				// read picture path
				CComPtr <IXMLDOMNode> spXMLPathNode;
				hr = spXMLNode->selectSingleNode (CComBSTR("./Path"), &spXMLPathNode);
				if ((hr != S_OK) || !spXMLPathNode)
					continue;
				
				CComPtr <IXMLDOMNode> spXMLNodeValue;
				hr = spXMLPathNode->get_firstChild (&spXMLNodeValue);
				if ((hr != S_OK) || !spXMLNodeValue)
					continue;

				VARIANT vPicturePath;
				hr = spXMLNodeValue->get_nodeValue(&vPicturePath);
				if ((hr != S_OK) || !vPicturePath.bstrVal)
					continue;

				// read mini-picture
				CComPtr <IXMLDOMNode> spXMLMiniPictureNode;
				hr = spXMLNode->selectSingleNode (CComBSTR("./Mini"), &spXMLMiniPictureNode);
				if ((hr != S_OK) || !spXMLMiniPictureNode)
					continue;

				CComPtr <IXMLDOMNode> spXMLMiniPictureNodeValue;
				hr = spXMLMiniPictureNode->get_firstChild (&spXMLMiniPictureNodeValue);
				if ((hr != S_OK) || !spXMLMiniPictureNodeValue)
					continue;

				VARIANT vMiniPicture;
				hr = spXMLMiniPictureNodeValue->get_nodeValue(&vMiniPicture);
				if ((hr != S_OK) || !vMiniPicture.bstrVal)
					continue;

				// everything is alright! add picture
				AddPicture (vPicturePath.bstrVal, vMiniPicture.bstrVal);
			}
		}
	}
}

HRESULT IGConfigManager::saveAndClearListPicturesPath()
{
	// TODO: to be replaced by commit config
	if (!m_bLoaded)
		return S_FALSE;
	// destroy previous picture list
	m_spNodeCommonConfig->removeChild (m_spNodeListPicturesPath, NULL);
	m_spNodeListPicturesPath.Release();

	// create new list
	CComPtr <IXMLDOMNode> spNodePictureList;
	HRESULT hr = m_spXMLDoc->createNode (CComVariant (NODE_ELEMENT), 
											CComBSTR (L"Pictures"), 
											NULL,
											&spNodePictureList);
	if ((hr != S_OK) || !spNodePictureList)
			return E_FAIL;
	m_spNodeCommonConfig->appendChild (spNodePictureList, &m_spNodeListPicturesPath);

	while (!m_lspPictures.empty())
	{
		CComPtr <IXMLDOMNode> spNodePicture, spNodePictureOut;
		hr = m_spXMLDoc->createNode (CComVariant (NODE_ELEMENT), 
										CComBSTR (L"Picture"), 
										NULL,
										&spNodePicture);
		if ((hr != S_OK) || !spNodePicture)
			return E_FAIL;
		m_spNodeListPicturesPath->appendChild (spNodePicture, &spNodePictureOut);

		// save picture path
		CComPtr <IXMLDOMNode> spNodePicturePath, spNodePicturePathOut;
		hr = m_spXMLDoc->createNode (CComVariant (NODE_ELEMENT), 
										CComBSTR (L"Path"), 
										NULL,
										&spNodePicturePath);
		if ((hr != S_OK) || !spNodePicturePath)
			return E_FAIL;
		spNodePictureOut->appendChild (spNodePicturePath, &spNodePicturePathOut);

		CComPtr <IXMLDOMNode> spNodePicturePathValue;
		hr = m_spXMLDoc->createNode (CComVariant (NODE_TEXT), 
										CComBSTR (L"PathValue"), 
										NULL,
										&spNodePicturePathValue);
		if ((hr != S_OK) || !spNodePicturePathValue)
			return E_FAIL;
		VARIANT vPicturePath;
		vPicturePath.vt = VT_BSTR;
		vPicturePath.bstrVal = CComBSTR (m_lspPictures.front()->m_wsPath.c_str());
		spNodePicturePathValue->put_nodeValue (vPicturePath);
		spNodePicturePathOut->appendChild (spNodePicturePathValue, NULL);		

		// save mini picture
		CComPtr <IXMLDOMNode> spNodeMiniPicture, spNodeMiniPictureOut;
		hr = m_spXMLDoc->createNode (CComVariant (NODE_ELEMENT), 
										CComBSTR (L"Mini"), 
										NULL,
										&spNodeMiniPicture);
		if ((hr != S_OK) || !spNodeMiniPicture)
			return E_FAIL;
		spNodePictureOut->appendChild (spNodeMiniPicture, &spNodeMiniPictureOut);

		CComPtr <IXMLDOMNode> spNodeMiniPictureValue;
		hr = m_spXMLDoc->createNode (CComVariant (NODE_TEXT), 
										CComBSTR (L"MiniValue"), 
										NULL,
										&spNodeMiniPictureValue);
		if ((hr != S_OK) || !spNodeMiniPictureValue)
			return E_FAIL;
		VARIANT vMiniPicture;
		vMiniPicture.vt = VT_BSTR;
		vMiniPicture.bstrVal = m_lspPictures.front()->m_bstrMiniPictureBase64;
		spNodeMiniPictureValue->put_nodeValue (vMiniPicture);
		spNodeMiniPictureOut->appendChild (spNodeMiniPictureValue, NULL);

		m_lspPictures.pop_front();
	}
	return S_OK;
}

void IGConfigManager::deleteHistory() const
{
	DeleteDirectory (m_wsUserHistoryPath.c_str());
}

bool IGConfigManager::DestroyUserAccount() const
{
	return DeleteDirectory (m_wsUserPath.c_str());
}

void IGConfigManager::garbageFolder (const wchar_t *pcwFolder) const
{
	if (!pcwFolder)
		return;
	wchar_t twGarbageFolder[_MAX_PATH];
	int nRetry = 10;
	do
	{
		::_swprintf_p (twGarbageFolder, _MAX_PATH, L"%s@@GARBAGE@@(%d)", pcwFolder, rand());
	}
	while (_wrename (pcwFolder, twGarbageFolder) != 0 && (--nRetry > 0));
}

HRESULT IGConfigManager::loadDeepZoomConfig (HINSTANCE hInstance, LPCWSTR pcwDeepZoomConfigRes)
{
	HRESULT hr = S_OK;
	CComPtr <IXMLDOMDocument> spXMLDoc;
	hr = ::CoCreateInstance(CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
									IID_IXMLDOMDocument, (void**)&spXMLDoc);
	if (FAILED(hr))
		return hr;
	VARIANT_BOOL bSucceeded;
	char *pValidXml = IGConfigManager::CreateBufferFromRes (hInstance, pcwDeepZoomConfigRes, L"Configuration");
	hr = spXMLDoc->loadXML (CComBSTR(pValidXml), &bSucceeded);
	if ((hr != S_OK) || !bSucceeded)
		return E_FAIL;
	delete [] pValidXml;	
	CComPtr <IXMLDOMNode> spNodeImageConfig;
	hr = spXMLDoc->selectSingleNode (CComBSTR("./Image/Size"), &spNodeImageConfig);
	if ((hr != S_OK) || !spNodeImageConfig)
		return E_FAIL;	
	CComPtr <IXMLDOMNamedNodeMap> spXMLImageSizeAttributes;
	hr = spNodeImageConfig->get_attributes (&spXMLImageSizeAttributes);
	if ((hr != S_OK) || !spXMLImageSizeAttributes)
		return E_FAIL;
	m_threadDataDeepZoomConfig = std::pair <CComPtr <IXMLDOMDocument>, CComPtr <IXMLDOMNamedNodeMap>> (spXMLDoc, spXMLImageSizeAttributes);
	return S_OK;
}

void IGConfigManager::SetUserLogin (LPCWSTR pwUserLogin)
{
	wstring wsDemoHeader (IGCONFIGMANAGER_DEMOACCOUNTHEADER);
	m_wsUserOutputLogin = pwUserLogin;
	m_wsUserLogin = pwUserLogin;
	if (m_wsUserLogin.find (wsDemoHeader.c_str()) != wstring::npos)
		m_wsUserLogin = wsDemoHeader.substr (0, wsDemoHeader.size() - 1);
}