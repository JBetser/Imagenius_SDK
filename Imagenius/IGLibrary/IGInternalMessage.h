#pragma once
#include "IGImageProcMessage.h"

namespace IGLibrary
{
	typedef enum {IGINTERNALPROC_RESAMPLE = 1} IGINTERNALPROC_TYPE;

	class IGInternalMessage :
		public IGImageProcMessage
	{
	public:
		IGInternalMessage (IGINTERNALPROC_TYPE eProcType);
		virtual ~IGInternalMessage();

		inline IGINTERNALPROC_TYPE GetInternalProcType() const;
		inline EnumStepType GetStepId() const;

		virtual int GetFirstParam (const IGFrame& frame) {return 0;}
		virtual int GetSecondParam (const IGFrame& frame) {return 0;}		

	private:
		IGINTERNALPROC_TYPE m_eProcType;
	};

	inline IGINTERNALPROC_TYPE IGInternalMessage::GetInternalProcType() const { 
		return m_eProcType;
	}

	class IGInternalMessageResize :
		public IGInternalMessage
	{
	public:
		IGInternalMessageResize() : IGInternalMessage (IGINTERNALPROC_RESAMPLE) {
			m_ptSize.x = -1;
			m_ptSize.y = -1;
		}
		virtual ~IGInternalMessageResize() {}
		virtual int GetFirstParam (const IGFrame& frame);
		virtual int GetSecondParam (const IGFrame& frame);
		virtual LPCWSTR GetImageProcessingName() const
		{
			return L"Resize";
		}
		virtual UINT ComputeNbProgressSteps (const IGFrame &frame)
		{
			GetFirstParam(frame); // init m_ptSize
			int nStretchMode = 0;
			if (!frame.GetProperty(IGFrameProperties::IGFRAMEPROPERTY_STRETCHING, nStretchMode))
				return 0;
			if (nStretchMode == (int)IGFRAMEPROPERTY_STRETCHING_NO)
				return 2;
			int nNbStepsPerLayer = (nStretchMode == (int)IGFRAMEPROPERTY_STRETCHING_FAST ? 1 : frame.GetHeight());
			return nNbStepsPerLayer * frame.GetNbLayers();
		}
	private:
		POINT m_ptSize;
	};

	inline EnumStepType IGInternalMessage::GetStepId() const
	{
		switch (m_eProcType)
		{
		case IGINTERNALPROC_RESAMPLE:
			return IGFRAMEHISTORY_STEP_RESIZE;
		}
		return IGFRAMEHISTORY_STEP_LAYERCHANGE;
	}
}