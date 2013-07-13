#include "StdAfx.h"
#include "IGRequest.h"
#include "IGRequest_simple.h"
#include "IGWorkspace.h"
#include "IGIPMergeLayers.h"
#include "IGImageProcMessages.h"
#include "IGFrameProperties.h"
#include "IGBrush.h"
#include "IGIPFilter.h"
#include <IGLog.h>
#include <shlobj.h>

using namespace std;
using namespace IGLibrary;

bool IGRequestPing::InternalProcess()
{
	return true;
}

bool IGRequestConnect::InternalProcess()
{
	wstring wsUserLogin;
	if (!GetParameter (IGREQUEST_USERLOGIN, wsUserLogin))
		return false;
	m_eStep = IGREQUESTCONNECT_STEP_LOADING;
	HRESULT hr = m_pWorkspace->ConnectUser (CComBSTR (wsUserLogin.c_str()), CComBSTR (GetGuid().c_str()), NULL); // feature currently disabled (LONG*)&m_nNbFrames);
	if (hr == S_OK)
	{
		// no frame loading launched
		m_bIsAsync = false;	
		m_eStep = IGREQUESTCONNECT_STEP_CONNECTING;
	}
	else if (hr == S_FALSE)
	{
		// a frame loading has launched
		if (!SetFrameId (m_spFrameMgr->GetNbFrames() - 1))
			return false;
	}
	return SUCCEEDED (hr);
}

LRESULT IGRequestConnect::PostProcessing (IGImageProcMessage *pMessage)
{
	// check if we are in a "recover unsaved files" process
	IGImageLoadMessage *pLoadMessage = NULL;
	IGImageSaveMessage *pSaveMessage = NULL;
	if (pMessage->GetImageProcessingId() == IGREQUEST_FRAME_SAVE)
		pSaveMessage = dynamic_cast <IGImageSaveMessage *> (pMessage);
	else if (pMessage->GetImageProcessingId() == IGREQUEST_WORKSPACE_LOADIMAGE)
		pLoadMessage = dynamic_cast <IGImageLoadMessage *> (pMessage);
	else
		return IGREQUEST_SUCCEEDED;		
	IGFrame *pFrame = GetFrame();
	if (!pFrame)
		return IGREQUEST_FAILED;
	switch (m_eStep)
	{
	case IGREQUESTCONNECT_STEP_LOADING:
		{
			// if cannot read message, then abort the frame restoration
			if (!pLoadMessage)
				return IGREQUEST_FAILED;
			m_wsImagePath = pLoadMessage->GetImagePath();
			// if loading failed, remove the loaded frame and continue
			if (!pLoadMessage->GetResult())
				return processNextImage (pLoadMessage, pFrame, false);
			pFrame->Initialize();
			m_eStep = IGREQUESTCONNECT_STEP_CREATINGHISTORY;
			IGImageProcessing::SetCurrentRequest (this);
			// if loading failed during creation of history, remove the loaded frame
			if (!pFrame->CreateHistory (getInstance(), MAKEINTRESOURCE(IDR_FRAMEHISTORY)))
				return processNextImage (pLoadMessage, pFrame, false);
			return IGREQUEST_PROCESSING;
		}
	case IGREQUESTCONNECT_STEP_CREATINGHISTORY:
		{
			// if loading failed during creation of history, remove the loaded frame
			if (!pMessage->GetResult() || !pSaveMessage)
				return processNextImage (pSaveMessage, pFrame, false);
			// if failed to delete current frame, continue
			::DeleteFileW (m_wsImagePath.c_str());
			// start the next connecting step
			return processNextImage (pSaveMessage, pFrame, true);
		}	
	}
	return IGREQUEST_FAILED;
}

