/*
 * File:	ximaig.cpp
 * Purpose:	Imagenius layered picture file format
 */
#include "stdafx.h"
#include <windows.h>
#include <atlbase.h>
#include <strstream>
#include "IGFrame.h"
#include "IGLayer.h"
#include "IGPacker.h"
#include "IGException.h"
#include "ximaig.h"
#include "../zlib/zlib.h"

#if CXIMAGE_SUPPORT_IG

#include "ximaiter.h"

#define CXIMAGEIG_MAX_NBSECTIONS	(CXIMAGEIG_MAX_NBLAYERS + CXIMAGEIG_MAX_NBSELECTIONS)
#define CXIMAGEIG_INIT_LAYERSIZE	16777216
#define CXIMAGEIG_LAYERFORMAT		CXIMAGE_FORMAT_JPG

using namespace IGLibrary;

IGException CxImageIGException ("CxImageIG");

////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_DECODE
bool CxImageIG::decodeHeader(CxFile *hFile, IGHEADER *pHeader)
{
	hFile->Seek (0, SEEK_SET);

	// write ig format header
	hFile->Read (pHeader, sizeof (IGHEADER), 1);
	if (pHeader->Manufacturer != IGHEADER_MAGICKEY)
		throw IGEXCEPTION (CxImageIGException, "decodeHeader", "pHeader->Manufacturer failed");

	_ASSERTE (((pHeader->nNbLayers + pHeader->nNbSelections) ==  pHeader->nNbSections)
					&& L"IG file is corrupted - number of sections inconsistent");
	if ((pHeader->nNbLayers + pHeader->nNbSelections) != pHeader->nNbSections)
		throw IGEXCEPTION (CxImageIGException, "decodeHeader", "pHeader->nNbSections failed");
	return true;
}

