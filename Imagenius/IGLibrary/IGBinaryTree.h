#pragma once
#include "IGSmartPtr.h"

namespace IGLibrary
{

template <class T>
class IGBinaryTreeNode
{
	// An object of type IGBinaryTreeNode represents one node
	// in a binary tree of T objects.
public:
	IGBinaryTreeNode () : m_pParent (NULL)
	{
	}

	IGBinaryTreeNode (IGSmartPtr <T> const & spNewItem) : m_spItem (spNewItem)
	{		
	}

	void SetItem (IGSmartPtr <T> const & spNewItem)
	{
		m_spItem = spNewItem;
	}

	void RemoveItem ()
	{
		m_spItem.reset ();
	}

	IGBinaryTreeNode *GetNode (IGSmartPtr <T> const & spItem)
	{
       // Return item node if item is one of the items in the binary
       // tree.   Return NULL if not.
       if (m_spItem == NULL || spItem == NULL)
	   {
          return NULL;
       }
       else if (m_spItem == spItem)
	   {
          return this;
       }
       else
	   {
			IGBinaryTreeNode *pNode = NULL;
			if (m_spLeft)
			{
				pNode = m_spLeft->GetNode (spItem);
				if (pNode)
					return pNode;
			}
			if (m_spRight)
			{
				pNode = m_spRight->GetNode (spItem);
				if (pNode)
					return pNode;
			}
	   }
       return NULL;
    }

	void InsertLeft (IGSmartPtr <T> const & spItem)
	{
		m_spLeft = new IGSmartPtr<IGBinaryTreeNode> (spItem);
		m_spLeft->m_pParent = this;
	}

	void InsertRight (IGSmartPtr <T> const & spItem)
	{
		m_spRight = new IGSmartPtr<IGBinaryTreeNode> (spItem);
		m_spRight->m_pParent = this;
	}

	IGSmartPtr<T> m_spItem;      // The data in this node.
	IGSmartPtr<IGBinaryTreeNode> m_spLeft;    // Pointer to left subtree.
	IGSmartPtr<IGBinaryTreeNode> m_spRight;   // Pointer to right subtree.
};

}