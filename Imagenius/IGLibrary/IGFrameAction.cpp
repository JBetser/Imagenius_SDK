#include "StdAfx.h"
#include "IGFrameAction.h"
#include "IGFrame.h"
#include "ximaig.h"
#include "IGLayer.h"
#include "IGException.h"
#include "Resource.h"

using namespace IGLibrary;
using namespace std;

#define IGFRAMEACTION_LAYERPOS				L"pos"
#define IGFRAMEACTION_LAYERID				L"id"
#define IGFRAMEACTION_TYPE					L"type"
#define IGFRAMEACTIONMOVELAYER_LAYERFROM	L"from"
#define IGFRAMEACTIONMOVELAYER_LAYERDEST	L"dest"
#define IGFRAMEACTION_NBLAYERS				L"nblayers"
#define IGFRAMEACTIONCHANGESIZE_WIDTHFROM	L"widthfrom"
#define IGFRAMEACTIONCHANGESIZE_HEIGHTFROM	L"heightfrom"
#define IGFRAMEACTIONCHANGESIZE_WIDTHTO		L"widthto"
#define IGFRAMEACTIONCHANGESIZE_HEIGHTTO	L"heightto"

IGException FrameActionException ("IGFrameAction");
IGException FrameActionAddLayerException ("IGFrameActionAddLayer");
IGException FrameActionChangeLayerException ("IGFrameActionChangeLayer");
IGException FrameActionChangeSelectionException ("IGFrameActionChangeSelection");
IGException FrameActionChangeSizeException ("IGFrameActionChangeSize");
IGException FrameActionMergeLayerException ("IGFrameActionMergeLayer");
IGException FrameActionMoveException ("IGFrameActionMove");
IGException FrameActionMoveLayerException ("IGFrameActionMoveLayer");
IGException FrameActionMovePixelsException ("IGFrameActionMovePixels");
IGException FrameActionRemoveLayerException ("IGFrameActionRemoveLayer");
IGException FrameActionStartException ("IGFrameActionStart");

IGFrameAction::IGFrameAction (CComPtr <IXMLDOMDocument> spXMLDoc, 
							  CComPtr <IXMLDOMNode> spXMLNode, 
							  IGFrame *pFrame,
							  int nLayerId,
							  int nResIconId)	: m_nLayerId (nLayerId)
												, m_pFrame (pFrame)
												, m_spXMLDoc (spXMLDoc)
												, m_spXMLNodeStep (spXMLNode)
												, m_nResIconId (nResIconId)
{	
}

IGFrameAction::~IGFrameAction()
{
}

int IGFrameAction::GetLayerId() const
{
	return m_nLayerId;
}

int IGFrameAction::GetResIconId() const
{
	return m_nResIconId;
}

bool IGFrameAction::WriteXml()
{
	if (!m_pFrame || !m_spXMLNodeStep || !m_spXMLDoc)
		return false;
	// Step node
	CComPtr <IXMLDOMNode> spNodeNewStep;
	HRESULT hr = m_spXMLDoc->createNode (CComVariant (NODE_ELEMENT), 
											CComBSTR (GetName()), 
											NULL,
											&spNodeNewStep);
	_ASSERTE ((hr == S_OK) && spNodeNewStep);
	if ((hr != S_OK) || !spNodeNewStep)
		throw IGEXCEPTION (FrameActionException, "WriteXml", "History msxml error");
	hr = m_spXMLNodeStep->appendChild (spNodeNewStep, &m_spXMLNodeStepValue);
	_ASSERTE ((hr == S_OK) && m_spXMLNodeStepValue);
	if ((hr != S_OK) || !m_spXMLNodeStepValue)
		return false;
	return true;
}

bool IGFrameAction::CreateAttribute (const wchar_t *pcwAttName, VARIANT varAttValue)
{
	if (!pcwAttName)
		return false;
	CComPtr <IXMLDOMNode> spAttributePos;
	HRESULT hr = m_spXMLDoc->createNode (CComVariant (NODE_ATTRIBUTE), 
		CComBSTR (pcwAttName), 
		NULL,
		&spAttributePos);
	_ASSERTE ((hr == S_OK) && spAttributePos);
	if ((hr != S_OK) || !spAttributePos)
		return false;
	CComPtr <IXMLDOMNamedNodeMap> spXMLNodeStepAttributes;
	hr = m_spXMLNodeStepValue->get_attributes (&spXMLNodeStepAttributes);
	_ASSERTE ((hr == S_OK) && spXMLNodeStepAttributes);
	if ((hr != S_OK) || !spXMLNodeStepAttributes)
		return false;
	CComPtr <IXMLDOMNode> spAttributePosAdded;
	hr = spXMLNodeStepAttributes->setNamedItem (spAttributePos, &spAttributePosAdded);
	_ASSERTE ((hr == S_OK) && spAttributePosAdded);
	if ((hr != S_OK) || !spAttributePosAdded)
		return false;
	return (spAttributePosAdded->put_nodeValue (varAttValue) == S_OK);
}

bool IGFrameAction::SetFrameSize (int nWidth, int nHeight)
{
	if (!m_pFrame)
		return false;
	m_pFrame->SetSize (nWidth, nHeight);
	return true;
}

int IGFrameAction::GetMaxId (const std::list <int>& lIndices, int nMaxRemovedLayerId)
{
	int nMaxId = -1;
	for (list <int>::const_iterator itIdx = lIndices.begin(); 
		(itIdx != lIndices.end()); ++itIdx)
	{
		if (nMaxId < *itIdx)
			nMaxId = *itIdx;
	}
	return max (nMaxId, nMaxRemovedLayerId);
}

int IGFrameAction::GetMaxId (const std::list <pair <int, bool>>& lIndices)
{
	int nMaxId = -1;
	for (list <pair <int, bool>>::const_iterator itIdx = lIndices.begin(); 
		(itIdx != lIndices.end()); ++itIdx)
	{
		if (nMaxId < (*itIdx).first)
			nMaxId = (*itIdx).first;
	}
	return nMaxId;
}

IGFrameActionStart::IGFrameActionStart (CComPtr <IXMLDOMDocument> spXMLDoc,
										CComPtr <IXMLDOMNode> spXMLNode,
										IGFrame *pFrame)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, -1, IDB_STEPSTART)
{
}

LPCWSTR IGFrameActionStart::GetName()
{
	return L"Start";
}

bool IGFrameActionStart::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;	
	return true;
}

bool IGFrameActionStart::WriteData()
{
	// no data to write
	return true;
}

bool IGFrameActionStart::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	// this action cannot be undone
	return false;
}

bool IGFrameActionStart::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	// initialize layer index list
	for (int idxLayer = 0; idxLayer < m_pFrame->GetNbLayers(); idxLayer++)
	{
		_ASSERTE ((int)m_pFrame->GetLayer (idxLayer)->GetId() >= 0 && L"IGFrameActionStart - Fatal error: found bad layer id");
		if ((int)m_pFrame->GetLayer (idxLayer)->GetId() < 0)
			throw IGEXCEPTION (FrameActionStartException, "Do", "History msxml error");
		lLayerIndices.push_back ((int)m_pFrame->GetLayer (idxLayer)->GetId());
	}
	return true;
}

