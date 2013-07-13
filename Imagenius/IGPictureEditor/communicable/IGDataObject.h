#pragma once

#include <list>
#include <ObjIdl.h>

class IGDataObjectEntry;

//=============================================================================
// IGDataObject
//=============================================================================

class IGDataObject : public IDataObject
{
public:
	IGDataObject();

	// IUnknow standard methods
	STDMETHOD(QueryInterface)(REFIID riid, void **ppv);
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG,Release)(void);

	// IDataObject's methods
	STDMETHOD(GetData)(LPFORMATETC pformatetcIn, LPSTGMEDIUM pmedium);
	STDMETHOD(GetDataHere)(LPFORMATETC pformatetc, LPSTGMEDIUM pmedium);
	STDMETHOD(QueryGetData)(LPFORMATETC pformatetc);
	STDMETHOD(GetCanonicalFormatEtc)(LPFORMATETC pformatetc, LPFORMATETC pformatetcOut);
	STDMETHOD(SetData)(LPFORMATETC pformatetc, STGMEDIUM* pmedium, BOOL fRelease);
	STDMETHOD(EnumFormatEtc)(DWORD dwDirection, LPENUMFORMATETC* ppenumFormatEtc);
	STDMETHOD(DAdvise)(FORMATETC* pFormatetc, DWORD advf, LPADVISESINK pAdvSink, DWORD* pdwConnection);
	STDMETHOD(DUnadvise)(DWORD dwConnection);
	STDMETHOD(EnumDAdvise)(LPENUMSTATDATA* ppenumAdvise);

	// helpers
	static HRESULT SetXMLData(IDataObject& refDataObject, unsigned long cfFormat, const IXMLDOMDocument2& refDOMDoc);
	static HRESULT GetXMLData(const IDataObject& refDataObject, unsigned long cfFormat, IXMLDOMDocument2** ppIDOMDoc);

private:
	~IGDataObject();  

	HRESULT findEntry(const FORMATETC& formatetc, IGDataObjectEntry** ppDataObjectEntry) const;

private:
	ULONG				m_refs;
	LPDATAADVISEHOLDER	m_pAdviseHolder;

	std::list<IGDataObjectEntry*> m_listEntries; // liste of handle for data
};
