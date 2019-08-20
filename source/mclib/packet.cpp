//---------------------------------------------------------------------------
//
// Packet.cpp -- File contains the actual code for the Packet File class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include Files
#ifndef PACKET_H
#include "packet.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef LZ_H
#include "lz.h"
#endif

#include "zlib.h"

#ifndef _MBCS
#include <gameos.hpp>
#else
#include <assert.h>
#define gosASSERT assert
#define gos_Malloc malloc
#define gos_Free free
#endif

#include <string.h>
//---------------------------------------------------------------------------
extern puint8_t LZPacketBuffer;
extern uint32_t LZPacketBufferSize;
//---------------------------------------------------------------------------
// class PacketFile
void
PacketFile::clear(void)
{
	currentPacket = -1;
	packetSize = packetBase = numPackets = 0;
	if (seekTable)
		systemHeap->Free(seekTable);
	seekTable = nullptr;
}

//---------------------------------------------------------------------------
void
PacketFile::atClose(void)
{
	if (isOpen() && fileMode != READ) // update filesize
	{
		int32_t endPtr = getLength();
		// seek(sizeof(int32_t));								//Move Past Version
		// Marker  writeLong(endPtr);								//Write File
		// length
		int32_t tableEntry;
		currentPacket = numPackets;
		if (!seekTable)
		{
			while (--currentPacket >= 0)
			{
				seek(TABLE_ENTRY(currentPacket));
				tableEntry = readLong();
				if (GetPacketType(tableEntry) == STORAGE_TYPE_NUL)
				{
					seek(TABLE_ENTRY(currentPacket));
					writeLong(SetPacketType(endPtr, STORAGE_TYPE_NUL));
				}
				else
				{
					endPtr = GetPacketOffset(tableEntry);
				}
			}
		}
		else
		{
			while (--currentPacket >= 0)
			{
				tableEntry = seekTable[currentPacket];
				if (GetPacketType(tableEntry) == STORAGE_TYPE_NUL)
				{
					seekTable[currentPacket] = SetPacketType(endPtr, STORAGE_TYPE_NUL);
				}
				else
				{
					endPtr = GetPacketOffset(tableEntry);
				}
			}
		}
		//-----------------------------------------------------
		// If seekTable was being used, write it back to file
		if (seekTable)
		{
			seek(sizeof(int32_t) * 2); // File Version & File Length
			write(puint8_t(seekTable), (numPackets * sizeof(int32_t)));
		}
		//------------------------------------------------------
		// Is we were using a checkSum, calc it and write it to
		// the beginning of the file.
		if (usesCheckSum)
		{
			int32_t checkSum = checkSumFile();
			seek(0);
			writeLong(checkSum);
		}
	}
	clear();
}

