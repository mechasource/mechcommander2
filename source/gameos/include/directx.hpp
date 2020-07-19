//===========================================================================//
// File:	 DirectX.hpp													 //
// Contents: Interface to DirectSetup										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

// clang-format off

extern uint8_t AllowFail;
extern volatile uint16_t FPUControl; // Current FPU control word
extern uint16_t FPUDefault;			 // Default FPU control word
extern PSTR __stdcall ErrorNumberToMessage(HRESULT hr);

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
HRESULT __stdcall wCoCreateInstance(REFCLSID rclsid, LPUNKNOWN punkouter, uint32_t clscontext, REFIID riid, PVOID* ppv);
HRESULT __stdcall wQueryInterface(IUnknown* iun, REFIID riid, PVOID* obp);
ULONG __stdcall wRelease(IUnknown* punk);
ULONG __stdcall wAddRef(IUnknown* punk);

//
// Direct Setup
//
void wDirectXSetupGetVersion(uint32_t* pversion, uint32_t* prevision);

// IDirect3DDevice7
//#ifdef LAB_ONLY

HRESULT __stdcall wDrawPrimitive(LPDIRECT3DDEVICE7 d3ddevice7, D3DPRIMITIVETYPE dptPrimitiveType,uint32_t dvtVertexType, PVOID lpvVertices, uint32_t dwVertexCount, uint32_t flags);
HRESULT __stdcall wDrawIndexedPrimitive(LPDIRECT3DDEVICE7 d3ddevice7,D3DPRIMITIVETYPE d3dptPrimitiveType, uint32_t dwVertexTypeDesc, PVOID pvVertices,uint32_t dwVertexCount, uint16_t* pwIndices, uint32_t dwIndexCount, uint32_t flags);

//#else
//#define wDrawPrimitive(d3ddevice7,dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,flags) d3ddevice7->DrawPrimitive(dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,flags)
//#define wDrawIndexedPrimitive(d3ddevice7,d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,flags) d3ddevice7->DrawIndexedPrimitive(d3dptPrimitiveType,dwVertexTypeDesc,lpvVertices,dwVertexCount,lpwIndices,dwIndexCount,flags)
//#endif
HRESULT __stdcall wDrawPrimitiveVB(LPDIRECT3DDEVICE7 d3ddevice7,D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,	uint32_t dwStartVertex, uint32_t dwNumVertices, uint32_t flags);
HRESULT __stdcall wDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE7 d3ddevice7,D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,uint32_t dwStartVertex, uint32_t dwNumVertices, uint16_t* lpwIndices, uint32_t dwIndexCount,	uint32_t flags);
HRESULT __stdcall wSetMaterial(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DMATERIAL7 lpMaterial);
HRESULT __stdcall wSetLight(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t dwLightIndex, LPD3DLIGHT7 lpLight);
HRESULT __stdcall wLightEnable(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t dwLightIndex, uint8_t Enable);
HRESULT __stdcall wMultiplyTransform(LPDIRECT3DDEVICE7 d3ddevice7,D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX pd3dmatrix);
HRESULT __stdcall wSetTransform(LPDIRECT3DDEVICE7 d3ddevice7,D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX pd3dmatrix);
HRESULT __stdcall wBeginScene(LPDIRECT3DDEVICE7 d3ddevice7);
HRESULT __stdcall wEndScene(LPDIRECT3DDEVICE7 d3ddevice7);
HRESULT __stdcall wGetCaps(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DDEVICEDESC7 pd3dhwdevdesc);
HRESULT __stdcall wSetRenderState(LPDIRECT3DDEVICE7 d3ddevice7, D3DRENDERSTATETYPE renderstatetype, uint32_t renderstate);
HRESULT __stdcall wEnumTextureFormats(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DENUMPIXELFORMATSCALLBACK pd3denumpixelproc, PVOID parg);
HRESULT __stdcall wSetTexture(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t stage, LPDIRECTDRAWSURFACE7 ptexture);
HRESULT __stdcall wSetTextureStageState(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t stage,D3DTEXTURESTAGESTATETYPE state, uint32_t value);
HRESULT __stdcall wValidateDevice(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t* pExtraPasses);
HRESULT __stdcall wLoad(LPDIRECT3DDEVICE7 d3ddevice7, LPDIRECTDRAWSURFACE7 lpDestTex,PPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, PRECT lprcSrcRect, uint32_t flags);
HRESULT __stdcall wSetViewport(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DVIEWPORT7 pviewport);
HRESULT __stdcall wClear(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t count, LPD3DRECT pd3drect, uint32_t flags, uint32_t colour, D3DVALUE dvZ, uint32_t stencil);

