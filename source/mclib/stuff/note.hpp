//===========================================================================//
// File:     notation.hpp                                                    //
// Title:    Declaration of NotationFile classes.                            //
// Purpose:  Provide general purpose access to data stored in a formatted    //
//           text file.                                                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _NOTE_HPP_
#define _NOTE_HPP_

#include <stuff/page.hpp>

namespace Stuff
{

//##########################################################################
//##############    Note    ##############################
//##########################################################################

	class Note:
		public Plug
	{
		friend class NotationFile;
		friend class Page;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors
		//
	protected:
		Note(Page* page):
			Plug(DefaultData)
		{
			m_page = page;
		}

		Page
		* m_page;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Note functions
		//
	public:
		void
		SetName(PCSTR entryname)
		{
			// Check_Object(this);
			Check_Pointer(entryname);
			m_name = entryname;
		}
		PCSTR
		GetName(void) const
		{
			// Check_Object(this);
			return m_name;
		}

	protected:
		MString
		m_name,
		m_text;

		void SetDirty(void)
		{
			// Check_Object(this);
			Check_Object(m_page);
			m_page->SetDirty();
		}

		void
		WriteNotation(MemoryStream* stream);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// string access
		//
	public:
		void
		GetEntry(PCSTR* contents)
		{
			// Check_Object(this);
			Check_Pointer(contents);
			if(contents) *contents = m_text;
		}
		void
		SetEntry(PCSTR contents)
		{
			// Check_Object(this);
			m_text = contents;
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// int32_t access
		//
	public:
		void
		GetEntry(pint32_t value);
		void
		SetEntry(int32_t value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// scalar access
		//
	public:
		void
		GetEntry(float* value);
		void
		SetEntry(float value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// bool access
		//
	public:
		void
		GetEntry(bool* value);
		void
		SetEntry(bool value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Vector3D access
		//
	public:
		void
		GetEntry(Vector3D* value);
		void
		SetEntry(const Vector3D& value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// YawPitchRoll access
		//
	public:
		void
		GetEntry(YawPitchRoll* value);
		void
		SetEntry(const YawPitchRoll& value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// UnitQuaternion access
		//
	public:
		void
		GetEntry(UnitQuaternion* value);
		void
		SetEntry(const UnitQuaternion& value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Motion3D access
		//
	public:
		void
		GetEntry(Motion3D* value);
		void
		SetEntry(const Motion3D& value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// RGBColor access
		//
	public:
		void
		GetEntry(RGBColor* value);
		void
		SetEntry(const RGBColor& value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// RGBAColor access
		//
	public:
		void
		GetEntry(RGBAColor* value);
		void
		SetEntry(const RGBAColor& value);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// NotationFile access
		//
	public:
		void
		GetEntry(NotationFile* value);
		void
		SetEntry(NotationFile* value);

	public:
		void TestInstance(void) const;
	};

}
#endif
