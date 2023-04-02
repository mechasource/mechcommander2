//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//
// *
// * File: Hash.h
// *
// * Contents: Hash table containers
// *
// *****************************************************************************/

#pragma once

#ifndef __HASH_H__
#define __HASH_H__

//#pragma warning( disable : 4244 )

//
// Node structures
//
#pragma pack(push, 4)

struct CMTListNode
{
	CMTListNode* m_Next; // next item in list
	CMTListNode* m_Prev; // previous item in list
	PVOID m_Data; // user's blob
	int32_t m_DeletedAndIdx; // node's index & high bit is lazy delete flag

	PVOID operator new(size_t size);
	PVOID operator new[](size_t size);

	void operator delete(PVOID ptr);
	void operator delete[](PVOID ptr);
};

#ifndef ASSERT
#define ASSERT(x)

#endif

#define DELETED_MASK 0x80000000
#define IDX_MASK 0x7fffffff

#define MARK_NODE_DELETED(pNode) (pNode->m_DeletedAndIdx |= DELETED_MASK)
#define CLEAR_NODE_DELETED(pNode) (pNode->m_DeletedAndIdx &= IDX_MASK)
#define IS_NODE_DELETED(pNode) (pNode->m_DeletedAndIdx & DELETED_MASK)
#define GET_NODE_IDX(pNode) (pNode->m_DeletedAndIdx & IDX_MASK)
#define SET_NODE_IDX(pNode, idx) \
	(pNode->m_DeletedAndIdx = (pNode->m_DeletedAndIdx & DELETED_MASK) | idx)

#pragma pack(pop)

//
// handle typedefs
//
typedef CMTListNode* MTListNodeHandle;

///////////////////////////////////////////////////////////////////////////////
// Basic hash table (NOT thread safe)
///////////////////////////////////////////////////////////////////////////////

template <class T, class K>
class CHash
{
public:
	//
	// Callback typedefs
	//
	typedef uint32_t (*PFHASHFUNC)(K);
	typedef BOOLEAN (*PFCOMPAREFUNC)(T*, K);
	typedef BOOLEAN (*PFITERCALLBACK)(T*, MTListNodeHandle, PVOID);
	typedef void (*PFDELFUNC)(T*, PVOID);
	typedef void (*PFGETFUNC)(T*);

	//
	// Constructor and destructor
	//
	CHash(PFHASHFUNC HashFunc, PFCOMPAREFUNC CompareFunc, PFGETFUNC GetFunc = nullptr,
		uint16_t NumBuckets = 256, uint16_t NumLocks = 16);
	~CHash(void);

	//
	// Adds object into hash table using the specified key.
	// It does not check for duplicate keys.
	//
	MTListNodeHandle Add(K Key, T* Object);

	//
	// Returns the number of objects in the hash table
	//
	int32_t Count() { return m_NumObjects; }

	//
	// Returns first object it finds associated with the specified key.
	//
	T* Get(K Key);

	//
	// Removes and returns first object it finds associated with the specified
	// key.
	//
	T* Delete(K Key);

	//
	// Removes specified node from table.
	//
	void DeleteNode(MTListNodeHandle node, PFDELFUNC pfDelete = nullptr, PVOID Cookie = nullptr);

	//
	// Marks node as deleted without locking the table. It can be
	// called from within the iterator callback (see ForEach).
	//
	void MarkNodeDeleted(
		MTListNodeHandle node, PFDELFUNC pfDelete = nullptr, PVOID Cookie = nullptr);

	//
	// Callback iterator. Returns false if the iterator was prematurely
	// ended by the callback function, otherwise true.
	//
	// Callback:
	// Form:
	// BOOLEAN callback_function( T* pObject, MTListNodeHandle hNode, PVOID Cookie
	// ) Behavior: If the callback returns false, the iterator immediately
	// stops. If the callback returns true, the iterator continues on to the
	// next node. Restrictions: (1) Using any CMTHash function other than
	// MarkNodeDeleted may result in a deadlock.
	//
	BOOLEAN ForEach(PFITERCALLBACK pfCallback, PVOID Cookie);

	//
	// Removed all nodes from hash table.
	//
	void RemoveAll(PFDELFUNC pfDelete = nullptr, PVOID Cookie = nullptr);

	//
	// Removes nodes marked as deleted
	//
	void TrashDay(void);

	PVOID operator new(size_t size);
	void operator delete(PVOID ptr);
	void operator delete[](PVOID ptr);

protected:
	typedef uint32_t (*PFHASHFUNC)(K);
	typedef BOOLEAN (*PFCOMPAREFUNC)(T*, K);
	typedef BOOLEAN (*PFITERCALLBACK)(T*, MTListNodeHandle, PVOID);
	typedef void (*PFDELFUNC)(T*, PVOID);