IGFrameActionChangeSelection::IGFrameActionChangeSelection (CComPtr <IXMLDOMDocument> spXMLDoc, 
															CComPtr <IXMLDOMNode> spXMLNode, 
															IGFrame *pFrame, 
															const RECT& rcSelection,
															IGSELECTIONENUM eType,
															int nSelectionId,
															int nLayerPos)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, -1, IDB_STEPCHANGESELECTION)
																								, m_nSelectionId (nSelectionId)
																								, m_rcSelection (rcSelection)
																								, m_eType (eType)
																								, m_nLayerPos (nLayerPos)
{
	if (pFrame && (m_nLayerPos < 0))
		m_nLayerPos = pFrame->GetLayerPos (pFrame->GetWorkingLayer());
	if (((m_eType & IGSELECTION_MAGIC) == IGSELECTION_MAGIC) ||
		((m_eType & IGSELECTION_LPE) == IGSELECTION_LPE) ||
		((m_eType & IGSELECTION_FACES) == IGSELECTION_FACES)){
		m_rcSelection.right = -1;	// full selection in case of magic or LPE or Face
		m_rcSelection.bottom = -1;
	}

}

LPCWSTR IGFrameActionChangeSelection::GetName()
{
	return L"Selection";
}

bool IGFrameActionChangeSelection::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Selection id attribute
	if (!CreateAttribute (IGFRAMEACTION_LAYERID, CComVariant (m_nSelectionId)))
		return false;
	// Type attribute
	return CreateAttribute (IGFRAMEACTION_TYPE, CComVariant (m_eType));
}

bool IGFrameActionChangeSelection::WriteData()
{
	if ((m_eType & IGSELECTION_CLEAR) != IGSELECTION_CLEAR)
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		if (!encodeIG.EncodeSelection (m_pFrame->GetHistoryDataPath(), m_nSelectionId, m_nLayerPos, m_rcSelection))
			throw IGEXCEPTION (FrameActionChangeSelectionException, "WriteData", "EncodeSelection failed");
	}
	return true;
}

bool IGFrameActionChangeSelection::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	list <pair <int, bool>> lNewSelectionIndices;
	int idxSel = 0;
	for (list <pair <int, bool>>::iterator itLayIdx = lSelectionIndices.begin(); 
				(itLayIdx != lSelectionIndices.end()); ++itLayIdx)
	{
		if (++idxSel == (int)lSelectionIndices.size())
			break;
		lNewSelectionIndices.push_back (*itLayIdx);		
	}
	lSelectionIndices = lNewSelectionIndices;
	//lSelectionIndices.remove (pair <int, bool> (m_nSelectionId, true));
	//lSelectionIndices.remove (pair <int, bool> (m_nSelectionId, false));
	m_pFrame->GetLayer (m_nLayerPos)->SelectionDelete();
	m_pFrame->ReplayAllSelections (m_nLayerPos);		
	m_pFrame->SetWorkingLayer (m_nLayerPos);
	return true;
}

bool IGFrameActionChangeSelection::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	bool bClear = ((m_eType & IGSELECTION_CLEAR) == IGSELECTION_CLEAR) ||
		((m_eType & IGSELECTION_REPLACE) == IGSELECTION_REPLACE);
	if (bRedo)
	{
		if (bClear)
		{
			list<POINT> lPts;
			m_pFrame->ManageSelection (IGSELECTION_CLEAR, lPts);
		}
		if ((m_eType & IGSELECTION_CLEAR) != IGSELECTION_CLEAR)
		{
			CxImageIG encodeIG;
			encodeIG.Ghost (m_pFrame);
			if (!encodeIG.DecodeSelection (m_pFrame->GetHistoryDataPath(), m_nSelectionId, m_nLayerPos))
			{
				_ASSERTE (false && L"IGFrameActionChangeSelection::Do - Fatal Error: Decoding selection failed");
				throw IGEXCEPTION (FrameActionChangeSelectionException, "Do", "EncodeSelection failed");
			}
		}
		m_pFrame->SetWorkingLayer (m_nLayerPos);
	}
	else
	{
		// at first time, retain the added selection id	
		if (m_nLayerPos >= m_pFrame->GetNbLayers())
			throw IGEXCEPTION (FrameActionChangeSelectionException, "Do", "m_nLayerPos >= m_pFrame->GetNbLayers()");
		if ((m_eType & IGSELECTION_CLEAR) != IGSELECTION_CLEAR)
		{
			// delete selections from m_nSelectionId
			m_nSelectionId = GetMaxId (lSelectionIndices);
			CxImageIG encodeIG;
			encodeIG.Ghost (m_pFrame);
			encodeIG.RemoveSections (m_pFrame->GetHistoryDataPath(), GetMaxId (lLayerIndices, m_pFrame->GetMaxRemovedLayerId()), m_nSelectionId);	
			m_nSelectionId++;				
		}
	}
	m_pFrame->AddSelectionId (m_nSelectionId, bClear);
	return true;
}

IGFrameActionChangeSize::IGFrameActionChangeSize (	CComPtr <IXMLDOMDocument> spXMLDoc, 
	CComPtr <IXMLDOMNode> spXMLNode, 
	IGFrame *pFrame,
	const POINT& ptSize, bool bStretch)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, -1, IDB_STEPADDLAYER)
	, m_ptNewSize (ptSize)
	, m_bStretch (bStretch)
{
	m_ptOldSize.x = pFrame->GetWidth();
	m_ptOldSize.y = pFrame->GetHeight(); 
}

LPCWSTR IGFrameActionChangeSize::GetName()
{
	return L"Resize";
}

bool IGFrameActionChangeSize::WriteXml()
{
	if (m_bStretch && (m_nLayerId < 0))
		return false;
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// size from attributes
	if (!CreateAttribute (IGFRAMEACTIONCHANGESIZE_WIDTHFROM, CComVariant (m_ptOldSize.x)))
		return false;
	if (!CreateAttribute (IGFRAMEACTIONCHANGESIZE_HEIGHTFROM, CComVariant (m_ptOldSize.y)))
		return false;
	// size to attributes
	if (!CreateAttribute (IGFRAMEACTIONCHANGESIZE_WIDTHTO, CComVariant (m_ptNewSize.x)))
		return false;
	return CreateAttribute (IGFRAMEACTIONCHANGESIZE_HEIGHTTO, CComVariant (m_ptNewSize.y));
}

bool IGFrameActionChangeSize::WriteData()
{
	if (m_bStretch)
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		for (int nPos = 0; nPos < m_pFrame->GetNbLayers(); nPos++)
		{
			int nCurLayerId = m_nLayerId + nPos;
			encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), nCurLayerId, nPos);
		}
	}
	return true;
}

