//---------------------------------------------------------------------------
//
// Packet.h -- File contains the header for the Packet File class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef PACKET_H
#define PACKET_H

//---------------------------------------------------------------------------
// Include Files

//#include "dpacket.h"
//#include "file.h"
//#include "lz.h"

//---------------------------------------------------------------------------
// Macro Definitions
enum _packet_storage : uint32_t
{
	PACKET_FILE_VERSION = 0xFEEDFACE,
	NEW_PACKET_FLAG = 0xFEEDFACE
};

// 3 bits of type info
enum _packet_storage_type : uint32_t
{
	STORAGE_TYPE_RAW = 0x00,
	STORAGE_TYPE_FWF = 0x01, // file within file
	STORAGE_TYPE_LZD = 0x02, // LZ Compressed Packet
	STORAGE_TYPE_HF = 0x03, // Huffman Compressed Packet
	STORAGE_TYPE_ZLIB = 0x04, // zLib Compressed Packet
	STORAGE_TYPE_NUL = 0x07, // nullptr packet.
};
#define DEFAULT_PACKET_TYPE STORAGE_TYPE_RAW

#define TYPE_SHIFT 29 // Bit position of masked type
#define OFFSET_MASK ((1L << TYPE_SHIFT) - 1L)
#define ANY_PACKET_TYPE 0x07

#define GetPacketType(offset) ((offset) >> TYPE_SHIFT)
#define GetPacketOffset(offset) (offset & OFFSET_MASK)
#define SetPacketType(offset, type) ((offset) + ((type) << TYPE_SHIFT))

#define TABLE_ENTRY(p) ((2 + p) << 2) // ((1+p)*sizeof(int32_t))

//---------------------------------------------------------------------------
// Structure and Class Definitions

//---------------------------------------------------------------------------
class PacketFile : public MechFile
{
	// Data Members
	//-------------
protected:
	int32_t numPackets;
	int32_t currentPacket;
	int32_t packetSize;
	int32_t packetBase;

	int32_t packetType;
	int32_t packetUnpackedSize;

	int32_t* seekTable;

	bool usesCheckSum;

	// Member Functions
	//-----------------
protected:
	void clear(void);
	void atClose(void);
	int32_t afterOpen(void);

public:
	PacketFile(void);
	~PacketFile(void);

	virtual int32_t open(std::wstring_view filename, FileMode _mode = READ, uint32_t numChildren = 50);
	virtual int32_t open(std::unique_ptr<File> _parent, size_t fileSize, uint32_t numChildren = 50);

	virtual int32_t create(std::wstring_view filename);
	virtual int32_t createWithCase(std::wstring_view filename);
	virtual void close(void);

	void forceUseCheckSum(void)
	{
		usesCheckSum = true;
	}

	int32_t readPacketOffset(int32_t packet, int32_t* lastType = 0);
	int32_t readPacket(int32_t packet, uint8_t* buffer);
	int32_t readPackedPacket(int32_t packet, uint8_t* buffer);

	int32_t seekPacket(int32_t packet);

	void operator++(void);
	void operator--(void);

	int32_t getNumPackets(void);
	int32_t getCurrentPacket(void);
	int32_t getPacketOffset(void);

	int32_t getPacketSize(void)
	{
		return packetUnpackedSize;
	}

	int32_t getPackedPacketSize(void);
	int32_t getStorageType(void);

	virtual FileClass getFileClass(void)
	{
		return FileClass::packetfile;
	}

	int32_t checkSumFile(void);

	//-------------------------------------------
	// Functions to Write Packet Files
	void reserve(int32_t count, bool withCheckSum = FALSE);
	int32_t writePacket(
		int32_t packet, uint8_t* buffer, int32_t nbytes, uint8_t p_type = ANY_PACKET_TYPE);
	int32_t insertPacket(
		int32_t packet, uint8_t* buffer, int32_t nbytes, uint8_t p_type = ANY_PACKET_TYPE);
	int32_t writePacket(int32_t packet, uint8_t* buffer);
};

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
// $Log: packetfile.h $
// Revision 1.1  1995/08/09 17:38:22  fsavage
// Initial revision
//
//
//---------------------------------------------------------------------------
