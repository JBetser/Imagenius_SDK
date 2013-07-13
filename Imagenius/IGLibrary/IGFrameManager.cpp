#include "StdAfx.h"
#include "ximage.h"
#include "IGSplashWindow.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGFrameManager.h"
#include <string>
#include <limits>
#include <math.h>

#undef max	// resolves conflict between std::numeric_limits and WinDef.h

using namespace IGLibrary;

unsigned int IGFrameManager::g_nNbMaxFrames = 10;

IGFrameManager::IGFrameManager()	: m_nMultiFrameWidth (0)
									, m_nMultiFrameHeight (0)
									, m_bFrameSelected (false)
{
	::ZeroMemory (&m_rcMultiFrame, sizeof (RECT));
}

IGFrameManager::~IGFrameManager()
{
}

bool IGFrameManager::AddFrame (IGSmartPtr <IGFrame> &spFrame)
{
	if (!spFrame)
	{
		_ASSERTE (FALSE && L"FrameMgr Error: NULL frame");
		return false;
	}
	if (m_lspFrames.size() >= g_nNbMaxFrames)
		return false;	// max number of frames reached !
	UnMaximize ();
	addFrame (spFrame);
	return true;
}

void IGFrameManager::addFrame (IGSmartPtr <IGFrame> &spFrame)
{
	spFrame->SetOrder (m_lspFrames.size());
	spFrame->SetMultiFrameSize (m_nMultiFrameWidth, m_nMultiFrameHeight);	
	insertFrame (spFrame);
	updateFrameSize();
	spFrame->ZoomToFrame();
}

void IGFrameManager::SetMultiFrameSize (int nWidth, int nHeight)
{
	m_nMultiFrameWidth = nWidth;
	m_nMultiFrameHeight = nHeight;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		(*iterFrames)->SetMultiFrameSize (nWidth, nHeight);
	}
}

void IGFrameManager::SelectFrame (IGSmartPtr <IGFrame> &spFrame)
{
	bool bChanged = false;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		bChanged = (*iterFrames)->SetSelected (*iterFrames == spFrame);
		if (*iterFrames == spFrame)
		{
			m_spSelectedFrame = spFrame;
			m_bFrameSelected = true;
		}
		if (bChanged)
			(*iterFrames)->Redraw();
	}
}

void IGFrameManager::RedrawFrame (IGSmartPtr <IGFrame> &spFrame)
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
		{
			(*iterFrames)->Redraw(true);
			break;
		}
	}
}

bool IGFrameManager::GetSelectedFrame (IGSmartPtr <IGFrame> &spFrame)
{
	if (m_bFrameSelected)
	{
		spFrame = m_spSelectedFrame;
		return true;
	}
	return false;
}

bool IGFrameManager::GetSelectedLayers (std::list<const IGLayer*> &lpSelectedLayers)
{
	if (m_bFrameSelected)
	{
		lpSelectedLayers = m_spSelectedFrame->GetSelectedCalcs();
		return true;
	}
	return false;
}

// Remove a frame
void IGFrameManager::RemoveFrame (IGSmartPtr <IGFrame> &spFrame)
{
	// unmaximize current maximized frame
	unmaximize ();	
	m_spTreeFrames.reset ();
	if (!m_lspFrames.empty())
		m_lspFrames.remove (spFrame);
	// check if frame to be removed is minimized
	if (!m_lspMinimizedFrames.empty())
		m_lspMinimizedFrames.remove (spFrame);
	// check if frame to be removed is selected
	if (m_spSelectedFrame == spFrame)
	{
		m_spSelectedFrame.reset();
		m_bFrameSelected = false;
	}
	updateFrames();	
}

// Remove all frames
void IGFrameManager::RemoveAllFrames()
{
	// unmaximize current maximized frame
	unmaximize ();	
	m_spTreeFrames.reset ();
	m_lspFrames.clear();
	m_lspMinimizedFrames.clear();
	m_spSelectedFrame.reset();
	m_bFrameSelected = false;
	updateFrames();	
}

// update frame display. remove all frames and readd them to the list to 
// recompute all coordinates
void IGFrameManager::updateFrames()
{
	if (!m_lspFrames.empty())
	{
		std::list<IGSmartPtr <IGFrame>> lTmpList;
		lTmpList.insert (lTmpList.begin(), m_lspFrames.begin(), m_lspFrames.end());
		m_lspFrames.clear();
		for (std::list<IGSmartPtr <IGFrame>>::iterator iterFrames = lTmpList.begin(); 
			iterFrames != lTmpList.end(); ++iterFrames)
		{
			addFrame (*iterFrames);
		}
	}
}

// Get the specified frame
bool IGFrameManager::GetFrame (int nIdFrame, IGSmartPtr <IGFrame> &spFrame)
{
	if ((nIdFrame < 0) || (nIdFrame >= (int)m_lspFrames.size()))
		return false;
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		(iterFrames != m_lspFrames.end()) && (nIterFrames <= nIdFrame);
		++iterFrames, ++nIterFrames)
	{
		if (nIterFrames == nIdFrame)
		{
			IGSmartPtr <IGFrame> spTmpFrame (*iterFrames);
			spTmpFrame.swap (spFrame);
			return true;
		}
	}
	return false;
}

// Get the specified frame
bool IGFrameManager::GetFrame (LPCWSTR pcwGuid, IGSmartPtr <IGFrame> &spFrame)
{
	if (!pcwGuid || !*pcwGuid || m_lspFrames.empty())
		return false;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		(iterFrames != m_lspFrames.end());
		++iterFrames)
	{
		if (!::wcscmp ((*iterFrames)->GetId(), pcwGuid))
		{
			IGSmartPtr <IGFrame> spTmpFrame (*iterFrames);
			spTmpFrame.swap (spFrame);
			return true;
		}
	}
	return false;
}

