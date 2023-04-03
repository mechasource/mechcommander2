//===========================================================================//
// File:     notation.hpp                                                    //
// Title:    Declaration of NotationFile classes.                            //
// Purpose:  Provide general purpose access to data stored in a formatted    //
//           text file.                                                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _NOTATIONFILE_HPP_
#define _NOTATIONFILE_HPP_

#include "stuff/scalar.h"
// #include "stuff/chain.h"
#include "stuff/filestream.h"
#include "stuff/filestreammanager.h"
#include "stuff/tree.h"

namespace Stuff
{

class NotationFile;
class Page;
class Note;

class Macro;
typedef TreeOf<Macro*, std::wstring> MacroTree;

class Macro : public Plug
{
public:
	std::wstring m_macro;
	std::wstring m_replacement;
	bool m_inUse;

	Macro(std::wstring* macro, std::wstring* replace);

	static void AddValue(MacroTree* macro_tree, std::wstring_view name, std::wstring_view value);

	static void ReplaceMacros(
		MacroTree* macro_tree, std::wstring_view buffer, PSTR new_buf, size_t new_buf_size);
};

//=======================================================================
// Format of notation file:
//
// !include file1.txt
// !include=file1a.txt
// !include "file2.txt"
//
// !example_macro=Field2Data
//
// // comment
//
// [RecordName]	// comment
// FieldName=FieldData
// Field2Name=Field2Data
// Field3Name=test.ini
//
// [Record2Name]
// FieldName=FieldData
// Field4Name=$(example_macro)
// Field5Name={
//  [Record3Name]
//  Field6Name=Whatever
// }
//
// /* [Page3Name]
// Field7Name=Uhhm
// Field8Name=Uhhh */
// ...
//
//=======================================================================

//##########################################################################
//##############    NotationFile    ########################################
//##########################################################################

class NotationFile
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	friend class Page;
	friend class Note;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor/Destructors
	//
public:
	enum Type
	{
		Standard,
		NonEmpty,
		Raw
	};

	NotationFile(std::wstring_view file_name, Type type = Standard);
	NotationFile(std::iostream& stream = nullptr, MacroTree* macro_tree = nullptr);

	~NotationFile(void);

	void TestInstance(void) const;
	static bool TestClass(void);

protected:
	void CommonConstruction(std::iostream& memory_stream, TreeOf<Macro*, std::wstring>* macro_tree);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Stream access
	//
public:
	const FileDependencies* GetFileDependencies(void) const
	{
		// Check_Object(this);
		return &m_fileDependencies;
	}

	std::wstring_view
	GetFileName(void) const
	{
		// Check_Object(this);
		return m_fileName;
	}

	void SaveAs(std::wstring_view file_name);
	void Save(void);

	void IgnoreChanges()
	{
		// Check_Object(this);
		m_dirtyFlag = false;
	}

	bool IsChanged(void) const
	{
		// Check_Object(this);
		return m_dirtyFlag;
	}

protected:
	void Read(std::iostream& stream, MacroTree* macro_tree, Page** page, bool nested);
	void Write(std::iostream& stream);

	void ProcessLine(std::iostream& stream, MacroTree* macro_tree, Page** notepage, PSTR buffer);

	void HandleBangStuff(PSTR buffer, MacroTree* macro_tree, Page** page);

	void SetDirty()
	{
		// Check_Object(this);
		m_dirtyFlag = true;
	}

	std::wstring m_fileName;
	bool m_dirtyFlag;
	Type m_type;
	FileDependencies m_fileDependencies;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Page access
	//
public:
	bool IsEmpty()
	{
		// Check_Object(this);
		return m_pages.IsEmpty();
	}

	bool DoesPageExist(std::wstring_view pagename)
	{
		// Check_Object(this);
		return FindPage(pagename) != nullptr;
	}
	Page* FindPage(std::wstring_view pagename);
	Page* GetPage(uint32_t index);
	Page* GetPage(std::wstring_view pagename);

	typedef ChainIteratorOf<Page*> PageIterator;
	PageIterator* MakePageIterator()
	{
		// Check_Object(this);
		return new PageIterator(&m_pages);
	}

	Page* AddPage(std::wstring_view pagename);
	Page* SetPage(std::wstring_view pagename);

	void DeletePage(std::wstring_view pagename);
	void DeleteAllPages(void);

protected:
	ChainOf<Page*> m_pages;
};
} // namespace Stuff
#endif
