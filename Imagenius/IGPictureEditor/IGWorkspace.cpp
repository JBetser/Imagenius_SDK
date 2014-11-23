// IGWorkspace.cpp : Implementation of CIGWorkspace

#include "stdafx.h"
#include "IGWorkspace.h"
#include "IGUserMessages.h"
#include "IGTexture.h"
#include <IGFrame.h>
#include <IGSmartPtr.h>
#include <gdiplus.h>
#include <shlobj.h>

#define IGWORKSPACE_UNSAVEDFILENAME		L"Unsaved"
#define IGWORKSPACE_RECOVEREDFILENAME	L"Recovered"

using namespace Gdiplus;
using namespace IGLibrary;

#define IGWORKSPACE_LISTBOX_HEIGHT	80
#define IGWORKSPACE_ITEMWIDTH		76
#define IGWORKSPACE_ITEMHEIGHT		76
#define IGWORKSPACE_ITEM_X			2
#define IGWORKSPACE_ITEM_Y			4
#define IGWORKSPACE_BUTTON_X		5
#define IGWORKSPACE_BUTTON_Y		35
#define IGWORKSPACE_BUTTON_HEIGHT	55
#define IGWORKSPACE_BUTTON_WIDTH	20
#define IGWORKSPACE_BUTTONNEW_X		5
#define IGWORKSPACE_BUTTONNEW_WIDTH		55
#define IGWORKSPACE_BUTTONNEW_HEIGHT 80
#define IGWORKSPACE_COLOR_TRIANGLE		Color(100, 0, 0, 0)			// Black

// CIGWorkspace
CIGWorkspace::CIGWorkspace()	: m_hWnd (NULL)
								, m_hListBox (NULL)
								, m_hButtonLeft (NULL)
								, m_hButtonRight (NULL)
								, m_hButtonNew (NULL)
								, m_eButtonState (IGBUTTON_MOUSEOUT)
								, m_hOverButton (NULL)
								, m_nNbVisibleItems (0)
								, m_nCurrentScrollPos (0)
{
	::ZeroMemory (&m_rcWindow, sizeof (RECT));
	::ZeroMemory (&m_rcListBox, sizeof (RECT));
	::ZeroMemory (&m_rcButtonLeft, sizeof (RECT));
	::ZeroMemory (&m_rcButtonRight, sizeof (RECT));
	::ZeroMemory (&m_rcButtonNew, sizeof (RECT));

	HRSRC hResIcon = ::FindResourceW (getInstance(), MAKEINTRESOURCE(IDB_NEWFRAME), L"PNG");
	m_cxIconNewFrame.LayerCreate(0);
	m_cxIconNewFrame.GetLayer(0)->LoadResource (hResIcon, CXIMAGE_FORMAT_PNG, getInstance());
}

CIGWorkspace::~CIGWorkspace()
{
}

STDMETHODIMP CIGWorkspace::CreateFrame (IDispatch *pDispWindowOwner, IDispatch *pDispMultiFrame)
{
	m_spDockPanelOwner = pDispWindowOwner;
	m_spMultiFrame = pDispMultiFrame;
	if (!m_spDockPanelOwner)
		return E_INVALIDARG;
	m_spDockPanelOwner->GetHWND ((OLE_HANDLE*)&m_hWnd);
	::SetWindowSubclass (m_hWnd, StaticHookPanelProc, 0, (DWORD_PTR)this);
	::GetWindowRect (m_hWnd, &m_rcWindow);
	m_rcWindow.right -= m_rcWindow.left;
	m_rcWindow.bottom -= m_rcWindow.top;
	m_rcWindow.left = m_rcWindow.top = 0;
	Move (&m_rcWindow);

	// listbox
	m_hListBox = ::CreateWindowExW (WS_EX_LEFT, L"LISTBOX", L"",
									WS_CHILD | WS_VISIBLE |
									LBS_MULTICOLUMN  | LBS_MULTIPLESEL | LBS_OWNERDRAWFIXED | LBS_EXTENDEDSEL | LBS_HASSTRINGS | LBS_NOREDRAW | LBS_NOINTEGRALHEIGHT,
									m_rcListBox.left, m_rcListBox.top,
									m_rcListBox.right - m_rcListBox.left, m_rcListBox.bottom - m_rcListBox.top,
									m_hWnd,	NULL, getInstance(), NULL);
	::SendMessageW (m_hListBox, (UINT) LB_SETCOLUMNWIDTH, (WPARAM) IGWORKSPACE_COLUMNWIDTH, 0 );
	::SetWindowSubclass (m_hListBox, StaticHookListBoxProc, 0, (DWORD_PTR)this);
	populateListBox ();

	// buttons left/right
	m_hButtonLeft = ::CreateWindowW (L"Button", L"",
									WS_CHILD | WS_VISIBLE,
									m_rcButtonLeft.left, m_rcButtonLeft.top,
									m_rcButtonLeft.right - m_rcButtonLeft.left, m_rcButtonLeft.bottom - m_rcButtonLeft.top,
									m_hWnd,	NULL, getInstance(), NULL);
	::SetWindowSubclass (m_hButtonLeft, StaticHookButtonProc, 0, (DWORD_PTR)this);
	m_hButtonRight = ::CreateWindowW (L"Button", L"",
									WS_CHILD | WS_VISIBLE,
									m_rcButtonRight.left, m_rcButtonRight.top,
									m_rcButtonRight.right - m_rcButtonRight.left, m_rcButtonRight.bottom - m_rcButtonRight.top,
									m_hWnd,	NULL, getInstance(), NULL);
	::SetWindowSubclass (m_hButtonRight, StaticHookButtonProc, 0, (DWORD_PTR)this);
	m_hButtonNew = ::CreateWindowW (L"Button", L"",
									WS_CHILD | WS_VISIBLE,
									m_rcButtonNew.left, m_rcButtonNew.top,
									m_rcButtonNew.right - m_rcButtonNew.left, m_rcButtonNew.bottom - m_rcButtonNew.top,
									m_hWnd,	NULL, getInstance(), NULL);
	::SetWindowSubclass (m_hButtonNew, StaticHookButtonProc, 0, (DWORD_PTR)this);

	// Connect workspace to DockPanel events
	IDispEventSimpleImpl <1, CIGWorkspace, &__uuidof(_IIGDockPanelEvents)>::DispEventAdvise (pDispWindowOwner);
	return S_OK;
}

STDMETHODIMP CIGWorkspace::DestroyFrame ()
{	
	::RemoveWindowSubclass (m_hButtonNew, StaticHookButtonProc, 0);
	::RemoveWindowSubclass (m_hButtonLeft, StaticHookButtonProc, 0);
	::RemoveWindowSubclass (m_hButtonRight, StaticHookButtonProc, 0);
	::RemoveWindowSubclass (m_hListBox, StaticHookListBoxProc, 0);
	::RemoveWindowSubclass (m_hWnd, StaticHookPanelProc, 0);
	::DestroyWindow (m_hButtonNew);
	::DestroyWindow (m_hButtonLeft);
	::DestroyWindow (m_hButtonRight);
	::DestroyWindow (m_hListBox);
	::DestroyWindow (m_hWnd);
	return S_OK;
}