// IDirect3D7
HRESULT __stdcall wCreateDevice(LPDIRECT3D7 d3d7, REFCLSID rclsid, LPDIRECTDRAWSURFACE7 pdds,LPDIRECT3DDEVICE7* ppd3ddevice7);
HRESULT __stdcall wEnumZBufferFormats(LPDIRECT3D7 d3d7, REFCLSID riidDevice,LPD3DENUMPIXELFORMATSCALLBACK penumcallback, PVOID pcontext);
HRESULT __stdcall wEnumDevices(LPDIRECT3D7 d3d7, LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, PVOID puserarg);
HRESULT __stdcall wCreateVertexBuffer(LPDIRECT3D7 d3d7, LPD3DVERTEXBUFFERDESC lpVBDesc,LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, uint32_t flags);

// IDirect3DVertexBuffer7
HRESULT __stdcall wLock(LPDIRECT3DVERTEXBUFFER7 d3dvb7, uint32_t flags, PVOID* ppdata, uint32_t* psize);
HRESULT __stdcall wUnlock(LPDIRECT3DVERTEXBUFFER7 d3dvb7);
HRESULT __stdcall wOptimize(LPDIRECT3DVERTEXBUFFER7 d3dvb7, LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t flags);
HRESULT __stdcall wProcessVertices(LPDIRECT3DVERTEXBUFFER7 d3dvb7, uint32_t dwVertexOp,uint32_t dwdestIndex, uint32_t count, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer,uint32_t dwsrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t flags);

// Direct Draw
HRESULT __stdcall wDirectDrawCreateEx(LPGUID pguid, PVOID* ppdd, REFIID iid, IUnknown* punkouter);
HRESULT __stdcall wDirectDrawEnumerate(LPDDENUMCALLBACK lpCallback, LPDDENUMCALLBACKEX lpCallbackEx, PVOID pcontext);

HRESULT __stdcall wSetHWnd(LPDIRECTDRAWCLIPPER lpddc, uint32_t flags, HWND hwnd);

HRESULT __stdcall wIsLost(LPDIRECTDRAWSURFACE7 pdds7);
HRESULT __stdcall wGetDC(LPDIRECTDRAWSURFACE7 pdds7, HDC* phdc);
HRESULT __stdcall wReleaseDC(LPDIRECTDRAWSURFACE7 pdds7, HDC hdc);
HRESULT __stdcall wSetColorKey(LPDIRECTDRAWSURFACE7 lpdd3, uint32_t flags, LPDDCOLORKEY lpDDColorKey);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 pdd7, PRECT pdestrect,LPDDSURFACEDESC2 pddsurfacedesc, uint32_t flags, HANDLE hevent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE7 pdds7, PRECT prect);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE lpdds, PRECT pdestrect,LPDDSURFACEDESC pddsurfacedesc, uint32_t flags, HANDLE hevent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE lpdds, PRECT prect);
HRESULT __stdcall wSetPalette(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWPALETTE lpDDPalette);
HRESULT __stdcall wRestore(LPDIRECTDRAWSURFACE7 pdd7);
HRESULT __stdcall wBlt(LPDIRECTDRAWSURFACE7 pdd7, PRECT pdestrect,LPDIRECTDRAWSURFACE7 pddsrcSurface, PRECT psrcrect, uint32_t flags, LPDDBLTFX lpDDBltFx);
HRESULT __stdcall wBltFast(LPDIRECTDRAWSURFACE7 pdd7, uint32_t dwX, uint32_t dwY,LPDIRECTDRAWSURFACE7 pddsrcSurface, PRECT psrcrect, uint32_t dwTrans);
HRESULT __stdcall wFlip(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pddsurfacetargetoverride, uint32_t flags);
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE7 pdd7, LPDDSURFACEDESC2 pddsurfacedesc);
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE lpdds, LPDDSURFACEDESC pddsurfacedesc);
HRESULT __stdcall wGetAttachedSurface(LPDIRECTDRAWSURFACE7 pdds7, LPDDSCAPS2 pddscaps2,LPDIRECTDRAWSURFACE7* ppddattachedsurface);
HRESULT __stdcall wGetPixelFormat(LPDIRECTDRAWSURFACE7 pdds7, LPDDPIXELFORMAT lpDDPixelFormat);
HRESULT __stdcall wAddAttachedSurface(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pddsattachedsurface);
HRESULT __stdcall wDeleteAttachedSurface(LPDIRECTDRAWSURFACE7 pdds7, uint32_t flags, LPDIRECTDRAWSURFACE7 pddsattachedsurface);
HRESULT __stdcall wSetClipper(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWCLIPPER pddclipper);