bool IGFrameActionChangeSize::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	if (m_bStretch)
	{
		bool bRes = true;
		vector <pair <int, int>>::iterator itMapIdx = m_vMapIndices.begin(); 
		list <int> lLayerIndicesCpy (lLayerIndices);
		int nPos = 0;
		for (list <int>::const_iterator itLayIdx = lLayerIndicesCpy.begin(); 
			(itLayIdx != lLayerIndicesCpy.end()); ++itLayIdx)
		{
			IGFrameActionChangeLayer changeLayer (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, nPos++,  (*itMapIdx).first,  (*itMapIdx).second);
			bRes &= changeLayer.Undo (lLayerIndices, lSelectionIndices);
			if (!bRes)
				throw IGEXCEPTION (FrameActionChangeSizeException, "Undo", "changeLayer.Undo failed");
			++itMapIdx;
		}
		bRes &= SetFrameSize (m_ptOldSize.x, m_ptOldSize.y);
		if (!bRes)
			throw IGEXCEPTION (FrameActionChangeSizeException, "Undo", "SetFrameSize failed");
		return bRes;
	}
	return SetFrameSize (m_ptOldSize.x, m_ptOldSize.y);
}

bool IGFrameActionChangeSize::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	if (m_bStretch)
	{
		if (bRedo)
		{
			bool bRes = true;
			vector <pair <int, int>>::iterator itMapIdx = m_vMapIndices.begin(); 
			list <int> lLayerIndicesCpy (lLayerIndices);
			int nPos = 0;
			for (list <int>::iterator itLayIdx = lLayerIndicesCpy.begin(); 
				(itLayIdx != lLayerIndicesCpy.end()); ++itLayIdx)
			{
				IGFrameActionChangeLayer changeLayer (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, nPos++,  (*itMapIdx).first,  (*itMapIdx).second);
				bRes &= changeLayer.Do (lLayerIndices, lSelectionIndices, true);
				if (!bRes)
					throw IGEXCEPTION (FrameActionChangeSizeException, "Do", "changeLayer.Do failed");
				++itMapIdx;
			}
			bRes &= SetFrameSize (m_ptNewSize.x, m_ptNewSize.y);
			if (!bRes)
				throw IGEXCEPTION (FrameActionChangeSizeException, "Do", "SetFrameSize failed");
			return bRes;
		}
		else
		{
			// in case of change size, the layer id has to be updated
			m_nLayerId = GetMaxId (lLayerIndices, m_pFrame->GetMaxRemovedLayerId());
			// delete layers from m_nLayerDestId
			CxImageIG encodeIG;
			encodeIG.Ghost (m_pFrame);
			_ASSERTE (m_nLayerId >= 0 && L"IGFrameActionChangeSize::Do - Fatal error: wrong layer id");
			if (m_nLayerId < 0)
				throw IGEXCEPTION (FrameActionChangeSizeException, "Do", "wrong layer id");
			encodeIG.RemoveSections (m_pFrame->GetHistoryDataPath(), m_nLayerId, GetMaxId (lSelectionIndices));	
			m_nLayerId++;
			int nLayDestIdx = m_nLayerId;
			int nPos = 0;
			for (list <int>::iterator itLayIdx = lLayerIndices.begin(); 
				(itLayIdx != lLayerIndices.end()); ++itLayIdx, nLayDestIdx++, nPos++)
			{
				IGLayer *pLayer = m_pFrame->GetLayer (nPos);
				if (!pLayer)
					throw IGEXCEPTION (FrameActionChangeSizeException, "Do", "wrong layer position");
				pLayer->SetId (nLayDestIdx);
				m_vMapIndices.push_back (pair <int, int> (*itLayIdx, nLayDestIdx));
				*itLayIdx = nLayDestIdx;
			}
			return true;
		}
	}
	return SetFrameSize (m_ptNewSize.x, m_ptNewSize.y);
}

IGFrameActionAddLayer::IGFrameActionAddLayer (CComPtr <IXMLDOMDocument> spXMLDoc, 
											  CComPtr <IXMLDOMNode> spXMLNode, 
											  IGFrame *pFrame, 
											  int nLayerPos,
											  int nLayerId)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, nLayerId, IDB_STEPADDLAYER)
															, m_nLayerPos (nLayerPos)
{
}

LPCWSTR IGFrameActionAddLayer::GetName()
{
	return L"AddLayer";
}

bool IGFrameActionAddLayer::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Layer id attribute
	return CreateAttribute (IGFRAMEACTION_LAYERPOS, CComVariant (m_nLayerId));
}

bool IGFrameActionAddLayer::WriteData()
{
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pFrame);
	if (!encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerId, m_nLayerPos))
	{
		_ASSERTE (0 && L"Failed encoding layer");
		throw IGEXCEPTION (FrameActionAddLayerException, "WriteData", "EncodeLayer failed");
	}
	return true;
}

bool IGFrameActionAddLayer::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	lLayerIndices.remove (m_nLayerId);
	bool bWorkingLast = (m_pFrame->GetWorkingLayer() == m_pFrame->GetLayer (m_nLayerPos));
	m_pFrame->RemoveCalc (m_nLayerPos, NULL, false);
	if (bWorkingLast)
		m_pFrame->SetWorkingLayer (m_pFrame->GetNbLayers() - 1);
	return true;
}

bool IGFrameActionAddLayer::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	bool bRet = true;
	if (bRedo)
	{
		IGLayer *pNewLayer = new IGLayer (m_pFrame);
		m_pFrame->AddCalc (pNewLayer, m_nLayerPos);
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		if (!encodeIG.DecodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerId, m_nLayerPos))
		{
			_ASSERTE (false && L"IGFrameActionAddLayer::Do - Fatal Error: Decoding layer failed");
			throw IGEXCEPTION (FrameActionAddLayerException, "Do", "DecodeLayer failed");
		}
	}
	else
	{
		// retain the added layer id
		if (m_nLayerId < 0)
		{
			IGLayer *pLayer = m_pFrame->GetLayer (m_nLayerPos);
			if (pLayer)
				m_nLayerId = (int)pLayer->GetId() - 1;
			else
				m_nLayerId = GetMaxId (lLayerIndices, m_pFrame->GetMaxRemovedLayerId());
			// delete layers from m_nLayerId
			CxImageIG encodeIG;
			encodeIG.Ghost (m_pFrame);
			_ASSERTE (m_nLayerId >= 0 && L"IGFrameActionAddLayer::Do - Fatal error: wrong layer id");
			if (m_nLayerId < 0)
				throw IGEXCEPTION (FrameActionAddLayerException, "Do", "wrong layer id");
			encodeIG.RemoveSections (m_pFrame->GetHistoryDataPath(), m_nLayerId, GetMaxId (lSelectionIndices));	
			m_nLayerId++;
		}
	}
	if (m_nLayerPos < (int)lLayerIndices.size())
	{
		list <int>::const_iterator itLayIdx = lLayerIndices.begin();
		for (int idxLayer = 0; 
			(itLayIdx != lLayerIndices.end() && idxLayer < m_nLayerPos);
			idxLayer++)
			++itLayIdx;
		lLayerIndices.insert (itLayIdx, m_nLayerId);
	}
	else
	{
		lLayerIndices.push_back (m_nLayerId);
	}
	return bRet;
}

