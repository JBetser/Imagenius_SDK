#include "stdafx.h"
#include "msxml2.h"
#include "IGDataObject.h"

#if defined(DEBUG)
#include <iostream>
#endif

//=============================================================================
// IGDataObjectEntry
//=============================================================================

class IGDataObjectEntry
{
public :
	IGDataObjectEntry(const FORMATETC& formatEtc, const STGMEDIUM& stgMedium, BOOL fRelease);
	~IGDataObjectEntry();

	HRESULT queryGetData(const FORMATETC& formatetc) const;
	HRESULT getData(const FORMATETC& formatetc, STGMEDIUM& stgmedium) const;
	HRESULT setData(const FORMATETC& formatetc, const STGMEDIUM& stgmedium, BOOL fRelease);
	const FORMATETC& FormatEtc() const;

private :
	void copyStgMediumTo(const STGMEDIUM& stgMedium, STGMEDIUM& stgMediumDst) const;
	void deleteStgMedium();

private :
	FORMATETC m_formatEtc;
	STGMEDIUM* m_pStdMedium;
};


IGDataObjectEntry::IGDataObjectEntry(const FORMATETC& formatEtc, const STGMEDIUM& stgMedium, BOOL fRelease)
: m_pStdMedium(0)
{
	setData(formatEtc, stgMedium, fRelease);
}

IGDataObjectEntry::~IGDataObjectEntry()
{
	deleteStgMedium();
}

const FORMATETC& IGDataObjectEntry::FormatEtc() const
{
	return m_formatEtc;
}

HRESULT IGDataObjectEntry::queryGetData(const FORMATETC& formatetc) const
{
	//support others if needed DVASPECT_THUMBNAIL  //DVASPECT_ICON   //DVASPECT_DOCPRINT
	if (m_formatEtc.cfFormat != formatetc.cfFormat)
	{
		return DV_E_CLIPFORMAT;
	}
	if ((m_formatEtc.dwAspect & DVASPECT_CONTENT) != DVASPECT_CONTENT)
	{
		return DV_E_DVASPECT;
	}
	if (m_formatEtc.tymed != formatetc.tymed)
	{
		return DV_E_TYMED;
	}
	return S_OK;
}

HRESULT IGDataObjectEntry::getData(const FORMATETC& formatetc, STGMEDIUM& stgmedium) const
{ 
	HRESULT hr = queryGetData(formatetc);
	if (FAILED(hr))
	{
		return hr;
	}
	copyStgMediumTo(*m_pStdMedium, stgmedium);
	return S_OK;
}

HRESULT IGDataObjectEntry::setData(const FORMATETC& formatEtc, const STGMEDIUM& stgMedium, BOOL fRelease)
{ 
	m_formatEtc = formatEtc;
	deleteStgMedium();
	m_pStdMedium = new STGMEDIUM;
	if (fRelease)
	{
		*m_pStdMedium = stgMedium;
	}
	else
	{
		copyStgMediumTo(stgMedium, *m_pStdMedium);
	}
	return S_OK;
}

void IGDataObjectEntry::deleteStgMedium()
{
	if (m_pStdMedium)
	{
		::ReleaseStgMedium(m_pStdMedium);
		delete m_pStdMedium;
		m_pStdMedium = 0;
	}
}

