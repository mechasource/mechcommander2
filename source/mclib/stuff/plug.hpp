//===========================================================================//
// File:	plug.hpp                                                         //
// Contents: Interface specifications for plugs and their iterators          //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _PLUG_HPP_
#define _PLUG_HPP_

#include <stuff/registeredclass.hpp>
#include <stuff/iterator.hpp>

namespace Stuff
{

//##########################################################################
//############################    Plug    ##################################
//##########################################################################

class PlugIterator;

typedef RegisteredClass__ClassData Plug__ClassData;

extern struct gos_Heap* ConnectionEngineHeap;

class Plug : public RegisteredClass
{
	friend class Link;
	friend class PlugIterator;
	friend class Socket;

  public:
	static void InitializeClass(void);
	static void TerminateClass(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor/Destructor
	//
  public:
	~Plug(void);

  protected:
	explicit Plug(ClassData* class_data);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
  public:
	typedef Plug__ClassData ClassData;

	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Utilities
	//
  public:
	CollectionSize GetSocketCount(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
  public:
	void TestInstance(void) const;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Private methods
	//
  private:
	void RemoveSocket(Socket* socket);
	bool IsSocketMember(Socket* socket);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Private data
	//
  private:
	Link* linkHead;
};

//##########################################################################
//############################    PlugOf    ################################
//##########################################################################

template <class T> class PlugOf : public Plug
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor, Destructor
	//
  public:
	//
	//--------------------------------------------------------------------
	// Constructor, Destructor
	//--------------------------------------------------------------------
	//
	explicit PlugOf(const T& item);
	~PlugOf(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Accessors, Casting
	//
  public:
	T GetItem(void) const { return item; }

	T* GetPointer(void) { return &item; }

	operator T(void) const { return item; }

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Private data
	//
  private:
	T item;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ PlugOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T> PlugOf<T>::PlugOf(const T& the_item) : Plug(DefaultData)
{
	item = the_item;
}

template <class T> PlugOf<T>::~PlugOf(void) {}

//##########################################################################
//########################    PlugIterator    ##############################
//##########################################################################

class PlugIterator : public Iterator
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor, destructor & testing
	//
  public:
	explicit PlugIterator(
		Plug* plug, RegisteredClass::ClassID class_to_iterate = NullClassID);
	PlugIterator(const PlugIterator& iterator);
	~PlugIterator(void);

	void TestInstance(void) const;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Iterator methods (see Iterator for full listing)
	//
  public:
	void First(void);
	void Last(void);
	void Next(void);
	void Previous(void);
	CollectionSize GetSize(void);
	void Remove(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Protected implementation
	//
  protected:
	PVOID ReadAndNextImplementation(void);
	PVOID ReadAndPreviousImplementation(void);
	PVOID GetCurrentImplementation(void);
	PVOID GetNthImplementation(CollectionSize index);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Protected data
	//
  protected:
	Plug* plug;
	Link* currentLink;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Private methods and data
	//
  private:
	void NextNode(void);
	void PreviousNode(void);

	RegisteredClass::ClassID classToIterate;
};

//##########################################################################
//#######################    PlugIteratorOf    #############################
//##########################################################################

template <class T> class PlugIteratorOf : public PlugIterator
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor, destructor
	//
  public:
	PlugIteratorOf(
		Plug* plug, RegisteredClass::ClassID class_to_iterate = NullClassID);
	PlugIteratorOf(
		Plug& plug, RegisteredClass::ClassID class_to_iterate = NullClassID);

	PlugIteratorOf(const PlugIteratorOf<T>& iterator);
	~PlugIteratorOf(void);

	Iterator* MakeClone(void)
	{
		// Check_Object(this);
		return new PlugIteratorOf<T>(*this);
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Iterator methods (see Iterator for full listing)
	//
  public:
	T ReadAndNext(void) { return (T)ReadAndNextImplementation(void); }

	T ReadAndPrevious(void) { return (T)ReadAndPreviousImplementation(void); }

	T GetCurrent(void) { return (T)GetCurrentImplementation(void); }

	T GetNth(CollectionSize index) { return (T)GetNthImplementation(index); }
};

//~~~~~~~~~~~~~~~~~~~~~~~ PlugIteratorOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
PlugIteratorOf<T>::PlugIteratorOf(
	Plug* plug, RegisteredClass::ClassID class_to_iterate)
	: PlugIterator(plug, class_to_iterate)
{
}

template <class T>
PlugIteratorOf<T>::PlugIteratorOf(
	Plug& plug, RegisteredClass::ClassID class_to_iterate)
	: PlugIterator(&plug, class_to_iterate)
{
}

template <class T>
PlugIteratorOf<T>::PlugIteratorOf(const PlugIteratorOf<T>& iterator)
	: PlugIterator(iterator)
{
}

template <class T> PlugIteratorOf<T>::~PlugIteratorOf(void) {}
}
#endif
