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
#include "stuff/angle.h"
#include "stuff/rotation.h"
#include "stuff/note.h"

// using namespace Stuff;

#define MAX_LINE_SIZE 512

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::WriteNotation(std::iostream& stream)
{
	// Check_Object(this);
	Check_Object(stream);
	if (m_name)
		*stream << m_name;
	if (m_text)
		*stream << '=' << m_text;
	*stream << "\r\n";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(int32_t* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	*value = atoi(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(int32_t value)
{
	// Check_Object(this);
	wchar_t contents[12];
	_itoa_s(value, contents, _countof(contents), 10);
	_ASSERT(strlen(contents) < (_countof(contents)));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(float* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	*value = AtoF(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(float value)
{
	// Check_Object(this);
	wchar_t contents[32];
	sprintf_s(contents, _countof(contents), "%f", value);
	_ASSERT(strlen(contents) < sizeof(contents));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(bool* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	*value = (!_stricmp(contents, "true") || !_stricmp(contents, "yes") || atoi(contents) != 0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(bool value)
{
	// Check_Object(this);
	SetEntry((value) ? "true" : "false");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(Vector3D* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	int32_t count = sscanf_s(contents, "%f %f %f", &value->x, &value->y, &value->z);
	if (count != 3)
	{
		Page* page = m_page;
		Check_Object(page);
		NotationFile* file = page->m_notationFile;
		Check_Object(file);
		STOP(("%s: {[%s]%s=%s} is not a Vector!", file->GetFileName(), page->m_name, m_name,
			contents));
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(const Vector3D& value)
{
	// Check_Object(this);
	static wchar_t contents[64];
	int32_t temp;
	temp = sprintf_s(contents, _countof(contents), "%f %f %f", value.x, value.y, value.z);
	_ASSERT(temp < sizeof(contents));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(YawPitchRoll* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	// AFAIU we need pointers to float to avoid warning C6272
	float fyaw = value->yaw;
	float fpitch = value->pitch;
	float froll = value->roll;
	int32_t count = sscanf_s(contents, "%f %f %f", &fyaw, &fpitch, &froll);
	if (count != 3)
	{
		Page* page = m_page;
		Check_Object(page);
		NotationFile* file = page->m_notationFile;
		Check_Object(file);
		STOP(("%s: {[%s]%s=%s} is not a YawPitchRoll!", file->GetFileName(), page->m_name, m_name,
			contents));
	}
	value->yaw *= Radians_Per_Degree;
	value->pitch *= Radians_Per_Degree;
	value->roll *= Radians_Per_Degree;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(const YawPitchRoll& value)
{
	// Check_Object(this);
	static wchar_t contents[32];
	sprintf_s(contents, _countof(contents), "%f %f %f", value.yaw * Degrees_Per_Radian,
		value.pitch * Degrees_Per_Radian, value.roll * Degrees_Per_Radian);
	_ASSERT(strlen(contents) < sizeof(contents));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(UnitQuaternion* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	// AFAIU we need pointers to float to avoid warning C6272
	YawPitchRoll ypr;
	float fyaw = ypr.yaw;
	float fpitch = ypr.pitch;
	float froll = ypr.roll;
	int32_t count = sscanf_s(contents, "%f %f %f", &fyaw, &fpitch, &froll);
	if (count != 3)
	{
		Page* page = m_page;
		Check_Object(page);
		NotationFile* file = page->m_notationFile;
		Check_Object(file);
		STOP(("%s: {[%s]%s=%s} is not a UnitQuaternion!", file->GetFileName(), page->m_name, m_name,
			contents));
	}
	ypr.yaw *= Radians_Per_Degree;
	ypr.pitch *= Radians_Per_Degree;
	ypr.roll *= Radians_Per_Degree;
	*value = ypr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(const UnitQuaternion& value)
{
	// Check_Object(this);
	static wchar_t contents[32] = {0};
	YawPitchRoll ypr(value);
	_ASSERT(strlen(contents) < sizeof(contents));
	sprintf_s(contents, _countof(contents), "%f %f %f", ypr.yaw * Degrees_Per_Radian,
		ypr.pitch * Degrees_Per_Radian, ypr.roll * Degrees_Per_Radian);
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(Motion3D* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	int32_t count = sscanf_s(contents, "%f %f %f %f %f %f", &value->linearMotion.x,
		&value->linearMotion.y, &value->linearMotion.z, &value->angularMotion.x,
		&value->angularMotion.y, &value->angularMotion.z);
	if (count != 6)
	{
		Page* page = m_page;
		Check_Object(page);
		NotationFile* file = page->m_notationFile;
		Check_Object(file);
		STOP(("%s: {[%s]%s=%s} is not a Motion!", file->GetFileName(), page->m_name, m_name,
			contents));
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(const Motion3D& value)
{
	// Check_Object(this);
	static wchar_t contents[64];
	sprintf_s(contents, _countof(contents), "%f %f %f %f %f %f", value.linearMotion.x,
		value.linearMotion.y, value.linearMotion.z, value.angularMotion.x, value.angularMotion.y,
		value.angularMotion.z);
	_ASSERT(strlen(contents) < sizeof(contents));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(RGBcolour* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	int32_t count = sscanf_s(contents, "%f %f %f", &value->red, &value->green, &value->blue);
	if (count != 3)
	{
		Page* page = m_page;
		Check_Object(page);
		NotationFile* file = page->m_notationFile;
		Check_Object(file);
		STOP(("%s: {[%s]%s=%s} is not an RGBcolour!", file->GetFileName(), page->m_name, m_name,
			contents));
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(const RGBcolour& value)
{
	// Check_Object(this);
	static wchar_t contents[32];
	sprintf_s(contents, _countof(contents), "%f %f %f", value.red, value.green, value.blue);
	_ASSERT(strlen(contents) < sizeof(contents));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(RGBAcolour* value)
{
	// Check_Object(this);
	Check_Pointer(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	int32_t count =
		sscanf_s(contents, "%f %f %f %f", &value->red, &value->green, &value->blue, &value->alpha);
	if (count == 3)
		value->alpha = 1.0f;
	if (count < 3)
	{
		Page* page = m_page;
		Check_Object(page);
		NotationFile* file = page->m_notationFile;
		Check_Object(file);
		STOP(("%s: {[%s]%s=%s} is not an RGBAcolour!", file->GetFileName(), page->m_name, m_name,
			contents));
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(const RGBAcolour& value)
{
	// Check_Object(this);
	static wchar_t contents[48];
	sprintf_s(contents, _countof(contents), "%f %f %f %f", value.red, value.green, value.blue,
		value.alpha);
	_ASSERT(strlen(contents) < sizeof(contents));
	SetEntry(contents);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::GetEntry(NotationFile* value)
{
	// Check_Object(this);
	Check_Object(value);
	const std::wstring_view& contents = nullptr;
	GetEntry(&contents);
	Check_Pointer(contents);
	//
	//-------------------------------------------------------
	// If this is a file reference, open the file and read it
	//-------------------------------------------------------
	//
	if (strncmp(contents, "{\r\n", 3))
	{
		FileStream file(contents);
		value->m_fileName = file.GetFileName();
		value->m_fileDependencies.AddDependency(&file);
		value->Read(&file, nullptr, nullptr, false);
	}
	//
	//--------------------------------------------------------------------
	// Otherwise, we are reading a nested file, so set up the dependencies
	//--------------------------------------------------------------------
	//
	else
	{
		std::iostream stream(const_cast<const std::wstring_view&>(contents + 3), strlen(contents) - 3);
		NotationFile* parent_file = m_page->GetNotationFile();
		Check_Object(parent_file);
		value->m_fileDependencies.AddDependencies(parent_file->GetFileDependencies());
		//
		//-----------------------------------
		// Figure out the name of the subfile
		//-----------------------------------
		//
		if (parent_file->GetFileName())
		{
			const std::wstring_view& name(parent_file->GetFileName());
			name += '[';
			name += GetName();
			name += ']';
			value->m_fileName = name;
		}
		//
		//--------------------
		// Read the string now
		//--------------------
		//
		value->Read(&stream, nullptr, nullptr, true);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::SetEntry(NotationFile* value)
{
	// Check_Object(this);
	Check_Object(value);
	//
	//----------------------------------------------------
	// If this isn't a nested file, write out the filename
	//----------------------------------------------------
	//
	const std::wstring_view& name = value->GetFileName();
	if (name && name[strlen(name) - 1] != ']')
		SetEntry(name);
	//
	//-------------------------------------------------------------------------
	// Otherwise, we need to write the notation file out to a memory stream and
	// assign that to the entry
	//-------------------------------------------------------------------------
	//
	else
	{
		std::fstream file_buffer(5);
		file_buffer << "{\r\n";
		value->Write(&file_buffer);
		file_buffer << "}" << '\0';
		file_buffer.Rewind();
		SetEntry(static_cast<const std::wstring_view&>(file_buffer.GetPointer()));
		value->IgnoreChanges();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
Note::TestInstance(void) const
{
}
