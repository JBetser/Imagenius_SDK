#include "StdAfx.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGThread.h"
#include "IGIPFilter.h"
#include "IGBrush.h"
#include "IGLPE.h"
#include "IGProperties.h"

using namespace IGLibrary;

bool IGIPFilter::OnImageProcessing (CxImage& image, IGImageProcMessage& message)
{
	IGIPFilterMessage *pIGThreadMessage = dynamic_cast <IGIPFilterMessage *> (&message);
	_ASSERTE (pIGThreadMessage && L"Wrong IGThread message, image processing aborted");
	if (!pIGThreadMessage || !m_pFrame)
		return false;
	IGLayer *pLayer = m_pFrame->GetLayer (m_pFrame->GetLayerPos (image.GetWorkingLayer()));
	if (!pLayer)
		return false;

	switch (pIGThreadMessage->m_eFilterType)
	{
	case IGIPFILTER_BLUR:
		return pLayer->GaussianBlur (5.0f);
	case IGIPFILTER_GRADIENT:
		{
			long tKernel [9];
			pLayer->GrayScale();
			CxImage g1(*pLayer), g2(*pLayer), g3(*pLayer), g4(*pLayer);
			tKernel[0] = 0;tKernel[1] = 1;tKernel[2] = -1;
			tKernel[3] = 0;tKernel[4] = 1;tKernel[5] = -1;
			tKernel[6] = 0;tKernel[7] = 1;tKernel[8] = -1;
			g1.Filter (tKernel, 3, 4, 0);
			tKernel[0] = -1;tKernel[1] = 1;tKernel[2] = 0;
			tKernel[3] = -1;tKernel[4] = 1;tKernel[5] = 0;
			tKernel[6] = -1;tKernel[7] = 1;tKernel[8] = 0;
			g2.Filter (tKernel, 3, 4, 0);
			tKernel[0] = -1;tKernel[1] = -1;tKernel[2] = -1;
			tKernel[3] = 1;tKernel[4] = 1;tKernel[5] = 1;
			tKernel[6] = 0;tKernel[7] = 0;tKernel[8] = 0;
			g3.Filter (tKernel, 3, 4, 0);
			tKernel[0] = 0;tKernel[1] = 0;tKernel[2] = 0;
			tKernel[3] = 1;tKernel[4] = 1;tKernel[5] = 1;
			tKernel[6] = -1;tKernel[7] = -1;tKernel[8] = -1;
			g4.Filter (tKernel, 3, 4, 0);
			pLayer->Copy (g1, true, false, false, false);
			*pLayer += g2;
			*pLayer += g3;
			*pLayer += g4;
			return true;
		}
	case IGIPFILTER_GRADIENT_MORPHO:
		return pLayer->ComputeGradient (true);
	case IGIPFILTER_GRAY:
		return pLayer->GrayScale();
	case IGIPFILTER_SKIN:
		return pLayer->FilterSkin();
	case IGIPFILTER_SKIN_UNNOISED:
		return pLayer->FilterSkinUnnoised();
	case IGIPFILTER_EYES:
		{
			RECT rcNo = {-1,-1,-1,-1};
			return pLayer->RedEyeRemove (rcNo, rcNo, 0.5f);
		}
	case IGIPFILTER_EYES_COLOR:
		{
			RECT rcNo = {-1,-1,-1,-1};
			RGBQUAD rgbq;
			IGTexture texture;
			if (!m_pFrame->GetProperty (IGProperties::IGPROPERTY_CURTEXTURE, texture))
				return false;
			COLORREF col = (COLORREF)texture;
			rgbq.rgbRed = GetRValue (col);
			rgbq.rgbBlue = GetBValue (col);
			rgbq.rgbGreen = GetGValue (col);
			rgbq.rgbReserved = 0x00;
			rgbq = pLayer->RGBtoHSL (rgbq);
			return pLayer->ChangeEyeColor (rcNo, rcNo, rgbq.rgbRed, rgbq.rgbGreen, 50.0f);
		}
	case IGIPFILTER_FFT:
		{
			CxImage copy (*pLayer);
			copy.GrayScale();
			CxImage res;
			if (!pLayer->FFT2 (&copy, NULL, &res, NULL))
				return false;
			pLayer->Copy (res);
			return true;
		}
	case IGIPFILTER_FACE_EFFECT:
		{
			IGIPFaceEffectMessage *pIGIPFaceEffectMessage = dynamic_cast <IGIPFaceEffectMessage *> (pIGThreadMessage);
			_ASSERTE (pIGIPFaceEffectMessage && L"Wrong IGThread message, face effect image processing aborted");
			if (!pIGIPFaceEffectMessage)
				return false;
			return pLayer->ProcessFaceEffect (pIGIPFaceEffectMessage);
		}
	case IGIPFILTER_COLORIZE:
		{
			RGBQUAD rgbq;
			IGTexture texture;
			if (!m_pFrame->GetProperty (IGProperties::IGPROPERTY_CURTEXTURE, texture))
				return false;
			COLORREF col = (COLORREF)texture;
			rgbq.rgbRed = GetRValue (col);
			rgbq.rgbBlue = GetBValue (col);
			rgbq.rgbGreen = GetGValue (col);
			rgbq.rgbReserved = 0x00;
			rgbq = pLayer->RGBtoHSL (rgbq);
			return pLayer->Colorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, (float)texture.GetTransparency() / 255.0f);
		}
	case IGIPFILTER_BRIGHTNESS:
		{
			int nStrength = -1;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_STRENGTH, nStrength) || (nStrength == -1))
				return false;
			return pLayer->Light ((long)((float)nStrength * 2.55f));
		}
	case IGIPFILTER_CONTRAST:
		{
			int nStrength = -1;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_STRENGTH, nStrength) || (nStrength == -1))
				return false;
			return pLayer->Light (0, nStrength);
		}
	case IGIPFILTER_SATURATE:
		{
			int nStrength = -1;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_STRENGTH, nStrength) || (nStrength == -1))
				return false;
			return pLayer->Saturate (nStrength);
		}
	case IGIPFILTER_MORPHING:
		{
			int nRadius = -1, nType = -1, nPosX = -1, nPosY = -1, nDirectionX = 0, nDirectionY = 0;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_STRENGTH, nRadius) || (nRadius == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_TYPE, nType) || (nType == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_POSX, nPosX) || (nPosX == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_POSY, nPosY) || (nPosY == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_DIRECTIONX, nDirectionX))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_DIRECTIONY, nDirectionY))
				return false;
			// convert DZ coords to IG
			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGConvertible::FromDZtoIGRatios (image.GetWidth(), image.GetHeight(), dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			POINT ptPos; ptPos.x = (int)((double)nPosX * dSeadragonToPixelRatioX); ptPos.y = (int)((double)nPosY * dSeadragonToPixelRatioY);
			POINT ptDir; ptDir.x = (int)((double)nDirectionX * dSeadragonToPixelRatioX); ptDir.y = (int)((double)nDirectionY * dSeadragonToPixelRatioY);
			// convert IG coords to CX
			IGConvertible::FromIGtoCxCoords(ptPos, image.GetHeight());
			ptDir.y *= -1;
			// apply morphing
			return pLayer->Morphing (ptPos.x, ptPos.y, (float)ptDir.x, (float)ptDir.y, (float)nRadius, nType);
		}
	case IGIPFILTER_NEGATIVE:
			return pLayer->Negative();
	case IGIPFILTER_TRANSPARENCY:
		{
			int nAlpha = 0;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_STRENGTH, nAlpha))
				return false;
			pLayer->AlphaDelete();
			return pLayer->AlphaCreate ((BYTE)((float)nAlpha * 2.55f));
		}
	case IGIPFILTER_PAINT:
		{
			int nPosX = -1, nPosY = -1;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_POSX, nPosX) || (nPosX == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_POSY, nPosY) || (nPosY == -1))
				return false;
			int nTolerance = 0;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_TOLERANCE, nTolerance))
				return false;
			RGBQUAD rgbq;
			IGTexture texture;
			if (!m_pFrame->GetProperty (IGProperties::IGPROPERTY_CURTEXTURE, texture))
				return false;
			// convert DZ coords to IG
			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGConvertible::FromDZtoIGRatios (image.GetWidth(), image.GetHeight(), dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			POINT ptPos; ptPos.x = (int)((double)nPosX * dSeadragonToPixelRatioX); ptPos.y = (int)((double)nPosY * dSeadragonToPixelRatioY);
			// convert IG coords to CX
			IGConvertible::FromIGtoCxCoords(ptPos, image.GetHeight());
			COLORREF col = (COLORREF)texture;
			rgbq.rgbRed = GetRValue (col);
			rgbq.rgbBlue = GetBValue (col);
			rgbq.rgbGreen = GetGValue (col);
			rgbq.rgbReserved = 0x00;			
			return pLayer->FloodFill (ptPos.x, ptPos.y, rgbq, nTolerance);
		}
	case IGIPFILTER_PAINTGRADIENT:
		{
			int nPosX = -1, nPosY = -1, nDirectionX = 0, nDirectionY = 0;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_POSX, nPosX) || (nPosX == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_POSY, nPosY) || (nPosY == -1))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_DIRECTIONX, nDirectionX))
				return false;
			if (!m_pFrame->GetRequestProperty(IGIPFILTER_PARAM_DIRECTIONY, nDirectionY))
				return false;
			RGBQUAD rgbq, rgbqBkgnd;
			IGTexture texture, bkgndTexture;
			if (!m_pFrame->GetProperty (IGProperties::IGPROPERTY_CURTEXTURE, texture))
				return false;
			COLORREF col = (COLORREF)texture;
			rgbq.rgbRed = GetRValue (col);
			rgbq.rgbBlue = GetBValue (col);
			rgbq.rgbGreen = GetGValue (col);
			rgbq.rgbReserved = 0x00;	
			if (!m_pFrame->GetProperty (IGProperties::IGPROPERTY_BKGNDTEXTURE, bkgndTexture))
				return false;
			// convert DZ coords to IG
			double dSeadragonToPixelRatioX, dSeadragonToPixelRatioY;
			IGConvertible::FromDZtoIGRatios (image.GetWidth(), image.GetHeight(), dSeadragonToPixelRatioX, dSeadragonToPixelRatioY);
			POINT ptPos; ptPos.x = (int)((double)nPosX * dSeadragonToPixelRatioX); ptPos.y = (int)((double)nPosY * dSeadragonToPixelRatioY);
			POINT ptDir; ptDir.x = (int)((double)nDirectionX * dSeadragonToPixelRatioX); ptDir.y = (int)((double)nDirectionY * dSeadragonToPixelRatioY);
			// convert IG coords to CX
			IGConvertible::FromIGtoCxCoords(ptPos, image.GetHeight());
			ptDir.y *= -1;
			COLORREF colBkgnd = (COLORREF)bkgndTexture;
			rgbqBkgnd.rgbRed = GetRValue (colBkgnd);
			rgbqBkgnd.rgbBlue = GetBValue (colBkgnd);
			rgbqBkgnd.rgbGreen = GetGValue (colBkgnd);
			rgbqBkgnd.rgbReserved = 0x00;	
			return pLayer->FillGradient (ptPos.x, ptPos.y, ptDir.x, ptDir.y, rgbq, rgbqBkgnd);
		}
	case IGIPFILTER_REPAIR:
		return pLayer->Repair (0.25f, 3, 0);
	case IGIPFILTER_SHARPEN:
		return pLayer->UnsharpMask (5.0f, 0.5f, 0);	
	case IGIPFILTER_MATIFY:
		{
			COLORREF col = IGIPFACE_EFFECT_MATIFY_COLORCODE;
			RGBQUAD rgbq;
			rgbq.rgbRed = GetRValue (col);
			rgbq.rgbBlue = GetBValue (col);
			rgbq.rgbGreen = GetGValue (col);
			rgbq.rgbReserved = 0x00;
			rgbq = pLayer->RGBtoHSL (rgbq);
			return pLayer->Colorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 1.0f);
		}
	case IGIPFILTER_THRESHOLD:
		return pLayer->AdaptiveThreshold(0);
	case IGIPFILTER_AUTOROTATE:
		return pLayer->AutoRotate();
	case IGIPFILTER_SKETCH:
		return pLayer->Sketch();
	case IGIPFILTER_CARTOON:
		return pLayer->Cartoon();
	case IGIPFILTER_OILPAINTING:
		return pLayer->OilPainting();
	case IGIPFILTER_WATERPAINTING:
		return pLayer->WaterPainting();
	case IGIPFILTER_SEPIA:
		{
			RGBQUAD rgbq;
			rgbq.rgbRed = 112;
			rgbq.rgbBlue = 20;
			rgbq.rgbGreen = 66;
			rgbq.rgbReserved = 0x00;
			rgbq = pLayer->RGBtoHSL (rgbq);
			return pLayer->Colorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 1.0f);
		}
	case IGIPFILTER_VINTAGE:
		{
			pLayer->UnsharpMask (5.0f, 0.5f, 0);	
			pLayer->UnsharpMask (5.0f, 0.5f, 0);	
			pLayer->UnsharpMask (5.0f, 0.5f, 0);	
			RGBQUAD rgbq;
			rgbq.rgbRed = 112;
			rgbq.rgbBlue = 20;
			rgbq.rgbGreen = 66;
			rgbq.rgbReserved = 0x00;
			rgbq = pLayer->RGBtoHSL (rgbq);
			return pLayer->Colorize (rgbq.rgbRed, rgbq.rgbGreen, rgbq.rgbBlue, 0.25f);
		}
	case IGIPFILTER_DITHER:
		return pLayer->Dither();
	case IGIPFILTER_CLAY:
		return pLayer->Clay();
	case IGIPFILTER_FILTER1:
		{
			CxImage *layer1 (*pLayer);
			CxImage *layer2 (*pLayer);
			CxImage *layer3 (*pLayer);
			int nAlpha = 0;

			// Contrast the image using the curve: out 116, in 139
			long contrast_in = 139, contrast_out = 116;
			CxImage *contrast_IN (*pLayer);
			CxImage *contrast_OUT (*pLayer);
			contrast_IN->Mix(*contrast_OUT, CxImage::OpAvg, false); 

			contrast_IN->Light(0, contrast_in);
			contrast_OUT->Light(0, contrast_out);

			// Add contrast IN & OUT images
			// Apply layer 2 as screen with opacity 48
			nAlpha = 48;
			layer2->AlphaDelete();
			layer2->AlphaCreate ((BYTE)((float)nAlpha * 2.55f));

			// Apply layer 3 as overlay with opacity 35
			nAlpha = 35;
			layer3->AlphaDelete();
			layer3->AlphaCreate ((BYTE)((float)nAlpha * 2.55f));

			// Adding the three layers 
			layer1->Mix(*layer2, CxImage::OpAvg, false);
			layer1->Mix(*layer3, CxImage::OpAvg, false);

			return layer1;
		}
	case IGIPFILTER_FILTER2:
		{
			int nAlpha =0;
			CxImage *layerA (*pLayer);
			CxImage *layerB (*pLayer);
			CxImage *layerC (*pLayer);
			// Apply the original as screen on white background with opacity 90 -> layer “A”
			nAlpha = 90;
			layerA->AlphaDelete();
			layerA->AlphaCreate((BYTE)((float)nAlpha * 2.55f));

			// Copy the original, convert to grayscale, add 30% contrast and multiply with layer “A” -> layer “B”
			layerB->GrayScale();
			layerB->Light(0, (long)255*.3);
			// multiplication function

			// Apply darken 55% to the paper layer -> layer “C”
			layerC->Light((long)(-255*.55));

			// Apply layer “C” to layer “B” as screen with 20% opacity
			nAlpha = (int) (255*.2);
			layerB->AlphaDelete();
			layerB->AlphaCreate((BYTE)((float)nAlpha *2.55f));

			layerC->Mix(*layerB, CxImage::OpScreen, true); 

			return layerC;

		}

	case IGIPFILTER_FILTER3:
		{
			CxImage *layerA (*pLayer);
			CxImage *layerSrc1, *layerSrc2;
			// Duotone original with colors: #741c19 (116, 28, 25) and #d7ad7f (215, 173, 127)
			// get the two most represented colors
			//[layerSrc1 layerSrc2 ] = CxImage::kmeanClustering(*layerA);
			layerSrc1->kmeanClustering(*layerA);
			
			// Convert the given colors to HSL
			RGBQUAD dest1;
			dest1.rgbRed = 116;
			dest1.rgbGreen = 28;
			dest1.rgbBlue = 25;

			RGBQUAD dest2;
			dest2.rgbRed = 215;
			dest2.rgbGreen = 173;
			dest2.rgbBlue = 127;

			RGBQUAD dest1HSL = CxImage::RGBtoHSL(dest1);	
			RGBQUAD dest2HSL = CxImage::RGBtoHSL(dest2);	
			
			// Map source image hues with requested hues
			RGBQUAD srcHSL1, srcHSL2;
			for (long y=0; y<layerSrc1->GetWidth(); y++ ){
				for (long x=0; x<layerSrc1->GetHeight(); x++){
					srcHSL1 = CxImage::RGBtoHSL(layerSrc1->BlindGetPixelColor(x,y));
					//srcHSL2 = CxImage::RGBtoHSL(layerSrc2->BlindGetPixelColor(x,y));
					if (srcHSL1.rgbBlue ==0 && srcHSL1.rgbGreen==0 && srcHSL1.rgbRed==0)
						layerA->SetPixelColor(x, y, dest2HSL);
					else
						layerA->SetPixelColor(x,y, dest1HSL);

				}
			}





			return layerA;
		}

	}
	return false;
}

