#pragma once
#include "IGSmartPtr.h"
#include "ximage.h"
#include "IGLayer.h"
#include "IGFrame.h"
#include "IGBinaryTree.h"
#include "IGProperties.h"
#include <list>

namespace IGLibrary
{
	class IGFrameManager : public IGProperties
	{
	public:
		IGFrameManager();
		virtual ~IGFrameManager();

		// Set the size in pixels of the multiframe panel
		// This recomputes coordinates of all frames
		void SetMultiFrameSize (int nWidth, int nHeight);

		// Add a new frame to the frame tree
		bool AddFrame (IGSmartPtr <IGFrame> &spFrame);	

		// Select a frame
		void SelectFrame (IGSmartPtr <IGFrame> &spFrame);

		// Get the current selected frame. Return false if no selected frame
		bool GetSelectedFrame (IGSmartPtr <IGFrame> &spFrame);

		// Get the current selected layer of selected frame. Return false if no selected frame.
		//bool GetSelectedLayer (int &nLayerId);
		bool GetSelectedLayers (std::list<const IGLayer*> &lpSelectedLayers);

		// Remove a frame
		void RemoveFrame (IGSmartPtr <IGFrame> &spFrame);
		void RemoveAllFrames();

		// Redraw a frame
		void RedrawFrame (IGSmartPtr <IGFrame> &spFrame);

		// Get the current number of loaded frames
		int GetNbFrames();

		// Get the number of visible frames
		int GetNbVisibleFrames ();

		// Get the specified frame
		bool GetFrame (int nIdFrame, IGSmartPtr <IGFrame> &spFrame);
		bool GetFrame (LPCWSTR pcwGuid, IGSmartPtr <IGFrame> &spFrame);

		// Get the specified frame position
		int GetFramePos (LPCWSTR pcwGuid) const;

		// Draw specified frame in given HDC and size
		bool DrawFrame (int nIdFrame, HDC hdc, LPRECT p_rc, bool bMaintainRatio = false);

		// Get specified frame width and height
		bool GetFrameSize (int nIdFrame, int &nWidth, int &nHeight);

		// Minimize specified frame, returns false if already minimized
		bool MinimizeFrame (int nIdFrame);

		// Maximize specified frame, returns false if already maximized
		bool MaximizeFrame (int nIdFrame);

		// UnMinimize specified frame, returns false if not minimized
		bool UnMinimizeFrame (int nIdFrame);

		// UnMaximize current maximized frame, returns false if no maximized frame
		bool UnMaximize ();

		// Clipboard management
		bool Copy(LPCWSTR pcwGuid);
		bool Cut(LPCWSTR pcwGuid);
		bool Paste(LPCWSTR pcwGuid);
	private:
		void updateFrameSize();

		// insert a frame in the best place in the frame tree
		void insertFrame (IGSmartPtr <IGFrame> const &spFrame);

		// compute the criterium value to select best site for the new frame
		float computeSiteCriterium (float fMaxRight, float fMaxBottom);

		// compute the best frame site in the frame tree
		void computeBestSite (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGBinaryTreeNode <IGFrame>>> const &spNodeSiteList, IGSmartPtr <IGBinaryTreeNode <IGFrame>> &spBestSite, bool &bRight);
		
		// get the list of frame sites for the new frame
		void getSiteList (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode, std::list <IGSmartPtr <IGBinaryTreeNode <IGFrame>>>& spNodeSiteList);
		
		// get the max frame right value
		float getMaxLayoutTryRight (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode);

		// get the max frame bottom value
		float getMaxLayoutTryBottom (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode);

		// set each frame try values to layout values
		void initTryValues ();

		// insert frame in a new child node of spParentNode.
		// it computes the frame layout coordinates, bTry means that try values are used instead of layout values
		void placeRight (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spParentNode, IGSmartPtr <IGFrame> const &spFrame, bool bTry = false);
		void placeBottom (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spParentNode, IGSmartPtr <IGFrame> const &spFrame, bool bTry = false);

		// get list of frames whose layouts intersect with spFrame layout, bTry means that try values are used instead of layout values
		void getIntersectFrameList (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &spIntersectFrameList, bool bTry = false);

		void resolveFrameIntersections (IGSmartPtr <IGFrame> const &spFrame, bool bTry = false);

		// translate whole subtree frames
		void translateSubTreeToBottom (IGBinaryTreeNode <IGFrame> *pNode, float fTranslateDown, std::list <IGSmartPtr <IGFrame>> &spIntersectFrameList, bool bTry = false);
		void translateSubTreeToRight (IGBinaryTreeNode <IGFrame> *pNode, float fTranslateRight, std::list <IGSmartPtr <IGFrame>> &spIntersectFrameList, bool bTry = false);

		// compute frame coordinates from layout coordinates and multiframe size
		void computeCoordinates ();
		void computeCoordinates (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode, float fMultiFrameLeft = 0.0f, float fMultiFrameTop = 0.0f, float fMultiFrameWidth = 1.0f, float fMultiFrameHeight = 1.0f);

		// get highest frame order in descendants of a node
		int findHigherOrder (IGSmartPtr <IGBinaryTreeNode <IGFrame>> const &spTreeNode);

		// improve frame placement by a post treatment
		void postTreatCoordinates();
		void postTreatHorizontalList (std::list <IGSmartPtr <IGFrame>> &lFrameList);
		void postTreatVerticalList (std::list <IGSmartPtr <IGFrame>> &lFrameList);
		float computeHorizontalGain (std::list <IGSmartPtr <IGFrame>> const &lFrameList);
		float computeVerticalGain (std::list <IGSmartPtr <IGFrame>> const &lFrameList);
		void findLeftFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList);
		void findRightFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList);
		void findTopFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList);
		void findBottomFrame (IGSmartPtr <IGFrame> const &spFrame, std::list <IGSmartPtr <IGFrame>> &lFrameList);

		// floating comparison with 10-5 floating precision
		bool IGFrameManager::comparef (float f1, float f2);

		// recompute all frame coordinates
		void updateFrames();

		// same as UnMaximize except that we do not recompute all frame coordinates
		void unmaximize ();

		bool isMinimized (const IGSmartPtr <IGFrame> &spFrame);
		void addFrame (IGSmartPtr <IGFrame> &spFrame);
		
		static unsigned int g_nNbMaxFrames;
		
		RECT										m_rcMultiFrame;
		std::list <IGSmartPtr <IGFrame>>			m_lspFrames;
		std::list <IGSmartPtr <IGFrame>>			m_lspMinimizedFrames;
		IGSmartPtr <IGFrame>						m_spMaximizedFrame;
		IGSmartPtr <IGBinaryTreeNode <IGFrame>>		m_spTreeFrames;
		int											m_nMultiFrameWidth;
		int											m_nMultiFrameHeight;
		IGSmartPtr <IGFrame>						m_spSelectedFrame;
		bool										m_bFrameSelected;
		IGSmartPtr <IGLayer>						m_spClipboardLayer;
	};
}