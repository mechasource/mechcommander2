//===========================================================================//
// File:	 DirectXDebugging.hpp											 //
// Contents: DirectX Debug routines											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

typedef uint32_t DPID;
typedef DPID* LPDPID;

typedef struct DPNAME* LPDPNAME;
typedef struct DPSESSIONDESC2* LPDPSESSIONDESC2;

// DirectX Debugging support
PSTR __stdcall GetSBStatus(uint32_t Status);
PSTR __stdcall GetQSupport(uint32_t Support);
PSTR __stdcall GetLightInfo(LPD3DLIGHT7 plight);
PSTR __stdcall GetMaterialInfo(LPD3DMATERIAL7 pmaterial);
PSTR __stdcall GetTransformType(D3DTRANSFORMSTATETYPE dtstTransformStateType);
PSTR __stdcall GetApplyFlag(uint32_t flag);
PSTR __stdcall GetPropertyItem(REFGUID rguid, int32_t Item);
PSTR __stdcall GetPropertySet(REFGUID rguid);
PSTR __stdcall GetSurfaceCaps(LPDDSCAPS2 pddscaps2);
PSTR __stdcall GetInputDevice(uint32_t Size);
PSTR __stdcall GetEnumPlayersInfo(LPGUID lpguidInstance, uint32_t flags);
PSTR __stdcall GetConnectionInfo(PVOID ConnectionInfo);
PSTR __stdcall GetNetworkMessage(NetPacket* pMessage);
PSTR __stdcall GetSendExInfo(DPID idFrom, DPID idTo, uint32_t flags, uint32_t dwDataSize,
	uint32_t dwPriority, uint32_t dwTimeout, PVOID pcontext, uint32_t* lpdwMsgID);
PSTR __stdcall GetSendInfo(DPID idFrom, DPID idTo, uint32_t flags, uint32_t dwDataSize);
PSTR __stdcall GetReceivedData(LPDPID lpidFrom, PVOID pdata, uint32_t* lpdwDataSize);
PSTR __stdcall GetReceiveFlags(uint32_t flags, LPDPID lpidFrom, LPDPID lpidTo);
PSTR __stdcall GetPlayerName(LPDPNAME lpPlayerName);
PSTR __stdcall GetCreatePlayerFlags(uint32_t flags);
PSTR __stdcall GetEnumSessionFlags(uint32_t Timeout, uint32_t flags);
PSTR __stdcall GetDPSessionDesc2(LPDPSESSIONDESC2 desc);
PSTR __stdcall GetDPOpenFLags(uint32_t flags);
PSTR __stdcall GetVertexType(PSTR Buffer, uint32_t vertextype);
PSTR __stdcall GetChanProp(uint32_t Prop);
PSTR __stdcall GetWaveFormat(PSTR Buffer, LPCWAVEFORMATEX lpcfxFormat);
PSTR __stdcall GetDSoundCoopLevel(uint32_t Level);
PSTR __stdcall GetFlipFlags(uint32_t flags);
PSTR __stdcall GetStageSet(D3DTEXTURESTAGESTATETYPE state, uint32_t value);
PSTR __stdcall GetRectangle(PSTR Buffer, LPRECT lp);
PSTR __stdcall GetDIDevice(REFGUID rguid);
PSTR __stdcall GetRenderState(uint32_t renderstate, uint32_t value);
PSTR __stdcall GetDrawPrimitiveData(D3DPRIMITIVETYPE dptprimitivetype, uint32_t dvtvertextype,
	PVOID pvvertices, uint32_t vertexcount, uint32_t flags);
PSTR __stdcall GetDrawIndexedPrimitiveData(D3DPRIMITIVETYPE d3dptprimitivetype,
	uint32_t vertextypedesc, PVOID pvvertices, uint32_t vertexcount, LPWORD pwindices,
	uint32_t indexcount, uint32_t flags);
PSTR __stdcall GetClearArea(uint32_t count, LPD3DRECT pd3drect, uint32_t flags);
PSTR __stdcall GetSurfaceDescription(LPDDSURFACEDESC2 pSD);
PSTR __stdcall GetSurfaceDescriptionOld(LPDDSURFACEDESC pSD);
PSTR __stdcall GetGosPixelFormat(LPDDPIXELFORMAT pf);
PSTR __stdcall GetSetCooperativeLevelFlags(uint32_t flags);
PSTR __stdcall GetCreatePaletteFlags(uint32_t flags);
PSTR __stdcall GetEnumDisplayModeFlags(uint32_t flags);
PSTR __stdcall GetSetColorKeyFlags(uint32_t flags);
PSTR __stdcall GetLockFlags(uint32_t flags);
PSTR __stdcall Get3DDevice(REFCLSID rclsid);
PSTR __stdcall GetReturnInterface(REFIID riid);
PSTR __stdcall GetANSIfromUNICODE(const LPCWSTR Unicode);
PSTR __stdcall GetBltInformation(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pdestrect, LPDIRECTDRAWSURFACE7 pddsrcSurface, LPRECT psrcrect, uint32_t flags, LPDDBLTFX lpDDBltFx);
PSTR __stdcall GetBltFastInformation(LPDIRECTDRAWSURFACE7 pdd7, uint32_t dwX, uint32_t dwY, LPDIRECTDRAWSURFACE7 pddsrcSurface, LPRECT psrcrect, uint32_t dwTrans);
PSTR __stdcall GetDIEnumFlags(uint32_t flags);
PSTR __stdcall GetDICoopFlags(uint32_t flags);
PSTR __stdcall GetDIGetDeviceDataFlags(uint32_t flags);
PSTR __stdcall GetDIInputFormat(LPCDIDATAFORMAT Format);
PSTR __stdcall GetDIDevType(uint32_t DevType);
PSTR __stdcall DSGetStreamType(STREAM_TYPE StreamType);
PSTR __stdcall DSGetInitFlags(uint32_t flags);
PSTR __stdcall DSGetUpdateFlags(uint32_t flags);
PSTR __stdcall GetDSSpeakerConfig(uint32_t dsFlags);
PSTR __stdcall GetDSBDFlags(uint32_t dsFlags);
PSTR __stdcall GetDShowSeekType(uint32_t seekFlag);
PSTR __stdcall GetEffectFlags(uint32_t flags);
