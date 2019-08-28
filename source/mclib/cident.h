//---------------------------------------------------------------------------
//
// cident.h - This file contains the class declarations for misc file stuff
//
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef CIDENT_H
#define CIDENT_H

//#include "dstd.h"
//#include "dident.h"
//#include <string.h>

//---------------------------------------------------------------------------
// Externs

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	class IDString
//---------------------------------------------------------------------------
class IDString
{
protected:
	wchar_t id[ID_SIZE]; // 8 characters and a terminator...

public:
	void init(void) { memset(id, 0, ID_SIZE); }

	void init(const std::wstring_view& new_id)
	{
		strncpy_s(id, ID_SIZE, new_id, ID_SIZE - 1); // pads unused ID with 0s!
		id[ID_SIZE - 1] = 0;
	}

	void init(IDString& new_ids)
	{
		strncpy_s(id, ID_SIZE, new_ids.id, ID_SIZE - 1);
		id[ID_SIZE - 1] = 0;
	}

	IDString(void) { init(); }

	IDString(const std::wstring_view& new_id) { init(new_id); }

	IDString(IDString& new_ids) { init(new_ids); }

	inline operator const std::wstring_view&(void) { return id; }

	bool operator==(const std::wstring_view& other_id)
	{
		if (other_id[0] != id[0])
			return false;
		if (other_id[0] == 0)
			return TRUE;
		if (other_id[1] != id[1])
			return false;
		if (other_id[1] == 0)
			return TRUE;
		if (other_id[2] != id[2])
			return false;
		if (other_id[2] == 0)
			return TRUE;
		if (other_id[3] != id[3])
			return false;
		if (other_id[3] == 0)
			return TRUE;
		if (other_id[4] != id[4])
			return false;
		if (other_id[4] == 0)
			return TRUE;
		if (other_id[5] != id[5])
			return false;
		if (other_id[5] == 0)
			return TRUE;
		if (other_id[6] != id[6])
			return false;
		if (other_id[6] == 0)
			return TRUE;
		return (other_id[7] == id[7]);
		//			return (strncmp(id,other_id,ID_SIZE-1) == 0);
	}

	bool operator==(IDString& other_ids)
	{
		if (other_ids.id[0] != id[0])
			return false;
		if (other_ids.id[0] == 0)
			return TRUE;
		if (other_ids.id[1] != id[1])
			return false;
		if (other_ids.id[1] == 0)
			return TRUE;
		if (other_ids.id[2] != id[2])
			return false;
		if (other_ids.id[2] == 0)
			return TRUE;
		if (other_ids.id[3] != id[3])
			return false;
		if (other_ids.id[3] == 0)
			return TRUE;
		if (other_ids.id[4] != id[4])
			return false;
		if (other_ids.id[4] == 0)
			return TRUE;
		if (other_ids.id[5] != id[5])
			return false;
		if (other_ids.id[5] == 0)
			return TRUE;
		if (other_ids.id[6] != id[6])
			return false;
		if (other_ids.id[6] == 0)
			return TRUE;
		return (other_ids.id[7] == id[7]);
		//			return (strncmp(id,other_ids.id,ID_SIZE-1) == 0);
	}

	bool operator!=(const std::wstring_view& other_id) { return !(*this == other_id); }

	bool operator!=(IDString& other_ids) { return !(*this == other_ids); }

	void operator=(const std::wstring_view& new_id) { init(new_id); }

	void operator=(IDString& new_ids) { init(new_ids); }

	bool isNull(void) { return (id[0] == 0); }

	void standardize(void) { _strupr_s(id, ID_SIZE); }
};

//--------------------------------------------------------------------------
class FullPathFileName
{
protected:
	const std::wstring_view& fullName;

public:
	FullPathFileName(void) {}

	FullPathFileName(const std::wstring_view& dir_path, const std::wstring_view& name, const std::wstring_view& ext)
	{
		init(dir_path, name, ext);
	}
	~FullPathFileName(void) {}

	operator const std::wstring_view&() { return fullName; }
	void changeExt(const std::wstring_view& from, const std::wstring_view& to);

protected:
	void init(const std::wstring_view& dir_path, const std::wstring_view& name, const std::wstring_view& ext);
	void destroy(void);
};

#endif
