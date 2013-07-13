#pragma once
#include "ximage.h"
#include "IGSmartPtr.h"

namespace IGLibrary
{
	class IGRegion;
	class IGSmartLayer;
	class IGIPFaceEffectMessage;
	class IGFrame;

	typedef enum {IGIPFACE_EFFECT_COLORIZE	= 0,
				IGIPFACE_EFFECT_BRIGHTNESS	=	1,
				IGIPFACE_EFFECT_SHARPEN = 2,
				IGIPFACE_EFFECT_BLUR = 3,
				IGIPFACE_EFFECT_SATURATE = 4,
				IGIPFACE_EFFECT_REDEYE = 5,
				IGIPFACE_EFFECT_MORPHING = 6,
				IGIPFACE_EFFECT_REPAIR = 7,
				IGIPFACE_EFFECT_EYE_COLOR = 8,
				IGIPFACE_EFFECT_MATIFY	= 9,
				IGIPFACE_EFFECT_POX	= 10,
				IGIPFACE_EFFECT_HOOLIGAN	= 11,
				IGIPFACE_EFFECT_SURPRISED	= 12,
				IGIPFACE_EFFECT_RANDOM	= 13,
				IGIPFACE_EFFECT_CARTOON = 14}	IGIPFACE_EFFECT;

	typedef enum {IGIPMORPHING_TYPE_SMILE	= 0,
		IGIPMORPHING_TYPE_SAD	=	1,
		IGIPMORPHING_TYPE_ANGRY = 2,
		IGIPMORPHING_TYPE_BIGNOSE = 3,
		IGIPMORPHING_TYPE_CROSSEYED = 4,
		IGIPMORPHING_TYPE_CHINESE = 5,
		IGIPMORPHING_TYPE_UGLY = 6}	IGIPMORPHING_TYPE;

	class IGFaceEffect
	{
	public:
		IGFaceEffect (IGSmartLayer& layer, bool bSelectFace);
		virtual ~IGFaceEffect(void);

		static bool Init (HRSRC hBlood1, HRSRC hBlood2, HRSRC hBlood3, HRSRC hBlood4,
							HRSRC hTears1, HRSRC hTears2, HRSRC hTears3,
							HRSRC hPox1, HRSRC hPox2, HRSRC hStar1, HRSRC hStar2);
		static bool Create (IGIPFaceEffectMessage *pEffectMessage, IGSmartLayer& layer, int nDescriptorIdx, IGFaceEffect*& pEffect);

		bool Process();
		bool IsSelectNeeded() { return m_bSelectFace; }
		virtual bool ProcessBackground() { return true; }
	protected:
		virtual bool InternalProcess() = 0;
		void SetFaceIdx (int nDescriptorIdx);
		void applyTexture (CxImage& text, int nNbOcc, float fRelativeSize, bool bNeedSel = true, bool bKeepSize = true, bool bApplyRot = true, bool bAvoidEyesAndMouth = true, bool bOnMinGradient = false);
		void applyDoubleTexture (CxImage& text, CxImage& text2, int nNbOcc, float fRelativeSize1, float fRelativeSize2, bool bNeedSel = true, bool bApplyRot = true, bool bOnMinGradient = false);

		static IGSmartPtr <IGFrame> mg_spBlood1;
		static IGSmartPtr <IGFrame> mg_spBlood2;
		static IGSmartPtr <IGFrame> mg_spBlood3;
		static IGSmartPtr <IGFrame> mg_spBlood4;
		static IGSmartPtr <IGFrame> mg_spTears1;
		static IGSmartPtr <IGFrame> mg_spTears2;
		static IGSmartPtr <IGFrame> mg_spTears3;
		static IGSmartPtr <IGFrame> mg_spPox1;
		static IGSmartPtr <IGFrame> mg_spPox2;
		static IGSmartPtr <IGFrame> mg_spStar1;
		static IGSmartPtr <IGFrame> mg_spStar2;

		IGSmartLayer& m_layer;
		bool m_bSelectFace;
		IGRegion* m_pCurReg;
	};