int IGFrameManager::GetFramePos (LPCWSTR pcwGuid) const
{
	if (!pcwGuid || !*pcwGuid)
		return -1;
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		(iterFrames != m_lspFrames.end());
		++iterFrames, ++nIterFrames)
	{
		if (!::wcscmp ((*iterFrames)->GetId(), pcwGuid))
			return nIterFrames;
	}
	return -1;
}

int IGFrameManager::GetNbFrames()
{
	return m_lspFrames.size();
}

int IGFrameManager::GetNbVisibleFrames ()
{
	return m_lspFrames.size() - m_lspMinimizedFrames.size();
}

bool IGFrameManager::DrawFrame (int nIdFrame, HDC hdc, LPRECT p_rc, bool bMaintainRatio)
{
	if ((nIdFrame < 0) || (nIdFrame >= (int)m_lspFrames.size()))
		return false;
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames, ++nIterFrames)
	{
		if (nIterFrames == nIdFrame)
		{
			int nWidth = p_rc->right - p_rc->left;
			int nHeight = p_rc->bottom - p_rc->top;
			int nItemWidth = nWidth;
			int nItemHeight = nHeight;
			if (bMaintainRatio)
			{
				nItemWidth = (*iterFrames)->GetWidth();
				nItemHeight = (*iterFrames)->GetHeight();
				float fRatio = 1.0f;
				if ((float)nWidth / (float)nHeight > (float)nItemWidth / (float)nItemHeight)
				{
					fRatio = (float)nHeight / (float)nItemHeight;
					nItemWidth = (int)((float)nItemWidth * fRatio);			
					nItemHeight = nHeight;
				}
				else
				{
					fRatio = (float)nWidth / (float)nItemWidth;
					nItemWidth = nWidth;
					nItemHeight = (int)((float)nItemHeight * fRatio);
				}
			}
			return ((*iterFrames)->RenderSecondary (hdc, p_rc->left, p_rc->top,
													nItemWidth, nItemHeight, true) == 1);
		}
	}
	return false;
}

bool IGFrameManager::GetFrameSize (int nIdFrame, int &nWidth, int &nHeight)
{
	if ((nIdFrame < 0) || (nIdFrame >= (int)m_lspFrames.size()))
		return false;
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames, ++nIterFrames)
	{
		if (nIterFrames == nIdFrame)
		{
			nWidth = (*iterFrames)->GetWidth();
			nHeight = (*iterFrames)->GetHeight();
			return true;
		}
	}
	return false;
}



bool IGFrameManager::isMinimized (const IGSmartPtr <IGFrame> &spFrame)
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspMinimizedFrames.begin(); 
		iterFrames != m_lspMinimizedFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
		{
			return true;
		}
	}
	return false;
}

bool IGFrameManager::MinimizeFrame (int nIdFrame)
{
	if ((nIdFrame < 0) || (nIdFrame >= (int)m_lspFrames.size()))
		return false;
	m_spTreeFrames.reset ();
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames, ++nIterFrames)
	{
		if (nIterFrames == nIdFrame)
		{
			m_lspMinimizedFrames.push_back (*iterFrames);
			//m_lspFrames.remove (*iterFrames);
			updateFrames();
			return true;
		}
	}
	return false;	
}

bool IGFrameManager::UnMinimizeFrame (int nIdFrame)
{
	if ((nIdFrame < 0) || (nIdFrame >= (int)m_lspFrames.size()))
		return false;
	// unmaximize current maximized frame
	unmaximize ();
	m_spTreeFrames.reset ();
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames, ++nIterFrames)
	{
		if (nIterFrames == nIdFrame)
		{
			//m_lspFrames.push_back (*iterFrames);
			m_lspMinimizedFrames.remove (*iterFrames);
			updateFrames();
			return true;
		}
	}
	return false;
}

bool IGFrameManager::MaximizeFrame (int nIdFrame)
{
	if ((nIdFrame < 0) || (nIdFrame >= (int)m_lspFrames.size()))
		return false;
	// unmaximize current maximized frame
	unmaximize ();
	m_spTreeFrames.reset ();
	int nIterFrames = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames, ++nIterFrames)
	{
		if (nIterFrames == nIdFrame)
		{
			m_spMaximizedFrame = *iterFrames;
			updateFrames();
			return true;
		}
	}
	return false;
}

bool IGFrameManager::UnMaximize ()
{
	if (!m_spMaximizedFrame)
		return false;
	unmaximize ();
	m_spTreeFrames.reset ();
	updateFrames();
	return true;
}

bool IGFrameManager::Copy(LPCWSTR pcwGuid)
{
	IGSmartPtr <IGFrame> spFrame;	
	if (!GetFrame(pcwGuid, spFrame))
		return false;
	CxImage *pCxLayer = spFrame->GetWorkingLayer();
	if (!pCxLayer)
		return false;
	RECT rcSel;
	if (pCxLayer->SelectionIsValid())
		pCxLayer->SelectionGetBox (rcSel);
	else{
		rcSel.bottom = 0;
		rcSel.left = 0;
		rcSel.top = pCxLayer->GetHeight();
		rcSel.right = pCxLayer->GetWidth();
	}
	m_spClipboardLayer = new IGLayer(NULL);
	m_spClipboardLayer->Create (rcSel.right - rcSel.left, rcSel.top - rcSel.bottom, 24);
	m_spClipboardLayer->AlphaCreate();
	for(long y=rcSel.bottom; y<rcSel.top; y++){
		for(long x=rcSel.left; x<rcSel.right; x++){
			if (pCxLayer->BlindSelectionIsInside(x,y))
				m_spClipboardLayer->BlindSetPixelColor (x - rcSel.left, y - rcSel.bottom, pCxLayer->BlindGetPixelColor(x,y));
			else
				m_spClipboardLayer->AlphaSet (x - rcSel.left, y - rcSel.bottom, 0);
		}
	}
	return true;
}

