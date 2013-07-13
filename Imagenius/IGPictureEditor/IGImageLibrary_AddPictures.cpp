#include "stdafx.h"
#include "IGImageLibrary_AddPictures.h"
#include "IGImageLibrary.h"

using namespace IGLibrary;

IGAddPictures::IGAddPictures(void)	
{
}

IGAddPictures::~IGAddPictures(void)
{
}

void IGAddPictures::ThreadProc (IGThreadMessage *pMessage)
{
	IGThreadMessage_AddPictures *pIGThreadMessage = (IGThreadMessage_AddPictures *)pMessage;
	if (!pIGThreadMessage)
		return;
	std::list <IGSmartPtr <IGLibPicture>> *plspPictures = new std::list <IGSmartPtr <IGLibPicture>>;
	std::wstring wsNewPicturePath = (wchar_t*)pIGThreadMessage->m_pwSelectedFiles;
	wchar_t *pwFileSelected = (wchar_t*)pIGThreadMessage->m_pwSelectedFiles;
	if (pIGThreadMessage->m_nNbPictures == 1)
	{
		if (!pIGThreadMessage->IsLoaded (wsNewPicturePath))
		{
			addPicture (*plspPictures,
						wsNewPicturePath,
						IGIMAGELIBRARY_MINIPICTUREWIDTH,
						IGIMAGELIBRARY_MINIPICTUREHEIGHT,
						pIGThreadMessage->m_spProgress);
		}
	}
	else
	{
		wchar_t *pwNextFile = pwFileSelected + wsNewPicturePath.length() + 1;
		if (*pwNextFile)
		{
			std::wstring wsFolderPath (wsNewPicturePath);
			// load pictures
			std::wstring wsFileTitle;
			while (*pwNextFile)
			{
				wsFileTitle = pwNextFile;
				wsNewPicturePath = wsFolderPath + L"\\" + wsFileTitle;
				if (!pIGThreadMessage->IsLoaded (wsNewPicturePath))
				{
					bool bRes = addPicture (*plspPictures,
											wsNewPicturePath,
											IGIMAGELIBRARY_MINIPICTUREWIDTH,
											IGIMAGELIBRARY_MINIPICTUREHEIGHT,
											pIGThreadMessage->m_spProgress);
					if (!bRes)
						break;
				}
				pwNextFile += wsFileTitle.length() + 1;					
			}
		}
	}
	::PostMessageW (pIGThreadMessage->GetParentWnd(), UM_ADDPICTURES, (WPARAM)plspPictures, 0);	
}

bool IGAddPictures::contains (const std::list <IGSmartPtr <IGLibPicture>> &lspPictures, const std::wstring &wPicturePath)
{
	for (std::list <IGSmartPtr <IGLibPicture>>::const_iterator iterPict = lspPictures.begin(); 
		iterPict != lspPictures.end();
		iterPict++)
	{
		if ((*iterPict)->m_wsPath == wPicturePath)
			return true;
	}
	return false;
}

bool IGAddPictures::addPicture (std::list <IGSmartPtr <IGLibPicture>> &lspPictures, const std::wstring &wPicturePath, int nWidth, int nHeight, IGSmartPtr <IGSplashWindow>	spProgress)
{
	if (contains (lspPictures, wPicturePath))
		return true;
	IGSmartPtr <IGLibPicture> spNewPicture (new IGLibPicture (wPicturePath, nWidth, nHeight, spProgress));
	if (!spNewPicture->m_hBitmap || !spNewPicture->m_bstrMiniPictureBase64)
	{
		return false;
	}
	lspPictures.push_back (spNewPicture);
	//lspPictures.sort();
	return true;
}