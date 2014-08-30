//===========================================================================//
// File:     namelist.hpp                                                    //
// Title:    Declaration of NameList classes.                                //
// Purpose:  Maintains an unsorted list of strings with (PVOID) to          //
//           anything the client needs to associate with the string.         //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _NAMELIST_HPP_
#define _NAMELIST_HPP_

#include <stuff/memorystream.hpp>

namespace Stuff {class NameList;}

namespace MemoryStreamIO
{
	Stuff::MemoryStream&
		Read(
		Stuff::MemoryStream *stream,
		Stuff::NameList *names
		);
	Stuff::MemoryStream&
		Write(
		Stuff::MemoryStream *stream,
		const Stuff::NameList *names
		);
}

namespace Stuff {

	class ObjectNameList;
	class ObjectNameList__Entry;
	class NameList;
	class AlphaNameList;

	//=======================================================================
	// Notice:
	//		In using ObjectNameList class, think of AddEntry sort of like a New
	// in the sense that you MUST store the (PSTR ) it returns in the name
	// field of the class you are bonding ObjectNameList to so that when you
	// DeleteEntry you can pass that exact same pointer back.  DeleteEntry
	// does not do a find on the name you pass it.  It expects to receive the
	// pointer that AddEntry gave to you.
	//    This restriction does not exist for NameList and AlphaNameList.
	//-----------------------------------------------------------------------
	// Another Notice:
	//		NameList::Entry objects are never _constructed_ and therefore
	// Check_Pointer(this) is used instead of Check_Object(this).  Also Register_
	// Pointer and Unregister_Pointer are used instead of Register_Object
	// and Unregister_Object.
	//=======================================================================

	//##########################################################################
	//##############    ObjectNameList    ######################################
	//##########################################################################

	class ObjectNameList
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		typedef ObjectNameList__Entry    Entry;

	protected:
		Entry
			*firstEntry,
			*lastEntry;

	public:
		ObjectNameList(void);
		virtual
			~ObjectNameList(void);

		virtual PCSTR
			AddEntry(
			PCSTR name,
			PVOID data
			);
		PVOID
			FindObject(PCSTR name);
		void
			DeleteEntry(PCSTR name);	// ** DANGEROUS!! see notice above **
		int32_t
			GetEntryCount(void) const;	// (implementation assumes infrequent use)
		bool
			IsEmpty(void) const
		{ Check_Object(this); return firstEntry == nullptr && lastEntry == nullptr; }
		Entry*
			GetFirstEntry()
		{ Check_Object(this); return firstEntry; }
		const Entry*
			GetFirstEntry(void) const
		{ Check_Object(this); return firstEntry; }
		Entry*
			GetLastEntry()
		{ Check_Object(this); return lastEntry; }
		const Entry*
			GetLastEntry(void) const
		{ Check_Object(this); return lastEntry; }
		int32_t
			BuildSubList(
			const ObjectNameList &source_list,
			PCSTR prefix
			);

		void TestInstance(void) const {}
		static bool
			TestClass(void);
	};

	//##########################################################################
	//##############    ObjectNameList::Entry    ###############################
	//##########################################################################

	class ObjectNameList__Entry
	{
		friend class ObjectNameList;
		friend class NameList;
		friend class AlphaNameList;

	private:
		ObjectNameList::Entry*	nextEntry;

	public:
		PVOID	dataReference;

	protected:
		void SetName(PCSTR name);

	public:
		PCSTR GetName(void) const
		{
			Check_Object(this);
			return &(
				Cast_Pointer(PCSTR, this)[
					sizeof(ObjectNameList::Entry)
				]
			);
		}
		bool
			IsName(PCSTR name) const;
		PVOID
			GetObject()
		{ Check_Object(this); return dataReference; }
		PVOID
			GetData()
		{ Check_Object(this); return dataReference; }
		PCVOID
			GetData(void) const
		{ Check_Object(this); return dataReference; }
		PSTR
			GetChar()
		{Check_Object(this); return Cast_Pointer(PSTR ,dataReference);}
		PCSTR
			GetChar(void) const
		{
			Check_Object(this);
			return Cast_Pointer(PCSTR ,dataReference);
		}
		int32_t
			GetAtoi(void) const
		{
			Check_Object(this); Check_Pointer(dataReference);
			return atoi(Cast_Pointer(PCSTR , dataReference));
		}
		int32_t
			GetAtol(void) const
		{
			Check_Object(this); Check_Pointer(dataReference);
			return atol(Cast_Pointer(PCSTR , dataReference));
		}
		float GetAtof(void) const
		{
			Check_Object(this); Check_Pointer(dataReference);
			return float(atof(Cast_Pointer(PCSTR, dataReference)));
		}
		ObjectNameList::Entry*
			GetNextEntry()
		{ Check_Object(this); return nextEntry; }
		const ObjectNameList::Entry*
			GetNextEntry(void) const
		{ Check_Object(this); return nextEntry; }
		void TestInstance(void) const {}
	};

	//##########################################################################
	//##############    NameList    ############################################
	//##########################################################################

	class NameList:
		public ObjectNameList
	{
	public:
		NameList(void);
		~NameList(void);

		PVOID
			FindData(PCSTR name)
		{ return FindObject(name); }
		PCSTR
			FindName(PVOID data);
		Entry*
			FindEntry(PCSTR name);
		int32_t
			FindEntryIndex(PCSTR name);
		Entry*
			FindEntry(PVOID data);
		void
			DeleteEntry(PCSTR name);	// this one is searches for name
		static bool
			TestClass(void);

		friend MemoryStream&
			MemoryStreamIO::Read(
			MemoryStream *stream,
			NameList *names
			);
		friend MemoryStream&
			MemoryStreamIO::Write(
			MemoryStream *stream,
			const NameList *names
			);
	};

	//##########################################################################
	//##############    AlphaNameList    #######################################
	//##########################################################################

	class AlphaNameList:
		public NameList
	{
	public:
		AlphaNameList(void);
		~AlphaNameList(void);

		PCSTR
			AddEntry(
			PCSTR name,
			PVOID data
			);
		static bool
			TestClass(void);
	};

}
#endif
