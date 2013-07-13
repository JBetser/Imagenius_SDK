#pragma once
#include <string>
#include <list>
#include <vector>
#include <msxml2.h>

namespace IGLibrary
{
class IGFrame;

typedef enum {	IGSELECTION_CLEAR = 1,
				IGSELECTION_SQUARE = 2,
				IGSELECTION_LASSO = 4,
				IGSELECTION_ADD = 8,
				IGSELECTION_REMOVE = 16,
				IGSELECTION_REPLACE = 32,
				IGSELECTION_MAGIC = 64,
				IGSELECTION_LPE = 128,
				IGSELECTION_FACES = 256,
				IGSELECTION_INVERT = 513,
				IGSELECTION_EYES = 1024,
				IGSELECTION_MOUTH = 2048,
				IGSELECTION_NOZE = 4096 }	IGSELECTIONENUM;

class IGFrameAction
{
public:
	IGFrameAction (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerId = -1, int nResIconId = -1);
	virtual ~IGFrameAction();

	virtual bool WriteXml();
	virtual bool WriteData() = 0;
	virtual LPCWSTR GetName() = 0;
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo) = 0;
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices) = 0;

	int GetLayerId() const;
	int GetResIconId() const;
	static int GetMaxId (const std::list <std::pair <int, bool>>& lIndices);
	static int GetMaxId (const std::list <int>& lIndices, int nMaxRemovedLayerId);

protected:
	bool CreateAttribute (const wchar_t *pcwAttName, VARIANT varAttValue);
	bool SetFrameSize (int nWidth, int nHeight);

	int							m_nLayerId;
	int							m_nResIconId;
	IGFrame						*m_pFrame;
	CComPtr <IXMLDOMNode>		m_spXMLNodeStepValue;
	CComPtr <IXMLDOMNode>		m_spXMLNodeStep;
	CComPtr <IXMLDOMDocument>	m_spXMLDoc;
};

class IGFrameActionStart : public IGFrameAction
{
public:
	IGFrameActionStart (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
};

class IGFrameActionChangeSelection : public IGFrameAction
{
public:
	IGFrameActionChangeSelection (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, const RECT& rcSelection, IGSELECTIONENUM eType, int nSelectionId = -1, int nLayerPos = -1);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

	int GetSelectionId() const {return m_nSelectionId;}
private:
	RECT	m_rcSelection;
	int		m_nSelectionId;
	int		m_nLayerPos;
	IGSELECTIONENUM m_eType;
};

class IGFrameActionChangeSize : public IGFrameAction
{
public:
	IGFrameActionChangeSize (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, const POINT& ptSize, bool bStretch);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
private:
	POINT	m_ptOldSize, m_ptNewSize;
	bool	m_bStretch;
	std::vector <std::pair <int, int>> m_vMapIndices;
};

class IGFrameActionAddLayer : public IGFrameAction
{
public:
	// nLayerId param is used for actions that use change layer action
	// in such cases layer id must be speficied the for undo processing
	IGFrameActionAddLayer (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPos, int nLayerId = -1);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

protected:
	int m_nLayerPos;
};

class IGFrameActionChangeLayer : public IGFrameAction
{
public:
	// nLayerId and nLayerDestId param is used for actions that use change layer action
	// in such cases dest id must be speficied the for undo processing
	IGFrameActionChangeLayer (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPos, int nLayerId = -1, int nLayerDestId = -1, RECT *p_rcSubLayer = NULL, int nSelectionId = -1);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

	int GetSelectionId() const {return m_nSelectionId;}
	int GetLayerDestId() const {return m_nLayerDestId;}
protected:	
	int m_nLayerDestId;
	int m_nLayerRemovedPos;
	RECT m_rcSubLayer;
	int m_nSelectionId;
};

class IGFrameActionMoveLayer : public IGFrameAction
{
public:
	IGFrameActionMoveLayer (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPosFrom, int nLayerPosDest);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

protected:
	int m_nLayerDestId;	// for this action, the layers id are actually the 
						// positions in the array
};

class IGFrameActionRemoveLayer : public IGFrameAction
{
public:	// only the removed position is set. the layer id is deduced if not set, and used for undo processing.
	IGFrameActionRemoveLayer (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPos, int nLayerId = -1, int nSelectionId = -1);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

	int GetSelectionId() const {return m_nSelectionId;}
protected:
	int m_nLayerPos;
	int	m_nSelectionId;
};

class IGFrameActionMergeLayer : public IGFrameAction
{
public:	// only the removed position is set. the layer id is not used.
	IGFrameActionMergeLayer (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nNbLayers, int *pnLayerPositions, int nLayerId = -1, int nLayerNewId = -1);
	virtual ~IGFrameActionMergeLayer();
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

protected:
	int m_nNbLayers;
	int m_nLayerNewId;	// the dest id of merged layer. used for undo processing
	int m_nWorkingLayerPos;
	int *m_pnLayerPositions;
	int *m_pnUpdatedLayerPositions;
	int *m_pnRemovedLayerIds;
	int m_nSelectionId;
	int m_nSelectionPos;
};

class IGFrameActionMove : public IGFrameAction
{
public:
	IGFrameActionMove (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPos, const POINT& ptVector);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
	POINT m_ptVector;
};

class IGFrameActionMovePixels : public IGFrameActionMove
{
public:
	IGFrameActionMovePixels (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPos, const POINT& ptVector, const RECT& p_rcSubLayer);
	
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);

protected:
	int m_nLayerPos;
	int m_nLayerDestId;
	RECT m_rcSubLayer;
	int m_nSelectionId;
};

class IGFrameActionRotateAndResize : public IGFrameActionMovePixels
{
public:
	IGFrameActionRotateAndResize (CComPtr <IXMLDOMDocument> spXMLDoc, CComPtr <IXMLDOMNode> spXMLNode, IGFrame *pFrame, int nLayerPos, const POINT& ptVector, float fRate, const RECT& p_rcSubLayer);
	
	virtual LPCWSTR GetName();
	virtual bool WriteXml();
	virtual bool WriteData();
	virtual bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices, bool bRedo = false);
	virtual bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
	float m_fRate;
	bool m_bFullSelection;
};

}