STDMETHODIMP CIGWorkspace::Move (LPRECT p_rc)
{
	m_rcWindow.right = p_rc->right - p_rc->left;
	m_rcWindow.bottom = p_rc->bottom - p_rc->top;
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	
	m_rcListBox.left = IGWORKSPACE_LISTBOX_X + IGWORKSPACE_BUTTON_X + IGWORKSPACE_BUTTON_WIDTH;
	m_rcListBox.right = m_rcWindow.right - m_rcWindow.left - IGWORKSPACE_LISTBOX_X - 2 * IGWORKSPACE_BUTTON_X - IGWORKSPACE_BUTTON_WIDTH - IGWORKSPACE_BUTTONNEW_WIDTH;
	if (m_rcListBox.left + nNbFrames * IGWORKSPACE_COLUMNWIDTH <= m_rcListBox.right)
		m_rcListBox.right = m_rcListBox.left + nNbFrames * IGWORKSPACE_COLUMNWIDTH;
	m_rcListBox.top = IGWORKSPACE_LISTBOX_Y;
	m_rcListBox.bottom = m_rcListBox.top + IGWORKSPACE_LISTBOX_HEIGHT;
	if (m_rcListBox.right < m_rcListBox.left)
		m_rcListBox.right = m_rcListBox.left;
	if (m_rcListBox.bottom < m_rcListBox.top)
		m_rcListBox.bottom = m_rcListBox.top;

	m_rcButtonLeft.left = IGWORKSPACE_BUTTON_X;
	m_rcButtonLeft.top = IGWORKSPACE_BUTTON_Y;
	m_rcButtonLeft.right = m_rcButtonLeft.left + IGWORKSPACE_BUTTON_WIDTH;
	m_rcButtonLeft.bottom = m_rcButtonLeft.top + IGWORKSPACE_BUTTON_HEIGHT;

	m_rcButtonNew.left = m_rcListBox.right + IGWORKSPACE_LISTBOX_X;
	m_rcButtonNew.top = IGWORKSPACE_LISTBOX_Y;
	m_rcButtonNew.right = m_rcButtonNew.left + IGWORKSPACE_BUTTONNEW_WIDTH;
	m_rcButtonNew.bottom = m_rcButtonNew.top + IGWORKSPACE_BUTTONNEW_HEIGHT;

	m_rcButtonRight.left = m_rcWindow.right - m_rcWindow.left - IGWORKSPACE_BUTTON_WIDTH - IGWORKSPACE_LISTBOX_X;
	m_rcButtonRight.top = IGWORKSPACE_BUTTON_Y;
	m_rcButtonRight.right = m_rcButtonRight.left + IGWORKSPACE_BUTTON_WIDTH;
	m_rcButtonRight.bottom = m_rcButtonLeft.top + IGWORKSPACE_BUTTON_HEIGHT;

	if (m_hListBox)
	{
		::MoveWindow (m_hListBox, m_rcListBox.left, m_rcListBox.top,
								m_rcListBox.right - m_rcListBox.left, m_rcListBox.bottom - m_rcListBox.top, TRUE);
	}
	if (m_hButtonLeft)
	{
		::MoveWindow (m_hButtonLeft, m_rcButtonLeft.left, m_rcButtonLeft.top,
								m_rcButtonLeft.right - m_rcButtonLeft.left, m_rcButtonLeft.bottom - m_rcButtonLeft.top, TRUE);
	}
	if (m_hButtonNew)
	{
		::MoveWindow (m_hButtonNew, m_rcButtonNew.left, m_rcButtonNew.top,
								m_rcButtonNew.right - m_rcButtonNew.left, m_rcButtonNew.bottom - m_rcButtonNew.top, TRUE);
	}
	if (m_hButtonRight)
	{
		::MoveWindow (m_hButtonRight, m_rcButtonRight.left, m_rcButtonRight.top,
								m_rcButtonRight.right - m_rcButtonRight.left, m_rcButtonRight.bottom - m_rcButtonRight.top, TRUE);
	}
	return S_OK;
}

STDMETHODIMP CIGWorkspace::AddFrame (BSTR bstrImagePath, VARIANT_BOOL bAbsolutePath, VARIANT_BOOL bAutoRotate)
{
	HRESULT hr = S_OK;
	if (!bstrImagePath)
	{
		// create a frame with a blank document
		return E_NOTIMPL;
	}
	else
	{
		if (IGFrame::GetImageType (bstrImagePath) == CXIMAGE_FORMAT_UNKNOWN)
			return E_INVALIDARG;
		wstring wsPath;
		if (bAbsolutePath)
			wsPath = bstrImagePath;
		else
		{
			if (m_spConfigMgr->IsServerConfig())
			{
				m_spConfigMgr->GetUserPicturesPath (wsPath);
				wsPath += L"\\";
			}
			wsPath += bstrImagePath;
		}
		hr = m_spMultiFrame->AddImage (CComBSTR (wsPath.c_str()), bAutoRotate);
		if (hr == S_OK)
		{
			int nItemId = ::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)L"Image"); 
			::SendMessage (m_hListBox, LB_SETITEMDATA, (WPARAM)nItemId, (LPARAM)nItemId);
		}
	}
	return hr;
}

