#include "StdAfx.h"
#include "IGFrame.h"
#include "IGLayer.h"
#include "IGConfigManager.h"
#include "IGFrameAction.h"
#include "IGFrameHistory.h"
#include "IGScrollBar.h"
#include "IGConvertible.h"
#include "IGException.h"
#include "ximaig.h"
#include <WindowsX.h>
#include <shlobj.h>
#include <gdiplus.h>

#define IGFRAMEHISTORY_BASETEMPFILENAME		L"Tmp"
#define IGFRAMEHISTORY_NODESTEPS			L"Steps"
#define IGFRAMEHISTORY_NODESTEP				L"Step"
#define IGFRAMEHISTORY_ATTRIBUTESTEPID		L"Num"
#define IGFRAMEHISTORY_ATTRIBUTESTEPNAME	L"Name"
#define IGFRAMEHISTORY_COLOR_ITEM				Color(255, 191, 193, 195)	// Medium Gray
#define IGFRAMEHISTORY_COLOR_SELECTEDITEM		Color(100, 52, 148, 214)	// Blue
#define IGFRAMEHISTORY_COLOR_SELECTEDITEMBORDER	Color(255, 52, 148, 214)	// Blue
#define IGFRAMEHISTORY_COLOR_OVERSELECTEDITEM		Color(100, 214, 52, 52)	// Red
#define IGFRAMEHISTORY_COLOR_OVERSELECTEDITEMBORDER	Color(255, 214, 52, 52)	// Red
#define IGFRAMEHISTORY_COLOR_FONT				Color(255, 34, 98, 141)
#define IGFRAMEHISTORY_COLOR_ITEMBORDER			Color(255, 76, 85, 118)		// Black
#define IGFRAMEHISTORY_OFFSET_STEPNAME		25
#define IGFRAMESTEP_ICONX					3
#define IGFRAMESTEP_ICONY					2
#define IGFRAMESTEP_ICONSIZE				12

using namespace IGLibrary;
using namespace std;
using namespace Gdiplus;

IGException FrameHistoryException ("IGFrameHistory");
IGException FrameStepException ("IGFrameStep");

IGFrameStep::IGFrameStep (CComPtr <IXMLDOMDocument> spXMLDoc)
{
}

IGFrameStep::IGFrameStep (IGFrame *pFrame,
						  CComPtr <IXMLDOMDocument> spXMLDoc,
						  CComPtr <IXMLDOMNode> spNodeParent)
						: m_eType (IGFRAMEHISTORY_STEP_UNDEFINED)
						, m_nFrameStepOrder (-1)		
						, m_spNodeParent (spNodeParent)
						, m_spXMLDoc (spXMLDoc)
						, m_pFrame (pFrame)
{
	_ASSERTE (pFrame && L"History error: Null frame");
	if (pFrame && spNodeParent)
	{
		// Step node
		CComPtr <IXMLDOMNode> spNodeNewStep;
		HRESULT hr = m_spXMLDoc->createNode (CComVariant (NODE_ELEMENT), 
												CComBSTR (IGFRAMEHISTORY_NODESTEP), 
												NULL,
												&spNodeNewStep);
		_ASSERTE ((hr == S_OK) && spNodeNewStep && L"History msxml error");		
		hr = spNodeParent->appendChild (spNodeNewStep, &m_spNodeStep);
		_ASSERTE ((hr == S_OK) && m_spNodeStep && L"History msxml error");
		if (hr != S_OK || !m_spNodeStep)
			throw IGEXCEPTION (FrameStepException, "IGFrameStep", "History msxml error");
	}
}

IGFrameStep::~IGFrameStep()
{
}