	uint16_t m_NumBuckets;
	uint16_t m_BucketMask;
	CMTListNode* m_Buckets;
	PFCOMPAREFUNC m_CompareFunc;
	PFHASHFUNC m_HashFunc;
	PFGETFUNC m_GetFunc;
	CMTListNode m_PreAllocatedBuckets[1];
	int32_t m_NumObjects;
	int32_t m_Recursion;
};

///////////////////////////////////////////////////////////////////////////////
// Inline implementation of CHash
///////////////////////////////////////////////////////////////////////////////

template <class T, class K>
inline CHash<T, K>::CHash(PFHASHFUNC HashFunc, PFCOMPAREFUNC CompareFunc, PFGETFUNC GetFunc,
	uint16_t NumBuckets, uint16_t NumLocks)
{
	CMTListNode* pBucket;
	uint16_t i;
	//
	// Set callback functions
	//
	m_HashFunc = HashFunc;
	m_CompareFunc = CompareFunc;
	m_GetFunc = GetFunc;
	ASSERT(HashFunc != nullptr);
	ASSERT(CompareFunc != nullptr);
	m_NumObjects = 0;
	m_Recursion = 0;
	//
	// Force the number of buckets to a power of 2 so we can replace
	// MOD with an AND.
	//
	for (i = 15; i >= 0; i--)
	{
		m_BucketMask = (1 << i);
		if (NumBuckets & m_BucketMask)
		{
			if (NumBuckets ^ m_BucketMask)
			{
				i++;
				m_BucketMask = (1 << i);
			}
			break;
		}
	}
	ASSERT(i < 16);
	m_NumBuckets = m_BucketMask;
	m_BucketMask--;
	//
	// Allocate and initialize buckets
	//
	if (m_NumBuckets <= (sizeof(m_PreAllocatedBuckets) / sizeof(CMTListNode)))
		m_Buckets = m_PreAllocatedBuckets;
	else
		m_Buckets = new CMTListNode[m_NumBuckets];
	ASSERT(m_Buckets != nullptr);
	for (i = 0; i < m_NumBuckets; i++)
	{
		pBucket = &m_Buckets[i];
		pBucket->m_Next = pBucket;
		pBucket->m_Prev = pBucket;
		pBucket->m_Data = nullptr;
		SET_NODE_IDX(pBucket, i);
		MARK_NODE_DELETED(pBucket); // unusual but it simplifies the lookup routines
	}
}

template <class T, class K>
inline CHash<T, K>::~CHash()
{
	ASSERT(!m_NumObjects);
	CMTListNode* pBucket;
	CMTListNode* node;
	CMTListNode* next;
	uint16_t i;
	// delete buckets
	for (i = 0; i < m_NumBuckets; i++)
	{
		pBucket = &m_Buckets[i];
		for (node = pBucket->m_Next; node != pBucket; node = next)
		{
			next = node->m_Next;
			delete node;
		}
	}
	if (m_NumBuckets > (sizeof(m_PreAllocatedBuckets) / sizeof(CMTListNode)))
		delete[] m_Buckets;
	m_Buckets = nullptr;
}

template <class T, class K>
inline MTListNodeHandle
CHash<T, K>::Add(K Key, T* Object)
{
	CMTListNode* node;
	uint16_t idx = (uint16_t)m_HashFunc(Key) & m_BucketMask;
	CMTListNode* pBucket = &m_Buckets[idx];
	node = new CMTListNode;
	if (!node)
		return nullptr;
	node->m_Data = Object;
	CLEAR_NODE_DELETED(node);
	SET_NODE_IDX(node, idx);
	node->m_Prev = pBucket;
	node->m_Next = pBucket->m_Next;
	pBucket->m_Next = node;
	node->m_Next->m_Prev = node;
	m_NumObjects++;
	return node;
}

template <class T, class K>
inline T*
CHash<T, K>::Get(K Key)
{
	T* Object;
	T* Found;
	CMTListNode* node;
	uint16_t idx = (uint16_t)m_HashFunc(Key) & m_BucketMask;
	CMTListNode* pBucket = &m_Buckets[idx];
	// look for object in bucket list
	Found = nullptr;
	for (node = pBucket->m_Next; node != pBucket; node = node->m_Next)
	{
		ASSERT(GET_NODE_IDX(node) == idx);
		// skip deleted nodes
		if (IS_NODE_DELETED(node))
			continue;
		// node we're looking for?
		Object = (T*)(node->m_Data);
		if (m_CompareFunc(Object, Key))
		{
			if (m_GetFunc)
			{
				m_GetFunc(Object);
			}
			Found = Object;
			break;
		}
	}
	return Found;
}

