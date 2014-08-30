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

#include <stuff/notationfile.hpp>
#include <stuff/vector3d.hpp>
#include <stuff/rotation.hpp>
#include <stuff/motion.hpp>
#include <stuff/color.hpp>

namespace Stuff{

	//##########################################################################
	//############## Page ##############################
	//##########################################################################

	class Page:
		public Plug
	{
		friend class NotationFile;
		friend class Note;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructor/Destructors
		//
	protected:
		Page(NotationFile *notation_file);
		~Page(void);

		NotationFile
			*m_notationFile;

	public:
		NotationFile*
			GetNotationFile(void)
		{Check_Object(this); return m_notationFile;}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Page functions
		//
	public:
		void
			SetName(PCSTR pagename)
		{Check_Object(this); m_name = pagename;}
		PCSTR
			GetName(void) const
		{ Check_Object(this); return m_name; }

		void
			WriteNotes(MemoryStream *stream);

	protected:
		MString
			m_name;

		void
			SetDirty(void)
		{Check_Object(this); Check_Object(m_notationFile); m_notationFile->SetDirty();}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Entry access
		//
	public:
		bool
			IsEmpty(void)
		{Check_Object(this); return m_notes.IsEmpty();}

		bool
			DoesNoteExist(PCSTR entryname)
		{Check_Object(this); return FindNote(entryname) != nullptr;}
		Note*
			FindNote(PCSTR entryname);
		Note*
			GetNote(uint32_t index);

		typedef ChainIteratorOf<Note*> NoteIterator;
		NoteIterator*
			MakeNoteIterator(void)
		{Check_Object(this); return new NoteIterator(&m_notes);}

		ChainOf<Note*>*
			MakeNoteChain(PCSTR prefix);

		Note*
			AddNote(PCSTR entryname);
		Note*
			SetNote(PCSTR entryname);

		void
			DeleteNote(PCSTR entryname);
		void
			DeleteAllNotes(void);

	protected:
		ChainOf<Note*>
			m_notes;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// string access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			PCSTR *contents,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			PCSTR contents
			);
		void
			AppendEntry(
			PCSTR entryname,
			PCSTR contents
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// int32_t access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			pint32_t value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			int32_t value
			);
		void
			AppendEntry(
			PCSTR entryname,
			int32_t value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// scalar access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			Scalar *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			Scalar value
			);
		void
			AppendEntry(
			PCSTR entryname,
			Scalar value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// bool access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			bool *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			bool value
			);
		void
			AppendEntry(
			PCSTR entryname,
			bool value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Vector3D access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			Vector3D *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			const Vector3D &value
			);
		void
			AppendEntry(
			PCSTR entryname,
			const Vector3D &value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// YawPitchRoll access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			YawPitchRoll *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			const YawPitchRoll &value
			);
		void
			AppendEntry(
			PCSTR entryname,
			const YawPitchRoll &value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// UnitQuaternion access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			UnitQuaternion *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			const UnitQuaternion &value
			);
		void
			AppendEntry(
			PCSTR entryname,
			const UnitQuaternion &value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Motion3D access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			Motion3D *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			const Motion3D &value
			);
		void
			AppendEntry(
			PCSTR entryname,
			const Motion3D &value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// RGBColor access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			RGBColor *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			const RGBColor &value
			);
		void
			AppendEntry(
			PCSTR entryname,
			const RGBColor &value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// RGBAColor access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			RGBAColor *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			const RGBAColor &value
			);
		void
			AppendEntry(
			PCSTR entryname,
			const RGBAColor &value
			);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// NotationFile access
		//
	public:
		bool
			GetEntry(
			PCSTR entryname,
			NotationFile *value,
			bool required=false
			);
		void
			SetEntry(
			PCSTR entryname,
			NotationFile *value
			);
		void
			AppendEntry(
			PCSTR entryname,
			NotationFile *value
			);

	public:
		void TestInstance(void) const;
	};

}
#endif