bool CxImageIG::decodeSections (CxFile *hFile, const IGHEADER *pHeader, IGSECTIONHEADER_LAYER *pLayerSections, IGSECTIONHEADER_SELECTION *pSelectionSections)
{
	hFile->Seek (sizeof (IGHEADER), SEEK_SET);

	// read section headers
	int nSizeOfSections = sizeof (IGSECTIONHEADER_LAYER) * CXIMAGEIG_MAX_NBLAYERS + sizeof (IGSECTIONHEADER_SELECTION) * CXIMAGEIG_MAX_NBSELECTIONS;
	BYTE *pSectionHeaders = new BYTE [nSizeOfSections];
	hFile->Read (pSectionHeaders, nSizeOfSections, 1);
	
	// decode section headers
	int nSelectionCurId = 0;
	int nLayerCurId = 0;
	BYTE *pSectionHeader = pSectionHeaders;
	for (int i = 0; i < pHeader->nNbSections; i++)
	{
		switch (*(IGSECTIONTYPE*)pSectionHeader)
		{
		case IGSECTION_LAYER:
			::memcpy_s (&pLayerSections [nLayerCurId++], sizeof (IGSECTIONHEADER_LAYER) * (CXIMAGEIG_MAX_NBLAYERS - nLayerCurId),
						pSectionHeader, sizeof (IGSECTIONHEADER_LAYER));
			pSectionHeader += sizeof (IGSECTIONHEADER_LAYER);
			break;

		case IGSECTION_SELECTION:
			::memcpy_s (&pSelectionSections [nSelectionCurId++], sizeof (IGSECTIONHEADER_SELECTION) * (CXIMAGEIG_MAX_NBSELECTIONS - nSelectionCurId),
						pSectionHeader, sizeof (IGSECTIONHEADER_SELECTION));
			pSectionHeader += sizeof (IGSECTIONHEADER_SELECTION);
			break;

		default:
			_ASSERTE (0 && L"IG file is corrupted - unknown section type");
			throw IGEXCEPTION (CxImageIGException, "decodeSections", "pSectionHeader failed");
		}
	}
	delete [] pSectionHeaders;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool CxImageIG::Decode(CxFile *hFile)
{
	IGHEADER igHeader;		
	if (!decodeHeader (hFile, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "Decode", "decodeHeader failed");	

	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER[CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections = new IGSECTIONHEADER_SELECTION[CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (hFile, &igHeader, &pLayerSections[0], &pSelectionSections[0]))
		throw IGEXCEPTION (CxImageIGException, "Decode", "decodeSections failed");	

	if (igHeader.nNbSelections > 1)
		throw IGEXCEPTION (CxImageIGException, "Decode", "igHeader.nNbSelections failed");

	head.biWidth = igHeader.nWidth;
	head.biHeight = igHeader.nHeight;
	info.nNumLayers = 0;

	ProgressSetRange (head.biHeight, 0);
	ProgressSetMessage (L"Decoding IG...");	

	// read layers
	IGLibrary::IGLayer *pLayer;
	int nNbSteps = 0;
	for (int i = 0; i < igHeader.nNbLayers; i++)
	{
		// create a layer on top with no alpha and selection
		LayerCreate (-1, false, false);
		pLayer = GetLayer (i);
		if (!pLayer)
			throw IGEXCEPTION (CxImageIGException, "Decode", "GetLayer failed");
		pLayer->setProgressOwner (info.hProgress);

		// read layer pixels
		BYTE *pBufImg = new BYTE [pLayerSections [i].commonHeader.nSizeBuf];
		hFile->Seek (pLayerSections[i].commonHeader.nFirstByteOffset, SEEK_SET);
		hFile->Read (pBufImg, pLayerSections [i].commonHeader.nSizeBuf, 1);
		if (!pLayer->Decode (pBufImg, pLayerSections [i].commonHeader.nSizeBuf, CXIMAGEIG_LAYERFORMAT))
			throw IGEXCEPTION (CxImageIGException, "Decode", "pLayer->Decode failed");
		delete [] pBufImg;
		// create selection
		//pLayer->SelectionCreate();
		// read layer alpha
		pLayer->AlphaCreate (255);
		hFile->Read (pLayer->pAlpha, pLayer->GetWidth() * pLayer->GetHeight(), 1);

		pLayer->info.xOffset = pLayerSections[i].ptOffset.x; // OffsetX
		pLayer->info.yOffset = pLayerSections[i].ptOffset.y; // OffsetY
		pLayer->setFrameOwner (this);
		pLayer->UpdateImageParameters();
		pLayer->SetId (i);
	}
	AddLayerCache(2);

	if (igHeader.nNbSelections > 0)
	{			
		// read selection
		BYTE *pSelectionZipped = new BYTE[pSelectionSections[0].commonHeader.nSizeBuf];
		hFile->Read (pSelectionZipped, pSelectionSections[0].commonHeader.nSizeBuf, 1);
		
		std::ostrstream strOstream;
		int nSelectionUnzippedSize = 0;
		if (IGPacker::gzipUncompress (pSelectionZipped, pSelectionSections [0].commonHeader.nSizeBuf, strOstream, nSelectionUnzippedSize) != Z_OK)
			throw IGEXCEPTION (CxImageIGException, "Decode", "gzipUncompress failed");
		delete [] pSelectionZipped;

		if (nSelectionUnzippedSize > 0)
		{
			// copy selection to dst selection rect
			BYTE *pFromSelection = (BYTE *)strOstream.str();
			CxImage *pSelectionLayer = GetLayer(igHeader.nPosWorkingLayer);
			RECT rcSel = pSelectionSections[0].rcSelection;
			BYTE *pDstSelection = pSelectionLayer->SelectionGetPointer(rcSel.left, rcSel.bottom);
			if (!pDstSelection)
			{
				pSelectionLayer->SelectionCreate();
				pDstSelection = pSelectionLayer->SelectionGetPointer (rcSel.left, rcSel.bottom);
			}

			int nLayerWidth = pSelectionLayer->GetWidth();
			int nLayerHeight = pSelectionLayer->GetHeight();
			if (((rcSel.right - rcSel.left + 1) == nLayerWidth) &&
				((rcSel.top - rcSel.bottom + 1) == nLayerHeight))
			{
				::memcpy (pDstSelection, pFromSelection, nLayerWidth * nLayerHeight);		
			}
			else
			{
				for (int i = 0; i < rcSel.top - rcSel.bottom; i++)
				{
					::memcpy (pDstSelection, pFromSelection, pSelectionSections [0].rcSelection.right - pSelectionSections [0].rcSelection.left + 1);
					pFromSelection += pSelectionSections [0].rcSelection.right - pSelectionSections [0].rcSelection.left + 1;
					pDstSelection += nLayerWidth;
				}
			}
		}
	}

	delete [] pLayerSections;
	delete [] pSelectionSections;	
	return true;
}
////////////////////////////////////////////////////////////////////////////////
#endif //CXIMAGE_SUPPORT_DECODE
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ENCODE

bool CxImageIG::encodeHeader(CxFile *hFile, const IGHEADER *pHeader)
{
	hFile->Seek (0, SEEK_SET);

	// write ig format header
	hFile->Write (pHeader, sizeof (IGHEADER), 1);
	return true;
}

bool CxImageIG::encodeSections (CxFile *hFile, const IGHEADER *pHeader, const IGSECTIONHEADER_LAYER *pLayerSections, const IGSECTIONHEADER_SELECTION *pSelectionSections)
{
	hFile->Seek (sizeof (IGHEADER), SEEK_SET);

	// encode section headers
	int nSelectionCurId = 0;
	int nLayerCurId = 0;
	for (int i = 0; i < pHeader->nNbSections; i++)
	{
		int nNbLayersLeft = pHeader->nNbLayers - nLayerCurId;
		int nNbSelectionsLeft = pHeader->nNbSelections - nSelectionCurId;
		_ASSERTE (((nNbLayersLeft == 0) || (nNbSelectionsLeft == 0) || 
				(pLayerSections [nLayerCurId].commonHeader.nSectionId != pSelectionSections [nSelectionCurId].commonHeader.nSectionId))
					&& L"Error encoding IG file");
		if ((nNbLayersLeft != 0) && (nNbSelectionsLeft != 0) && 
			(pLayerSections [nLayerCurId].commonHeader.nSectionId == pSelectionSections [nSelectionCurId].commonHeader.nSectionId))
			throw IGEXCEPTION (CxImageIGException, "encodeSections", "commonHeader.nSectionId failed");
		if ((nNbLayersLeft > 0) && (pLayerSections [nLayerCurId].commonHeader.nSectionId == i))
			hFile->Write (&pLayerSections [nLayerCurId++], sizeof (IGSECTIONHEADER_LAYER), 1);
		else if ((nNbSelectionsLeft > 0) && (pSelectionSections [nSelectionCurId].commonHeader.nSectionId == i))
			hFile->Write (&pSelectionSections [nSelectionCurId++], sizeof (IGSECTIONHEADER_SELECTION), 1);
		else
		{
			_ASSERTE (0 && L"Error encoding IG file");
			throw IGEXCEPTION (CxImageIGException, "encodeSections", "commonHeader.nSectionId failed");
		}
		_ASSERTE ((nLayerCurId <= pHeader->nNbLayers) && (nSelectionCurId <= pHeader->nNbSelections) 
			&& L"Error encoding IG file");
		if ((nLayerCurId > pHeader->nNbLayers) || (nSelectionCurId > pHeader->nNbSelections))
			throw IGEXCEPTION (CxImageIGException, "encodeSections", "nLayerCurId failed");
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
bool CxImageIG::Encode(CxFile *hFile)
{
	if (!hFile)
		throw IGEXCEPTION (CxImageIGException, "Encode", "hFile failed");
	// create ig format header
	IGHEADER	igHeader;
	igHeader.Manufacturer = IGHEADER_MAGICKEY;
	igHeader.Version = 0x01;
	igHeader.Encoding = 0x01;
	igHeader.nWidth = head.biWidth;
	igHeader.nHeight = head.biHeight;
	igHeader.nNbLayers = info.nNumLayers;
	igHeader.nNbSelections = 0;
	igHeader.nNbSections = info.nNumLayers;	
	igHeader.nPosWorkingLayer = (BYTE)GetLayerPos(dynamic_cast<IGLibrary::IGLayer*> (GetSelectionLayer()));		

	// write layer headers
	int nLayerSumHeight = 0;
	IGSECTIONHEADER_LAYER *pLayerHeader = new IGSECTIONHEADER_LAYER[CXIMAGEIG_MAX_NBLAYERS];
	for (int i = 0; i < info.nNumLayers; i++)
	{
		if (i < info.nNumLayers)
		{
			IGLibrary::IGLayer *pLayer = GetLayer (i);
			pLayerHeader[i].commonHeader.eSectionType = IGSECTION_LAYER;
			pLayerHeader[i].commonHeader.nId = (int)pLayer->GetId();
			pLayerHeader[i].commonHeader.nSectionId = i;
			pLayerHeader[i].commonHeader.nFirstByteOffset = 0;
			pLayerHeader[i].ptOffset.x = pLayer->info.xOffset;
			pLayerHeader[i].ptOffset.y = pLayer->info.yOffset;
			pLayerHeader[i].ptSize.x = pLayer->GetWidth();
			pLayerHeader[i].ptSize.y = pLayer->GetHeight();
			pLayerHeader[i].nSubLayers = 0;
			nLayerSumHeight += pLayer->GetHeight();		
		}
	}
	if (info.nNumLayers < CXIMAGEIG_MAX_NBLAYERS)
		ZeroMemory (&pLayerHeader [info.nNumLayers], sizeof(IGSECTIONHEADER_LAYER) * (CXIMAGEIG_MAX_NBLAYERS - info.nNumLayers));	

	ProgressSetRange (nLayerSumHeight, 0);
	ProgressSetMessage (L"Encoding picture...");	
	
	// write layer data
	if (!hFile->Seek (sizeof(IGHEADER) + sizeof (IGSECTIONHEADER_LAYER) * CXIMAGEIG_MAX_NBLAYERS + sizeof (IGSECTIONHEADER_SELECTION) * CXIMAGEIG_MAX_NBSELECTIONS, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "Encode", "hFile->Seek failed");
	// write layers
	IGLibrary::IGLayer	*pLayer;
	for (int i = 0; i < info.nNumLayers; i++)
	{
		// write layer pixels
		pLayer = GetLayer (i);
		if (i == 0)
			pLayerHeader [i].commonHeader.nFirstByteOffset = sizeof(IGHEADER) + sizeof (IGSECTIONHEADER_LAYER) * CXIMAGEIG_MAX_NBLAYERS + sizeof (IGSECTIONHEADER_SELECTION) * CXIMAGEIG_MAX_NBSELECTIONS;
		else
			pLayerHeader [i].commonHeader.nFirstByteOffset = pLayerHeader [i - 1].commonHeader.nEndByteOffset;
		float fCurQuality = pLayer->GetJpegQualityF();
		pLayer->SetJpegQualityF (100.0f);
		
		BYTE *pBuf = new BYTE [CXIMAGEIG_INIT_LAYERSIZE];
		CxMemFile memFile (pBuf, CXIMAGEIG_INIT_LAYERSIZE);
		if (!pLayer->Encode (&memFile, CXIMAGEIG_LAYERFORMAT))
			throw IGEXCEPTION (CxImageIGException, "Encode", "pLayer->Encode failed");
		BYTE *pBufImg = memFile.GetBuffer();
		long nSizeBufImg = memFile.Tell();

		pLayer->SetJpegQualityF (fCurQuality);

		hFile->Write (pBufImg, nSizeBufImg, 1);
		delete [] pBufImg;
		pLayerHeader [i].commonHeader.nSizeBuf = nSizeBufImg;
		// write layer alpha
		if (!pLayer->pAlpha)
			pLayer->AlphaCreate(255);
		int nNbPixels = pLayer->GetWidth() * pLayer->GetHeight();
		hFile->Write (pLayer->pAlpha, nNbPixels, 1);
		pLayerHeader [i].commonHeader.nEndByteOffset = pLayerHeader [i].commonHeader.nFirstByteOffset + nSizeBufImg + nNbPixels;
	}
	// write section layer headers
	if (!hFile->Seek (sizeof(IGHEADER), SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "Encode", "hFile->Seek failed");
	hFile->Write (pLayerHeader, sizeof (IGSECTIONHEADER_LAYER) * info.nNumLayers, 1);
	
	// write section selection headers
	if (IsSelection())
	{
		// the selection is saved as well
		igHeader.nNbSelections = 1; 
		igHeader.nNbSections++;
		RECT rcSel;
		CxImage	*pSelectionLayer = GetSelectionLayer();
		pSelectionLayer->SelectionGetBox(rcSel);		
		IGSECTIONHEADER_SELECTION *pSelectionHeader = new IGSECTIONHEADER_SELECTION;
		pSelectionHeader->commonHeader.eSectionType = IGSECTION_SELECTION;
		pSelectionHeader->commonHeader.nFirstByteOffset = pLayerHeader [info.nNumLayers - 1].commonHeader.nEndByteOffset;
		pSelectionHeader->commonHeader.nSizeBuf = (rcSel.right - rcSel.left + 1) * (rcSel.top - rcSel.bottom + 1);
		pSelectionHeader->commonHeader.nEndByteOffset = pSelectionHeader->commonHeader.nFirstByteOffset + pSelectionHeader->commonHeader.nSizeBuf;
		pSelectionHeader->commonHeader.nId = IGFrameHistory::GetMaxSelectionId() + 1;
		pSelectionHeader->commonHeader.nSectionId = (BYTE)info.nNumLayers;
		pSelectionHeader->nLayerId = (int)(dynamic_cast<IGLibrary::IGLayer&> (*pSelectionLayer)).GetId();
		pSelectionHeader->rcSelection = rcSel;
		if (!hFile->Seek (sizeof(IGHEADER) + sizeof (IGSECTIONHEADER_LAYER) * info.nNumLayers, SEEK_SET))
			throw IGEXCEPTION (CxImageIGException, "Encode", "hFile->Seek failed");
		hFile->Write (pSelectionHeader, sizeof (IGSECTIONHEADER_SELECTION), 1);

		if (!writeSelection (*hFile, *pSelectionHeader, *pSelectionLayer))
			throw IGEXCEPTION (CxImageIGException, "Encode", "writeSelection failed");

		delete pSelectionHeader;
	}
	delete [] pLayerHeader;

	// write ig format header
	if (!hFile->Seek (0, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "Encode", "hFile->Seek failed");
	hFile->Write (&igHeader, sizeof(IGHEADER), 1);

	if (!hFile->Seek (0, SEEK_END))
		throw IGEXCEPTION (CxImageIGException, "Encode", "hFile->Seek failed");
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ENCODE

void CxImageIG::Ghost (const CxImage *src)
{
	IGFrame::Ghost (src);
}

void CxImageIG::Ghost (const IGLibrary::IGFrame *src)
{
	IGFrame::Ghost (src);
}

void CxImageIG::CopyInfo (const CxImage &src)
{
	IGFrame::CopyInfo (src);
}

bool CxImageIG::Transfer (CxImage &from, bool bTransferFrames /*=true*/)
{
	return IGFrame::Transfer (from, bTransferFrames);
}

const char *CxImageIG::GetLastError()
{
	return IGFrame::GetLastError();
}

int CxImageIG::GetNbLayers (const wchar_t *pcwFilePath)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		return -1;
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		return -1;
	return igHeader.nNbLayers;
}

int CxImageIG::GetNbSelections (const wchar_t *pcwFilePath)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		return -1;
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		return -1;
	return igHeader.nNbSelections;
}

int CxImageIG::GetMaxLayerId (const wchar_t *pcwFilePath)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		return -1;
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		return -1;
	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER [CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections  = new IGSECTIONHEADER_SELECTION [CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, &pLayerSections[0], &pSelectionSections[0]))
		return -1;
	if (igHeader.nNbLayers <= 0)
		return -1;
	int nMaxLayerId = pLayerSections[0].commonHeader.nId;
	for (int nLayerId = 1; nLayerId < igHeader.nNbLayers; nLayerId++)
	{
		if (nMaxLayerId < pLayerSections[nLayerId].commonHeader.nId)
			nMaxLayerId = pLayerSections[nLayerId].commonHeader.nId;
	}
	return nMaxLayerId;
}

int CxImageIG::GetMaxSelectionId (const wchar_t *pcwFilePath)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		return -1;
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		return -1;
	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER [CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections  = new IGSECTIONHEADER_SELECTION [CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, &pLayerSections[0], &pSelectionSections[0]))
		return -1;
	if (igHeader.nNbSelections <= 0)
		return -1;
	int nMaxSelectionId = pSelectionSections[0].commonHeader.nId;
	for (int nSelectionId = 1; nSelectionId < igHeader.nNbSelections; nSelectionId++)
	{
		if (nMaxSelectionId < pSelectionSections[nSelectionId].commonHeader.nId)
			nMaxSelectionId = pSelectionSections[nSelectionId].commonHeader.nId;
	}
	return nMaxSelectionId;
}

bool CxImageIG::EncodeLayer (const wchar_t *pcwFilePath, int nLayerIdx, int nLayerPos, RECT *p_rcSubLayer, int nSubLayerOwnerId)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "file.Open failed");
	// read current header
	IGHEADER igHeader;	
	if (!decodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "decodeHeader failed");

	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER[CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections = new IGSECTIONHEADER_SELECTION[CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, &pLayerSections[0], &pSelectionSections[0]))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "decodeSections failed");

	IGLibrary::IGLayer *pLayer = GetLayer (nLayerPos);
	if (!pLayer)
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "GetLayer failed");
	_ASSERTE ((int)pLayer->GetId() == nLayerIdx && "CxImageIG::EncodeLayer FAILED");
	bool bIsSubLayerOwner = false;
	int nLayerSectionIndex = findLayerSectionIndex (&igHeader, pLayerSections, nLayerIdx, bIsSubLayerOwner);
	if (nLayerSectionIndex < 0)
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "findLayerSectionIndex failed");
	if ((nLayerSectionIndex > igHeader.nNbLayers) || (nLayerSectionIndex >= CXIMAGEIG_MAX_NBLAYERS))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "findLayerSectionIndex failed");
	CxImage cxSubLayer;
	CxImage *pEncodingLayer = pLayer;
	int nSubLayerWidth = 0;
	int nSubLayerHeight = 0;
	if (p_rcSubLayer)
	{
		// Encode sub-layer
		nSubLayerWidth = p_rcSubLayer->right - p_rcSubLayer->left + 1;
		nSubLayerHeight = p_rcSubLayer->bottom - p_rcSubLayer->top + 1;
		cxSubLayer.Create (nSubLayerWidth, nSubLayerHeight, 24);
		cxSubLayer.AlphaCreate (255);
		BYTE *pLayerBits = NULL;
		BYTE *pSubLayerBits = NULL;
		for (int i = 0; i < nSubLayerHeight; i++)
		{
			pSubLayerBits = cxSubLayer.GetBits (i);
			pLayerBits = pLayer->GetBits (p_rcSubLayer->top + i) + 3 * p_rcSubLayer->left;
			::memcpy (pSubLayerBits, pLayerBits, nSubLayerWidth * 3);
		}
		BYTE *pLayerAlpha = NULL;
		BYTE *pSubLayerAlpha = NULL;
		for (int i = 0; i < nSubLayerHeight; i++)
		{
			pLayerAlpha = pLayer->AlphaGetPointer (p_rcSubLayer->left, p_rcSubLayer->top + i);
			pSubLayerAlpha = cxSubLayer.AlphaGetPointer (0, i);		
			::memcpy (pSubLayerAlpha, pLayerAlpha, nSubLayerWidth);
		}
		pEncodingLayer = &cxSubLayer;
		int nSubLayerSectionIndex = findLayerSectionIndex (&igHeader, pLayerSections, nSubLayerOwnerId, bIsSubLayerOwner);
		if (nSubLayerSectionIndex < 0)
			throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "findLayerSectionIndex failed");
		pLayerSections [nSubLayerSectionIndex].pnSubLayers [pLayerSections [nSubLayerSectionIndex].nSubLayers++] = nLayerSectionIndex;
	}
	ProgressSetRange (pLayer->GetHeight(), 0);
	ProgressSetMessage (L"Encoding layer...");	
	// fill layer offset and size
	pLayerSections [nLayerSectionIndex].nSubLayers = 0;
	pLayerSections [nLayerSectionIndex].ptOffset.x = p_rcSubLayer ? p_rcSubLayer->left : pLayer->info.xOffset;
	pLayerSections [nLayerSectionIndex].ptOffset.y = p_rcSubLayer ? p_rcSubLayer->top : pLayer->info.yOffset;
	pLayerSections [nLayerSectionIndex].ptSize.x = p_rcSubLayer ? p_rcSubLayer->right - p_rcSubLayer->left + 1 : pLayer->GetWidth();
	pLayerSections [nLayerSectionIndex].ptSize.y = p_rcSubLayer ? p_rcSubLayer->bottom - p_rcSubLayer->top + 1 : pLayer->GetHeight();
	if (nLayerSectionIndex == igHeader.nNbLayers)
		pLayerSections [nLayerSectionIndex].commonHeader.nSectionId = (BYTE)igHeader.nNbSections;
	pLayerSections [nLayerSectionIndex].commonHeader.nId = nLayerIdx;
	pLayerSections [nLayerSectionIndex].commonHeader.eSectionType = IGSECTION_LAYER;
	// set byte offset
	if (igHeader.nNbSections == 0)
		pLayerSections [nLayerSectionIndex].commonHeader.nFirstByteOffset = sizeof (IGHEADER) + sizeof (IGSECTIONHEADER_LAYER) * CXIMAGEIG_MAX_NBLAYERS + sizeof (IGSECTIONHEADER_SELECTION) * CXIMAGEIG_MAX_NBSELECTIONS;
	else
		pLayerSections [nLayerSectionIndex].commonHeader.nFirstByteOffset = findSectionFirstByteOffset (&igHeader, pLayerSections, pSelectionSections, pLayerSections [nLayerSectionIndex].commonHeader.nSectionId);

	igHeader.nNbLayers = nLayerSectionIndex + 1;
	if (igHeader.nNbSelections == 0)
	{
		igHeader.nNbSections = pLayerSections [nLayerSectionIndex].commonHeader.nSectionId + 1;
		igHeader.nNbSelections = igHeader.nNbSections - igHeader.nNbLayers;
	}
	else
	{
		if (pSelectionSections [igHeader.nNbSelections - 1].commonHeader.nSectionId == pLayerSections [nLayerSectionIndex].commonHeader.nSectionId + 1)
			igHeader.nNbSections = igHeader.nNbSelections + igHeader.nNbLayers;
		else
		{
			igHeader.nNbSections = pLayerSections [nLayerSectionIndex].commonHeader.nSectionId + 1;
			igHeader.nNbSelections = igHeader.nNbSections - igHeader.nNbLayers;
		}
	}

	// write layer pixels
	if (!file.Seek (pLayerSections [nLayerSectionIndex].commonHeader.nFirstByteOffset, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "file.Seek failed");

	// set max JPEG quality
	float fCurQuality = pEncodingLayer->GetJpegQualityF();
	pEncodingLayer->SetJpegQualityF (100.0f);
	// JPEG encoding
	BYTE *pBuf = new BYTE [CXIMAGEIG_INIT_LAYERSIZE];
	CxMemFile memFile (pBuf, CXIMAGEIG_INIT_LAYERSIZE);
	if (!pEncodingLayer->Encode (&memFile, CXIMAGEIG_LAYERFORMAT))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "pEncodingLayer->Encode failed");
	BYTE *pBufImg = memFile.GetBuffer();
	long nSizeBufImg = memFile.Tell();
	// reset JPEG quality
	pEncodingLayer->SetJpegQualityF (fCurQuality);
	file.Write (pBufImg, nSizeBufImg, 1);
	delete [] pBufImg;
	pLayerSections [nLayerSectionIndex].commonHeader.nSizeBuf = nSizeBufImg;
	// write layer alpha
	if (!pEncodingLayer->pAlpha)
		pEncodingLayer->AlphaCreate(255);
	int nNbPixels = pEncodingLayer->GetWidth() * pEncodingLayer->GetHeight();
	file.Write (pEncodingLayer->pAlpha, nNbPixels, 1);
	pLayerSections [nLayerSectionIndex].commonHeader.nEndByteOffset = pLayerSections [nLayerSectionIndex].commonHeader.nFirstByteOffset + nSizeBufImg + nNbPixels;

	// write new headers
	if (!encodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "encodeHeader failed");
	if (!encodeSections (&file, &igHeader, pLayerSections, pSelectionSections))
		throw IGEXCEPTION (CxImageIGException, "EncodeLayer", "encodeSections failed");
	delete [] pLayerSections;
	delete [] pSelectionSections;
	return true;
}

