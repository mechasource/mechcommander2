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

#ifndef DPACKET_H
#include "dpacket.h"
#endif

#ifndef FILE_H
#include "file.h"
#endif

#ifndef LZ_H
#include "lz.h"
#endif

//---------------------------------------------------------------------------
// Macro Definitions

#define PACKET_FILE_VERSION 0xFEEDFACE
#define NEW_PACKET_FLAG 0xFEEDFACE

// 3 bits of type info

#define STORAGE_TYPE_RAW 0x00L
#define STORAGE_TYPE_FWF 0x01L  // file within file
#define STORAGE_TYPE_LZD 0x02L  // LZ Compressed Packet
#define STORAGE_TYPE_HF 0x03L   // Huffman Compressed Packet
#define STORAGE_TYPE_ZLIB 0x04L // zLib Compressed Packet
#define STORAGE_TYPE_NUL 0x07L  // nullptr packet.

#define TYPE_SHIFT 29 // Bit position of masked type
#define DEFAULT_PACKET_TYPE STORAGE_TYPE_RAW
#define ANY_PACKET_TYPE 0x07
#define OFFSET_MASK ((1L << TYPE_SHIFT) - 1L)

#define GetPacketType(offset) (((uint32_t)offset) >> TYPE_SHIFT)
#define GetPacketOffset(offset) (offset & OFFSET_MASK)
#define SetPacketType(offset, type) ((offset) + (int32_t(type) << TYPE_SHIFT))

#define TABLE_ENTRY(p) ((2 + p) << 2) // ((1+p)*sizeof(int32_t))

//---------------------------------------------------------------------------
// Structure and Class Definitions

//---------------------------------------------------------------------------
class PacketFile : public File
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

	virtual int32_t open(
		PCSTR fName, FileMode _mode = READ, uint32_t numChildren = 50);
	virtual int32_t open(
		FilePtr _parent, size_t fileSize, uint32_t numChildren = 50);

	virtual int32_t create(PCSTR fName);
	virtual int32_t createWithCase(PSTR fName); // don't strlwr for me please!
	virtual void close(void);

	void forceUseCheckSum(void) { usesCheckSum = true; }

	int32_t readPacketOffset(int32_t packet, int32_t* lastType = 0);
	int32_t readPacket(int32_t packet, puint8_t buffer);
	int32_t readPackedPacket(int32_t packet, puint8_t buffer);

	int32_t seekPacket(int32_t packet);

	void operator++(void);
	void operator--(void);

	int32_t getNumPackets(void);
	int32_t getCurrentPacket(void);
	int32_t getPacketOffset(void);

	int32_t getPacketSize(void) { return packetUnpackedSize; }

	int32_t getPackedPacketSize(void);
	int32_t getStorageType(void);

	virtual FileClass getFileClass(void) { return PACKETFILE; }

	int32_t checkSumFile(void);

	//-------------------------------------------
	// Functions to Write Packet Files
	void reserve(int32_t count, bool withCheckSum = FALSE);
	int32_t writePacket(int32_t packet, puint8_t buffer, int32_t nbytes,
		uint8_t p_type = ANY_PACKET_TYPE);
	int32_t insertPacket(int32_t packet, puint8_t buffer, int32_t nbytes,
		uint8_t p_type = ANY_PACKET_TYPE);
	int32_t writePacket(int32_t packet, puint8_t buffer);
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
