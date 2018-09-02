//===========================================================================//
// File:	 DirectX.hpp													 //
// Contents: Interface to DirectSetup										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

extern uint8_t AllowFail;
extern volatile uint16_t FPUControl; // Current FPU control word
extern uint16_t FPUDefault;			 // Default FPU control word
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
// DEFINE_GUID( IID_IDirect3DBladeDevice,    0x706e9520, 0x6db1, 0x11d2, 0xb9,
// 0x1, 0x0, 0x10, 0x4b, 0x36, 0xae, 0x4b);

//
// Used to save and restore FPU control word around DirectX functions
//
//#if defined(LAB_ONLY)
//#define SafeFPU() _asm fldcw FPUDefault
//#define GameOSFPU() _asm fninit _asm fldcw FPUControl
//#else
//#define SafeFPU()
//#define GameOSFPU()
//#endif

//
// General DirectX
//
HRESULT __stdcall wCoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter,uint32_t dwClsContext, REFIID riid, PVOID* ppv);
HRESULT __stdcall wQueryInterface(IUnknown* iun, REFIID riid, PVOID* obp);
uint32_t __stdcall wRelease(IUnknown* iun);
uint32_t __stdcall wAddRef(IUnknown* iun);

//
// Direct Setup
//
void wDirectXSetupGetVersion(puint32_t dwVersion, puint32_t dwRevision);

// IDirect3DDevice7
//#ifdef LAB_ONLY
HRESULT __stdcall wDrawPrimitive(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE dptPrimitiveType, uint32_t dvtVertexType,
	PVOID lpvVertices, uint32_t dwVertexCount, uint32_t dwFlags);
HRESULT __stdcall wDrawIndexedPrimitive(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, uint32_t dwVertexTypeDesc,
	PVOID pvVertices, uint32_t dwVertexCount, puint16_t pwIndices,
	uint32_t dwIndexCount, uint32_t dwFlags);
//#else
//#define
//wDrawPrimitive(d3dDevice7,dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,dwFlags
//) d3dDevice7->DrawPrimitive(
//dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,dwFlags ) #define
//wDrawIndexedPrimitive(d3dDevice7,d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,dwFlags
//)
//d3dDevice7->DrawIndexedPrimitive(d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,dwFlags
//) #endif
HRESULT __stdcall wDrawPrimitiveVB(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType,
	LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, uint32_t dwStartVertex,
	uint32_t dwNumVertices, uint32_t dwFlags);
HRESULT __stdcall wDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType,
	LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer, uint32_t dwStartVertex,
	uint32_t dwNumVertices, puint16_t lpwIndices, uint32_t dwIndexCount,
	uint32_t dwFlags);
HRESULT __stdcall wSetMaterial(
	LPDIRECT3DDEVICE7 d3dDevice7, LPD3DMATERIAL7 lpMaterial);
HRESULT __stdcall wSetLight(
	LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwLightIndex, LPD3DLIGHT7 lpLight);
HRESULT __stdcall wLightEnable(
	LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwLightIndex, uint8_t Enable);
HRESULT __stdcall wMultiplyTransform(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix);
HRESULT __stdcall wSetTransform(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix);
HRESULT __stdcall wBeginScene(LPDIRECT3DDEVICE7 d3dDevice7);
HRESULT __stdcall wEndScene(LPDIRECT3DDEVICE7 d3dDevice7);
HRESULT __stdcall wGetCaps(
	LPDIRECT3DDEVICE7 d3dDevice7, LPD3DDEVICEDESC7 lpD3DHWDevDesc);
HRESULT __stdcall wSetRenderState(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DRENDERSTATETYPE dwRenderStateType, uint32_t dwRenderState);
HRESULT __stdcall wEnumTextureFormats(LPDIRECT3DDEVICE7 d3dDevice7,
	LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, PVOID lpArg);
HRESULT __stdcall wSetTexture(LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwStage,
	LPDIRECTDRAWSURFACE7 lpTexture);
HRESULT __stdcall wSetTextureStageState(LPDIRECT3DDEVICE7 d3dDevice7,
	uint32_t dwStage, D3DTEXTURESTAGESTATETYPE dwState, uint32_t dwValue);
HRESULT __stdcall wValidateDevice(
	LPDIRECT3DDEVICE7 d3dDevice7, puint32_t pExtraPasses);
HRESULT __stdcall wLoad(LPDIRECT3DDEVICE7 d3dDevice7,
	LPDIRECTDRAWSURFACE7 lpDestTex, LPPOINT lpDestPoint,
	LPDIRECTDRAWSURFACE7 lpSrcTex, LPRECT lprcSrcRect, uint32_t dwFlags);