bool CxImageIG::DecodeLayer (CxFile &file, int nLayerIdx, int nLayerPos, RECT *p_rcSubLayer, bool bUndo, int nSubLayerId)
{	
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "decodeHeader failed");
	if (nLayerIdx < 0)
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "nLayerIdx failed");
	if ((nLayerPos < 0) || (nLayerPos >= info.nNumLayers))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "nLayerPos failed");

	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER [CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections  = new IGSECTIONHEADER_SELECTION [CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, &pLayerSections[0], &pSelectionSections[0]))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "decodeSections failed");

	IGLibrary::IGLayer *pLayer = GetLayer (nLayerPos);
	if (!pLayer)
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "GetLayer failed");
	CxImage cxSubLayer;
	CxImage *pDecodingLayer = p_rcSubLayer ? &cxSubLayer : pLayer;
	bool bIsSubLayerOwner = bUndo && p_rcSubLayer; // no need to find sub-layer owner in do mode
	int nLayerSectionIndex = findLayerSectionIndex (&igHeader, pLayerSections, nLayerIdx, bIsSubLayerOwner);
	if ((nLayerSectionIndex < 0) || (nLayerSectionIndex >= igHeader.nNbLayers))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "findLayerSectionIndex failed");

	// in undo mode, if a sub-layer owner is found, decode it
	if (bUndo){
		if (bIsSubLayerOwner)
		{
			// decode sub-layer owner
			if (!DecodeLayer (file, pLayerSections [nLayerSectionIndex].commonHeader.nId, nLayerPos))
				throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "DecodeLayer failed");
			// decode sub-layers
			int nSubLayerSectionIndex = findLayerSectionIndex (&igHeader, pLayerSections, nLayerIdx, bIsSubLayerOwner);
			if ((nSubLayerSectionIndex < 0) || (nSubLayerSectionIndex >= igHeader.nNbLayers))
				throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "findLayerSectionIndex failed");
			RECT rcSubLayer;
			rcSubLayer.left = pLayerSections[nSubLayerSectionIndex].ptOffset.x;
			rcSubLayer.right = pLayerSections[nSubLayerSectionIndex].ptOffset.x + pLayerSections[nSubLayerSectionIndex].ptSize.x - 1;
			rcSubLayer.top = pLayerSections[nSubLayerSectionIndex].ptOffset.y;
			rcSubLayer.bottom = pLayerSections[nSubLayerSectionIndex].ptOffset.y + pLayerSections[nSubLayerSectionIndex].ptSize.y - 1;
			return DecodeLayer (file, pLayerSections [nLayerSectionIndex].commonHeader.nId, nLayerPos, &rcSubLayer, true, nLayerIdx);
		}
	}

	if (!file.Seek (pLayerSections [nLayerSectionIndex].commonHeader.nFirstByteOffset, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "file.Seek failed");
	// read layer pixels
	BYTE *pBufImg = new BYTE [pLayerSections [nLayerSectionIndex].commonHeader.nSizeBuf];
	file.Read (pBufImg, pLayerSections [nLayerSectionIndex].commonHeader.nSizeBuf, 1);
	if (!pDecodingLayer->Decode (pBufImg, pLayerSections [nLayerSectionIndex].commonHeader.nSizeBuf, CXIMAGEIG_LAYERFORMAT))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "pDecodingLayer->Decode failed");
	delete [] pBufImg;

	// read layer alpha
	pDecodingLayer->AlphaCreate (255);
	file.Read (pDecodingLayer->pAlpha, pDecodingLayer->GetWidth() * pDecodingLayer->GetHeight(), 1);
	
	int nSubLayerWidth = 0;
	int nSubLayerHeight = 0;
	if (p_rcSubLayer)
	{
		// decode sub-layer
		nSubLayerWidth = p_rcSubLayer->right - p_rcSubLayer->left + 1;
		nSubLayerHeight = p_rcSubLayer->bottom - p_rcSubLayer->top + 1;
		BYTE *pLayerBits = NULL;
		BYTE *pSubLayerBits = NULL;
		for (int i = 0; i < nSubLayerHeight; i++)
		{
			// if undo, then sub-layer == original layer
			pSubLayerBits = bUndo ? pDecodingLayer->GetBits (p_rcSubLayer->top + i) + 3 * p_rcSubLayer->left : pDecodingLayer->GetBits (i);
			pLayerBits = pLayer->GetBits (p_rcSubLayer->top + i) + 3 * p_rcSubLayer->left;
			::memcpy (pLayerBits, pSubLayerBits, nSubLayerWidth * 3);			
		}
		BYTE *pLayerAlpha = NULL;
		BYTE *pSubLayerAlpha = NULL;
		for (int i = 0; i < nSubLayerHeight; i++)
		{
			pLayerAlpha = pLayer->AlphaGetPointer (p_rcSubLayer->left, p_rcSubLayer->top + i);
			pSubLayerAlpha = bUndo ? pDecodingLayer->AlphaGetPointer (p_rcSubLayer->left, p_rcSubLayer->top + i) : pDecodingLayer->AlphaGetPointer (0, i);
			::memcpy (pLayerAlpha, pSubLayerAlpha, nSubLayerWidth);
		}
		if (bUndo)
		{
			// original layer is now decoded. decode sub-layers
			bIsSubLayerOwner = false;
			int nMaxSubLayerSectionIndex = findLayerSectionIndex (&igHeader, pLayerSections, nSubLayerId, bIsSubLayerOwner); // we always have bIsSubLayerOwner = false, its used for the reference only
			if (nMaxSubLayerSectionIndex < 0)
				throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "findLayerSectionIndex failed");
			if (nMaxSubLayerSectionIndex <= igHeader.nNbLayers)
			{
				for (int i = 0; i < pLayerSections [nLayerSectionIndex].nSubLayers; i++)
				{
					if (pLayerSections [nLayerSectionIndex].pnSubLayers [i] <= nMaxSubLayerSectionIndex)
					{
						if (!decodeSubLayer (file, pLayer, &pLayerSections [pLayerSections [nLayerSectionIndex].pnSubLayers [i]]))
							throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "decodeSubLayer failed");
					}
				}
			}
		}
	}
	else
	{
		pLayer->info.xOffset = pLayerSections [nLayerSectionIndex].ptOffset.x;
		pLayer->info.yOffset = pLayerSections [nLayerSectionIndex].ptOffset.y;
	}
	pLayer->SetId (nLayerIdx);
	pLayer->UpdateImageParameters();
	delete [] pLayerSections;
	delete [] pSelectionSections;
	return true;
}

