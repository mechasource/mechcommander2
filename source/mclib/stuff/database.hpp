//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include "stuff.hpp"

namespace Stuff {

	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	// ****************************	   DATABASE API	   **************************** //
	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////

	class Record;
	class RecordHandle;
	class Database;
	class DatabaseHandle;

	class RecordHandle
	{
	public:
		DatabaseHandle
			*m_databaseHandle;
		ULONG
			m_ID;
		PCSTR 
			m_name;
		__int64
			m_timeStamp;
		ULONG
			m_length;
		const void*
			m_data;
		const Record*
			m_record;

		//
		// Add - Adds a record to the database and returns a unique ID that can be used to access the record.
		//       The record can be found by the RecordID returned or the RecordName
		//       The info fields will be changed to point at the database's copy of the field
		//
		void
			Add();

		//
		// Replace - Overwrites a record based on the RecordID passed.
		//       The record can be found by the RecordID returned or the RecordName
		//       The info fields will be changed to point at the database's copy of the field
		//
		void
			Replace();

		//
		// Delete - Removes the record with the given RecordID.
		//
		void
			Delete();

		//
		// FindID - Returns a pointer to the Data contained in the record with the ID specified, also fills in a pointer to the original name.
		//			True is returned if a record is found
		//
		bool
			FindID();

		//
		// FindName - Returns a pointer to the Data contained in the record with the Name specified, also fills in a pointer to the record ID number.
		//       True is returned if a record is found
		//       The info fields will be changed to point at the database's copy of the field
		//
		bool
			FindName();

		//
		// Return the current record information and moves the current record to the next record.
		// If there is a current record, True will be returned.
		//
		bool
			ReadAndNext();

		void
			TestInstance() const;
	};

	class DatabaseHandle
	{
		friend class Database;
		friend class Record;
		friend class RecordHandle;

	public:
		Database
			*m_dataBase;

		DatabaseHandle(
			PCSTR  filename,
			bool read_only
		);
		~DatabaseHandle();

		void
			Save();


		//
		// GetNumberOfRecords - Returns the number of records in the database
		//
		ULONG
			GetNumberOfRecords();

		//
		// First - Makes the 'current' record the first in the database (Based on RecordID number)
		//
		void
			First();

		void
			TestInstance() const;

	protected:
		HANDLE 		m_handle;
		bool		m_dirtyFlag;
		bool		m_readOnly;
		ULONG 		m_currentRecord;
		UINT_PTR	m_baseAddress;
		Record*		m_currentPointer;
		MString		m_fileName;
	};

}