bool IGFrameStep::Initialize (EnumStepType eType,
								int nFrameStepOrder,
								std::wstring wsStepName,
								int nResIconId,
								int nParam1, int nParam2)
{
	if (!m_pFrame || !m_spXMLDoc || !m_spNodeParent || (eType <= IGFRAMEHISTORY_STEP_UNDEFINED))
		throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
	m_eType = eType;
	m_nFrameStepOrder = nFrameStepOrder;
	m_wsStepName = wsStepName;
	// Num order attribute
	CComPtr <IXMLDOMNode> spAttributeId;
	HRESULT hr = m_spXMLDoc->createNode (CComVariant (NODE_ATTRIBUTE), 
											CComBSTR (IGFRAMEHISTORY_ATTRIBUTESTEPID), 
											NULL,
											&spAttributeId);
	_ASSERTE ((hr == S_OK) && spAttributeId && L"History msxml error");
	if (hr != S_OK)
		throw IGEXCEPTION (FrameStepException, "Initialize", "History msxml error");
	CComPtr <IXMLDOMNamedNodeMap> spNodeNewStepAttributes;
	hr = m_spNodeStep->get_attributes (&spNodeNewStepAttributes);
	_ASSERTE ((hr == S_OK) && spNodeNewStepAttributes && L"History msxml error");
	CComPtr <IXMLDOMNode> spAttributeIdAdded;
	hr = spNodeNewStepAttributes->setNamedItem (spAttributeId, &spAttributeIdAdded);
	_ASSERTE ((hr == S_OK) && spAttributeIdAdded && L"History msxml error");
	spAttributeIdAdded->put_nodeValue (CComVariant (nFrameStepOrder));

	// Name attribute
	CComPtr <IXMLDOMNode> spAttributeName;
	hr = m_spXMLDoc->createNode (CComVariant (NODE_ATTRIBUTE), 
											CComBSTR (IGFRAMEHISTORY_ATTRIBUTESTEPNAME), 
											NULL,
											&spAttributeName);
	_ASSERTE ((hr == S_OK) && spAttributeName && L"History msxml error");
	if (hr != S_OK)
		return false;
	CComPtr <IXMLDOMNode> spAttributeNameAdded;
	hr = spNodeNewStepAttributes->setNamedItem (spAttributeName, &spAttributeNameAdded);
	_ASSERTE ((hr == S_OK) && spAttributeNameAdded && L"History msxml error");
	spAttributeNameAdded->put_nodeValue (CComVariant (wsStepName.c_str()));

	CxImage *pLayer = m_pFrame->GetWorkingLayer();
	if (!pLayer)
		throw IGEXCEPTION (FrameStepException, "Initialize", "No working layer");
	POINT ptDef = {0,0};
	RECT rcDef = {0,0,0,0};
	switch (eType)
	{
	case IGFRAMEHISTORY_STEP_START:			
		m_spAction.reset (new IGFrameActionStart (m_spXMLDoc, m_spNodeStep, m_pFrame));
		if (nParam1 > (int)IGFRAMEHISTORY_STEP_START)
		{
			// history swap recursive case (start new history)
			IGFrameStep realStep (m_pFrame, m_spXMLDoc, m_spNodeParent);
			realStep.Initialize ((EnumStepType)nParam1, 0, wsStepName, -1, 1, 1);
			nResIconId = realStep.GetResIconId();
		}
		break;

	case IGFRAMEHISTORY_STEP_SELECTION:
		{
			SELECTIONPARAMS *pSelectionParams = (SELECTIONPARAMS *)nParam1;
			SELECTIONPARAMS selDef;
			if ((int)pSelectionParams == 1L) // recursive case (swap history)
				pSelectionParams = &selDef;
			if (!pSelectionParams)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			std::list<POINT> *plptSel = (std::list<POINT> *)nParam2;
			std::list<POINT> ptSelDef;
			if ((int)plptSel == 1L) // recursive case (swap history)
				plptSel = &ptSelDef;
			RECT rcSel = IGConvertible::FromListPtsToRECT (*plptSel, pLayer->GetWidth(), pLayer->GetHeight());
			m_spAction.reset (new IGFrameActionChangeSelection (m_spXMLDoc, m_spNodeStep, m_pFrame, rcSel, pSelectionParams->eSelectionType));
		}
		break;

	case IGFRAMEHISTORY_STEP_RESIZE:
		{
			POINT *ptSize = (POINT*)nParam1;
			if ((int)ptSize == 1L) // recursive case (swap history)
				ptSize = &ptDef;
			if (!ptSize)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionChangeSize (m_spXMLDoc, m_spNodeStep, m_pFrame, 
				*ptSize, 
				nParam2 == 1));
			break;
		}

	case IGFRAMEHISTORY_STEP_LAYERCHANGE:
		{
			int nLayerPos = nParam1;
			RECT *p_rcSubLayerPos = (RECT *)nParam2;
			if ((int)p_rcSubLayerPos == 1L) // recursive case (swap history)
				p_rcSubLayerPos = &rcDef;
			if (nLayerPos < 0)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionChangeLayer (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPos, -1, -1, p_rcSubLayerPos));
		}
		break;

	case IGFRAMEHISTORY_STEP_LAYERADD:
		{
			int nLayerPos = nParam1;
			if (nLayerPos < 0)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionAddLayer (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPos));
		}
		break;

	case IGFRAMEHISTORY_STEP_LAYERMOVE:
		{
			int nLayerPosFrom = nParam1;
			int nLayerPosDest = nParam2;
			if ((nLayerPosFrom < 0) || (nLayerPosDest < 0))
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionMoveLayer (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPosFrom, nLayerPosDest));
		}
		break;

	case IGFRAMEHISTORY_STEP_LAYERREMOVE:
		{
			int nLayerPos = nParam1;
			if (nLayerPos < 0)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionRemoveLayer (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPos));
		}
		break;

	case IGFRAMEHISTORY_STEP_LAYERMERGE:
		{
			int nNbLayers = nParam1;
			int *pnLayerIds = (int *)nParam2;
			int nIds[2];
			if ((int)pnLayerIds == 1L) { // recursive case (swap history)
				pnLayerIds = nIds;
				nNbLayers = 2;
			}
			if (!pnLayerIds || (nNbLayers <= 1))
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionMergeLayer (m_spXMLDoc, m_spNodeStep, m_pFrame, nNbLayers, pnLayerIds));
		}
		break;

	case IGFRAMEHISTORY_STEP_MOVE:
		{
			int nLayerPos = nParam1;
			POINT *p_ptVector = (POINT *)nParam2;			
			if ((int)p_ptVector == 1L) // recursive case (swap history)
				p_ptVector = &ptDef;
			if (!p_ptVector)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionMove (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPos, *p_ptVector));
		}
		break;

	case IGFRAMEHISTORY_STEP_MOVEPIXELS:
		{
			RECT *p_rcLayerPosAndVector = (RECT *)nParam1;
			if ((int)p_rcLayerPosAndVector == 1L) // recursive case (swap history)
				p_rcLayerPosAndVector = &rcDef;
			if (!p_rcLayerPosAndVector)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			int nLayerPos = p_rcLayerPosAndVector->top;
			POINT ptVector;
			ptVector.x = p_rcLayerPosAndVector->left;
			ptVector.y = p_rcLayerPosAndVector->right;
			RECT *p_rcSubLayerPos = (RECT *)nParam2;
			if ((int)p_rcSubLayerPos == 1L) // recursive case (swap history)
				p_rcSubLayerPos = &rcDef;
			if (!p_rcSubLayerPos)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionMovePixels (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPos, ptVector, *p_rcSubLayerPos));
		}
		break;

	case IGFRAMEHISTORY_STEP_ROTATEANDRESIZE:
		{
			RECT *p_rcLayerPosAndVector = (RECT *)nParam1;
			if ((int)p_rcLayerPosAndVector == 1L) // recursive case (swap history)
				p_rcLayerPosAndVector = &rcDef;
			if (!p_rcLayerPosAndVector)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			int nLayerPos = p_rcLayerPosAndVector->top;
			float fRate = (float)p_rcLayerPosAndVector->bottom / (float)PIXEL_SELECTOR_PRECISION;
			POINT ptVector;
			ptVector.x = p_rcLayerPosAndVector->left;
			ptVector.y = p_rcLayerPosAndVector->right;
			RECT *p_rcSubLayerPos = (RECT *)nParam2;
			if ((int)p_rcSubLayerPos == 1L) // recursive case (swap history)
				p_rcSubLayerPos = &rcDef;
			if (!p_rcSubLayerPos)
				throw IGEXCEPTION (FrameStepException, "Initialize", "Bad arguments");
			m_spAction.reset (new IGFrameActionRotateAndResize (m_spXMLDoc, m_spNodeStep, m_pFrame, nLayerPos, ptVector, fRate, *p_rcSubLayerPos));
		}
		break;

	default:
		_ASSERTE (0 && L"Unknown history step");
		throw IGEXCEPTION (FrameStepException, "Initialize", "Unknown history step");
	}

	// load step icon
	bool bRes = false;
	if (nResIconId < 0)	// take default action icon id
		nResIconId = m_spAction->GetResIconId();
	if (nResIconId >= 0)
	{
		HRSRC hResIconNormal = ::FindResourceW (GetInstance(), MAKEINTRESOURCEW(nResIconId), L"PNG");
		if (hResIconNormal)
		{
			if (m_cxIcon.LoadResource (hResIconNormal, CXIMAGE_FORMAT_PNG, GetInstance()))
			{
				bRes = true;
			}
		}
	}		
	_ASSERTE (bRes && L"New history step initialization failed");
	return bRes;
}