void IGDataObjectEntry::copyStgMediumTo(const STGMEDIUM& stgMedium, STGMEDIUM& stgMediumDst) const
{
	stgMediumDst.hGlobal = NULL;
	switch (stgMedium.tymed)
	{
		case TYMED_HGLOBAL:
			stgMediumDst.hGlobal = (HGLOBAL)::OleDuplicateData(stgMedium.hGlobal, m_formatEtc.cfFormat, NULL);
			break;
		case TYMED_GDI:
			stgMediumDst.hBitmap = (HBITMAP)::CopyImage(stgMedium.hBitmap, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
			break;
		case TYMED_MFPICT:
			stgMediumDst.hMetaFilePict = (HMETAFILEPICT)::OleDuplicateData(stgMedium.hMetaFilePict,m_formatEtc.cfFormat, NULL);
			break;
		case TYMED_ENHMF:
			stgMediumDst.hEnhMetaFile = (HENHMETAFILE)::OleDuplicateData(stgMedium.hEnhMetaFile,m_formatEtc.cfFormat, NULL);
			break;
		case TYMED_FILE:
			stgMediumDst.lpszFileName = (LPOLESTR)::OleDuplicateData(stgMedium.lpszFileName,m_formatEtc.cfFormat, NULL);
			break;
		case TYMED_ISTREAM:
			stgMediumDst.pstm = stgMedium.pstm;
			stgMedium.pstm->AddRef();
			break;
		case TYMED_ISTORAGE:
			stgMediumDst.pstg = stgMedium.pstg;
			stgMedium.pstg->AddRef();
			break;
		case TYMED_NULL:
		default:
			break;
	}
	stgMediumDst.tymed = stgMedium.tymed;
	stgMediumDst.pUnkForRelease = stgMedium.pUnkForRelease;
	if (stgMedium.pUnkForRelease != NULL)
	{
		stgMedium.pUnkForRelease->AddRef();
	}
}

//=============================================================================
// Class CEnumFORMATETC: implementation (IGDataObject.cpp)
//=============================================================================

class CEnumFORMATETC : public IEnumFORMATETC
{
public:
	CEnumFORMATETC(const std::list<IGDataObjectEntry*>&);

	// IUnknown members
	STDMETHODIMP QueryInterface (REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef ();
	STDMETHODIMP_(ULONG) Release ();

	// IEnumFORMATETC members
	STDMETHODIMP  Next (ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched);
	STDMETHODIMP  Skip (ULONG celt);
	STDMETHODIMP  Reset ();
	STDMETHODIMP  Clone (IEnumFORMATETC **ppenum);

private:
	CEnumFORMATETC (const CEnumFORMATETC&);
	~CEnumFORMATETC ();

private:
	DWORD       m_dwRefCount;        // COM Reference count
	DWORD       m_dwCurrent;         // Current enumerator index
	DWORD       m_dwNumFormats;      // Size of FORMATETC array
	FORMATETC*  m_pFormats;     // FORMATETC array
};


CEnumFORMATETC::CEnumFORMATETC (const std::list<IGDataObjectEntry*>& formatList)
: m_dwRefCount (1)
, m_dwCurrent (0)
, m_pFormats (0)
{
	m_dwNumFormats = formatList.size();
	if (m_dwNumFormats)
	{
		m_pFormats = new FORMATETC [m_dwNumFormats];
		int index = 0;
		for(std::list<IGDataObjectEntry*>::const_iterator iter = formatList.begin(); iter != formatList.end(); ++iter)
		{
			// Bitwise copy is sufficient (assuming ptd is NULL)
			m_pFormats[index] = (*iter)->FormatEtc();
			++index;
		}
	}
}

CEnumFORMATETC::CEnumFORMATETC ( const CEnumFORMATETC& enumFormatEtc)
: m_dwRefCount (1)
, m_dwCurrent (enumFormatEtc.m_dwCurrent)
, m_dwNumFormats (enumFormatEtc.m_dwNumFormats)
, m_pFormats (0)
{
	if (m_dwNumFormats)
	{
		m_pFormats = new FORMATETC [m_dwNumFormats];

		for (DWORD i = 0 ; i < m_dwNumFormats ; ++i)
		{
			// Bitwise copy is sufficient (assuming ptd is NULL)
			m_pFormats[i] = enumFormatEtc.m_pFormats[i];
		}
	}
}


//----------------------------------------------------------------------
CEnumFORMATETC::~CEnumFORMATETC ( )
{
	delete [] m_pFormats;
}

//----------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETC::QueryInterface ( REFIID riid, void **ppvObj )
{
	if (ppvObj == 0)
	{
		return E_INVALIDARG;
	}

	*ppvObj = 0;
	if (::IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = static_cast<IUnknown*>(this);
	}
	else if (::IsEqualIID(riid, IID_IEnumFORMATETC))
	{
		*ppvObj = static_cast<IEnumFORMATETC*>(this);
	}
	if (*ppvObj == 0)
	{
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) CEnumFORMATETC::AddRef ( )
{
	return ++m_dwRefCount;
}

//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) CEnumFORMATETC::Release ( )
{
	if (--m_dwRefCount == 0)
	{
#if defined(DEBUG)
		std::wcout << L"deleting CEnumFormatETC"<<std::endl;
#endif
		delete this;
		return 0;
	}
#if defined(DEBUG)
	std::wcout << L"NOT deleting CEnumFormatETC"<<std::endl;
#endif

	return m_dwRefCount;
}

//----------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETC::Next ( ULONG celt, FORMATETC *rgelt, ULONG *pceltFetched )
{
	if (rgelt == 0)
		return E_INVALIDARG;

	DWORD i;

	for (i = 0 ; i < celt ; i++)
	{
		if (m_dwCurrent == m_dwNumFormats)
			break;

		// Bitwise copy is sufficient (assuming ptd is NULL)
		rgelt[i] = m_pFormats[m_dwCurrent];

		m_dwCurrent ++;
	}

	if (pceltFetched)
		*pceltFetched = i;

	if (i != celt)
		return S_FALSE;

	return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETC::Skip ( ULONG celt )
{
	while (celt)
	{
		if (m_dwCurrent == m_dwNumFormats)
			return S_FALSE;

		m_dwCurrent ++;
		celt --;
	}
	return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETC::Reset ( )
{
	m_dwCurrent = 0;
	return S_OK;
}

//----------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETC::Clone ( IEnumFORMATETC **ppenum )
{
	if (ppenum == 0)
		return E_INVALIDARG;

	CEnumFORMATETC* pClone = new CEnumFORMATETC (*this);
	*ppenum = pClone;
	return S_OK;
}

//=============================================================================
// IGDataObject
//=============================================================================

//----------------------------------------------------------------------
IGDataObject::IGDataObject()
: m_refs(1)
, m_pAdviseHolder(0)
{
}

//----------------------------------------------------------------------
IGDataObject::~IGDataObject()
{
	while(!m_listEntries.empty())
	{
		IGDataObjectEntry* pDataObjectEntry = m_listEntries.front();
		delete pDataObjectEntry;
		m_listEntries.pop_front();
	}

	if (m_pAdviseHolder)
	{
		m_pAdviseHolder->Release();
	}
}

HRESULT IGDataObject::findEntry(const FORMATETC& formatetc, IGDataObjectEntry** ppDataObjectEntry) const
{	
	HRESULT hr = DV_E_CLIPFORMAT;
	std::list<IGDataObjectEntry*>::const_iterator iter = m_listEntries.begin();
	while(iter != m_listEntries.end())
	{
		HRESULT hrLocal = (*iter)->queryGetData(formatetc);
		if (SUCCEEDED(hrLocal))
		{
			*ppDataObjectEntry = (*iter);
			hr = hrLocal;
			break;
		}
		else if ((hr == DV_E_DVASPECT) || (hr == DV_E_TYMED))
		{
			*ppDataObjectEntry = (*iter);
			hr = hrLocal;
		}
		++iter;
	}
	return hr;
}

//----------------------------------------------------------------------
//  NAME        :QueryInterface
//
//  DESCRIPTION :Standard OLE method. See OLE documentation.
//
//  RETURNED VALUE:See OLE documentation.
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == 0)
	{
		return E_INVALIDARG;
	}
	*ppvObj = 0;
	if (::IsEqualIID(riid, IID_IUnknown))
	{
		*ppvObj = static_cast<IUnknown*>(this);
	}
	else if (::IsEqualIID(riid, IID_IDataObject))
	{
		*ppvObj = static_cast<IDataObject*>(this);
	}
	if (*ppvObj == 0)
	{
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

//----------------------------------------------------------------------
//  NAME        :AddRef
//
//  DESCRIPTION :Standard OLE method. See OLE documentation.
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP_(ULONG)IGDataObject::AddRef()
{
	++m_refs;
	return m_refs;
};

//----------------------------------------------------------------------
//  NAME        :Release
//
//  DESCRIPTION :Standard OLE method. See OLE documentation.
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP_(ULONG) IGDataObject::Release()
{
	if(--m_refs==0)
	{
		// We have to delete the object
#if defined(DEBUG)
		std::cout<<"AddRef ["<<m_refs<<"]"<<std::endl;
#endif
		delete this;
		return 0;
	}
	return m_refs;
}

//----------------------------------------------------------------------
//  NAME        :GetData
//
//  DESCRIPTION :See OLE documentation on IDataObject::GetData
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::GetData(LPFORMATETC pFormatEtcIn, LPSTGMEDIUM pStgMedium)
{
	// Parameter checking
	if (pFormatEtcIn == 0) 
	{
		return DV_E_FORMATETC;
	}
	if (pStgMedium == 0)
	{
		return E_INVALIDARG;
	}

	IGDataObjectEntry* pDataObjectEntry = 0;
	HRESULT hr = findEntry(*pFormatEtcIn, &pDataObjectEntry);
	if (FAILED(hr))
	{
		return hr;
	}
	if (pStgMedium)
	{
		hr = pDataObjectEntry->getData(*pFormatEtcIn, *pStgMedium);
	}
	return hr;
}

//----------------------------------------------------------------------
//  NAME        :GetDataHere
//
//  DESCRIPTION :See OLE documentation on IDataObject::GetDataHere
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::GetDataHere(LPFORMATETC /*pFormatEtc*/, LPSTGMEDIUM /*pStgMedium*/)
{
	// Not implemeted here
#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDataObject::GetDataHere() called."<<std::endl;
#endif
	return E_NOTIMPL;
}

//----------------------------------------------------------------------
//  NAME        :QueryGetData
//
//  DESCRIPTION :See OLE documentation on IDataObject::QueryGetData
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::QueryGetData(LPFORMATETC pFormatEtc)
{
	// Parameter checking
	if (pFormatEtc == 0) 
	{
		return DV_E_FORMATETC;
	}

	IGDataObjectEntry* pDataObjectEntry = 0;
	return findEntry(*pFormatEtc, &pDataObjectEntry);
}

//----------------------------------------------------------------------
//  NAME        :GetCanonicalFormatEtc
//
//  DESCRIPTION :See OLE documentation on IDataObject::GetCanonicalFormatEtc
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::GetCanonicalFormatEtc(LPFORMATETC /*pFormatEtc*/, LPFORMATETC pFormatEtcOut)
{
	// Since we do not support any specific device rendering, we just return that is it the same formatEtc
	if (pFormatEtcOut == NULL)
	{
		return E_INVALIDARG;
	}
	pFormatEtcOut = 0;
	return DATA_S_SAMEFORMATETC;
}

//----------------------------------------------------------------------
//  NAME        :SetData
//
//  DESCRIPTION :See OLE documentation on IDataObject::SetData
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::SetData(LPFORMATETC pFormatEtc, STGMEDIUM* pStgMedium, BOOL fRelease)
{
	if ((pFormatEtc == NULL) || (pStgMedium == NULL))
	{
		return E_POINTER;
	}
	if (pStgMedium->hGlobal == NULL)
	{
		return E_POINTER;
	}
	IGDataObjectEntry* pDataObjectEntry = 0;
	HRESULT hr = findEntry(*pFormatEtc, &pDataObjectEntry);
	if (pDataObjectEntry) // SUCCEEDED(hr) || (hr == DV_E_DVASPECT) || (hr == DV_E_TYMED))
	{
		hr = pDataObjectEntry->setData(*pFormatEtc, *pStgMedium, fRelease);
	}
	else
	{
		pDataObjectEntry = new IGDataObjectEntry(*pFormatEtc, *pStgMedium, fRelease);
		m_listEntries.push_back(pDataObjectEntry);
		hr = S_OK;
	}
	return hr;
}

//----------------------------------------------------------------------
//  NAME        :EnumFormatEtc
//
//  DESCRIPTION :See OLE documentation on IDataObject:EnumFormatEtc
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* ppenumFormatEtc)
{
	if(dwDirection != DATADIR_GET)
		return E_FAIL;

#if defined(DEBUG)&&defined(MYMSG)
	std::cout<<"IGDataObject::EnumFormatEtc() called."<<std::endl;
#endif

	*ppenumFormatEtc = new CEnumFORMATETC(m_listEntries);
	return NOERROR;
}

//----------------------------------------------------------------------
//  NAME        :DAdvise
//
//  DESCRIPTION :See OLE documentation on IDataObject::DAdvise
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::DAdvise(FORMATETC* pFormatEtc, DWORD advf, LPADVISESINK pAdvSink, DWORD * pdwConnection)
{
	if(FAILED(QueryGetData(pFormatEtc))) 
	{
		return DV_E_FORMATETC;
	}
	if (m_pAdviseHolder == 0)
	{
		::CreateDataAdviseHolder(&m_pAdviseHolder);
	};
	// Just delegate here
	return m_pAdviseHolder->Advise((LPDATAOBJECT) this, pFormatEtc, advf, pAdvSink, pdwConnection);
}

//----------------------------------------------------------------------
//  NAME        :DUnadvise
//
//  DESCRIPTION :See OLE documentation on IDataObject::DUnadvise
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::DUnadvise(DWORD dwConnection)
{
	if(m_pAdviseHolder==0)
		return E_FAIL;

	return m_pAdviseHolder->Unadvise(dwConnection);
}


//----------------------------------------------------------------------
//  NAME        :EnumDAdvise
//
//  DESCRIPTION :See OLE documentatio on IDataObject::EnumDAdvise
//
//  RETURNED VALUE:See OLE documentation
//----------------------------------------------------------------------
STDMETHODIMP IGDataObject::EnumDAdvise(LPENUMSTATDATA* ppenumAdvise)
{
	if(m_pAdviseHolder==0)
		return E_FAIL;

	return m_pAdviseHolder->EnumAdvise(ppenumAdvise);
}

// static
HRESULT IGDataObject::SetXMLData(IDataObject& refDataObject, unsigned long cfFormat, const IXMLDOMDocument2& refDOMDoc)
{
	//if ((tymed != TYMED_ISTREAM) && (tymed != TYMED_HGLOBAL))
	//{
	//	return E_INVALIDARG;
	//}
	IXMLDOMDocument2* pIDOMDoc = const_cast<IXMLDOMDocument2*>(&refDOMDoc);
	CComPtr<IStream> spStream;
	HRESULT hr = pIDOMDoc->QueryInterface(IID_IStream, (void**) &spStream);
	if (FAILED(hr))
	{
		return hr;
	}

	FORMATETC formatEtc = {0};
	STGMEDIUM stgMedium = {0};
/*		
	if (tymed == TYMED_HGLOBAL)
	{
		// Get the size of the stream
		STATSTG statsg = {0};
		hr = spStream->Stat(&statsg, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		SIZE_T lBufferSize = (SIZE_T) statsg.cbSize.LowPart;

		HANDLE hData = ::GlobalAlloc(GHND, lBufferSize);
		if (hData == 0)
		{
			return E_OUTOFMEMORY;
		}
		char* pData = (char *) ::GlobalLock(hData);
		if (pData == 0)
		{
			::GlobalUnlock(hData); 
			::GlobalFree(hData);
			return E_OUTOFMEMORY;
		}
		// Read the stream in one shot
		ULONG nbByteRead = 0;
		hr = spStream->Read(pData, lBufferSize, &nbByteRead);
		if (FAILED(hr))
		{
			::GlobalUnlock(hData); 
			::GlobalFree(hData);
			return hr;
		}
		::GlobalUnlock(hData); 
	
		formatEtc.cfFormat = static_cast<CLIPFORMAT>(cfFormat);
		formatEtc.dwAspect = DVASPECT_CONTENT;
		formatEtc.ptd = 0;
		formatEtc.lindex = -1;
		stgMedium.tymed = formatEtc.tymed = TYMED_HGLOBAL;
		stgMedium.hGlobal = hData;
		stgMedium.pUnkForRelease = 0;
	}
	else if (tymed == TYMED_ISTREAM)
*/
	{
		formatEtc.cfFormat = static_cast<CLIPFORMAT>(cfFormat);
		formatEtc.dwAspect = DVASPECT_CONTENT;
		formatEtc.ptd = 0;
		formatEtc.lindex = -1;
		stgMedium.tymed = formatEtc.tymed = TYMED_ISTREAM;
		stgMedium.pstm = spStream.Detach();
		stgMedium.pUnkForRelease = 0;
		if (FAILED(hr))
		{
			return hr;
		}
	}
	return refDataObject.SetData(&formatEtc, &stgMedium, TRUE);
}

// static
HRESULT IGDataObject::GetXMLData(const IDataObject& refDataObject, unsigned long cfFormat, IXMLDOMDocument2** ppIDOMDoc)
{
	if (ppIDOMDoc == 0)
	{
		return E_POINTER;
	}

	STGMEDIUM stgMedium = {0};
	FORMATETC formatEtc = {0};
	formatEtc.cfFormat = static_cast<CLIPFORMAT>(cfFormat);
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.ptd = 0;
	formatEtc.tymed = TYMED_ISTREAM;
	formatEtc.lindex = -1;

	IDataObject* pIDataObject = const_cast<IDataObject*>(&refDataObject);
	HRESULT hr = pIDataObject->GetData (&formatEtc, &stgMedium);
	if (FAILED(hr))
	{
		formatEtc.tymed = TYMED_HGLOBAL;
		hr = pIDataObject->GetData (&formatEtc, &stgMedium);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	if ((stgMedium.tymed != TYMED_ISTREAM) && (stgMedium.tymed != TYMED_HGLOBAL))
	{
		return E_INVALIDARG;
	}

	CComPtr<IStream> spStream;
	if (stgMedium.tymed == TYMED_ISTREAM)
	{
		if (stgMedium.pstm)
		{
			spStream = stgMedium.pstm;
		}
	}
	else if (stgMedium.tymed == TYMED_HGLOBAL)
	{
		// if persistent data have been found, export them and leave
		if (stgMedium.hGlobal)
		{
			hr = ::CreateStreamOnHGlobal (stgMedium.hGlobal, FALSE, &spStream);
		}
	}
	if (!spStream)
	{
		::ReleaseStgMedium(&stgMedium);
		return E_INVALIDARG;
	}
	CComPtr<IXMLDOMDocument2> spXMLDOMDocument2;
	hr = spXMLDOMDocument2.CoCreateInstance(__uuidof(DOMDocument30));
	if (hr != S_OK)
	{
		::ReleaseStgMedium(&stgMedium);
		return hr;
	}
	LARGE_INTEGER pos = {0};
	//the key is to reset the seek pointer
	hr = spStream->Seek((LARGE_INTEGER)pos, STREAM_SEEK_SET, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	VARIANT_BOOL varSuccessful;
	hr = spXMLDOMDocument2->load(CComVariant(spStream), &varSuccessful);
	if ((hr != S_OK) ||(varSuccessful == VARIANT_FALSE))
	{
		hr = E_FAIL;
	}
	if (hr == S_OK)
	{
		*ppIDOMDoc = spXMLDOMDocument2.Detach();
	}
	::ReleaseStgMedium(&stgMedium);
	return hr;
}

