//===========================================================================//
// File:	 DirectX.hpp													 //
// Contents: Interface to DirectSetup										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

extern UCHAR	AllowFail;
extern volatile USHORT FPUControl;	// Current FPU control word
extern USHORT FPUDefault;				// Default FPU control word
extern PSTR __stdcall ErrorNumberToMessage(HRESULT hResult);


//
// Blade GUID

//#ifdef __cplusplus
//#define EXTERN_C    extern "C"
//#else
//#define EXTERN_C    extern
//#endif
//
//#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//	EXTERN_C const GUID DECLSPEC_SELECTANY name \
//	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
//
//DEFINE_GUID( IID_IDirect3DBladeDevice,    0x706e9520, 0x6db1, 0x11d2, 0xb9, 0x1, 0x0, 0x10, 0x4b, 0x36, 0xae, 0x4b);


//
// Used to save and restore FPU control word around DirectX functions
//
#if defined(LAB_ONLY)
#define SafeFPU()		_asm fldcw FPUDefault
#define GameOSFPU()		_asm fninit _asm fldcw FPUControl
#else
#define SafeFPU()
#define GameOSFPU()
#endif




//
// General DirectX
//
HRESULT __stdcall wCoCreateInstance(REFCLSID rclsid,LPUNKNOWN pUnkOuter,ULONG dwClsContext,REFIID riid,PVOID* ppv);
HRESULT __stdcall wQueryInterface( IUnknown *iun, REFIID riid, PVOID* obp );
ULONG __stdcall wRelease( IUnknown *iun );
ULONG __stdcall wAddRef( IUnknown *iun );

//
// Direct Setup
//
void wDirectXSetupGetVersion( PULONG dwVersion,PULONG dwRevision );

// IDirect3DDevice7
//#ifdef LAB_ONLY
HRESULT __stdcall wDrawPrimitive(LPDIRECT3DDEVICE7 d3dDevice7, D3DPRIMITIVETYPE dptPrimitiveType, ULONG dvtVertexType, PVOID lpvVertices, ULONG dwVertexCount, ULONG dwFlags );
HRESULT __stdcall wDrawIndexedPrimitive(LPDIRECT3DDEVICE7 d3dDevice7, D3DPRIMITIVETYPE d3dptPrimitiveType, ULONG dwVertexTypeDesc, PVOID pvVertices, ULONG dwVertexCount, PUSHORT pwIndices, ULONG dwIndexCount, ULONG dwFlags );
//#else
//#define wDrawPrimitive(d3dDevice7,dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,dwFlags ) d3dDevice7->DrawPrimitive( dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,dwFlags )
//#define wDrawIndexedPrimitive(d3dDevice7,d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,dwFlags ) d3dDevice7->DrawIndexedPrimitive(d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,dwFlags )
//#endif
HRESULT __stdcall wDrawPrimitiveVB(LPDIRECT3DDEVICE7 d3dDevice7, D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, ULONG dwStartVertex, ULONG dwNumVertices, ULONG dwFlags );
HRESULT __stdcall wDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE7 d3dDevice7, D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, ULONG dwStartVertex, ULONG dwNumVertices, PUSHORT lpwIndices, ULONG dwIndexCount, ULONG dwFlags );
HRESULT __stdcall wSetMaterial(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DMATERIAL7 lpMaterial );
HRESULT __stdcall wSetLight(LPDIRECT3DDEVICE7 d3dDevice7, ULONG dwLightIndex, LPD3DLIGHT7 lpLight );
HRESULT __stdcall wLightEnable(LPDIRECT3DDEVICE7 d3dDevice7, ULONG dwLightIndex, UCHAR Enable );
HRESULT __stdcall wMultiplyTransform(LPDIRECT3DDEVICE7 d3dDevice7, D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix );
HRESULT __stdcall wSetTransform(LPDIRECT3DDEVICE7 d3dDevice7, D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix );
HRESULT __stdcall wBeginScene(LPDIRECT3DDEVICE7 d3dDevice7 );
HRESULT __stdcall wEndScene(LPDIRECT3DDEVICE7 d3dDevice7 );
HRESULT __stdcall wGetCaps(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DDEVICEDESC7 lpD3DHWDevDesc );
HRESULT __stdcall wSetRenderState(LPDIRECT3DDEVICE7 d3dDevice7, D3DRENDERSTATETYPE dwRenderStateType, ULONG dwRenderState );
HRESULT __stdcall wEnumTextureFormats(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, PVOID lpArg );
HRESULT __stdcall wSetTexture(LPDIRECT3DDEVICE7 d3dDevice7, ULONG dwStage, LPDIRECTDRAWSURFACE7 lpTexture );
HRESULT __stdcall wSetTextureStageState(LPDIRECT3DDEVICE7 d3dDevice7, ULONG dwStage, D3DTEXTURESTAGESTATETYPE dwState, ULONG dwValue );
HRESULT __stdcall wValidateDevice(LPDIRECT3DDEVICE7 d3dDevice7, PULONG pExtraPasses );
HRESULT __stdcall wLoad(LPDIRECT3DDEVICE7 d3dDevice7, LPDIRECTDRAWSURFACE7 lpDestTex, LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, LPRECT lprcSrcRect, ULONG dwFlags );
HRESULT __stdcall wSetViewport(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DVIEWPORT7 lpViewport );
HRESULT __stdcall wClear(LPDIRECT3DDEVICE7 d3dDevice7, ULONG dwCount, LPD3DRECT lpRects, ULONG dwFlags, ULONG dwColor, D3DVALUE dvZ, ULONG dwStencil );