bool IGFrameStep::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	if (!m_spAction)
		return false;
	bool bRes = m_spAction->Do (lLayerIndices, lSelectionIndices, false);
	if (!m_pFrame->GetWorkingLayer())
		m_pFrame->SetWorkingLayer (m_pFrame->GetNbLayers() - 1);	
	return bRes;
}

bool IGFrameStep::Redo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	if (!m_spAction)
		return false;
	// disable history recording
	m_pFrame->SetHistoryEnabled (false);
	bool bRes = m_spAction->Do (lLayerIndices, lSelectionIndices, true);
	// re-enable history recording
	m_pFrame->SetHistoryEnabled();
	if (!m_pFrame->GetWorkingLayer())
		m_pFrame->SetWorkingLayer (m_pFrame->GetNbLayers() - 1);
	return bRes;
}

bool IGFrameStep::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	if (!m_spAction)
		return false;
	// disable history recording
	m_pFrame->SetHistoryEnabled (false);
	bool bRes = m_spAction->Undo (lLayerIndices, lSelectionIndices);
	// re-enable history recording
	m_pFrame->SetHistoryEnabled();
	if (!m_pFrame->GetWorkingLayer())
		m_pFrame->SetWorkingLayer (m_pFrame->GetNbLayers() - 1);
	return bRes;
}

LPCWSTR IGFrameStep::GetName() const
{
	return m_wsStepName.c_str();
}

EnumStepType IGFrameStep::GetType() const
{
	return m_eType;
}

int IGFrameStep::GetResIconId() const
{
	if (!m_spAction)
		return -1;
	return m_spAction->GetResIconId();
}

bool IGFrameStep::Save()
{
	// write step xml log
	bool bRes = m_spAction->WriteXml();
	if (bRes) // write step pixel data
		bRes = m_spAction->WriteData();
	return bRes;
}

bool IGFrameStep::Draw (HDC hDC, RECT *pRcStep, bool bSelected, bool bOverSelection)
{
	//item background
	Graphics graphics (hDC);	
	SolidBrush backgroundBrush (IGFRAMEHISTORY_COLOR_ITEM);
	Rect rectBackgroundItem (pRcStep->left, pRcStep->top
							, pRcStep->right - pRcStep->left, pRcStep->bottom - pRcStep->top);
	graphics.FillRectangle (&backgroundBrush, rectBackgroundItem);
	Pen penBorder (IGFRAMEHISTORY_COLOR_ITEMBORDER);
	graphics.DrawRectangle (&penBorder, rectBackgroundItem);

	if (bSelected || bOverSelection)
	{
		rectBackgroundItem.X++; rectBackgroundItem.Y++;
		rectBackgroundItem.Height-=2; rectBackgroundItem.Width-=2;
		SolidBrush selectItemBrush (bSelected ? IGFRAMEHISTORY_COLOR_SELECTEDITEM : IGFRAMEHISTORY_COLOR_OVERSELECTEDITEM);
		graphics.FillRectangle (&selectItemBrush, rectBackgroundItem);
		penBorder.SetColor (bSelected ? IGFRAMEHISTORY_COLOR_SELECTEDITEMBORDER : IGFRAMEHISTORY_COLOR_OVERSELECTEDITEMBORDER);
		graphics.DrawRectangle (&penBorder, rectBackgroundItem);
	}

	FontFamily fontFamily (L"Times New Roman");
	Font font (&fontFamily, 12, (m_eType == IGFRAMEHISTORY_STEP_START) ? FontStyleItalic : FontStyleRegular, UnitPixel);
	SolidBrush  solidFontBrush (IGFRAMEHISTORY_COLOR_FONT);
	StringFormat	format (StringFormat::GenericDefault());
	graphics.DrawString (m_wsStepName.c_str(), -1, &font, RectF ((float)pRcStep->left + IGFRAMEHISTORY_OFFSET_STEPNAME, (float)pRcStep->top, (float)(pRcStep->right - pRcStep->left - 1 - IGFRAMEHISTORY_OFFSET_STEPNAME), (float)(pRcStep->bottom - pRcStep->top)),
							&format, &solidFontBrush);	
	return (m_cxIcon.Draw (hDC, IGFRAMESTEP_ICONX, pRcStep->top + IGFRAMESTEP_ICONY, IGFRAMESTEP_ICONSIZE, IGFRAMESTEP_ICONSIZE) == 1);
}

void IGFrameStep::Remove()
{
	// Remove step from xml
	if (m_spNodeStep && m_spNodeParent)
		m_spNodeParent->removeChild (m_spNodeStep, NULL);
}

IGFrameHistory::IGFrameHistory(bool bSwap, IGFrame *pThisFrame)	: m_pThisFrame (pThisFrame)
																, m_nCurrentStep (-1)
																, m_nLastStep (-1)
																, m_hListBox (NULL)
																, m_bSwap (bSwap)
																, m_bHistoryEnabled (true)
{
}

IGFrameHistory::~IGFrameHistory()
{
}