bool CxImageIG::DecodeLayer (const wchar_t *pcwFilePath, int nLayerIdx, int nLayerPos, RECT *p_rcSubLayer, bool bUndo, int nSubLayerId)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"rb"))
		throw IGEXCEPTION (CxImageIGException, "DecodeLayer", "file.Open failed");
	return DecodeLayer (file, nLayerIdx, nLayerPos, p_rcSubLayer, bUndo, nSubLayerId);
}

bool CxImageIG::DecodeSelection (const wchar_t *pcwFilePath, int nSelectionIdx, int nLayerPos, bool bUndo)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"rb"))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "file.Open failed");
	return DecodeSelection (file, nSelectionIdx, nLayerPos, bUndo);
}

bool CxImageIG::decodeSubLayer (CxFile& file, IGLibrary::IGLayer *pSubLayerOwner, IGSECTIONHEADER_LAYER *pLayerSection)
{
	CxImage cxSubLayer;
	if (!file.Seek (pLayerSection->commonHeader.nFirstByteOffset, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "decodeSubLayer", "file.Seek failed");
	// read sub-layer pixels
	BYTE *pBufImg = new BYTE [pLayerSection->commonHeader.nSizeBuf];
	file.Read (pBufImg, pLayerSection->commonHeader.nSizeBuf, 1);
	if (!cxSubLayer.Decode (pBufImg, pLayerSection->commonHeader.nSizeBuf, CXIMAGEIG_LAYERFORMAT))
		throw IGEXCEPTION (CxImageIGException, "decodeSubLayer", "cxSubLayer.Decode failed");
	delete [] pBufImg;
	// read sub-layer alpha
	cxSubLayer.AlphaCreate (255);
	file.Read (cxSubLayer.pAlpha, cxSubLayer.GetWidth() * cxSubLayer.GetHeight(), 1);
	// decode sub-layer into layer
	BYTE *pLayerBits = NULL;
	BYTE *pSubLayerBits = NULL;
	for (int i = 0; i < pLayerSection->ptSize.y; i++)
	{
		pSubLayerBits = cxSubLayer.GetBits (i);
		pLayerBits = pSubLayerOwner->GetBits (pLayerSection->ptOffset.y + i) + 3 * pLayerSection->ptOffset.x;
		::memcpy (pLayerBits, pSubLayerBits, (pLayerSection->ptSize.x) * 3);			
	}
	BYTE *pLayerAlpha = NULL;
	BYTE *pSubLayerAlpha = NULL;
	for (int i = 0; i < (pLayerSection->ptSize.y); i++)
	{
		pLayerAlpha = pSubLayerOwner->AlphaGetPointer (pLayerSection->ptOffset.x, pLayerSection->ptOffset.y + i);
		pSubLayerAlpha = cxSubLayer.AlphaGetPointer (0, i);	
		::memcpy (pLayerAlpha, pSubLayerAlpha, (pLayerSection->ptSize.x));
	}
	return true;
}

bool CxImageIG::EncodeSelection (const wchar_t *pcwFilePath, int nSelectionIdx, int nLayerPos, const RECT& rcSelection)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "file.Open failed");

	if (((rcSelection.left == -1) && (rcSelection.top != -1)) ||
		((rcSelection.right != -1) && (rcSelection.bottom == -1)))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "rcSelection failed");
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "decodeHeader failed");
	if ((nLayerPos < 0) || (nLayerPos >= info.nNumLayers))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "nLayerPos failed");

	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER [CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections  = new IGSECTIONHEADER_SELECTION [CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, &pLayerSections[0], &pSelectionSections[0]))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "decodeSections failed");

	IGLibrary::IGLayer *pLayer = GetLayer (nLayerPos);
	if (!pLayer)
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "GetLayer failed");
	int nSelectionSectionIndex = findSelectionSectionIndex (&igHeader, pSelectionSections, nSelectionIdx);
	if (nSelectionSectionIndex < 0)
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "findSelectionSectionIndex failed");
	if ((nSelectionSectionIndex > igHeader.nNbSelections) || (nSelectionSectionIndex >= CXIMAGEIG_MAX_NBSELECTIONS)
		|| (pLayer->GetId() < 0))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "findSelectionSectionIndex failed");

	// check rect bounds
	if (rcSelection.right != -1)	// right = -1 is a special case (full selection)
	{
		if ((rcSelection.left < 0) || (rcSelection.top < 0) ||
				(rcSelection.left >= pLayer->GetWidth()) || (rcSelection.top >= pLayer->GetHeight()) ||
				(rcSelection.right >= pLayer->GetWidth()) || (rcSelection.bottom >= pLayer->GetHeight()) ||
				(rcSelection.right <= rcSelection.left) || (rcSelection.bottom <= rcSelection.top))
				throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "rcSelection failed");
	}	
	// fill layer offset and size
	pSelectionSections [nSelectionSectionIndex].rcSelection.left = (rcSelection.right == -1) ? 0 : rcSelection.left;
	pSelectionSections [nSelectionSectionIndex].rcSelection.bottom = (rcSelection.right == -1) ? 0 : pLayer->GetHeight() - 1 - rcSelection.bottom;
	pSelectionSections [nSelectionSectionIndex].rcSelection.right = (rcSelection.right == -1) ? pLayer->GetWidth() - 1 : rcSelection.right;
	pSelectionSections [nSelectionSectionIndex].rcSelection.top = (rcSelection.right == -1) ? pLayer->GetHeight()  - 1 : pLayer->GetHeight() - 1 - rcSelection.top;
	if (nSelectionSectionIndex == igHeader.nNbSelections)
		pSelectionSections [nSelectionSectionIndex].commonHeader.nSectionId = (BYTE)igHeader.nNbSections;
	pSelectionSections [nSelectionSectionIndex].commonHeader.nId = nSelectionIdx;
	pSelectionSections [nSelectionSectionIndex].nLayerId = (int)pLayer->GetId();
	pSelectionSections [nSelectionSectionIndex].commonHeader.eSectionType = IGSECTION_SELECTION;
	// no need to set byte offset if it is not the last layer
	if (igHeader.nNbSections == 0)
		pSelectionSections [nSelectionSectionIndex].commonHeader.nFirstByteOffset = sizeof (IGHEADER) + sizeof (IGSECTIONHEADER_LAYER) * CXIMAGEIG_MAX_NBLAYERS + sizeof (IGSECTIONHEADER_SELECTION) * CXIMAGEIG_MAX_NBSELECTIONS;
	else
		pSelectionSections [nSelectionSectionIndex].commonHeader.nFirstByteOffset = findSectionFirstByteOffset (&igHeader, pLayerSections, pSelectionSections, pSelectionSections [nSelectionSectionIndex].commonHeader.nSectionId);

	if (nSelectionSectionIndex == igHeader.nNbSelections)
		igHeader.nNbSections++;
	else
		igHeader.nNbSections = pSelectionSections [nSelectionSectionIndex].commonHeader.nSectionId + 1;

	igHeader.nNbSelections = nSelectionSectionIndex + 1;
	igHeader.nNbLayers = igHeader.nNbSections - igHeader.nNbSelections;

	if (!writeSelection (file, pSelectionSections [nSelectionSectionIndex], *pLayer, rcSelection.right == -1))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "writeSelection failed");
	
	// write new headers
	if (!encodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "encodeHeader failed");
	if (!encodeSections (&file, &igHeader, pLayerSections, pSelectionSections))
		throw IGEXCEPTION (CxImageIGException, "EncodeSelection", "encodeSections failed");

	delete [] pLayerSections;
	delete [] pSelectionSections;
	return true;
}