// IDirect3D7
HRESULT __stdcall wCreateDevice(LPDIRECT3D7 d3d7, REFCLSID rclsid, LPDIRECTDRAWSURFACE7 lpDDS, LPDIRECT3DDEVICE7* lplpD3DDevice7 );
HRESULT __stdcall wEnumZBufferFormats(LPDIRECT3D7 d3d7, REFCLSID riidDevice, LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, PVOID pContext );
HRESULT __stdcall wEnumDevices(LPDIRECT3D7 d3d7, LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, PVOID lpUserArg );
HRESULT __stdcall wCreateVertexBuffer(LPDIRECT3D7 d3d7, LPD3DVERTEXBUFFERDESC lpVBDesc, LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, ULONG dwFlags );

// IDirect3DVertexBuffer7
HRESULT __stdcall wLock(LPDIRECT3DVERTEXBUFFER7 D3Dvb7, ULONG dwFlags, PVOID* plpData, PULONG pdwSize );
HRESULT __stdcall wUnlock(LPDIRECT3DVERTEXBUFFER7 D3Dvb7 );
HRESULT __stdcall wOptimize(LPDIRECT3DVERTEXBUFFER7 D3Dvb7, LPDIRECT3DDEVICE7 lpD3DDevice, ULONG dwFlags );
HRESULT __stdcall wProcessVertices(LPDIRECT3DVERTEXBUFFER7 D3Dvb7, ULONG dwVertexOp, ULONG dwDestIndex, ULONG dwCount, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer, ULONG dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, ULONG dwFlags );

// Direct Draw
HRESULT __stdcall wDirectDrawCreateEx( LPGUID lpGUID, PVOID* lplpDD, REFIID iid, IUnknown* pUnkOuter );
HRESULT __stdcall wDirectDrawEnumerate( LPDDENUMCALLBACK lpCallback,LPDDENUMCALLBACKEX lpCallbackEx,PVOID lpContext );

HRESULT __stdcall wSetHWnd( LPDIRECTDRAWCLIPPER lpddc, ULONG dwFlags, HWND hWnd );

HRESULT __stdcall wIsLost( LPDIRECTDRAWSURFACE7 lpdds7 );
HRESULT __stdcall wGetDC( LPDIRECTDRAWSURFACE7 lpdds7, HDC* lphDC );
HRESULT __stdcall wReleaseDC( LPDIRECTDRAWSURFACE7 lpdds7, HDC hDC );
HRESULT __stdcall wSetColorKey( LPDIRECTDRAWSURFACE7 lpdd3, ULONG dwFlags, LPDDCOLORKEY lpDDColorKey );
HRESULT __stdcall wLock( LPDIRECTDRAWSURFACE7 lpdd7, LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, ULONG dwFlags, HANDLE hEvent );
HRESULT __stdcall wUnlock( LPDIRECTDRAWSURFACE7 lpdds7, LPRECT lprect );
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE lpdds, LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, ULONG dwFlags, HANDLE hEvent );
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect );
HRESULT __stdcall wSetPalette( LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWPALETTE lpDDPalette );
HRESULT __stdcall wRestore( LPDIRECTDRAWSURFACE7 lpdd7);
HRESULT __stdcall wBlt( LPDIRECTDRAWSURFACE7 lpdd7, LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, ULONG dwFlags, LPDDBLTFX lpDDBltFx );
HRESULT __stdcall wBltFast( LPDIRECTDRAWSURFACE7 lpdd7, ULONG dwX, ULONG dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, ULONG dwTrans );
HRESULT __stdcall wFlip( LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, ULONG dwFlags );
HRESULT __stdcall wGetSurfaceDesc( LPDIRECTDRAWSURFACE7 lpdd7, LPDDSURFACEDESC2 lpDDSurfaceDesc );
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpDDSurfaceDesc );
HRESULT __stdcall wGetAttachedSurface( LPDIRECTDRAWSURFACE7 lpdds7, LPDDSCAPS2 lpDDSCaps2, LPDIRECTDRAWSURFACE7* lplpDDAttachedSurface );
HRESULT __stdcall wGetPixelFormat( LPDIRECTDRAWSURFACE7 lpdds7, LPDDPIXELFORMAT lpDDPixelFormat );
HRESULT __stdcall wAddAttachedSurface( LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface );
HRESULT __stdcall wDeleteAttachedSurface( LPDIRECTDRAWSURFACE7 lpdds7, ULONG dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface );
HRESULT __stdcall wSetClipper( LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWCLIPPER lpDDClipper );