LRESULT IGRequestConnect::processNextImage (IGImageProcMessage *pMessage, 
											IGFrame *pPreviousFrame,
											bool bLastSucceeded)
{
	if (!bLastSucceeded)
	{
		POSTPONE (pMessage);
		m_pWorkspace->RemoveFrame (m_spFrameMgr->GetFramePos (pPreviousFrame->GetId()));
		::DeleteFileW (m_wsImagePath.c_str());
	}
	// start the next connecting step
	m_eStep = IGREQUESTCONNECT_STEP_LOADING;
	IGImageProcessing::SetCurrentRequest(this);
	HRESULT hr = m_pWorkspace->ConnectUser (CComBSTR (L"Processing"));
	// check if still some files to be restored
	if (hr == S_FALSE) 
	{
		// a frame loading has launched
		if (!SetFrameId (m_spFrameMgr->GetNbFrames() - 1))
			return IGREQUEST_FAILED;
		return IGREQUEST_PROCESSING;
	}
	if (FAILED (hr))
		return IGREQUEST_FAILED;
	return IGREQUEST_PROCESSED;
}

void IGRequestConnect::OnProgress (int nProgress, LPCWSTR pwMessage)
{
	IGRequestFrame::OnProgress (nProgress / m_nNbFrames, pwMessage);
}

bool IGRequestDisconnect::InternalProcess()
{
	m_bEnableProgess = false;
	if (!m_spConfigManager->IsUserLogged())	
	{
		wstring wsLog (L"Processing ");
		wsLog += ToString() + L". User is already disconnected.\n";
		IGLog log;
		log.Add (wsLog.c_str());
	}
	HRESULT hr = m_pWorkspace->DisconnectUser(VARIANT_FALSE);
	if (hr == S_OK)
		m_bIsAsync = false;	// no frame saving launched
	return SUCCEEDED (hr);
}

LRESULT IGRequestDisconnect::PostProcessing (IGImageProcMessage *pMessage)
{
	// check if we are in a "saving all and terminate" process
	if (pMessage->GetImageProcessingId() != IGREQUEST_FRAME_SAVE)
		return IGREQUEST_SUCCEEDED;
	IGImageSaveMessage *pSaveMessage = dynamic_cast <IGImageSaveMessage *> (pMessage);
	if (!pSaveMessage)
		return IGREQUEST_SUCCEEDED;
	if (!pSaveMessage->NeedClosing())
		return IGREQUEST_FAILED;
	POSTPONE (pSaveMessage);
	IIGMainWindow *pMainWindow = GetMainWindow();
	wstring wsDisconnectLog = L"Disconnecting server - Frame ";
	wsDisconnectLog += pSaveMessage->GetImagePath();
	wsDisconnectLog += L" saved.";
	IGLog log;
	log.Add (wsDisconnectLog.c_str());
	if (FAILED (m_pWorkspace->RemoveFrame(0)))
	{
		wsDisconnectLog = L"An error occured while disconnecting server. A frame cannot be removed";
		log.Add (wsDisconnectLog.c_str());
		return IGREQUEST_FAILED;
	}
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	IGImageProcessing::SetCurrentRequest (this);
	if (FAILED (m_pWorkspace->DisconnectUser (VARIANT_FALSE)))
		return IGREQUEST_FAILED;			
	return (nNbFrames > 0) ? IGREQUEST_PROCESSING : IGREQUEST_PROCESSED;
}	

bool IGRequestNew::InternalProcess()
{
	int nWidth;
	if (!GetParameter (IGFrameProperties::IGFRAMEPROPERTY_WIDTH, nWidth))
		return false;
	int nHeight;
	if (!GetParameter (IGFrameProperties::IGFRAMEPROPERTY_HEIGHT, nHeight))
		return false;
	int nColorMode;
	if (!GetParameter (IGREQUEST_COLORMODE, nColorMode))
		return false;
	int nBackgroundMode;
	if (!GetParameter (IGREQUEST_BACKGROUNDMODE, nBackgroundMode))
		return false;
	HRESULT hr = m_pWorkspace->CreateNewFrame (nWidth, nHeight, nColorMode, nBackgroundMode);
	if (hr == S_FALSE)	// too many frames
		return false;
	if (FAILED (hr))
	{
		m_pWorkspace->RemoveFrame (m_spFrameMgr->GetNbFrames() - 1);
		return false;
	}
	if (!SetFrameId (m_spFrameMgr->GetNbFrames() - 1))
		return false;
	return true;
}

