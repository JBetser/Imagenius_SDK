/*
 * File:	ximaDZ.h
 * Purpose:	multithreaded generation of microsoft DeepZoom file format
 */
#if !defined(__ximaDZ_h)
#define __ximaDZ_h

#include "ximage.h"
#include "IGThreadSafeData.h"

#if CXIMAGE_SUPPORT_DZ

class IGDZMonitor;
class CxImageDZThreadSampler;
class CxImageDZCropAndSaveThread;

class IGDZSampleMessage : public IGLibrary::IGThreadMessage
{
public:
	IGDZSampleMessage(IGDZMonitor *pMonitor, 
						const RECT& rcDst,
						const RECT& rcSrc,
						const RECT& rcUpdate,
						const RECT& rcLastUpdate,
						int nLevel);
	virtual ~IGDZSampleMessage (){}

	inline int GetLevel() const {return m_nLevel;}
	inline IGDZMonitor* GetMonitor() {return m_pMonitor;}
	inline int GetWidth() const {return m_rcDst.right - m_rcDst.left;}
	inline int GetHeight() const {return m_rcDst.bottom - m_rcDst.top;}
	inline RECT GetUpdateRect() const {return m_rcUpdate;}
	inline RECT GetLastUpdateRect() const {return m_rcLastUpdate;}

protected:
	IGDZSampleMessage(IGDZMonitor *pMonitor, 
		const RECT& rcDst,
		const RECT& rcSrc,
		int nLevel);

	virtual bool IsValid ()
	{
		return InternalIsValid <IGDZSampleMessage> ();
	}
	virtual UINT GetMsgId () const
	{
		return UM_DEEPZOOM_PROCESSING;
	}

	RECT m_rcDst;	

private:
	RECT m_rcUpdate;
	RECT m_rcLastUpdate;
	RECT m_rcSrc;
	int m_nLevel;
	IGDZMonitor *m_pMonitor;
};

class IGDZCropAndSaveMessage : public IGDZSampleMessage
{
public:
	IGDZCropAndSaveMessage(IGDZMonitor *pMonitor, 
							const RECT& rcDst,
							const RECT& rcSrc,
							int nLevel,
							const wchar_t *pcwPath) : IGDZSampleMessage (pMonitor, rcDst, rcSrc, nLevel){
		::wcscpy_s (m_twPath, pcwPath);
	}
	virtual ~IGDZCropAndSaveMessage (){}

	inline RECT GetCropRect() const {return m_rcDst;}
	inline wchar_t* GetPath() {return m_twPath;}

private:
	wchar_t m_twPath[256];
};

class IGDZTile
{
public:
	IGDZTile (int nWidth, int nHeight, int nLevel = -1, int nBitCount = 24);
	virtual ~IGDZTile(void);

	inline int GetLevel() const {return m_nLevel;}
	inline void GetBits (const BYTE*& pBits, int& nWidth, int& nHeight) const
	{
		pBits = (const BYTE*)m_pDib;
		nWidth = m_nWidth;
		nHeight = m_nHeight;
	}
	inline const BYTE* GetBits (int nRow) const
	{
		return (const BYTE*)m_pDib + ((((m_nBitCount * m_nWidth) + 31) / 32) * 4) * nRow;
	}
	void Copy (const BYTE *pSrcBits);
	void Resample (const BYTE *pSrcBits, int nSrcWidth, int nSrcHeight);
	void Save (const std::wstring& wsPath);
	void CopyTo (CxImage& copyDest);
	void BW();

	int			m_nWidth; 
	int			m_nHeight; 

private:
	inline BYTE* GetBitsRW (int nRow)
	{
		return (BYTE*)m_pDib + ((((m_nBitCount * m_nWidth) + 31) / 32) * 4) * nRow;
	}

	int			m_nLevel;
	void		*m_pDib; //the bitmap
	HDC			m_hTileDC;
	HBITMAP		m_hTileBmp;
	int			m_nBitCount;
	BITMAPINFO  m_bmInfo;
};

class IGDZMonitor
{
public:
	IGDZMonitor (std::vector <CxImageDZThreadSampler*> vSamplingThreads, void *pBits, int nWidth, int nHeight, int nParentThreadId, HWND hProgressBar, const std::wstring& wsDZRootFolder);
	virtual ~IGDZMonitor();

	// Posting task API
	bool PostTask (IGDZSampleMessage *pMessage);

	// End of task Notification API
	void NotifySamplingTaskEnded (IGDZTile* pImageDZSampled);
	void NotifyCroppingTaskEnded();
	///////////////////////////////
	
	int GetParentThreadId();
	HWND GetProgressBar();
	std::wstring GetRootFolder();
	void GetSampledImageBits(int nLevel, const BYTE*& pBits, int& nWidth, int& nHeight);
	bool IsEmpty()		 {	std::vector <IGDZTile*> vpImages = m_vpFullImages.Lock();
	bool bEmpty = (vpImages.size() == 0);
	m_vpFullImages.UnLock();
	return bEmpty; }
	bool IsWorkFinished();

private:
	void	*m_pOriginalBits;
	IGLibrary::IGThreadSafeData <int> m_nParentThreadId;
	IGLibrary::IGThreadSafeData <HWND> m_hProgressBar;
	IGLibrary::IGThreadSafeData <int> m_nNbSteps;
	IGLibrary::IGThreadSafeData <std::vector <CxImageDZThreadSampler*>> m_vpSamplingThreads;
	IGLibrary::IGThreadSafeData <std::vector <IGDZTile*>> m_vpFullImages;
	IGLibrary::IGThreadSafeData <std::vector <IGDZTile*>> m_vpTiles;
	IGLibrary::IGThreadSafeData <std::wstring> m_wsDZRootFolder;
};

class CxImageDZ : public CxImage
{
public:
	CxImageDZ()	: CxImage(CXIMAGE_FORMAT_DZ) {}
	virtual ~CxImageDZ() {}

#if CXIMAGE_SUPPORT_ENCODE
	bool Encode(CxFile * hFile);
	bool Encode(FILE *hFile) { CxIOFile file(hFile); return Encode(&file); }
#endif // CXIMAGE_SUPPORT_ENCODE
};

#endif

#endif
