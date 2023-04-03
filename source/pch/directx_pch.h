/*******************************************************************************
 directx_pch.h - standard includes

 DirectX samples source code

 2018-10-09 Jerker Bäck, created

*******************************************************************************/

#pragma once

//clang-format off
// *INDENT-OFF*

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#define _CRTDBG_MAP_ALLOC

#define _WIN32_WINNT 0x0601 // minimum Windows 7
#include <winsdkver.h>
#include <sdkddkver.h>
#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif

#ifdef _MSC_VER
#define MSDISABLE_WARNING(x) __pragma(warning(disable \
											  : x))
#define MSDISABLE_WARNING_PUSH(x) \
	__pragma(warning(push));      \
	__pragma(warning(disable      \
					 : x))
#define MSDISABLE_WARNING_POP __pragma(warning(pop))
#define MSSUPPRESS_WARNING(x) __pragma(warning(suppress \
											   : x))
#define MSADD_LIBRARY(x) __pragma(comment(lib, x))
#else
#define MSDISABLE_WARNING_PUSH
#define MSDISABLE_WARNING_POP
#define MSSUPPRESS_WARNING(x)
#define MSADD_LIBRARY
#endif

#define CONSIDERED_OBSOLETE 0
#define CONSIDERED_UNSUPPORTED 0
#define CONSIDERED_DISABLED 0
#define CONSIDERED_UNUSED 0

// useless warnings
MSDISABLE_WARNING(4571 4710)

// temporary disable warnings when compiling with -Wall
MSDISABLE_WARNING_PUSH(4191 4350 4355 4365 4774 4619 4623 4626 4946 5026 5027 5039 5040 5045 4514 4710 4711 4625 4820 26439 26495)
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cwctype>
#include <cmath>
#include <cassert>
#include <climits>
#include <limits>
#include <locale>
#include <memory>
#include <string>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <stack>
#include <array>
#include <set>
#include <exception>
#include <mutex>

#include <malloc.h>
#include <intrin.h>
#include <crtdbg.h>
MSDISABLE_WARNING_POP
namespace stdfs = std::filesystem;

#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_ALL_USER_WARNINGS
#define _ATL_ALL_WARNINGS

MSDISABLE_WARNING_PUSH(4062 4191 4265 4355 4365 4571 4619 4625 4626 4668 4820 5026 5027 5039 5045 6340)
#include <atlbase.h>
#include <atlwin.h>
#include <comdef.h>
#include <wrl.h>
#include <wrl/wrappers/corewrappers.h>
namespace mswrl = Microsoft::WRL;
#include <wil/resource.h>
#include <wil/result.h>
#include <wil/com.h>
#include <wil/safecast.h>
MSDISABLE_WARNING_POP

#define _XM_NO_XMVECTOR_OVERLOADS_

MSDISABLE_WARNING_PUSH(4365 4820 4946 5039 6387)
#include <d3d11_1.h>
#include <directxmath.h>
#include <directxpackedvector.h>
#include <directxcollision.h>
#include <wincodec.h>

#include <windows.h>
#include <winbase.h>
#include <strsafe.h>
#include <intsafe.h>
#include <commctrl.h>
#include <psapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>

#include <uiribbon.h>
#include <uiribbonpropertyhelpers.h>
#include <propkey.h>
#include <propvarutil.h>
#include <unknwn.h>
#include <wincodecsdk.h>
#include <wincodec.h>
#include <uianimation.h>
#include <structuredquery.h>
#include <thumbcache.h>
#include <ddeml.h>
#include <uiribbon.h>
#include <uiribbonpropertyhelpers.h>

// web service
#include <webservices.h>
#include <xmllite.h>
#include <winhttp.h>
#include <ncrypt.h>

// DirectX
#include <d2d1.h>
#include <dwrite.h>
MSDISABLE_WARNING_POP

#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4626) // assignment operator was implicitly defined as deleted
#pragma warning(disable : 4820) // bytes padding added
#pragma warning(disable : 5027) // move assignment operator was implicitly defined as deleted