STDMETHODIMP CIGWorkspace::RemoveFrame (LONG nFrameId)
{
	if (m_spMultiFrame->RemoveFrame (nFrameId) != S_OK)
		return E_FAIL;
	::PostMessageW (m_hListBox, LB_DELETESTRING, (WPARAM)nFrameId, 0);
	m_spDockPanelOwner->RedrawControls();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::SaveFrame (LONG nFrameId, VARIANT_BOOL bSilent, BSTR bstrImagePath, VARIANT_BOOL bCloseAfterSave)
{
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	if (nFrameId < 0 || nFrameId >= nNbFrames)
		return E_INVALIDARG;
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	m_spFrameMgr->GetFrame (nFrameId, spFrame);
	if (!spFrame)
		return E_FAIL;
	std::wstring wsFileName;
	OPENFILENAME ofn;
	::ZeroMemory (&ofn, sizeof(OPENFILENAME));
	HWND hWnd = NULL;
	if (bSilent)
	{
		if (bCloseAfterSave)
		{
			// not a user picture but a temporary file path
			wsFileName = bstrImagePath;
		}
		else
		{
			m_spConfigMgr->GetUserPicturesPath (wsFileName);
			if (bstrImagePath)
				wsFileName += bstrImagePath;
			else
				wsFileName = createTempFile();
		}
	}
	else
	{
		// open dialog box
		ofn.lStructSize = sizeof (OPENFILENAME);
		CComQIPtr <IOleWindow> spOleWindow (m_spMultiFrame);
		spOleWindow->GetWindow (&hWnd);
		ofn.hwndOwner = hWnd;	
		ofn.hInstance = getInstance();
		ofn.lpstrFilter = L"Imagenius Format (*.ig)\0*.ig\0JPEG (*.jpg)\0*.jpg\0PNG (*.png)\0*.png\0BMP (*.bmp)\0*.bmp\0GIF (*.gif)\0*.gif\0ICO (*.ico)\0*.ico\0TIFF (*.tiff)\0*.tiff\0\0";
		ofn.nFilterIndex = 1;
		DWORD nUserProfilePathLenght = 0;
		wchar_t pwUserPicturesPath [MAX_PATH];
		std::wstring wUserPicturesPath;
		m_spConfigMgr->GetUserPicturesPath (wUserPicturesPath);
		if (!wUserPicturesPath.empty())
			::wcscpy_s (pwUserPicturesPath, MAX_PATH, wUserPicturesPath.c_str());
		else
			::SHGetFolderPath (NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, pwUserPicturesPath);
		ofn.lpstrInitialDir = pwUserPicturesPath;
		wchar_t pwFileSelected [65536];
		::ZeroMemory (pwFileSelected, 65536);
		ofn.nMaxFile = 65536;
		ofn.lpstrFile = pwFileSelected;
		wchar_t pwFileTitle [MAX_PATH];
		::ZeroMemory (pwFileTitle, MAX_PATH);
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.lpstrFileTitle = pwFileTitle;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;
		if (!::GetSaveFileName (&ofn))
			return S_FALSE;
		wsFileName = ofn.lpstrFile;
	}
	LPWSTR pwExtension = ::PathFindExtensionW (wsFileName.c_str());
	if (pwExtension)
	{
		std::wstring wsExtension = pwExtension;
		wsFileName = wsFileName.substr (0, wsFileName.length() - wsExtension.length());
	}
	DWORD dwFileType = 0;
	if (bstrImagePath)
	{
		dwFileType = IGFrame::GetImageType (bstrImagePath);
	}
	else
	{
		// convert dialog filter index to file type
		switch (ofn.nFilterIndex)
		{
		case 1:
			dwFileType = CXIMAGE_FORMAT_IG;
			break;
		case 2:
			dwFileType = CXIMAGE_FORMAT_JPG;
			break;
		case 3:
			dwFileType = CXIMAGE_FORMAT_PNG;
			break;
		case 4:
			dwFileType = CXIMAGE_FORMAT_BMP;
			break;
		case 5:
			dwFileType = CXIMAGE_FORMAT_GIF;
			break;
		case 6:
			dwFileType = CXIMAGE_FORMAT_ICO;
			break;
		case 7:
			dwFileType = CXIMAGE_FORMAT_TIF;
			break;
		}
	}
	// add extension to path
	switch (dwFileType)
	{
	case CXIMAGE_FORMAT_IG:
		wsFileName += L".ig";
		break;
	case CXIMAGE_FORMAT_JPG:
		wsFileName += L".jpg";
		break;
	case CXIMAGE_FORMAT_PNG:
		wsFileName += L".png";
		break;
	case CXIMAGE_FORMAT_BMP:
		wsFileName += L".bmp";
		break;
	case CXIMAGE_FORMAT_GIF:
		wsFileName += L".gif";
		break;
	case CXIMAGE_FORMAT_ICO:
		wsFileName += L".ico";
		break;
	case CXIMAGE_FORMAT_TIF:
		wsFileName += L".tiff";
		break;
	}
	if (::PathFileExists (wsFileName.c_str()))
	{
		if (hWnd)
		{
			if (::MessageBoxW ((HWND)hWnd, L"The file exists.\nDo you want to overwrite it?", L"Image saving", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
				return S_FALSE;
		}
	}
	if (FAILED (spFrame->Save (wsFileName.c_str(), dwFileType, (bCloseAfterSave == VARIANT_TRUE))))
	{
		if (hWnd)
			::MessageBoxW ((HWND)hWnd, L"The file could not be saved.\nThe specified path may be invalid or locked.", L"Image saving error", MB_OK | MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetActiveFrame (OLE_HANDLE *p_hFrame)
{
	if (!p_hFrame)
		return E_INVALIDARG;
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (m_spFrameMgr->GetSelectedFrame (spFrame))
		*p_hFrame = (OLE_HANDLE)((IGLibrary::IGFrame*)spFrame);
	else
		*p_hFrame = NULL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::AddFrameFromHandle (OLE_HANDLE hMem)
{
	if (!hMem)
		return E_INVALIDARG;
	HRESULT hr = m_spMultiFrame->AddImageFromHandle (hMem);
	if (hr == S_OK)
	{
		int nItemId = ::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)L"Image"); 
		::SendMessage (m_hListBox, LB_SETITEMDATA, (WPARAM)nItemId, (LPARAM)nItemId);
	}
	return hr;
}

STDMETHODIMP CIGWorkspace::CreateNewFrame (LONG nWidth, LONG nHeight, LONG nColorMode, LONG nBackgroundMode)
{
	HRESULT hr = m_spMultiFrame->AddNewImage (nWidth, nHeight, nColorMode, nBackgroundMode);
	if (hr == S_OK)
	{
		int nItemId = ::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)L"Image"); 
		::SendMessage (m_hListBox, LB_SETITEMDATA, (WPARAM)nItemId, (LPARAM)nItemId);
	}
	return hr;
}

STDMETHODIMP CIGWorkspace::ConnectUser (BSTR bstrUserLogin, BSTR bstrGuid, LONG *p_nNbFrames)
{
	if (p_nNbFrames)
		*p_nNbFrames = 0;
	bool bUserLogged = m_spConfigMgr->IsUserLogged();
	HRESULT hr = S_OK;
	int nRes = ERROR_SUCCESS;
	if (!bUserLogged)
	{
		m_spConfigMgr->SetUserLogin (bstrUserLogin);
		hr = m_spConfigMgr->LoadUserConfiguration (getInstance(),
															MAKEINTRESOURCE(IDR_IGPICTUREEDITORCONFIG));
		if (SUCCEEDED (hr))
			hr = m_spConfigMgr->SaveConfiguration();	
		if (SUCCEEDED (hr))
			hr = m_spConfigMgr->CleanDirectories();
		if (SUCCEEDED (hr) && bstrGuid)
			hr = copyMiniPictures(bstrGuid);
		if (FAILED (hr))
		{
			DisconnectUser();
			return hr;
		}
	}
	if (p_nNbFrames){
		// search for files to recover
		wstring wsTempPath;
		m_spConfigMgr->GetUserTempPath (wsTempPath);
		nRes = ::SHCreateDirectory (NULL, wsTempPath.c_str());
		if ((nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
			return S_OK; // no file to recover	
		CComPtr <IEnumIDList> spFileList;
		CComPtr <IShellFolder> spFolder;
		hr = m_spConfigMgr->GetFileList (wsTempPath, &spFileList, &spFolder);	
		if (FAILED (hr))
			return S_OK; // temp file recovering is not critical
		LPITEMIDLIST pidCurFile = NULL;
		if (spFileList->Next (1, &pidCurFile, NULL) == S_OK)
		{
			*p_nNbFrames++;
			STRRET tempFile;
			if (FAILED (spFolder->GetDisplayNameOf (pidCurFile, SHGDN_NORMAL, &tempFile)))
				return S_OK;		
			wstring wsTempFilePath (wsTempPath);
			wsTempFilePath += tempFile.pOleStr;
			::CoTaskMemFree (tempFile.pOleStr);
			if (FAILED (AddFrame (CComBSTR (wsTempFilePath.c_str()), VARIANT_TRUE)))
				return S_OK;
			// leave now to wait all recovered files to be destroyed
			return S_FALSE;
		}
		// remove temp directory
		m_spConfigMgr->DeleteDirectory (wsTempPath.c_str());	
	}
	return S_OK;
}

STDMETHODIMP CIGWorkspace::DisconnectUser (VARIANT_BOOL bSaveAllToTemp)
{
	wstring wsUserTempPath;
	if (bSaveAllToTemp)
	{
		m_spConfigMgr->GetUserTempPath (wsUserTempPath);
		::SHCreateDirectory (NULL, wsUserTempPath.c_str());
	}
	wstring wsUserPictureTempPath;
	while (m_spFrameMgr->GetNbFrames() > 0)
	{
		if (bSaveAllToTemp)
		{
			wsUserPictureTempPath = createTempFile (true);		
			SaveFrame (0, VARIANT_TRUE, CComBSTR (wsUserPictureTempPath.c_str()), VARIANT_TRUE);
			// the disconnection will be done once the frame is saved
			return S_FALSE;
		}
		RemoveFrame (0);
	}
	m_spFrameMgr->Reset();
	m_spConfigMgr->DisconnectUser();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameProperty (LONG nFrameId, BSTR bstrPropId, VARIANT *pPropVal)
{
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (!pPropVal || !m_spFrameMgr->GetFrame (nFrameId, spFrame))
		return E_FAIL;
	if (!spFrame->GetPropertyVARIANT (bstrPropId, *pPropVal))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameNbLayers (BSTR *p_bstrNbLayers)
{
	std::wstring wsNbLayers;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			wchar_t twNbLayers[32];
			::_itow_s (spFrame->GetNbLayers(), twNbLayers, 10);
			wsNbLayers += twNbLayers;
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsNbLayers += L",";
		}
	}
	CComBSTR spbstrNbLayers (wsNbLayers.c_str());
	*p_bstrNbLayers = spbstrNbLayers.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameIds (BSTR *p_bstrIds)
{
	std::wstring wsIds;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			wsIds += spFrame->GetId();
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsIds += L",";
		}
	}
	CComBSTR spbstrIds (wsIds.c_str());
	*p_bstrIds = spbstrIds.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetActiveFrameId (BSTR *p_bstrId)
{
	std::wstring wsId;
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (m_spFrameMgr->GetFrame (m_spFrameMgr->GetNbFrames() - 1, spFrame))
		wsId = spFrame->GetId();	
	CComBSTR spbstrId (wsId.c_str());
	*p_bstrId = spbstrId.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetActiveFrameReqGuid (BSTR *p_bstrReqGuid)
{
	std::wstring wsId;
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (m_spFrameMgr->GetFrame (m_spFrameMgr->GetNbFrames() - 1, spFrame))
		wsId = spFrame->GetLastReqGuid();
	CComBSTR spbstrId (wsId.c_str());
	*p_bstrReqGuid = spbstrId.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameNames (BSTR *p_bstrNames)
{
	std::wstring wsNames;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			std::wstring wsCurName;
			spFrame->GetName (wsCurName);
			wsNames += wsCurName;
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsNames += L",";
		}
	}
	CComBSTR spbstrNames (wsNames.c_str());
	*p_bstrNames = spbstrNames.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameReqGuids (BSTR *p_bstrReqGuids)
{
	std::wstring wsReqGuids;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			std::wstring wsLastReqGuids = spFrame->GetLastReqGuid();
			wsReqGuids += wsLastReqGuids;
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsReqGuids += L",";
		}
	}
	CComBSTR spbstrReqGuids (wsReqGuids.c_str());
	*p_bstrReqGuids = spbstrReqGuids.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameProperties (BSTR *p_bstrFrameProperties)
{
	std::wstring wsFrameProperties;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			std::wstring wsCurFrameProperties;
			spFrame->GetPropertiesAndValues (wsCurFrameProperties);
			wsFrameProperties += wsCurFrameProperties;
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsFrameProperties += L",";
		}
	}
	CComBSTR spbstrFrameProperties (wsFrameProperties.c_str());
	*p_bstrFrameProperties = spbstrFrameProperties.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameStepIds (BSTR *p_bstrFrameStepIds)
{
	std::wstring wsFrameStepIds;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			std::wstring wsCurStepIds;
			spFrame->GetStepIds (wsCurStepIds);
			wsFrameStepIds += wsCurStepIds;
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsFrameStepIds += L",";
		}
	}
	CComBSTR spbstrFrameStepIds (wsFrameStepIds.c_str());
	*p_bstrFrameStepIds = spbstrFrameStepIds.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetFrameLayerVisibility (BSTR *p_bstrLayerVisibility)
{
	std::wstring wsLayerVisibility;
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			int nNbLayers = spFrame->GetNbLayers();
			for(int idxLayer = 0; idxLayer < nNbLayers; idxLayer++){
				wsLayerVisibility += spFrame->GetLayer(idxLayer)->GetVisible() ? L"1" : L"0";
				if (idxLayer < nNbLayers - 1)
					wsLayerVisibility += L"_";
			}
			if (nIterItem < m_spFrameMgr->GetNbFrames() - 1)
				wsLayerVisibility += L",";
		}
	}
	CComBSTR spbstrLayerVisibility (wsLayerVisibility.c_str());
	*p_bstrLayerVisibility = spbstrLayerVisibility.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::SetFrameProperty (LONG nFrameId, BSTR bstrPropId, VARIANT *pPropVal)
{
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (!pPropVal || !m_spFrameMgr->GetFrame (nFrameId, spFrame))
		return E_FAIL;
	if (!spFrame->SetPropertyVARIANT (bstrPropId, *pPropVal))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetProperty (BSTR bstrPropId, VARIANT *pPropVal)
{
	if (!pPropVal || !m_spFrameMgr->GetPropertyVARIANT (bstrPropId, *pPropVal))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::SetProperty (BSTR bstrPropId, VARIANT *pPropVal)
{
	if (!pPropVal || !m_spFrameMgr->SetPropertyVARIANT (bstrPropId, *pPropVal))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::GetPropertiesAndValues (BSTR *p_bstrPropId)
{
	std::wstring wsProperties;
	if (!m_spFrameMgr->GetPropertiesAndValues (wsProperties))
		return E_FAIL;
	CComBSTR spbstrProperties (wsProperties.c_str());
	*p_bstrPropId = spbstrProperties.Detach();
	return S_OK;
}

STDMETHODIMP CIGWorkspace::UpdateFrame (LONG nFrameId)
{
	IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
	if (!m_spFrameMgr->GetFrame (nFrameId, spFrame))
		return E_FAIL;
	if (!spFrame->Update())
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::Update ()
{
	int nTransp = 100;
	if (!m_spFrameMgr->GetProperty(IGProperties::IGPROPERTY_TRANSPARENCY, nTransp))
		return E_FAIL;
	if (nTransp < 0)
		return E_FAIL;
	nTransp = (int)((float)nTransp * 2.55f);
	IGTexture texture1, texture2;
	if (!m_spFrameMgr->GetProperty(IGProperties::IGPROPERTY_TEXTURE1, texture1) ||
		!m_spFrameMgr->GetProperty(IGProperties::IGPROPERTY_TEXTURE2, texture2))
		return E_FAIL;
	texture1.SetTransparency((BYTE)nTransp);
	texture2.SetTransparency((BYTE)nTransp);
	if (!m_spFrameMgr->SetProperty(IGProperties::IGPROPERTY_TEXTURE1, texture1) ||
		!m_spFrameMgr->SetProperty(IGProperties::IGPROPERTY_TEXTURE2, texture2))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::Copy (BSTR bstrImageGuid)
{
	if (!m_spFrameMgr->Copy(bstrImageGuid))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::Cut (BSTR bstrImageGuid)
{
	if (!m_spFrameMgr->Cut(bstrImageGuid))
		return E_FAIL;
	return S_OK;
}

STDMETHODIMP CIGWorkspace::Paste (BSTR bstrImageGuid)
{
	if (!m_spFrameMgr->Paste(bstrImageGuid))
		return E_FAIL;
	return S_OK;
}

bool CIGWorkspace::drawListBoxItem (HDC hdc, int nIdxItem)
{	
	int nNbItems = ::SendMessageW (m_hListBox, (UINT)LB_GETCOUNT, 0, 0);  
	//_ASSERTE ((nIdxItem < nNbItems) && L"The number of frames in the frame manager is higher than in the workspace");
	if (nIdxItem >= nNbItems)
		return false;
	int nFrameWidth = 0;
	int nFrameHeight = 0;
	m_spFrameMgr->GetFrameSize (nIdxItem, nFrameWidth, nFrameHeight);
	RECT rcItem;
	::ZeroMemory (&rcItem, sizeof (RECT));
	::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)nIdxItem, (LPARAM)&rcItem);  
	rcItem.left += IGWORKSPACE_ITEM_X;
	rcItem.right -= IGWORKSPACE_ITEM_X;
	rcItem.top += IGWORKSPACE_ITEM_Y;
	rcItem.bottom -= IGWORKSPACE_ITEM_Y;
	int nItemWidth = rcItem.right - rcItem.left;
	int nItemHeight = rcItem.bottom - rcItem.top;		
	float fRatio = 1.0f;
	if ((float)nItemWidth / (float)nItemHeight > (float)nFrameWidth / (float)nFrameHeight)
	{
		fRatio = (float)nItemHeight / (float)nFrameHeight;
		nFrameWidth = (int)((float)nFrameWidth * fRatio);			
		nFrameHeight = nItemHeight;
	}
	else
	{
		fRatio = (float)nItemWidth / (float)nFrameWidth;
		nFrameWidth = nItemWidth;
		nFrameHeight = (int)((float)nFrameHeight * fRatio);
	}

	rcItem.left += (nItemWidth - nFrameWidth) / 2;
	rcItem.top += (nItemHeight - nFrameHeight) / 2;

	RECT rcListBox;
	::GetWindowRect (m_hListBox, &rcListBox);	
	bool bFromCache = false;
	if ((m_rcListBox.right - m_rcListBox.left == rcListBox.right - rcListBox.left)
		&& (m_rcListBox.bottom - m_rcListBox.left == rcListBox.bottom - rcListBox.top))
	{
		bFromCache = true;
	}
	if ((rcItem.left >= 0)
		&& (rcItem.right < rcListBox.right - rcListBox.left - 1))
	{
		m_spMultiFrame->DrawFrame (nIdxItem, (OLE_HANDLE)hdc, (UINT_PTR)&rcItem);
		return true;
	}	
	return false;
}

void CIGWorkspace::populateListBox ()
{
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	int nItemId = 0;
	for (int nIterItem = 0; nIterItem < nNbFrames; nIterItem++)
	{
		nItemId = ::SendMessage (m_hListBox, LB_ADDSTRING, 0, (LPARAM)L"Image"); 
		::SendMessage (m_hListBox, LB_SETITEMDATA, (WPARAM)nItemId, (LPARAM)nItemId);
	}	
}

void CIGWorkspace::registerButtonPanel (CIGWorkspaceButtonPanel *pButtonPanel)
{
	m_spDockPanelOwner->RegisterControls ((OLE_HANDLE)pButtonPanel->m_hCloseWnd);
	m_spDockPanelOwner->RegisterControls ((OLE_HANDLE)pButtonPanel->m_hMaximizeWnd);
	m_spDockPanelOwner->RegisterControls ((OLE_HANDLE)pButtonPanel->m_hMinimizeWnd);
	m_spDockPanelOwner->RegisterControls ((OLE_HANDLE)pButtonPanel->m_hSaveWnd);
	m_spDockPanelOwner->RegisterControls ((OLE_HANDLE)pButtonPanel->m_hSaveAsWnd);
	m_spDockPanelOwner->RegisterControls ((OLE_HANDLE)pButtonPanel->m_hUploadWnd);
}

void CIGWorkspace::unregisterButtonPanel (CIGWorkspaceButtonPanel *pButtonPanel)
{
	m_spDockPanelOwner->UnRegisterControls ((OLE_HANDLE)pButtonPanel->m_hCloseWnd);
	m_spDockPanelOwner->UnRegisterControls ((OLE_HANDLE)pButtonPanel->m_hMaximizeWnd);
	m_spDockPanelOwner->UnRegisterControls ((OLE_HANDLE)pButtonPanel->m_hMinimizeWnd);
	m_spDockPanelOwner->UnRegisterControls ((OLE_HANDLE)pButtonPanel->m_hSaveWnd);
	m_spDockPanelOwner->UnRegisterControls ((OLE_HANDLE)pButtonPanel->m_hSaveAsWnd);
	m_spDockPanelOwner->UnRegisterControls ((OLE_HANDLE)pButtonPanel->m_hUploadWnd);
}

void CIGWorkspace::checkLeftRightButtonState ()
{
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	// manage showing of left and right buttons
	if (m_nCurrentScrollPos + m_nNbVisibleItems < nNbFrames)
	{
		if (!::IsWindowVisible (m_hButtonRight))
			::ShowWindow (m_hButtonRight, TRUE);
	}
	else
	{
		if (::IsWindowVisible (m_hButtonRight))
			::ShowWindow (m_hButtonRight, FALSE);
	}
	if (m_nCurrentScrollPos > 0)
	{
		if (!::IsWindowVisible (m_hButtonLeft))
			::ShowWindow (m_hButtonLeft, TRUE);
	}
	else
	{
		if (::IsWindowVisible (m_hButtonLeft))
			::ShowWindow (m_hButtonLeft, FALSE);
	}
}

LRESULT CIGWorkspace::OnDrawListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
	if (lpDrawItem)
	{
		drawListBoxItem (lpDrawItem->hDC, lpDrawItem->itemID);
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;
}

void CIGWorkspace::OnPanelOpening ()
{
	::EnableWindow (m_hListBox, FALSE);
	::ShowWindow (m_hButtonLeft, SW_HIDE);
	::ShowWindow (m_hButtonRight, SW_HIDE);
	m_nCurrentScrollPos = 0;
	::SendMessage (m_hListBox, WM_HSCROLL, MAKEWPARAM (SB_THUMBPOSITION, m_nCurrentScrollPos), NULL);

	// Refresh the cache of the small pictures
	for (int nIterItem = 0; nIterItem < m_spFrameMgr->GetNbFrames(); nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			spFrame->RedrawSecondary();
		}
	}
}

void CIGWorkspace::OnPanelOpened ()
{
	::EnableWindow (m_hListBox, TRUE);
	::EnableWindow (m_hButtonLeft, TRUE);
	::EnableWindow (m_hButtonRight, TRUE);	
	redrawButton (m_hButtonLeft);
	redrawButton (m_hButtonRight);
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	m_nNbVisibleItems = nNbFrames;
	RECT rcListBox;
	::GetWindowRect (m_hListBox, &rcListBox);
	RECT rcButtonPanel;
	ZeroMemory (&rcButtonPanel, sizeof (RECT));
	rcButtonPanel.left = -1 * m_nCurrentScrollPos * IGWORKSPACE_COLUMNWIDTH;
	rcButtonPanel.right = rcButtonPanel.left + IGWORKSPACE_COLUMNWIDTH;
	rcButtonPanel.bottom = IGWORKSPACE_LISTBOX_HEIGHT;	
	for (int nIterItem = 0; nIterItem < nNbFrames; nIterItem++)
	{
		IGLibrary::IGSmartPtr <IGLibrary::IGFrame> spFrame;
		if (!m_spFrameMgr->GetFrame (nIterItem, spFrame))
		{
			_ASSERTE (0 && L"Merdum, where the fuck has gone that frame?");
			continue;
		}		
		IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel> spNewButtonPanel (
							new CIGWorkspaceButtonPanel (m_hWnd,
														m_hListBox,
														&rcButtonPanel, 
														(IDispatch *)spFrame->getFramePtr(), 
														m_spMultiFrame,
														m_spDockPanelOwner,
														this));
		m_lspButtonPanels.push_back (spNewButtonPanel);
		// hide pannels out of the listbox bounds
		if ((rcButtonPanel.right > rcListBox.right - rcListBox.left) ||
			(rcButtonPanel.left < 0))
		{
			spNewButtonPanel->Show (SW_HIDE);
			m_nNbVisibleItems--;
		}
		rcButtonPanel.left += IGWORKSPACE_COLUMNWIDTH;
		rcButtonPanel.right += IGWORKSPACE_COLUMNWIDTH;
		registerButtonPanel (spNewButtonPanel);
	}
	checkLeftRightButtonState();
}

void CIGWorkspace::OnPanelClosing ()
{
	for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
		iterButtonPanel != m_lspButtonPanels.end();
		++iterButtonPanel)
	{
		unregisterButtonPanel (*iterButtonPanel);
	}
	m_lspButtonPanels.clear();
}

LRESULT CIGWorkspace::OnMeasureListBoxItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT) lParam;
	if (lpMeasureItem)
	{
		lpMeasureItem->itemWidth = IGWORKSPACE_ITEMWIDTH;
		lpMeasureItem->itemHeight = IGWORKSPACE_ITEMHEIGHT;
		bHandled = TRUE;
		return 0L;
	}
	bHandled = FALSE;
	return 1L;	
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGWorkspace::StaticHookPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGWorkspace* pThis = (CIGWorkspace*)dwRefData;
	if( pThis != NULL )
		return pThis->HookPanelProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGWorkspace::HookPanelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch(msg)
	{
	case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS *p_wpos = (WINDOWPOS *)lParam;
			RECT rcWpos;
			rcWpos.left = p_wpos->x;
			rcWpos.right = p_wpos->x + p_wpos->cx;
			rcWpos.top = p_wpos->y;
			rcWpos.bottom = p_wpos->y + p_wpos->cy;
			Move (&rcWpos);
		}
		break;

	case WM_MEASUREITEM:
		{
			BOOL bHandled;
			return OnMeasureListBoxItem (msg, wParam, lParam, bHandled);
		}

	case WM_DRAWITEM:
		{
			BOOL bHandled;
			return OnDrawListBoxItem (msg, wParam, lParam, bHandled);
		}	
	}
	return ::DefSubclassProc(hWnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGWorkspace::StaticHookListBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGWorkspace* pThis = (CIGWorkspace*)dwRefData;
	if( pThis != NULL )
		return pThis->HookListBoxProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGWorkspace::HookListBoxProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch(msg)
	{
	case WM_PAINT:
		{
			int nNbFrames = m_spFrameMgr->GetNbFrames();
			if (!nNbFrames)
				break;
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			m_nNbVisibleItems = 0;
			for (int nIdxItem = 0; nIdxItem < nNbFrames; nIdxItem++)
			{
				if (drawListBoxItem (hDC, nIdxItem))
					m_nNbVisibleItems++;
			}
			::EndPaint(hWnd, &ps);
			checkLeftRightButtonState ();
		}
		break;
	// hacked messages
	case WM_ERASEBKGND:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		return 0L;	
	}

	LRESULT lRes = ::DefSubclassProc(hWnd, msg, wParam, lParam);

	switch(msg)
	{
		case LB_DELETESTRING:
		{
			if ((m_nCurrentScrollPos > 0) && (m_nCurrentScrollPos + m_nNbVisibleItems <= m_spFrameMgr->GetNbFrames() + 1))
			{
				m_nCurrentScrollPos --;
				updateListBoxScrollPos();
				for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
					iterButtonPanel != m_lspButtonPanels.end();
					++iterButtonPanel)
				{
					(*iterButtonPanel)->MoveRight ();
				}
			}
			int idxButtonPanel = 0;
			// remove button panel
			for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
				iterButtonPanel != m_lspButtonPanels.end();
				++iterButtonPanel, ++idxButtonPanel)
			{
				if (idxButtonPanel == (int)wParam)
				{
					unregisterButtonPanel (*iterButtonPanel);
					m_lspButtonPanels.remove (*iterButtonPanel);
					break;
				}
			}
			// transfer mouse tracking to dockpanel
			::SendMessageW (m_hWnd, WM_MOUSELEAVE, IGDOCKPANEL_TRANSFERMOUSETRACKEVENT, 0);
			// move next button panels to the left
			idxButtonPanel = 0;
			for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
				iterButtonPanel != m_lspButtonPanels.end();
				++iterButtonPanel, ++idxButtonPanel)
			{
				if (idxButtonPanel >= (int)wParam)
					(*iterButtonPanel)->MoveLeft ();
			}
		}
		break;
	}

	return lRes;
}