int IGIPFilter::GetFirstParam (IGImageProcMessage& message, CxImage& image) const
{
	IGIPFilterMessage *pIGThreadMessage = dynamic_cast <IGLibrary::IGIPFilterMessage *> (&message);
	if (!pIGThreadMessage)
		return false;
	return (int)image.GetLayerPos (image.GetWorkingLayer());
}

int IGIPFilter::GetSecondParam (IGImageProcMessage& message, CxImage& image) const
{
	IGIPFilterMessage *pIGThreadMessage = dynamic_cast <IGLibrary::IGIPFilterMessage *> (&message);
	if (!pIGThreadMessage)
		return false;	
	ZeroMemory (&pIGThreadMessage->m_rcSubLayer, sizeof (RECT));
	if (image.GetWorkingLayer())
		image.GetWorkingLayer()->SelectionGetBox (pIGThreadMessage->m_rcSubLayer);
	if ((pIGThreadMessage->m_rcSubLayer.right == 0) || (pIGThreadMessage->m_rcSubLayer.bottom == 0))
	{
		pIGThreadMessage->m_rcSubLayer.right = -1;
		pIGThreadMessage->m_rcSubLayer.bottom = -1;
	}
	return (int)&pIGThreadMessage->m_rcSubLayer;
}

bool IGIPIndex::OnImageProcessing (CxImage& image, IGImageProcMessage& message)
{
	IGIPIndexMessage *pIGThreadMessage = dynamic_cast <IGIPIndexMessage *> (&message);
	_ASSERTE (pIGThreadMessage && L"Wrong IGThread message, image processing aborted");
	if (!pIGThreadMessage || !m_pFrame)
		return false;
	IGLayer *pLayer = m_pFrame->GetLayer (m_pFrame->GetLayerPos (image.GetWorkingLayer()));
	if (!pLayer)
		return false;

	switch (pIGThreadMessage->m_eFilterType)
	{
	case IGIPINDEX_LPE:
		{
			int nNbMarkers = 0;
			if (pIGThreadMessage->m_pMarkerBackground && pIGThreadMessage->m_pMarkerObject) {
				IGMarker* tM [2];
				tM[0] = pIGThreadMessage->m_pMarkerBackground;
				tM[1] = pIGThreadMessage->m_pMarkerObject;
				nNbMarkers = 2;
				return pLayer->IndexLPE (tM, nNbMarkers);
			}
			else
				return pLayer->IndexLPE (NULL, nNbMarkers);
		}
	case IGIPINDEX_FACE:
		return pLayer->IndexFaces();
	case IGIPINDEX_IRIS:	// added by TQ
		return pLayer->IndexFacenIris();
//<<<<<<< HEAD
//=======
//
//>>>>>>> New Commit
	}

	return false;
}

