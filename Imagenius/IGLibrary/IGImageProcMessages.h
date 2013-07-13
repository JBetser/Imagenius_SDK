#pragma once
#include <WinGDI.h>
#include <list>
#include "IGFrame.h"
#include "IGIPMergeLayers.h"

class CxImageThread;

namespace IGLibrary
{
	class IGImageProcessing;
	class IGImageProcessingCallback;	

class IGImageSaveMessage : public IGImageProcMessage
{
	friend class IGImageProcessing;
public:
	IGImageSaveMessage (std::wstring wsImagePath, DWORD dwImageType, bool bCloseAfterSave)	: IGImageProcMessage (IGREQUEST_FRAME_SAVE)
																							, m_wsImagePath (wsImagePath)
																							, m_dwImageType (dwImageType)
																							, m_bCloseAfterSave (bCloseAfterSave)
	{
	}

	static UINT ComputeNbProgressSteps (const std::list <CxImage*>& lpAllLayers)
	{
		unsigned long ulDstHeight = 0;
		UINT nNbSteps = 0;
		for (std::list <CxImage*>::const_iterator itItem = lpAllLayers.begin();
			 itItem != lpAllLayers.end(); ++itItem)
		{
			long lSrcXOffset, lSrcYOffset;
			(*itItem)->GetOffset (&lSrcXOffset, &lSrcYOffset);
			ulDstHeight = (unsigned long)max ((long)ulDstHeight, lSrcYOffset + (long)(*itItem)->GetHeight()) - min (0, lSrcYOffset);
		}
		return IGIPMergeLayersMessage::ComputeNbProgressSteps (lpAllLayers) + ulDstHeight;
	}

	bool NeedClosing() {return m_bCloseAfterSave;}
	LPCWSTR	GetImagePath() {return m_wsImagePath.c_str();}

protected:
	virtual UINT ComputeNbProgressSteps (const IGFrame &frame) const
	{
		std::list <CxImage*> lpAllLayers;
		lpAllLayers.insert (lpAllLayers.begin(), frame.lpLayers.begin(), frame.lpLayers.end());
		return ComputeNbProgressSteps (lpAllLayers);
	}

	virtual bool NeedOuputGeneration () const {return false;}

	std::wstring	m_wsImagePath;
	DWORD			m_dwImageType;
	bool			m_bCloseAfterSave;
};

class IGImageLoadMessage : public IGImageProcMessage
{
	friend class IGImageProcessing;
public:
	IGImageLoadMessage (std::wstring wsImagePath, bool bAutoRotate = false, int nIdxLayer = -1)	: IGImageProcMessage (IGREQUEST_WORKSPACE_LOADIMAGE)
																		, m_wsImagePath (wsImagePath)
																		, m_nIdxLayer (nIdxLayer)
																		, m_bAutoRotate (bAutoRotate) {}
	static UINT ComputeNbLoadProgressSteps()	{return 100;}
	LPCWSTR GetImagePath () const {return m_wsImagePath.c_str();}
	
protected:
	virtual UINT ComputeNbProgressSteps() const
	{
		return IGImageLoadMessage::ComputeNbLoadProgressSteps();
	}
	std::wstring	m_wsImagePath;
	int				m_nIdxLayer;
	bool			m_bAutoRotate;
};

class IGImageUpdateMessage : public IGImageProcMessage
{
	friend class IGImageProcessing;
public:
	IGImageUpdateMessage()	: IGImageProcMessage (IGREQUEST_FRAME_UPDATE){}
	static UINT ComputeNbLoadProgressSteps()	{return 100;}
	
protected:
	virtual UINT ComputeNbProgressSteps() const
	{
		return IGImageUpdateMessage::ComputeNbLoadProgressSteps();
	}
	virtual bool NeedOuputGeneration () const {return true;}
};

}