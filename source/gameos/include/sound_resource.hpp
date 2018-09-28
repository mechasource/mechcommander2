#pragma once
//===========================================================================//
// File:	 Sound Resource.hpp												 //
// Contents: Direct Sound manager											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "music.hpp"

struct SoundResource
{
public:
	gosAudio_ResourceType m_Type; // Specifies which other structure members are valid
	PSTR m_FileName;			  // SOUNDTYPE_FILE and SOUNDTYPE_STREAMINGFILE
	puint8_t m_FileDataPointer;   // SOUNDTYPE_MEMORY
	puint8_t m_DataPointer;		  // SOUNDTYPE_MEMORY
	uint32_t m_FileLength,		  // SOUNDTYPE_MEMORY only
		m_DataLength,			  // SOUNDTYPE_MEMORY only
		m_Frequency,			  // SOUNDTYPE_MEMORY only
		m_Flags;				  // SOUNDTYPE_MEMORY only

	WAVEFORMATEX* m_WaveFormatEx;
	float m_fDuration;		 // duration of sound in msec
	uint32_t m_nDataSize;	// size of data chunk
	uint32_t m_nBytesPlayed; // offset into data chunk
	uint32_t m_nBytesUncompressed;
	uint32_t m_totalBytesRead;
	bool m_microsoftPCM;
	WAVEFORMATEX m_suggFormat;
	HACMSTREAM m_hACMStream;
	PSTR m_uncompressedData;
	uint32_t m_uncompressedDataSize;

	LPDIRECTSOUNDBUFFER lpMasterBuffer;
	LPDIRECTSOUNDBUFFER lpDuplicateBuffers[16];
	bool duplicateInUse[16];
	uint32_t duplicateCount;

	bool m_only2D;

	HGOSFILE m_FILE;

	// for playlist type
	int32_t m_placeInList;
	int32_t m_playListDataLength;
	int32_t m_playListOffsetWithin;
	gosAudio_PlayList* m_playList;
	DSBUFFERDESC masterDesc;

	HGOSMUSIC m_hMusic;
	uint32_t m_cueSeekDistance;

public:
	SoundResource(PCSTR file_name, enum gosAudio_ResourceType rt, bool only2D);
	SoundResource(
		PVOID data, gosAudio_Format* wf, PCSTR caching_nametag, int32_t size, bool only2D);
	SoundResource(PCSTR name, gosAudio_PlayList* playlist, bool only2D);
	SoundResource(PCSTR identifier_name, HGOSFILE file, uint32_t offset, bool only2D);

	~SoundResource(void);
	void GetWaveInfo(puint8_t lpBuffer, WAVEFORMATEX* lplpWaveFormatEX, puint8_t* lplpWaveData,
		uint32_t* lpWaveSize);
	void LoadFile(void);
	int32_t SoundResource::ReadPCM(
		puint8_t pbDest, uint32_t bytestofill, bool loopMe, bool prevFailed = false);
	int32_t ReadACM(puint8_t pbDest, uint32_t bytestoFill, bool loopflag, bool prevFailed = false);
	void CloseStream(void);
	void PauseStream(void);
	void PlayStream(void);
	void StopStream(void);
	void RestartStream(void);
	void Cue(void);
	void OpenFromMemory(void);
	uint32_t ConvertACM(uint32_t numBytes);

	void GetDuplicateBuffer(LPDIRECTSOUNDBUFFER*);
	void RelinquishDuplicate(LPDIRECTSOUNDBUFFER lpBuf);
	void CreateMasterBuffer(void);
};