//------------------------------------------------------------------------------
//static
LRESULT APIENTRY CIGWorkspace::StaticHookButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                                    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CIGWorkspace* pThis = (CIGWorkspace*)dwRefData;
	if( pThis != NULL )
		return pThis->HookButtonProc(hwnd, msg, wParam, lParam);

	return ::DefSubclassProc(hwnd, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
LRESULT CIGWorkspace::HookButtonProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	switch(msg)
	{
	case WM_ERASEBKGND:
		return 1L;

	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
		return 0L;

	case WM_MOUSEMOVE:
		if (m_eButtonState != IGBUTTON_MOUSEOVER)
		{
			m_eButtonState = IGBUTTON_MOUSEOVER;
			m_hOverButton = hWnd;
			TRACKMOUSEEVENT trackMouseEvent;
			trackMouseEvent.cbSize = sizeof (TRACKMOUSEEVENT);
			trackMouseEvent.dwFlags = TME_LEAVE;
			trackMouseEvent.hwndTrack = hWnd;
			trackMouseEvent.dwHoverTime = 0xFFFFFFFE;
			::TrackMouseEvent (&trackMouseEvent);
			redrawButton (hWnd);
		}			
		break;

	case WM_MOUSELEAVE:
		m_eButtonState = IGBUTTON_MOUSEOUT;
		redrawButton (hWnd);
		break;

	case WM_LBUTTONUP:
		::DefSubclassProc(hWnd, WM_LBUTTONDOWN, wParam, lParam);
		if (hWnd == m_hButtonNew)
			OnButtonNewClick ();
		else
			OnButtonClick (hWnd == m_hButtonLeft);
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = ::BeginPaint(hWnd, &ps);
			if (hWnd == m_hButtonNew)
				drawButtonNew (hDC);
			else
				drawButton (hDC, (hWnd == m_hButtonLeft));
			::EndPaint(hWnd, &ps);	
			return 0L;
		}
	}
	// Default behavior using the old window proc
	LRESULT lRes = ::DefSubclassProc(hWnd, msg, wParam, lParam);	

	switch(msg)
	{
		case WM_KILLFOCUS:
		case WM_LBUTTONUP:
			redrawButton (hWnd);
			break;
	}
	return lRes;
}

