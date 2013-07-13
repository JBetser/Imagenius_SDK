#include "stdafx.h"
#include "Format.h"
#include "FBitmap.h"

static int writeToDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::istream& is, HWND hwnd, int bufsize);
static int readFromDataObject(LPDATAOBJECT lpDataObject, UINT fid, std::ostream& os, int* size, HWND hwnd);

int writeToDataObject (LPDATAOBJECT lpDataObject, UINT fid, std::istream& is, HWND hwnd, int bufsize)
{
	HBITMAP hBitmap = 0;	
	// The client placed the bitmap HANDLE in the stream
	// Note: the SController adds a trailing null char to the data (hence the +1)
	if ((bufsize) && (bufsize == sizeof (HBITMAP) + 1)) 
    {
		HBITMAP hBitmapTemp;		
		is.read ((char *)&hBitmapTemp, sizeof (HBITMAP));		
		hBitmap = hBitmapTemp;
		//hBitmap = (HBITMAP)::OleDuplicateData (hBitmapTemp, Format::F_IGFRAME_BMP, 0);
    }
	
	if (hBitmap == 0)
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

	medium.tymed = format.tymed = TYMED_GDI;
	medium.hBitmap = hBitmap;
	medium.pUnkForRelease = 0;
	
	return SUCCEEDED (lpDataObject->SetData (&format, &medium, TRUE)) ? 1 : 0;
}

int readFromDataObject (LPDATAOBJECT lpDataObject, UINT fid, std::ostream& os, int* size, HWND hwnd)
{
	FORMATETC format;	
	format.cfFormat = static_cast<CLIPFORMAT> (fid);
	format.dwAspect = DVASPECT_CONTENT;
	format.ptd = 0;
	format.tymed = TYMED_GDI;
	format.lindex = -1;

	STGMEDIUM medium;
	ZeroMemory (&medium, sizeof (medium));
	
	HBITMAP hBitmap = 0;
	// Get the bitmap handle from the dropped IDataObject,
	// and then write it in the stream
	if (SUCCEEDED(lpDataObject->GetData(&format, &medium)))
    {
		hBitmap = medium.hBitmap;
		if (hBitmap != 0)
		{
			os.write ((char *)&hBitmap, sizeof (HBITMAP));
			os << std::ends;
			*size = sizeof(HBITMAP) + 1;
		}
    }
	return (hBitmap != 0);
}

FBitmap::FBitmap()
: Format (Format::F_IGFRAME_BMP)
{
}
 
FBitmap::~FBitmap()
{
}


int FBitmap::write(LPDATAOBJECT pIDataObject, std::istream& is, HWND hwnd, int bufsize)
{  
	return writeToDataObject(pIDataObject, sid_, is, hwnd, bufsize);
}

int FBitmap::read(LPDATAOBJECT pIDataObject, std::ostream& os, int* size, HWND hwnd)
{
	return readFromDataObject(pIDataObject, sid_, os, size, hwnd);
}
