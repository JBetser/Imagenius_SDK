#pragma once
#include <IGThread.h>
#include <IGLibPicture.h>
#include <IGSplashProxy.h>
#include <IGSmartPtr.h>
#include <IGUserMessages.h>
#include <vector>
#include <list>
#include <string>

class IGThreadMessage_AddPictures : public IGLibrary::IGThreadMessage
{
public:
	IGThreadMessage_AddPictures (const wchar_t *pcwSelectedFiles, size_t szBuffer, int nNbPictures, const std::vector <std::wstring> &vLoadedFiles, HWND hParent, IGLibrary::IGSmartPtr <IGLibrary::IGSplashWindow> spProgress)
	{
		m_pwSelectedFiles = new wchar_t [szBuffer];
		::memcpy_s (m_pwSelectedFiles, szBuffer * sizeof (wchar_t), pcwSelectedFiles, szBuffer * sizeof (wchar_t));
		m_pvLoadedFiles = new std::vector <std::wstring> (vLoadedFiles);
		m_hParent = hParent;
		m_spProgress = spProgress;
		m_nNbPictures = nNbPictures;
	}

	virtual ~IGThreadMessage_AddPictures()
	{
		delete [] m_pwSelectedFiles;
		delete m_pvLoadedFiles;
	}

	virtual UINT GetMsgId () const
	{
		return UM_ADDPICTURES;
	}

	virtual bool IsValid ()
	{
		return InternalIsValid <IGThreadMessage_AddPictures> ();
	}

	bool IsLoaded (std::wstring wsFile)
	{
		for (std::vector<std::wstring>::const_iterator iterFile = m_pvLoadedFiles->begin(); 
		iterFile != m_pvLoadedFiles->end();
		iterFile++)
		{
			if ((*iterFile) == wsFile)
				return true;
		}
		return false;
	}

	wchar_t												*m_pwSelectedFiles;
	std::vector <std::wstring>							*m_pvLoadedFiles;
	IGLibrary::IGSmartPtr <IGLibrary::IGSplashWindow>	m_spProgress;
	int													m_nNbPictures;
};

class IGAddPictures : public IGLibrary::IGThread
{
public:
	IGAddPictures(void);
	virtual ~IGAddPictures(void);	

protected:
	virtual void ThreadProc (IGLibrary::IGThreadMessage *pMessage);

private:
	static bool contains (const std::list <IGLibrary::IGSmartPtr <IGLibrary::IGLibPicture>> &lspPictures, const std::wstring &wPicturePath);
	static bool addPicture (std::list <IGLibrary::IGSmartPtr <IGLibrary::IGLibPicture>> &lspPictures, const std::wstring &wPicturePath, int nWidth, int nHeight, IGLibrary::IGSmartPtr <IGLibrary::IGSplashWindow> spProgress);
};