IGFrameActionChangeLayer::IGFrameActionChangeLayer (CComPtr <IXMLDOMDocument> spXMLDoc, 
													CComPtr <IXMLDOMNode> spXMLNode, 
													IGFrame *pFrame, 
													int nLayerPos,
													int nLayerId,
													int nLayerDestId,
													RECT *p_rcSubLayer,
													int nSelectionId)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, nLayerId, IDB_STEPADDLAYER)
																		, m_nLayerRemovedPos (nLayerPos)
																		, m_nLayerDestId (nLayerDestId)
																		, m_nSelectionId (nSelectionId)
{
	if (p_rcSubLayer)
		::memcpy_s (&m_rcSubLayer, sizeof (RECT), p_rcSubLayer, sizeof (RECT));
	else
	{
		m_rcSubLayer.top = 0;m_rcSubLayer.left = 0;
		m_rcSubLayer.bottom = -1;m_rcSubLayer.right = -1;
	}	
}
	
LPCWSTR IGFrameActionChangeLayer::GetName()
{
	return L"ChangeLayer";
}

bool IGFrameActionChangeLayer::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Layer id attribute
	return CreateAttribute (IGFRAMEACTION_LAYERPOS, CComVariant (m_nLayerId));
}

bool IGFrameActionChangeLayer::WriteData()
{
	// two possible modes : whole layer or a part of it
	if ((m_rcSubLayer.right != -1) && (m_rcSubLayer.bottom != -1))
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		int nWidth = abs (m_rcSubLayer.right - m_rcSubLayer.left) + 1;
		int nHeight = abs (m_rcSubLayer.bottom - m_rcSubLayer.top) + 1;
		m_rcSubLayer.left = (m_rcSubLayer.left < m_rcSubLayer.right) ? m_rcSubLayer.left : m_rcSubLayer.right;
		m_rcSubLayer.top = (m_rcSubLayer.top < m_rcSubLayer.bottom) ? m_rcSubLayer.top : m_rcSubLayer.bottom;
		m_rcSubLayer.right = m_rcSubLayer.left + nWidth -  1;
		m_rcSubLayer.bottom = m_rcSubLayer.top + nHeight - 1;
		if (!encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerDestId, m_nLayerRemovedPos, &m_rcSubLayer, m_nLayerId))
		{
			_ASSERTE (0 && L"Failed encoding layer");
			throw IGEXCEPTION (FrameActionChangeLayerException, "WriteData", "EncodeLayer failed");
		}
	}
	else
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		if (!encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerDestId, m_nLayerRemovedPos))
		{
			_ASSERTE (0 && L"Failed encoding layer");
			throw IGEXCEPTION (FrameActionChangeLayerException, "WriteData", "EncodeLayer failed");
		}
	}
	return true;
}

bool IGFrameActionChangeLayer::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	// two possible modes : whole layer or a part of it
	if ((m_rcSubLayer.right != -1) && (m_rcSubLayer.bottom != -1))
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		// decode original layer and all sub-layers before m_nLayerDestId
		if (!encodeIG.DecodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerId, m_nLayerRemovedPos, &m_rcSubLayer, true, m_nLayerDestId - 1))
		{
			_ASSERTE (false && L"IGFrameActionChangeLayer::Undo - Fatal Error: Decoding layer failed");
			throw IGEXCEPTION (FrameActionChangeLayerException, "Undo", "DecodeLayer failed");
		}
		list <int>::const_iterator itLayIdx = lLayerIndices.begin();
		for (int idxLayer = 0; 
			(itLayIdx != lLayerIndices.end() && idxLayer < m_nLayerRemovedPos);
			idxLayer++)
			++itLayIdx;
		lLayerIndices.insert (itLayIdx, m_nLayerId);
		lLayerIndices.remove (m_nLayerDestId);	
		m_pFrame->RemoveRemovedLayer (m_nLayerId);
	}
	else
	{
		// undo remove operation must be performed first because 0 layer frame is forbidden
		IGFrameActionRemoveLayer actionRemove (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerRemovedPos + 1, m_nLayerId, m_nSelectionId);
		actionRemove.Undo (lLayerIndices, lSelectionIndices);	
		IGFrameActionAddLayer actionAdd (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerRemovedPos, m_nLayerDestId);
		actionAdd.Undo (lLayerIndices, lSelectionIndices);
	}
	m_pFrame->ReplayAllSelections (m_nLayerRemovedPos);		
	return true;
}

bool IGFrameActionChangeLayer::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	// two possible modes : whole layer or a part of it
	if (bRedo && (m_rcSubLayer.right != -1) && (m_rcSubLayer.bottom != -1))
	{
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		if (!encodeIG.DecodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerDestId, m_nLayerRemovedPos, &m_rcSubLayer))
		{
			_ASSERTE (false && L"IGFrameActionChangeLayer::Do - Fatal Error: Decoding layer failed");
			throw IGEXCEPTION (FrameActionChangeLayerException, "Do", "DecodeLayer failed");
		}
		list <int>::const_iterator itLayIdx = lLayerIndices.begin();
		for (int idxLayer = 0; 
			(itLayIdx != lLayerIndices.end() && idxLayer < m_nLayerRemovedPos);
			idxLayer++)
			++itLayIdx;
		lLayerIndices.insert (itLayIdx, m_nLayerDestId);
		lLayerIndices.remove (m_nLayerId);
	}
	else
	{
		// regular case (e.g. filter frame, move pixels)
		// layer ids are specified in case of merge
		if ((m_nLayerId < 0) || (m_nLayerDestId < 0))
		{
			// set the added and removed layer ids, for undo processing
			m_nLayerId = (int)m_pFrame->GetLayer (m_nLayerRemovedPos)->GetId();
			m_nLayerDestId = GetMaxId (lLayerIndices, m_pFrame->GetMaxRemovedLayerId());
			// delete layers from m_nLayerDestId
			CxImageIG encodeIG;
			encodeIG.Ghost (m_pFrame);
			_ASSERTE (m_nLayerDestId >= 0 && L"IGFrameActionAddLayer::Do - Fatal error: wrong layer id");
			if (m_nLayerDestId < 0)
				throw IGEXCEPTION (FrameActionChangeLayerException, "Do", "wrong layer id");
			encodeIG.RemoveSections (m_pFrame->GetHistoryDataPath(), m_nLayerDestId, GetMaxId (lSelectionIndices));		
			m_nLayerDestId++;
		}
		// add operation must be performed first because 0 layer frame is forbidden
		IGFrameActionAddLayer actionAdd (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerRemovedPos + 1, m_nLayerDestId);
		actionAdd.Do (lLayerIndices, lSelectionIndices, bRedo);
		IGFrameActionRemoveLayer actionRemove (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerRemovedPos, m_nLayerId);
		actionRemove.Do (lLayerIndices, lSelectionIndices, bRedo);
		m_nSelectionId = actionRemove.GetSelectionId();
		m_pFrame->GetLayer (m_nLayerRemovedPos)->SetId (m_nLayerDestId);
	}
	return true;
}

