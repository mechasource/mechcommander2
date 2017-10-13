//===========================================================================//
// File:	tree.cc                                                          //
// Contents: Implementation details of Tree class                            //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <stuff/node.hpp>
#include <stuff/tree.hpp>

using namespace Stuff;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeNode ~~~~~~~~~~~~~~~~~~~~~~~~~~~

//
//###########################################################################
// TreeNode
//###########################################################################
//
TreeNode::TreeNode(Tree* tree, Plug* plug)
	: Link(tree, plug)
{
	less = nullptr;
	greater = nullptr;
	parent = nullptr;
}

//
//###########################################################################
// ~TreeNode
//###########################################################################
//
TreeNode::~TreeNode()
{
	// Check_Object(this);
	Tree* tree = Cast_Object(Tree*, socket);
	Check_Object(tree);
	//
	//-------------------------------------------
	// Notify iterators that deletion is occuring
	//-------------------------------------------
	//
	tree->SendIteratorMemo(PlugRemoved, this);
	//
	//-----------------------------------
	// Tell the tree to release this node
	//-----------------------------------
	//
	tree->SeverFromTreeNode(this);
	//
	//------------------------------------------
	// Remove this link from any plug references
	//------------------------------------------
	//
	ReleaseFromPlug();
	//
	//-------------------------------------------------------------
	// Tell the node to release this link.  Note that this link
	// is not referenced by the plug or the chain at this point in
	// time.
	//-------------------------------------------------------------
	//
	if(tree->GetReleaseNode() != nullptr)
	{
		Check_Object(tree->GetReleaseNode());
		tree->GetReleaseNode()->ReleaseLinkHandler(tree, plug);
	}
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void
TreeNode::TestInstance(void)
{
	Link::TestInstance();
	if(less != nullptr)
	{
		Check_Signature(less);
	}
	if(greater != nullptr)
	{
		Check_Signature(greater);
	}
	if(parent != nullptr)
	{
		Check_Signature(parent);
	}
}

//
//###########################################################################
// SetupTreeLinks
//###########################################################################
//
void TreeNode::SetupTreeLinks(TreeNode* less, TreeNode* greater, TreeNode* parent)
{
	// Check_Object(this);
	this->less = less;
	this->greater = greater;
	this->parent = parent;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ Tree ~~~~~~~~~~~~~~~~~~~~~~~~~~~

//
//###########################################################################
// Tree
//###########################################################################
//
Tree::Tree(Node* node, bool has_unique_entries)
	: SortedSocket(node, has_unique_entries)
{
	root = nullptr;
}

//
//###########################################################################
// ~Tree
//###########################################################################
//
Tree::~Tree()
{
	// Check_Object(this);
	SetReleaseNode(nullptr);
	while(root != nullptr)
	{
		Unregister_Object(root);
		delete root;
	}
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void
Tree::TestInstance()
{
	SortedSocket::TestInstance();
	//
	// Check root if not null
	//
	if(root != nullptr)
	{
		Check_Object(root);
	}
}

//
//###########################################################################
// AddImplementation
//###########################################################################
//
void
Tree::AddImplementation(
	Plug* plug
)
{
	// Check_Object(this);
	AddValueImplementation(plug, nullptr);
}

//
//###########################################################################
// AddValueImplementation
//###########################################################################
//
void
Tree::AddValueImplementation(
	Plug* plug,
	PCVOID value
)
{
	// Check_Object(this);
	Check_Object(plug);
	/*
	* Verify that value has not been added
	*/
	Verify(HasUniqueEntries() ? SearchForValue(value) == nullptr : true);
	/*
	* Make new tree node
	*/
	TreeNode* node;
	node = MakeTreeNode(plug, value);
	Register_Object(node);
	/*
	* Add to the tree and send iterators memo to
	* update pointers
	*/
	AddTreeNode(node);
	SendIteratorMemo(PlugAdded, node);
}

//
//###########################################################################
// FindImplementation
//###########################################################################
//
Plug*
Tree::FindImplementation(
	PCVOID value
)
{
	// Check_Object(this);
	TreeNode* node;
	if((node = SearchForValue(value)) != nullptr)
	{
		Check_Object(node);
		return node->GetPlug();
	}
	return nullptr;
}

//
//#############################################################################
// IsEmpty
//#############################################################################
//
bool
Tree::IsEmpty()
{
	// Check_Object(this);
	return (root == nullptr);
}

//
//###########################################################################
// MakeTreeNode
//###########################################################################
//
TreeNode*
Tree::MakeTreeNode(
	Plug*,
	PCVOID
)
{
	// Check_Object(this);
	STOP(("Tree::MakeTreeNode - Should never reach here"));
	return nullptr;
}

//
//###########################################################################
// CompareTreeNodes
//###########################################################################
//
int32_t
Tree::CompareTreeNodes(
	TreeNode*,
	TreeNode*
)
{
	// Check_Object(this);
	STOP(("Tree::CompareTreeNodes - Should never reach here"));
	return 0;
}

//
//###########################################################################
// CompareValueToTreeNode
//###########################################################################
//
int32_t
Tree::CompareValueToTreeNode(
	PCVOID,
	TreeNode*
)
{
	// Check_Object(this);
	STOP(("Tree::CompareValueToTreeNode - Should never reach here"));
	return 0;
}

//
//###########################################################################
// AddTreeNode
//###########################################################################
//
void
Tree::AddTreeNode(
	TreeNode* newNode
)
{
	// Check_Object(this);
	Check_Object(newNode);
	/*
	* If root is nullptr this is the first item
	*/
	if(root == nullptr)
	{
		newNode->SetupTreeLinks(nullptr, nullptr, nullptr);
		root = newNode;
		return;
	}
	/*
	* Search for insertion point
	*/
	TreeNode* node;
	node = root;
	while(node != nullptr)
	{
		Check_Object(node);
		if(CompareTreeNodes(newNode, node) < 0)
		{
			if(node->less == nullptr)
			{
				newNode->SetupTreeLinks(nullptr, nullptr, node);
				node->less = newNode;
				break;
			}
			else
				node = node->less;
		}
		else
		{
			if(node->greater == nullptr)
			{
				newNode->SetupTreeLinks(nullptr, nullptr, node);
				node->greater = newNode;
				break;
			}
			else
				node = node->greater;
		}
	}
}

//
//###########################################################################
// SeverFromTreeNode
//###########################################################################
//
void
Tree::SeverFromTreeNode(
	TreeNode* node
)
{
	// Check_Object(this);
	Check_Object(node);
	if(node->greater == nullptr)
	{
		if(node->less == nullptr)
		{
			//
			//--------------------------------------------------------------------
			// The node has no subtrees
			//--------------------------------------------------------------------
			//
			if(node == root)
			{
				//
				// Tree is now empty, set root to null
				//
				root = nullptr;
			}
			else
			{
				//
				// Set appropiate branch to nullptr
				//
				Check_Object(node->parent);
				if(node->parent->less == node)
					node->parent->less = nullptr;
				else
					node->parent->greater = nullptr;
			}
		}
		else
		{
			//
			//--------------------------------------------------------------------
			// The node has a less subtree
			//--------------------------------------------------------------------
			//
			Check_Object(node->less);
			if(node == root)
			{
				//
				// Node is root... Set subtree to new root
				//
				root = node->less;
				node->less->parent = nullptr;
			}
			else
			{
				//
				// Node is not root
				// Set parents pointer to subtree
				//
				Check_Object(node->parent);
				if(node->parent->less == node)
				{
					node->parent->less = node->less;
				}
				else
				{
					node->parent->greater = node->less;
				}
				//
				// Set subtree parent to node parent
				//
				node->less->parent = node->parent;
			}
		}
	}
	else
	{
		if(node->less == nullptr)
		{
			//
			//--------------------------------------------------------------------
			// The node has a greater subtree
			//--------------------------------------------------------------------
			//
			Check_Object(node->greater);
			if(node == root)
			{
				//
				// Node is root
				// Set subtree to new root
				//
				root = node->greater;
				//
				// Set new root to have null parent
				//
				node->greater->parent = nullptr;
			}
			else
			{
				//
				// Node is not root
				// Set parents pointer to subtree
				//
				Check_Object(node->parent);
				if(node->parent->less == node)
					node->parent->less = node->greater;
				else
					node->parent->greater = node->greater;
				//
				// Set subtree parent to node parent
				//
				node->greater->parent = node->parent;
			}
		}
		else
		{
			//
			//--------------------------------------------------------------------
			// The node has lesser and greater sub-trees
			//--------------------------------------------------------------------
			//
			TreeNode* successor;
			Check_Object(node->less);
			Check_Object(node->greater);
			successor = node->greater;
			while(successor->less != nullptr)
			{
				successor = successor->less;
				Check_Object(successor);
			}
			//
			// Set successor's parent to subtree
			//
			Check_Object(successor->parent);
			if(successor->parent->less == successor)
				successor->parent->less = successor->greater;
			else
				successor->parent->greater = successor->greater;
			//
			// Set successor's subtree to parent
			//
			if(successor->greater != nullptr)
			{
				Check_Object(successor->greater);
				successor->greater->parent = successor->parent;
			}
			//
			// Place at node
			//
			successor->parent = node->parent;
			successor->less = node->less;
			successor->greater = node->greater;
			if(root == node)
			{
				//
				// Node was root
				//
				root = successor;
			}
			else
			{
				//
				// Set nodes parent to successor
				//
				Check_Object(successor->parent);
				if(successor->parent->less == node)
					successor->parent->less = successor;
				else
					successor->parent->greater = successor;
			}
			//
			// Set subtrees parent to successor
			//
			if(successor->greater != nullptr)
			{
				Check_Object(successor->greater);
				successor->greater->parent = successor;
			}
			if(successor->less != nullptr)
			{
				Check_Object(successor->less);
				successor->less->parent = successor;
			}
		}
	}
}

//
//###########################################################################
// SearchForValue
//###########################################################################
//
TreeNode*
Tree::SearchForValue(
	PCVOID value
)
{
	// Check_Object(this);
	TreeNode* node;
	int32_t ret;
	node = root;
	while(node != nullptr)
	{
		Check_Object(node);
		if((ret = CompareValueToTreeNode(value, node)) == 0)
			break;
		node = (ret < 0) ? node->less : node->greater;
	}
	return node;
}

//
//###########################################################################
// TreeIterator
//###########################################################################
//
TreeIterator::TreeIterator(Tree* tree):
	SortedIterator(tree)
{
	First();
}

Iterator*
TreeIterator::MakeClone()
{
	// Check_Object(this);
	return new TreeIterator(*this);
}

//
//###########################################################################
//###########################################################################
//
TreeIterator::~TreeIterator()
{
	// Check_Object(this);
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void
TreeIterator::TestInstance()
{
	SortedIterator::TestInstance();
	if(currentNode != nullptr)
	{
		Check_Object(currentNode);
	}
}

//
//###########################################################################
// First
//###########################################################################
//
void
TreeIterator::First()
{
	TreeNode* node;
	node = Cast_Object(Tree*, socket)->root;
	if(node != nullptr)
	{
		Check_Object(node);
		while(node->less != nullptr)
		{
			node = node->less;
			Check_Object(node);
		}
	}
	currentNode = node;
}

//
//###########################################################################
// Last
//###########################################################################
//
void TreeIterator::Last()
{
	// Check_Object(this);
	//
	// Should never reach here
	//
#ifdef __BCPLUSPLUS__
#pragma warn -ccc
	Verify(False);
#pragma warn +ccc
#endif
}

//
//###########################################################################
// Next
//###########################################################################
//
void TreeIterator::Next()
{
	// Check_Object(this);
	TreeNode* node;
	if((node = currentNode) == nullptr)
		return;
	Check_Object(node);
	if(node->greater != nullptr)
	{
		node = node->greater;
		Check_Object(node);
		while(node->less != nullptr)
		{
			node = node->less;
			Check_Object(node);
		}
		currentNode = node;
		return;
	}
	currentNode = nullptr;
	while(node->parent != nullptr)
	{
		Check_Object(node->parent);
		if(node == node->parent->less)
		{
			currentNode = node->parent;
			return;
		}
		node = node->parent;
		Check_Object(node);
	}
}

//
//###########################################################################
// Previous
//###########################################################################
//
void
TreeIterator::Previous()
{
	// Check_Object(this);
	//
	// Should never reach here
	//
#ifdef __BCPLUSPLUS__
#pragma warn -ccc
	Verify(False);
#pragma warn +ccc
#endif
}

#if 0
//
//###########################################################################
// ReadAndNextImplementation
//###########################################################################
//
void
* TreeIterator::ReadAndNextImplementation()
{
	// Check_Object(this);
	PVOIDplug;
	if((plug = GetCurrentImplementation()) != nullptr)
	{
		Next();
	}
	return plug;
}
#endif

#if 0
//
//###########################################################################
// ReadAndPreviousImplementation
//###########################################################################
//
void
* TreeIterator::ReadAndPreviousImplementation()
{
	// Check_Object(this);
#ifdef __BCPLUSPLUS__
#pragma warn -ccc
	Verify(False);
#pragma warn +ccc
#endif
	return(nullptr);
}
#endif

//
//###########################################################################
// GetCurrentImplementation
//###########################################################################
//
void
* TreeIterator::GetCurrentImplementation()
{
	// Check_Object(this);
	if(currentNode != nullptr)
	{
		Check_Object(currentNode);
		return currentNode->GetPlug();
	}
	return nullptr;
}

//
//###########################################################################
// GetSize
//###########################################################################
//
CollectionSize
TreeIterator::GetSize()
{
	// Check_Object(this);
	TreeIterator	iterator(Cast_Object(Tree*, socket));
	CollectionSize i = 0;
	while(iterator.ReadAndNextImplementation() != nullptr)
	{
		i++;
	}
	return(i);
}

#if 0
//
//###########################################################################
// GetNthImplementation
//###########################################################################
//
void
* TreeIterator::GetNthImplementation(
	CollectionSize index
)
{
	// Check_Object(this);
	CollectionSize i = 0;
	void*				 plug;
	First();
	while((plug = GetCurrentImplementation()) != nullptr)
	{
		if(i == index)
			return plug;
		Next();
		i++;
	}
	return nullptr;
}
#endif

//
//###########################################################################
// Remove
//###########################################################################
//
void
TreeIterator::Remove()
{
	// Check_Object(this);
	if(currentNode != nullptr)
	{
		Unregister_Object(currentNode);
		delete currentNode;
	}
}

//
//###########################################################################
// FindImplementation
//###########################################################################
//
Plug*
TreeIterator::FindImplementation(
	PCVOID value
)
{
	// Check_Object(this);
	TreeNode* node;
	if((node = Cast_Object(Tree*, socket)->SearchForValue(value)) != nullptr)
	{
		Check_Object(node);
		return (currentNode = node)->GetPlug();
	}
	return nullptr;
}

//
//###########################################################################
// ReceiveMemo
//###########################################################################
//
void
TreeIterator::ReceiveMemo(
	IteratorMemo memo,
	PVOID content
)
{
	// Check_Object(this);
	if(memo == PlugRemoved)
	{
		if(content == currentNode)
			Next();
	}
}
