#pragma once
#include "IGImageProcessing.h"
#include "IGInternalMessage.h"

namespace IGLibrary
{
	class IGInternalProcessing :
		public IGImageProcessing
	{
	public:
		IGInternalProcessing (IGFrame *pFrame, IGInternalMessage *pMessage);
		virtual ~IGInternalProcessing();

	protected:
		virtual bool OnImageProcessing (CxImage& image, IGImageProcMessage& message);
		virtual EnumStepType GetStepId() const;
		virtual int GetFirstParam (IGImageProcMessage& message, CxImage& image) const;
		virtual int GetSecondParam (IGImageProcMessage& message, CxImage& image) const;

	private:
		IGInternalMessage *m_pMessage;
	};
}
