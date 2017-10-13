//===========================================================================//
// File:	tree.hh                                                          //
// Contents: Interface specification of Tree class                           //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _TREE_HPP_
#define _TREE_HPP_

#include <stuff/link.hpp>
#include <stuff/memoryblock.hpp>
#include <stuff/iterator.hpp>
#include <stuff/sortedsocket.hpp>

namespace Stuff
{

class Tree;
class TreeIterator;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeNode ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TreeNode : public Link
{
	friend class Tree;
	friend class TreeIterator;

  public:
	~TreeNode(void);
	void TestInstance(void);

  protected:
	TreeNode(Tree* tree, Plug* plug);

  private:
	void SetupTreeLinks(TreeNode* less, TreeNode* greater, TreeNode* parent);

	TreeNode* less;
	TreeNode* greater;
	TreeNode* parent;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeNodeOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum
{
	TreeNode_MemoryBlock_Allocation = 100
};

template <class V> class TreeNodeOf : public TreeNode
{
  public:
	TreeNodeOf(Tree* tree, Plug* plug, const V& value);
	~TreeNodeOf(void);

	PVOID
	operator new(size_t);
	void operator delete(PVOID where);

	V GetValue() { return value; }
	V* GetValuePointer() { return &value; }

  private:
	static MemoryBlock* allocatedMemory;
	static CollectionSize allocationCount;

	V value;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeNodeOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~

template <class V> MemoryBlock* TreeNodeOf<V>::allocatedMemory   = nullptr;
template <class V> CollectionSize TreeNodeOf<V>::allocationCount = 0;

template <class V>
TreeNodeOf<V>::TreeNodeOf(Tree* tree, Plug* plug, const V& value)
	: TreeNode(tree, plug)
{
	this->value = value;
}

template <class V> TreeNodeOf<V>::~TreeNodeOf() {}

template <class V> PVOID TreeNodeOf<V>::operator new(size_t)
{
	Verify(allocationCount >= 0);
	if (allocationCount++ == 0)
	{
		allocatedMemory = new MemoryBlock(sizeof(TreeNodeOf<V>),
			TreeNode_MemoryBlock_Allocation, TreeNode_MemoryBlock_Allocation,
			"Stuff::TreeNodeOf", Stuff::ConnectionEngineHeap);
		Register_Object(allocatedMemory);
	}
	Verify(allocationCount < INT_MAX);
	Check_Object(allocatedMemory);
	return allocatedMemory->New();
}

template <class V> void TreeNodeOf<V>::operator delete(PVOID where)
{
	Check_Object(allocatedMemory);
	allocatedMemory->Delete(where);
	if (--allocationCount == 0)
	{
		Unregister_Object(allocatedMemory);
		delete allocatedMemory;
		allocatedMemory = nullptr;
	}
	Verify(allocationCount >= 0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Tree ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Tree : public SortedSocket
{
	friend class TreeNode;
	friend class TreeIterator;

  public:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Public interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//

	//
	//--------------------------------------------------------------------
	// Constructor, Destructor and testing
	//--------------------------------------------------------------------
	//
	Tree(Node* node, bool has_unique_entries);
	~Tree(void);

	void TestInstance(void);
	static void TestClass(void);
	static void ProfileClass(void);

	//
	//-----------------------------------------------------------------------
	// IsEmpty - Returns true if the socket contains no plugs.
	//-----------------------------------------------------------------------
	//
	bool IsEmpty(void);

  protected:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Protected interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	void AddImplementation(Plug*);
	void AddValueImplementation(Plug* plug, PCVOID value);
	Plug* FindImplementation(PCVOID value);

  private:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Private interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	virtual TreeNode* MakeTreeNode(Plug* plug, PCVOID value);
	virtual int32_t CompareTreeNodes(TreeNode* link1, TreeNode* link2);
	virtual int32_t CompareValueToTreeNode(PCVOID value, TreeNode* link);

	void AddTreeNode(TreeNode* node);
	void SeverFromTreeNode(TreeNode* node);
	TreeNode* SearchForValue(PCVOID value);

	//
	//--------------------------------------------------------------------
	// Private data
	//--------------------------------------------------------------------
	//
	TreeNode* root;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T, class V> class TreeOf : public Tree
{
  public:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Public interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	TreeOf(Node* node, bool has_unique_entries);
	~TreeOf(void);

	//
	//--------------------------------------------------------------------
	// Socket methods (see Socket for full listing)
	//--------------------------------------------------------------------
	//
	void AddValue(T plug, const V& value)
	{
		AddValueImplementation(Cast_Object(Plug*, plug), &value);
	}
	void Remove(T plug) { RemovePlug(Cast_Object(Plug*, plug)); }
	T Find(const V& value) { return (T)FindImplementation(&value); }

  private:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Private interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	TreeNode* MakeTreeNode(Plug* plug, PCVOID value)
	{
		return new TreeNodeOf<V>(this, plug, *Cast_Pointer(const V*, value));
	}
	int32_t CompareTreeNodes(TreeNode* link1, TreeNode* link2);
	int32_t CompareValueToTreeNode(PCVOID value, TreeNode* link);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T, class V>
TreeOf<T, V>::TreeOf(Node* node, bool has_unique_entries)
	: Tree(node, has_unique_entries)
{
}

template <class T, class V> TreeOf<T, V>::~TreeOf() {}

template <class T, class V>
int32_t TreeOf<T, V>::CompareTreeNodes(TreeNode* node1, TreeNode* node2)
{
	V* ptr1 = Cast_Object(TreeNodeOf<V>*, node1)->GetValuePointer();
	V* ptr2 = Cast_Object(TreeNodeOf<V>*, node2)->GetValuePointer();
	Check_Pointer(ptr1);
	Check_Pointer(ptr2);
	if (ptr1 && ptr2)
	{
		if (*ptr1 == *ptr2)
			return 0;
		else
			return ((*ptr1 > *ptr2) ? 1 : -1);
	}
	return 0;
}

template <class T, class V>
int32_t TreeOf<T, V>::CompareValueToTreeNode(PCVOID value, TreeNode* node)
{
	Check_Pointer(value);
	V* ptr = Cast_Object(TreeNodeOf<V>*, node)->GetValuePointer();
	Check_Pointer(ptr);
	if (value && ptr)
	{
		if (*Cast_Pointer(const V*, value) == *ptr)
			return 0;
		else
			return (*Cast_Pointer(const V*, value) > *ptr) ? 1 : -1;
	}
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class TreeIterator : public SortedIterator
{
  public:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Public interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//

	//
	//--------------------------------------------------------------------
	// Constructors, Destructor and testing
	//--------------------------------------------------------------------
	//
	explicit TreeIterator(Tree* tree);
	Iterator* MakeClone(void);
	~TreeIterator(void);
	void TestInstance(void);

	//
	//--------------------------------------------------------------------
	// Iterator methods (see Iterator for full listing)
	//--------------------------------------------------------------------
	//
	void First(void);
	void Next(void);
	CollectionSize GetSize(void);
	void Remove(void);

  protected:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Protected interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	PVOID GetCurrentImplementation(void);
	Plug* FindImplementation(PCVOID value);

  protected:
	//
	//--------------------------------------------------------------------
	// Protected data
	//--------------------------------------------------------------------
	//
	TreeNode* currentNode;

  private:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Private interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	void Last(void);	 // Marked as private so that clients do not use it
	void Previous(void); // Marked as private so that clients do not use it

	void ReceiveMemo(IteratorMemo memo, PVOID content);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TreeIteratorOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T, class V> class TreeIteratorOf : public TreeIterator
{
  public:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Public interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//

	//
	//--------------------------------------------------------------------
	// Constructors and Destructor
	//--------------------------------------------------------------------
	//
	explicit TreeIteratorOf(TreeOf<T, V>* tree);
	Iterator* MakeClone(void);
	~TreeIteratorOf(void);

	//
	//--------------------------------------------------------------------
	// Iterator methods (see Iterator for full listing)
	//--------------------------------------------------------------------
	//
	T ReadAndNext(void) { return (T)ReadAndNextImplementation(); }

	T GetCurrent(void) { return (T)GetCurrentImplementation(); }

	T GetNth(CollectionSize index) { return (T)GetNthImplementation(index); }

	T Find(const V& value) { return (T)FindImplementation(&value); }

	V GetValue(void)
	{
		return Cast_Object(TreeNodeOf<V>*, currentNode)->GetValue();
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~ TreeIteratorOf templates ~~~~~~~~~~~~~~~~~~~~~~~

template <class T, class V>
TreeIteratorOf<T, V>::TreeIteratorOf(TreeOf<T, V>* tree) : TreeIterator(tree)
{
}

template <class T, class V> Iterator* TreeIteratorOf<T, V>::MakeClone(void)
{
	return new TreeIteratorOf<T, V>(*this);
}

template <class T, class V> TreeIteratorOf<T, V>::~TreeIteratorOf(void) {}
}
#endif