LRESULT IGRequestNew::PostProcessing (IGImageProcMessage *pMessage)
{
	if (!pMessage->GetResult())
	{
		// if new frame failed, remove the created frame
		POSTPONE (pMessage);
		m_pWorkspace->RemoveFrame (GetFrameId());
		return IGREQUEST_FAILED;
	}
	if (m_eStep == IGREQUESTCONNECT_STEP_CREATINGHISTORY) 
	{
		m_eStep = IGREQUESTCONNECT_STEP_GENERATINGOUTPUT;
		IGImageProcessing::SetCurrentRequest (this);
		IGImageProcessing *pUpdateImageProcessing = new IGImageProcessing (GetFrame(), new IGImageUpdateMessage());
		pUpdateImageProcessing->Start();
		return IGREQUEST_PROCESSING; 
	}
	return IGREQUEST_PROCESSED;
}

bool IGRequestLoad::InternalProcess()
{
	wstring wsPath;
	if (!GetParameter (IGREQUEST_PATH, wsPath))
		return false;	
	int nAutoRotate;
	if (!GetParameter (IGREQUEST_AUTOROTATE, nAutoRotate))
		nAutoRotate = 0;
	int nNbFrames = m_spFrameMgr->GetNbFrames();
	HRESULT hr = m_pWorkspace->AddFrame (CComBSTR (wsPath.c_str()), VARIANT_FALSE, nAutoRotate == 1 ? VARIANT_TRUE : VARIANT_FALSE);
	if (hr == S_FALSE)	// too many frames
		return false;
	if (FAILED (hr))
	{
		if (m_spFrameMgr->GetNbFrames() > nNbFrames)
			m_pWorkspace->RemoveFrame (m_spFrameMgr->GetNbFrames() - 1);
		return false;
	}
	if (!SetFrameId (m_spFrameMgr->GetNbFrames() - 1))
		return false;	
	IGSmartPtr <IGFrame> spFrame;
	hr = m_spFrameMgr->GetFrame (m_spFrameMgr->GetNbFrames() - 1, spFrame);
	if (FAILED (hr))
		return false;
	if (!m_wsLoadAs.empty())
		spFrame->SetName (m_wsLoadAs);
	else
		spFrame->SetName (wsPath);
	return SUCCEEDED (hr);
}

LRESULT IGRequestLoad::PostProcessing (IGImageProcMessage *pMessage)
{
	if (!pMessage->GetResult())
	{
		// if loading failed during creation of history, remove the loaded frame
		POSTPONE (pMessage);
		m_pWorkspace->RemoveFrame (GetFrameId());
		return IGREQUEST_FAILED;
	}
	// check if it is the final step
	if (m_eStep == IGREQUESTLOAD_STEP_CREATINGHISTORY)
		return IGREQUEST_PROCESSED;
	m_eStep = IGREQUESTLOAD_STEP_CREATINGHISTORY;
	IGImageProcessing::SetCurrentRequest (this);	
	IGFrame *pFrame = GetFrame();
	if (!pFrame)
		return false;
	pFrame->Initialize();
	pFrame->CreateHistory (getInstance(), MAKEINTRESOURCE(IDR_FRAMEHISTORY));
	return IGREQUEST_PROCESSING;
}

bool IGRequestSave::InternalProcess()
{
	wstring wsPath;
	if (!GetParameter (IGREQUEST_PATH, wsPath))
		return false;
	return SUCCEEDED (m_pWorkspace->SaveFrame (GetFrameId(), VARIANT_TRUE, CComBSTR (wsPath.c_str())));
}

