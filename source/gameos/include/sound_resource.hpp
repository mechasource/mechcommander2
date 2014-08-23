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
		gosAudio_ResourceType	
			m_Type;			// Specifies which other structure members are valid
		PSTR 	
			m_FileName;	// SOUNDTYPE_FILE and SOUNDTYPE_STREAMINGFILE
		puint8_t	
			m_FileDataPointer;	// SOUNDTYPE_MEMORY
		puint8_t	
			m_DataPointer;		// SOUNDTYPE_MEMORY
		ULONG		
			m_FileLength,		// SOUNDTYPE_MEMORY only
			m_DataLength,		// SOUNDTYPE_MEMORY only
			m_Frequency,		// SOUNDTYPE_MEMORY only
			m_Flags;			// SOUNDTYPE_MEMORY only

		WAVEFORMATEX *		m_WaveFormatEx;
		float				m_fDuration;           // duration of sound in msec
		UINT				m_nDataSize;           // size of data chunk
		UINT				m_nBytesPlayed;        // offset into data chunk
		UINT				m_nBytesUncompressed;
		UINT				m_totalBytesRead;
		bool				m_microsoftPCM;
		WAVEFORMATEX 		m_suggFormat;
		HACMSTREAM 			m_hACMStream;
		PSTR 				m_uncompressedData;
		UINT				m_uncompressedDataSize;

		LPDIRECTSOUNDBUFFER lpMasterBuffer;
		LPDIRECTSOUNDBUFFER lpDuplicateBuffers[16];
		bool				duplicateInUse[16];
		ULONG 				duplicateCount;

		bool m_only2D;

		HGOSFILE m_FILE;

		// for playlist type
		int m_placeInList;
		int m_playListDataLength;
		int m_playListOffsetWithin;
		gosAudio_PlayList * m_playList;
		DSBUFFERDESC 	masterDesc;

		HGOSMUSIC m_hMusic;
		ULONG			m_cueSeekDistance;

	public:
		SoundResource
		(
			PCSTR  file_name,
			enum gosAudio_ResourceType rt,
			bool only2D
		);
		SoundResource
		(
			PVOID data,
			gosAudio_Format * wf,
			PCSTR  caching_nametag,
			int size,
			bool only2D
		);
		SoundResource
		(
			PCSTR  name,
			gosAudio_PlayList * playlist,
			bool only2D
		);
		SoundResource
		(
			PCSTR  identifier_name,
			HGOSFILE file,
			ULONG offset,
			bool only2D
		);


		~SoundResource();
		void GetWaveInfo
			( 
				puint8_t lpBuffer,
				WAVEFORMATEX *lplpWaveFormatEX,
				puint8_t* lplpWaveData,
				ULONG *lpWaveSize 
			);
		void LoadFile();
		int SoundResource::ReadPCM (puint8_t  pbDest, UINT bytestofill, bool loopMe, bool prevFailed=false );
		int ReadACM (puint8_t  pbDest, UINT bytestoFill, bool loopflag, bool prevFailed=false );
		void CloseStream();
		void PauseStream();
		void PlayStream();
		void StopStream();
		void RestartStream();
		void Cue();
		void OpenFromMemory();
		ULONG ConvertACM(ULONG numBytes);

		void GetDuplicateBuffer( LPDIRECTSOUNDBUFFER* );
		void RelinquishDuplicate(LPDIRECTSOUNDBUFFER lpBuf);
		void CreateMasterBuffer();
};

