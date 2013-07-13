#include "stdafx.h"
#include "Format.h"
#include "FIGLayer.h"
#include "FIGFrame.h"

static int writeToDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::istream& is, HWND hwnd, int bufsize);
static int readFromDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::ostream& os, int* size, HWND hwnd);

int writeToDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::istream& is, HWND hwnd, int bufsize)
{
	HANDLE hDib = 0;
	// The client placed the bitmap HANDLE in the stream
	// Note: the SController adds a trailing null char to the data (hence the +1)
	if ((bufsize) && (bufsize == sizeof (HANDLE) + 1)) 
    {
		HANDLE hBitmapTemp;		
		is.read ((char *)&hBitmapTemp, sizeof (HANDLE));	
		hDib = hBitmapTemp;
		//hDib = (HBITMAP)::OleDuplicateData (hBitmapTemp, Format::F_IGFRAME, 0);
    }
	
	if (hDib == 0)
	{
		return 0;
	}
	
	FORMATETC format;
	STGMEDIUM medium;
	ZeroMemory (&format, sizeof (format));
	ZeroMemory (&medium, sizeof (medium));
	
	format.cfFormat = static_cast<CLIPFORMAT> (fid);
	format.dwAspect = DVASPECT_CONTENT;
	format.ptd = 0;
	format.lindex = -1;
	medium.tymed = format.tymed = TYMED_HGLOBAL;
	medium.hGlobal = hDib;
	medium.pUnkForRelease = 0;
	
	lpDataObject->SetData (&format, &medium, TRUE);
	return 1;
}

int readFromDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::ostream& os, int* size, HWND hwnd)
{
	FORMATETC format;
	format.cfFormat = static_cast<CLIPFORMAT> (fid);
	format.dwAspect = DVASPECT_CONTENT;
	format.ptd = 0;
	format.tymed = TYMED_HGLOBAL;
	format.lindex = -1;

	STGMEDIUM medium;
	ZeroMemory (&medium, sizeof (medium));
	
	// Get the bitmap handle from the dropped IDataObject,
	// and then write it in the stream
	if (SUCCEEDED(lpDataObject->GetData(&format, &medium)))
    {
		if(medium.hGlobal != NULL)
		{
			os.write ((char *)&medium.hGlobal, sizeof(HANDLE));
			os << std::ends;
			*size = sizeof(HANDLE) + 1;
		}
    }
	return (medium.hGlobal != NULL);
}

FIGFrame::FIGFrame()
: FIGLayer (Format::F_IGFRAME, L"Imagenius Frame")
{
}
 
FIGFrame::~FIGFrame()
{
}


int FIGFrame::write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize)
{  
	return writeToDataObject(pIDataObject, sid_, is, hwnd, bufsize);
}

int FIGFrame::read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd)
{
	return readFromDataObject(pIDataObject, sid_, os, size, hwnd);
}