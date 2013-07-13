/*
 * File:	ximaig.h
 * Purpose:	Imagenius Picture file Loader and Writer
 */
#pragma once

#if CXIMAGE_SUPPORT_IG

#define IGHEADER_MAGICKEY	0xBEBE

#define CXIMAGEIG_MAX_NBLAYERS		50
#define CXIMAGEIG_MAX_NBSELECTIONS	50
#define CXIMAGEIG_MAX_NBSELPOINTS	512

class CxImageIG: public IGLibrary::IGFrame
{
typedef enum {IGSECTION_LAYER, IGSECTION_SELECTION} IGSECTIONTYPE;

typedef struct tagIGSECTIONHEADER_COMMON
{
	IGSECTIONTYPE eSectionType;		// either a layer or a selection
	BYTE  nSectionId;		// IG file section index (0 to CXIMAGEIG_MAX_NBSECTIONS - 1)
	BYTE  nId;				// For layers : Image history layer id 
							// For selection : Image history selection id 
	LONG  nFirstByteOffset;	// Number of bytes from the file beginning to data start
	LONG  nEndByteOffset;	// Number of bytes from the file beginning to data end
	LONG  nSizeBuf;			// For layers : size of the JPEG buffer
							// For selection : size of the selection point buffer
} IGSECTIONHEADER_COMMON;

typedef struct tagIGSECTIONHEADER_LAYER
{
	IGSECTIONHEADER_COMMON commonHeader;
	POINT ptOffset;			// Layer origin coordinates from parent layer origin
	POINT ptSize;			// Layer size
	BYTE  nSubLayers;		// number of sub-layers
	BYTE  pnSubLayers[CXIMAGEIG_MAX_NBLAYERS - 1]; // sub-layer indices	
} IGSECTIONHEADER_LAYER;

typedef struct tagIGSECTIONHEADER_SELECTION
{
	IGSECTIONHEADER_COMMON commonHeader;
	BYTE  nLayerId;	  // Associated layer id
	RECT rcSelection; // the rect of the selection to be fully copied
} IGSECTIONHEADER_SELECTION;

typedef struct tagIGHEADER
{
	DWORD 			Manufacturer;	// always 0xBEBE
	char 			Version;		// version number
	char 			Encoding;		// always 1
	LONG			nWidth;	
	LONG			nHeight;
	LONG 			nNbSections;
	LONG 			nNbLayers;
	LONG 			nNbSelections;
	BYTE			nPosWorkingLayer;
} IGHEADER;
#pragma pack()

public:
	CxImageIG(): IGFrame(CXIMAGE_FORMAT_IG) {}

#if CXIMAGE_SUPPORT_DECODE
	bool Decode(CxFile * hFile);
	bool Decode(FILE *hFile) { CxIOFile file(hFile); return Decode(&file); }

	// history management
	bool DecodeLayer (CxFile& file, int nLayerIdx, int nLayerPos, RECT *p_rcSubLayer = NULL, bool bUndo = false, int nSubLayerId = -1);
	bool DecodeSelection (CxFile& file, int nSelectionIdx, int nLayerPos, bool bUndo = false);
	bool DecodeLayer (const wchar_t *pcwFilePath, int nLayerIdx, int nLayerPos, RECT *p_rcSubLayer = NULL, bool bUndo = false, int nSubLayerId = -1);
	bool DecodeSelection (const wchar_t *pcwFilePath, int nSelectionIdx, int nLayerPos, bool bUndo = false);
#endif // CXIMAGE_SUPPORT_DECODE

#if CXIMAGE_SUPPORT_ENCODE
	bool Encode(CxFile * hFile);
	bool Encode(FILE *hFile) { CxIOFile file(hFile); return Encode(&file); }

	// history management
	bool EncodeLayer (const wchar_t *pcwFilePath, int nLayerId, int nLayerPos, RECT *p_rcSubLayer = NULL, int nSubLayerOwnerId = -1);
	bool EncodeSelection (const wchar_t *pcwFilePath, int nSelectionIdx, int nLayerPos, const RECT& rcSelection);
#endif // CXIMAGE_SUPPORT_ENCODE
	
	static int GetNbLayers (const wchar_t *pcwFilePath);
	static int GetNbSelections (const wchar_t *pcwFilePath);
	static int GetMaxLayerId (const wchar_t *pcwFilePath);
	static int GetMaxSelectionId (const wchar_t *pcwFilePath);
	static bool RemoveSections (const wchar_t *pcwFilePath, int nLayerId, int nSelectionId);

	void Ghost(const CxImage *src);
	void Ghost (const IGLibrary::IGFrame *src);
	void CopyInfo(const CxImage &src);
	bool Transfer(CxImage &from, bool bTransferFrames /*=true*/);
	const char *GetLastError();
	
private:
	// history management
#if CXIMAGE_SUPPORT_ENCODE	
	static bool encodeHeader(CxFile *hFile, const IGHEADER *pHeader);
	static bool encodeSections (CxFile *hFile, const IGHEADER *pHeader, const IGSECTIONHEADER_LAYER *pLayerSections, const IGSECTIONHEADER_SELECTION *pSelectionSections);
#endif // CXIMAGE_SUPPORT_ENCODE

#if CXIMAGE_SUPPORT_DECODE
	static bool decodeHeader(CxFile *hFile, IGHEADER *pHeader);
	static bool decodeSections (CxFile *hFile, const IGHEADER *pHeader, IGSECTIONHEADER_LAYER *pLayerSections, IGSECTIONHEADER_SELECTION *pSelectionSections);
#endif // CXIMAGE_SUPPORT_DECODE

	int findSectionFirstByteOffset (const IGHEADER *pHeader, const IGSECTIONHEADER_LAYER *pLayerSections, const IGSECTIONHEADER_SELECTION *pSelectionSections, int nSectionId);
	int findLayerSectionIndex (const IGHEADER *pHeader, const IGSECTIONHEADER_LAYER *pLayerSections, int nLayerId, bool& bIsSubLayerOwner);
	int findSelectionSectionIndex (const IGHEADER *pHeader, const IGSECTIONHEADER_SELECTION *pSelectionSections, int nSelectionId);

	bool decodeSubLayer (CxFile& file, IGLibrary::IGLayer *pSubLayerOwner, IGSECTIONHEADER_LAYER *pLayerSection);
	bool writeSelection (CxFile& file, IGSECTIONHEADER_SELECTION& selectionSection, CxImage& layer, bool bOneMemoryBlock = false);
};

#endif