IGFrameActionMoveLayer::IGFrameActionMoveLayer (CComPtr <IXMLDOMDocument> spXMLDoc, 
												CComPtr <IXMLDOMNode> spXMLNode, 
												IGFrame *pFrame,
												int nLayerPosFrom,
												int nLayerPosDest)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, nLayerPosFrom, IDB_STEPMOVELAYER)
																										, m_nLayerDestId (nLayerPosDest)
{
}
	
LPCWSTR IGFrameActionMoveLayer::GetName()
{
	return L"MoveLayer";
}

bool IGFrameActionMoveLayer::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Layer from id attribute
	if (!CreateAttribute (IGFRAMEACTIONMOVELAYER_LAYERFROM, CComVariant (m_nLayerId)))
		return false;
	// Layer dest id attribute
	return CreateAttribute (IGFRAMEACTIONMOVELAYER_LAYERDEST, CComVariant (m_nLayerDestId));
}

bool IGFrameActionMoveLayer::WriteData()
{
	// nothing to do
	return true;
}

bool IGFrameActionMoveLayer::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	list <int>::const_iterator itLayIdx = lLayerIndices.begin();
	int idxLayer = 0;
	for (; (itLayIdx != lLayerIndices.end()) && (idxLayer < m_nLayerDestId); idxLayer++)
		++itLayIdx;
	int nRemovedLayerId = *itLayIdx;
	lLayerIndices.remove (nRemovedLayerId);
	IGLayer *pMoveLayer;
	m_pFrame->RemoveCalc (m_nLayerDestId, &pMoveLayer, false);
	itLayIdx = lLayerIndices.begin();
	for (idxLayer = 0; (itLayIdx != lLayerIndices.end()) && (idxLayer < m_nLayerId); idxLayer++)
		++itLayIdx;
	lLayerIndices.insert (itLayIdx, nRemovedLayerId);
	m_pFrame->AddCalc (pMoveLayer, m_nLayerId);
	return true;
}

bool IGFrameActionMoveLayer::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	list <int>::const_iterator itLayIdx = lLayerIndices.begin();
	int idxLayer = 0;
	for (; (itLayIdx != lLayerIndices.end()) && (idxLayer < m_nLayerId); idxLayer++)
		++itLayIdx;
	int nRemovedLayerId = *itLayIdx;
	lLayerIndices.remove (nRemovedLayerId);
	IGLayer *pMoveLayer;
	if (bRedo)
		m_pFrame->RemoveCalc (m_nLayerId, &pMoveLayer, false);
	itLayIdx = lLayerIndices.begin();
	for (idxLayer = 0; (itLayIdx != lLayerIndices.end()) && (idxLayer < m_nLayerDestId); idxLayer++)
		++itLayIdx;
	lLayerIndices.insert (itLayIdx, nRemovedLayerId);
	if (bRedo)
		m_pFrame->AddCalc (pMoveLayer, m_nLayerDestId);
	return true;
}

IGFrameActionMove::IGFrameActionMove (CComPtr <IXMLDOMDocument> spXMLDoc, 
												CComPtr <IXMLDOMNode> spXMLNode, 
												IGFrame *pFrame,
												int nLayerPos,
												const POINT& ptVector)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, nLayerPos, IDB_STEPMOVE)
																										, m_ptVector (ptVector)
{
}
	
LPCWSTR IGFrameActionMove::GetName()
{
	return L"Move";
}

bool IGFrameActionMove::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Layer id attribute
	return CreateAttribute (IGFRAMEACTION_LAYERPOS, CComVariant (m_nLayerId));
}

bool IGFrameActionMove::WriteData()
{
	// nothing to do
	return true;
}

bool IGFrameActionMove::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	IGLayer *pCurLayer = m_pFrame->GetLayer (m_nLayerId);
	if (!pCurLayer)
		throw IGEXCEPTION (FrameActionMoveException, "Undo", "wrong layer id");
	pCurLayer->Move (-m_ptVector.x, m_ptVector.y);
	return true;
}

bool IGFrameActionMove::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{	
	if (bRedo) {
		IGLayer *pCurLayer = m_pFrame->GetLayer (m_nLayerId);
		if (!pCurLayer)
			throw IGEXCEPTION (FrameActionMoveException, "Do", "wrong layer id");
		pCurLayer->Move (m_ptVector.x, -m_ptVector.y);
	}
	return true;
}

IGFrameActionMovePixels::IGFrameActionMovePixels (CComPtr <IXMLDOMDocument> spXMLDoc, 
												CComPtr <IXMLDOMNode> spXMLNode, 
												IGFrame *pFrame,
												int nLayerPos,
												const POINT& ptVector,
												const RECT& rcSubLayer)	: IGFrameActionMove (spXMLDoc, spXMLNode, pFrame, nLayerPos, ptVector)
																										, m_nLayerPos (nLayerPos)
																										, m_rcSubLayer (rcSubLayer)
																										, m_nLayerDestId (-1)
																										, m_nSelectionId (-1)
{
}

bool IGFrameActionMovePixels::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Layer id attribute
	return CreateAttribute (IGFRAMEACTION_LAYERPOS, CComVariant (m_nLayerId));
}

bool IGFrameActionMovePixels::WriteData()
{
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pFrame);	
	int nWidth = m_rcSubLayer.right - m_rcSubLayer.left + 1;
	int nHeight = m_rcSubLayer.bottom - m_rcSubLayer.top + 1;
	nWidth = (nWidth >= 0) ? nWidth : -1 * nWidth;
	nHeight = (nHeight >= 0) ? nHeight : -1 * nHeight;
	m_rcSubLayer.left = (m_rcSubLayer.left < m_rcSubLayer.right) ? m_rcSubLayer.left : m_rcSubLayer.right;
	m_rcSubLayer.top = (m_rcSubLayer.top < m_rcSubLayer.bottom) ? m_rcSubLayer.top : m_rcSubLayer.bottom;
	m_rcSubLayer.right = m_rcSubLayer.left + nWidth - 1;
	m_rcSubLayer.bottom = m_rcSubLayer.top + nHeight - 1;
	if (!encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerDestId, m_nLayerPos, &m_rcSubLayer, m_nLayerId))
	{
		_ASSERTE (0 && L"Failed encoding layer");
		throw IGEXCEPTION (FrameActionMovePixelsException, "WriteData", "EncodeLayer failed");
	}
	
	RECT rcSelection = m_rcSubLayer;
	IGLayer *pLayer = m_pFrame->GetLayer (m_nLayerPos);
	rcSelection.top = pLayer->GetHeight() - 1 - m_rcSubLayer.bottom;
	rcSelection.bottom = pLayer->GetHeight() - 1 - m_rcSubLayer.top;
	if (!encodeIG.EncodeSelection (m_pFrame->GetHistoryDataPath(), m_nSelectionId, m_nLayerPos, rcSelection)){
		_ASSERTE (0 && L"Failed encoding selection");
		throw IGEXCEPTION (FrameActionMovePixelsException, "WriteData", "EncodeSelection failed");
	}
	return true;
}

