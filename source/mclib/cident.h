//---------------------------------------------------------------------------
//
// cident.h - This file contains the class declarations for misc file stuff
//
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CIDENT_H
#define CIDENT_H
//---------------------------------------------------------------------------
// Include files

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef DIDENT_H
#include "dident.h"
#endif

#include <string.h>

//---------------------------------------------------------------------------
// Externs

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	class IDString
//---------------------------------------------------------------------------
class IDString
{
	protected:

		char id[ID_SIZE];					// 8 characters and a terminator...

	public:

		void init (void)
		{
			memset(id,0,ID_SIZE);
		}

		void init (char *new_id)
		{
			strncpy(id,new_id,ID_SIZE-1);   // pads unused ID with 0s!
			id[ID_SIZE-1] = 0;
		}

		void init (IDString & new_ids)
		{
			strncpy(id,new_ids.id,ID_SIZE-1);
			id[ID_SIZE-1] = 0;
		}

		IDString (void)
		{
			init();
		}

		IDString (char * new_id)
		{
			init(new_id);
		}

		IDString (IDString & new_ids)
		{
			init(new_ids);
		}

		inline operator char * (void)
		{
			return id;
		}

		bool operator == (char *other_id)
		{
			if( other_id[0]!=id[0] )
				return FALSE;
			if( other_id[0]==0 )
				return TRUE;

			if( other_id[1]!=id[1] )
				return FALSE;
			if( other_id[1]==0 )
				return TRUE;

			if( other_id[2]!=id[2] )
				return FALSE;
			if( other_id[2]==0 )
				return TRUE;

			if( other_id[3]!=id[3] )
				return FALSE;
			if( other_id[3]==0 )
				return TRUE;

			if( other_id[4]!=id[4] )
				return FALSE;
			if( other_id[4]==0 )
				return TRUE;

			if( other_id[5]!=id[5] )
				return FALSE;
			if( other_id[5]==0 )
				return TRUE;

			if( other_id[6]!=id[6] )
				return FALSE;
			if( other_id[6]==0 )
				return TRUE;

			return (other_id[7]==id[7]);
//			return (strncmp(id,other_id,ID_SIZE-1) == 0);
		}

		bool operator == (IDString &other_ids)
		{
			if( other_ids.id[0]!=id[0] )
				return FALSE;
			if( other_ids.id[0]==0 )
				return TRUE;

			if( other_ids.id[1]!=id[1] )
				return FALSE;
			if( other_ids.id[1]==0 )
				return TRUE;

			if( other_ids.id[2]!=id[2] )
				return FALSE;
			if( other_ids.id[2]==0 )
				return TRUE;

			if( other_ids.id[3]!=id[3] )
				return FALSE;
			if( other_ids.id[3]==0 )
				return TRUE;

			if( other_ids.id[4]!=id[4] )
				return FALSE;
			if( other_ids.id[4]==0 )
				return TRUE;

			if( other_ids.id[5]!=id[5] )
				return FALSE;
			if( other_ids.id[5]==0 )
				return TRUE;

			if( other_ids.id[6]!=id[6] )
				return FALSE;
			if( other_ids.id[6]==0 )
				return TRUE;

			return (other_ids.id[7]==id[7]);
//			return (strncmp(id,other_ids.id,ID_SIZE-1) == 0);
		}

		bool operator != (char *other_id)
		{
			return !(*this == other_id);
		}

		bool operator != (IDString &other_ids)
		{
			return !(*this == other_ids);
		}

		void operator = (char *new_id)
		{
			init(new_id);
		}

		void operator = (IDString & new_ids)
		{
			init(new_ids);
		}

		bool isNull (void)
		{
	      return (id[0] == 0);
		}

		void standardize (void)
		{
			strupr(id);
		}
};

//--------------------------------------------------------------------------
class FullPathFileName
{
	//Data items...
	//-------------

	private:

	protected:

		char *fullName;

	public:

	//Member functions...
	//-------------------

	private:

	protected:

	public:

		void init (void)
		{
			fullName = NULL;
		}

		void init (char *dir_path, const char *name, char *ext);

		FullPathFileName (void)
		{
			init();
		}

		FullPathFileName (char *dir_path, const char *name, char *ext)
		{
			init();
			init(dir_path, name, ext);
		}

		inline operator char * (void)
		{
			return fullName;
		}

		void destroy (void);

		void changeExt (char *from, char *to);

		~FullPathFileName (void);
};

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
// $Log$
//
//---------------------------------------------------------------------------
