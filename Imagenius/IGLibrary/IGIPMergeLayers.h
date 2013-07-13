#pragma once

#include "IGImageProcessing.h"
#include "IGImageProcMessage.h"
#include <list>

namespace IGLibrary
{

class IGIPMergeLayersMessage : public IGImageProcMessage
{
	friend class IGIPMergeLayers;
public:
	IGIPMergeLayersMessage (std::list <CxImage*> lpSelLayers)
							: IGImageProcMessage (IGREQUEST_FRAME_MERGELAYER)
							, m_lpSelLayers (lpSelLayers)
							, m_pDstLayer (NULL)
	{
	}

	virtual ~IGIPMergeLayersMessage()
	{
		for (list <CxImage*>::const_iterator itItem = m_lpSrcLayers.begin();
			 itItem != m_lpSrcLayers.end(); ++itItem)
		{
			delete (*itItem);
		}
	}

	static UINT ComputeNbProgressSteps (const std::list <CxImage*>& lpSelLayers)
	{
		if (lpSelLayers.size() < 2)
			return 0;
		unsigned long ulDstHeight = lpSelLayers.front()->GetHeight();
		UINT nNbSteps = 0;
		std::list <CxImage*>::const_iterator itItem = lpSelLayers.begin();
		++itItem;
		for (; itItem != lpSelLayers.end(); ++itItem)
		{
			long lSrcXOffset, lSrcYOffset;
			(*itItem)->GetOffset (&lSrcXOffset, &lSrcYOffset);
			unsigned long ulHeight = (unsigned long)max ((long)ulDstHeight, lSrcYOffset + (long)(*itItem)->GetHeight()) - min (0, lSrcYOffset);
			if (ulHeight == ulDstHeight)
			{
				nNbSteps += 2 * (*itItem)->GetHeight();
			}
			else
			{
				nNbSteps += ulDstHeight + (*itItem)->GetHeight();
				ulDstHeight = ulHeight;
			}
		}
		return nNbSteps;
	}

protected:
	virtual bool IsValid ()
	{
		return InternalIsValid <IGIPMergeLayersMessage> ();
	}
	virtual UINT ComputeNbProgressSteps (const IGFrame &frame) const
	{
		return ComputeNbProgressSteps (m_lpSelLayers);
	}

private:
	CxImage					*m_pDstLayer;
	std::list <CxImage*>	m_lpSrcLayers;
	std::list <CxImage*>	m_lpSelLayers;
};

class IGIPMergeLayers : public IGImageProcessing
{
public:
	IGIPMergeLayers (IGFrame *pFrame, const wchar_t *pcwXml, HWND hProgress = NULL)
				: IGImageProcessing (pFrame, pcwXml, hProgress)
	{
	}
	IGIPMergeLayers (IGFrame *pFrame, IGImageProcMessage *pMessage, HWND hProgress = NULL)
				: IGImageProcessing (pFrame, pMessage, hProgress)
	{
	}
	virtual ~IGIPMergeLayers()
	{
	}

protected:
	// Image processing thread callback
	virtual bool OnImageProcessing (CxImage& image, IGImageProcMessage& message);
	virtual int GetFirstParam (IGImageProcMessage& message, CxImage& image) const;
	virtual int GetSecondParam (IGImageProcMessage& message, CxImage& image) const;
};

}