HRESULT __stdcall wSetViewport(
	LPDIRECT3DDEVICE7 d3dDevice7, LPD3DVIEWPORT7 lpViewport);
HRESULT __stdcall wClear(LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwCount,
	LPD3DRECT lpRects, uint32_t dwFlags, uint32_t dwColor, D3DVALUE dvZ,
	uint32_t dwStencil);

// IDirect3D7
HRESULT __stdcall wCreateDevice(LPDIRECT3D7 d3d7, REFCLSID rclsid,
	LPDIRECTDRAWSURFACE7 lpDDS, LPDIRECT3DDEVICE7* lplpD3DDevice7);
HRESULT __stdcall wEnumZBufferFormats(LPDIRECT3D7 d3d7, REFCLSID riidDevice,
	LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, PVOID pContext);
HRESULT __stdcall wEnumDevices(LPDIRECT3D7 d3d7,
	LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, PVOID lpUserArg);
HRESULT __stdcall wCreateVertexBuffer(LPDIRECT3D7 d3d7,
	LPD3DVERTEXBUFFERDESC lpVBDesc,
	LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, uint32_t dwFlags);

// IDirect3DVertexBuffer7
HRESULT __stdcall wLock(LPDIRECT3DVERTEXBUFFER7 D3Dvb7, uint32_t dwFlags,
	PVOID* plpData, puint32_t pdwSize);
HRESULT __stdcall wUnlock(LPDIRECT3DVERTEXBUFFER7 D3Dvb7);
HRESULT __stdcall wOptimize(LPDIRECT3DVERTEXBUFFER7 D3Dvb7,
	LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t dwFlags);
HRESULT __stdcall wProcessVertices(LPDIRECT3DVERTEXBUFFER7 D3Dvb7,
	uint32_t dwVertexOp, uint32_t dwDestIndex, uint32_t dwCount,
	LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer, uint32_t dwSrcIndex,
	LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t dwFlags);

// Direct Draw
HRESULT __stdcall wDirectDrawCreateEx(
	LPGUID lpGUID, PVOID* lplpDD, REFIID iid, IUnknown* pUnkOuter);
HRESULT __stdcall wDirectDrawEnumerate(LPDDENUMCALLBACK lpCallback,
	LPDDENUMCALLBACKEX lpCallbackEx, PVOID lpContext);

HRESULT __stdcall wSetHWnd(
	LPDIRECTDRAWCLIPPER lpddc, uint32_t dwFlags, HWND hWnd);

HRESULT __stdcall wIsLost(LPDIRECTDRAWSURFACE7 lpdds7);
HRESULT __stdcall wGetDC(LPDIRECTDRAWSURFACE7 lpdds7, HDC* lphDC);
HRESULT __stdcall wReleaseDC(LPDIRECTDRAWSURFACE7 lpdds7, HDC hDC);
HRESULT __stdcall wSetColorKey(
	LPDIRECTDRAWSURFACE7 lpdd3, uint32_t dwFlags, LPDDCOLORKEY lpDDColorKey);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 lpdd7, LPRECT lpDestRect,
	LPDDSURFACEDESC2 lpDDSurfaceDesc, uint32_t dwFlags, HANDLE hEvent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE7 lpdds7, LPRECT lprect);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE lpdds, LPRECT lpDestRect,
	LPDDSURFACEDESC lpDDSurfaceDesc, uint32_t dwFlags, HANDLE hEvent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE lpdds, LPRECT lprect);
HRESULT __stdcall wSetPalette(
	LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWPALETTE lpDDPalette);
HRESULT __stdcall wRestore(LPDIRECTDRAWSURFACE7 lpdd7);
HRESULT __stdcall wBlt(LPDIRECTDRAWSURFACE7 lpdd7, LPRECT lpDestRect,
	LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, uint32_t dwFlags,
	LPDDBLTFX lpDDBltFx);
HRESULT __stdcall wBltFast(LPDIRECTDRAWSURFACE7 lpdd7, uint32_t dwX,
	uint32_t dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect,
	uint32_t dwTrans);
HRESULT __stdcall wFlip(LPDIRECTDRAWSURFACE7 lpdd7,
	LPDIRECTDRAWSURFACE7 lpDDSurfaceTargetOverride, uint32_t dwFlags);
HRESULT __stdcall wGetSurfaceDesc(
	LPDIRECTDRAWSURFACE7 lpdd7, LPDDSURFACEDESC2 lpDDSurfaceDesc);
HRESULT __stdcall wGetSurfaceDesc(
	LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC lpDDSurfaceDesc);
HRESULT __stdcall wGetAttachedSurface(LPDIRECTDRAWSURFACE7 lpdds7,
	LPDDSCAPS2 lpDDSCaps2, LPDIRECTDRAWSURFACE7* lplpDDAttachedSurface);