bool IGFrameManager::Cut(LPCWSTR pcwGuid)
{
	IGSmartPtr <IGFrame> spFrame;
	if (!GetFrame(pcwGuid, spFrame))
		return false;
	CxImage *pCxLayer = spFrame->GetWorkingLayer();
	if (!pCxLayer)
		return false;
	RECT rcSel;
	if (pCxLayer->SelectionIsValid())
		pCxLayer->SelectionGetBox (rcSel);
	else{
		rcSel.bottom = 0;
		rcSel.left = 0;
		rcSel.top = pCxLayer->GetHeight();
		rcSel.right = pCxLayer->GetWidth();
	}
	m_spClipboardLayer = new IGLayer(NULL);
	m_spClipboardLayer->Create (rcSel.right - rcSel.left, rcSel.top - rcSel.bottom, 24);
	m_spClipboardLayer->AlphaCreate();
	pCxLayer->AlphaCreate();
	RGBQUAD qGray; qGray.rgbBlue = CXIMAGE_GRAY; qGray.rgbGreen = CXIMAGE_GRAY; qGray.rgbRed = CXIMAGE_GRAY;
	for(long y=rcSel.bottom; y<rcSel.top; y++){
		for(long x=rcSel.left; x<rcSel.right; x++){
			if (pCxLayer->BlindSelectionIsInside(x,y)){
				m_spClipboardLayer->BlindSetPixelColor(x - rcSel.left, y - rcSel.bottom, pCxLayer->BlindGetPixelColor(x,y));
				pCxLayer->BlindSetPixelColor(x, y, qGray);
				pCxLayer->AlphaSet(x, y, 0);
			}
			else
				m_spClipboardLayer->AlphaSet (x - rcSel.left, y - rcSel.bottom, 0);
		}
	}
	return true;
}

bool IGFrameManager::Paste(LPCWSTR pcwGuid)
{
	IGSmartPtr <IGFrame> spFrame;
	if (!GetFrame(pcwGuid, spFrame) || !m_spClipboardLayer)
		return false;
	IGLayer *pNewLayer = new IGLayer(NULL);
	pNewLayer->Copy(*m_spClipboardLayer, true, false, true, false);
	return spFrame->AddCalc(pNewLayer) == S_OK;
}

void IGFrameManager::unmaximize ()
{
	if (m_spMaximizedFrame)
		m_spMaximizedFrame.reset();
}

void IGFrameManager::updateFrameSize()
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		(*iterFrames)->UpdateFrameSize ();
	}
}

void IGFrameManager::insertFrame (IGSmartPtr <IGFrame> const &spFrame)
{	
	// maximized mode management
	if (m_spMaximizedFrame)
	{
		if (m_spMaximizedFrame != spFrame)
		{
			m_lspFrames.push_back (spFrame);
			return;
		}
	}	

	// minimized mode management	
	if (isMinimized (spFrame))
	{
		m_lspFrames.push_back (spFrame);
		return;
	}

	if (m_spTreeFrames)
	{
		IGSmartPtr <IGBinaryTreeNode <IGFrame>> spBestSite;
		std::list <IGSmartPtr <IGBinaryTreeNode <IGFrame>>> spNodeSiteList;
		getSiteList (m_spTreeFrames, spNodeSiteList);
		bool bRight = true;
		computeBestSite (spFrame, spNodeSiteList, spBestSite, bRight);
		if (bRight)
		{
			placeRight (spBestSite, spFrame);
		}
		else
		{
			placeBottom (spBestSite, spFrame);
		}		
	}
	else
	{
		// tree is empty, set new frame as root
		m_spTreeFrames.reset (new IGBinaryTreeNode <IGFrame> (spFrame));
	}
	// add new frame to frame list
	m_lspFrames.push_back (spFrame);

	// compute frame coordinates
	computeCoordinates ();
}

float IGFrameManager::getMaxLayoutTryRight (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode)
{
	if (!spTreeNode)
		return 0;
	if (!spTreeNode->m_spItem)
		return 0;
	float fLayoutWidth = 0.0f;
	float fLayoutHeight = 0.0f;
	float fLayoutTop = 0.0f;
	float fLayoutLeft = 0.0f;
	spTreeNode->m_spItem->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, true);
	float fMaxLayoutRight = fLayoutLeft + fLayoutWidth;
	float fMaxLayoutChildRight = 0.0f;
	if (spTreeNode->m_spLeft)
	{
		fMaxLayoutChildRight = getMaxLayoutTryRight (spTreeNode->m_spLeft);
		if (fMaxLayoutRight < fMaxLayoutChildRight)
		{
			fMaxLayoutRight = fMaxLayoutChildRight;
		}
	}
	if (spTreeNode->m_spRight)
	{
		fMaxLayoutChildRight = getMaxLayoutTryRight (spTreeNode->m_spRight);
		if (fMaxLayoutRight < fMaxLayoutChildRight)
		{
			fMaxLayoutRight = fMaxLayoutChildRight;
		}
	}
	return fMaxLayoutRight;
}

