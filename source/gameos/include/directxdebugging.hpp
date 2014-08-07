//===========================================================================//
// File:	 DirectXDebugging.hpp											 //
// Contents: DirectX Debug routines											 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

typedef ULONG	DPID;
typedef DPID*	LPDPID;

typedef struct DPNAME *LPDPNAME;
typedef struct DPSESSIONDESC2 *LPDPSESSIONDESC2;

// DirectX Debugging support
PSTR __stdcall GetSBStatus( ULONG Status );
PSTR __stdcall GetQSupport( ULONG Support );
PSTR __stdcall GetLightInfo( LPD3DLIGHT7 lpLight );
PSTR __stdcall GetMaterialInfo( LPD3DMATERIAL7 lpMaterial );
PSTR __stdcall GetTransformType( D3DTRANSFORMSTATETYPE dtstTransformStateType );
PSTR __stdcall GetApplyFlag( ULONG flag );
PSTR __stdcall GetPropertyItem( REFGUID rguid, LONG Item );
PSTR __stdcall GetPropertySet( REFGUID rguid );
PSTR __stdcall GetSurfaceCaps( LPDDSCAPS2 lpDDSCaps2 );
PSTR __stdcall GetInputDevice( ULONG Size );
PSTR __stdcall GetEnumPlayersInfo( LPGUID lpguidInstance, ULONG Flags );
PSTR __stdcall GetConnectionInfo( PVOID ConnectionInfo );
PSTR __stdcall GetNetworkMessage( NetPacket* pMessage );
PSTR __stdcall GetSendExInfo( DPID idFrom, DPID idTo, ULONG dwFlags, ULONG dwDataSize, ULONG dwPriority, ULONG dwTimeout, PVOID lpContext, PULONG lpdwMsgID );
PSTR __stdcall GetSendInfo( DPID idFrom, DPID idTo, ULONG dwFlags, ULONG dwDataSize );
PSTR __stdcall GetReceivedData( LPDPID lpidFrom, PVOID lpData, PULONG lpdwDataSize );
PSTR __stdcall GetReceiveFlags( ULONG dwFlags, LPDPID lpidFrom, LPDPID lpidTo );
PSTR __stdcall GetPlayerName( LPDPNAME lpPlayerName );
PSTR __stdcall GetCreatePlayerFlags( ULONG dwFlags );
PSTR __stdcall GetEnumSessionFlags( ULONG Timeout, ULONG Flags );
PSTR __stdcall GetDPSessionDesc2( LPDPSESSIONDESC2 desc );
PSTR __stdcall GetDPOpenFLags( ULONG Flags );
PSTR __stdcall GetVertexType( PSTR Buffer, ULONG VertexType );
PSTR __stdcall GetChanProp( ULONG Prop );
PSTR __stdcall GetWaveFormat( PSTR Buffer, LPCWAVEFORMATEX lpcfxFormat );
PSTR __stdcall GetDSoundCoopLevel( ULONG Level );
PSTR __stdcall GetFlipFlags( ULONG dwFlags );
PSTR __stdcall GetStageSet( D3DTEXTURESTAGESTATETYPE dwState, ULONG dwValue );
PSTR __stdcall GetRectangle( PSTR Buffer, LPRECT lp );
PSTR __stdcall GetDIDevice( REFGUID rguid );
PSTR __stdcall GetRenderState( ULONG RenderState, ULONG Value );
PSTR __stdcall GetDrawPrimitiveData( D3DPRIMITIVETYPE dptPrimitiveType, ULONG dvtVertexType, PVOID lpvVertices, ULONG dwVertexCount, ULONG dwFlags );
PSTR __stdcall GetDrawIndexedPrimitiveData( D3DPRIMITIVETYPE d3dptPrimitiveType, ULONG dwVertexTypeDesc, PVOID lpvVertices, ULONG dwVertexCount, LPWORD lpwIndices, ULONG dwIndexCount, ULONG dwFlags );
PSTR __stdcall GetClearArea( ULONG dwCount, LPD3DRECT lpRects, ULONG dwFlags );
PSTR __stdcall GetSurfaceDescription( LPDDSURFACEDESC2 pSD );
PSTR __stdcall GetSurfaceDescriptionOld( LPDDSURFACEDESC pSD );
PSTR __stdcall GetGosPixelFormat( LPDDPIXELFORMAT pf );
PSTR __stdcall GetSetCooperativeLevelFlags( ULONG dwFlags );
PSTR __stdcall GetCreatePaletteFlags( ULONG dwFlags );
PSTR __stdcall GetEnumDisplayModeFlags( ULONG dwFlags );
PSTR __stdcall GetSetColorKeyFlags( ULONG dwFlags );
PSTR __stdcall GetLockFlags( ULONG dwFlags );
PSTR __stdcall Get3DDevice( REFCLSID rclsid );
PSTR __stdcall GetReturnInterface( REFIID riid );
PSTR __stdcall GetANSIfromUNICODE( const LPCWSTR Unicode );
PSTR __stdcall GetBltInformation( LPDIRECTDRAWSURFACE7 lpdd7, LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, ULONG dwFlags, LPDDBLTFX lpDDBltFx );
PSTR __stdcall GetBltFastInformation( LPDIRECTDRAWSURFACE7 lpdd7, ULONG dwX, ULONG dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, ULONG dwTrans );
PSTR __stdcall GetDIEnumFlags(ULONG flags);
PSTR __stdcall GetDICoopFlags(ULONG flags);
PSTR __stdcall GetDIGetDeviceDataFlags(ULONG dwFlags);
PSTR __stdcall GetDIInputFormat( LPCDIDATAFORMAT Format ); 
PSTR __stdcall GetDIDevType( ULONG DevType );
PSTR __stdcall DSGetStreamType(STREAM_TYPE StreamType);
PSTR __stdcall DSGetInitFlags(ULONG dwFlags);
PSTR __stdcall DSGetUpdateFlags(ULONG dwFlags);
PSTR __stdcall GetDSSpeakerConfig(ULONG dsFlags);
PSTR __stdcall GetDSBDFlags(ULONG dsFlags);
PSTR __stdcall GetDShowSeekType(ULONG seekFlag);
PSTR __stdcall GetEffectFlags(ULONG flags);
