/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Beck. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 mechclassids.h - mclib class id enumerations

 MechCommander 2 source code

 2014-08-22 Jerker Beck, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#pragma once

#ifndef _MECHCLASSIDS_H_
#define _MECHCLASSIDS_H_

namespace Stuff
{

//-------------------------
// ClassID reserved blocks
//-------------------------
enum __stuff_classids : uint32_t
{
	FirstStuffClassID = 0,
	StuffClassIDCount = 64,
	LastStuffClassID = FirstStuffClassID + StuffClassIDCount - 1,

	FirstMLRClassID,
	MLRClassIDCount = 64,
	LastMLRClassID = FirstMLRClassID + MLRClassIDCount - 1,

	FirstElementRendererClassID,
	ElementRendererClassIDCount = 64,
	LastElementRendererClassID = FirstElementRendererClassID + ElementRendererClassIDCount - 1,

	FirstProxyClassID,
	ProxyClassIDCount = 64,
	LastProxyClassID = FirstProxyClassID + ProxyClassIDCount - 1,

	FirstElementProxyClassID,
	ElementProxyClassIDCount = 64,
	LastElementProxyClassID = FirstElementProxyClassID + ElementProxyClassIDCount - 1,

	FirstMGProxyClassID,
	MGProxyClassIDCount = 64,
	LastMGProxyClassID = FirstMGProxyClassID + MGProxyClassIDCount - 1,

	FirstSIProxyClassID,
	SIProxyClassIDCount = 64,
	LastSIProxyClassID = FirstSIProxyClassID + SIProxyClassIDCount - 1,

	FirstAdeptClassID,
	AdeptClassIDCount = 256,
	LastAdeptClassID = FirstAdeptClassID + AdeptClassIDCount - 1,

	FirstWindowsAdeptClassID,
	WindowsAdeptClassIDCount = 32,
	LastWindowsAdeptClassID = FirstWindowsAdeptClassID + WindowsAdeptClassIDCount - 1,

	FirstGOSAdeptClassID,
	GOSAdeptClassIDCount = 16,
	LastGOSAdeptClassID = FirstGOSAdeptClassID + GOSAdeptClassIDCount - 1,

	FirstSRClassID,
	SRClassIDCount = 256,
	LastSRClassID = FirstSRClassID + SRClassIDCount - 1,

	FirstSRGOSClassID,
	SRGOSClassIDCount = 16,
	LastSRGOSClassID = FirstSRGOSClassID + SRGOSClassIDCount - 1,

	FirstMWGOSClassID,
	MWGOSClassIDCount = 16,
	LastMWGOSClassID = FirstMWGOSClassID + MWGOSClassIDCount - 1,

	FirstMAXProxiesClassID,
	MAXProxiesClassIDCount = 16,
	LastMAXProxiesClassID = FirstMAXProxiesClassID + MAXProxiesClassIDCount - 1,

	FirstMW4ClassID,
	MW4ClassIDCount = 256,
	LastMW4ClassID = FirstMW4ClassID + MW4ClassIDCount - 1,

	FirstgosFXClassID,
	gosFXClassIDCount = 64,
	LastgosFXClassID = FirstgosFXClassID + gosFXClassIDCount - 1,

	//
	//------------------------------------------
	// Please note that all new class id's
	// should be above this comment.  NO
	// class id's should be put between
	// the temporary and the class id count
	//------------------------------------------
	//
	FirstTemporaryClassID,
	TemporaryClassIDCount = 16,
	LastTemporaryClassID = FirstTemporaryClassID + TemporaryClassIDCount - 1,

	ClassIDCount // does this work?
};

//
//--------------
// Stuff classes
//--------------
//
enum __stuff_attributeids : uint32_t
{
	//
	//-------------------------
	// attribute IDs
	//-------------------------
	//
	NullClassID = __stuff_classids::ClassIDCount,
	MStringClassID = __stuff_classids::FirstStuffClassID,
	IntClassID,
	ScalarClassID,
	RadianClassID,
	Vector3DClassID,
	Point3DClassID,
	Normal3DClassID,
	UnitQuaternionClassID,
	Motion3DClassID,
	Origin3DClassID,
	LinearMatrix4DClassID,
	Vector2DOfIntClassID,
	Vector2DOfScalarClassID,
	EulerAnglesClassID,
	BoolClassID,

	//
	//---------------------
	// Registered class IDs
	//---------------------
	//
	RegisteredClassClassID,
	MemoryStreamClassID,
	FileStreamClassID,
	PlugClassID,
	NodeClassID,
	CharClassID,
	RGBAColorClassID,
	FirstFreeStuffClassID
};
} // namespace Stuff

namespace MidLevelRenderer
{

//
//--------------
// Stuff classes
//--------------
//
enum __mlr_classids : uint32_t
{
	MLRStateClassID = Stuff::__stuff_classids::FirstMLRClassID,
	MLRClippingStateClassID,
	MLRClipperClassID,
	MLRSorterClassID,
	MLRSortByOrderClassID,
	MLRLightClassID,
	MLRTexturePoolClassID,
	MLRPrimitiveClassID,
	MLRIndexedPrimitiveClassID,
	MLRPolyMeshClassID,
	MLRIndexedPolyMeshClassID,
	MLRShapeClassID,
	MLREffectClassID,
	MLRPointCloudClassID,
	MLRTriangleCloudClassID,
	MLRAmbientLightClassID,
	MLRInfiniteLightClassID,
	MLRInfiniteLightWithFalloffClassID,
	MLRPointLightClassID,
	MLRSpotLightClassID,
	MLRLightMapClassID,

	MLRPrimitiveBaseClassID,
	MLRIndexedPrimitiveBaseClassID,
	MLR_I_PMeshClassID,
	MLR_I_C_PMeshClassID,
	MLR_I_L_PMeshClassID,
	MLR_I_DT_PMeshClassID,
	MLR_I_C_DT_PMeshClassID,
	MLR_I_L_DT_PMeshClassID,
	MLRNGonCloudClassID,
	MLRCardCloudClassID,

	MLR_I_MT_PMeshClassID,

	MLR_I_DeT_PMeshClassID,
	MLR_I_C_DeT_PMeshClassID,
	MLR_I_L_DeT_PMeshClassID,

	MLR_I_TMeshClassID,
	MLR_I_DeT_TMeshClassID,
	MLR_I_C_TMeshClassID,
	MLR_I_L_TMeshClassID,

	MLR_TerrainClassID,
	MLR_Terrain2ClassID,

	MLRLineCloudClassID,

	MLRIndexedTriangleCloudClassID,

	MLR_I_DT_TMeshClassID,
	MLR_I_C_DT_TMeshClassID,
	MLR_I_L_DT_TMeshClassID,

	MLR_I_C_DeT_TMeshClassID,
	MLR_I_L_DeT_TMeshClassID,

	MLRLookUpLightClassID,

	FirstFreeMLRClassID
};
} // namespace MidLevelRenderer

#endif