HRESULT __stdcall wGetDeviceIdentifier(LPDIRECTDRAW7 lpdd7, LPDDDEVICEIDENTIFIER2 lpdddi2, ULONG dwFlags );
HRESULT __stdcall wGetCaps(LPDIRECTDRAW7 lpdd7, LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps );
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 lpdd7,ULONG dwFlags,LPPALETTEENTRY lpColorTable,LPDIRECTDRAWPALETTE* lplpDDPalette, IUnknown* pUnkOuter );
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 lpdd7, LPDDSURFACEDESC2 lpDDSurfaceDesc, LPDIRECTDRAWSURFACE7* lplpDDSurface7, IUnknown* pUnkOuter );
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTDRAW7 lpdd7, HWND hWnd, ULONG dwFlags );
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 lpdd7,ULONG dwFlags,LPDDSURFACEDESC2 lpDDSurfaceDesc,PVOID lpContext,LPDDENUMMODESCALLBACK2 lpEnumModesCallback );
HRESULT __stdcall wRestoreDisplayMode(LPDIRECTDRAW7 lpdd7 );
HRESULT __stdcall wGetDisplayMode(LPDIRECTDRAW7 lpdd7, LPDDSURFACEDESC2 lpDDSurfaceDesc );
HRESULT __stdcall wCreateClipper(LPDIRECTDRAW7 lpdd7, ULONG dwFlags, LPDIRECTDRAWCLIPPER* lplpDDClipper, IUnknown* pUnkOuter );
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 lpdd7,ULONG dwWidth,ULONG dwHeight,ULONG dwBPP,ULONG dwRefreshRate,ULONG dwFlags );
HRESULT __stdcall wGetAvailableVidMem(LPDIRECTDRAW7 lpdd7, LPDDSCAPS2 lpDDSCaps2, PULONG lpdwTotal, PULONG lpdwFree );
HRESULT __stdcall wGetFourCCCodes(LPDIRECTDRAW7 lpdd7, PULONG lpNumCodes, PULONG lpCodes );