HRESULT __stdcall wGetPixelFormat(
	LPDIRECTDRAWSURFACE7 lpdds7, LPDDPIXELFORMAT lpDDPixelFormat);
HRESULT __stdcall wAddAttachedSurface(
	LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
HRESULT __stdcall wDeleteAttachedSurface(LPDIRECTDRAWSURFACE7 lpdds7,
	uint32_t dwFlags, LPDIRECTDRAWSURFACE7 lpDDSAttachedSurface);
HRESULT __stdcall wSetClipper(
	LPDIRECTDRAWSURFACE7 lpdd7, LPDIRECTDRAWCLIPPER lpDDClipper);

HRESULT __stdcall wGetDeviceIdentifier(
	LPDIRECTDRAW7 lpdd7, LPDDDEVICEIDENTIFIER2 lpdddi2, uint32_t dwFlags);
HRESULT __stdcall wGetCaps(
	LPDIRECTDRAW7 lpdd7, LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps);
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 lpdd7, uint32_t dwFlags,
	LPPALETTEENTRY lpColorTable, LPDIRECTDRAWPALETTE* lplpDDPalette,
	IUnknown* pUnkOuter);
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 lpdd7,
	LPDDSURFACEDESC2 lpDDSurfaceDesc, LPDIRECTDRAWSURFACE7* lplpDDSurface7,
	IUnknown* pUnkOuter);
HRESULT __stdcall wSetCooperativeLevel(
	LPDIRECTDRAW7 lpdd7, HWND hWnd, uint32_t dwFlags);
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 lpdd7, uint32_t dwFlags,
	LPDDSURFACEDESC2 lpDDSurfaceDesc, PVOID lpContext,
	LPDDENUMMODESCALLBACK2 lpEnumModesCallback);
HRESULT __stdcall wRestoreDisplayMode(LPDIRECTDRAW7 lpdd7);
HRESULT __stdcall wGetDisplayMode(
	LPDIRECTDRAW7 lpdd7, LPDDSURFACEDESC2 lpDDSurfaceDesc);
HRESULT __stdcall wCreateClipper(LPDIRECTDRAW7 lpdd7, uint32_t dwFlags,
	LPDIRECTDRAWCLIPPER* lplpDDClipper, IUnknown* pUnkOuter);
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 lpdd7, uint32_t dwWidth,
	uint32_t dwHeight, uint32_t dwBPP, uint32_t dwRefreshRate,
	uint32_t dwFlags);
HRESULT __stdcall wGetAvailableVidMem(LPDIRECTDRAW7 lpdd7,
	LPDDSCAPS2 lpDDSCaps2, puint32_t lpdwTotal, puint32_t lpdwFree);
HRESULT __stdcall wGetFourCCCodes(
	LPDIRECTDRAW7 lpdd7, puint32_t lpNumCodes, puint32_t lpCodes);

// Direct Sound
HRESULT __stdcall wDirectSoundEnumerate(LPDSENUMCALLBACK cb, PVOID vp);
HRESULT __stdcall wDirectSoundCreate(
	LPGUID lpGuid, LPDIRECTSOUND* ppDS, IUnknown* pUnkOuter);
HRESULT __stdcall wGetCaps(LPDIRECTSOUND ppDS, DSCAPS* dsc);
HRESULT __stdcall wSetSpeakerConfig(LPDIRECTSOUND ppDS, uint32_t dsflags);
HRESULT __stdcall wGetSpeakerConfig(LPDIRECTSOUND ppDS, puint32_t dsflags);
HRESULT __stdcall wLock(LPDIRECTSOUNDBUFFER lpdsb, uint32_t dwWriteCursor,
	uint32_t dwWriteBytes, PVOID* lplpvAudioPtr1, puint32_t lpdwAudioBytes1,
	PVOID* lplpvAudioPtr2, puint32_t lpdwAudioBytes2, uint32_t dwFlags);
HRESULT __stdcall wUnlock(LPDIRECTSOUNDBUFFER lpdsb, PVOID lplpvAudioPtr1,
	uint32_t lpdwAudioBytes1, PVOID lplpvAudioPtr2, uint32_t lpdwAudioBytes2);