void CIGWorkspace::redrawButton (HWND hWnd)
{
	RECT rcButton;
	::GetWindowRect (hWnd, &rcButton);
	RECT rcWnd;
	::GetWindowRect (m_hWnd, &rcWnd);
	rcButton.left -= rcWnd.left;
	rcButton.top -= rcWnd.top;
	rcButton.right -= rcWnd.left;
	rcButton.bottom -= rcWnd.top;
	::InvalidateRect (m_hWnd, &rcButton, TRUE);
	::UpdateWindow (m_hWnd);
}

void CIGWorkspace::drawButtonNew (HDC hdc)
{
	Graphics graphics (hdc);
	m_cxIconNewFrame.LayerDrawAll (hdc, IGWORKSPACE_BUTTONNEW_X, 0);
	if ((m_eButtonState == IGBUTTON_MOUSEOVER)
		&& (m_hOverButton == m_hButtonNew))
	{
		Rect rcButton = Rect (IGWORKSPACE_BUTTONNEW_X, 0,
							IGWORKSPACE_BUTTONNEW_WIDTH - IGWORKSPACE_BUTTONNEW_X - 1,
							IGWORKSPACE_BUTTONNEW_HEIGHT - 1);
		// draw blue square in mouseover mode
		SolidBrush solBrushButton (IGWORKSPACE_COLOR_BUTTON);
		graphics.FillRectangle (&solBrushButton, rcButton);
		Pen penButtonBorder (IGWORKSPACE_COLOR_BUTTONBORDER);
		graphics.DrawRectangle (&penButtonBorder, rcButton);
	}
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	if (nNbFrames > 0)
	{
		// draw dashed line
		Pen penSepLine (IGWORKSPACE_COLOR_TRIANGLE);
		penSepLine.SetDashStyle (DashStyleDash);
		graphics.DrawLine (&penSepLine, Point (0,0), Point (0,IGWORKSPACE_BUTTONNEW_HEIGHT));
	}
}

