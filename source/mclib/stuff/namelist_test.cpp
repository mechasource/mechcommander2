//===========================================================================//
// File:     namelist.tst                                                    //
// Title:    Definition of NameList TestClass methods.                       //
// Purpose:  Maintains an unsorted list of strings with (PVOID) to          //
//           anything the client needs to associate with the string.         //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "stuff/stuffheaders.h"
#include "namelist.hpp"

#define TEST_COUNT 50

//#############################################################################
//##############    TestHost    ###############################################
//#############################################################################

class TestHost
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
public:
	//--------------------------------------------------------------------
	// ObjectNameList is designed to bond with another class to provide
	// dymanic naming of the objects represented by that class.  TestHost
	// provides a means of testing the functionality of ObjectNameList.
	//--------------------------------------------------------------------
	std::wstring_view name;
	TestHost* next;
	static ObjectNameList names;

	TestHost(std::wstring_view a_name);
	~TestHost();

	bool TestInstance(void) const;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ObjectNameList TestHost::names;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
TestHost::TestHost(std::wstring_view a_name)
{
	// Check_Pointer(this);
	Check_Pointer(a_name);
	name = names.AddEntry(a_name, this);
	next = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
TestHost::~TestHost()
{
	// Check_Object(this);
	names.DeleteEntry(name);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool TestHost::TestInstance(void) const
{
	return true;
}

//#############################################################################
//##############    ObjectNameList::TestClass    ##############################
//#############################################################################

bool ObjectNameList::TestClass()
{
	SPEW((GROUP_STUFF_TEST, "Starting ObjectNameList test..."));
	SPEW((GROUP_STUFF_TEST, "ObjectNameList test is stubbed out..."));
#if 0
	TestHost
	*host,
	*save_host;
	ObjectNameList::Entry
	*next_entry,
	*entry,
	*save_entry = nullptr;
	NameList
	sub_list;
	int32_t
	freq,
	count,
	i;
	div_t
	mod;
	wchar_t
	name[20],
		 save_name[20];
	//----------------------
	// initialize variables
	//----------------------
	save_host = nullptr;
	freq = TEST_COUNT / 10;
	count = 0;
	//--------------------------------
	// add objects to test and delete
	// a few in the process
	//--------------------------------
	Test_Assumption(TestHost::names.IsEmpty());
	for(i = 0; i < TEST_COUNT; i++)
	{
		//-------------------------------------
		// reset stream to beginning of buffer
		// and create the object's name
		//-------------------------------------
		stream.clear();
		stream.seekp(0);
		stream << "NAME" << i << ends;
		host = new TestHost(name);
		Register_Object(host);
		count++;
		mod = div(i, freq);
		if(mod.rem == 1)
		{
			//-----------------------------------------------
			// save name and host pointer for later deletion
			//-----------------------------------------------
			Str_Copy(save_name, name, sizeof(save_name));
			save_host = host;
		}
		if(mod.rem == 0 && save_host != nullptr)
		{
			//----------------------------------------------
			// find and delete host object previously saved
			//----------------------------------------------
			host = Cast_Pointer(TestHost*, TestHost::names.FindObject(save_name));
			Test_Assumption(host == save_host);
			Unregister_Object(host);
			delete host;
			count--;
		}
	}
	Test_Assumption(!TestHost::names.IsEmpty());
	//-----------------------
	// check count and names
	//-----------------------
	Test_Assumption(count == TestHost::names.GetEntryCount());
	i = count / 2;
	entry = TestHost::names.GetFirstEntry();
	while(entry)
	{
		Check_Pointer(entry);	// special case (entry is not constructed)
		host = Cast_Pointer(TestHost*, entry->GetObject());
		Check_Pointer(host->name);
		Check_Pointer(entry->GetName());
		Test_Assumption(host->name == entry->GetName());
		if((--i) == 0)
		{
			save_entry = entry;
		}
		entry = entry->GetNextEntry();
	}
	//--------------------------------------
	// delete middle, first and last object
	//--------------------------------------
	entry = save_entry;
	if(entry)
	{
		Check_Pointer(entry);	// special case (entry is not constructed)
		host = Cast_Pointer(TestHost*, entry->GetObject());
		Unregister_Object(host);
		delete host;
		count--;
	}
	entry = TestHost::names.GetFirstEntry();
	if(entry)
	{
		Check_Pointer(entry);	// special case (entry is not constructed)
		host = Cast_Pointer(TestHost*, entry->GetObject());
		Unregister_Object(host);
		delete host;
		count--;
	}
	entry = TestHost::names.GetLastEntry();
	if(entry)
	{
		Check_Pointer(entry);	// special case (entry is not constructed)
		host = Cast_Pointer(TestHost*, entry->GetObject());
		Unregister_Object(host);
		delete host;
		count--;
	}
	//------------------------
	// add a few more objects
	//------------------------
	for(i = TEST_COUNT; i < TEST_COUNT + freq; i++)
	{
		//-------------------------------------
		// reset stream to beginning of buffer
		// and create the object's name
		//-------------------------------------
		stream.clear();
		stream.seekp(0);
		stream << "TEST" << i << ends;
#if defined(_ARMOR)
		host =
#endif
			new TestHost(name);
		Register_Object(host);
		count++;
	}
	//-----------------
	// create sub list
	//-----------------
	i = sub_list.BuildSubList(TestHost::names, "TEST");
	Test_Assumption(i == freq);
	Test_Assumption(i == sub_list.GetEntryCount());
	//-----------------------------
	// check count and names again
	// and delete as we go
	//-----------------------------
	Test_Assumption(count == TestHost::names.GetEntryCount());
	entry = TestHost::names.GetFirstEntry();
	while(entry)
	{
		Check_Pointer(entry);	// special case (entry is not constructed)
		host = Cast_Pointer(TestHost*, entry->GetObject());
		Check_Pointer(host->name);
		Check_Pointer(entry->GetName());
		Test_Assumption(host->name == entry->GetName());
		next_entry = entry->GetNextEntry();
		if(!next_entry)
		{
			Test_Assumption(entry == TestHost::names.GetLastEntry());
			Test_Assumption(entry == TestHost::names.GetFirstEntry());
		}
		Unregister_Object(host);
		delete host;
		count--;
		entry = next_entry;
	}
	Test_Assumption(count == 0);
#endif
	//	Tell("  ObjectNameList::TestClass() is stubbed out!\n");
	return true;
}

//#############################################################################
//##############    NameList::TestClass    ####################################
//#############################################################################

bool NameList::TestClass()
{
	SPEW((GROUP_STUFF_TEST, "Starting NameList test..."));
	SPEW((GROUP_STUFF_TEST, "  NameList::TestClass() is stubbed out!"));
	return false;
}

//#############################################################################
//##############    AlphaNameList::TestClass    ###############################
//#############################################################################

bool AlphaNameList::TestClass()
{
	SPEW((GROUP_STUFF_TEST, "Starting AlphaNameList test..."));
	SPEW((GROUP_STUFF_TEST, "  AlphaNameList::TestClass() is stubbed out!\n"));
	return false;
}

//#############################################################################
//#############################################################################
