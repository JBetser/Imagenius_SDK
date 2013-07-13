#pragma once
#include "IGConfigManager.h"
#include "IGSingleton.h"
#include "IGSmartPtr.h"
#include "IGFrameAction.h"
#include "ximathread.h"
#include <set>

#define HISTORY_FILENAME_DATA			L"History.ig"
#define HISTORY_FILENAME_DATA_SWAP		L"HistorySwap.ig"
#define HISTORY_FILENAME_CONFIG			L"History.xml"
#define HISTORY_FILENAME_CONFIG_SWAP	L"HistorySwap.xml"

#ifdef _DEBUG
#define HISTORY_SWAPNBSTEPS			1	// the maximum nb steps for one history file
#else
#define HISTORY_SWAPNBSTEPS			50	// the maximum nb steps for one history file
#endif
#define HISTORY_MAXNBSTEPS			2 * HISTORY_SWAPNBSTEPS	// the total max nb steps

namespace IGLibrary
{
class IGFrame;

typedef enum {	IGFRAMEHISTORY_STEP_UNDEFINED = 0,
				IGFRAMEHISTORY_STEP_START = 1,
				IGFRAMEHISTORY_STEP_SELECTION = 2,
				IGFRAMEHISTORY_STEP_RESIZE = 3,
				IGFRAMEHISTORY_STEP_LAYERCHANGE = 4,
				IGFRAMEHISTORY_STEP_LAYERADD = 5,
				IGFRAMEHISTORY_STEP_LAYERMOVE = 6,
				IGFRAMEHISTORY_STEP_LAYERREMOVE = 7,
				IGFRAMEHISTORY_STEP_LAYERMERGE = 8,
				IGFRAMEHISTORY_STEP_MOVE = 9,
				IGFRAMEHISTORY_STEP_MOVEPIXELS = 10,
				IGFRAMEHISTORY_STEP_ROTATEANDRESIZE = 11}	EnumStepType;

typedef struct structSELECTIONPARAMS {	
					structSELECTIONPARAMS()	: eSelectionType (IGSELECTION_REPLACE)
											, nTolerance (10)
					{
					}
					structSELECTIONPARAMS(IGSELECTIONENUM type)	: eSelectionType (type)
																	, nTolerance (10)
					{
					}
					IGSELECTIONENUM		eSelectionType;
					int		nTolerance;} SELECTIONPARAMS;

// Class representing one step in imagenius history
class IGFrameStep
{
public:
	IGFrameStep::IGFrameStep (CComPtr <IXMLDOMDocument> spXMLDoc);
	IGFrameStep (IGFrame *pFrame,
				CComPtr <IXMLDOMDocument> spXMLDoc,
				CComPtr <IXMLDOMNode> spNodeParent);
	~IGFrameStep();

	bool Initialize (EnumStepType eType,
					int nFrameStepOrder,
					std::wstring wsStepName,
					int nResIconId,
					int nParam1, int nParam2);

	bool Do (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
	bool Redo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
	bool Undo (std::list <int>& lLayerIndices, std::list <std::pair <int, bool>>& lSelectionIndices);
	LPCWSTR GetName() const;
	EnumStepType GetType() const;
	int GetResIconId() const;
	bool Draw (HDC hDC, RECT *pRcStep, bool bSelected, bool bOverSelection);
	void Remove();
	bool Save();
	const IGFrameAction& GetAction() const { return *m_spAction; }

private:	
	EnumStepType					m_eType;
	int								m_nFrameStepOrder;
	std::wstring					m_wsStepName;
	IGSmartPtr <IGFrameAction>		m_spAction;
	CComPtr <IXMLDOMNode>			m_spNodeStep;
	CComPtr <IXMLDOMNode>			m_spNodeParent;
	CComPtr <IXMLDOMDocument>		m_spXMLDoc;
	CxImage							m_cxIcon;
	IGFrame							*m_pFrame;
};

// Class managing imagenius history for one frame
class IGFrameHistory
{
public:
	IGFrameHistory(bool bSwap = false, IGFrame *pThisFrame = NULL);
	virtual ~IGFrameHistory();

	// to allow/avoid history recording
	inline void SetHistoryEnabled (bool bEnabled = true) {m_bHistoryEnabled = bEnabled;}
	
	bool CreateHistory (HINSTANCE hInstance, LPCWSTR pcwHistoryRes, 
					LPCWSTR pcwFileName = NULL, bool bAsync = true,
					EnumStepType eStartStepId = IGFRAMEHISTORY_STEP_START,
					LPCWSTR pcwStartStepName = NULL);
	// history is stored in two files. this methods swaps between files.
	bool SwapHistory ();

	bool RestoreStep (int nIdxRestoreStep);
	bool SaveStep();
	bool CancelSteps (int nIdxStep = -1);
	LPCWSTR	GetHistoryDataPath() const;
	int GetMaxLayerId() const;
	void SetMaxLayerId(int nMaxLayerId);
	int GetMaxSelectionId() const;
	int GetMaxRemovedLayerId() const;
	void AddSelectionId(int nSelectionId, bool bClear);
	void ReplayAllSelections(int nLayerPos);
	void PopulateListBox (HWND hListBox, bool bFillOnly = false);
	bool HistoryDraw (HWND hListBox, int nOffsetWidth, HDC hDC); // offsetwidth is due to scrollbar
	bool HistoryDraw (HWND hListBox, int nOffsetWidth, HDC hDC, int nItemId, int nIdxOffset = 0); // nIdxOffset is due to history file swap
	void GetStepIds (std::wstring& wsStepIds) const;
	int GetNbSteps() const;
	void AddRemovedLayer (int nLayerId);
	void RemoveRemovedLayer (int nLayerId);
	LPCWSTR GetStepName(int nStepId) const;
	const IGFrameAction& GetAction(int nStepId) const;
	void CleanIndexLists (int nMaxLayerId, int nMaxSelectionId);
	int GetCurrentStepId() const { return m_nCurrentStep; }
	
protected:
	// USE CAREFULLY: this method can access directly frame memory
	bool AddNewStep (EnumStepType eStepType, const std::wstring& wsStepName, int nParam1 = -1, int nParam2 = -1, int nResIconId = -1, bool bSaveStep = true, bool bAsync = true);
	
	IGSingleton <IGConfigManager>			m_spConfigMgr;

private:
	std::wstring createTempDirectory();

	std::wstring							m_wsFileName;		// current main History.ig / HistorySwap.ig
	std::wstring							m_wsSwapFileName;	// current alternate History.ig / HistorySwap.ig
	std::wstring							m_wsConfigFileName; // current main History.xml / HistorySwap.xml
	std::wstring							m_wsConfigSwapFileName; // current alternate History.xml / HistorySwap.xml
	IGFrame									*m_pThisFrame;
	CComPtr <IXMLDOMDocument>				m_spXMLDoc;
	CComPtr <IXMLDOMNode>					m_spXMLNodeSteps;
	int										m_nCurrentStep;
	int										m_nLastStep;
	std::list <IGSmartPtr <IGFrameStep>>	m_lspFrameSteps;
	std::list <int>							m_lLayerIndices;		// current layer state
	std::list <std::pair <int, bool>>		m_lSelectionIndices;	// current selection state
	HWND									m_hListBox;
	IGSmartPtr <IGFrameHistory>				m_spSwapHistory;
	HINSTANCE								m_hInstance;
	LPCWSTR									m_pcwHistoryRes;
	bool									m_bSwap;
	bool									m_bHistoryEnabled; 
	std::set <int>							m_setRemovedLayers;	// necessary to compute next layer id
};
}