bool IGRequestMergeLayer::InternalProcess()
{
	size_t szNextComa = wstring::npos;
	wstring wsCurId;
	list <CxImage*> lpLayers;
	int nMinId = 1000000;
	int nMaxId = -1;
	do
	{
		szNextComa = m_wsLayerFromIds.find (L',');
		wsCurId = m_wsLayerFromIds.substr (0, szNextComa);
		int nCurId = ::_wtoi (wsCurId.c_str());
		if (nMinId > nCurId)
			nMinId = nCurId;
		if (nMaxId < nCurId)
			nMaxId = nCurId;
		lpLayers.push_back (GetFrame()->GetLayer (nCurId));		
		if (szNextComa != wstring::npos)
			m_wsLayerFromIds = m_wsLayerFromIds.substr (szNextComa + 1);
	} while (szNextComa != wstring::npos);	
	_ASSERTE ((lpLayers.size() >= 2 && ((nMaxId - nMinId) < IGFRAME_MAX_NBLAYERS)) && L"Invalid number of layers to merge");
	if ((lpLayers.size() < 2 || ((nMaxId - nMinId) >= IGFRAME_MAX_NBLAYERS)))
		return false;
	_ASSERTE (GetFrame()->GetLayer (m_nLayerId) == lpLayers.front() && L"Destination layer should be equal to first source layer");
	if (GetFrame()->GetLayer (m_nLayerId) != lpLayers.front())
		return false;
	IGIPMergeLayers *pMergeLayerImageProcessing = new IGIPMergeLayers (GetFrame(), 
																		new IGIPMergeLayersMessage (lpLayers));
	return pMergeLayerImageProcessing->Start();
}

bool IGRequestFilter::InternalProcess()
{
	if (m_bOriginal)
		GetFrame()->RestoreStep(0);
	if (!m_wsFaceDescriptor.empty()){
		GetFrame()->GetLayer(m_nLayerId)->SetFaceDescriptor (m_wsFaceDescriptor);
		IGLibrary::IGFaceDescriptor *pFaceDescriptor = GetFrame()->GetLayer(m_nLayerId)->GetFaceDescriptor();
		GetFrame()->SetProperty (IGLibrary::IGFrameProperties::IGFRAMEPROPERTY_FACEDESCRIPTOR, *pFaceDescriptor);
	}
	IGBrush *pBrush = IGBrush::CreateBrush (*m_spFrameMgr);
	if (!pBrush)
		return false;
	if (m_nFilterId == IGIPFILTER_FACE_EFFECT)
		return GetFrame()->StartFaceEffect (m_nFaceEffectId, (COLORREF)*pBrush, (COLORREF)*pBrush, m_dParam1, m_dParam2, m_dParam3, m_wsPicName1.c_str(), m_wsPicName2.c_str());
	return GetFrame()->StartFiltering (m_nFilterId);
}

bool IGRequestFilter::IsFullUpdatingFrame()
{
	return (m_nFilterId == IGIPFILTER_FACE_EFFECT) || !GetFrame()->GetLayer (m_nLayerId)->SelectionIsValid();
}

bool IGRequestIndex::InternalProcess()
{
	return GetFrame()->StartIndexing (m_nIndexerId, m_pMarker2, m_pMarker);
}

bool IGRequestIndex::IsFullUpdatingFrame()
{
	return true;
}

bool IGRequestDrawLines::InternalProcess()
{	
	if (!m_spFrameMgr)
		return false;
	IGBrush *pBrush = IGBrush::CreateBrush (*m_spFrameMgr);
	if (!pBrush)
		return false;
	pBrush->SetTransparency (m_nTransparency);
	return GetFrame()->StartDrawing (m_lPts, pBrush);
}

bool IGRequestRubber::InternalProcess()
{	
	if (!m_spFrameMgr)
		return false;
	IGBrush *pBrush = IGBrush::CreateBrush (*m_spFrameMgr);
	if (!pBrush)
		return false;
	pBrush->SetColor (0xFFFFFFFF);
	pBrush->SetTransparency (m_nTransparency);
	pBrush->SetType (IGBRUSH_TYPE_ENUM_SHAPE_ROUND_SOLID);
	return GetFrame()->StartDrawing (m_lPts, pBrush);
}

bool IGRequestCloseFrame::InternalProcess()
{
	return SUCCEEDED (m_pWorkspace->RemoveFrame (GetFrameId()));
}

bool IGRequestTerminate::InternalProcess()
{
	// leave !!
	m_pWorkspace->DisconnectUser(VARIANT_FALSE);
	exit(0);
	return true;
}

bool IGRequestShow::InternalProcess()
{
	GetMainWindow()->ShowOwner(IsVisible() ? TRUE : FALSE);
	return true;
}

bool IGRequestDestroyAccount::InternalProcess()
{
	if (FAILED (m_pWorkspace->DisconnectUser (VARIANT_FALSE)))
		return false;
	return m_spConfigManager->DestroyUserAccount();
}