HRESULT __stdcall wGetDeviceIdentifier(LPDIRECTDRAW7 pdd7, LPDDDEVICEIDENTIFIER2 lpdddi2, uint32_t flags);
HRESULT __stdcall wGetCaps(LPDIRECTDRAW7 pdd7, LPDDCAPS pdddrivercaps, LPDDCAPS pddhelcaps);
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 pdd7, uint32_t flags, LPPALETTEENTRY lpColorTable,LPDIRECTDRAWPALETTE* lplpDDPalette, IUnknown* punkouter);
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pddsurfacedesc,LPDIRECTDRAWSURFACE7* ppddsurface7, IUnknown* punkouter);
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTDRAW7 pdd7, HWND hwnd, uint32_t flags);
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 pdd7, uint32_t flags,LPDDSURFACEDESC2 pddsurfacedesc, PVOID pcontext, LPDDENUMMODESCALLBACK2 lpEnumModesCallback);
HRESULT __stdcall wRestoreDisplayMode(LPDIRECTDRAW7 pdd7);
HRESULT __stdcall wGetDisplayMode(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pddsurfacedesc);
HRESULT __stdcall wCreateClipper(LPDIRECTDRAW7 pdd7, uint32_t flags, LPDIRECTDRAWCLIPPER* ppddclipper, IUnknown* punkouter);
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 pdd7, uint32_t width, uint32_t height,uint32_t bpp, uint32_t refreshrate, uint32_t flags);
HRESULT __stdcall wGetAvailableVidMem(LPDIRECTDRAW7 pdd7, LPDDSCAPS2 pddscaps2, PULONG lpdwTotal, PULONG lpdwFree);
HRESULT __stdcall wGetFourCCCodes(LPDIRECTDRAW7 pdd7, PULONG lpNumCodes, PULONG lpCodes);