float IGFrameManager::getMaxLayoutTryBottom (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode)
{
	if (!spTreeNode)
		return 0;
	if (!spTreeNode->m_spItem)
		return 0;
	float fMaxLayoutChildValue = 0.0f;
	float fLayoutWidth = 0.0f;
	float fLayoutHeight = 0.0f;
	float fLayoutTop = 0.0f;
	float fLayoutLeft = 0.0f;		
	spTreeNode->m_spItem->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, true);
	float fMaxLayoutBottom = fLayoutTop + fLayoutHeight;
	float fMaxLayoutChildBottom = 0.0f;
	if (spTreeNode->m_spLeft)
	{
		fMaxLayoutChildBottom = getMaxLayoutTryBottom (spTreeNode->m_spLeft);
		if (fMaxLayoutBottom < fMaxLayoutChildBottom)
		{
			fMaxLayoutBottom = fMaxLayoutChildBottom;
		}
	}
	if (spTreeNode->m_spRight)
	{
		fMaxLayoutChildBottom = getMaxLayoutTryBottom (spTreeNode->m_spRight);
		if (fMaxLayoutBottom < fMaxLayoutChildBottom)
		{
			fMaxLayoutBottom = fMaxLayoutChildBottom;
		}
	}
	return fMaxLayoutBottom;
}

void IGFrameManager::getSiteList (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode, std::list <IGSmartPtr <IGBinaryTreeNode <IGFrame>>> &spNodeSiteList)
{
	if (!spTreeNode)
		return;
	if (!spTreeNode->m_spItem)
		return;
	if (!spTreeNode->m_spLeft || !spTreeNode->m_spRight)
		spNodeSiteList.push_back (spTreeNode);
	if (spTreeNode->m_spLeft)
		getSiteList (spTreeNode->m_spLeft, spNodeSiteList);
	if (spTreeNode->m_spRight)
		getSiteList (spTreeNode->m_spRight, spNodeSiteList);
}

void IGFrameManager::computeBestSite (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGBinaryTreeNode <IGFrame>>> const &spNodeSiteList, IGSmartPtr <IGBinaryTreeNode <IGFrame>> &spBestSite, bool &bRight)
{
#ifdef _DEBUG
	if (!spFrame)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL frame");
	if (spNodeSiteList.empty())
		_ASSERTE (FALSE && L"FrameMgr Error: Site list is empty");
#endif
	float fMaxLayoutRight = 0.0f;
	float fMaxLayoutBottom = 0.0f;
	float fCriteriumValue = 0.0f;
	float fMinCriterium = std::numeric_limits<float>::max();
	for (std::list<IGSmartPtr <IGBinaryTreeNode <IGFrame>>>::const_iterator iterSites = spNodeSiteList.begin(); 
		iterSites != spNodeSiteList.end();
		++iterSites)
	{
#ifdef _DEBUG
		if ((*iterSites)->m_spLeft && (*iterSites)->m_spRight)
			_ASSERTE (FALSE && L"FrameMgr Error: No frame site found on node");
#endif
		if (!(*iterSites)->m_spLeft)
		{
			initTryValues ();
			placeRight (*iterSites, spFrame, true);
			fMaxLayoutRight = getMaxLayoutTryRight (m_spTreeFrames);
			fMaxLayoutBottom = getMaxLayoutTryBottom (m_spTreeFrames);
			fCriteriumValue = computeSiteCriterium (fMaxLayoutRight, fMaxLayoutBottom);
			if (fCriteriumValue < fMinCriterium)
			{
				fMinCriterium = fCriteriumValue;
				spBestSite = *iterSites;
				bRight = true;	// left node means right of frame
			}
			// remove frame from right
			(*iterSites)->m_spLeft.reset();
		}
		if (!(*iterSites)->m_spRight)
		{			
			initTryValues ();
			placeBottom (*iterSites, spFrame, true);
			fMaxLayoutRight = getMaxLayoutTryRight (m_spTreeFrames);
			fMaxLayoutBottom = getMaxLayoutTryBottom (m_spTreeFrames);
			fCriteriumValue = computeSiteCriterium (fMaxLayoutRight, fMaxLayoutBottom);
			if (fCriteriumValue < fMinCriterium)
			{
				fMinCriterium = fCriteriumValue;
				spBestSite = *iterSites;
				bRight = false;	// right node means bottom of frame
			}
			// remove frame from bottom
			(*iterSites)->m_spRight.reset();
		}
	}
#ifdef _DEBUG
	if (!spBestSite)
		_ASSERTE (FALSE && L"FrameMgr Error: Best site could not be computed");
#endif
}

void IGFrameManager::initTryValues ()
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		(*iterFrames)->ResetTry();
	}
}

void IGFrameManager::placeRight (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spParentNode, IGSmartPtr <IGFrame> const &spFrame, bool bTry)
{
#ifdef _DEBUG
	if (!spFrame)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL frame");
	if (!spParentNode)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL parent node");
	if (!spParentNode->m_spItem)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL parent frame");
#endif
	// insert new frame in frame tree
	spParentNode->m_spLeft = new IGBinaryTreeNode <IGFrame> (spFrame);
	// get new frame layouts
	float fLayoutWidth = 0.0f;
	float fLayoutHeight = 0.0f;
	float fLayoutTop = 0.0f;
	float fLayoutLeft = 0.0f;		
	spFrame->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	// get parent frame layouts
	float fParentLayoutWidth = 0.0f;
	float fParentLayoutHeight = 0.0f;
	float fParentLayoutTop = 0.0f;
	float fParentLayoutLeft = 0.0f;		
	spParentNode->m_spItem->GetLayout (fParentLayoutLeft, fParentLayoutTop, fParentLayoutWidth, fParentLayoutHeight, bTry);
	// compute new frame layout coordinates
	fLayoutLeft = fParentLayoutLeft + fParentLayoutWidth;
	fLayoutTop = fParentLayoutTop;
	spFrame->SetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	// check if frame intersects with other frames and moves them if needed
	resolveFrameIntersections (spFrame, bTry);
}