bool CxImageIG::DecodeSelection (CxFile& file, int nSelectionIdx, int nLayerPos, bool bUndo)
{
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "decodeHeader failed");
	if ((nLayerPos < 0) || (nLayerPos >= info.nNumLayers))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "nLayerPos failed");

	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER[CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections = new IGSECTIONHEADER_SELECTION[CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, pLayerSections, pSelectionSections))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "decodeSections failed");

	int nSelectionSectionIndex = findSelectionSectionIndex (&igHeader, pSelectionSections, nSelectionIdx);
	if ((nSelectionSectionIndex < 0) || (nSelectionSectionIndex >= igHeader.nNbSelections))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "findSelectionSectionIndex failed");
	if ((nSelectionSectionIndex > igHeader.nNbSelections) || (nSelectionSectionIndex >= CXIMAGEIG_MAX_NBSELECTIONS))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "findSelectionSectionIndex failed");
	if (bUndo)
	{
		bool bIsSubLayerOwner = false;
		int nLayerSectionIndex = findLayerSectionIndex (&igHeader, pLayerSections, pSelectionSections [nSelectionSectionIndex].nLayerId, bIsSubLayerOwner);
		if (nLayerSectionIndex < 0)
			throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "findLayerSectionIndex failed");
		if ((nLayerSectionIndex > igHeader.nNbLayers) || (nLayerSectionIndex >= CXIMAGEIG_MAX_NBLAYERS))
			throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "findLayerSectionIndex failed");
	}
	// read selection data
	if (!file.Seek (pSelectionSections [nSelectionSectionIndex].commonHeader.nFirstByteOffset, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "file.Seek failed");
	// uncompress data
	BYTE *pSelectionZipped = new BYTE [pSelectionSections [nSelectionSectionIndex].commonHeader.nSizeBuf];
	file.Read (pSelectionZipped, pSelectionSections [nSelectionSectionIndex].commonHeader.nSizeBuf, 1);
	std::ostrstream strOstream;
	int nSelectionUnzippedSize = 0;
	if (IGPacker::gzipUncompress (pSelectionZipped, pSelectionSections [nSelectionSectionIndex].commonHeader.nSizeBuf, strOstream, nSelectionUnzippedSize) != Z_OK)
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "gzipUncompress failed");
	delete [] pSelectionZipped;

	IGLibrary::IGLayer *pLayer = GetLayerFromId (pSelectionSections [nSelectionSectionIndex].nLayerId);
	if (!pLayer)
		throw IGEXCEPTION (CxImageIGException, "DecodeSelection", "GetLayerFromId failed");

	// copy selection to dst selection rect
	BYTE *pFromSelection = (BYTE *)strOstream.str();
	BYTE *pDstSelection = pLayer->SelectionGetPointer(pSelectionSections [nSelectionSectionIndex].rcSelection.left,
														pSelectionSections [nSelectionSectionIndex].rcSelection.bottom);
	if (!pDstSelection)
	{
		pLayer->SelectionCreate();
		pDstSelection = pLayer->SelectionGetPointer (pSelectionSections [nSelectionSectionIndex].rcSelection.left,
														pSelectionSections [nSelectionSectionIndex].rcSelection.bottom);
	}
	int nLayerWidth = pLayer->GetWidth();
	int nLayerHeight = pLayer->GetHeight();
	if (((pSelectionSections [nSelectionSectionIndex].rcSelection.right - pSelectionSections [nSelectionSectionIndex].rcSelection.left + 1) == nLayerWidth) &&
		((pSelectionSections [nSelectionSectionIndex].rcSelection.top - pSelectionSections [nSelectionSectionIndex].rcSelection.bottom + 1) == nLayerHeight))
	{
		::memcpy (pDstSelection, pFromSelection, nLayerWidth * nLayerHeight);		
	}
	else
	{
		for (int i = 0; i < pSelectionSections [nSelectionSectionIndex].rcSelection.top - pSelectionSections [nSelectionSectionIndex].rcSelection.bottom; i++)
		{
			::memcpy (pDstSelection, pFromSelection, pSelectionSections [nSelectionSectionIndex].rcSelection.right - pSelectionSections [nSelectionSectionIndex].rcSelection.left + 1);
			pFromSelection += pSelectionSections [nSelectionSectionIndex].rcSelection.right - pSelectionSections [nSelectionSectionIndex].rcSelection.left + 1;
			pDstSelection += nLayerWidth;
		}
	}
	delete [] pLayerSections;
	delete [] pSelectionSections;
	return true;
}