bool IGFrameHistory::CreateHistory (HINSTANCE hInstance, LPCWSTR pcwHistoryRes,
									LPCWSTR pcwFileName, bool bAsync,
									EnumStepType eStartStepId, LPCWSTR pcwStartStepName)
{	
	m_hInstance = hInstance;
	m_pcwHistoryRes = pcwHistoryRes;
	if (!m_pThisFrame)
		m_pThisFrame = dynamic_cast <IGFrame*> (this);
	if (!m_pThisFrame)
		return false;
	m_spSwapHistory = (m_bSwap ? NULL : new IGFrameHistory(true, m_pThisFrame));

	std::wstring wsHistoryPath;
	// retrieve the history path
	if (!pcwFileName)
	{
		wsHistoryPath = createTempDirectory();
		m_wsFileName = wsHistoryPath + L"\\";
		m_wsSwapFileName = m_wsFileName;
		m_wsFileName += HISTORY_FILENAME_DATA;
		m_wsSwapFileName += HISTORY_FILENAME_DATA_SWAP;
		m_wsConfigFileName = wsHistoryPath + L"\\";
		m_wsConfigSwapFileName = m_wsConfigFileName;
		m_wsConfigFileName += HISTORY_FILENAME_CONFIG;
		m_wsConfigSwapFileName += HISTORY_FILENAME_CONFIG_SWAP;
	}
	else
	{		
		m_spConfigMgr->GetHistoryPath (wsHistoryPath);
		int nRes = ::SHCreateDirectory (NULL, wsHistoryPath.c_str());
		if ((nRes != ERROR_SUCCESS) && (nRes != ERROR_FILE_EXISTS) && (nRes != ERROR_ALREADY_EXISTS))
			return false;

		m_wsFileName = wsHistoryPath;
		if (pcwFileName[0] != L'\\')
			m_wsFileName += L"\\";
		m_wsFileName += pcwFileName;		
		wstring wsFrameHistoryFolder = m_wsFileName.substr(0, m_wsFileName.find_last_of(L"\\") + 1);
		if (m_wsFileName.find (HISTORY_FILENAME_DATA) != wstring::npos)
		{
			m_wsSwapFileName = wsFrameHistoryFolder + HISTORY_FILENAME_DATA_SWAP;
			m_wsConfigFileName = wsFrameHistoryFolder + HISTORY_FILENAME_CONFIG;
			m_wsConfigSwapFileName = wsFrameHistoryFolder + HISTORY_FILENAME_CONFIG_SWAP;
		}
		else
		{
			m_wsSwapFileName = wsFrameHistoryFolder + HISTORY_FILENAME_DATA;
			m_wsConfigFileName = wsFrameHistoryFolder + HISTORY_FILENAME_CONFIG_SWAP;
			m_wsConfigSwapFileName = wsFrameHistoryFolder + HISTORY_FILENAME_CONFIG;
		}
	}	
	
	if (m_wsConfigSwapFileName.length() >= MAX_PATH)
		throw IGEXCEPTION (FrameHistoryException, "CreateHistory", "Bad path");

	m_nCurrentStep = -1;
	m_nLastStep = -1;

	HRESULT hr = S_OK;
	if (!m_spXMLDoc)
	{
		hr = ::CoCreateInstance (CLSID_DOMDocument30, NULL, CLSCTX_INPROC_SERVER, 
								IID_IXMLDOMDocument, (void**)&m_spXMLDoc);
	}
	if (hr != S_OK)
		throw IGEXCEPTION (FrameHistoryException, "CreateHistory", "MSXml error");

	// open the history file
	HANDLE hFile = ::CreateFile (m_wsConfigFileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
								 NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = ::CreateFile (m_wsConfigFileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,
								 NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			throw IGEXCEPTION (FrameHistoryException, "CreateHistory", "I/O Error");
		char *pValidXml = IGConfigManager::CreateBufferFromRes (hInstance, pcwHistoryRes, L"Configuration");
		VARIANT_BOOL bSucceeded;
		hr = m_spXMLDoc->loadXML (CComBSTR (pValidXml), &bSucceeded);
		if ((hr != S_OK) || !bSucceeded)
			throw IGEXCEPTION (FrameHistoryException, "CreateHistory", "MSXml error");
		delete [] pValidXml;
		::CloseHandle (hFile);
	}
	else
	{
		::CloseHandle (hFile);
		VARIANT_BOOL bSucceeded;
		hr = m_spXMLDoc->load (CComVariant (m_wsConfigFileName.c_str()), &bSucceeded);
		if ((hr != S_OK) || !bSucceeded)
			throw IGEXCEPTION (FrameHistoryException, "CreateHistory", "MSXml error");
	}

	hr = m_spXMLDoc->selectSingleNode (CComBSTR (IGFRAMEHISTORY_NODESTEPS), &m_spXMLNodeSteps);
	if ((hr != S_OK) || !m_spXMLNodeSteps)
		return false;
	
	return AddNewStep (IGFRAMEHISTORY_STEP_START, pcwStartStepName ? pcwStartStepName : L"Original", eStartStepId, -1, -1, true, bAsync);
}

bool IGFrameHistory::SwapHistory ()
{
	if (!m_spSwapHistory)
		throw IGEXCEPTION (FrameHistoryException, "SwapHistory", "NULL m_spSwapHistory");
	m_wsFileName = m_spSwapHistory->m_wsFileName;
	m_wsConfigFileName = m_spSwapHistory->m_wsConfigFileName;
	m_wsSwapFileName = m_spSwapHistory->m_wsSwapFileName;
	m_wsConfigSwapFileName = m_spSwapHistory->m_wsConfigSwapFileName;
	m_spXMLDoc = m_spSwapHistory->m_spXMLDoc;
	m_spXMLNodeSteps = m_spSwapHistory->m_spXMLNodeSteps;
	m_nCurrentStep = m_spSwapHistory->m_nCurrentStep;
	m_nLastStep = m_spSwapHistory->m_nLastStep;
	m_lspFrameSteps = m_spSwapHistory->m_lspFrameSteps;
	m_lLayerIndices = m_spSwapHistory->m_lLayerIndices;
	m_lSelectionIndices = m_spSwapHistory->m_lSelectionIndices;	
	// delete the next history swap files
	if (!::DeleteFileW (m_wsSwapFileName.c_str()))
		throw IGEXCEPTION (FrameHistoryException, "SwapHistory", "I/O Error");
	if (!::DeleteFileW (m_wsConfigSwapFileName.c_str()))
		throw IGEXCEPTION (FrameHistoryException, "SwapHistory", "I/O Error");
	m_spSwapHistory.reset(new IGFrameHistory(true, m_pThisFrame));
	return true;
}

bool IGFrameHistory::CancelSteps (int nIdxStep)
{
	if (nIdxStep > 0)
		m_nCurrentStep = nIdxStep;
	if (m_nCurrentStep <= 0)	// cannot cancel first step
		throw IGEXCEPTION (FrameHistoryException, "CancelSteps", "StepId is out of range");
	m_nCurrentStep--;	// the current step is comprised in the deletion
	// remove the steps whose position is over the current position
	if (m_spSwapHistory && (m_nLastStep > HISTORY_MAXNBSTEPS))
	{
		if (m_nCurrentStep <= HISTORY_MAXNBSTEPS)
		{
			m_spSwapHistory.reset(new IGFrameHistory(true, m_pThisFrame));
			m_nLastStep = HISTORY_MAXNBSTEPS;
		}
		else
		{
			m_nLastStep = m_nCurrentStep;
			return m_spSwapHistory->CancelSteps (m_nCurrentStep - HISTORY_MAXNBSTEPS + 1);
		}
	}
	for (int idxStep = 0; idxStep < m_nLastStep - m_nCurrentStep; idxStep++)
	{
		m_lspFrameSteps.back()->Remove();
		m_lspFrameSteps.pop_back();
	}
	CleanIndexLists (IGFrameAction::GetMaxId (m_lLayerIndices, m_pThisFrame->GetMaxRemovedLayerId()), IGFrameAction::GetMaxId (m_lSelectionIndices));	

	m_nLastStep = m_nCurrentStep;
	PopulateListBox (m_hListBox);
	HWND hParent = ::GetParent (m_hListBox);
	RECT rcListBox;
	::GetWindowRect (m_hListBox, &rcListBox);
	MapWindowRect (NULL, hParent, &rcListBox);
	::InvalidateRect (hParent, &rcListBox, FALSE);
	return true;
}

bool IGFrameHistory::AddNewStep (EnumStepType eStepType, const wstring& wsStepName, int nParam1, int nParam2, int nResIconId, bool bSaveStep, bool bAsync)
{
	if (!m_bHistoryEnabled)
		return true;
	if (!m_spXMLNodeSteps || !m_pThisFrame)
		return false;
	if (m_nCurrentStep == -1) // First step
	{
		// layers have to be sorted withe their ids in ascending order in the history file because it removes all the layers after the layer with the max id to free space
		m_pThisFrame->PushAndSortLayers();
		// save image to ig format
		HRESULT hRes = m_pThisFrame->InternalSave (m_wsFileName.c_str(), CXIMAGE_FORMAT_IG, false, bAsync);
		// set the layers back to the original order
		m_pThisFrame->PullLayers();
		if (FAILED (hRes))
			throw IGEXCEPTION (FrameHistoryException, "AddNewStep", "I/O Error");	
	}
	else if (m_nCurrentStep < m_nLastStep)
	{
		// remove the steps whose position is over the current position
		if (!CancelSteps (m_nCurrentStep + 1))
			return false;
	}

	if (m_nCurrentStep == 2 * HISTORY_MAXNBSTEPS)
	{
		// swap history
		if (!SwapHistory ())
			return false;
		m_nCurrentStep = HISTORY_MAXNBSTEPS;
	}
	
	if (m_nCurrentStep == HISTORY_MAXNBSTEPS)
	{
		// create alternate history
		wstring wsHistoryPath;
		m_spConfigMgr->GetHistoryPath (wsHistoryPath);
		std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterStep = m_lspFrameSteps.begin();
		for (int idxStep = 0; idxStep < m_nCurrentStep; idxStep++)
			++iterStep;
		m_spSwapHistory->CreateHistory (m_hInstance,
										m_pcwHistoryRes,
										m_wsSwapFileName.substr(wsHistoryPath.length()).c_str(),
										false,
										(*iterStep)->GetType(),
										(*iterStep)->GetName());
		// delete sections from nCurLayerId
		int nCurLayerId = IGFrameAction::GetMaxId (m_spSwapHistory->m_lLayerIndices, m_pThisFrame->GetMaxRemovedLayerId());
		int nCurSelectionId = IGFrameAction::GetMaxId (m_spSwapHistory->m_lSelectionIndices);
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pThisFrame);
		encodeIG.RemoveSections (m_wsSwapFileName.c_str(), nCurLayerId, nCurSelectionId);	
		// check if a selection is active
		if (m_pThisFrame->IsSelection())
		{
			int nCurSelectionId = IGFrameAction::GetMaxId (m_lSelectionIndices);
			if (nCurSelectionId < 0)
				nCurSelectionId = 0;
			m_pThisFrame->AddSelectionId (nCurSelectionId, false);
			// record current selection
			RECT rcFull = {0,0,-1,-1};
			if (!encodeIG.EncodeSelection (m_wsSwapFileName.c_str(), nCurSelectionId, m_pThisFrame->GetLayerPos (m_pThisFrame->GetSelectionLayer()), rcFull))
			{
				_ASSERTE (false && L"IGFrameHistory::AddNewStep - Fatal Error: Encoding selection failed");
				throw IGEXCEPTION (FrameHistoryException, "AddNewStep", "Encoding selection failed");
			}
		}
	}	
	
	if (m_nCurrentStep >= HISTORY_MAXNBSTEPS)
	{
		// record alternate history
		if (!m_spSwapHistory->AddNewStep (eStepType, wsStepName, nParam1, nParam2, nResIconId, bSaveStep))
		{
			if (m_nCurrentStep == HISTORY_MAXNBSTEPS)
				m_spSwapHistory.reset(new IGFrameHistory(true, m_pThisFrame));
			return false;
		}
		m_nCurrentStep++;
		m_nLastStep = m_nCurrentStep;			
	}
	else
	{
		// create the new history step
		IGFrameStep *pNewStep = new IGFrameStep (m_pThisFrame, m_spXMLDoc, m_spXMLNodeSteps);
		if (!pNewStep->Initialize (eStepType, m_nCurrentStep, wsStepName, nResIconId, nParam1, nParam2))
			return false;
		IGSmartPtr <IGFrameStep> spNewStep (pNewStep);
		if (!spNewStep->Do (m_lLayerIndices, m_lSelectionIndices))
			return false;	
		
		if (bSaveStep)
		{
			if (!spNewStep->Save())
				return false;
		}
		
		// save History.xml
		HRESULT hr = m_spXMLDoc->save (CComVariant (m_wsConfigFileName.c_str()));
		_ASSERTE ((hr == S_OK) && L"History saving failed");
		if (hr != S_OK)
			throw IGEXCEPTION (FrameHistoryException, "AddNewStep", "MSXml Error");
		m_lspFrameSteps.push_back (spNewStep);

		m_nCurrentStep++;
		m_nLastStep = m_nCurrentStep;

		if (m_hListBox)
		{
			::SendMessageW (m_hListBox, LB_INSERTSTRING, 0, (LPARAM)spNewStep->GetName());
			::SendMessageW (m_hListBox, LB_SETCURSEL, 0, 0);
			::InvalidateRect (m_hListBox, NULL, FALSE);
		}
	}	
	return true;
}