//---------------------------------------------------------------------------
int32_t
PacketFile::checkSumFile(void)
{
	//-----------------------------------------
	int32_t currentPosition = logicalPosition;
	seek(4);
	puint8_t fileMap = (puint8_t)malloc(fileSize());
	read(fileMap, fileSize());
	int32_t sum = 0;
	puint8_t curFileByte = fileMap;
	for (uint32_t i = 4; i < fileSize(); i++, curFileByte++)
	{
		sum += *curFileByte;
	}
	free(fileMap);
	seek(currentPosition);
	return sum;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::afterOpen(void)
{
	if (!numPackets && getLength() >= 12)
	{
		int32_t firstPacketOffset;
		int32_t firstCheck = readLong();
		if (firstCheck == PACKET_FILE_VERSION && !usesCheckSum)
		{
		}
		else
		{
			//---------------------------------------
			// This is probably a checksum.  Check it
			int32_t checkSum = checkSumFile();
			if (checkSum != firstCheck)
				return PACKET_OUT_OF_RANGE;
		}
		firstPacketOffset = readLong();
		numPackets = (firstPacketOffset / sizeof(int32_t)) - 2;
	}
	currentPacket = -1;
	if (fileMode == READ || fileMode == RDWRITE)
	{
		if (numPackets && !seekTable)
		{
			seekTable = (int32_t*)systemHeap->Malloc(numPackets * sizeof(int32_t));
			gosASSERT(seekTable != nullptr);
			seek(sizeof(int32_t) * 2); // File Version & File Length
			read(puint8_t(seekTable), (numPackets * sizeof(int32_t)));
		}
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
PacketFile::PacketFile(void)
{
	seekTable = nullptr;
	usesCheckSum = FALSE;
	clear();
}

//---------------------------------------------------------------------------
PacketFile::~PacketFile(void)
{
	close();
}

//---------------------------------------------------------------------------
int32_t
PacketFile::open(PSTR fName, FileMode _mode, int32_t numChild)
{
	int32_t openResult = MechFile::open(fName, _mode, numChild);
	if (openResult != NO_ERROR)
	{
		return (openResult);
	}
	openResult = afterOpen();
	return (openResult);
}

//---------------------------------------------------------------------------
int32_t
PacketFile::open(FilePtr _parent, uint32_t fileSize, int32_t numChild)
{
	int32_t result = MechFile::open(_parent, fileSize, numChild);
	if (result != NO_ERROR)
		return (result);
	result = afterOpen();
	return (result);
}

//---------------------------------------------------------------------------
int32_t
PacketFile::create(PSTR fName)
{
	int32_t openResult = MechFile::create(fName);
	if (openResult != NO_ERROR)
	{
		return (openResult);
	}
	openResult = afterOpen();
	return (openResult);
}

int32_t
PacketFile::createWithCase(PSTR fName)
{
	int32_t openResult = MechFile::createWithCase(fName);
	if (openResult != NO_ERROR)
	{
		return (openResult);
	}
	openResult = afterOpen();
	return (openResult);
}

//---------------------------------------------------------------------------
void
PacketFile::close(void)
{
	atClose();
	MechFile::close();
}

//---------------------------------------------------------------------------
int32_t
PacketFile::readPacketOffset(int32_t packet, int32_t* lastType)
{
	int32_t offset = -1;
	if (packet < numPackets)
	{
		if (seekTable)
			offset = seekTable[packet];
		if (lastType)
			*lastType = GetPacketType(offset);
		offset = GetPacketOffset(offset);
	}
	return offset;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::readPacket(int32_t packet, puint8_t buffer)
{
	int32_t result = 0;
	if ((packet == -1) || (packet == currentPacket) || (seekPacket(packet) == NO_ERROR))
	{
		if ((getStorageType() == STORAGE_TYPE_RAW) || (getStorageType() == STORAGE_TYPE_FWF))
		{
			seek(packetBase);
			result = read(buffer, packetSize);
		}
		else
		{
			switch (getStorageType())
			{
			case STORAGE_TYPE_LZD:
			{
				seek(packetBase + sizeof(int32_t));
				if (!LZPacketBuffer)
				{
					LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
					gosASSERT(LZPacketBuffer);
				}
				if ((int32_t)LZPacketBufferSize < packetSize)
				{
					LZPacketBufferSize = packetSize;
					free(LZPacketBuffer);
					LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
					gosASSERT(LZPacketBuffer);
				}
				if (LZPacketBuffer)
				{
					read(LZPacketBuffer, (packetSize - sizeof(int32_t)));
					int32_t decompLength =
						LZDecomp(buffer, LZPacketBuffer, packetSize - sizeof(int32_t));
					if (decompLength != packetUnpackedSize)
						result = 0;
					else
						result = decompLength;
				}
			}
			break;
			case STORAGE_TYPE_ZLIB:
			{
				seek(packetBase + sizeof(int32_t));
				if (!LZPacketBuffer)
				{
					LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
					gosASSERT(LZPacketBuffer);
				}
				if ((int32_t)LZPacketBufferSize < packetSize)
				{
					LZPacketBufferSize = packetSize;
					free(LZPacketBuffer);
					LZPacketBuffer = (puint8_t)malloc(LZPacketBufferSize);
					gosASSERT(LZPacketBuffer);
				}
				if (LZPacketBuffer)
				{
					read(LZPacketBuffer, (packetSize - sizeof(int32_t)));
					uint32_t decompLength = LZPacketBufferSize;
					int32_t decompResult = uncompress(
						buffer, &decompLength, LZPacketBuffer, packetSize - sizeof(int32_t));
					if ((decompResult != Z_OK) || ((int32_t)decompLength != packetUnpackedSize))
						result = 0;
					else
						result = decompLength;
				}
			}
			break;
			case STORAGE_TYPE_HF:
				STOP(("Tried to read a Huffman Compressed Packet.  No Longer "
					  "Supported!!"));
				break;
			}
		}
	}
	return result;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::readPackedPacket(int32_t packet, puint8_t buffer)
{
	int32_t result = 0;
	if ((packet == -1) || (packet == currentPacket) || (seekPacket(packet) == NO_ERROR))
	{
		if ((getStorageType() == STORAGE_TYPE_RAW) || (getStorageType() == STORAGE_TYPE_FWF))
		{
			seek(packetBase);
			result = read(buffer, packetSize);
		}
		else
		{
			switch (getStorageType())
			{
			case STORAGE_TYPE_LZD:
			{
				seek(packetBase + sizeof(int32_t));
				read(buffer, packetSize);
			}
			break;
			case STORAGE_TYPE_ZLIB:
			{
				seek(packetBase + sizeof(int32_t));
				read(buffer, packetSize);
			}
			break;
			}
		}
	}
	return result;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::seekPacket(int32_t packet)
{
	int32_t offset, next;
	if (packet < 0)
	{
		return (PACKET_OUT_OF_RANGE);
	}
	offset = readPacketOffset(packet, &packetType);
	currentPacket = packet++;
	if (packet == numPackets)
		next = getLength();
	else
		next = readPacketOffset(packet);
	packetSize = next - offset;
	packetBase = offset; // seek to beginning of packet
	seek(packetBase);
	switch (getStorageType())
	{
	case STORAGE_TYPE_LZD:
		// the first uint32_t of a compressed packet is the unpacked length
		packetUnpackedSize = readLong();
		break;
	case STORAGE_TYPE_ZLIB:
		// the first uint32_t of a compressed packet is the unpacked length
		packetUnpackedSize = readLong();
		break;
	case STORAGE_TYPE_RAW:
		packetUnpackedSize = packetSize;
		break;
	case STORAGE_TYPE_NUL:
		packetUnpackedSize = 0;
		break;
	default:
		return (BAD_PACKET_VERSION);
	}
	if (offset > 0)
		return (NO_ERROR);
	return (PACKET_OUT_OF_RANGE);
}

//---------------------------------------------------------------------------
void
PacketFile::operator++(void)
{
	if (++currentPacket >= numPackets)
	{
		currentPacket = numPackets - 1;
	}
	seekPacket(currentPacket);
}

//---------------------------------------------------------------------------
void
PacketFile::operator--(void)
{
	if (currentPacket-- <= 0)
	{
		currentPacket = 0;
	}
	seekPacket(currentPacket);
}

//---------------------------------------------------------------------------
int32_t
PacketFile::getNumPackets(void)
{
	return numPackets;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::getCurrentPacket(void)
{
	return currentPacket;
}

//---------------------------------------------------------------------------
inline int32_t
PacketFile::getPacketOffset(void)
{
	return packetBase;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::getPackedPacketSize(void)
{
	return packetSize;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::getStorageType(void)
{
	return packetType;
}

//---------------------------------------------------------------------------
void
PacketFile::reserve(int32_t count, bool useCheckSum)
{
	//---------------------------------------------------
	// If we already have packets, reserve does nothing.
	// Otherwise, reserve sets up the file.  Must be
	// called before any writing to a newly created file.
	if (numPackets)
	{
		return;
	}
	usesCheckSum = useCheckSum;
	numPackets = count;
	int32_t firstPacketOffset = TABLE_ENTRY(numPackets);
	writeLong(PACKET_FILE_VERSION);
	writeLong(firstPacketOffset);
	//----------------------------
	// initialize the seek table
	while (count-- > 0)
		writeLong(SetPacketType(firstPacketOffset, STORAGE_TYPE_NUL));
	//-------------------------------------------------------------
	// If we called this, chances are we are writing a packet file
	// from start to finish.  It is MUCH faster if this table is
	// updated in memory and flushed when the file is closed.
	if (!seekTable)
	{
		seekTable = (int32_t*)systemHeap->Malloc(numPackets * sizeof(int32_t));
		if (seekTable != nullptr)
		{
			seek(sizeof(int32_t) * 2); // File Version & File Length
			read(puint8_t(seekTable), (numPackets * sizeof(int32_t)));
		}
	}
}

//---------------------------------------------------------------------------
int32_t
PacketFile::writePacket(int32_t packet, puint8_t buffer, int32_t nbytes, uint8_t pType)
{
	//--------------------------------------------------------
	// This function writes the packet to the current end
	// of file and stores the packet address in the seek
	// table.  NOTE that this cannot be used to replace
	// a packet.  That function is writePacket which takes
	// a packet number and a buffer.  The size cannot change
	// and, as such, is irrelevant.  You must write the
	// same sized packet each time, if the packet already
	// exists.  In theory, it could be smaller but the check
	// right now doesn't allow anything but same size.
	int32_t result = 0;
	puint8_t workBuffer = nullptr;
	if (pType == ANY_PACKET_TYPE || pType == STORAGE_TYPE_LZD || pType == STORAGE_TYPE_ZLIB)
	{
		if ((nbytes << 1) < 4096)
			workBuffer = (puint8_t)malloc(4096);
		else
			workBuffer = (puint8_t)malloc(nbytes << 1);
		gosASSERT(workBuffer != nullptr);
	}
	gosASSERT((packet > 0) || (packet < numPackets));
	packetBase = getLength();
	currentPacket = packet;
	packetSize = packetUnpackedSize = nbytes;
	//-----------------------------------------------
	// Code goes in here to pick the best compressed
	// version of the packet.  Otherwise, default
	// to RAW.
	if ((pType == ANY_PACKET_TYPE) || (pType == STORAGE_TYPE_LZD) || (pType == STORAGE_TYPE_ZLIB))
	{
		if (pType == ANY_PACKET_TYPE)
			pType = STORAGE_TYPE_RAW;
		//-----------------------------
		// Find best compression here.
		// This USED to use LZ.  Use ZLib from now on.
		// Game will ALWAYS be able to READ LZ Packets!!
		uint32_t actualSize = nbytes << 1;
		if (actualSize < 4096)
			actualSize = 4096;
		uint32_t workBufferSize = actualSize;
		uint32_t oldBufferSize = nbytes;
		int32_t compressedResult =
			compress2(workBuffer, &workBufferSize, buffer, nbytes, Z_DEFAULT_COMPRESSION);
		if (compressedResult != Z_OK)
			STOP(("Unable to write packet %d to file %s.  Error %d", packet, m_fileName,
				compressedResult));
		compressedResult = uncompress(buffer, &oldBufferSize, workBuffer, nbytes);
		if ((int32_t)oldBufferSize != nbytes)
			STOP((
				"Packet size changed after compression.  Was %d is now %d", nbytes, oldBufferSize));
		if ((pType == STORAGE_TYPE_LZD) || (pType == STORAGE_TYPE_ZLIB) || ((int32_t)workBufferSize < nbytes))
		{
			pType = STORAGE_TYPE_ZLIB;
			packetSize = workBufferSize;
		}
	}
	packetType = pType;
	seek(packetBase);
	if (packetType == STORAGE_TYPE_ZLIB)
	{
		writeLong(packetUnpackedSize);
		result = write(workBuffer, packetSize);
	}
	else
	{
		result = write(buffer, packetSize);
	}
	if (!seekTable)
	{
		seek(TABLE_ENTRY(packet));
		writeLong(SetPacketType(packetBase, packetType));
	}
	else
	{
		seekTable[packet] = SetPacketType(packetBase, packetType);
	}
	int32_t* currentEntry = nullptr;
	if (seekTable)
	{
		packet++;
		currentEntry = &(seekTable[packet]);
	}
	int32_t tableData = SetPacketType(getLength(), STORAGE_TYPE_NUL);
	while (packet < (numPackets - 1))
	{
		if (!seekTable)
		{
			writeLong(tableData);
		}
		else
		{
			*currentEntry = tableData;
			currentEntry++;
		}
		packet++;
	}
	if (workBuffer)
		free(workBuffer);
	return result;
}

#define DEFAULT_MAX_PACKET 65535
//---------------------------------------------------------------------------
int32_t
PacketFile::insertPacket(int32_t packet, puint8_t buffer, int32_t nbytes, uint8_t pType)
{
	//--------------------------------------------------------
	// This function writes the packet to the current end
	// of file and stores the packet address in the seek
	// table.  Originally, replace was a NONO.  No, we check
	// for the size and, if it is different, insert the new
	// packet into a new file and basically spend many timeparts doing it.
	// Necessary for the teditor.
	// I Love it.
	int32_t result = 0;
	if (packet < 0)
	{
		return result;
	}
	//---------------------------------------------------------------
	// Only used here, so OK if regular WINDOWS(tm) malloc!
	puint8_t workBuffer = (puint8_t)malloc(DEFAULT_MAX_PACKET);
	//-------------------------------------------------------------
	// All new code here.  Basically, open a new packet file,
	// write each of the old packets and this new one.  Close all
	// and copy the new one over the old one.  Open the new one and
	// set pointers accordingly.
	PacketFile tmpFile;
	result = tmpFile.create("AF3456AF.788");
	if (packet >= numPackets)
	{
		numPackets++;
	}
	tmpFile.reserve(numPackets);
	for (size_t i = 0; i < numPackets; i++)
	{
		if (i == packet)
		{
			if (nbytes >= DEFAULT_MAX_PACKET)
			{
				//----------------------------------------------------
				// Not sure what to do here.  We'll try reallocating
				::free(workBuffer);
				workBuffer = (puint8_t)malloc(packetSize);
			}
			tmpFile.writePacket(i, buffer, nbytes, pType);
		}
		else
		{
			seekPacket(i);
			int32_t storageType = getStorageType();
			int32_t packetSize = getPacketSize();
			if (packetSize >= DEFAULT_MAX_PACKET)
			{
				//----------------------------------------------------
				// Not sure what to do here.  We'll try reallocating
				::free(workBuffer);
				workBuffer = (puint8_t)malloc(packetSize);
			}
			readPacket(i, workBuffer);
			tmpFile.writePacket(i, workBuffer, packetSize, storageType);
		}
	}
	//------------------------------------
	// Now close and reassign everything.
	char ourFileName[250];
	int32_t ourFileMode = 0;
	strcpy(ourFileName, m_fileName);
	ourFileMode = fileMode;
	tmpFile.close();
	close();
	remove(ourFileName);
	rename("AF3456AF.788", ourFileName);
	remove("AF3456AF.788");
	open(ourFileName, (FileMode)ourFileMode);
	seekPacket(packet);
	return result;
}

//---------------------------------------------------------------------------
int32_t
PacketFile::writePacket(int32_t packet, puint8_t buffer)
{
	//--------------------------------------------------------
	// This function replaces the packet with the contents
	// of buffer.  There are two restrictions.  The first is
	// that the packet must be the same length as the existing
	// packet.  If not, buffer over/under run will occur.
	// The second is that the packet cannot be compressed since
	// there is no gaurantee that the new data will compress
	// to exactly the same length.  Returns NO_ERROR if packet
	// written successfully.  Otherwise returns error.
	int32_t result = 0;
	if ((packet < 0) || (packet >= numPackets))
	{
		return 0;
	}
	seekPacket(packet);
	if (packetType == STORAGE_TYPE_LZD || packetType == STORAGE_TYPE_HF || packetType == STORAGE_TYPE_ZLIB)
	{
		return (PACKET_WRONG_SIZE);
	}
	else
	{
		result = write(buffer, packetSize);
	}
	if (result == packetUnpackedSize)
	{
		return (NO_ERROR);
	}
	return BAD_WRITE_ERR;
}