int CxImageIG::findSectionFirstByteOffset (const IGHEADER *pHeader, const IGSECTIONHEADER_LAYER *pLayerSections, const IGSECTIONHEADER_SELECTION *pSelectionSections, int nSectionId)
{
	for (int i = 0; i < pHeader->nNbLayers; i++)
	{
		if (pLayerSections [i].commonHeader.nSectionId == nSectionId - 1)
			return pLayerSections [i].commonHeader.nEndByteOffset;
	}
	for (int i = 0; i < pHeader->nNbSelections; i++)
	{
		if (pSelectionSections [i].commonHeader.nSectionId == nSectionId - 1)
			return pSelectionSections [i].commonHeader.nEndByteOffset;
	}
	return -1;
}

int CxImageIG::findLayerSectionIndex (const IGHEADER *pHeader, const IGSECTIONHEADER_LAYER *pLayerSections, int nLayerId, bool& bIsSubLayerOwner)
{
	if (nLayerId < 0)
		return -1;
	for (int idxLayer = 0; idxLayer < pHeader->nNbLayers; idxLayer++)
	{
		if (pLayerSections [idxLayer].commonHeader.nId == nLayerId)
		{
			if (bIsSubLayerOwner)
			{
				bIsSubLayerOwner = false;
				// look for a sub-layer owner
				for (int idxSubLayerOwner = 0; idxSubLayerOwner < pHeader->nNbLayers; idxSubLayerOwner++)
				{
					for (int idxSubLayer = 0; idxSubLayer < pLayerSections [idxSubLayerOwner].nSubLayers; idxSubLayer++)
					{
						if (pLayerSections [idxSubLayerOwner].pnSubLayers [idxSubLayer] == idxLayer)
						{
							bIsSubLayerOwner = true;
							return idxSubLayerOwner;
						}
					}
				}
			}
			return idxLayer;
		}
	}
	if (pHeader->nNbLayers >= CXIMAGEIG_MAX_NBLAYERS)
		return -1;
	return pHeader->nNbLayers;
}