bool IGFrameHistory::RestoreStep (int nIdxRestoreStep)
{
	if (!m_spXMLNodeSteps || !m_pThisFrame)
		return false;	
	if (!m_pThisFrame->RequestAccess())
		throw IGEXCEPTION (FrameHistoryException, "RestoreStep", "Critical section error");
	if (nIdxRestoreStep < 0 || nIdxRestoreStep > m_nLastStep)
		throw IGEXCEPTION (FrameHistoryException, "RestoreStep", "StepId is out of range");
	if (m_nCurrentStep == nIdxRestoreStep)
		return true;

	// update alternate history
	if (m_nCurrentStep > HISTORY_MAXNBSTEPS)
	{
		if (nIdxRestoreStep > HISTORY_MAXNBSTEPS)
		{
			m_nCurrentStep = nIdxRestoreStep;
			return m_spSwapHistory->RestoreStep (nIdxRestoreStep - HISTORY_MAXNBSTEPS);
		}
		else
		{
			m_spSwapHistory->RestoreStep (0);
			m_nCurrentStep = HISTORY_MAXNBSTEPS;
		}
	}

	if (nIdxRestoreStep > m_nCurrentStep)
	{		
		std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterStep = m_lspFrameSteps.begin();
		for (int idxStep = 0; idxStep <= m_nCurrentStep; idxStep++)
			++iterStep;
		for (int idxStep = m_nCurrentStep; ((iterStep != m_lspFrameSteps.end()) && (idxStep < nIdxRestoreStep)) ; ++iterStep, idxStep++)
		{
			if (!(*iterStep)->Redo (m_lLayerIndices, m_lSelectionIndices))
				return false;
		}
		if (nIdxRestoreStep > HISTORY_MAXNBSTEPS)
		{
			m_nCurrentStep = nIdxRestoreStep;
			m_spSwapHistory->RestoreStep (nIdxRestoreStep - HISTORY_MAXNBSTEPS);
		}
	}
	else if (nIdxRestoreStep < m_nCurrentStep)
	{
		std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterStep = m_lspFrameSteps.begin();
		for (int idxStep = 0; idxStep < m_nCurrentStep; idxStep++)
			++iterStep;
		int idxStep = m_nCurrentStep - nIdxRestoreStep;
		while (idxStep-- > 0 && iterStep != m_lspFrameSteps.begin())
		{
			if (!(*iterStep--)->Undo (m_lLayerIndices, m_lSelectionIndices))
				return false;
		}
	}	

	// transfer current layer history set to current frame
	m_nCurrentStep = nIdxRestoreStep;
	m_pThisFrame->GetWorkingLayer()->SelectionRebuildBox();	// parsing historay affects the selection box
	m_pThisFrame->Redraw (true);
	::SendMessageW (m_pThisFrame->m_hOwner, UM_IGFRAME_LAYERCHANGED, 0, 0);
	return true;
}