void IGFrameManager::resolveFrameIntersections (IGSmartPtr <IGFrame> const &spFrame, bool bTry)
{
	if (!spFrame)
		return;
	// First check if frame intersects
	std::list <IGSmartPtr <IGFrame>> spIntersectFrameList;
	// get list of frames whose layouts intersect with frame layout		
	getIntersectFrameList (spFrame, spIntersectFrameList, bTry);
	if (spIntersectFrameList.empty())
		return; // This frame does not intersect, leave !
	// get frame layouts
	float fLayoutWidth = 0.0f;
	float fLayoutHeight = 0.0f;
	float fLayoutTop = 0.0f;
	float fLayoutLeft = 0.0f;		
	spFrame->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	// frame translation value
	float fTranslate = 0.0f;
	// current frame layout
	float fTranslatedLayoutWidth = 0.0f;
	float fTranslatedLayoutHeight = 0.0f;
	float fTranslatedLayoutTop = 0.0f;
	float fTranslatedLayoutLeft = 0.0f;
	// Now compute intersections due to frame moving
	std::list <IGSmartPtr <IGFrame>> spNextIntersectFrameList;
	IGSmartPtr <IGFrame> spIntersectedFrame;
	// First move all subtrees
	do
	{
		spIntersectedFrame = spIntersectFrameList.front();		
		// get intersected frame layout
		spIntersectedFrame->GetLayout (fTranslatedLayoutLeft, fTranslatedLayoutTop, fTranslatedLayoutWidth, fTranslatedLayoutHeight, bTry);
		// get frame corresponding node
		IGBinaryTreeNode <IGFrame> *pNode = m_spTreeFrames->GetNode (spIntersectedFrame);
#ifdef _DEBUG
		if (!pNode)
			_ASSERTE (FALSE && L"FrameMgr Error: NULL intersected frame node");
#endif
		// compute translation value
		if (fTranslatedLayoutTop < fLayoutTop)
		{	
			// translate whole frame subtree to the right
			// moved children are removed from list			
			fTranslate = fLayoutLeft + fLayoutWidth - fTranslatedLayoutLeft;
			translateSubTreeToRight (pNode, fTranslate, spIntersectFrameList, bTry);
		}
		else
		{
			// translate whole frame subtree to the bottom
			// moved children are removed from list
			fTranslate = fLayoutTop + fLayoutHeight - fTranslatedLayoutTop;
			translateSubTreeToBottom (pNode, fTranslate, spIntersectFrameList, bTry);
		}		
		// process moved frame
		spIntersectFrameList.remove (spIntersectedFrame);
		resolveFrameIntersections (spIntersectedFrame, bTry);
	}
	while (!spIntersectFrameList.empty());
}

void IGFrameManager::placeBottom (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spParentNode, IGSmartPtr <IGFrame> const &spFrame, bool bTry)
{
#ifdef _DEBUG
	if (!spFrame)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL frame");
	if (!spParentNode)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL parent node");
	if (!spParentNode->m_spItem)
		_ASSERTE (FALSE && L"FrameMgr Error: NULL parent frame");
#endif
	// insert new frame in frame tree
	spParentNode->m_spRight = new IGBinaryTreeNode <IGFrame> (spFrame);
	// get new frame layouts
	float fLayoutWidth = 0.0f;
	float fLayoutHeight = 0.0f;
	float fLayoutTop = 0.0f;
	float fLayoutLeft = 0.0f;	
	spFrame->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	// get parent frame layouts
	float fParentLayoutWidth = 0.0f;
	float fParentLayoutHeight = 0.0f;
	float fParentLayoutTop = 0.0f;
	float fParentLayoutLeft = 0.0f;		
	spParentNode->m_spItem->GetLayout (fParentLayoutLeft, fParentLayoutTop, fParentLayoutWidth, fParentLayoutHeight, bTry);
	// compute new frame layout coordinates
	fLayoutLeft = fParentLayoutLeft;
	fLayoutTop = fParentLayoutTop + fParentLayoutHeight;
	spFrame->SetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	// check if frame intersects with other frames and moves them if needed
	resolveFrameIntersections (spFrame, bTry);
}

void IGFrameManager::getIntersectFrameList (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &spIntersectFrameList, bool bTry)
{
	// get frame layout
	float fLayoutWidth = 0;
	float fLayoutHeight = 0;
	float fLayoutTop = 0;
	float fLayoutLeft = 0;
	spFrame->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	float fLayoutRight = fLayoutLeft + fLayoutWidth;
	float fLayoutBottom = fLayoutTop + fLayoutHeight;
	// current frame layout
	float fCurrentLayoutWidth = 0;
	float fCurrentLayoutHeight = 0;
	float fCurrentLayoutTop = 0;
	float fCurrentLayoutLeft = 0;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
			continue;	// do not process intersection with itself
		if (isMinimized (*iterFrames))
			continue;
		// if current frame is already in intersect list, just skip it
		bool bAlreadyInList = false;
		for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterIsectFrames = spIntersectFrameList.begin(); 
			iterIsectFrames != spIntersectFrameList.end();
			++iterIsectFrames)
		{
			if (*iterIsectFrames == *iterFrames)
			{
				bAlreadyInList = true;
				break;
			}
		}
		if (bAlreadyInList)
			continue;		
		// get current frame layout
		(*iterFrames)->GetLayout (fCurrentLayoutLeft, fCurrentLayoutTop, fCurrentLayoutWidth, fCurrentLayoutHeight, bTry);
		// compute intersection
		if (fLayoutRight <= fCurrentLayoutLeft)
			continue;
		if (fLayoutBottom <= fCurrentLayoutTop)
			continue;
		if (fLayoutLeft >= (float)(fCurrentLayoutLeft + fCurrentLayoutWidth))
			continue;
		if (fLayoutTop >= (float)(fCurrentLayoutTop + fCurrentLayoutHeight))
			continue;
		// an intersection has been found, add it to the list
		spIntersectFrameList.push_back (*iterFrames);
	}
}