int CxImageIG::findSelectionSectionIndex (const IGHEADER *pHeader, const IGSECTIONHEADER_SELECTION *pSelectionSections, int nSelectionId)
{
	if (nSelectionId < 0)
		return -1;
	for (int i = 0; i < pHeader->nNbSelections; i++)
	{
		if (pSelectionSections [i].commonHeader.nId == nSelectionId)
			return i;
	}
	if (pHeader->nNbSelections >= CXIMAGEIG_MAX_NBSELECTIONS)
		return -1;
	return pHeader->nNbSelections;
}

bool CxImageIG::RemoveSections (const wchar_t *pcwFilePath, int nLayerId, int nSelectionId)
{
	CxIOFile file;
	if (!file.Open (pcwFilePath, L"r+b"))
		throw IGEXCEPTION (CxImageIGException, "RemoveSections", "file.Open failed");
	// read current header
	IGHEADER igHeader;
	if (!decodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "RemoveSections", "decodeHeader failed");
	IGSECTIONHEADER_LAYER *pLayerSections = new IGSECTIONHEADER_LAYER[CXIMAGEIG_MAX_NBLAYERS];
	IGSECTIONHEADER_SELECTION *pSelectionSections = new IGSECTIONHEADER_SELECTION[CXIMAGEIG_MAX_NBSELECTIONS];
	if (!decodeSections (&file, &igHeader, pLayerSections, pSelectionSections))
		throw IGEXCEPTION (CxImageIGException, "RemoveSections", "decodeSections failed");
	// remove layers
	bool bFound = false;
	int nLayerSectionId = 0;
	int nSelectionSectionId = 0;
	if (nLayerId >= 0 && igHeader.nNbLayers > 0)
	{
		int nSubLayerId = 0;
		while (nLayerSectionId < igHeader.nNbLayers)
		{
			// update the number of sublayer sections if the removed layer is a sublayer
			nSubLayerId = 0;
			bool bSubLayerFound = false;
			while (nSubLayerId < pLayerSections [nLayerSectionId].nSubLayers)
			{
				if (pLayerSections [nLayerSectionId].pnSubLayers [nSubLayerId] == nLayerId)
				{
					bSubLayerFound = true;
					break;
				}
				nSubLayerId++;
			}
			if (bSubLayerFound)
				pLayerSections [nLayerSectionId].nSubLayers = nSubLayerId + 1;
			if (pLayerSections [nLayerSectionId].commonHeader.nId == nLayerId)
			{
				bFound = true;
				break;
			}
			nLayerSectionId++;
		}
		if (bFound)
		{
			igHeader.nNbLayers = nLayerSectionId + 1;
			// remove selections if necessary
			int nNbRemovedSelections = 0;
			nSelectionSectionId = -1;
			while (++nSelectionSectionId < igHeader.nNbSelections)
			{
				if (pSelectionSections [nSelectionSectionId].commonHeader.nSectionId > pLayerSections [nLayerSectionId].commonHeader.nSectionId)
				{
					if (pSelectionSections [nSelectionSectionId].commonHeader.nId > nSelectionId && nSelectionId >= 0)
					{
						nNbRemovedSelections = igHeader.nNbSelections - nSelectionSectionId;
						break;
					}
				}
			}
			igHeader.nNbSelections -= nNbRemovedSelections;
		}
	}
	// remove selections
	bFound = false;
	if (igHeader.nNbSelections > 0) 
	{
		if (nSelectionId >= 0)
		{
			nSelectionSectionId = 0;
			while (nSelectionSectionId < igHeader.nNbSelections)
			{
				if (pSelectionSections [nSelectionSectionId].commonHeader.nId == nSelectionId)
				{
					bFound = true;
					break;
				}
				nSelectionSectionId++;
			}
			if (bFound)
			{
				igHeader.nNbSelections = nSelectionSectionId + 1;
				// remove layers if necessary
				int nNbRemovedLayers = 0;
				nLayerSectionId = -1;
				while (++nLayerSectionId < igHeader.nNbLayers)
				{
					if (pLayerSections [nLayerSectionId].commonHeader.nSectionId > pSelectionSections [nSelectionSectionId].commonHeader.nSectionId)
					{
						if (pLayerSections [nLayerSectionId].commonHeader.nId > nLayerId)
						{
							nNbRemovedLayers = igHeader.nNbLayers - nLayerSectionId;
							break;
						}
					}
				}
				igHeader.nNbLayers -= nNbRemovedLayers;
			}
		}
		else
			igHeader.nNbSelections = 0;
	}
	igHeader.nNbSections = igHeader.nNbSelections + igHeader.nNbLayers;
	// write new headers
	if (!encodeHeader (&file, &igHeader))
		throw IGEXCEPTION (CxImageIGException, "RemoveSections", "encodeHeader failed");
	if (!encodeSections (&file, &igHeader, pLayerSections, pSelectionSections))
		throw IGEXCEPTION (CxImageIGException, "RemoveSections", "encodeSections failed");
	delete [] pLayerSections;
	delete [] pSelectionSections;
	return true;
}

