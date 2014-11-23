#pragma once
#include "IGSmartPtr.h"
#include "IGView.h"
#include "IGLayout.h"
#include "IGFrameHistory.h"
#include "IGImageProcessing.h"
#include "IGSplashWindow.h"
#include "IGFrameProperties.h"
#include "IGSmartPtr.h"
#include <list>

#define IGFRAME_MAX_NBLAYERS					10

class IGRequestAddLayer;
class IGRequestRemoveLayer;
class IGRequestMoveLayer;

namespace IGLibrary
{
class IGLayer;
class IGBrush;

class IGFrame	:	protected CxImage,
					public IGView <IGFrame>,
					public IGLayout,
					public IGFrameHistory,
					public IGFrameProperties
{
	// The class that manages image processing thread
	friend class IGImageProcessing;
	friend class IGInternalProcessing;
	friend class IGImageSaveMessage;
	friend class IGLibPicture;
	friend class IGFrameHistory;
	friend class IGFrameAction;

public:	
	IGFrame (HRSRC hRes, DWORD dwType, HINSTANCE hInstance, HWND hOwner = NULL, bool bStart = false);
	IGFrame (HWND hOwner, LPCWSTR pwPath, bool bAutoRotate, bool bStart = false);
	IGFrame (HWND hOwner, int nWidth, int nHeight,
				  int nColorMode, int nBackgroundMode, bool bStart = false);
	IGFrame (HWND hOwner, HANDLE hMem, bool bStart = false);
	IGFrame (DWORD imagetype);
	IGFrame (IGSmartPtr <IGSplashWindow> spProgress, LPCWSTR pwPath, bool bAutoRotate, bool bStart = false);
	IGFrame (const IGFrame& frameCopy, bool bStart = false);
	virtual ~IGFrame(void);	

	// Method to be called after a frame creation or a frame loading to initialize everything
	void Initialize();

	// Method to know if the current thread can access the frame
	bool RequestAccess ();
	////////////////////////////

	// Multiframe methods
	bool SetSelected (bool bSelected = true);
	bool GetSelected () const;
	////////////////////////////

	// get Image size, in pixels
	int GetWidth() const;
	int GetHeight() const;

	// move the splash to the right place
	void UpdatePosition();

	// Rendering methods
	// Render all layers to hdc with cache management
	bool Render (HDC hdc);
	// Render all layers to hdc in another cache channel
	bool RenderSecondary (HDC hdc, int nLeft, int nTop,
						int nWidth, int nHeight, bool bChessBackground);
	void Redraw (bool bForceRedraw = false, bool bAllFrame = true);
	void RedrawSecondary();
	long LayerDrawAllInCache(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bShowChessBackground = false, bool bShowCurrentBackground = false, COLORREF cBackgroundColor = CLR_INVALID, COLORREF cTranspMask = CLR_INVALID);
	long LayerDrawAllInClearedCache(HDC hdc, long x=0, long y=0, long cx = -1, long cy = -1, RECT* pClipRect = 0, bool bSmooth = false, bool bShowChessBackground = false, bool bShowCurrentBackground = false, COLORREF cBackgroundColor = CLR_INVALID, COLORREF cTranspMask = CLR_INVALID);
	long LayerDrawAllClearCache();
	long LayerDrawAllFromCache (HDC hdc, int nDestX=0, int nDestY=0, unsigned char ucAlpha = 255, COLORREF cTranspMask = CLR_INVALID);
	////////////////////////////

	// Layer management methods
	HRESULT AddCalc (LPCWSTR pwPath, long nPos = -1, bool bAutoRotate = false);
	HRESULT AddCalc (IGLayer *pLayer, long nPos = -1, bool bHistory = true, int x = 0, int y = 0);
	bool CreateNewCalc (long nPos = -1);
	HRESULT RemoveCalc (long nPos = -1, IGLayer **ppDeletedLayer = NULL, bool bHistory = true);
	HRESULT MoveCalc (long nPosFrom, long nPosDest);
	HRESULT RemoveAllCalcs();
	int GetNbLayers () const;
	bool SelectCalc (int nPos);
	void UnselectCalcs();
	std::list <const IGLayer*> GetSelectedCalcs () const;
	IGLayer* GetLayer (int nPos) const;	
	IGLayer* GetLayerFromId (int nId) const;	
	long GetLayerPos (const CxImage *pLayer) const;
	////////////////////////////

	// call this function when access to protected resource can be granted
	void GrantAccess() {IGThread::GrantAccess();};

	void OnImageProcessingEnded (IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcId);

	// zoom attribute
	float GetCurrentZoom() const;
	void SetCurrentZoom (float fCurrentZoom);

	HANDLE CopyToHandle (bool bCopyAlpha = false);
	bool PasteFromHandle (HANDLE hMem, long x = 0, long y = 0);
	HBITMAP MakeBitmap (HDC hdc = NULL, int nImageWidth = -1, int nImageHeight = -1, bool bDrawChessBackground = false, bool bDrawWhiteBackground = true);

	inline CxImage* GetWorkingLayer() const {return CxImage::GetWorkingLayer();}
	inline void SetWorkingLayer (int nPos) {CxImage::SetWorkingLayer(nPos); Redraw (true);}

	HRESULT Save (const TCHAR* filename, DWORD imagetype, bool bCloseAfterSave, bool bAsync = true);
	HRESULT Load (const TCHAR* filename, bool bAutoRotate);

	HRESULT GetPath (std::wstring& wsPath) const;
	HRESULT GetName (std::wstring& wsName) const;
	HRESULT SetName (const std::wstring& wsName);

	static DWORD GetImageType (LPCWSTR lpPicturePath);
	static LPCWSTR GetUserOutputPath();
	static void SetOutputPath (LPCWSTR pcwPath);

	void setFramePtr (OLE_HANDLE pFrame);
	OLE_HANDLE getFramePtr();

	// selection management
	bool ManageSelection (const IGLibrary::SELECTIONPARAMS& selParams, const std::list<POINT>& lPts);
	bool IsSelection();
	CxImage *GetSelectionLayer();

	bool DrawLines (std::list<POINT>& lPts, const IGBrush& brush);

	void SetCurrentCursor();
	LPCWSTR GetId() const;

	bool StartFiltering (int nFilterId);
	bool StartIndexing (int nFilterId, IGMarker *pMarkerObject, IGMarker *pMarkerBackground);
	bool StartDrawing (std::list<POINT>& lPts, IGBrush *pBrush);
	bool StartFaceEffect (int nFaceEffectId, COLORREF col1, COLORREF col2, double dParam1, double dParam2, double dParam3, const TCHAR* imageFile1, const TCHAR* imageFile2);

	void SetLastOutputPath(const TCHAR *filename) {CxImage::SetLastOutputPath (filename);}
	std::wstring GetLastOutputPath() const { return CxImage::GetLastOutputPath();}

	void SetLastReqGuid(const TCHAR *reqGuid) {CxImage::SetLastReqGuid (reqGuid);}
	std::wstring GetLastReqGuid() const { return CxImage::GetLastReqGuid();}

	// properties
	virtual bool Update();
	virtual void SetSize(int nWidth, int nHeight);
	template <typename T>
	bool SetRequestProperty (const wchar_t *pcwPropId, const T& propVal){		
		return m_curRequestParams.SetProperty(pcwPropId, propVal);
	}
	template <typename T>
	bool GetRequestProperty (const wchar_t *pcwPropId, T& propVal) const{
		return m_curRequestParams.GetProperty(pcwPropId, propVal);
	}
	void ClearRequestProperties(){
		m_curRequestParams.Clear();
	}

	bool PickColor (POINT ptCoords, RGBQUAD& rgbColor);

protected:
	virtual bool Load(const TCHAR* filename, DWORD imagetype);
	virtual bool Save(const TCHAR* filename, DWORD imagetype);
	virtual bool LayerCreate (long nPos = -1, bool bAlpha = true, bool bSelection = true, int x = 0, int y = 0);	// Adds a new empty calcs
	virtual void SetPath(const TCHAR* filename);
	virtual const TCHAR* GetPath();
	virtual bool Start();
	virtual bool Exit();

	virtual bool OnFrameSizeChanged (int nNewWidth, int nNewHeight, IGFRAMEPROPERTY_STRETCHING_ENUM stretchMode);

	void Ghost(const CxImage *src);
	void Ghost (const IGLibrary::IGFrame *src);
	void CopyInfo(const CxImage &src);
	bool Transfer(CxImage &from, bool bTransferFrames /*=true*/);
	bool Resample (long newx, long newy);
	HRESULT Resample (long newx, long newy, IGFRAMEPROPERTY_STRETCHING_ENUM stretchMode);
	HRESULT InternalSave (const TCHAR* filename, DWORD imagetype, bool bCloseAfterSave, bool bAsync = true);

private:	
	void setGuid();
	HRESULT resampleNoStretch (long newx, long newy);
	HRESULT resampleFast (long newx, long newy);
	HRESULT resampleBicubic (long newx, long newy);

	OLE_HANDLE							m_pFrame;	// IIGFrame *
	std::list <IGLayer*>				m_lpSelectedLayers;
	IGSmartPtr <IGSplashWindow>			m_spProgress;
	HWND								m_hOwner;
	std::wstring						m_wsFilePath;
	bool								m_bRedraw;
	bool								m_bSelected;
	bool								m_bNewDragAndDrop;
	IGHasProperties						m_curRequestParams;

	//output members
	static std::wstring					g_wsOutputPath;	
	IGLibrary::IGThreadSafeData <std::wstring>	m_safewsPath;
};

inline bool IGFrame::GetSelected () const
{
	// This method is safe for multithreading => no request access on beginning
	return m_bSelected;
}

inline int IGFrame::GetWidth() const
{
	return CxImage::GetWidth();
}

inline int IGFrame::GetHeight() const
{
	return CxImage::GetHeight();
}

inline bool IGFrame::RequestAccess ()
{
	return CxImageThread::RequestAccess();
}

}