template <class T, class K>
inline T*
CHash<T, K>::Delete(K Key)
{
	T* Object;
	T* Found;
	CMTListNode* node;
	CMTListNode* next;
	uint16_t idx = (uint16_t)m_HashFunc(Key) & m_BucketMask;
	CMTListNode* pBucket = &m_Buckets[idx];
	Found = nullptr;
	for (node = pBucket->m_Next; node != pBucket; node = next)
	{
		ASSERT(GET_NODE_IDX(node) == idx);
		next = node->m_Next;
		// remove deleted objects
		if (IS_NODE_DELETED(node))
		{
			node->m_Prev->m_Next = next;
			next->m_Prev = node->m_Prev;
			node->m_Prev = nullptr;
			node->m_Next = nullptr;
			delete node;
			continue;
		}
		// node we're looking for?
		Object = (T*)(node->m_Data);
		if (m_CompareFunc(Object, Key))
		{
			Found = Object;
			if (m_Recursion == 0)
			{
				node->m_Prev->m_Next = next;
				next->m_Prev = node->m_Prev;
				node->m_Prev = nullptr;
				node->m_Next = nullptr;
				delete node;
				m_NumObjects--;
			}
			else
			{
				MarkNodeDeleted(node);
			}
			break;
		}
	}
	return Found;
}

template <class T, class K>
inline void
CHash<T, K>::DeleteNode(MTListNodeHandle node, PFDELFUNC pfDelete, PVOID Cookie)
{
	ASSERT(GET_NODE_IDX(node) < m_NumBuckets);
	if (pfDelete && node->m_Data)
	{
		pfDelete((T*)node->m_Data, Cookie);
		node->m_Data = nullptr;
	}
	node->m_Prev->m_Next = node->m_Next;
	node->m_Next->m_Prev = node->m_Prev;
	node->m_Prev = nullptr;
	node->m_Next = nullptr;
	delete node;
	m_NumObjects--;
}

template <class T, class K>
inline void
CHash<T, K>::MarkNodeDeleted(MTListNodeHandle node, PFDELFUNC pfDelete, PVOID Cookie)
{
	if (!IS_NODE_DELETED(node))
	{
		MARK_NODE_DELETED(node);
		if (pfDelete && node->m_Data)
		{
			pfDelete((T*)node->m_Data, Cookie);
		}
		node->m_Data = nullptr;
		m_NumObjects--;
	}
}

template <class T, class K>
inline BOOLEAN
CHash<T, K>::ForEach(PFITERCALLBACK pfCallback, PVOID Cookie)
{
	CMTListNode* pBucket;
	CMTListNode* node;
	uint16_t idx;
	m_Recursion++;
	for (idx = 0; idx < m_NumBuckets; idx++)
	{
		// step through bucket
		pBucket = &m_Buckets[idx];
		for (node = pBucket->m_Next; node != pBucket; node = node->m_Next)
		{
			// skip deleted nodes
			if (IS_NODE_DELETED(node))
				continue;
			if (!pfCallback((T*)node->m_Data, node, Cookie))
			{
				m_Recursion--;
				return false;
			}
		}
	}
	m_Recursion--;
	return true;
}

template <class T, class K>
inline void
CHash<T, K>::RemoveAll(PFDELFUNC pfDelete, PVOID Cookie)
{
	CMTListNode* pBucket;
	CMTListNode* node;
	CMTListNode* next;
	uint16_t idx;
	for (idx = 0; idx < m_NumBuckets; idx++)
	{
		pBucket = &m_Buckets[idx];
		for (node = pBucket->m_Next; node != pBucket; node = next)
		{
			if (pfDelete && node->m_Data)
			{
				pfDelete((T*)node->m_Data, Cookie);
				node->m_Data = nullptr;
			}
			next = node->m_Next;
			node->m_Prev->m_Next = node->m_Next;
			node->m_Next->m_Prev = node->m_Prev;
			node->m_Prev = nullptr;
			node->m_Next = nullptr;
			delete node;
		}
	}
	m_NumObjects = 0;
	;
}

template <class T, class K>
inline void
CHash<T, K>::TrashDay()
{
	CMTListNode* pBucket;
	CMTListNode* node;
	CMTListNode* next;
	uint16_t idx;
	for (idx = 0; idx < m_NumBuckets; idx++)
	{
		// step through buckets deleting marked nodes
		pBucket = &m_Buckets[idx];
		for (node = pBucket->m_Next; node != pBucket; node = next)
		{
			ASSERT(GET_NODE_IDX(node) == idx);
			next = node->m_Next;
			if (IS_NODE_DELETED(node))
			{
				node->m_Prev->m_Next = next;
				next->m_Prev = node->m_Prev;
				node->m_Prev = nullptr;
				node->m_Next = nullptr;
				delete node;
			}
		}
	}
}

//#pragma warning( default : 4244 )

#endif //!__HASH_H__
