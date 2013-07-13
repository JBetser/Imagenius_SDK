#include "StdAfx.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGThread.h"
#include "IGIPMergeLayers.h"

using namespace IGLibrary;

int IGIPMergeLayers::GetFirstParam (IGImageProcMessage& message, CxImage& image) const
{
	IGIPMergeLayersMessage *pIGThreadMessage = (IGLibrary::IGIPMergeLayersMessage *)&message;
	if (!pIGThreadMessage)
		return 0;
	if (pIGThreadMessage->m_lpSelLayers.size() < 2)
		return 0;
	return pIGThreadMessage->m_lpSelLayers.size();
}

int IGIPMergeLayers::GetSecondParam (IGImageProcMessage& message, CxImage& image) const
{
	IGIPMergeLayersMessage *pIGThreadMessage = (IGLibrary::IGIPMergeLayersMessage *)&message;
	if (!pIGThreadMessage)
		return 0;
	if (pIGThreadMessage->m_lpSelLayers.size() < 2)
		return 0;
	int *pnLayerPositions = new int [pIGThreadMessage->m_lpSelLayers.size()];
	list <CxImage*>::const_iterator itItem = pIGThreadMessage->m_lpSelLayers.begin();
	for (unsigned int idxLayer = 0;
		 itItem != pIGThreadMessage->m_lpSelLayers.end(); ++itItem, idxLayer++)
	{
		pnLayerPositions [idxLayer] = image.GetLayerPos ((IGLayer*)(*itItem));
	}
	return (int)pnLayerPositions;
}

bool IGIPMergeLayers::OnImageProcessing (CxImage& image, IGImageProcMessage& message)
{
	IGIPMergeLayersMessage *pIGThreadMessage = dynamic_cast <IGLibrary::IGIPMergeLayersMessage *> (&message);
	_ASSERTE (pIGThreadMessage && L"Wrong IGThread message, image processing aborted");
	if (!pIGThreadMessage)
		return false;
	if (pIGThreadMessage->m_lpSelLayers.size() < 2 || pIGThreadMessage->m_lpSelLayers.size() > IGFRAME_MAX_NBLAYERS)
		return false;
	pIGThreadMessage->m_pDstLayer = const_cast <CxImage*> (pIGThreadMessage->m_lpSelLayers.front());
	pIGThreadMessage->m_lpSelLayers.pop_front();

	CxImage *pDeletedLayer = NULL;
	list <CxImage*>::const_iterator itItem = pIGThreadMessage->m_lpSelLayers.begin();
	for (; itItem != pIGThreadMessage->m_lpSelLayers.end(); ++itItem)
	{
		image.LayerDelete (image.GetLayerPos (*itItem), &pDeletedLayer);
		pIGThreadMessage->m_lpSrcLayers.push_back (pDeletedLayer);
	}

	CxImage *pMergedLayer = pIGThreadMessage->m_pDstLayer;
	for (list <CxImage*>::const_iterator itItem = pIGThreadMessage->m_lpSrcLayers.begin();
		 itItem != pIGThreadMessage->m_lpSrcLayers.end(); ++itItem)
	{
		pMergedLayer->Mix (*(*itItem), CxImage::OpAlphaBlend, true);
	}
	dynamic_cast <IGLayer*> (pMergedLayer)->SetWorking();
	return true;
}