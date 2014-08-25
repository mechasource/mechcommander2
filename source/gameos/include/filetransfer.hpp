//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// FileTransferInfo.h: interface for the FileTransferInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(FILETRANSFER_HPP)
#define FILETRANSFER_HPP


#ifndef FLINKEDLIST_HPP
#include "flinkedlist.hpp"
#endif

#ifndef NETWORKMESSAGES_H
#include "networkmessages.hpp"
#endif


class FileTransferInfo:public ListItem  
{
private:
	PVOID			dataForMessages;
	
public:
	// static data that must be initialized before this class
	// can be used
	static CHUNKSIZE;
	//static char HomeDirectory[512];

	typedef enum 
	{
		FTReceive = 0,
		FTSend
	} TransferType;


	// data
	FIDPMessage *	messageDescriptor;
	uint32_t	fileID;
	FILE *			fileToTransfer;
	PSTR 			fileName;
	PSTR 			relativeFilePath;
	uint32_t	fileSize;
	int32_t				percentDone;
	
	
	LPFILESENT_CALLBACK callbackFunction;

	// functions

	//PVOID operator new(size_t class_size);
	//void operator delete(PVOID us);

	
	FileTransferInfo(
		DPID this_player_id,
		DPID receiver_id,
		PSTR file_name,
		PSTR file_path,
		uint32_t size = 0,
		TransferType transfer_direction = FTSend);
	
	virtual ~FileTransferInfo(void);

	// PrepareNextMessage retrieves CHUNKSIZE bytes from the
	// file and places them into messageInfo.  
	// Returns 0 if there are still more bytes to send.
	// Returns 1 if this is the last of the file to send 
	int32_t PrepareNextMessage(void);

	// The addition operator allows the caller to add 
	// bytes from a new message to the file.  
	// Returns 0 if there are still more bytes to receive.
	// Returns 1 if the file is completely received.
	int32_t AddBytes(PVOIDbytes,int32_t size); 


	void SetID(int32_t id)
	{
		fileID = id;
	}

	void SetCallback(LPFILESENT_CALLBACK callback)
	{
		callbackFunction = callback;
	}

	// AllocateBeginTransferMessage creates a new message with information
	// that tells the receiver that a message is on its way.
	FIBeginFileTransferMessage *CreateBeginTransferMessage(int32_t& size);

};

#endif // !defined(FILETRANSFER_HPP)