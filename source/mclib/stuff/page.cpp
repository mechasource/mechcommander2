//===========================================================================//
// File:     notation.cpp                                                    //
// Title:    Definition of NotationFile classes.                             //
// Purpose:  Provide general purpose access to data stored in a formatted    //
//           text file.                                                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
#include "stuffheaders.hpp"

#define MAX_LINE_SIZE 512

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Page::Page(NotationFile *notation_file):
	Plug(DefaultData),
	m_notes(NULL)
{
	Check_Pointer(this);
	Check_Object(notation_file);
	m_notationFile = notation_file;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Page::~Page()
{
	Check_Object(this);
	NoteIterator notes(&m_notes);
	notes.DeletePlugs();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::WriteNotes(MemoryStream *stream)
{
	Check_Object(this);

	PCSTR name = m_name;
	if (name)
		*stream << '[' << name << "]\r\n";
	NoteIterator notes(&m_notes);
	Note *note;
	while ((note = notes.ReadAndNext()) != NULL)
	{
		Check_Object(note);
		note->WriteNotation(stream);
	}
	*stream << "\r\n";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Note*
	Page::FindNote(PCSTR entryname)
{
	Check_Object(this);
	Check_Pointer(entryname);

	NoteIterator notes(&m_notes);
	Note *note;
	while ((note = notes.ReadAndNext()) != NULL)
	{
		Check_Object(note);
		if (!_stricmp(note->m_name, entryname))
			return note;
	}
	return NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
Note*
	Page::GetNote(unsigned i)
{
	Check_Object(this);

	NoteIterator notes(&m_notes);
	return notes.GetNth(i);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ChainOf<Note*>*
	Page::MakeNoteChain(PCSTR entryname)
{
	Check_Object(this);
	Check_Pointer(entryname);

	NoteIterator notes(&m_notes);
	Note *note;
	ChainOf<Note*>* chain = new ChainOf<Note*>(NULL);
	Check_Object(chain);
	int len = strlen(entryname);
	while ((note = notes.ReadAndNext()) != NULL)
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
	Page::AddNote(PCSTR entryname)
{
	Check_Object(this);
	Check_Pointer(entryname);

	SetDirty();
	Note *note = new Note(this);
	Check_Object(note);
	note->SetName(entryname);
	m_notes.Add(note);

	return note;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::DeleteNote(PCSTR entryname)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
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
	Check_Object(this);

	NoteIterator notes(&m_notes);
	Note *note;
	while ((note = notes.ReadAndNext()) != NULL)
	{
		Check_Object(note);
		delete note;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		PCSTR *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		PCSTR contents
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		PCSTR contents
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		int *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		int contents
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		int contents
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		Scalar *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		Scalar value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		Scalar value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		bool *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		bool value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		bool value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		Vector3D *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		const Vector3D &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		const Vector3D &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		YawPitchRoll *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		const YawPitchRoll &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		const YawPitchRoll &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		UnitQuaternion *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		const UnitQuaternion &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		const UnitQuaternion &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		Motion3D *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		const Motion3D &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		const Motion3D &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		RGBColor *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		const RGBColor &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		const RGBColor &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		RGBAColor *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		const RGBAColor &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		const RGBAColor &value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	Page::GetEntry(
		PCSTR entryname,
		NotationFile *contents,
		bool required
	)
{
	Check_Object(this);
	Check_Pointer(entryname);
	Check_Pointer(contents);

	Note *note = FindNote(entryname);
	if (note)
	{
		Check_Object(note);
		note->GetEntry(contents);
		return true;
	}

	if (required)
		STOP((
			"%s: [%s]%s is a required entry!",
			m_notationFile->GetFileName(),
			(char*)m_name,
			entryname
		));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::SetEntry(
		PCSTR entryname,
		NotationFile *value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = FindNote(entryname);
	if (!note)
		note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::AppendEntry(
		PCSTR entryname,
		NotationFile *value
	)
{
	Check_Object(this);
	Check_Pointer(entryname);

	Note *note = AddNote(entryname);
	Check_Object(note);
	note->SetEntry(value);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	Page::TestInstance() const
{
}
