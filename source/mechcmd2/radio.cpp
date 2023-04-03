//******************************************************************************************
//
//	Radio.cpp -- File contains the Radio class functions
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

// #include "mclib.h"
#include "radio.h"
#include "gamesound.h"
#include "mover.h"

//------------------------------------------------------------------------------------------
// Macro Definitions
extern bool useSound;

PacketFilePtr Radio::noiseFile = nullptr;
RadioPtr Radio::radioList[radioconst::maxradios]; // Warriors no longer delete their
	// radios.
bool Radio::radioListGo = false;
bool Radio::messageInfoLoaded = false;
int32_t Radio::currentRadio = 0;
UserHeapPtr Radio::radioHeap = nullptr;
PacketFilePtr Radio::messagesFile[radioconst::maxradios];

RadioMessageInfo messageInfo[RADIO_MESSAGE_COUNT];

//------------------------------------------------------------------------------------------
// Class Radio
//------------------------------------------------------------------------------------------
PVOID
Radio::operator new(size_t mySize)
{
	if (!radioListGo)
	{
		radioListGo = true;
		for (size_t i = 0; i < radioconst::maxradios; i++)
		{
			radioList[i] = nullptr;
			messagesFile[i] = nullptr;
		}
		currentRadio = 0;
		radioHeap = new UserHeap;
		radioHeap->init((4096 * 51) - 1, "Radios");
		radioHeap->setMallocFatals(false);
	}
	PVOID result = radioHeap->Malloc(mySize);
	return result;
}

//-------------------------------------------------------------------------------
void Radio::operator delete(PVOID us)
{
	radioHeap->Free(us);
}

//-------------------------------------------------------------------------------
int32_t
Radio::init(std::wstring_view fileName, uint32_t heapSize, std::wstring_view movie)
{
	FullPathFileName pilotAudioPath;
	pilotAudioPath.init(CDsoundPath, fileName, ".pak");
	FullPathFileName noisePath;
	noisePath.init(CDsoundPath, "noise", ".pak");
	//--------------------------------------
	// Startup the packet file.
	radioID = currentRadio;
	messagesFile[radioID] = new PacketFile;
	gosASSERT(messagesFile[radioID] != nullptr);
	int32_t result = messagesFile[radioID]->open(pilotAudioPath);
	gosASSERT(result == NO_ERROR);
	if (!noiseFile)
	{
		//--------------------------------------
		// Startup the Noise packet file.
		noiseFile = new PacketFile;
		gosASSERT(noiseFile != nullptr);
		result = noiseFile->open(noisePath);
		gosASSERT(result == NO_ERROR);
	}
	// load message parameters
	if (!messageInfoLoaded)
	{
		if (loadMessageInfo() == NO_ERROR)
			messageInfoLoaded = TRUE;
		else
			Fatal(0, "Unable to load message info.");
	}
	radioList[currentRadio] = this;
	currentRadio++;
	return (NO_ERROR);
}

