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
	PSTR m_FileName; // SOUNDTYPE_FILE and SOUNDTYPE_STREAMINGFILE
	BOOLEAN* m_FileDataPointer; // SOUNDTYPE_MEMORY
	BOOLEAN* m_DataPointer; // SOUNDTYPE_MEMORY
	uint32_t m_FileLength, // SOUNDTYPE_MEMORY only
		m_DataLength, // SOUNDTYPE_MEMORY only
		m_Frequency, // SOUNDTYPE_MEMORY only
		m_Flags; // SOUNDTYPE_MEMORY only

	WAVEFORMATEX* m_WaveFormatEx;
	float m_fDuration; // duration of sound in msec
	uint32_t m_nDataSize; // size of data chunk
	uint32_t m_nBytesPlayed; // offset into data chunk
	uint32_t m_nBytesUncompressed;
	uint32_t m_totalBytesRead;
	BOOLEAN m_microsoftPCM;
	WAVEFORMATEX m_suggFormat;
	HACMSTREAM m_hACMStream;
	PSTR m_uncompressedData;
	uint32_t m_uncompressedDataSize;

	LPDIRECTSOUNDBUFFER lpMasterBuffer;
	LPDIRECTSOUNDBUFFER lpDuplicateBuffers[16];
	BOOLEAN duplicateInUse[16];
	uint32_t duplicateCount;

	BOOLEAN m_only2D;

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
	SoundResource(PSTR file_name, enum gosAudio_ResourceType rt, BOOLEAN only2D);
	SoundResource(
		PVOID data, gosAudio_Format* wf, PSTR caching_nametag, int32_t size, BOOLEAN only2D);
	SoundResource(PSTR name, gosAudio_PlayList* playlist, BOOLEAN only2D);
	SoundResource(PSTR identifier_name, HGOSFILE file, uint32_t offset, BOOLEAN only2D);

	~SoundResource(void);
	void GetWaveInfo(BOOLEAN* lpBuffer, WAVEFORMATEX* lplpWaveFormatEX, BOOLEAN** lplpWaveData,
		uint32_t* lpWaveSize);
	void LoadFile(void);
	int32_t SoundResource::ReadPCM(
		BOOLEAN* pbDest, uint32_t bytestofill, BOOLEAN loopMe, BOOLEAN prevFailed = false);
	int32_t ReadACM(BOOLEAN* pbDest, uint32_t bytestoFill, BOOLEAN loopflag, BOOLEAN prevFailed = false);
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