bool IGFrameHistory::SaveStep()
{
	if (!m_bHistoryEnabled)
		return true;
	if (!m_spXMLNodeSteps || !m_pThisFrame)
		return false;	
	if (!m_pThisFrame->RequestAccess())
		throw IGEXCEPTION (FrameHistoryException, "SaveStep", "Critical section error");
	// save alternate history if present
	if (m_spSwapHistory && (m_nCurrentStep > HISTORY_MAXNBSTEPS))
	{
		return m_spSwapHistory->SaveStep();
	}
	else
	{
		std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterStep = m_lspFrameSteps.begin();
		for (int idxStep = 0; idxStep < m_nCurrentStep; idxStep++)
			++iterStep;
		if (!(*iterStep)->Save())
			return false;	
	}	
	return true;
}

int IGFrameHistory::GetNbSteps() const
{
	// the original step is not taken into account
	int nNbListItems = m_lspFrameSteps.size();
	return (nNbListItems > 0 ? nNbListItems - 1 : 0);
}

LPCWSTR IGFrameHistory::GetStepName(int nStepId) const
{
	if (m_spSwapHistory && (nStepId > HISTORY_MAXNBSTEPS))
		return m_spSwapHistory->GetStepName (nStepId - HISTORY_MAXNBSTEPS);
	if (nStepId >= (int)m_lspFrameSteps.size())
		throw IGEXCEPTION (FrameHistoryException, "GetStepName", "StepId is out of range");
	std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterStep = m_lspFrameSteps.begin();
	for (int idxStep = 0; idxStep < nStepId; idxStep++)
		++iterStep;
	return (*iterStep)->GetName();
}

