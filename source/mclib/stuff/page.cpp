//===========================================================================//
// File:     notation.cpp                                                    //
// Title:    Definition of NotationFile classes.                             //
// Purpose:  Provide general purpose access to data stored in a formatted    //
//           text file.                                                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

//#include "gameos.hpp"
#include "stuff/note.h"
#include "stuff/page.h"

using namespace Stuff;

#define MAX_LINE_SIZE 512

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Page::Page(NotationFile* notation_file) :
	Plug(DefaultData), m_notes(nullptr)
{
	// Check_Pointer(this);
	Check_Object(notation_file);
	m_notationFile = notation_file;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Page::~Page()
{
	// Check_Object(this);
	NoteIterator notes(&m_notes);
	notes.DeletePlugs();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::WriteNotes(std::iostream& stream)
{
	// Check_Object(this);
	const std::wstring_view& name = m_name;
	if (name)
		*stream << '[' << name << "]\r\n";
	NoteIterator notes(&m_notes);
	Note* note;
	while ((note = notes.ReadAndNext()) != nullptr)
	{
		Check_Object(note);
		note->WriteNotation(stream);
	}
	*stream << "\r\n";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Note*
Page::FindNote(const std::wstring_view& entryname)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	NoteIterator notes(&m_notes);
	Note* note;
	while ((note = notes.ReadAndNext()) != nullptr)
	{
		Check_Object(note);
		if (!_stricmp(note->m_name, entryname))
			return note;
	}
	return nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Note*
Page::GetNote(uint32_t i)
{
	// Check_Object(this);
	NoteIterator notes(&m_notes);
	return notes.GetNth(i);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ChainOf<Note*>*
Page::MakeNoteChain(const std::wstring_view& entryname)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	NoteIterator notes(&m_notes);
	Note* note;
	ChainOf<Note*>* chain = new ChainOf<Note*>(nullptr);
	Check_Object(chain);
	size_t len = strlen(entryname);
	while ((note = notes.ReadAndNext()) != nullptr)
	{
		Check_Object(note);
		if (!_strnicmp(note->GetName(), entryname, len))
			chain->Add(note);
	}
	return chain;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Note*
Page::AddNote(const std::wstring_view& entryname)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	SetDirty();
	Note* note = new Note(this);
	Check_Object(note);
	note->SetName(entryname);
	m_notes.Add(note);
	return note;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::DeleteNote(const std::wstring_view& entryname)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		delete note;
		SetDirty();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::DeleteAllNotes()
{
	// Check_Object(this);
	NoteIterator notes(&m_notes);
	Note* note;
	while ((note = notes.ReadAndNext()) != nullptr)
	{
		Check_Object(note);
		delete note;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, const std::wstring_view&* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const std::wstring_view& contents)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const std::wstring_view& contents)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, int32_t* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, int32_t contents)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, int32_t contents)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, float* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, float value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, float value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, bool* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, bool value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, bool value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, Vector3D* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const Vector3D& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const Vector3D& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, YawPitchRoll* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const YawPitchRoll& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const YawPitchRoll& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, UnitQuaternion* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const UnitQuaternion& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const UnitQuaternion& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, Motion3D* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const Motion3D& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const Motion3D& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, RGBcolour* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const RGBcolour& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const RGBcolour& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, RGBAcolour* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), m_name, entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, const RGBAcolour& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, const RGBAcolour& value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
Page::GetEntry(const std::wstring_view& entryname, NotationFile* contents, bool required)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);
	Note* note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}
	if (required)
		STOP(("%s: [%s]%s is a required entry!", m_notationFile->GetFileName(), (const std::wstring_view&)m_name,
			entryname));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::SetEntry(const std::wstring_view& entryname, NotationFile* value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::AppendEntry(const std::wstring_view& entryname, NotationFile* value)
{
	// Check_Object(this);
	Check_Pointer(entryname);
	Note* note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Page::TestInstance(void) const
{
}