bool IGFrameActionMovePixels::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	IGFrameActionChangeSelection actionChangeSelection (m_spXMLDoc, m_spXMLNodeStep,
																m_pFrame, m_rcSubLayer,
																(IGSELECTIONENUM)(IGSELECTION_REPLACE | IGSELECTION_SQUARE),
																m_nSelectionId,
																m_nLayerPos);
	actionChangeSelection.Undo (lLayerIndices, lSelectionIndices);

	IGFrameActionChangeLayer actionChange (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerPos, m_nLayerId, m_nLayerDestId, &m_rcSubLayer, m_nSelectionId);
	actionChange.Undo (lLayerIndices, lSelectionIndices);
	return true;
}

bool IGFrameActionMovePixels::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{	
	IGLayer *pLayer = m_pFrame->GetLayer (m_nLayerPos);
	if ((bRedo && (m_rcSubLayer.left >= pLayer->GetWidth() - 1 || 
		m_rcSubLayer.right <= 0 ||
		m_rcSubLayer.top >= pLayer->GetHeight() - 1 || 
		m_rcSubLayer.bottom <= 0)) || 
		(!bRedo && (m_rcSubLayer.left >= pLayer->GetWidth() - 1 || 
		m_rcSubLayer.right <= 0 ||
		m_rcSubLayer.bottom >= pLayer->GetHeight() - 1 || 
		m_rcSubLayer.top <= 0)))
		throw IGEXCEPTION (FrameActionMovePixelsException, "Do", "wrong selection area");

	IGFrameActionChangeLayer actionChange (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerPos, m_nLayerId, m_nLayerDestId, &m_rcSubLayer);
	actionChange.Do (lLayerIndices, lSelectionIndices, bRedo);
	
	IGFrameActionChangeSelection actionChangeSelection (m_spXMLDoc, m_spXMLNodeStep,
																m_pFrame, m_rcSubLayer,
																(IGSELECTIONENUM)(IGSELECTION_REPLACE | IGSELECTION_SQUARE),
																m_nSelectionId,
																m_nLayerPos);
	actionChangeSelection.Do (lLayerIndices, lSelectionIndices, bRedo);
	if (!bRedo) {
		// layer and selection ids have to be saved for undo/redo
		m_nLayerId = actionChange.GetLayerId();
		m_nLayerDestId = actionChange.GetLayerDestId();
		m_nSelectionId = actionChangeSelection.GetSelectionId();
	}
	return true;
}


IGFrameActionRotateAndResize::IGFrameActionRotateAndResize (CComPtr <IXMLDOMDocument> spXMLDoc, 
												CComPtr <IXMLDOMNode> spXMLNode, 
												IGFrame *pFrame,
												int nLayerPos,
												const POINT& ptVector,
												float fRate,												
												const RECT& rcSubLayer)	: IGFrameActionMovePixels (spXMLDoc, spXMLNode, pFrame, nLayerPos, ptVector, rcSubLayer)
																		, m_fRate (fRate)
{
	m_bFullSelection = (!m_pFrame->GetLayer (m_nLayerPos)->SelectionIsValid());
}

LPCWSTR IGFrameActionRotateAndResize::GetName()
{
	return L"RotateAndResize";
}

bool IGFrameActionRotateAndResize::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// Layer id attribute
	return CreateAttribute (IGFRAMEACTION_LAYERPOS, CComVariant (m_nLayerId));
}

bool IGFrameActionRotateAndResize::WriteData()
{
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pFrame);	
	int nWidth = m_rcSubLayer.right - m_rcSubLayer.left + 1;
	int nHeight = m_rcSubLayer.bottom - m_rcSubLayer.top + 1;
	nWidth = (nWidth >= 0) ? nWidth : -1 * nWidth;
	nHeight = (nHeight >= 0) ? nHeight : -1 * nHeight;
	m_rcSubLayer.left = (m_rcSubLayer.left < m_rcSubLayer.right) ? m_rcSubLayer.left : m_rcSubLayer.right;
	m_rcSubLayer.top = (m_rcSubLayer.top < m_rcSubLayer.bottom) ? m_rcSubLayer.top : m_rcSubLayer.bottom;
	m_rcSubLayer.right = m_rcSubLayer.left + nWidth - 1;
	m_rcSubLayer.bottom = m_rcSubLayer.top + nHeight - 1;
	if (!encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerDestId, m_nLayerPos, &m_rcSubLayer, m_nLayerId))
	{
		_ASSERTE (0 && L"Failed encoding layer");
		throw IGEXCEPTION (FrameActionMovePixelsException, "WriteData", "EncodeLayer failed");
	}
	if (!m_bFullSelection){
		RECT rcSelection = m_rcSubLayer;
		IGLayer *pLayer = m_pFrame->GetLayer (m_nLayerPos);
		rcSelection.top = pLayer->GetHeight() - 1 - m_rcSubLayer.bottom;
		rcSelection.bottom = pLayer->GetHeight() - 1 - m_rcSubLayer.top;
		if (!encodeIG.EncodeSelection (m_pFrame->GetHistoryDataPath(), m_nSelectionId, m_nLayerPos, rcSelection)){
			_ASSERTE (0 && L"Failed encoding selection");
			throw IGEXCEPTION (FrameActionMovePixelsException, "WriteData", "EncodeSelection failed");
		}
	}
	return true;
}

bool IGFrameActionRotateAndResize::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	if (!m_bFullSelection){
		IGFrameActionChangeSelection actionChangeSelection (m_spXMLDoc, m_spXMLNodeStep,
																	m_pFrame, m_rcSubLayer,
																	(IGSELECTIONENUM)(IGSELECTION_REPLACE | IGSELECTION_SQUARE),
																	m_nSelectionId,
																	m_nLayerPos);
		actionChangeSelection.Undo (lLayerIndices, lSelectionIndices);
	}
	IGFrameActionChangeLayer actionChange (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerPos, m_nLayerId, m_nLayerDestId, &m_rcSubLayer, m_nSelectionId);
	actionChange.Undo (lLayerIndices, lSelectionIndices);
	return true;
}