const IGFrameAction& IGFrameHistory::GetAction(int nStepId) const
{
	if (m_spSwapHistory && (nStepId > HISTORY_MAXNBSTEPS))
		return m_spSwapHistory->GetAction (nStepId - HISTORY_MAXNBSTEPS);
	if (nStepId >= (int)m_lspFrameSteps.size())
		throw IGEXCEPTION (FrameHistoryException, "GetStep", "StepId is out of range");
	std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterStep = m_lspFrameSteps.begin();
	for (int idxStep = 0; idxStep < nStepId; idxStep++)
		++iterStep;
	return (*iterStep)->GetAction();
}

void IGFrameHistory::PopulateListBox (HWND hListBox, bool bFillOnly)
{
	m_hListBox = hListBox;
	if (!bFillOnly)
		::SendMessage (hListBox, LB_RESETCONTENT, 0, 0); 
	int nIdxOffset = 0;
	if (m_spSwapHistory)
	{
		m_spSwapHistory->PopulateListBox (hListBox, true);
		nIdxOffset = m_spSwapHistory->GetNbSteps();
	}
	std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterIdxLayer = m_lspFrameSteps.begin();
	for (unsigned int idxLayer = (bFillOnly ? 1 : 0); 
		(idxLayer < m_lspFrameSteps.size() && iterIdxLayer != m_lspFrameSteps.end());
		idxLayer++, ++iterIdxLayer)
	{
		::SendMessage (m_hListBox, LB_INSERTSTRING, nIdxOffset, (LPARAM)(*iterIdxLayer)->GetName()); 
	}	
	if (!bFillOnly)
		::SendMessageW (m_hListBox, LB_SETCURSEL, m_nLastStep - m_nCurrentStep, 0);
}

bool IGFrameHistory::HistoryDraw (HWND hListBox, int nOffsetWidth, HDC hDC)
{
	bool bRes = true;
	if (!m_lspFrameSteps.empty())
	{
		int nIdxOffset = 0;
		int nOffsetNoFirstStep = 1;
		if (m_spSwapHistory)
		{
			bRes = m_spSwapHistory->HistoryDraw (hListBox, nOffsetWidth, hDC); 
			nIdxOffset = m_spSwapHistory->GetNbSteps();
			nOffsetNoFirstStep = 0;
		}
		std::list <IGSmartPtr <IGFrameStep>>::const_iterator itItem = m_lspFrameSteps.begin();
		for (unsigned int idxItem = 0; idxItem < m_lspFrameSteps.size() - nOffsetNoFirstStep; ++idxItem, ++itItem)
		{
			if (bRes)
				bRes &= HistoryDraw (hListBox, nOffsetWidth, hDC, idxItem, nIdxOffset);
		}		
	}
	return bRes;
}

bool IGFrameHistory::HistoryDraw (HWND hListBox, int nOffsetWidth, HDC hDC, int nItemId, int nIdxOffset)
{
	RECT rcListBox;
	::GetWindowRect (hListBox, &rcListBox);	
	int nCurSel = (unsigned int)::SendMessageW (hListBox, (UINT)LB_GETCURSEL, 0, 0);
	int nListboxItemId = nItemId + nIdxOffset;
	RECT rcItem;
	::ZeroMemory (&rcItem, sizeof (RECT));
	::SendMessageW (m_hListBox, (UINT)LB_GETITEMRECT, (WPARAM)nListboxItemId, (LPARAM)&rcItem);
	rcItem.right -= nOffsetWidth;
	list <IGSmartPtr <IGFrameStep>>::const_iterator itItem = m_lspFrameSteps.begin();
	for (int idxItem = 0; idxItem < ((int)m_lspFrameSteps.size() - nItemId - 1); ++idxItem, ++itItem)
	{
		if (itItem == m_lspFrameSteps.end())
			return false;
	}
	if ((rcItem.top >= rcListBox.bottom - rcListBox.top) || 
		(rcItem.bottom <= 0))
		return false;
	if (!(*itItem)->Draw (hDC, &rcItem, (nListboxItemId == nCurSel), nListboxItemId < nCurSel))
		return false;
	return true;
}

LPCWSTR	IGFrameHistory::GetHistoryDataPath() const
{
	return (m_nCurrentStep > HISTORY_MAXNBSTEPS) ? m_spSwapHistory->GetHistoryDataPath() : m_wsFileName.c_str();
}

int IGFrameHistory::GetMaxLayerId() const
{
	int nMaxLayerId = -1;
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pThisFrame);
	nMaxLayerId = encodeIG.GetMaxLayerId (GetHistoryDataPath());
	if (m_spSwapHistory)
	{
		int nAlternateHistoryMaxLayerId = encodeIG.GetMaxLayerId (m_spSwapHistory->GetHistoryDataPath());
		if (nAlternateHistoryMaxLayerId > nMaxLayerId)
			nMaxLayerId = nAlternateHistoryMaxLayerId;
	}
	int nMaxRemovedLayerId = GetMaxRemovedLayerId();
	if (nMaxRemovedLayerId > nMaxLayerId)
		return nMaxRemovedLayerId;
	return nMaxLayerId;
}

int IGFrameHistory::GetMaxRemovedLayerId() const
{
	if (!m_setRemovedLayers.empty())
	{
		set <int>::const_iterator itMaxRemovedLayer = m_setRemovedLayers.end();
		return *--itMaxRemovedLayer;
	}
	return -1;
}

int IGFrameHistory::GetMaxSelectionId() const
{
	int nMaxSelectionId = -1;
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pThisFrame);
	nMaxSelectionId = encodeIG.GetMaxSelectionId (GetHistoryDataPath());
	if (m_spSwapHistory)
	{
		int nAlternateHistoryMaxSelectionId = encodeIG.GetMaxSelectionId (m_spSwapHistory->GetHistoryDataPath());
		if (nAlternateHistoryMaxSelectionId > nMaxSelectionId)
			return nAlternateHistoryMaxSelectionId;
	}	
	return nMaxSelectionId;
}