void IGFrameManager::translateSubTreeToBottom (IGBinaryTreeNode <IGFrame> *pNode, float fTranslateDown, std::list <IGSmartPtr <IGFrame>> &spIntersectFrameList, bool bTry)
{
	if (!pNode)
		return;
	if (!pNode->m_spItem)
		return;
	spIntersectFrameList.remove (pNode->m_spItem);
	float fLayoutWidth = 0;
	float fLayoutHeight = 0;
	float fLayoutTop = 0;
	float fLayoutLeft = 0;
	pNode->m_spItem->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	fLayoutTop += fTranslateDown;
	pNode->m_spItem->SetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	if (pNode->m_spLeft)
		translateSubTreeToBottom (pNode->m_spLeft, fTranslateDown, spIntersectFrameList, bTry);
	if (pNode->m_spRight)
		translateSubTreeToBottom (pNode->m_spRight, fTranslateDown, spIntersectFrameList, bTry);
}

void IGFrameManager::translateSubTreeToRight (IGBinaryTreeNode <IGFrame> *pNode, float fTranslateRight, std::list <IGSmartPtr <IGFrame>> &spIntersectFrameList, bool bTry)
{
	if (!pNode)
		return;
	if (!pNode->m_spItem)
		return;
	spIntersectFrameList.remove (pNode->m_spItem);
	float fLayoutWidth = 0.0f;
	float fLayoutHeight = 0.0f;
	float fLayoutTop = 0.0f;
	float fLayoutLeft = 0.0f;
	pNode->m_spItem->GetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	fLayoutLeft += fTranslateRight;
	pNode->m_spItem->SetLayout (fLayoutLeft, fLayoutTop, fLayoutWidth, fLayoutHeight, bTry);
	if (pNode->m_spLeft)
		translateSubTreeToRight (pNode->m_spLeft, fTranslateRight, spIntersectFrameList, bTry);
	if (pNode->m_spRight)
		translateSubTreeToRight (pNode->m_spRight, fTranslateRight, spIntersectFrameList, bTry);
}

float IGFrameManager::computeSiteCriterium (float fMaxRight, float fMaxBottom)
{
	float fLayoutWidth = sqrtf ((float)m_nMultiFrameWidth / (float)m_nMultiFrameHeight);
	float fLayoutHeight = sqrtf ((float)m_nMultiFrameHeight / (float)m_nMultiFrameWidth);
	return /*fMaxRight * fMaxBottom */ ((powf ((float)(fabs (fMaxRight - (float)fLayoutWidth)), 2.0f) + 
					powf ((float)(fabs (fMaxBottom - (float)fLayoutHeight)), 2.0f)));
	/*return fMaxRight * fMaxBottom * abs ((float)fMaxRight / (float)fMaxBottom 
				- (float)m_nMultiFrameWidth / (float)m_nMultiFrameHeight);*/
}

void IGFrameManager::computeCoordinates ()
{
	computeCoordinates (m_spTreeFrames);
	postTreatCoordinates ();
}

