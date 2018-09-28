//===========================================================================//
// File:	 VideoPlayback.hpp												 //
// Contents: Video Playback routines										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

typedef struct gos_Video
{
public:
	PSTR m_lpPath;

	static HGOSHEAP m_videoHeap;
	PIMULTIMEDIASTREAM m_pMMStream;
	PIMEDIASTREAM m_pPrimaryVidStream;
	IBasicAudio* m_pBasicAudio;
	PIDIRECTDRAWMEDIASTREAM m_pDDStream;
	LPDIRECTDRAWSURFACE m_pSurface;
	IDirectDrawStreamSample* m_pSample;
	LPDIRECTDRAWSURFACE m_hDestSurf;
	LPDIRECTDRAWSURFACE m_hSrcSurf;
	//
	// Surface that is used to decompress the video to (DirectX 1 surface) and
	// it's description
	//
	LPDIRECTDRAWSURFACE m_pMMStreamSurface;
	LPDIRECTDRAWSURFACE7 m_pMMStreamSurface7;
	DDSURFACEDESC2 m_pMMStreamSurfaceDesc;

	gosVideo_PlayMode m_videoStatus, m_videoPlayMode;
	RECT m_videoSrcRect, m_videoRect;

	int32_t m_videoLocX, m_videoLocY;

	float m_scaleX, m_scaleY;
	float m_volume, m_panning;

	uint32_t m_texture;

	STREAM_TIME m_duration;
	STREAM_TIME m_lastKnownTime;
	STREAM_TIME m_nextFrameTime;

public:
	gos_Video(PSTR path, bool texture);
	~gos_Video(void);
	bool Update(void);
	void Pause(void);
	void Continue(void);
	void Stop(void);
	void FF(double time);
	void Restore(void);
	void Release(void);
	void SetLocation(uint32_t, uint32_t);
	void OpenMMStream(PCSTR pszFileName, IDirectDraw* pDD, IMultiMediaStream** ppMMStream);
} gos_Video;

void __stdcall VideoManagerInstall(void);
void __stdcall VideoManagerPause(void);
void __stdcall VideoManagerContinue(void);
void __stdcall VideoManagerRelease(void);
void __stdcall VideoManagerRestore(void);
void __stdcall VideoManagerUninstall(void);
void __stdcall VideoManagerUpdate(void);
void __stdcall VideoManagerFF(double sec);
void __stdcall OpenMMStream(PCSTR pszFileName, IDirectDraw* pDD, IMultiMediaStream** ppMMStream,
	IBasicAudio** ppBasicAudio);