void CIGWorkspace::drawButton (HDC hdc, bool bIsLeft)
{
	// draw black triangle in closed mode	
	Point trgPoints [3];	
	if (bIsLeft)
	{		
		trgPoints [0] = Point (IGWORKSPACE_BUTTON_WIDTH - 2, 2);
		trgPoints [1] = Point (2, IGWORKSPACE_BUTTON_HEIGHT / 2);
		trgPoints [2] = Point (IGWORKSPACE_BUTTON_WIDTH - 2, IGWORKSPACE_BUTTON_HEIGHT - 2);
	}
	else
	{
		trgPoints [0] = Point (2, 2);
		trgPoints [1] = Point (IGWORKSPACE_BUTTON_WIDTH - 2, IGWORKSPACE_BUTTON_HEIGHT / 2);
		trgPoints [2] = Point (2, IGWORKSPACE_BUTTON_HEIGHT - 2);
	}

	Rect rcButton = Rect (0, 0, IGWORKSPACE_BUTTON_WIDTH - 1,
								IGWORKSPACE_BUTTON_HEIGHT - 1);

	Graphics graphics (hdc);
	// draw blue square in waiting mode
	if ((m_eButtonState == IGBUTTON_MOUSEOVER) && (m_hOverButton != m_hButtonNew) &&
		((m_hOverButton == m_hButtonLeft) == bIsLeft))
	{		
		SolidBrush solBrushButton (IGWORKSPACE_COLOR_BUTTON);
		graphics.FillRectangle (&solBrushButton, rcButton);

		// draw blue square with black triangle in closed mode
		Pen penButtonBorder (IGWORKSPACE_COLOR_BUTTONBORDER);
		graphics.DrawRectangle (&penButtonBorder, rcButton);		
	}
	SolidBrush solBrushTrg (IGWORKSPACE_COLOR_TRIANGLE);
	graphics.SetSmoothingMode (SmoothingModeAntiAlias);
	graphics.FillPolygon (&solBrushTrg, &trgPoints[0], 3);
}