void IGFrameHistory::AddSelectionId(int nSelectionId, bool bClear)
{
	if (m_spSwapHistory && (m_nCurrentStep >= HISTORY_MAXNBSTEPS)){
		m_spSwapHistory->AddSelectionId(nSelectionId, bClear);
	}
	else{
		bool bIsMax = true;
		if (nSelectionId >= 0){
			for (list <pair <int, bool>>::const_iterator itSelIdx = m_lSelectionIndices.begin(); 
					itSelIdx != m_lSelectionIndices.end();
					++itSelIdx)
			{
				/*
				_ASSERTE (((*itSelIdx).first != nSelectionId) && L"IGFrameHistory::AddSelectionId Error");
				if ((*itSelIdx).first == nSelectionId)
					throw IGEXCEPTION (FrameHistoryException, "AddSelectionId", "Cannot add the same selectionId twice");*/
				if ((*itSelIdx).first >= nSelectionId)
					bIsMax = false;
			}
		}/*
		_ASSERTE (bIsMax && L"IGFrameHistory::AddSelectionId Error");
		if (!bIsMax)
			throw IGEXCEPTION (FrameHistoryException, "AddSelectionId", "A higher selectionId has been found");*/
		if (bIsMax)
			m_lSelectionIndices.push_back (pair <int, bool>(nSelectionId, bClear));
		else if (nSelectionId == -1 && bClear)
			m_lSelectionIndices.push_back (pair <int, bool>(nSelectionId, bClear));
	}
}

void IGFrameHistory::ReplayAllSelections (int nLayerPos)
{
	for (list <pair <int, bool>>::const_iterator itSelIdx = m_lSelectionIndices.begin(); 
				itSelIdx != m_lSelectionIndices.end();
				++itSelIdx)
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pThisFrame);
		if ((*itSelIdx).second)
		{
			std::list<POINT> lPts;
			m_pThisFrame->ManageSelection (IGSELECTION_CLEAR, lPts);
		}
		if ((*itSelIdx).first >= 0){
			if (!encodeIG.DecodeSelection (m_wsFileName.c_str(), (*itSelIdx).first, nLayerPos, true))
			{
				// try to fix the pb
				m_lSelectionIndices.remove (*itSelIdx);
				ReplayAllSelections (nLayerPos);
				return;
			}
		}
	}
	if (m_spSwapHistory && (m_nCurrentStep > HISTORY_MAXNBSTEPS)){
		m_spSwapHistory->ReplayAllSelections (nLayerPos);
	}
}

void IGFrameHistory::CleanIndexLists (int nMaxLayerId, int nMaxSelectionId)
{
	list <int> lRemoveLayerIndices;
	for (list <int>::const_iterator itLayIdx = m_lLayerIndices.begin(); 
				itLayIdx != m_lLayerIndices.end();
				++itLayIdx)
	{
		if (*itLayIdx > nMaxLayerId)
			lRemoveLayerIndices.push_back (*itLayIdx);
	}
	for (list <int>::const_iterator itLayIdx = lRemoveLayerIndices.begin(); 
				itLayIdx != lRemoveLayerIndices.end();
				++itLayIdx)
		m_lLayerIndices.remove (*itLayIdx);

	list <pair <int, bool>> lRemoveSelectionIndices;
	for (list <pair <int, bool>>::const_iterator itSelIdx = m_lSelectionIndices.begin(); 
				itSelIdx != m_lSelectionIndices.end();
				++itSelIdx)
	{
		if ((*itSelIdx).first > nMaxSelectionId)
			lRemoveSelectionIndices.push_back (*itSelIdx);
	}
	for (list <pair <int, bool>>::const_iterator itSelIdx = lRemoveSelectionIndices.begin(); 
				itSelIdx != lRemoveSelectionIndices.end();
				++itSelIdx)
		m_lSelectionIndices.remove (*itSelIdx);
}

void IGFrameHistory::GetStepIds (std::wstring& wsStepIds) const
{	
	std::list<IGSmartPtr <IGFrameStep>>::const_iterator iterIdxLayer = m_lspFrameSteps.begin();
	int nInitialStepIdx = 0;
	if (!m_spSwapHistory && (iterIdxLayer != m_lspFrameSteps.end()))
	{
		// do not display initial step for history swap (intermediary history creation virtual step)
		nInitialStepIdx = 1;
		++iterIdxLayer;
	}
	for (unsigned int idxLayer = nInitialStepIdx; 
		(idxLayer < m_lspFrameSteps.size() && iterIdxLayer != m_lspFrameSteps.end());
		idxLayer++, ++iterIdxLayer)
	{
		if (!wsStepIds.empty())
			wsStepIds += L"_";
		wchar_t wStepIdBuf[8];
		ZeroMemory(wStepIdBuf, sizeof(wStepIdBuf));
		_itow_s((int)(*iterIdxLayer)->GetType(), &wStepIdBuf[0], 8, 10); 
		wsStepIds += wStepIdBuf;
		wsStepIds += L"%";
		wsStepIds += (*iterIdxLayer)->GetName();
	}
	if (m_spSwapHistory)
		m_spSwapHistory->GetStepIds (wsStepIds);
}

std::wstring IGFrameHistory::createTempDirectory()
{
	std::wstring wsHistoryTmpPath;
	m_spConfigMgr->GetHistoryPath (wsHistoryTmpPath);
	wsHistoryTmpPath += L"\\";	
	wsHistoryTmpPath += IGFRAMEHISTORY_BASETEMPFILENAME;
	int nNumTest = 0;
	std::wstring wsHistoryTestTmpPath;
	int nRes = ERROR_FILE_EXISTS;
	wchar_t pwNum [MAX_PATH];
	::ZeroMemory (pwNum, MAX_PATH * sizeof (wchar_t));
	while ((nRes == ERROR_FILE_EXISTS) || (nRes == ERROR_ALREADY_EXISTS))
	{
		wsHistoryTestTmpPath = wsHistoryTmpPath;			
		_itow (nNumTest++, pwNum, 10);
		_ASSERTE (pwNum && "History internal error");
		wsHistoryTestTmpPath += pwNum;
		nRes = ::SHCreateDirectory (NULL, wsHistoryTestTmpPath.c_str());
	}
	_ASSERTE ((nRes == ERROR_SUCCESS) && "History internal error");
	return wsHistoryTestTmpPath;
}

void IGFrameHistory::AddRemovedLayer (int nLayerId)
{
	m_setRemovedLayers.insert (nLayerId);
}

void IGFrameHistory::RemoveRemovedLayer (int nLayerId)
{
	m_setRemovedLayers.erase (nLayerId);
}