// Direct Sound
HRESULT __stdcall wDirectSoundEnumerate( LPDSENUMCALLBACK cb, PVOID vp );
HRESULT __stdcall wDirectSoundCreate( LPGUID lpGuid, LPDIRECTSOUND* ppDS, IUnknown* pUnkOuter );
HRESULT __stdcall wGetCaps( LPDIRECTSOUND ppDS, DSCAPS* dsc );
HRESULT __stdcall wSetSpeakerConfig( LPDIRECTSOUND ppDS, ULONG dsflags );
HRESULT __stdcall wGetSpeakerConfig( LPDIRECTSOUND ppDS, PULONG dsflags );
HRESULT __stdcall wLock( LPDIRECTSOUNDBUFFER lpdsb, ULONG dwWriteCursor, ULONG dwWriteBytes, PVOID* lplpvAudioPtr1, PULONG lpdwAudioBytes1, PVOID* lplpvAudioPtr2, PULONG lpdwAudioBytes2, ULONG dwFlags );
HRESULT __stdcall wUnlock( LPDIRECTSOUNDBUFFER lpdsb, PVOID lplpvAudioPtr1, ULONG lpdwAudioBytes1, PVOID lplpvAudioPtr2, ULONG lpdwAudioBytes2 );
HRESULT __stdcall wGetVolume( LPDIRECTSOUNDBUFFER lpdsb, LONG* vol );
HRESULT __stdcall wGetFrequency( LPDIRECTSOUNDBUFFER lpdsb, PULONG freq);
HRESULT __stdcall wGetPan( LPDIRECTSOUNDBUFFER lpdsb, LONG* pan);
HRESULT __stdcall wSetVolume( LPDIRECTSOUNDBUFFER lpdsb, LONG vol );
HRESULT __stdcall wSetFrequency( LPDIRECTSOUNDBUFFER lpdsb, ULONG freq);
HRESULT __stdcall wSetPan( LPDIRECTSOUNDBUFFER lpdsb, LONG pan);
HRESULT __stdcall wPlay( LPDIRECTSOUNDBUFFER lpdsb, ULONG res1, ULONG res2, ULONG flags);
HRESULT __stdcall wStop( LPDIRECTSOUNDBUFFER lpdsb);
HRESULT __stdcall wGetStatus( LPDIRECTSOUNDBUFFER lpdsb, PULONG status);
HRESULT __stdcall wSetCurrentPosition( LPDIRECTSOUNDBUFFER lpdsb, ULONG pos);
HRESULT __stdcall wGetCurrentPosition( LPDIRECTSOUNDBUFFER lpdsb, PULONG lpdwCurrentPlayCursor, PULONG lpdwCurrentWriteCursor );
HRESULT __stdcall wCreateSoundBuffer( LPDIRECTSOUND lpds, LPCDSBUFFERDESC lpcDSBufferDesc, LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer, IUnknown* pUnkOuter );
HRESULT __stdcall wSetFormat( LPDIRECTSOUNDBUFFER lpdsb, LPCWAVEFORMATEX lpcfxFormat );
HRESULT __stdcall wGetCaps( LPDIRECTSOUNDBUFFER ppDSB, DSBCAPS* dsc );
HRESULT __stdcall wSetCooperativeLevel( LPDIRECTSOUND lpds, HWND hwnd, ULONG dwLevel );
HRESULT __stdcall wPlay( LPDIRECTSOUNDBUFFER lpdsb, ULONG dwReserved1, ULONG dwReserved2, ULONG dwFlags );
HRESULT __stdcall wSetRolloffFactor( LPDIRECTSOUND3DLISTENER lpdsb, float dist, ULONG flag );
HRESULT __stdcall wSetDopplerFactor( LPDIRECTSOUND3DLISTENER lpdsb, float dist, ULONG flag );
HRESULT __stdcall wSetDistanceFactor( LPDIRECTSOUND3DLISTENER lpdsb, float dist, ULONG flag );
HRESULT __stdcall wSetOrientation( LPDIRECTSOUND3DLISTENER lpdsb, float fx, float fy, float fz, float tx, float ty, float tz, ULONG flag );
HRESULT __stdcall wSetPosition( LPDIRECTSOUND3DBUFFER lpdsb, float x, float y, float z, ULONG flag );
HRESULT __stdcall wSetPosition( LPDIRECTSOUND3DLISTENER lpdsb, float x, float y, float z, ULONG flag );
HRESULT __stdcall wSetVelocity( LPDIRECTSOUND3DBUFFER lpdsb, float x, float y, float z, ULONG flag );
HRESULT __stdcall wSetVelocity( LPDIRECTSOUND3DLISTENER lpdsb, float x, float y, float z, ULONG flag );
HRESULT __stdcall wSetMinDistance( LPDIRECTSOUND3DBUFFER lpdsb, float x, ULONG flag );
HRESULT __stdcall wSetMaxDistance( LPDIRECTSOUND3DBUFFER lpdsb, float x, ULONG flag );
HRESULT __stdcall wSetConeAngles( LPDIRECTSOUND3DBUFFER ppDSB3D, ULONG min, ULONG max, ULONG flag );
HRESULT __stdcall wSetConeOrientation( LPDIRECTSOUND3DBUFFER ppDSB3D, float x, float y, float z, ULONG flag );
HRESULT __stdcall wCommitDeferredSettings( LPDIRECTSOUND3DLISTENER lpList );
HRESULT __stdcall wQuerySupport( LPKSPROPERTYSET lpPS, REFGUID rGUID, ULONG uLong, PULONG support );
HRESULT __stdcall wSet( LPKSPROPERTYSET lpPS, REFGUID rguidPropSet, ULONG ulId, PVOID pInstanceData, ULONG ulInstanceLength, PVOID pPropertyData, ULONG ulDataLength );
HRESULT __stdcall wGetFormat( LPDIRECTSOUNDBUFFER lpdsb, LPWAVEFORMATEX lpwf, ULONG sizeIn, PULONG sizeOut );
HRESULT __stdcall wSetAllParameters(LPDIRECTSOUND3DBUFFER lpdsb, DS3DBUFFER * ds3d, ULONG flag);
HRESULT __stdcall wSetAllParameters(LPDIRECTSOUND3DLISTENER lpdsb, DS3DLISTENER * ds3d, ULONG flag);
HRESULT __stdcall wDuplicateSoundBuffer(LPDIRECTSOUND lpds, LPDIRECTSOUNDBUFFER src, LPDIRECTSOUNDBUFFER * dst );

