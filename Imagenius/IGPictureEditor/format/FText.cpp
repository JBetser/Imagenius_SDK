#include "stdafx.h"
#include "FText.h"

int writeToDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::istream& is, int bufsize, bool isUnicodeText)
{
	HANDLE hData = ::GlobalAlloc(GHND, bufsize);
	char* dest = (char *) ::GlobalLock(hData);
	is.read(dest, bufsize);
	::GlobalUnlock(hData); 
	
	// Call to SetData
	FORMATETC format;
	STGMEDIUM medium;
	ZeroMemory (&format, sizeof (format));
	ZeroMemory (&medium, sizeof (medium));
	
	format.cfFormat = static_cast<CLIPFORMAT>(fid);
	format.dwAspect = DVASPECT_CONTENT;
	format.ptd = 0;
	format.lindex = -1;
	medium.tymed = format.tymed = TYMED_HGLOBAL;
	medium.hGlobal = hData;
	medium.pUnkForRelease = 0;
	return SUCCEEDED(lpDataObject->SetData(&format, &medium, TRUE)) ? 1 : 0;
}

int readFromDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::ostream& os, int* size, bool isUnicodeText)
{
	// Call to GetData
	FORMATETC format;
	format.cfFormat = static_cast<CLIPFORMAT>(fid);
	format.dwAspect = DVASPECT_CONTENT;
	format.ptd = 0;
	format.tymed = TYMED_HGLOBAL;
	format.lindex = -1;

	STGMEDIUM medium;
	ZeroMemory (&medium, sizeof (medium));

	char* ptr = 0;
	if (SUCCEEDED(lpDataObject->GetData(&format, &medium)))
    {
		if (isUnicodeText)
		{
			wchar_t* wptr = (wchar_t*) ::GlobalLock(medium.hGlobal);
			*size = (::wcslen(wptr) + 1) * sizeof(wchar_t);
			ptr = (char*) wptr;
		}
		else
		{
			ptr = (char*) ::GlobalLock(medium.hGlobal); 
			*size = (strlen(ptr) + 1); 
		}
// ::GlobalSize(medium.hGlobal) contains two terminal zeroes, assert failed
//		assert(GlobalSize(medium.hGlobal) == *size);
		os.write(ptr, *size);
		::GlobalUnlock(medium.hGlobal);
		::ReleaseStgMedium(&medium);
		return 1;
    }
	else
    {
		return 0;
    }
}

FText::FText(int fid)
: Format(fid)
{
}

FText::FText(int fid, int sid)
: Format(fid, sid)
{
}

FText::FText(int fid, const std::wstring& sFormatName)
: Format(fid, sFormatName)
{
}

FText::FText(const std::wstring& sFormatName)
: Format(sFormatName)
{
}

int FText::write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize)
{
	return writeToDataObject(pIDataObject, sid_, is, bufsize, isUnicodeText());
}

int FText::read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd)
{
	return readFromDataObject(pIDataObject, sid_, os, size, isUnicodeText());
}

// virtual
bool FText::isUnicodeText() const
{
	return false;
}