// Direct Sound
HRESULT __stdcall wDirectSoundEnumerate(LPDSENUMCALLBACK cb, PVOID vp);
HRESULT __stdcall wDirectSoundCreate(LPGUID pguid, LPDIRECTSOUND* ppDS, IUnknown* punkouter);
HRESULT __stdcall wGetCaps(LPDIRECTSOUND ppDS, DSCAPS* dsc);
HRESULT __stdcall wSetSpeakerConfig(LPDIRECTSOUND ppDS, uint32_t dsflags);
HRESULT __stdcall wGetSpeakerConfig(LPDIRECTSOUND ppDS, uint32_t* dsflags);
HRESULT __stdcall wLock(LPDIRECTSOUNDBUFFER lpdsb, uint32_t dwWriteCursor, uint32_t dwWriteBytes,PVOID* lplpvAudioPtr1, uint32_t* lpdwAudioBytes1, PVOID* lplpvAudioPtr2,uint32_t* lpdwAudioBytes2, uint32_t flags);
HRESULT __stdcall wUnlock(LPDIRECTSOUNDBUFFER lpdsb, PVOID lplpvAudioPtr1, uint32_t lpdwAudioBytes1,PVOID lplpvAudioPtr2, uint32_t lpdwAudioBytes2);
HRESULT __stdcall wGetVolume(LPDIRECTSOUNDBUFFER lpdsb, int32_t* vol);
HRESULT __stdcall wGetFrequency(LPDIRECTSOUNDBUFFER lpdsb, uint32_t* freq);
HRESULT __stdcall wGetPan(LPDIRECTSOUNDBUFFER lpdsb, int32_t* pan);
HRESULT __stdcall wSetVolume(LPDIRECTSOUNDBUFFER lpdsb, int32_t vol);
HRESULT __stdcall wSetFrequency(LPDIRECTSOUNDBUFFER lpdsb, uint32_t freq);
HRESULT __stdcall wSetPan(LPDIRECTSOUNDBUFFER lpdsb, int32_t pan);
HRESULT __stdcall wPlay(LPDIRECTSOUNDBUFFER lpdsb, uint32_t res1, uint32_t res2, uint32_t flags);
HRESULT __stdcall wStop(LPDIRECTSOUNDBUFFER lpdsb);
HRESULT __stdcall wGetStatus(LPDIRECTSOUNDBUFFER lpdsb, uint32_t* status);
HRESULT __stdcall wSetCurrentPosition(LPDIRECTSOUNDBUFFER lpdsb, uint32_t pos);
HRESULT __stdcall wGetCurrentPosition(LPDIRECTSOUNDBUFFER lpdsb, uint32_t* lpdwCurrentPlayCursor, uint32_t* lpdwCurrentWriteCursor);
HRESULT __stdcall wCreateSoundBuffer(LPDIRECTSOUND lpds, LPCDSBUFFERDESC lpcDSBufferDesc,LPLPDIRECTSOUNDBUFFER lplpDirectSoundBuffer, IUnknown* punkouter);
HRESULT __stdcall wSetFormat(LPDIRECTSOUNDBUFFER lpdsb, LPCWAVEFORMATEX lpcfxFormat);
HRESULT __stdcall wGetCaps(LPDIRECTSOUNDBUFFER ppDSB, DSBCAPS* dsc);
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTSOUND lpds, HWND hwnd, uint32_t dwLevel);
HRESULT __stdcall wPlay(LPDIRECTSOUNDBUFFER lpdsb, uint32_t dwReserved1, uint32_t dwReserved2, uint32_t flags);
HRESULT __stdcall wSetRolloffFactor(LPDIRECTSOUND3DLISTENER lpdsb, float dist, uint32_t flag);
HRESULT __stdcall wSetDopplerFactor(LPDIRECTSOUND3DLISTENER lpdsb, float dist, uint32_t flag);
HRESULT __stdcall wSetDistanceFactor(LPDIRECTSOUND3DLISTENER lpdsb, float dist, uint32_t flag);
HRESULT __stdcall wSetOrientation(LPDIRECTSOUND3DLISTENER lpdsb, float fx, float fy, float fz,float tx, float ty, float tz, uint32_t flag);
HRESULT __stdcall wSetPosition(LPDIRECTSOUND3DBUFFER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetPosition(LPDIRECTSOUND3DLISTENER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetVelocity(LPDIRECTSOUND3DBUFFER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetVelocity(LPDIRECTSOUND3DLISTENER lpdsb, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wSetMinDistance(LPDIRECTSOUND3DBUFFER lpdsb, float x, uint32_t flag);
HRESULT __stdcall wSetMaxDistance(LPDIRECTSOUND3DBUFFER lpdsb, float x, uint32_t flag);
HRESULT __stdcall wSetConeAngles(LPDIRECTSOUND3DBUFFER ppDSB3D, uint32_t min, uint32_t max, uint32_t flag);
HRESULT __stdcall wSetConeOrientation(LPDIRECTSOUND3DBUFFER ppDSB3D, float x, float y, float z, uint32_t flag);
HRESULT __stdcall wCommitDeferredSettings(LPDIRECTSOUND3DLISTENER lpList);
HRESULT __stdcall wQuerySupport(LPKSPROPERTYSET lpPS, REFGUID rGUID, uint32_t uLong, uint32_t* support);
HRESULT __stdcall wSet(LPKSPROPERTYSET lpPS, REFGUID rguidPropSet, uint32_t ulId,PVOID pInstanceData, uint32_t ulInstanceLength, PVOID pPropertyData, uint32_t ulDataLength);
HRESULT __stdcall wGetFormat(LPDIRECTSOUNDBUFFER lpdsb, LPWAVEFORMATEX lpwf, uint32_t sizeIn, uint32_t* sizeOut);
HRESULT __stdcall wSetAllParameters(LPDIRECTSOUND3DBUFFER lpdsb, DS3DBUFFER* ds3d, uint32_t flag);
HRESULT __stdcall wSetAllParameters(LPDIRECTSOUND3DLISTENER lpdsb, DS3DLISTENER* ds3d, uint32_t flag);
HRESULT __stdcall wDuplicateSoundBuffer(LPDIRECTSOUND lpds, LPDIRECTSOUNDBUFFER src, LPDIRECTSOUNDBUFFER* dst);

typedef LONGLONG STREAM_TIME;
typedef struct IMultiMediaStream* PIMULTIMEDIASTREAM;
typedef struct IDirectDrawMediaStream* PIDIRECTDRAWMEDIASTREAM;
typedef struct IAMMultiMediaStream* PIAMMULTIMEDIASTREAM;
typedef struct IMediaStream* PIMEDIASTREAM;
typedef struct IGraphBuilder* PIGRAPHBUILDER;

#if CONSIDERED_OBSOLETE
// Direct Show
HRESULT __stdcall wSetState(PIMULTIMEDIASTREAM imms, STREAM_STATE NewState);
HRESULT __stdcall wGetState(PIMULTIMEDIASTREAM imms, STREAM_STATE* NewState);
HRESULT __stdcall wSeek(PIMULTIMEDIASTREAM imms, STREAM_TIME SeekTime);
HRESULT __stdcall wGetTime(PIMULTIMEDIASTREAM imms, STREAM_TIME* SeekTime);
HRESULT __stdcall wGetDuration(PIMULTIMEDIASTREAM imms, STREAM_TIME* theTime);
HRESULT __stdcall wGetMediaStream(PIMULTIMEDIASTREAM imms, REFMSPID idPurpose, PIMEDIASTREAM* pMediaStream);

HRESULT __stdcall wInitialize(PIAMMULTIMEDIASTREAM iammms, STREAM_TYPE StreamType, uint32_t flags,PIGRAPHBUILDER pFilterGraph);
HRESULT __stdcall wAddMediaStream(PIAMMULTIMEDIASTREAM iammms, IUnknown* pStreamObject,const MSPID* pPurposeID, uint32_t flags, PIMEDIASTREAM* ppNewStream);
HRESULT __stdcall wOpenFile(PIAMMULTIMEDIASTREAM iammms, LPCWSTR pszFileName, uint32_t flags);

HRESULT __stdcall wCreateSample(PIDIRECTDRAWMEDIASTREAM iddms, LPDIRECTDRAWSURFACE pSurface,const RECT* pRect, uint32_t flags, IDirectDrawStreamSample** ppSample);
HRESULT __stdcall wGetSurface(IDirectDrawStreamSample* iddss, IDirectDrawSurface** ppDirectDrawSurface, RECT* pRect);

HRESULT __stdcall wSetFormat(PIDIRECTDRAWMEDIASTREAM iddss, DDSURFACEDESC* ddsd);
HRESULT __stdcall wGetFormat(PIDIRECTDRAWMEDIASTREAM iddss, DDSURFACEDESC* ddsd_current,DDSURFACEDESC* ddsd_desired, uint32_t* flags);
HRESULT __stdcall wSetSampleTimes(IStreamSample* iddss, STREAM_TIME* st1, STREAM_TIME* st2);
HRESULT __stdcall wUpdate(IStreamSample* iss, uint32_t flags, HANDLE event, PAPCFUNC papcfunc, uint32_t apc);
HRESULT __stdcall wGetFilterGraph(PIAMMULTIMEDIASTREAM iamms, PIGRAPHBUILDER* igb);
HRESULT __stdcall wSetPositions(IMediaSeeking* ims, STREAM_TIME* st, uint32_t stflags, STREAM_TIME* end, uint32_t eflags);
HRESULT __stdcall wGetStopPosition(IMediaSeeking* ims, STREAM_TIME* st);
HRESULT __stdcall wGetSampleTimes(IDirectDrawStreamSample* iddss, STREAM_TIME* st, STREAM_TIME* end, STREAM_TIME* cur);

HRESULT __stdcall wPut_CurrentPosition(IMediaPosition* iba, REFTIME value);
HRESULT __stdcall wGet_CurrentPosition(IMediaPosition* iba, REFTIME* value);
HRESULT __stdcall wCanSeekForward(IMediaPosition* iba, int32_t* canDo);
HRESULT __stdcall wGet_Duration(IMediaPosition* iba, REFTIME* value);
HRESULT __stdcall wStop(IMediaControl* iba);
HRESULT __stdcall wPause(IMediaControl* iba);
HRESULT __stdcall wRun(IMediaControl* iba);
HRESULT __stdcall wPut_Volume(IBasicAudio* iba, int32_t value);
HRESULT __stdcall wPut_Balance(IBasicAudio* iba, int32_t value);
HRESULT __stdcall wGetEndOfStreamEventHandle(PIMULTIMEDIASTREAM imms, HANDLE* ph);
HRESULT __stdcall wGetInformation(PIMULTIMEDIASTREAM imms, uint32_t* flags, STREAM_TYPE* theType);
#endif

//
// Direct Input
//
HRESULT __stdcall wDirectInputCreateEx(HINSTANCE hinst, uint32_t version, REFIID riidltf, PVOID* ppvOut, LPUNKNOWN punkOuter);
HRESULT __stdcall wCreateDeviceEx(LPDIRECTINPUT7 lpdi, REFGUID rguid, REFIID riid,PVOID* lplpDirectInputDevice, LPUNKNOWN punkouter);
HRESULT __stdcall wSetDataFormat(LPDIRECTINPUTDEVICE7 dev, LPCDIDATAFORMAT lpdf);
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTINPUTDEVICE7 dev, HWND hwnd, uint32_t flags);
HRESULT __stdcall wUnacquire(LPDIRECTINPUTDEVICE7 dev);
HRESULT __stdcall wAcquire(LPDIRECTINPUTDEVICE7 dev);
HRESULT __stdcall wGetDeviceState(LPDIRECTINPUTDEVICE7 dev, uint32_t cbData, PVOID lpvData);
HRESULT __stdcall wEnumDevices(LPDIRECTINPUT7 di, uint32_t dwDevType,LPDIENUMDEVICESCALLBACK lpCallback, PVOID pvRef, uint32_t flags);
HRESULT __stdcall wSetProperty(LPDIRECTINPUTDEVICE7 dev, REFGUID rguidProp, LPCDIPROPHEADER pdiph);
HRESULT __stdcall wGetDeviceInfo(LPDIRECTINPUTDEVICE7 dev, LPDIDEVICEINSTANCE pdidi);
HRESULT __stdcall wGetCapabilities(LPDIRECTINPUTDEVICE7 dev, LPDIDEVCAPS lpDIDevCaps);
HRESULT __stdcall wGetDeviceData(LPDIRECTINPUTDEVICE7 dev, uint32_t cbObjectData,LPDIDEVICEOBJECTDATA rgdod, uint32_t* pdwInOut, uint32_t flags);
HRESULT __stdcall wPoll(LPDIRECTINPUTDEVICE7 lpdid2);
HRESULT __stdcall wCreateEffect(LPDIRECTINPUTDEVICE7 lpdid2, REFGUID rguid, LPCDIEFFECT lpeff,LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter);
HRESULT __stdcall wStart(LPDIRECTINPUTEFFECT lpeff, uint32_t iterations, uint32_t flags);
HRESULT __stdcall wStop(LPDIRECTINPUTEFFECT lpeff);
HRESULT __stdcall wGetEffectStatus(LPDIRECTINPUTEFFECT lpeff, uint32_t* status);
HRESULT __stdcall wDownload(LPDIRECTINPUTEFFECT lpeff);
HRESULT __stdcall wSetParameters(LPDIRECTINPUTEFFECT lpeff, LPCDIEFFECT lpdi, uint32_t flags);

#if CONSIDERED_OBSOLETE
// DirectPlay
HRESULT __stdcall wAddPlayerToGroup(IDirectPlay4A* dplay4, DPID idGroup, DPID idPlayer);
HRESULT __stdcall wCancelMessage(IDirectPlay4A* dplay4, uint32_t dwMsgID, uint32_t flags);
HRESULT __stdcall wCancelPriority(IDirectPlay4A* dplay4, uint32_t dwMinPriority, uint32_t dwMaxPriority, uint32_t flags);
HRESULT __stdcall wClose(IDirectPlay4A* dplay4);
HRESULT __stdcall wCreateGroup(IDirectPlay4A* dplay4, LPDPID lpidGroup, LPDPNAME lpGroupName,PVOID pdata, uint32_t dwDataSize, uint32_t flags);
HRESULT __stdcall wCreateGroupInGroup(IDirectPlay4A* dplay4, DPID idParentGroup, LPDPID lpidGroup,LPDPNAME lpGroupName, PVOID pdata, uint32_t dwDataSize, uint32_t flags);
HRESULT __stdcall wCreatePlayer(IDirectPlay4A* dplay4, LPDPID lpidPlayer, LPDPNAME lpPlayerName, HANDLE hevent, PVOID pdata, uint32_t dwDataSize, uint32_t flags);
HRESULT __stdcall wDeleteGroupFromGroup(IDirectPlay4A* dplay4, DPID idParentGroup, DPID idGroup);
HRESULT __stdcall wDeletePlayerFromGroup(IDirectPlay4A* dplay4, DPID idGroup, DPID idPlayer);
HRESULT __stdcall wdestroyGroup(IDirectPlay4A* dplay4, DPID idGroup);
HRESULT __stdcall wdestroyPlayer(IDirectPlay4A* dplay4, DPID idPlayer);
HRESULT __stdcall wEnumConnections(IDirectPlay4A* dplay4, LPCGUID lpguidApplication,LPDPENUMCONNECTIONSCALLBACK penumcallback, PVOID pcontext, uint32_t flags);
HRESULT __stdcall wEnumGroupPlayers(IDirectPlay4A* dplay4, DPID idGroup, LPGUID lpguidInstance,	LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID pcontext, uint32_t flags);
HRESULT __stdcall wEnumGroups(IDirectPlay4A* dplay4, LPGUID lpguidInstance,LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID pcontext, uint32_t flags);
HRESULT __stdcall wEnumGroupsInGroup(IDirectPlay4A* dplay4, DPID idGroup, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 penumcallback, PVOID pcontext, uint32_t flags);
HRESULT __stdcall wEnumPlayers(IDirectPlay4A* dplay4, LPGUID lpguidInstance, LPDPENUMPLAYERSCALLBACK2 lpEnumPlayersCallback2, PVOID pcontext, uint32_t flags);
HRESULT __stdcall wEnumSessions(IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpsd, uint32_t dwTimeout,LPDPENUMSESSIONSCALLBACK2 lpEnumSessionsCallback2, PVOID pcontext, uint32_t flags);
HRESULT __stdcall wGetCaps(IDirectPlay4A* dplay4, LPDPCAPS lpDPCaps, uint32_t flags);
HRESULT __stdcall wGetGroupConnectionSettings(IDirectPlay4A* dplay4, uint32_t flags, DPID idGroup, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wGetGroupData(IDirectPlay4A* dplay4, DPID idGroup, PVOID pdata, uint32_t* lpdwDataSize, uint32_t flags);
HRESULT __stdcall wGetGroupFlags(IDirectPlay4A* dplay4, DPID idGroup, uint32_t* lpdwFlags);
HRESULT __stdcall wGetGroupName(IDirectPlay4A* dplay4, DPID idGroup, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wGetGroupOwner(IDirectPlay4A* dplay4, DPID idGroup, LPDPID lpidOwner);
HRESULT __stdcall wGetGroupParent(IDirectPlay4A* dplay4, DPID idGroup, LPDPID lpidParent);
HRESULT __stdcall wGetMessageCount(IDirectPlay4A* dplay4, DPID idPlayer, uint32_t* lpcount);
HRESULT __stdcall wGetMessageQueue(IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, uint32_t flags,uint32_t* lpdwNumMsgs, uint32_t* lpdwNumBytes);
HRESULT __stdcall wGetPlayerAccount(IDirectPlay4A* dplay4, DPID idPlayer, uint32_t flags, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wGetPlayerAddress(IDirectPlay4A* dplay4, DPID idPlayer, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wGetPlayerCaps(IDirectPlay4A* dplay4, DPID idPlayer, LPDPCAPS lpPlayerCaps, uint32_t flags);
HRESULT __stdcall wGetPlayerData(IDirectPlay4A* dplay4, DPID idPlayer, PVOID pdata, uint32_t* lpdwDataSize, uint32_t flags);
HRESULT __stdcall wGetPlayerFlags(IDirectPlay4A* dplay4, DPID idPlayer, uint32_t* lpdwFlags);
HRESULT __stdcall wGetPlayerName(IDirectPlay4A* dplay4, DPID idPlayer, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wGetSessionDesc(IDirectPlay4A* dplay4, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wInitialize(IDirectPlay4A* dplay4, LPGUID pguid);
HRESULT __stdcall wInitializeConnection(IDirectPlay4A* dplay4, PVOID lpConnection, uint32_t flags);
HRESULT __stdcall wOpen(IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpsd, uint32_t flags);
HRESULT __stdcall wReceive(IDirectPlay4A* dplay4, LPDPID lpidFrom, LPDPID lpidTo, uint32_t flags,PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wSecureOpen(IDirectPlay4A* dplay4, LPCDPSESSIONDESC2 lpsd, uint32_t flags,LPCDPSECURITYDESC lpSecurity, LPCDPCREDENTIALS lpCredentials);
HRESULT __stdcall wSendChatMessage(IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, uint32_t flags, LPDPCHAT lpChatMessage);
HRESULT __stdcall wSend(IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, uint32_t flags, PVOID pdata, uint32_t dwDataSize);
HRESULT __stdcall wSendEx(IDirectPlay4A* dplay4, DPID idFrom, DPID idTo, uint32_t flags, PVOID pdata, uint32_t dwDataSize, uint32_t dwPriority, uint32_t dwTimeout, PVOID pcontext,	uint32_t* lpdwMsgID);
HRESULT __stdcall wSetGroupConnectionSettings(IDirectPlay4A* dplay4, uint32_t flags, DPID idGroup, LPDPLCONNECTION lpConnection);
HRESULT __stdcall wSetGroupData(IDirectPlay4A* dplay4, DPID idGroup, PVOID pdata, uint32_t dwDataSize, uint32_t flags);
HRESULT __stdcall wSetGroupName(IDirectPlay4A* dplay4, DPID idGroup, LPDPNAME lpGroupName, uint32_t flags);
HRESULT __stdcall wSetGroupOwner(IDirectPlay4A* dplay4, DPID idGroup, DPID idOwner);
HRESULT __stdcall wSetPlayerData(IDirectPlay4A* dplay4, DPID idPlayer, PVOID pdata, uint32_t dwDataSize, uint32_t flags);
HRESULT __stdcall wSetPlayerName(IDirectPlay4A* dplay4, DPID idPlayer, LPDPNAME lpPlayerName, uint32_t flags);
HRESULT __stdcall wSetSessionDesc(IDirectPlay4A* dplay4, LPDPSESSIONDESC2 lpSessDesc, uint32_t flags);
HRESULT __stdcall wStartSession(IDirectPlay4A* dplay4, uint32_t flags, DPID idGroup);

// DirectPlay Lobby functions

HRESULT __stdcall wCreateAddress(IDirectPlayLobby3A* dplobby3, REFGUID guidSP, REFGUID guidDataType,
	LPCVOID pdata, uint32_t dwDataSize, PVOID lpAddress, uint32_t* lpdwAddressSize);
HRESULT __stdcall wCreateCompoundAddress(IDirectPlayLobby3A* dplobby3,
	LPDPCOMPOUNDADDRESSELEMENT lpElements, uint32_t dwElementCount, PVOID lpAddress,
	uint32_t* lpdwAddressSize);
HRESULT __stdcall wEnumAddress(IDirectPlayLobby3A* dplobby3,
	LPDPENUMADDRESSCALLBACK lpEnumAddressCallback, LPCVOID lpAddress, uint32_t dwAddressSize,
	PVOID pcontext);
HRESULT __stdcall wGetConnectionSettings(
	IDirectPlayLobby3A* dplobby3, uint32_t dwAppID, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wReceiveLobbyMessage(IDirectPlayLobby3A* dplobby3, uint32_t flags,
	uint32_t dwAppID, uint32_t* lpdwMessageFlags, PVOID pdata, uint32_t* lpdwDataSize);
HRESULT __stdcall wRegisterApplication(
	IDirectPlayLobby3A* dplobby3, uint32_t flags, LPDPAPPLICATIONDESC lpAppDesc);
HRESULT __stdcall wRunApplication(IDirectPlayLobby3A* dplobby3, uint32_t flags,
	uint32_t* lpdwAppID, LPDPLCONNECTION lpConn, HANDLE hReceiveEvent);
HRESULT __stdcall wSendLobbyMessage(IDirectPlayLobby3A* dplobby3, uint32_t flags,
	uint32_t dwAppID, PVOID pdata, uint32_t dwDataSize);
HRESULT __stdcall wSetConnectionSettings(
	IDirectPlayLobby3A* dplobby3, uint32_t flags, uint32_t dwAppID, LPDPLCONNECTION lpConn);
HRESULT __stdcall wSetLobbyMessageEvent(
	IDirectPlayLobby3A* dplobby3, uint32_t flags, uint32_t dwAppID, HANDLE hReceiveEvent);
HRESULT __stdcall wUnregisterApplication(
	IDirectPlayLobby3A* dplobby3, uint32_t flags, REFGUID guidApplication);
HRESULT __stdcall wWaitForConnectionSettings(IDirectPlayLobby3A* dplobby3, uint32_t flags);
HRESULT __stdcall wConnectEx(
	IDirectPlayLobby3A* dplobby3, uint32_t flags, REFIID riid, PVOID* lplpDP, IUnknown* pUnk);
#endif

// clang-format on