bool CxImageIG::writeSelection (CxFile& file, IGSECTIONHEADER_SELECTION& selectionSection, CxImage& layer, bool bOneMemoryBlock)
{
	// write selection data
	if (!file.Seek (selectionSection.commonHeader.nFirstByteOffset, SEEK_SET))
		throw IGEXCEPTION (CxImageIGException, "writeSelection", "file.Seek failed");
	// extracting rect selection
	BYTE *pFromSelection = layer.SelectionGetPointer(selectionSection.rcSelection.left,
													selectionSection.rcSelection.bottom);
	BYTE *pSelection = new BYTE [(selectionSection.rcSelection.right - selectionSection.rcSelection.left + 1) *
		(selectionSection.rcSelection.top - selectionSection.rcSelection.bottom + 1)];
	if (!pFromSelection)
	{
		::ZeroMemory (pSelection, (selectionSection.rcSelection.right - selectionSection.rcSelection.left + 1) *
			(selectionSection.rcSelection.top - selectionSection.rcSelection.bottom + 1));
	}	
	else
	{
		BYTE *pDstSelection = pSelection;
		if (bOneMemoryBlock)
		{
			::memcpy (pDstSelection, pFromSelection, (selectionSection.rcSelection.right - selectionSection.rcSelection.left + 1) *
										selectionSection.rcSelection.top - selectionSection.rcSelection.bottom + 1);		
		}
		else
		{
			for (int i = 0; i < selectionSection.rcSelection.top - selectionSection.rcSelection.bottom + 1; i++)
			{
				::memcpy (pDstSelection, pFromSelection, selectionSection.rcSelection.right - selectionSection.rcSelection.left + 1);
				pFromSelection += layer.GetWidth();
				pDstSelection += selectionSection.rcSelection.right - selectionSection.rcSelection.left + 1;		
			}
		}
	}
	// compressing data
	BYTE *pSelectionZipped = NULL;
	unsigned long ulSelectionZippedSize = IGPacker::gzipCompress (pSelection, (selectionSection.rcSelection.right - selectionSection.rcSelection.left + 1) *
																	(selectionSection.rcSelection.top - selectionSection.rcSelection.bottom + 1), &pSelectionZipped);
	delete [] pSelection;
	file.Write (pSelectionZipped, ulSelectionZippedSize, 1);
	delete [] pSelectionZipped;

	selectionSection.commonHeader.nSizeBuf = ulSelectionZippedSize;
	selectionSection.commonHeader.nEndByteOffset = selectionSection.commonHeader.nFirstByteOffset + ulSelectionZippedSize;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_IG
