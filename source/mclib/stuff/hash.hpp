//===========================================================================//
// File:	hash.hh                                                          //
// Contents:                                                                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _HASH_HPP_
#define _HASH_HPP_

#include <stuff/sortedchain.hpp>

namespace GetHashFunctions {
	inline Stuff::IteratorPosition
		GetHashValue(size_t value_to_hash)
	{
		return value_to_hash;
	}
};

namespace Stuff {

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Hash ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class Hash:
		public SortedSocket
	{
		friend class HashIterator;

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
		// Constructor, Destructor and Testing
		//--------------------------------------------------------------------
		//
		Hash(
	      	CollectionSize size,
			Node *node,
			bool has_unique_entries
		);
		~Hash(void);

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

		//
		//-----------------------------------------------------------------------
		// Unimplemented
		//-----------------------------------------------------------------------
		//
		void
			RemovePlug(Plug *plug);
		bool
			IsPlugMember(Plug *plug);

	protected:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		void
			AddImplementation(Plug *plug);
		void
			AddValueImplementation(
				Plug *plug,
				PCVOID value
			);
		Plug
			*FindImplementation(PCVOID value);

		//
		//--------------------------------------------------------------------
		// Protected data
		//--------------------------------------------------------------------
		//
		SortedChain **hashTable;
		CollectionSize hashTableSize;

	private:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Private interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		virtual SortedChain*
			MakeSortedChain(void);

		virtual IteratorPosition
			GetHashIndex(PCVOID value);

		void
			BuildHashTable(void);

		bool
			CheckForPrimeSize(void);
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Hash inlines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	inline void
		Hash::RemovePlug(Plug* /*plug*/)
	{
#ifdef _GAMEOS_HPP_
		STOP(("Hash::RemovePlug - no efficient implementation, use iterator find & remove"));
#endif
	}

	inline bool
		Hash::IsPlugMember(Plug* /*plug*/)
	{
#ifdef _GAMEOS_HPP_
		STOP(("Hash::IsPlugMember - no efficient implementation, use find"));
#endif
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HashOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V> class HashOf:
		public Hash
	{
	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		HashOf(
			CollectionSize size,
			Node *node,
			bool has_unique_entries
		);
		~HashOf(void);

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
		SortedChain*
			MakeSortedChain(void);

		IteratorPosition
			GetHashIndex(PCVOID value);
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HashOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V>
		HashOf<T, V>::HashOf(
			CollectionSize size,
			Node *node,
			bool has_unique_entries
		):
			Hash(
				size,
				node,
				has_unique_entries
			)
	{
	}

	template <class T, class V>
		HashOf<T, V>::~HashOf()
	{
		#if defined(_ARMOR)
			CollectionSize over_loaded_bins = 0;
			for (size_t i = 0; i < hashTableSize; i++)
			{
				Check_Pointer(hashTable);
				if (hashTable[i] != nullptr)
				{
					Check_Object(hashTable[i]);
					SortedChainIteratorOf<T, V> 
						iterator((SortedChainOf<T, V>*)hashTable[i]);
					if (iterator.GetSize() > 6)
					{
						over_loaded_bins++;
					}
				}
			}
			Warn(over_loaded_bins != 0);
		#endif
	}

	template <class T, class V> SortedChain*
		HashOf<T, V>::MakeSortedChain()
	{
		return new SortedChainOf<T, V>(GetReleaseNode(), HasUniqueEntries());
	}

	template <class T, class V> IteratorPosition
		HashOf<T, V>::GetHashIndex(PCVOID value)
	{
		Check_Pointer(value);
		return (GetHashFunctions::GetHashValue(*Cast_Pointer(const V*, value)) % hashTableSize);
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HashIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class HashIterator:
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
		explicit HashIterator(Hash *hash);
		Iterator*
			MakeClone(void);
		~HashIterator(void);
		void TestInstance(void);

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
			GetCurrentImplementation(void);
		Plug*
			FindImplementation(PCVOID value);

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

		void
			DeleteSortedChainIterator(void);

		void
			NextSortedChainIterator(IteratorPosition index);

		//
		//--------------------------------------------------------------------
		// Private data
		//--------------------------------------------------------------------
		//
		SortedChain
			**hashTable;
		CollectionSize
			hashTableSize;
		IteratorPosition
			currentPosition;
		SortedChainIterator
			*vchainIterator;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ HashIteratorOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V> class HashIteratorOf:
		public HashIterator
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
		explicit HashIteratorOf(HashOf<T, V> *hash);
		Iterator*
			MakeClone(void);
		~HashIteratorOf(void);

		//
		//--------------------------------------------------------------------
		// Iterator methods (see Iterator for full listing)
		//--------------------------------------------------------------------
		//
		T
			ReadAndNext()
				{return (T)ReadAndNextImplementation(void);}
		T
			ReadAndPrevious()
				{return (T)ReadAndPreviousImplementation(void);}
		T
			GetCurrent()
				{return (T)GetCurrentImplementation(void);}
		T
			GetNth(CollectionSize index)
				{return (T)GetNthImplementation(index);}
		T
			Find(const V &value)
				{return (T)FindImplementation(&value);}
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~ HashIteratorOf templates ~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T, class V>
		HashIteratorOf<T, V>::HashIteratorOf(HashOf<T, V> *hash):
			HashIterator(hash)
	{
	}

	template <class T, class V> Iterator*
		HashIteratorOf<T, V>::MakeClone()
	{
		return new HashIteratorOf<T,V>(*this);
	}

	template <class T, class V>
		HashIteratorOf<T, V>::~HashIteratorOf()
	{
	}

}

#endif
