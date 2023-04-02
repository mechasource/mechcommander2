//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRSORTER_HPP
#define MLR_MLRSORTER_HPP

#include "stuff/point3d.h"
#include "stuff/linearmatrix.h"
//#include "stuff/marray.h"
#include "mlr/mlrstate.h"
#include "mlr/mlrclippingstate.h"
#include "mlr/mlrtexturepool.h"

namespace MidLevelRenderer
{

class MLRPrimitive;
class MLREffect;
class MLRLight;
class GOSVertex;
class GOSVertexPool;
class DrawScreenQuadsInformation;

struct SortAlpha;

class SortData
{
public:
	SortData(void) :
		vertices(nullptr), indices(nullptr), numvertices(0), numIndices(0), type(TriList),
		texture2(0)
	{
	}

	void DrawTriList(void);
	void DrawTriIndexedList(void);
	void DrawPointCloud(void);
	void DrawLineCloud(void);
	void DrawQuads(void);

	int32_t LoadAlphaFromTriList(SortAlpha**);
	int32_t LoadAlphaFromTriIndexedList(SortAlpha**);
	int32_t LoadAlphaFromPointCloud(SortAlpha**);
	int32_t LoadAlphaFromLineCloud(SortAlpha**);
	int32_t LoadAlphaFromQuads(SortAlpha**);

	enum : uint32_t
	{
		TriList = 0,
		TriIndexedList,
		PointCloud,
		Quads,
		LineCloud,
		LastMode
	};

	typedef void (SortData::*DrawFunc)(void);
	typedef int32_t (SortData::*LoadSortAlphaFunc)(SortAlpha**);

	static DrawFunc Draw[LastMode];
	static LoadSortAlphaFunc LoadSortAlpha[LastMode];

	MLRState state;
	PVOID vertices;
	uint16_t* indices;
	uint32_t numvertices;
	uint32_t numIndices;
	uint32_t type;
	uint32_t texture2;
};

class MLRPrimitiveBase;

#define CalDraw

#ifdef CalDraw
struct ToBeDrawnPrimitive
{
	ToBeDrawnPrimitive(void);

	MLRPrimitiveBase* primitive;

	MLRState state;
	Stuff::Point3D cameraPosition;
	MLRClippingState clippingFlags;
	Stuff::Matrix4D shapeToClipMatrix;
	Stuff::LinearMatrix4D worldToShape;

	static GOSVertexPool* allVerticesToDraw;

	MLRLight* activeLights[Limits::Max_Number_Of_Lights_Per_Primitive];

	uint32_t nrOfActiveLights;
};
#endif

//##########################################################################
//######################### MLRSorter ################################
//##########################################################################

class _declspec(novtable) MLRSorter // : public Stuff::RegisteredClass
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructor
	//
public:
	MLRSorter(ClassData* class_data, MLRTexturePool*);
	~MLRSorter(void);

	virtual void AddPrimitive(MLRPrimitiveBase*, uint32_t = 0) = 0;
	virtual void AddEffect(MLREffect*, const MLRState&) = 0;
	virtual void AddScreenQuads(GOSVertex*, const DrawScreenQuadsInformation*) = 0;

	virtual void AddSortRawData(SortData*) = 0;

	virtual void DrawPrimitive(MLRPrimitiveBase*, uint32_t = 0);

	void SetTexturePool(MLRTexturePool* tp)
	{
		// Check_Object(this);
		Check_Object(tp);
		texturePool = tp;
	}

	bool SetDifferences(const MLRState& original, const MLRState& newer);

	// starts the action
	virtual void RenderNow(void) = 0;

	// resets the sorting
	virtual void Reset(void);

	// lets begin the dance
	virtual void StartDraw(const MLRState& default_state);

	// enter raw data
	SortData* SetRawData(PVOID vertices, uint32_t numvertices, const MLRState& state,
		const int32_t& mode, int32_t tex2 = 0);

	SortData* SetRawIndexedData(PVOID vertices, uint32_t numvertices, uint16_t* indices,
		int32_t numIndices, const MLRState& state, const int32_t& mode, int32_t tex2 = 0);

	SortData* SetRawData(MLRPrimitiveBase*, int32_t = 0);

	// Just scaling down a bit to keep z under 1.0f
	void SetFarClipReciprocal(float fcr)
	{
		// Check_Object(this);
		farClipReciprocal = fcr * (1.0f - Stuff::SMALL);
	}

#ifdef CalDraw
	ToBeDrawnPrimitive* GetCurrentTBDP(void)
	{
		// Check_Object(this);
		return &drawData[lastUsedDraw];
	}
	ToBeDrawnPrimitive* GetCurrentTBDP(size_t index)
	{
		// Check_Object(this);
		_ASSERT(index < lastUsedDraw);
		return &drawData[index];
	}
	void IncreaseTBDPCounter(void);
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const;

protected:
	MLRState theCurrentState;
	MLRTexturePool* texturePool;

	int32_t lastUsedRaw;
	std::vector<SortData> rawDrawData; // Max_Number_Primitives_Per_Frame

	int32_t lastUsedInBucketNotDrawn[MLRState::PriorityCount];

#ifdef CalDraw
	size_t lastUsedDraw;
	std::vector<ToBeDrawnPrimitive> drawData; // Max_Number_Primitives_Per_Frame
	std::vector<ToBeDrawnPrimitive*>
		priorityBucketsNotDrawn[MLRState::PriorityCount]; //, Max_Number_Primitives_Per_Frame +
		// Max_Number_ScreenQuads_Per_Frame
#endif
	float farClipReciprocal;
};
} // namespace MidLevelRenderer
#endif
