//===========================================================================//
// File:	memblock.hh                                                      //
// Contents: Interface specification of the memory block class               //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _MEMORYBLOCK_HPP_
#define _MEMORYBLOCK_HPP_

#include <stuff/style.hpp>

#ifndef _GAMEOS_HPP_
typedef struct	gos_Heap*		HGOSHEAP;
extern HGOSHEAP ParentClientHeap;
#endif

namespace Stuff {

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MemoryBlockHeader ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class MemoryBlockHeader
	{
	public:
		MemoryBlockHeader*	nextBlock;
		size_t				blockSize;
		void TestInstance() {}
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MemoryBlockBase ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class MemoryBlockBase
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		void TestInstance(void) {Verify(blockMemory != NULL);}

		static void UsageReport(void);
		static void	CollapseBlocks(void);

	protected:
		PCSTR				blockName;
		MemoryBlockHeader*	blockMemory;			// the first record block allocated

		size_t	blockSize;				// size in bytes of the current record block
		size_t	recordSize;				// size in bytes of the individual record
		size_t	deltaSize;				// size in bytes of the growth blocks

		puint8_t	firstHeaderRecord;	// the beginning of useful free space
		puint8_t	freeRecord;			// the next address to allocate from the block
		puint8_t	deletedRecord;		// the next record to reuse

		MemoryBlockBase(
			size_t rec_size, size_t start, size_t delta, PCSTR name, 
			HGOSHEAP parent = ParentClientHeap);
		~MemoryBlockBase(void);

		PVOID Grow(void);
		HGOSHEAP blockHeap;

	private:
		static MemoryBlockBase*	firstBlock;
		MemoryBlockBase*		nextBlock;
		MemoryBlockBase*		previousBlock;
		void Collapse(void);
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MemoryBlock ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class MemoryBlock:
		public MemoryBlockBase
	{
	public:
		static bool TestClass(void);

		MemoryBlock(
			size_t rec_size,
			size_t start,
			size_t delta,
			PCSTR name,
			HGOSHEAP parent = ParentClientHeap
			):
		MemoryBlockBase(rec_size, start, delta, name, parent)
		{}

		PVOID
			New();
		void
			Delete(PVOID Where);

		PVOID
			operator[](size_t Index);
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MemoryBlockOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T> class MemoryBlockOf:
	public MemoryBlock
	{
	public:
		MemoryBlockOf(
			size_t start,
			size_t delta,
			PCSTR name,
			HGOSHEAP parent = ParentClientHeap
			):
		MemoryBlock(sizeof(T), start, delta, name, parent)
		{}

		T*
			New()
		{return Cast_Pointer(T*, MemoryBlock::New());}
		void
			Delete(PVOID where)
		{MemoryBlock::Delete(where);}

		T*
			operator[](size_t index)
		{return Cast_Pointer(T*, MemoryBlock::operator[](index));}
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MemoryStack ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class MemoryStack:
		public MemoryBlockBase
	{
	public:
		static bool
			TestClass();

	protected:
		UCHAR
			*topOfStack;

		MemoryStack(
			size_t rec_size,
			size_t start,
			size_t delta,
			PCSTR name,
			HGOSHEAP parent = ParentClientHeap
			):
		MemoryBlockBase(rec_size, start, delta, name, parent)
		{topOfStack = NULL;}

		PVOID
			Push(PCVOID What);
		PVOID
			Push();
		PVOID
			Peek()
		{return topOfStack;}
		void
			Pop();
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MemoryStackOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T> class MemoryStackOf:
	public MemoryStack
	{
	public:
		MemoryStackOf(
			size_t start,
			size_t delta,
			PCSTR name,
			HGOSHEAP parent = ParentClientHeap
			):
		MemoryStack(sizeof(T), start, delta, name, parent)
		{}

		T*
			Push(const T *what)
		{return Cast_Pointer(T*, MemoryStack::Push(what));}
		T*
			Peek()
		{return static_cast<T*>(MemoryStack::Peek());}
		void
			Pop()
		{MemoryStack::Pop();}
	};

}
#endif