IGIPDrawMessage::~IGIPDrawMessage()
{
	delete m_pBrush;
}

bool IGIPDraw::OnImageProcessing (CxImage& image, IGImageProcMessage& message)
{
	IGIPDrawMessage *pIGThreadMessage = dynamic_cast <IGLibrary::IGIPDrawMessage *> (&message);
	if (!pIGThreadMessage)
		return false;
	if (!m_pFrame)
		return false;
	return m_pFrame->DrawLines (pIGThreadMessage->m_lPts, *pIGThreadMessage->m_pBrush);
}

int IGIPDraw::GetSecondParam (IGImageProcMessage& message, CxImage& image) const
{	
	IGIPDrawMessage *pIGThreadMessage = dynamic_cast <IGLibrary::IGIPDrawMessage *> (&message);
	if (!pIGThreadMessage)
		return false;
	RECT *prcSubLayer = (RECT*)IGIPFilter::GetSecondParam (message, image);
	if (!prcSubLayer)
		return false;
	int nLayerWidth = image.GetWorkingLayer()->GetWidth();
	int nLayerHeight = image.GetWorkingLayer()->GetHeight();
	RECT rcDraw = IGConvertible::FromListPtsToRECT (pIGThreadMessage->m_lPts, nLayerWidth, nLayerHeight);
	int nTop = rcDraw.top;
	rcDraw.top = nLayerHeight - 1 - rcDraw.bottom;	// convert to Cx Coords
	rcDraw.bottom = nLayerHeight - 1 - nTop;
	// enlarge the region with the brush size + 1 in every direction
	int nOffsetSize = (int)pIGThreadMessage->m_pBrush->GetSize() + 1;
	rcDraw.left = max (0, rcDraw.left - nOffsetSize);
	rcDraw.right = min (nLayerWidth - 1, rcDraw.right + nOffsetSize);
	rcDraw.top = max (0, rcDraw.top - nOffsetSize);
	rcDraw.bottom = min (nLayerHeight - 1, rcDraw.bottom + nOffsetSize);
	if ((prcSubLayer->bottom != -1) && (prcSubLayer->right != -1))
	{
		prcSubLayer->left = max (prcSubLayer->left, rcDraw.left);
		prcSubLayer->right = min (prcSubLayer->right, rcDraw.right);
		prcSubLayer->top = max (prcSubLayer->top, rcDraw.top);
		prcSubLayer->bottom = min (prcSubLayer->bottom, rcDraw.bottom);
	}
	else
	{
		*prcSubLayer = rcDraw;
	}
	return (int)prcSubLayer;
}
