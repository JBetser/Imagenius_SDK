#pragma once

#include "IGConfigManager.h"
#include "IGSingleton.h"

#include <WinGDI.h>

class CxImageThread;

namespace IGLibrary
{
	class IGFrame;
	class IGImageProcessing;
	class IGImageProcessingCallback;

// multithreading message object to post message to imagenius threads
// once a IGThreadMessage is posted, NEVER ACCESS IT OR DELETE IT
class IGImageProcMessage : public IGThreadMessage
{
	// The class that owns the processing thread
	friend class CxImageThread;
	// The class that processes the message
	friend class IGImageProcessing;

public:
	IGImageProcMessage (IGImageProcessing::TYPE_IMAGE_PROC_ID nImageProcessingId);
	virtual ~IGImageProcMessage ();

	IGImageProcessing::TYPE_IMAGE_PROC_ID	GetImageProcessingId() const;
	LPCWSTR									GetImageProcessingName (IGREQUEST_ID eImageProcessingId) const;
	IGCommunicationBase						*GetRequest();

	virtual LPCWSTR	GetImageProcessingName() const;
	virtual UINT GetMsgId () const;
	virtual bool IsValid ();
	virtual UINT ComputeNbProgressSteps (const IGFrame &frame);
	virtual bool NeedOuputGeneration () const {return m_pRequest ? m_pRequest->IsUpdatingFrame() : false;}
	UINT GetNbProgressSteps() const;
	std::wstring GetGuid() const;
	int GetRequestId() const;
	void SetPosted (bool bIsPosted) {m_bPosted = bIsPosted;}
	bool IsPosted() const {return m_bPosted;}
	bool GetResult() const {return m_bResult;}
	
protected:
	IGImageProcessing::TYPE_IMAGE_PROC_ID	m_nImageProcessingId;
	IGImageProcessingCallback			*m_pCallback;
	IGCommunicationBase					*m_pRequest;
	bool								m_bPosted;
	bool								m_bResult;
	UINT								m_nRange;
	
private:
	IGSingleton <IGConfigManager>	m_spConfigMgr;
};

}