HRESULT __stdcall wGetVolume(LPDIRECTSOUNDBUFFER lpdsb, int32_t* vol);
HRESULT __stdcall wGetFrequency(LPDIRECTSOUNDBUFFER lpdsb, puint32_t freq);
HRESULT __stdcall wGetPan(LPDIRECTSOUNDBUFFER lpdsb, int32_t* pan);
HRESULT __stdcall wSetVolume(LPDIRECTSOUNDBUFFER lpdsb, int32_t vol);
HRESULT __stdcall wSetFrequency(LPDIRECTSOUNDBUFFER lpdsb, uint32_t freq);
HRESULT __stdcall wSetPan(LPDIRECTSOUNDBUFFER lpdsb, int32_t pan);
HRESULT __stdcall wPlay(
	LPDIRECTSOUNDBUFFER lpdsb, uint32_t res1, uint32_t res2, uint32_t flags);
HRESULT __stdcall wStop(LPDIRECTSOUNDBUFFER lpdsb);
HRESULT __stdcall wGetStatus(LPDIRECTSOUNDBUFFER lpdsb, puint32_t status);
HRESULT __stdcall wSetCurrentPosition(LPDIRECTSOUNDBUFFER lpdsb, uint32_t pos);
HRESULT __stdcall wGetCurrentPosition(LPDIRECTSOUNDBUFFER lpdsb,
	puint32_t lpdwCurrentPlayCursor, puint32_t lpdwCurrentWriteCursor);
HRESULT __stdcall wCreateSoundBuffer(LPDIRECTSOUND lpds,
	LPCDSBUFFERDESC lpcDSBufferDesc,
	LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer, IUnknown* pUnkOuter);
HRESULT __stdcall wSetFormat(
	LPDIRECTSOUNDBUFFER lpdsb, LPCWAVEFORMATEX lpcfxFormat);
HRESULT __stdcall wGetCaps(LPDIRECTSOUNDBUFFER ppDSB, DSBCAPS* dsc);
HRESULT __stdcall wSetCooperativeLevel(
	LPDIRECTSOUND lpds, HWND hwnd, uint32_t dwLevel);
HRESULT __stdcall wPlay(LPDIRECTSOUNDBUFFER lpdsb, uint32_t dwReserved1,
	uint32_t dwReserved2, uint32_t dwFlags);
HRESULT __stdcall wSetRolloffFactor(
	LPDIRECTSOUND3DLISTENER lpdsb, float dist, uint32_t flag);
HRESULT __stdcall wSetDopplerFactor(
	LPDIRECTSOUND3DLISTENER lpdsb, float dist, uint32_t flag);
HRESULT __stdcall wSetDistanceFactor(
	LPDIRECTSOUND3DLISTENER lpdsb, float dist, uint32_t flag);
HRESULT __stdcall wSetOrientation(LPDIRECTSOUND3DLISTENER lpdsb, float fx,
	float fy, float fz, float tx, float ty, float tz, uint32_t flag);
