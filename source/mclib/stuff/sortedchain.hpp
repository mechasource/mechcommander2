//===========================================================================//
// File:	vchain.hh                                                        //
// Contents: Interface specification of SortedChain class                    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _SORTEDCHAIN_HPP_
#define _SORTEDCHAIN_HPP_

#include <stuff/link.hpp>
#include <stuff/iterator.hpp>
#include <stuff/memoryblock.hpp>
#include <stuff/sortedsocket.hpp>

namespace Stuff {


	// dependent external classes
	class MemoryBlock;

	// forward declaration of classes in this class
	class SortedChain;
	class SortedChainIterator;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainLink ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SortedChainLink:
		public Link
	{
		friend class SortedChain;
		friend class SortedChainIterator;

	public:
		~SortedChainLink(void);
		void TestInstance(void);

	protected:
		SortedChainLink(
			SortedChain *vchain,
			Plug *plug
			);

	private:
		void
			SetupSortedChainLinks(
			SortedChainLink *next,
			SortedChainLink *prev
			);

		SortedChainLink *next;
		SortedChainLink *prev;
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainLinkOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	enum {VChainLink_Memoryblock_Allocation=100};

	template <class V> class SortedChainLinkOf:
	public SortedChainLink
	{
	public:
		SortedChainLinkOf(
			SortedChain *vchain,
			Plug *plug,
			const V &value
			);
		~SortedChainLinkOf(void);

		PVOID
			operator new(size_t);
		void
			operator delete(PVOID where);

		V
			GetValue()
		{return value;}
		V*
			GetValuePointer()
		{return &value;}

	private:
		static MemoryBlock*		allocatedMemory;
		static CollectionSize	allocationCount;
		V value;
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainLinkOf templates ~~~~~~~~~~~~~~~~~~~~~~~~

	template <class V> MemoryBlock*
		SortedChainLinkOf<V>::allocatedMemory = NULL;
	template <class V> CollectionSize
		SortedChainLinkOf<V>::allocationCount = 0;

	template <class V>
	SortedChainLinkOf<V>::SortedChainLinkOf(
		SortedChain *vchain,
		Plug *plug,
		const V &value
		):
	SortedChainLink(vchain, plug)
	{
		this->value = value;
	}

	template <class V>
	SortedChainLinkOf<V>::~SortedChainLinkOf()
	{
	}

	template <class V> PVOID
		SortedChainLinkOf<V>::operator new(size_t)
	{
		Verify(allocationCount >= 0);
		if (allocationCount++ == 0)
		{
			allocatedMemory =
				new MemoryBlock(
				sizeof(SortedChainLinkOf<V>),
				VChainLink_Memoryblock_Allocation,
				VChainLink_Memoryblock_Allocation,
				"Stuff::SortedChainLinkOf",
				Stuff::ConnectionEngineHeap);
			Register_Object(allocatedMemory);
		}
		Verify(allocationCount < INT_MAX);
		Check_Object(allocatedMemory);
		return allocatedMemory->New();
	}

	template <class V> void
		SortedChainLinkOf<V>::operator delete(PVOID where)
	{
		Check_Object(allocatedMemory);
		allocatedMemory->Delete(where);
		if (--allocationCount == 0)
		{
			Unregister_Object(allocatedMemory);
			delete allocatedMemory;
			allocatedMemory = NULL;
		}
		Verify(allocationCount >= 0);
	}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChain ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SortedChain:
		public SortedSocket
	{
		friend class SortedChainLink;
		friend class SortedChainIterator;

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
		SortedChain(
			Node *node,
			bool has_unique_entries
			);
		~SortedChain(void);

		void TestInstance(void);
		static bool
			TestClass(void);
		static bool
			ProfileClass(void);

		//
		//-----------------------------------------------------------------------
		// IsEmpty - Returns true if the socket contains no plugs.
		//-----------------------------------------------------------------------
		//
		bool
			IsEmpty(void);

	protected:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		void
			AddImplementation(Plug *);
		void
			AddValueImplementation(
			Plug *plug,
			PCVOID value
			);
		Plug
			*FindImplementation(PCVOID value);

	private:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Private interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		virtual SortedChainLink
			*MakeSortedChainLink(
			Plug *plug,
			PCVOID value
			);
		virtual int32_t
			CompareSortedChainLinks(
			SortedChainLink *link1,
			SortedChainLink *link2
			);
		virtual int32_t
			CompareValueToSortedChainLink(
			PCVOID value,
			SortedChainLink *link
			);

		SortedChainLink*
			SearchForValue(PCVOID value);

		//
		//--------------------------------------------------------------------
		// Private data
		//--------------------------------------------------------------------
		//
		SortedChainLink *head;
		SortedChainLink *tail;
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	enum {VChain_Memoryblock_Allocation=100};

	template <class T, class V> class SortedChainOf:
	public SortedChain
	{
	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		SortedChainOf(
			Node *node,
			bool has_unique_entries
			);
		~SortedChainOf(void);

		PVOID	operator new(size_t);
		void	operator delete(PVOID where);

		//
		//--------------------------------------------------------------------
		// Socket methods (see Socket for full listing)
		//--------------------------------------------------------------------
		//
		void
			AddValue(
			T plug,
			const V &value
			)
		{AddValueImplementation(Cast_Object(Plug*,plug), &value);}
		void
			Remove(T plug)
		{RemovePlug(Cast_Object(Plug*,plug));}
		T
			Find(const V &value)
		{return (T)FindImplementation(&value);}

	private:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Private interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		SortedChainLink
			*MakeSortedChainLink(
			Plug *plug,
			PCVOID value
			)
		{
			return
				new SortedChainLinkOf<V>(
				this,
				plug,
				*Cast_Pointer(const V*, value)
				);
		}
		int32_t
			CompareSortedChainLinks(
			SortedChainLink *link1,
			SortedChainLink *link2
			);
		int32_t
			CompareValueToSortedChainLink(
			PCVOID value,
			SortedChainLink *link
			);

		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Private data
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		static MemoryBlock
			*allocatedMemory;
		static CollectionSize
			allocationCount;
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V> MemoryBlock*
		SortedChainOf<T, V>::allocatedMemory = NULL;
	template <class T, class V> CollectionSize
		SortedChainOf<T, V>::allocationCount = 0;

	template <class T, class V>
	SortedChainOf<T, V>::SortedChainOf(
		Node *node,
		bool has_unique_entries
		):
	SortedChain(
		node,
		has_unique_entries
		)
	{
	}

	template <class T, class V>
	SortedChainOf<T, V>::~SortedChainOf(void)
	{
	}

	template <class T, class V> int32_t
		SortedChainOf<T, V>::CompareSortedChainLinks(
		SortedChainLink *node1,
		SortedChainLink *node2
		)
	{
		V *ptr1 = Cast_Object(SortedChainLinkOf<V>*, node1)->GetValuePointer();
		V *ptr2 = Cast_Object(SortedChainLinkOf<V>*, node2)->GetValuePointer();

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

	template <class T, class V> int32_t
		SortedChainOf<T, V>::CompareValueToSortedChainLink(
		PCVOID value,
		SortedChainLink *node
		)
	{
		Check_Pointer(value);

		if (value){
			V *ptr = Cast_Object(SortedChainLinkOf<V>*, node)->GetValuePointer();
			Check_Pointer(ptr);
			if (ptr)
			{
				if (*Cast_Pointer(const V*, value) == *ptr)
					return 0;
				else
					return (*Cast_Pointer(const V*, value) > *ptr) ? 1 : -1;
			}
		}
		return 0;
	}

	template <class T, class V> PVOID
		SortedChainOf<T, V>::operator new(size_t)
	{
		Verify(allocationCount >= 0);
		if (allocationCount++ == 0)
		{
			allocatedMemory =
				new MemoryBlock(
				sizeof(SortedChainOf<T, V>),
				VChain_Memoryblock_Allocation,
				VChain_Memoryblock_Allocation,
				"Stuff::SortedChainOf",
				Stuff::ConnectionEngineHeap
				);
			Register_Object(allocatedMemory);
		}
		Verify(allocationCount < INT_MAX);
		Check_Object(allocatedMemory);
		return allocatedMemory->New();
	}

	template <class T, class V> void
		SortedChainOf<T, V>::operator delete(PVOID where)
	{
		Check_Object(allocatedMemory);
		allocatedMemory->Delete(where);
		if (--allocationCount == 0)
		{
			Unregister_Object(allocatedMemory);
			delete allocatedMemory;
			allocatedMemory = NULL;
		}
		Verify(allocationCount >= 0);
	}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SortedChainIterator:
		public SortedIterator
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
		explicit SortedChainIterator(SortedChain *vchain);
		explicit SortedChainIterator(const SortedChainIterator *iterator);
		Iterator*
			MakeClone(void);
		~SortedChainIterator(void);
		void TestInstance(void) const;

		//
		//--------------------------------------------------------------------
		// Iterator methods (see Iterator for full listing)
		//--------------------------------------------------------------------
		//
		void
			First(void);
		void
			Last(void);
		void
			Next(void);
		void
			Previous(void);
		CollectionSize
			GetSize(void);
		void
			Remove(void);

	protected:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		PVOID
			ReadAndNextImplementation(void);
		PVOID
			ReadAndPreviousImplementation(void);
		PVOID
			GetCurrentImplementation(void);
		PVOID
			GetNthImplementation(
			CollectionSize index
			);
		Plug*
			FindImplementation(PCVOID value);

	protected:
		//
		//--------------------------------------------------------------------
		// Protected data
		//--------------------------------------------------------------------
		//
		SortedChainLink
			*currentLink;

	private:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Private interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		void
			ReceiveMemo(
			IteratorMemo memo,
			PVOID content
			);
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainIteratorOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V> class SortedChainIteratorOf:
	public SortedChainIterator
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
		explicit SortedChainIteratorOf(SortedChainOf<T, V> *vchain);
		explicit SortedChainIteratorOf(const SortedChainIteratorOf<T, V> &iterator);
		Iterator*
			MakeClone(void);
		~SortedChainIteratorOf(void);

		//
		//--------------------------------------------------------------------
		// Iterator methods (see Iterator for full listing)
		//--------------------------------------------------------------------
		//
		T ReadAndNext(void)				{return (T)ReadAndNextImplementation();}
		T ReadAndPrevious(void)			{return (T)ReadAndPreviousImplementation();}
		T GetCurrent(void)				{return (T)GetCurrentImplementation();}
		T GetNth(CollectionSize index) 	{return (T)GetNthImplementation(index);}
		T Find(const V &value)			{return (T)FindImplementation(&value);}
		V GetValue()					
		{
			return Cast_Object(SortedChainLinkOf<V>*, currentLink)->GetValue();
		}
	};

//~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainIteratorOf templates ~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V>
	SortedChainIteratorOf<T, V>::SortedChainIteratorOf(SortedChainOf<T, V> *vchain):
	SortedChainIterator(vchain)
	{
	}

	template <class T, class V>
	SortedChainIteratorOf<T, V>::SortedChainIteratorOf(
		const SortedChainIteratorOf<T, V> &iterator
		):
	SortedChainIterator(&iterator)
	{
	}

	template <class T, class V> Iterator*
		SortedChainIteratorOf<T, V>::MakeClone()
	{
		return new SortedChainIteratorOf<T, V>(*this);
	}

	template <class T, class V>
	SortedChainIteratorOf<T, V>::~SortedChainIteratorOf()
	{
	}

}
#endif