void CIGWorkspace::updateCurrentScrollPos()
{
	m_nCurrentScrollPos = ::SendMessage (m_hListBox, LB_GETTOPINDEX, 0, 0);
}

void CIGWorkspace::updateListBoxScrollPos()
{
	for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
				iterButtonPanel != m_lspButtonPanels.end();
				++iterButtonPanel)
	{
		(*iterButtonPanel)->Show (SW_HIDE);
	}
	::SendMessage (m_hListBox, WM_HSCROLL, MAKEWPARAM (SB_THUMBPOSITION, m_nCurrentScrollPos), NULL);
	::InvalidateRect (m_hWnd, NULL, FALSE);	
	::UpdateWindow (m_hWnd);
	::InvalidateRect (m_hListBox, NULL, FALSE);
	::UpdateWindow (m_hListBox);
}

void CIGWorkspace::OnButtonNewClick ()
{
	CreateNewFrame (800, 600, 0, 0);
}

void CIGWorkspace::OnButtonClick (bool bIsLeft)
{
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	if (!nNbFrames)
		return;
	updateCurrentScrollPos();
	if (bIsLeft)
	{
		if (m_nCurrentScrollPos)
		{
			m_nCurrentScrollPos --;
			updateListBoxScrollPos();
			for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
				iterButtonPanel != m_lspButtonPanels.end();
				++iterButtonPanel)
			{
				(*iterButtonPanel)->MoveRight ();
			}
		}
	}
	else
	{
		if (m_nCurrentScrollPos + m_nNbVisibleItems < nNbFrames)
		{
			m_nCurrentScrollPos ++;
			updateListBoxScrollPos();
			for (std::list<IGLibrary::IGSmartPtr <CIGWorkspaceButtonPanel>>::const_iterator iterButtonPanel = m_lspButtonPanels.begin(); 
				iterButtonPanel != m_lspButtonPanels.end();
				++iterButtonPanel)
			{
				(*iterButtonPanel)->MoveLeft ();
			}
		}
	}
}