HRESULT __stdcall wSetPosition(
	LPDIRECTSOUND3DBUFFER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetPosition(
	LPDIRECTSOUND3DLISTENER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetVelocity(
	LPDIRECTSOUND3DBUFFER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetVelocity(
	LPDIRECTSOUND3DLISTENER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetMinDistance(
	LPDIRECTSOUND3DBUFFER lpdsb, float x, uint32_t flag);
HRESULT __stdcall wSetMaxDistance(
	LPDIRECTSOUND3DBUFFER lpdsb, float x, uint32_t flag);
HRESULT __stdcall wSetConeAngles(
	LPDIRECTSOUND3DBUFFER ppDSB3D, uint32_t min, uint32_t max, uint32_t flag);
HRESULT __stdcall wSetConeOrientation(
	LPDIRECTSOUND3DBUFFER ppDSB3D, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wCommitDeferredSettings(LPDIRECTSOUND3DLISTENER lpList);
HRESULT __stdcall wQuerySupport(
	LPKSPROPERTYSET lpPS, REFGUID rGUID, uint32_t uLong, puint32_t support);
HRESULT __stdcall wSet(LPKSPROPERTYSET lpPS, REFGUID rguidPropSet,
	uint32_t ulId, PVOID pInstanceData, uint32_t ulInstanceLength,
	PVOID pPropertyData, uint32_t ulDataLength);
HRESULT __stdcall wGetFormat(LPDIRECTSOUNDBUFFER lpdsb, LPWAVEFORMATEX lpwf,
	uint32_t sizeIn, puint32_t sizeOut);
HRESULT __stdcall wSetAllParameters(
	LPDIRECTSOUND3DBUFFER lpdsb, DS3DBUFFER* ds3d, uint32_t flag);
HRESULT __stdcall wSetAllParameters(
	LPDIRECTSOUND3DLISTENER lpdsb, DS3DLISTENER* ds3d, uint32_t flag);
HRESULT __stdcall wDuplicateSoundBuffer(
	LPDIRECTSOUND lpds, LPDIRECTSOUNDBUFFER src, LPDIRECTSOUNDBUFFER* dst);

typedef LONGLONG STREAM_TIME;
typedef struct IMultiMediaStream* PIMULTIMEDIASTREAM;
typedef struct IDirectDrawMediaStream* PIDIRECTDRAWMEDIASTREAM;
typedef struct IAMMultiMediaStream* PIAMMULTIMEDIASTREAM;
typedef struct IMediaStream* PIMEDIASTREAM;
typedef struct IGraphBuilder* PIGRAPHBUILDER;

#if _CONSIDERED_OBSOLETE
// Direct Show
HRESULT __stdcall wSetState(PIMULTIMEDIASTREAM imms, STREAM_STATE NewState);
HRESULT __stdcall wGetState(PIMULTIMEDIASTREAM imms, STREAM_STATE* NewState);
HRESULT __stdcall wSeek(PIMULTIMEDIASTREAM imms, STREAM_TIME SeekTime);
HRESULT __stdcall wGetTime(PIMULTIMEDIASTREAM imms, STREAM_TIME* SeekTime);
HRESULT __stdcall wGetDuration(PIMULTIMEDIASTREAM imms, STREAM_TIME* theTime);
HRESULT __stdcall wGetMediaStream(
	PIMULTIMEDIASTREAM imms, REFMSPID idPurpose, PIMEDIASTREAM* pMediaStream);

HRESULT __stdcall wInitialize(PIAMMULTIMEDIASTREAM iammms,
	STREAM_TYPE StreamType, uint32_t dwFlags, PIGRAPHBUILDER pFilterGraph);
HRESULT __stdcall wAddMediaStream(PIAMMULTIMEDIASTREAM iammms,
	IUnknown* pStreamObject, const MSPID* pPurposeID, uint32_t dwFlags,
	PIMEDIASTREAM* ppNewStream);
HRESULT __stdcall wOpenFile(
	PIAMMULTIMEDIASTREAM iammms, LPCWSTR pszFileName, uint32_t dwFlags);

HRESULT __stdcall wCreateSample(PIDIRECTDRAWMEDIASTREAM iddms,
	LPDIRECTDRAWSURFACE pSurface, const RECT* pRect, uint32_t dwFlags,
	IDirectDrawStreamSample** ppSample);
HRESULT __stdcall wGetSurface(IDirectDrawStreamSample* iddss,
	IDirectDrawSurface** ppDirectDrawSurface, RECT* pRect);

HRESULT __stdcall wSetFormat(
	PIDIRECTDRAWMEDIASTREAM iddss, DDSURFACEDESC* ddsd);
HRESULT __stdcall wGetFormat(PIDIRECTDRAWMEDIASTREAM iddss,
	DDSURFACEDESC* ddsd_current, DDSURFACEDESC* ddsd_desired, puint32_t flags);
HRESULT __stdcall wSetSampleTimes(
	IStreamSample* iddss, STREAM_TIME* st1, STREAM_TIME* st2);
HRESULT __stdcall wUpdate(IStreamSample* iss, uint32_t flags, HANDLE event,
	PAPCFUNC papcfunc, uint32_t apc);
HRESULT __stdcall wGetFilterGraph(
	PIAMMULTIMEDIASTREAM iamms, PIGRAPHBUILDER* igb);
HRESULT __stdcall wSetPositions(IMediaSeeking* ims, STREAM_TIME* st,
	uint32_t stflags, STREAM_TIME* end, uint32_t eflags);
HRESULT __stdcall wGetStopPosition(IMediaSeeking* ims, STREAM_TIME* st);
HRESULT __stdcall wGetSampleTimes(IDirectDrawStreamSample* iddss,
	STREAM_TIME* st, STREAM_TIME* end, STREAM_TIME* cur);

HRESULT __stdcall wPut_CurrentPosition(IMediaPosition* iba, REFTIME value);
HRESULT __stdcall wGet_CurrentPosition(IMediaPosition* iba, REFTIME* value);
HRESULT __stdcall wCanSeekForward(IMediaPosition* iba, int32_t* canDo);
HRESULT __stdcall wGet_Duration(IMediaPosition* iba, REFTIME* value);
HRESULT __stdcall wStop(IMediaControl* iba);
HRESULT __stdcall wPause(IMediaControl* iba);
HRESULT __stdcall wRun(IMediaControl* iba);
HRESULT __stdcall wPut_Volume(IBasicAudio* iba, int32_t value);
HRESULT __stdcall wPut_Balance(IBasicAudio* iba, int32_t value);
HRESULT __stdcall wGetEndOfStreamEventHandle(
	PIMULTIMEDIASTREAM imms, HANDLE* ph);
HRESULT __stdcall wGetInformation(
	PIMULTIMEDIASTREAM imms, puint32_t flags, STREAM_TYPE* theType);
#endif

//
// Direct Input
//
HRESULT __stdcall wDirectInputCreateEx(HINSTANCE hinst, uint32_t dwVersion,
	REFIID riidltf, PVOID* ppvOut, LPUNKNOWN punkOuter);
HRESULT __stdcall wCreateDeviceEx(LPDIRECTINPUT7 lpdi, REFGUID rguid,
	REFIID riid, PVOID* lplpDirectInputDevice, LPUNKNOWN pUnkOuter);
HRESULT __stdcall wSetDataFormat(
	LPDIRECTINPUTDEVICE7 dev, LPCDIDATAFORMAT lpdf);
HRESULT __stdcall wSetCooperativeLevel(
	LPDIRECTINPUTDEVICE7 dev, HWND hwnd, uint32_t dwFlags);
HRESULT __stdcall wUnacquire(LPDIRECTINPUTDEVICE7 dev);
HRESULT __stdcall wAcquire(LPDIRECTINPUTDEVICE7 dev);
HRESULT __stdcall wGetDeviceState(
	LPDIRECTINPUTDEVICE7 dev, uint32_t cbData, PVOID lpvData);
HRESULT __stdcall wEnumDevices(LPDIRECTINPUT7 di, uint32_t dwDevType,
	LPDIENUMDEVICESCALLBACK lpCallback, PVOID pvRef, uint32_t dwFlags);
HRESULT __stdcall wSetProperty(
	LPDIRECTINPUTDEVICE7 dev, REFGUID rguidProp, LPCDIPROPHEADER pdiph);
HRESULT __stdcall wGetDeviceInfo(
	LPDIRECTINPUTDEVICE7 dev, LPDIDEVICEINSTANCE pdidi);
HRESULT __stdcall wGetCapabilities(
	LPDIRECTINPUTDEVICE7 dev, LPDIDEVCAPS lpDIDevCaps);
HRESULT __stdcall wGetDeviceData(LPDIRECTINPUTDEVICE7 dev,
	uint32_t cbObjectData, LPDIDEVICEOBJECTDATA rgdod, puint32_t pdwInOut,
	uint32_t dwFlags);
HRESULT __stdcall wPoll(LPDIRECTINPUTDEVICE7 lpdid2);
HRESULT __stdcall wCreateEffect(LPDIRECTINPUTDEVICE7 lpdid2, REFGUID rguid,
	LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter);
HRESULT __stdcall wStart(
	LPDIRECTINPUTEFFECT lpeff, uint32_t iterations, uint32_t flags);
HRESULT __stdcall wStop(LPDIRECTINPUTEFFECT lpeff);
HRESULT __stdcall wGetEffectStatus(LPDIRECTINPUTEFFECT lpeff, puint32_t status);
HRESULT __stdcall wDownload(LPDIRECTINPUTEFFECT lpeff);
HRESULT __stdcall wSetParameters(
	LPDIRECTINPUTEFFECT lpeff, LPCDIEFFECT lpdi, uint32_t flags);

#if _CONSIDERED_OBSOLETE
// DirectPlay
HRESULT __stdcall wAddPlayerToGroup(
	IDirectPlay4A* dplay4, DPID idGroup, DPID idPlayer);
HRESULT __stdcall wCancelMessage(
	IDirectPlay4A* dplay4, uint32_t dwMsgID, uint32_t dwFlags);
HRESULT __stdcall wCancelPriority(IDirectPlay4A* dplay4, uint32_t dwMinPriority,
	uint32_t dwMaxPriority, uint32_t dwFlags);
HRESULT __stdcall wClose(IDirectPlay4A* dplay4);
HRESULT __stdcall wCreateGroup(IDirectPlay4A* dplay4, LPDPID lpidGroup,
	LPDPNAME lpGroupName, PVOID lpData, uint32_t dwDataSize, uint32_t dwFlags);
HRESULT __stdcall wCreateGroupInGroup(IDirectPlay4A* dplay4, DPID idParentGroup,
	LPDPID lpidGroup, LPDPNAME lpGroupName, PVOID lpData, uint32_t dwDataSize,
	uint32_t dwFlags);
HRESULT __stdcall wCreatePlayer(IDirectPlay4A* dplay4, LPDPID lpidPlayer,
	LPDPNAME lpPlayerName, HANDLE hEvent, PVOID lpData, uint32_t dwDataSize,
	uint32_t dwFlags);
HRESULT __stdcall wDeleteGroupFromGroup(
	IDirectPlay4A* dplay4, DPID idParentGroup, DPID idGroup);
HRESULT __stdcall wDeletePlayerFromGroup(
	IDirectPlay4A* dplay4, DPID idGroup, DPID idPlayer);
HRESULT __stdcall wDestroyGroup(IDirectPlay4A* dplay4, DPID idGroup);
HRESULT __stdcall wDestroyPlayer(IDirectPlay4A* dplay4, DPID idPlayer);
HRESULT __stdcall wEnumConnections(IDirectPlay4A* dplay4,
	LPCGUID lpguidApplication, LPDPENUMCONNECTIONSCALLBACK lpEnumCallback,
	PVOID lpContext, uint32_t dwFlags);
HRESULT __stdcall wEnumGroupPlayers(IDirectPlay4A* dplay4, DPID idGroup,
	LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2,
	PVOID lpContext, uint32_t dwFlags);
HRESULT __stdcall wEnumGroups(IDirectPlay4A* dplay4, LPGUID lpguidInstance,
	LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID lpContext,
	uint32_t dwFlags);
HRESULT __stdcall wEnumGroupsInGroup(IDirectPlay4A* dplay4, DPID idGroup,
	LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumCallback,
	PVOID lpContext, uint32_t dwFlags);
HRESULT __stdcall wEnumPlayers(IDirectPlay4A* dplay4, LPGUID lpguidInstance,
	LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID lpContext,
	uint32_t dwFlags);
HRESULT __stdcall wEnumSessions(IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpsd,
	uint32_t dwTimeout, LPDPENUMSESSIONSCALLBACK2 lpEnumSessionsCallback2,
	PVOID lpContext, uint32_t dwFlags);
HRESULT __stdcall wGetCaps(
	IDirectPlay4A* dplay4, LPDPCAPS lpDPCaps, uint32_t dwFlags);
HRESULT __stdcall wGetGroupConnectionSettings(IDirectPlay4A* dplay4,
	uint32_t dwFlags, DPID idGroup, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wGetGroupData(IDirectPlay4A* dplay4, DPID idGroup,
	PVOID lpData, puint32_t lpdwDataSize, uint32_t dwFlags);
HRESULT __stdcall wGetGroupFlags(
	IDirectPlay4A* dplay4, DPID idGroup, puint32_t lpdwFlags);
HRESULT __stdcall wGetGroupName(
	IDirectPlay4A* dplay4, DPID idGroup, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wGetGroupOwner(
	IDirectPlay4A* dplay4, DPID idGroup, LPDPID lpidOwner);
HRESULT __stdcall wGetGroupParent(
	IDirectPlay4A* dplay4, DPID idGroup, LPDPID lpidParent);
HRESULT __stdcall wGetMessageCount(
	IDirectPlay4A* dplay4, DPID idPlayer, puint32_t lpdwCount);
HRESULT __stdcall wGetMessageQueue(IDirectPlay4A* dplay4, DPID idFrom,
	DPID idTo, uint32_t dwFlags, puint32_t lpdwNumMsgs, puint32_t lpdwNumBytes);
HRESULT __stdcall wGetPlayerAccount(IDirectPlay4A* dplay4, DPID idPlayer,
	uint32_t dwFlags, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wGetPlayerAddress(
	IDirectPlay4A* dplay4, DPID idPlayer, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wGetPlayerCaps(IDirectPlay4A* dplay4, DPID idPlayer,
	LPDPCAPS lpPlayerCaps, uint32_t dwFlags);
HRESULT __stdcall wGetPlayerData(IDirectPlay4A* dplay4, DPID idPlayer,
	PVOID lpData, puint32_t lpdwDataSize, uint32_t dwFlags);
HRESULT __stdcall wGetPlayerFlags(
	IDirectPlay4A* dplay4, DPID idPlayer, puint32_t lpdwFlags);
HRESULT __stdcall wGetPlayerName(
	IDirectPlay4A* dplay4, DPID idPlayer, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wGetSessionDesc(
	IDirectPlay4A* dplay4, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wInitialize(IDirectPlay4A* dplay4, LPGUID lpGUID);
HRESULT __stdcall wInitializeConnection(
	IDirectPlay4A* dplay4, PVOID lpConnection, uint32_t dwFlags);
HRESULT __stdcall wOpen(
	IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpsd, uint32_t dwFlags);
HRESULT __stdcall wReceive(IDirectPlay4A* dplay4, LPDPID lpidFrom,
	LPDPID lpidTo, uint32_t dwFlags, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wSecureOpen(IDirectPlay4A* dplay4, LPCDPSESSIONDESC2 lpsd,
	uint32_t dwFlags, LPCDPSECURITYDESC lpSecurity,
	LPCDPCREDENTIALS lpCredentials);
HRESULT __stdcall wSendChatMessage(IDirectPlay4A* dplay4, DPID idFrom,
	DPID idTo, uint32_t dwFlags, LPDPCHAT lpChatMessage);
HRESULT __stdcall wSend(IDirectPlay4A* dplay4, DPID idFrom, DPID idTo,
	uint32_t dwFlags, PVOID lpData, uint32_t dwDataSize);
HRESULT __stdcall wSendEx(IDirectPlay4A* dplay4, DPID idFrom, DPID idTo,
	uint32_t dwFlags, PVOID lpData, uint32_t dwDataSize, uint32_t dwPriority,
	uint32_t dwTimeout, PVOID lpContext, puint32_t lpdwMsgID);
HRESULT __stdcall wSetGroupConnectionSettings(IDirectPlay4A* dplay4,
	uint32_t dwFlags, DPID idGroup, LPDPLCONNECTION lpConnection);
HRESULT __stdcall wSetGroupData(IDirectPlay4A* dplay4, DPID idGroup,
	PVOID lpData, uint32_t dwDataSize, uint32_t dwFlags);
HRESULT __stdcall wSetGroupName(IDirectPlay4A* dplay4, DPID idGroup,
	LPDPNAME lpGroupName, uint32_t dwFlags);
HRESULT __stdcall wSetGroupOwner(
	IDirectPlay4A* dplay4, DPID idGroup, DPID idOwner);
HRESULT __stdcall wSetPlayerData(IDirectPlay4A* dplay4, DPID idPlayer,
	PVOID lpData, uint32_t dwDataSize, uint32_t dwFlags);
HRESULT __stdcall wSetPlayerName(IDirectPlay4A* dplay4, DPID idPlayer,
	LPDPNAME lpPlayerName, uint32_t dwFlags);
HRESULT __stdcall wSetSessionDesc(
	IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpSessDesc, uint32_t dwFlags);
HRESULT __stdcall wStartSession(
	IDirectPlay4A* dplay4, uint32_t dwFlags, DPID idGroup);

// DirectPlay Lobby functions

HRESULT __stdcall wCreateAddress(IDirectPlayLobby3A* dplobby3, REFGUID guidSP,
	REFGUID guidDataType, LPCVOID lpData, uint32_t dwDataSize, PVOID lpAddress,
	puint32_t lpdwAddressSize);
HRESULT __stdcall wCreateCompoundAddress(IDirectPlayLobby3A* dplobby3,
	LPDPCOMPOUNDADDRESSELEMENT lpElements, uint32_t dwElementCount,
	PVOID lpAddress, puint32_t lpdwAddressSize);
HRESULT __stdcall wEnumAddress(IDirectPlayLobby3A* dplobby3,
	LPDPENUMADDRESSCALLBACK lpEnumAddressCallback, LPCVOID lpAddress,
	uint32_t dwAddressSize, PVOID lpContext);
HRESULT __stdcall wGetConnectionSettings(IDirectPlayLobby3A* dplobby3,
	uint32_t dwAppID, PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wReceiveLobbyMessage(IDirectPlayLobby3A* dplobby3,
	uint32_t dwFlags, uint32_t dwAppID, puint32_t lpdwMessageFlags,
	PVOID lpData, puint32_t lpdwDataSize);
HRESULT __stdcall wRegisterApplication(IDirectPlayLobby3A* dplobby3,
	uint32_t dwFlags, LPDPAPPLICATIONDESC lpAppDesc);
HRESULT __stdcall wRunApplication(IDirectPlayLobby3A* dplobby3,
	uint32_t dwFlags, puint32_t lpdwAppID, LPDPLCONNECTION lpConn,
	HANDLE hReceiveEvent);
HRESULT __stdcall wSendLobbyMessage(IDirectPlayLobby3A* dplobby3,
	uint32_t dwFlags, uint32_t dwAppID, PVOID lpData, uint32_t dwDataSize);
HRESULT __stdcall wSetConnectionSettings(IDirectPlayLobby3A* dplobby3,
	uint32_t dwFlags, uint32_t dwAppID, LPDPLCONNECTION lpConn);
HRESULT __stdcall wSetLobbyMessageEvent(IDirectPlayLobby3A* dplobby3,
	uint32_t dwFlags, uint32_t dwAppID, HANDLE hReceiveEvent);
HRESULT __stdcall wUnregisterApplication(
	IDirectPlayLobby3A* dplobby3, uint32_t dwFlags, REFGUID guidApplication);
HRESULT __stdcall wWaitForConnectionSettings(
	IDirectPlayLobby3A* dplobby3, uint32_t dwFlags);
HRESULT __stdcall wConnectEx(IDirectPlayLobby3A* dplobby3, uint32_t dwFlags,
	REFIID riid, PVOID* lplpDP, IUnknown* pUnk);
#endif