typedef LONGLONG STREAM_TIME;
typedef struct IMultiMediaStream*			PIMULTIMEDIASTREAM;
typedef struct IDirectDrawMediaStream*		PIDIRECTDRAWMEDIASTREAM;
typedef struct IAMMultiMediaStream*			PIAMMULTIMEDIASTREAM;
typedef struct IMediaStream*				PIMEDIASTREAM;
typedef struct IGraphBuilder*				PIGRAPHBUILDER;

#if _CONSIDERED_OBSOLETE
// Direct Show
HRESULT __stdcall wSetState( PIMULTIMEDIASTREAM imms, STREAM_STATE NewState );
HRESULT __stdcall wGetState( PIMULTIMEDIASTREAM imms, STREAM_STATE * NewState );
HRESULT __stdcall wSeek( PIMULTIMEDIASTREAM imms, STREAM_TIME SeekTime );
HRESULT __stdcall wGetTime( PIMULTIMEDIASTREAM imms, STREAM_TIME* SeekTime );
HRESULT __stdcall wGetDuration( PIMULTIMEDIASTREAM imms, STREAM_TIME *theTime );
HRESULT __stdcall wGetMediaStream( PIMULTIMEDIASTREAM imms, REFMSPID idPurpose, PIMEDIASTREAM* pMediaStream );

HRESULT __stdcall wInitialize( PIAMMULTIMEDIASTREAM iammms, STREAM_TYPE StreamType, ULONG dwFlags, PIGRAPHBUILDER pFilterGraph );
HRESULT __stdcall wAddMediaStream( PIAMMULTIMEDIASTREAM iammms, IUnknown* pStreamObject, const MSPID *pPurposeID, ULONG dwFlags, PIMEDIASTREAM* ppNewStream );
HRESULT __stdcall wOpenFile( PIAMMULTIMEDIASTREAM iammms, LPCWSTR pszFileName, ULONG dwFlags );

HRESULT __stdcall wCreateSample( PIDIRECTDRAWMEDIASTREAM iddms,LPDIRECTDRAWSURFACE pSurface, const RECT *pRect, ULONG dwFlags, IDirectDrawStreamSample** ppSample );
HRESULT __stdcall wGetSurface( IDirectDrawStreamSample* iddss, IDirectDrawSurface ** ppDirectDrawSurface, RECT* pRect );

HRESULT __stdcall wSetFormat( PIDIRECTDRAWMEDIASTREAM iddss, DDSURFACEDESC* ddsd );
HRESULT __stdcall wGetFormat( PIDIRECTDRAWMEDIASTREAM iddss, DDSURFACEDESC* ddsd_current, DDSURFACEDESC* ddsd_desired, PULONG flags );
HRESULT __stdcall wSetSampleTimes( IStreamSample* iddss, STREAM_TIME* st1, STREAM_TIME* st2 );
HRESULT __stdcall wUpdate( IStreamSample* iss, ULONG flags, HANDLE event, PAPCFUNC papcfunc, ULONG apc );
HRESULT __stdcall wGetFilterGraph( PIAMMULTIMEDIASTREAM iamms, PIGRAPHBUILDER* igb );
HRESULT __stdcall wSetPositions( IMediaSeeking* ims, STREAM_TIME* st, ULONG stflags, STREAM_TIME* end, ULONG eflags );
HRESULT __stdcall wGetStopPosition( IMediaSeeking* ims, STREAM_TIME* st );
HRESULT __stdcall wGetSampleTimes( IDirectDrawStreamSample* iddss, STREAM_TIME* st, STREAM_TIME* end, STREAM_TIME* cur );