bool IGFrameActionRotateAndResize::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{	
	IGLayer *pLayer = m_pFrame->GetLayer (m_nLayerPos);
	if ((bRedo && (m_rcSubLayer.left >= pLayer->GetWidth() - 1 || 
		m_rcSubLayer.right <= 0 ||
		m_rcSubLayer.top >= pLayer->GetHeight() - 1 || 
		m_rcSubLayer.bottom <= 0)) || 
		(!bRedo && (m_rcSubLayer.left >= pLayer->GetWidth() - 1 || 
		m_rcSubLayer.right <= 0 ||
		m_rcSubLayer.bottom >= pLayer->GetHeight() - 1 || 
		m_rcSubLayer.top <= 0)))
		throw IGEXCEPTION (FrameActionMovePixelsException, "Do", "wrong selection area");

	IGFrameActionChangeLayer actionChange (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_nLayerPos, m_nLayerId, m_nLayerDestId, &m_rcSubLayer);
	actionChange.Do (lLayerIndices, lSelectionIndices, bRedo);
	if (!bRedo) {
		// layer ids have to be saved for undo/redo
		m_nLayerId = actionChange.GetLayerId();
		m_nLayerDestId = actionChange.GetLayerDestId();
	}
	if (!m_bFullSelection){
		IGFrameActionChangeSelection actionChangeSelection (m_spXMLDoc, m_spXMLNodeStep,
																	m_pFrame, m_rcSubLayer,
																	(IGSELECTIONENUM)(IGSELECTION_REPLACE | IGSELECTION_SQUARE),
																	m_nSelectionId,
																	m_nLayerPos);
		actionChangeSelection.Do (lLayerIndices, lSelectionIndices, bRedo);
		if (!bRedo) // selection id has to be saved for undo/redo
			m_nSelectionId = actionChangeSelection.GetSelectionId();
	}	
	return true;
}

IGFrameActionRemoveLayer::IGFrameActionRemoveLayer (CComPtr <IXMLDOMDocument> spXMLDoc, 
													CComPtr <IXMLDOMNode> spXMLNode, 
													IGFrame *pFrame,
													int nLayerPos,
													int nLayerId,
													int nSelectionId)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, nLayerId, IDB_STEPREMOVELAYER)
																	, m_nLayerPos (nLayerPos)
																	, m_nSelectionId (nSelectionId)
{
}
	
LPCWSTR IGFrameActionRemoveLayer::GetName()
{
	return L"RemoveLayer";
}

bool IGFrameActionRemoveLayer::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// write layer pos attribute
	if (!CreateAttribute (IGFRAMEACTION_LAYERPOS, CComVariant (m_nLayerPos)))
		return false;
	// Layer id attribute
	return CreateAttribute (IGFRAMEACTION_LAYERID, CComVariant (m_nLayerId));
}

bool IGFrameActionRemoveLayer::WriteData()
{
	// nothing to do
	return true;
}

bool IGFrameActionRemoveLayer::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	list <int>::const_iterator itLayIdx = lLayerIndices.begin();
	for (int idxLayer = 0; 
		(itLayIdx != lLayerIndices.end() && idxLayer < m_nLayerPos);
		idxLayer++)
		++itLayIdx;
	lLayerIndices.insert (itLayIdx, m_nLayerId);
	IGLayer *pRemovedLayer = new IGLayer (m_pFrame);
	m_pFrame->AddCalc (pRemovedLayer, m_nLayerPos);
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pFrame);
	if (!encodeIG.DecodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerId, m_nLayerPos, NULL, true))
	{
		_ASSERTE (false && L"IGFrameActionRemoveLayer::Undo - Fatal Error: Decoding layer failed");
		throw IGEXCEPTION (FrameActionRemoveLayerException, "Undo", "DecodeLayer failed");
	}
	if (m_nSelectionId >= 0)
	{
		RECT rcSel = {0,0,-1,-1};
		IGFrameActionChangeSelection actionClearSelection (m_spXMLDoc, m_spXMLNodeStep,
															m_pFrame,
															rcSel,
															IGSELECTION_CLEAR,
															m_nSelectionId,
															m_nLayerPos);
		actionClearSelection.Undo (lLayerIndices, lSelectionIndices);
	}
	m_pFrame->RemoveRemovedLayer (m_nLayerId);
	return true;
}

bool IGFrameActionRemoveLayer::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{	
	if (bRedo)
	{
		if (m_nSelectionId >= 0)
		{
			RECT rcSel = {0,0,-1,-1};
			IGFrameActionChangeSelection actionClearSelection (m_spXMLDoc, m_spXMLNodeStep,
																m_pFrame,
																rcSel,
																IGSELECTION_CLEAR,
																m_nSelectionId,
																m_nLayerPos);
			actionClearSelection.Do (lLayerIndices, lSelectionIndices, bRedo);
		}
		bool bWorkingLast = (m_pFrame->GetWorkingLayer() == m_pFrame->GetLayer (m_nLayerPos));
		m_pFrame->RemoveCalc (m_nLayerPos, NULL, false);
		if (bWorkingLast)
			m_pFrame->SetWorkingLayer (m_pFrame->GetNbLayers() - 1);
	}
	else 
	{
		if (m_nLayerId < 0)
		{
			if (m_nLayerPos >= m_pFrame->GetNbLayers())
				throw IGEXCEPTION (FrameActionRemoveLayerException, "Do", "wrong layer position");
			// retrieve the removed layer id
			m_nLayerId = (int)m_pFrame->GetLayer (m_nLayerPos)->GetId();
		}
		// clear selection if removed layer owns selection
		if (m_pFrame->GetLayer (m_nLayerPos) == m_pFrame->GetSelectionLayer())
		{
			RECT rcSel = {0,0,-1,-1};
			IGFrameActionChangeSelection actionClearSelection (m_spXMLDoc, m_spXMLNodeStep,
																m_pFrame,
																rcSel,
																IGSELECTION_CLEAR,
																m_nLayerPos);
			actionClearSelection.Do (lLayerIndices, lSelectionIndices, bRedo);
			m_nSelectionId = actionClearSelection.GetSelectionId();
		}
	}
	lLayerIndices.remove (m_nLayerId);
	m_pFrame->AddRemovedLayer (m_nLayerId);
	return true;
}

IGFrameActionMergeLayer::IGFrameActionMergeLayer (CComPtr <IXMLDOMDocument> spXMLDoc, 
													CComPtr <IXMLDOMNode> spXMLNode, 
													IGFrame *pFrame,
													int nNbLayers,
													int *pnLayerPositions,
													int nLayerId,
													int nLayerNewId)	: IGFrameAction (spXMLDoc, spXMLNode, pFrame, nLayerId, IDB_STEPMERGELAYERS)
																		, m_nNbLayers (nNbLayers)
																		, m_pnLayerPositions (pnLayerPositions)
																		, m_nLayerNewId (nLayerNewId)
																		, m_nWorkingLayerPos (-1)
																		, m_nSelectionId (-1)
																		, m_nSelectionPos (-1)
{
	// first layer is not removed, it is changed, as it is the merging destination
	m_pnUpdatedLayerPositions = new int[nNbLayers];
	::memcpy_s (m_pnUpdatedLayerPositions, sizeof(int) * nNbLayers, m_pnLayerPositions, sizeof(int) * nNbLayers);
	m_pnRemovedLayerIds = new int[nNbLayers - 1];
	ZeroMemory (m_pnRemovedLayerIds, (nNbLayers - 1) * sizeof(int));
}

