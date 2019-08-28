//===========================================================================//
// File: notation.hpp //
// Title: Declaration of NotationFile classes. //
// Purpose: Provide general purpose access to data stored in a formatted //
// text file. //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

#pragma once

#ifndef _PAGE_HPP_
#define _PAGE_HPP_

#include "stuff/notationfile.h"
#include "stuff/vector3d.h"
#include "stuff/rotation.h"
#include "stuff/motion.h"
#include "stuff/color.h"

namespace Stuff
{

//##########################################################################
//############## Page ##############################
//##########################################################################

class Page : public Plug
{
	friend class NotationFile;
	friend class Note;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor/Destructors
	//
protected:
	Page(NotationFile* notation_file);
	~Page(void);

	NotationFile* m_notationFile;

public:
	NotationFile* GetNotationFile(void)
	{
		// Check_Object(this);
		return m_notationFile;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Page functions
	//
public:
	void SetName(const std::wstring_view& pagename)
	{
		// Check_Object(this);
		m_name = pagename;
	}
	const std::wstring_view&
	GetName(void) const
	{
		// Check_Object(this);
		return m_name;
	}

	void WriteNotes(std::iostream& stream);

protected:
	std::wstring m_name;

	void SetDirty(void)
	{
		// Check_Object(this);
		Check_Object(m_notationFile);
		m_notationFile->SetDirty();
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Entry access
	//
public:
	bool IsEmpty(void)
	{
		// Check_Object(this);
		return m_notes.IsEmpty();
	}

	bool DoesNoteExist(const std::wstring_view& entryname)
	{
		// Check_Object(this);
		return FindNote(entryname) != nullptr;
	}
	Note* FindNote(const std::wstring_view& entryname);
	Note* GetNote(uint32_t index);

	typedef ChainIteratorOf<Note*> NoteIterator;
	NoteIterator* MakeNoteIterator(void)
	{
		// Check_Object(this);
		return new NoteIterator(&m_notes);
	}

	ChainOf<Note*>* MakeNoteChain(const std::wstring_view& prefix);

	Note* AddNote(const std::wstring_view& entryname);
	Note* SetNote(const std::wstring_view& entryname);

	void DeleteNote(const std::wstring_view& entryname);
	void DeleteAllNotes(void);

protected:
	ChainOf<Note*> m_notes;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// string access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, const std::wstring_view&* contents, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const std::wstring_view& contents);
	void AppendEntry(const std::wstring_view& entryname, const std::wstring_view& contents);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// int32_t access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, int32_t* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, int32_t value);
	void AppendEntry(const std::wstring_view& entryname, int32_t value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// scalar access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, float* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, float value);
	void AppendEntry(const std::wstring_view& entryname, float value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// bool access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, bool* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, bool value);
	void AppendEntry(const std::wstring_view& entryname, bool value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Vector3D access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, Vector3D* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const Vector3D& value);
	void AppendEntry(const std::wstring_view& entryname, const Vector3D& value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// YawPitchRoll access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, YawPitchRoll* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const YawPitchRoll& value);
	void AppendEntry(const std::wstring_view& entryname, const YawPitchRoll& value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// UnitQuaternion access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, UnitQuaternion* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const UnitQuaternion& value);
	void AppendEntry(const std::wstring_view& entryname, const UnitQuaternion& value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Motion3D access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, Motion3D* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const Motion3D& value);
	void AppendEntry(const std::wstring_view& entryname, const Motion3D& value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RGBcolour access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, RGBcolour* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const RGBcolour& value);
	void AppendEntry(const std::wstring_view& entryname, const RGBcolour& value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RGBAcolour access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, RGBAcolour* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, const RGBAcolour& value);
	void AppendEntry(const std::wstring_view& entryname, const RGBAcolour& value);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// NotationFile access
	//
public:
	bool GetEntry(const std::wstring_view& entryname, NotationFile* value, bool required = false);
	void SetEntry(const std::wstring_view& entryname, NotationFile* value);
	void AppendEntry(const std::wstring_view& entryname, NotationFile* value);

public:
	void TestInstance(void) const;
};
} // namespace Stuff
#endif