HRESULT __stdcall wPut_CurrentPosition( IMediaPosition* iba, REFTIME value );
HRESULT __stdcall wGet_CurrentPosition( IMediaPosition* iba, REFTIME *value );
HRESULT __stdcall wCanSeekForward( IMediaPosition* iba, LONG * canDo );
HRESULT __stdcall wGet_Duration( IMediaPosition* iba, REFTIME *value );
HRESULT __stdcall wStop( IMediaControl* iba );
HRESULT __stdcall wPause( IMediaControl* iba );
HRESULT __stdcall wRun( IMediaControl* iba );
HRESULT __stdcall wPut_Volume( IBasicAudio* iba, LONG value );
HRESULT __stdcall wPut_Balance( IBasicAudio* iba, LONG value );
HRESULT __stdcall wGetEndOfStreamEventHandle(PIMULTIMEDIASTREAM imms, HANDLE * ph);
HRESULT __stdcall wGetInformation( PIMULTIMEDIASTREAM imms, PULONG  flags, STREAM_TYPE *theType );
#endif

//
// Direct Input
//
HRESULT __stdcall wDirectInputCreateEx( HINSTANCE hinst, ULONG dwVersion, REFIID riidltf, PVOID* ppvOut, LPUNKNOWN punkOuter );
HRESULT __stdcall wCreateDeviceEx( LPDIRECTINPUT7 lpdi, REFGUID rguid, REFIID riid, PVOID* lplpDirectInputDevice, LPUNKNOWN pUnkOuter );
HRESULT __stdcall wSetDataFormat( LPDIRECTINPUTDEVICE7 dev, LPCDIDATAFORMAT lpdf );
HRESULT __stdcall wSetCooperativeLevel( LPDIRECTINPUTDEVICE7 dev, HWND hwnd, ULONG dwFlags );
HRESULT __stdcall wUnacquire( LPDIRECTINPUTDEVICE7 dev );
HRESULT __stdcall wAcquire( LPDIRECTINPUTDEVICE7 dev );
HRESULT __stdcall wGetDeviceState( LPDIRECTINPUTDEVICE7 dev, ULONG cbData, PVOID lpvData );
HRESULT __stdcall wEnumDevices( LPDIRECTINPUT7 di, ULONG dwDevType, LPDIENUMDEVICESCALLBACK lpCallback, PVOID pvRef, ULONG dwFlags );
HRESULT __stdcall wSetProperty( LPDIRECTINPUTDEVICE7 dev, REFGUID rguidProp, LPCDIPROPHEADER pdiph );
HRESULT __stdcall wGetDeviceInfo( LPDIRECTINPUTDEVICE7 dev, LPDIDEVICEINSTANCE pdidi );
HRESULT __stdcall wGetCapabilities( LPDIRECTINPUTDEVICE7 dev, LPDIDEVCAPS lpDIDevCaps );
HRESULT __stdcall wGetDeviceData( LPDIRECTINPUTDEVICE7 dev, ULONG cbObjectData, LPDIDEVICEOBJECTDATA rgdod, PULONG pdwInOut, ULONG dwFlags );
HRESULT __stdcall wPoll( LPDIRECTINPUTDEVICE7 lpdid2 );
HRESULT __stdcall wCreateEffect( LPDIRECTINPUTDEVICE7 lpdid2, REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter );
HRESULT __stdcall wStart( LPDIRECTINPUTEFFECT lpeff, ULONG iterations, ULONG flags );
HRESULT __stdcall wStop( LPDIRECTINPUTEFFECT lpeff );
HRESULT __stdcall wGetEffectStatus( LPDIRECTINPUTEFFECT lpeff, PULONG  status );
HRESULT __stdcall wDownload( LPDIRECTINPUTEFFECT lpeff );
HRESULT __stdcall wSetParameters( LPDIRECTINPUTEFFECT lpeff, LPCDIEFFECT lpdi, ULONG flags );