IGFrameActionMergeLayer::~IGFrameActionMergeLayer()
{
	// frame action is responsible for m_pnUpdatedLayerPositions deletion
	delete [] m_pnLayerPositions;
	delete [] m_pnUpdatedLayerPositions;
	delete [] m_pnRemovedLayerIds;
}
	
LPCWSTR IGFrameActionMergeLayer::GetName()
{
	return L"MergeLayer";
}

bool IGFrameActionMergeLayer::WriteXml()
{
	if (!IGFrameAction::WriteXml() || !m_spXMLNodeStepValue)
		return false;
	// write nb layers attribute
	if (!CreateAttribute (IGFRAMEACTION_NBLAYERS, CComVariant (m_nNbLayers)))
		return false;

	for (int idxLayer = 0; idxLayer < m_nNbLayers; idxLayer++)
	{
		// Layer id attribute
		if (!CreateAttribute (IGFRAMEACTION_LAYERID, CComVariant (m_pnUpdatedLayerPositions [idxLayer])))
			return false;
	}
	return true;
}

bool IGFrameActionMergeLayer::WriteData()
{
	CxImageIG encodeIG;
	encodeIG.Ghost (m_pFrame);
	if (!encodeIG.EncodeLayer (m_pFrame->GetHistoryDataPath(), m_nLayerNewId, m_pnUpdatedLayerPositions[0]))
	{
		_ASSERTE (0 && L"Failed encoding layer");
		throw IGEXCEPTION (FrameActionMergeLayerException, "WriteData", "EncodeLayer failed");
	}
	return true;
}

bool IGFrameActionMergeLayer::Undo (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices)
{
	int nIdxSelection = -1;
	for (int idxLayer = 0; idxLayer < m_nNbLayers; idxLayer++)
	{
		if (m_nSelectionPos == m_pnLayerPositions [idxLayer])
			nIdxSelection = idxLayer;
	}
	// restore the merged layer
	IGFrameActionChangeLayer actionChange (m_spXMLDoc,
											m_spXMLNodeStep,
											m_pFrame,
											m_pnUpdatedLayerPositions[0],
											m_nLayerId,
											m_nLayerNewId,
											NULL,
											(nIdxSelection == 0) ? m_nSelectionId : -1);
	actionChange.Undo (lLayerIndices, lSelectionIndices);
	// restore the removed layers
	for (int idxLayer = m_nNbLayers - 1; idxLayer > 0; idxLayer--)
	{
		IGFrameActionRemoveLayer actionRemove (m_spXMLDoc, m_spXMLNodeStep,
												m_pFrame,
												m_pnUpdatedLayerPositions [idxLayer],
												m_pnRemovedLayerIds [idxLayer - 1],
												(nIdxSelection == idxLayer) ? m_nSelectionId : -1);
		actionRemove.Undo (lLayerIndices, lSelectionIndices);
		// update next relative position values
		for (int idxUpdatedLayer = idxLayer + 1; idxUpdatedLayer < m_nNbLayers; idxUpdatedLayer++)
			m_pnUpdatedLayerPositions [idxUpdatedLayer]++;
	}
	m_pFrame->SetWorkingLayer (m_nWorkingLayerPos);
	return true;
}

bool IGFrameActionMergeLayer::Do (list <int>& lLayerIndices, list <pair <int, bool>>& lSelectionIndices, bool bRedo)
{
	if (!bRedo)
	{
		// retain the changed layer id for undo processing
		list <int>::const_iterator itLayIdx;
		if (m_nLayerId < 0)
			m_nLayerId = (int)m_pFrame->GetLayer (m_pnUpdatedLayerPositions[0])->GetId();
		// retain the removed layer ids for undo processing
		itLayIdx = lLayerIndices.begin();
		int idxMergedLayer = 1;
		for (int idxLayer = 0; itLayIdx != lLayerIndices.end() && idxMergedLayer < m_nNbLayers; idxLayer++, ++itLayIdx)
		{
			if (idxLayer == m_pnUpdatedLayerPositions[idxMergedLayer])
			{				
				m_pnRemovedLayerIds [idxMergedLayer - 1] = *itLayIdx;
				idxMergedLayer++;
			}
		}
		// retain the new layer id for undo processing
		m_nLayerNewId = GetMaxId (lLayerIndices, m_pFrame->GetMaxRemovedLayerId());
		// delete layers from m_nLayerNewId
		CxImageIG encodeIG;
		encodeIG.Ghost (m_pFrame);
		_ASSERTE (m_nLayerNewId >= 0 && L"IGFrameActionMergeLayer::Do - Fatal error: wrong layer id");
		if (m_nLayerNewId < 0)
			throw IGEXCEPTION (FrameActionMergeLayerException, "Do", "wrong layer id");
		encodeIG.RemoveSections (m_pFrame->GetHistoryDataPath(), m_nLayerNewId, GetMaxId (lSelectionIndices));	
		m_nLayerNewId++;
		m_nWorkingLayerPos = m_pFrame->GetLayerPos (m_pFrame->GetWorkingLayer());
		if (m_pFrame->IsSelection())
			m_nSelectionPos = m_pFrame->GetLayerPos (m_pFrame->GetSelectionLayer());
	}
	// remove the merged layer ids
	for (int idxLayer = 1; idxLayer < m_nNbLayers; idxLayer++)
	{
		int nLayerPos = m_pFrame->GetLayerPos (m_pFrame->GetLayerFromId (m_pnRemovedLayerIds [idxLayer - 1]));
		IGFrameActionRemoveLayer actionRemove (m_spXMLDoc, m_spXMLNodeStep,
												m_pFrame,
												nLayerPos,
												m_pnRemovedLayerIds [idxLayer - 1]);
		actionRemove.Do (lLayerIndices, lSelectionIndices, bRedo);
		// if a selection is part of a removed layer, retain it for undoing process
		if (!bRedo && (nLayerPos == m_nSelectionPos))
			m_nSelectionId = actionRemove.GetSelectionId();
		// update next relative position values
		for (int idxUpdatedLayer = idxLayer + 1; idxUpdatedLayer < m_nNbLayers; idxUpdatedLayer++)
			m_pnUpdatedLayerPositions [idxUpdatedLayer]--;
	}
	// swap the merging destination layer by the merge result
	IGFrameActionChangeLayer actionChange (m_spXMLDoc, m_spXMLNodeStep, m_pFrame, m_pnUpdatedLayerPositions[0], m_nLayerId, m_nLayerNewId);
	actionChange.Do (lLayerIndices, lSelectionIndices, bRedo);
	if (!bRedo && (m_nSelectionPos == m_pnUpdatedLayerPositions[0]))
		m_nSelectionId = actionChange.GetSelectionId();
	return true;
}