void IGFrameManager::computeCoordinates (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode, float fMultiFrameLeft, float fMultiFrameTop, float fMultiFrameWidth, float fMultiFrameHeight)
{
	if (!spTreeNode)
		return;
	if (!spTreeNode->m_spItem)
		return;
	// compute nb child nodes on the right
	int nNbRightFrames = 0;
	IGSmartPtr <IGBinaryTreeNode <IGFrame>> spFrameNode (spTreeNode);
	do
	{
		nNbRightFrames++;
		spFrameNode = spFrameNode->m_spLeft;
	}
	while (spFrameNode);
	// compute nb child nodes on the bottom
	int nNbBottomFrames = 0;
	spFrameNode = spTreeNode;
	do
	{
		nNbBottomFrames++;
		spFrameNode = spFrameNode->m_spRight;
	}
	while (spFrameNode);
	// compute frame size
	float fFrameWidth = fMultiFrameWidth / (float)nNbRightFrames;
	float fFrameHeight = fMultiFrameHeight / (float)nNbBottomFrames;
	// Set frame coordinates (in frame ratio)
	spTreeNode->m_spItem->SetFrameLeftF (fMultiFrameLeft);
	spTreeNode->m_spItem->SetFrameTopF (fMultiFrameTop);
	spTreeNode->m_spItem->SetFrameWidthF (fFrameWidth);
	spTreeNode->m_spItem->SetFrameHeightF (fFrameHeight);
	float fFrameRight = fMultiFrameLeft + fFrameWidth;
	float fFrameBottom = fMultiFrameTop + fFrameHeight;
	// Set subtree frame coordinates
	int nFrameRightOrder = (spTreeNode->m_spLeft && spTreeNode->m_spRight) ? findHigherOrder (spTreeNode->m_spLeft) : 0;//spTreeNode->m_spLeft->m_spItem->GetOrder() : 0;
	int nFrameBottomOrder = (spTreeNode->m_spLeft && spTreeNode->m_spRight) ? findHigherOrder (spTreeNode->m_spRight) : 0;//spTreeNode->m_spRight->m_spItem->GetOrder() : 0;
	if (spTreeNode->m_spLeft)
	{
		bool bPrioritySolvedByChildren = false;
		if (spTreeNode->m_spRight)
		{
			if (spTreeNode->m_spRight->m_spLeft && !spTreeNode->m_spLeft->m_spRight)
			{
				// Priority is given to bottom
				computeCoordinates (spTreeNode->m_spRight, fMultiFrameLeft, fFrameBottom, fMultiFrameWidth, fMultiFrameHeight - fFrameHeight);
				computeCoordinates (spTreeNode->m_spLeft, fFrameRight, fMultiFrameTop, fMultiFrameWidth - fFrameWidth, fFrameHeight);
				bPrioritySolvedByChildren = true;
			}
			else if (!spTreeNode->m_spRight->m_spLeft && spTreeNode->m_spLeft->m_spRight)
			{
				// Priority is given to right
				computeCoordinates (spTreeNode->m_spLeft, fFrameRight, fMultiFrameTop, fMultiFrameWidth - fFrameWidth, fMultiFrameHeight);
				computeCoordinates (spTreeNode->m_spRight, fMultiFrameLeft, fFrameBottom, fFrameWidth, fMultiFrameHeight - fFrameHeight);
				bPrioritySolvedByChildren = true;
			}
			if (!bPrioritySolvedByChildren)
			{
				if (nFrameRightOrder < nFrameBottomOrder)
				{
					// Priority is given to bottom
					computeCoordinates (spTreeNode->m_spRight, fMultiFrameLeft, fFrameBottom, fMultiFrameWidth, fMultiFrameHeight - fFrameHeight);
					computeCoordinates (spTreeNode->m_spLeft, fFrameRight, fMultiFrameTop, fMultiFrameWidth - fFrameWidth, fFrameHeight);
				}
				else
				{
					// Priority is given to right
					computeCoordinates (spTreeNode->m_spLeft, fFrameRight, fMultiFrameTop, fMultiFrameWidth - fFrameWidth, fMultiFrameHeight);
					computeCoordinates (spTreeNode->m_spRight, fMultiFrameLeft, fFrameBottom, fFrameWidth, fMultiFrameHeight - fFrameHeight);
				}
			}
		}
		else
		{
			computeCoordinates (spTreeNode->m_spLeft, fFrameRight, fMultiFrameTop, fMultiFrameWidth - fFrameWidth, fMultiFrameHeight);
		}
	}
	else if (spTreeNode->m_spRight)
	{
		computeCoordinates (spTreeNode->m_spRight, fMultiFrameLeft, fFrameBottom, fMultiFrameWidth, fMultiFrameHeight - fFrameHeight);
	}
}

int IGFrameManager::findHigherOrder (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode)
{
	if (!spTreeNode)
		return 0;
	if (!spTreeNode->m_spItem)
		return 0;
	int nHigherOrder = spTreeNode->m_spItem->GetOrder();
	int nHigherChildOrder = 0;
	if (spTreeNode->m_spLeft)
	{
		nHigherChildOrder = findHigherOrder (spTreeNode->m_spLeft);
		if (nHigherChildOrder > nHigherOrder)
			nHigherOrder = nHigherChildOrder;
	}
	if (spTreeNode->m_spRight)
	{
		nHigherChildOrder = findHigherOrder (spTreeNode->m_spRight);
		if (nHigherChildOrder > nHigherOrder)
			nHigherOrder = nHigherChildOrder;
	}
	return nHigherOrder;	
}

void IGFrameManager::postTreatCoordinates ()
{
	// foreach frame, iterate finding the neighbor frames with a common side
	// common sided frames are placed in two lists : horizontalList and verticalList
	// frames on left and top are pushed front, frames on right and bottom are pushed back
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (isMinimized (*iterFrames))
			continue;
		std::list <IGSmartPtr <IGFrame>> lHorizontalList, lVerticalList;
		findLeftFrame (*iterFrames, lHorizontalList);
		bool bIsFrameInserted = false;
		if (!lHorizontalList.empty())
		{
			lHorizontalList.push_back (*iterFrames);
			bIsFrameInserted = true;
		}
		findRightFrame (*iterFrames, lHorizontalList);
		if (!lHorizontalList.empty() && !bIsFrameInserted)
			lHorizontalList.push_front (*iterFrames);
		findTopFrame (*iterFrames, lVerticalList);
		bIsFrameInserted = false;
		if (!lVerticalList.empty())
		{
			lVerticalList.push_back (*iterFrames);
			bIsFrameInserted = true;
		}
		findBottomFrame (*iterFrames, lVerticalList);
		if (!lVerticalList.empty() && !bIsFrameInserted)
			lVerticalList.push_front (*iterFrames);
		float fHorizontalGain = computeHorizontalGain (lHorizontalList);
		float fVerticalGain = computeVerticalGain (lVerticalList);
		if (!comparef (fVerticalGain, 0.0f) || !comparef (fHorizontalGain, 0.0f))
		{
			if (fHorizontalGain < fVerticalGain)
				postTreatVerticalList (lVerticalList);
			else
				postTreatHorizontalList (lHorizontalList);
		}
	}
}

void IGFrameManager::postTreatHorizontalList (std::list <IGSmartPtr <IGFrame>> &lFrameList)
{
	float fSeparationStep = (lFrameList.back()->GetFrameLeftF() + lFrameList.back()->GetFrameWidthF() - lFrameList.front()->GetFrameLeftF()) / (float)lFrameList.size();
	float fSeparationValue = lFrameList.front()->GetFrameLeftF();
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = lFrameList.begin(); 
		iterFrames != lFrameList.end();
		++iterFrames)
	{
		(*iterFrames)->SetFrameLeftF (fSeparationValue);
		(*iterFrames)->SetFrameWidthF (fSeparationStep);
		fSeparationValue += fSeparationStep;
	}
}