#ifdef _DEBUG
#define NODEFAULT _ASSERT(0)
#else
#define NODEFAULT __assume(0)
#endif

#ifdef _MSC_VER
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

// *INDENT-ON*
// clang-format on

#if CONSIDERED_DISABLED

#include "alignednew.h"
#include "alignednew.h"
#include "alignednew.h"
#include "alignednew.h"
#include "alignednew.h"
#include "audio.h"
#include "audio.h"
#include "audio.h"
#include "audio.h"
#include "bezier.h"
#include "binaryreader.h"
#include "binaryreader.h"
#include "binaryreader.h"
#include "binaryreader.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "commonstates.h"
#include "dds.h"
#include "dds.h"
#include "dds.h"
#include "dds.h"
#include "ddstextureloader.h"
#include "ddstextureloader.h"
#include "ddstextureloader.h"
#include "demandcreate.h"
#include "demandcreate.h"
#include "demandcreate.h"
#include "demandcreate.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "descriptorheap.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "directxhelpers.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectcommon.h"
#include "effectpipelinestatedescription.h"
#include "effectpipelinestatedescription.h"
#include "effectpipelinestatedescription.h"
#include "effectpipelinestatedescription.h"
#include "effectpipelinestatedescription.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "effects.h"
#include "gamepad.h"
#include "geometricprimitive.h"
#include "geometry.h"
#include "geometry.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "graphicsmemory.h"
#include "keyboard.h"
#include "linearallocator.h"
#include "linearallocator.h"
#include "loaderhelpers.h"
#include "loaderhelpers.h"
#include "loaderhelpers.h"
#include "loaderhelpers.h"
#include "model.h"
#include "model.h"
#include "model.h"
#include "mouse.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "platformhelpers.h"
#include "postprocess.h"
#include "postprocess.h"
#include "postprocess.h"
#include "primitivebatch.h"
#include "rendertargetstate.h"
#include "rendertargetstate.h"
#include "rendertargetstate.h"
#include "rendertargetstate.h"
#include "rendertargetstate.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "resourceuploadbatch.h"
#include "sdkmesh.h"
#include "screengrab.h"
#include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestEqNe.inc"
#include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestEqNeNoFog.inc"
#include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestLtGt.inc"
#include "Shaders/Compiled/AlphaTestEffect_PSAlphaTestLtGtNoFog.inc"
#include "Shaders/Compiled/AlphaTestEffect_VSAlphaTest.inc"
#include "Shaders/Compiled/AlphaTestEffect_VSAlphaTestNoFog.inc"
#include "Shaders/Compiled/AlphaTestEffect_VSAlphaTestVc.inc"
#include "Shaders/Compiled/AlphaTestEffect_VSAlphaTestVcNoFog.inc"
#include "Shaders/Compiled/BasicEffect_PSBasic.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicNoFog.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicPixelLighting.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicPixelLightingTx.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicTx.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicTxNoFog.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicVertexLighting.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicVertexLightingNoFog.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicVertexLightingTx.inc"
#include "Shaders/Compiled/BasicEffect_PSBasicVertexLightingTxNoFog.inc"
#include "Shaders/Compiled/BasicEffect_VSBasic.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicNoFog.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLighting.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTx.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTxBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTxVc.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingTxVcBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingVc.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicPixelLightingVcBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicTx.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicTxNoFog.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicTxVc.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicTxVcNoFog.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVc.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVcNoFog.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLighting.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTx.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTxBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTxVc.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingTxVcBn.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingVc.inc"
#include "Shaders/Compiled/BasicEffect_VSBasicVertexLightingVcBn.inc"
#include "Shaders/Compiled/DebugEffect_PSHemiAmbient.inc"
#include "Shaders/Compiled/DebugEffect_PSRGBBiTangents.inc"
#include "Shaders/Compiled/DebugEffect_PSRGBNormals.inc"
#include "Shaders/Compiled/DebugEffect_PSRGBTangents.inc"
#include "Shaders/Compiled/DebugEffect_VSDebug.inc"
#include "Shaders/Compiled/DebugEffect_VSDebugBn.inc"
#include "Shaders/Compiled/DebugEffect_VSDebugVc.inc"
#include "Shaders/Compiled/DebugEffect_VSDebugVcBn.inc"
#include "Shaders/Compiled/DualTextureEffect_PSDualTexture.inc"
#include "Shaders/Compiled/DualTextureEffect_PSDualTextureNoFog.inc"
#include "Shaders/Compiled/DualTextureEffect_VSDualTexture.inc"
#include "Shaders/Compiled/DualTextureEffect_VSDualTextureNoFog.inc"
#include "Shaders/Compiled/DualTextureEffect_VSDualTextureVc.inc"
#include "Shaders/Compiled/DualTextureEffect_VSDualTextureVcNoFog.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMap.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapNoFog.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLighting.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpecular.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_PSEnvMapSpecularNoFog.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMap.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapBn.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapFresnel.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapFresnelBn.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapPixelLighting.inc"
#include "Shaders/Compiled/EnvironmentMapEffect_VSEnvMapPixelLightingBn.inc"
#include "Shaders/Compiled/GenerateMips_main.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTxNoFog.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc"
#include "Shaders/Compiled/NormalMapEffect_PSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxBn.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVc.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVcBn.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc"
#include "Shaders/Compiled/NormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc"
#include "Shaders/Compiled/PBREffect_PSConstant.inc"
#include "Shaders/Compiled/PBREffect_PSTextured.inc"
#include "Shaders/Compiled/PBREffect_PSTexturedEmissive.inc"
#include "Shaders/Compiled/PBREffect_PSTexturedEmissiveVelocity.inc"
#include "Shaders/Compiled/PBREffect_PSTexturedVelocity.inc"
#include "Shaders/Compiled/PBREffect_VSConstant.inc"
#include "Shaders/Compiled/PBREffect_VSConstantBn.inc"
#include "Shaders/Compiled/PBREffect_VSConstantVelocity.inc"
#include "Shaders/Compiled/PBREffect_VSConstantVelocityBn.inc"
#include "Shaders/Compiled/PostProcess_PSBloomBlur.inc"
#include "Shaders/Compiled/PostProcess_PSBloomCombine.inc"
#include "Shaders/Compiled/PostProcess_PSBloomExtract.inc"
#include "Shaders/Compiled/PostProcess_PSCopy.inc"
#include "Shaders/Compiled/PostProcess_PSDownScale2x2.inc"
#include "Shaders/Compiled/PostProcess_PSDownScale4x4.inc"
#include "Shaders/Compiled/PostProcess_PSGaussianBlur5x5.inc"
#include "Shaders/Compiled/PostProcess_PSMerge.inc"
#include "Shaders/Compiled/PostProcess_PSMonochrome.inc"
#include "Shaders/Compiled/PostProcess_PSSepia.inc"
#include "Shaders/Compiled/PostProcess_VSQuad.inc"
#include "Shaders/Compiled/PostProcess_VSQuadDual.inc"
#include "Shaders/Compiled/PostProcess_VSQuadNoCB.inc"
#include "Shaders/Compiled/SkinnedEffect_PSSkinnedPixelLighting.inc"
#include "Shaders/Compiled/SkinnedEffect_PSSkinnedVertexLighting.inc"
#include "Shaders/Compiled/SkinnedEffect_PSSkinnedVertexLightingNoFog.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingFourBones.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingOneBone.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingOneBoneBn.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingTwoBones.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedPixelLightingTwoBonesBn.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingFourBones.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingOneBone.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingOneBoneBn.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingTwoBones.inc"
#include "Shaders/Compiled/SkinnedEffect_VSSkinnedVertexLightingTwoBonesBn.inc"
#include "Shaders/Compiled/SpriteEffect_SpritePixelShader.inc"
#include "Shaders/Compiled/SpriteEffect_SpritePixelShaderHeap.inc"
#include "Shaders/Compiled/SpriteEffect_SpriteVertexShader.inc"
#include "Shaders/Compiled/SpriteEffect_SpriteVertexShaderHeap.inc"
#include "Shaders/Compiled/ToneMap_PS_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSACESFilmic.inc"
#include "Shaders/Compiled/ToneMap_PSACESFilmic_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSCopy.inc"
#include "Shaders/Compiled/ToneMap_PSHDR10.inc"
#include "Shaders/Compiled/ToneMap_PSReinhard.inc"
#include "Shaders/Compiled/ToneMap_PSReinhard_SRGB.inc"
#include "Shaders/Compiled/ToneMap_PSSaturate.inc"
#include "Shaders/Compiled/ToneMap_PSSaturate_SRGB.inc"
#include "Shaders/Compiled/ToneMap_VSQuad.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestEqNe.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestEqNeNoFog.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestLtGt.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_PSAlphaTestLtGtNoFog.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTest.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTestNoFog.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTestVc.inc"
#include "Shaders/Compiled/XboxOneAlphaTestEffect_VSAlphaTestVcNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasic.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicPixelLighting.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicPixelLightingTx.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicTx.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicTxNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLighting.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLightingNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLightingTx.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_PSBasicVertexLightingTxNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasic.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLighting.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTx.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTxBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTxVc.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingTxVcBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingVc.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicPixelLightingVcBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTx.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTxNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTxVc.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicTxVcNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVc.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVcNoFog.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLighting.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTx.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTxBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTxVc.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingTxVcBn.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingVc.inc"
#include "Shaders/Compiled/XboxOneBasicEffect_VSBasicVertexLightingVcBn.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_PSHemiAmbient.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_PSRGBBiTangents.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_PSRGBNormals.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_PSRGBTangents.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_VSDebug.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_VSDebugBn.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_VSDebugVc.inc"
#include "Shaders/Compiled/XboxOneDebugEffect_VSDebugVcBn.inc"
#include "Shaders/Compiled/XboxOneDualTextureEffect_PSDualTexture.inc"
#include "Shaders/Compiled/XboxOneDualTextureEffect_PSDualTextureNoFog.inc"
#include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTexture.inc"
#include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTextureNoFog.inc"
#include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTextureVc.inc"
#include "Shaders/Compiled/XboxOneDualTextureEffect_VSDualTextureVcNoFog.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMap.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapNoFog.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLighting.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLightingFresnel.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLightingFresnelNoFog.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapPixelLightingNoFog.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpecular.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_PSEnvMapSpecularNoFog.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMap.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapBn.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapFresnel.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapFresnelBn.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapPixelLighting.inc"
#include "Shaders/Compiled/XboxOneEnvironmentMapEffect_VSEnvMapPixelLightingBn.inc"
#include "Shaders/Compiled/XboxOneGenerateMips_main.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTxNoFog.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTxNoFogSpec.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_PSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTx.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxBn.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxNoSpec.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxNoSpecBn.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVc.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVcBn.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVcNoSpec.inc"
#include "Shaders/Compiled/XboxOneNormalMapEffect_VSNormalPixelLightingTxVcNoSpecBn.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSConstant.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTextured.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTexturedEmissive.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTexturedEmissiveVelocity.inc"
#include "Shaders/Compiled/XboxOnePBREffect_PSTexturedVelocity.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstant.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstantBn.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstantVelocity.inc"
#include "Shaders/Compiled/XboxOnePBREffect_VSConstantVelocityBn.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSBloomBlur.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSBloomCombine.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSBloomExtract.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSCopy.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSDownScale2x2.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSDownScale4x4.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSGaussianBlur5x5.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSMerge.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSMonochrome.inc"
#include "Shaders/Compiled/XboxOnePostProcess_PSSepia.inc"
#include "Shaders/Compiled/XboxOnePostProcess_VSQuad.inc"
#include "Shaders/Compiled/XboxOnePostProcess_VSQuadDual.inc"
#include "Shaders/Compiled/XboxOnePostProcess_VSQuadNoCB.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_PSSkinnedPixelLighting.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_PSSkinnedVertexLighting.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_PSSkinnedVertexLightingNoFog.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingFourBones.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingFourBonesBn.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingOneBone.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingOneBoneBn.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingTwoBones.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedPixelLightingTwoBonesBn.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingFourBones.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingFourBonesBn.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingOneBone.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingOneBoneBn.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingTwoBones.inc"
#include "Shaders/Compiled/XboxOneSkinnedEffect_VSSkinnedVertexLightingTwoBonesBn.inc"
#include "Shaders/Compiled/XboxOneSpriteEffect_SpritePixelShader.inc"
#include "Shaders/Compiled/XboxOneSpriteEffect_SpritePixelShaderHeap.inc"
#include "Shaders/Compiled/XboxOneSpriteEffect_SpriteVertexShader.inc"
#include "Shaders/Compiled/XboxOneSpriteEffect_SpriteVertexShaderHeap.inc"
#include "Shaders/Compiled/XboxOneToneMap_PS_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSACESFilmic.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSACESFilmic_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSCopy.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_ACESFilmic.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_ACESFilmic_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Reinhard.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Reinhard_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Saturate.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSHDR10_Saturate_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSReinhard.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSReinhard_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSSaturate.inc"
#include "Shaders/Compiled/XboxOneToneMap_PSSaturate_SRGB.inc"
#include "Shaders/Compiled/XboxOneToneMap_VSQuad.inc"
#include "sharedresourcepool.h"
#include "sharedresourcepool.h"
#include "sharedresourcepool.h"
#include "sharedresourcepool.h"
#include "sharedresourcepool.h"
#include "simplemath.h"
#include "simplemath.inl"
#include "soundcommon.h"
#include "soundcommon.h"
#include "soundcommon.h"
#include "soundcommon.h"
#include "soundcommon.h"
#include "soundcommon.h"
#include "spritebatch.h"
#include "spritebatch.h"
#include "spritefont.h"
#include "teapotdata.inc"
#include "vertextypes.h"
#include "vertextypes.h"
#include "vertextypes.h"
#include "vertextypes.h"
#include "vertextypes.h"
#include "vertextypes.h"
#include "wavfilereader.h"
#include "wavfilereader.h"
#include "wictextureloader.h"
#include "wictextureloader.h"
#include "wavebankreader.h"
#include "wavebankreader.h"
#include "xboxddstextureloader.h"
#include "d3d12.h"

#include "june2010/comdecl.h>
#include "june2010/x3daudio.h>
#include "june2010/xapofx.h>
#include "june2010/xaudio2.h>
#include "june2010/xaudio2fx.h>

#include <algorithm>
#include <array>
#include <_ASSERT.h>
#include <atomic>
#include <exception>
#include <functional>
#include <future>
#include <initializer_list>
#include <iterator>
#include <list>
#include <malloc.h>
#include <map>
#include <memory.h>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <wrl.h>
#include <wrl/client.h>

#include <Windows.Devices.Input.h>
#include <Windows.Foundation.Collections.h>
#include <Windows.Media.Devices.h>
#include <Windows.Xbox.Input.h>

#include <mmreg.h>
#include <objbase.h>
#include <ocidl.h>

#include <d3d12.h>
#include <dxgi1_2.h>

#include <x3daudio.h>
#include <xapofx.h>
#include <xaudio2.h>
#include <xaudio2fx.h>

#include <windows.devices.enumeration.h>
#include <windows.gaming.input.h>

#include <apu.h>
#include <shapexmacontext.h>
#include <xdk.h>
#include <xma2defs.h>

#endif