#define NO_PLAY -1
//------------------------------------------------------------------------------------------
int32_t
Radio::playMessage(RadioMessageType msgType)
{
	int32_t i, roll, callsign, fragmentNum, dropOut = 0;
	if (!useSound)
		return (NO_PLAY);
	if (!enabled)
		return (NO_PLAY);
	if (!owner)
		return (NO_PLAY);
	if (!soundSystem->checkMessage(owner, messageInfo[msgType].priority, msgType))
		return (NO_PLAY);
	if ((msgType == RADIO_AMMO_OUT) && ammoOutPlayed)
		return (NO_PLAY);
	if (messageInfo[msgType].styleCount > 1)
	{
		roll = RandomNumber(100); // choose which style of message to play
		for (i = 0; i < messageInfo[msgType].styleCount; i++)
		{
			dropOut += messageInfo[msgType].styleChance[i];
			if (roll < dropOut)
				break;
		}
		if (i != 0 && i == messageInfo[msgType].styleCount)
			return NO_PLAY;
		if (messageInfo[msgType].messageMapping + i == owner->getLastMessage())
			i++;
		if (i >= messageInfo[msgType].styleCount)
			i = 0;
	}
	else
		i = 0;
	RadioData* msgData = (RadioData*)radioHeap->Malloc(sizeof(RadioData));
	if (!msgData)
	{
		return (NO_PLAY);
	}
	memset(msgData, 0, sizeof(RadioData));
	msgData->noiseId = RadioNoise::SHORT_STATIC;
	msgData->msgType = msgType;
	msgData->msgId = messageInfo[msgType].messageMapping + i;
	msgData->movieCode = messageInfo[msgType].movieCode;
	msgData->msgHeap = radioHeap;
	msgData->turnQueued = turn;
	msgData->priority = messageInfo[msgType].priority;
	msgData->pilot = owner;
	msgData->expirationDate = scenarioTime + messageInfo[msgType].shelfLife;
	//-----------------------------------------------------------------------
	// Load the pieces need for playback.
	callsign = 0;
	fragmentNum = 0;
	if (messageInfo[msgType].pilotIdentifiesSelf)
	{
		if (messageInfo[RADIO_CALLSIGN].styleCount > 1)
		{
			roll = RandomNumber(100); // choose which style of message to play
			for (i = 0; i < messageInfo[RADIO_CALLSIGN].styleCount; i++)
			{
				dropOut += messageInfo[RADIO_CALLSIGN].styleChance[i];
				if (roll < dropOut)
					break;
			}
			if (i != 0 && i == messageInfo[RADIO_CALLSIGN].styleCount)
				callsign = 0;
			if (i >= messageInfo[RADIO_CALLSIGN].styleCount)
				callsign = 0;
		}
		else
			callsign = 0;
		callsign += messageInfo[RADIO_CALLSIGN].messageMapping;
	}
	if (callsign)
	{
		if (messagesFile[radioID]->seekPacket(callsign) == NO_ERROR)
		{
			uint32_t messageSize = messagesFile[radioID]->getPacketSize();
			msgData->data[fragmentNum] = (uint8_t*)radioHeap->Malloc(messageSize);
			if (!msgData->data[fragmentNum])
			{
				radioHeap->Free(msgData);
				return (NO_PLAY);
			}
			messagesFile[radioID]->readPacket(callsign, msgData->data[fragmentNum]);
			msgData->dataSize[fragmentNum] = messageSize;
			fragmentNum++;
		}
	}
	if (messagesFile[radioID]->seekPacket(msgData->msgId) == NO_ERROR)
	{
		uint32_t messageSize = messagesFile[radioID]->getPacketSize();
		msgData->data[fragmentNum] = (uint8_t*)radioHeap->Malloc(messageSize);
		if (!msgData->data[fragmentNum])
		{
			while (fragmentNum >= 0)
			{
				radioHeap->Free(msgData->data[fragmentNum]);
				fragmentNum--;
			}
			radioHeap->Free(msgData);
			return (NO_PLAY);
		}
		messagesFile[radioID]->readPacket(msgData->msgId, msgData->data[fragmentNum]);
		msgData->dataSize[fragmentNum] = messageSize;
		if (noiseFile->seekPacket(msgData->noiseId) == NO_ERROR)
		{
			uint32_t messageSize = noiseFile->getPacketSize();
			msgData->noise[0] = (uint8_t*)radioHeap->Malloc(messageSize);
			if (!msgData->noise[0])
			{
				radioHeap->Free(msgData);
				if (fragmentNum > 0)
					radioHeap->Free(msgData->data[0]);
				return (NO_PLAY);
			}
			noiseFile->readPacket(msgData->noiseId, msgData->noise[0]);
			msgData->noiseSize[fragmentNum] = messageSize;
		}
	}
	//------------------------------------------------------
	// Big ol bug here.  We weren't checking to see if the
	// queue was full.  If it was, memory would leak from
	// the smacker window.  It wouldn't leak from the RadioHeap
	// because we clear the radio heap every mission!!
	if (soundSystem->queueRadioMessage(msgData) != NO_ERROR)
	{
		if (msgData)
		{
			for (size_t j = 0; j < radioconst::maxfragments; j++)
			{
				radioHeap->Free(msgData->data[j]);
				msgData->data[j] = nullptr;
				radioHeap->Free(msgData->noise[j]);
				msgData->noise[j] = nullptr;
			}
			radioHeap->Free(msgData);
			msgData = nullptr;
		}
		return (NO_PLAY);
	}
	if (msgType == RADIO_AMMO_OUT)
		ammoOutPlayed = true;
	return (msgData->msgId);
}

//------------------------------------------------------------------------------------------
int32_t
Radio::loadMessageInfo(void)
{
	FullPathFileName messageInfoPath;
	std::unique_ptr<File> messageInfoFile;
	int32_t result;
	wchar_t dataLine[512];
	std::wstring_view field;
	messageInfoPath.init(soundPath, "radio", ".csv");
	messageInfoFile = new File;
	if (!messageInfoFile)
		return -1; // quasit
	result = messageInfoFile->open(messageInfoPath);
	if (result != NO_ERROR)
	{
		delete messageInfoFile;
		return result;
	}
	messageInfoFile->readLine((uint8_t*)dataLine, 511); // skip title line
	for (size_t i = 0; i < RADIO_MESSAGE_COUNT; i++)
	{
		result = messageInfoFile->readLine((uint8_t*)dataLine, 511);
		if (!result)
			Fatal(0, "Bad message Info File");
		field = strtok(dataLine, ","); // get past command name
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].priority = atoi(field);
		else
			messageInfo[i].priority = 4;
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].shelfLife = atoi(field);
		else
			messageInfo[i].shelfLife = 0;
		field = strtok(nullptr, ",");
		if (field && *field != 'x')
			messageInfo[i].movieCode = *field;
		else
			messageInfo[i].movieCode = '\0';
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].styleCount = atoi(field);
		else
			messageInfo[i].styleCount = 1;
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].styleChance[0] = atoi(field);
		else
			messageInfo[i].styleChance[0] = 0;
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].styleChance[1] = atoi(field);
		else
			messageInfo[i].styleChance[1] = 0;
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].styleChance[2] = atoi(field);
		else
			messageInfo[i].styleChance[2] = 0;
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].pilotIdentifiesSelf = (*field == 'y');
		else
			messageInfo[i].pilotIdentifiesSelf = FALSE;
		field = strtok(nullptr, ",");
		if (field)
			messageInfo[i].messageMapping = atoi(field);
		else
			messageInfo[i].messageMapping = 0;
		field = strtok(nullptr, ",");
		if (field)
		{
			field = strtok(nullptr, ",");
			if (field)
			{
				messageInfo[i].kludgeStyle = (*field == 'x');
			}
		}
		if (!field)
			messageInfo[i].kludgeStyle = FALSE;
	}
	messageInfoFile->close();
	delete messageInfoFile;
	return NO_ERROR;
}