void IGFrameManager::postTreatVerticalList (std::list <IGSmartPtr <IGFrame>> &lFrameList)
{
	float fSeparationStep = (lFrameList.back()->GetFrameTopF() + lFrameList.back()->GetFrameHeightF() - lFrameList.front()->GetFrameTopF()) / (float)lFrameList.size();
	float fSeparationValue = lFrameList.front()->GetFrameTopF();
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = lFrameList.begin(); 
		iterFrames != lFrameList.end();
		++iterFrames)
	{
		(*iterFrames)->SetFrameTopF (fSeparationValue);
		(*iterFrames)->SetFrameHeightF (fSeparationStep);
		fSeparationValue += fSeparationStep;
	}
}

float IGFrameManager::computeHorizontalGain (std::list <IGSmartPtr <IGFrame>> const &lFrameList)
{
	if (lFrameList.size() < 2)
		return 0.0f;
	float fGain = 0.0f;
	float fSeparationStep = (lFrameList.back()->GetFrameLeftF() + lFrameList.back()->GetFrameWidthF() - lFrameList.front()->GetFrameLeftF()) / (float)lFrameList.size();
	float fSeparationValue = lFrameList.front()->GetFrameLeftF() + fSeparationStep;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = lFrameList.begin(); 
		iterFrames != lFrameList.end();
		++iterFrames)
	{
		// skip first frame
		if (iterFrames == lFrameList.begin())
			++iterFrames;
		fGain += fabs ((*iterFrames)->GetFrameLeftF() - fSeparationValue);
		fSeparationValue += fSeparationStep;
	}
	return fGain;
}

float IGFrameManager::computeVerticalGain (std::list <IGSmartPtr <IGFrame>> const &lFrameList)
{
	if (lFrameList.size() < 2)
		return 0.0f;
	float fGain = 0.0f;
	float fSeparationStep = (lFrameList.back()->GetFrameTopF() + lFrameList.back()->GetFrameHeightF() - lFrameList.front()->GetFrameTopF()) / (float)lFrameList.size();
	float fSeparationValue = lFrameList.front()->GetFrameTopF() + fSeparationStep;
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = lFrameList.begin(); 
		iterFrames != lFrameList.end();
		++iterFrames)
	{
		// skip first frame
		if (iterFrames == lFrameList.begin())
			++iterFrames;
		fGain += fabs ((*iterFrames)->GetFrameTopF() - fSeparationValue);
		fSeparationValue += fSeparationStep;
	}
	return fGain;
}

void IGFrameManager::findLeftFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList)
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
			continue;
		if (isMinimized (*iterFrames))
			continue;
		if (comparef (spFrame->GetFrameLeftF(), ((*iterFrames)->GetFrameLeftF() + (*iterFrames)->GetFrameWidthF()))
			&& comparef (spFrame->GetFrameTopF(), (*iterFrames)->GetFrameTopF())
			&& comparef (spFrame->GetFrameHeightF(), (*iterFrames)->GetFrameHeightF()))
		{
			lFrameList.push_front (*iterFrames);
			findLeftFrame (*iterFrames, lFrameList);
		}
	}
}

void IGFrameManager::findRightFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList)
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
			continue;
		if (isMinimized (*iterFrames))
			continue;
		if (comparef ((spFrame->GetFrameLeftF() + spFrame->GetFrameWidthF()), (*iterFrames)->GetFrameLeftF())
			&& comparef (spFrame->GetFrameTopF(), (*iterFrames)->GetFrameTopF())
			&& comparef (spFrame->GetFrameHeightF(), (*iterFrames)->GetFrameHeightF()))
		{
			lFrameList.push_back (*iterFrames);
			findRightFrame (*iterFrames, lFrameList);
		}
	}
}

void IGFrameManager::findTopFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList)
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
			continue;
		if (isMinimized (*iterFrames))
			continue;
		if (comparef (spFrame->GetFrameTopF(), ((*iterFrames)->GetFrameTopF() + (*iterFrames)->GetFrameHeightF()))
			&& comparef (spFrame->GetFrameLeftF(), (*iterFrames)->GetFrameLeftF())
			&& comparef (spFrame->GetFrameWidthF(), (*iterFrames)->GetFrameWidthF()))
		{
			lFrameList.push_front (*iterFrames);
			findTopFrame (*iterFrames, lFrameList);
		}
	}
}

void IGFrameManager::findBottomFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList)
{
	for (std::list<IGSmartPtr <IGFrame>>::const_iterator iterFrames = m_lspFrames.begin(); 
		iterFrames != m_lspFrames.end();
		++iterFrames)
	{
		if (*iterFrames == spFrame)
			continue;
		if (isMinimized (*iterFrames))
			continue;
		if (comparef ((spFrame->GetFrameTopF() + spFrame->GetFrameHeightF()), (*iterFrames)->GetFrameTopF())
			&& comparef (spFrame->GetFrameLeftF(), (*iterFrames)->GetFrameLeftF())
			&& comparef (spFrame->GetFrameWidthF(), (*iterFrames)->GetFrameWidthF()))
		{
			lFrameList.push_back (*iterFrames);
			findBottomFrame (*iterFrames, lFrameList);
		}
	}
}

bool IGFrameManager::comparef (float f1, float f2)
{
	return (f1 < f2 + 0.00001f && f1 > f2 - 0.00001f);
}