	class IGFaceEffectColorize : public IGFaceEffect
	{
	public:
		IGFaceEffectColorize (IGSmartLayer& layer, COLORREF color, float fAlpha, int nMixVal);
	protected:
		virtual bool InternalProcess();
	private:
		COLORREF m_color;
		float m_fAlpha;
		bool m_bFun;
	};

	class IGFaceEffectBrightness : public IGFaceEffect
	{
	public:
		IGFaceEffectBrightness (IGSmartLayer& layer, int nBrightness, int nContrast);
	protected:
		virtual bool InternalProcess();
	private:
		int m_nBrightness;
		int m_nContrast;
	};

	class IGFaceEffectSharpen : public IGFaceEffect
	{
	public:
		IGFaceEffectSharpen (IGSmartLayer& layer, float fRadius, float fAmount, int nThreshold);
	protected:
		virtual bool InternalProcess();
	private:
		float m_fRadius;
		float m_fAmount;
		int m_nThreshold;
	};

	class IGFaceEffectBlur : public IGFaceEffect
	{
	public:
		IGFaceEffectBlur (IGSmartLayer& layer, float fRadius, int nThreshold);
	protected:
		virtual bool InternalProcess();
	private:
		float m_fRadius;
		int m_nThreshold;
	};

	class IGFaceEffectRedEyeRemove : public IGFaceEffect
	{
	public:
		IGFaceEffectRedEyeRemove (IGSmartLayer& layer, float fStrength);
	protected:
		virtual bool InternalProcess();
	private:
		float m_fStrength;
	};

	class IGFaceEffectChangeEyeColor : public IGFaceEffect
	{
	public:
		IGFaceEffectChangeEyeColor (IGSmartLayer& layer, COLORREF col, float fStrength, bool bAuto);
	protected:
		virtual bool InternalProcess();
	private:
		COLORREF m_color;
		float m_fStrength;
		bool m_bAuto;
	};

	class IGFaceEffectCartoon : public IGFaceEffect
	{
	public:
		IGFaceEffectCartoon (IGSmartLayer& layer);
	protected:
		virtual bool InternalProcess();
	private:
	};

	class IGFaceEffectMorphing : public IGFaceEffect
	{
	public:
		IGFaceEffectMorphing (IGSmartLayer& layer, int nType, float fCoeff1, float fCoeff2);
	protected:
		virtual bool InternalProcess();
		virtual bool ProcessBackground();
	private:
		bool crossEyed(const RECT& rcEyeLeft, const RECT& rcEyeRight);
		bool bigNose(const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth);
		bool smile(const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth, bool bNoLove = false);
		bool sad(const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth);
		bool angry(const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth);
		bool ugly (const RECT& rcEyeLeft, const RECT& rcEyeRight, const RECT& rcMouth);

		float m_fCoeff1;
		float m_fCoeff2;
		int m_nType;
	};

	class IGFaceEffectSaturate : public IGFaceEffect
	{
	public:
		IGFaceEffectSaturate (IGSmartLayer& layer, int nSaturation);
	protected:
		virtual bool InternalProcess();
	private:
		int m_nSaturation;
	};

	class IGFaceEffectRepair : public IGFaceEffect
	{
	public:
		IGFaceEffectRepair (IGSmartLayer& layer);
		virtual bool InternalProcess();
	};

	class IGFaceEffectPox : public IGFaceEffect
	{
	public:
		IGFaceEffectPox (IGSmartLayer& layer, float fStrength);
	protected:
		virtual bool InternalProcess();
	private:
		float m_fStrength;
	};

	class IGFaceEffectHooligan : public IGFaceEffect
	{
	public:
		IGFaceEffectHooligan (IGSmartLayer& layer, float fStrength);
	protected:
		virtual bool InternalProcess();
	private:
		float m_fStrength;
	};

	class IGFaceEffectSurprised : public IGFaceEffect
	{
	public:
		IGFaceEffectSurprised (IGSmartLayer& layer, float fStrength);
	protected:
		virtual bool InternalProcess();
	private:
		float m_fStrength;
	};
}