#if _CONSIDERED_OBSOLETE
// DirectPlay
HRESULT __stdcall wAddPlayerToGroup( IDirectPlay4A* dplay4, DPID idGroup, DPID idPlayer );
HRESULT __stdcall wCancelMessage( IDirectPlay4A* dplay4, ULONG dwMsgID, ULONG dwFlags );
HRESULT __stdcall wCancelPriority( IDirectPlay4A* dplay4, ULONG dwMinPriority, ULONG dwMaxPriority, ULONG dwFlags );
HRESULT __stdcall wClose( IDirectPlay4A* dplay4 );
HRESULT __stdcall wCreateGroup( IDirectPlay4A* dplay4, LPDPID lpidGroup, LPDPNAME lpGroupName, PVOID lpData, ULONG dwDataSize, ULONG dwFlags );
HRESULT __stdcall wCreateGroupInGroup( IDirectPlay4A* dplay4, DPID idParentGroup, LPDPID lpidGroup, LPDPNAME lpGroupName, PVOID lpData, ULONG dwDataSize, ULONG dwFlags );
HRESULT __stdcall wCreatePlayer( IDirectPlay4A* dplay4, LPDPID lpidPlayer, LPDPNAME lpPlayerName, HANDLE hEvent, PVOID lpData, ULONG dwDataSize, ULONG dwFlags );
HRESULT __stdcall wDeleteGroupFromGroup( IDirectPlay4A* dplay4, DPID idParentGroup, DPID idGroup );
HRESULT __stdcall wDeletePlayerFromGroup( IDirectPlay4A* dplay4, DPID idGroup, DPID idPlayer );
HRESULT __stdcall wDestroyGroup( IDirectPlay4A* dplay4, DPID idGroup );
HRESULT __stdcall wDestroyPlayer( IDirectPlay4A* dplay4, DPID idPlayer );
HRESULT __stdcall wEnumConnections( IDirectPlay4A* dplay4, LPCGUID lpguidApplication, LPDPENUMCONNECTIONSCALLBACK lpEnumCallback, PVOID lpContext, ULONG dwFlags );
HRESULT __stdcall wEnumGroupPlayers( IDirectPlay4A* dplay4, DPID idGroup, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID lpContext, ULONG dwFlags );
HRESULT __stdcall wEnumGroups( IDirectPlay4A* dplay4, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID lpContext, ULONG dwFlags );
HRESULT __stdcall wEnumGroupsInGroup( IDirectPlay4A* dplay4, DPID idGroup, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumCallback, PVOID lpContext, ULONG dwFlags );
HRESULT __stdcall wEnumPlayers( IDirectPlay4A* dplay4, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID lpContext, ULONG dwFlags );
HRESULT __stdcall wEnumSessions( IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpsd, ULONG dwTimeout, LPDPENUMSESSIONSCALLBACK2 lpEnumSessionsCallback2, PVOID lpContext, ULONG dwFlags );
HRESULT __stdcall wGetCaps( IDirectPlay4A* dplay4, LPDPCAPS lpDPCaps, ULONG dwFlags );
HRESULT __stdcall wGetGroupConnectionSettings( IDirectPlay4A* dplay4, ULONG dwFlags, DPID idGroup, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wGetGroupData( IDirectPlay4A* dplay4, DPID idGroup, PVOID lpData, PULONG lpdwDataSize, ULONG dwFlags );
HRESULT __stdcall wGetGroupFlags( IDirectPlay4A* dplay4, DPID idGroup, PULONG lpdwFlags );
HRESULT __stdcall wGetGroupName( IDirectPlay4A* dplay4, DPID idGroup, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wGetGroupOwner( IDirectPlay4A* dplay4, DPID idGroup, LPDPID lpidOwner );
HRESULT __stdcall wGetGroupParent( IDirectPlay4A* dplay4, DPID idGroup, LPDPID lpidParent );
HRESULT __stdcall wGetMessageCount( IDirectPlay4A* dplay4, DPID idPlayer, PULONG lpdwCount );
HRESULT __stdcall wGetMessageQueue( IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, ULONG dwFlags, PULONG lpdwNumMsgs, PULONG lpdwNumBytes );
HRESULT __stdcall wGetPlayerAccount( IDirectPlay4A* dplay4, DPID idPlayer, ULONG dwFlags, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wGetPlayerAddress( IDirectPlay4A* dplay4, DPID idPlayer, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wGetPlayerCaps( IDirectPlay4A* dplay4, DPID idPlayer, LPDPCAPS lpPlayerCaps, ULONG dwFlags );
HRESULT __stdcall wGetPlayerData( IDirectPlay4A* dplay4, DPID idPlayer, PVOID lpData, PULONG lpdwDataSize, ULONG dwFlags );
HRESULT __stdcall wGetPlayerFlags( IDirectPlay4A* dplay4, DPID idPlayer, PULONG lpdwFlags );
HRESULT __stdcall wGetPlayerName( IDirectPlay4A* dplay4, DPID idPlayer, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wGetSessionDesc( IDirectPlay4A* dplay4, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wInitialize( IDirectPlay4A* dplay4, LPGUID lpGUID );
HRESULT __stdcall wInitializeConnection( IDirectPlay4A* dplay4, PVOID lpConnection, ULONG dwFlags );
HRESULT __stdcall wOpen( IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpsd, ULONG dwFlags );
HRESULT __stdcall wReceive( IDirectPlay4A* dplay4, LPDPID lpidFrom, LPDPID lpidTo, ULONG dwFlags, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wSecureOpen( IDirectPlay4A* dplay4, LPCDPSESSIONDESC2 lpsd, ULONG dwFlags, LPCDPSECURITYDESC lpSecurity, LPCDPCREDENTIALS lpCredentials );
HRESULT __stdcall wSendChatMessage( IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, ULONG dwFlags, LPDPCHAT lpChatMessage );
HRESULT __stdcall wSend( IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, ULONG dwFlags, PVOID lpData, ULONG dwDataSize );
HRESULT __stdcall wSendEx( IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, ULONG dwFlags, PVOID lpData, ULONG dwDataSize, ULONG dwPriority, ULONG dwTimeout, PVOID lpContext, PULONG lpdwMsgID );
HRESULT __stdcall wSetGroupConnectionSettings( IDirectPlay4A* dplay4, ULONG dwFlags, DPID idGroup, LPDPLCONNECTION lpConnection );
HRESULT __stdcall wSetGroupData( IDirectPlay4A* dplay4, DPID idGroup, PVOID lpData, ULONG dwDataSize, ULONG dwFlags );
HRESULT __stdcall wSetGroupName( IDirectPlay4A* dplay4, DPID idGroup, LPDPNAME lpGroupName, ULONG dwFlags );
HRESULT __stdcall wSetGroupOwner( IDirectPlay4A* dplay4, DPID idGroup, DPID idOwner );
HRESULT __stdcall wSetPlayerData( IDirectPlay4A* dplay4, DPID idPlayer, PVOID lpData, ULONG dwDataSize, ULONG dwFlags );
HRESULT __stdcall wSetPlayerName( IDirectPlay4A* dplay4, DPID idPlayer, LPDPNAME lpPlayerName, ULONG dwFlags );
HRESULT __stdcall wSetSessionDesc( IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpSessDesc, ULONG dwFlags );
HRESULT __stdcall wStartSession( IDirectPlay4A* dplay4, ULONG dwFlags, DPID idGroup );

// DirectPlay Lobby functions

HRESULT __stdcall wCreateAddress( IDirectPlayLobby3A* dplobby3, REFGUID guidSP, REFGUID guidDataType, LPCVOID lpData, ULONG dwDataSize, PVOID lpAddress, PULONG lpdwAddressSize );
HRESULT __stdcall wCreateCompoundAddress( IDirectPlayLobby3A* dplobby3, LPDPCOMPOUNDADDRESSELEMENT lpElements, ULONG dwElementCount, PVOID lpAddress, PULONG lpdwAddressSize );
HRESULT __stdcall wEnumAddress( IDirectPlayLobby3A* dplobby3, LPDPENUMADDRESSCALLBACK lpEnumAddressCallback, LPCVOID lpAddress, ULONG dwAddressSize, PVOID lpContext );
HRESULT __stdcall wGetConnectionSettings( IDirectPlayLobby3A* dplobby3, ULONG dwAppID, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wReceiveLobbyMessage( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, ULONG dwAppID, PULONG lpdwMessageFlags, PVOID lpData, PULONG lpdwDataSize );
HRESULT __stdcall wRegisterApplication( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, LPDPAPPLICATIONDESC lpAppDesc );
HRESULT __stdcall wRunApplication( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, PULONG lpdwAppID, LPDPLCONNECTION lpConn, HANDLE hReceiveEvent );
HRESULT __stdcall wSendLobbyMessage( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, ULONG dwAppID, PVOID lpData, ULONG dwDataSize );
HRESULT __stdcall wSetConnectionSettings( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, ULONG dwAppID, LPDPLCONNECTION lpConn );
HRESULT __stdcall wSetLobbyMessageEvent( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, ULONG dwAppID, HANDLE hReceiveEvent );
HRESULT __stdcall wUnregisterApplication( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, REFGUID guidApplication );
HRESULT __stdcall wWaitForConnectionSettings( IDirectPlayLobby3A* dplobby3, ULONG dwFlags );
HRESULT __stdcall wConnectEx( IDirectPlayLobby3A* dplobby3, ULONG dwFlags, REFIID riid, PVOID *lplpDP, IUnknown* pUnk );
#endif