std::wstring CIGWorkspace::createTempFile (bool bRecover)
{
	std::wstring wsUserTempPath;
	if (bRecover)
	{
		m_spConfigMgr->GetUserTempPath (wsUserTempPath);
		wsUserTempPath += IGWORKSPACE_RECOVEREDFILENAME;
	}
	else
	{
		m_spConfigMgr->GetUserPicturesPath (wsUserTempPath);
		wsUserTempPath += IGWORKSPACE_UNSAVEDFILENAME;
	}
	int nNumTest = 1;
	std::wstring wsUserPictureTmpPath;
	HANDLE hFile = NULL;
	wchar_t pwNum [MAX_PATH];
	::ZeroMemory (pwNum, MAX_PATH * sizeof (wchar_t));
	while (hFile != INVALID_HANDLE_VALUE)
	{
		wsUserPictureTmpPath = wsUserTempPath;		
		wsUserPictureTmpPath += L"(";
		_itow (nNumTest++, pwNum, 10);
		wsUserPictureTmpPath += pwNum;
		wsUserPictureTmpPath += L")";
		wsUserPictureTmpPath += L".ig";
		hFile = ::CreateFile (wsUserPictureTmpPath.c_str(), GENERIC_READ, FILE_SHARE_READ,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			::CloseHandle (hFile);
	}
	return wsUserPictureTmpPath;
}

HRESULT CIGWorkspace::copyMiniPictures(BSTR bstrGuid) 
{
	// copy mini picture files
	wstring wsUserMiniPicturesOutputPath;
	m_spConfigMgr->GetUserMiniPicturesOutputPath (wsUserMiniPicturesOutputPath);
	wstring wsMiniPicturesPath;
	m_spConfigMgr->GetUserMiniPicturesPath (wsMiniPicturesPath);
	CComPtr <IEnumIDList> spFileList;
	CComPtr <IShellFolder> spFolder;
	HRESULT hr = m_spConfigMgr->GetFileList (wsMiniPicturesPath, &spFileList, &spFolder);	
	if (FAILED (hr))
		return hr;
	LPITEMIDLIST pidCurFile = NULL;
	while (spFileList->Next (1, &pidCurFile, NULL) == S_OK)
	{
		STRRET minipicFile;
		hr = spFolder->GetDisplayNameOf (pidCurFile, SHGDN_NORMAL, &minipicFile);
		if (FAILED (hr) || !minipicFile.pOleStr)
			return hr;
		wstring wsMiniPictureFilePath (wsMiniPicturesPath);
		wsMiniPictureFilePath += minipicFile.pOleStr;
		std::wstring wsMiniPicExt = ::PathFindExtensionW (wsMiniPictureFilePath.c_str());
		if (::_wcsicmp (wsMiniPicExt.c_str(), L".JPG") != 0)
			continue;
		std::wstring wsOutputPicturePath = ::PathFindFileNameW (wsMiniPictureFilePath.c_str());
		wsOutputPicturePath = wsUserMiniPicturesOutputPath + wsOutputPicturePath.substr(0, wsOutputPicturePath.find_last_of(L'.')) + L"$";
		wsOutputPicturePath += bstrGuid;
		std::wstring wsOutputPicturePathJPEG (wsOutputPicturePath + L".JPG");
		wsOutputPicturePath += ::PathFindExtensionW (wsMiniPictureFilePath.c_str());
		m_spConfigMgr->CopyFile (wsMiniPictureFilePath.c_str(), wsOutputPicturePathJPEG.c_str());
		CxImage cxMiniPic;
		cxMiniPic.Load (wsMiniPictureFilePath.c_str(), CXIMAGE_FORMAT_JPG);
		m_spConfigMgr->AddMiniPicture (wsOutputPicturePath, cxMiniPic.GetWidth(), cxMiniPic.GetHeight());
		::CoTaskMemFree (minipicFile.pOleStr);		
	}
	return S_OK;
}