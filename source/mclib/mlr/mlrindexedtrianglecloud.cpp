//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlr.h"
#include "mlr/mlrclipper.h"
#include "mlr/mlrsorter.h"
#include "mlr/mlreffect.h"
#include "mlr/mlrprimitivebase.h"
#include "mlr/mlrcliptrick.h"
#include "mlr/mlrindexedtrianglecloud.h"

#ifndef HUNT_CLIP_ERROR
#define HUNT_CLIP_ERROR 0
#endif

namespace MidLevelRenderer
{

extern uint32_t gShowClippedPolys;
extern uint16_t* indexOffset; // [MidLevelRenderer::Max_Number_Vertices_Per_Mesh]

//#############################################################################
//#################### MLRIndexedTriangleCloud ##########################
//#############################################################################

std::vector<uint16_t>* MLRIndexedTriangleCloud::clipExtraIndex;
std::vector<Stuff::Vector2DScalar>* MLRIndexedTriangleCloud::clipExtraTexCoords;
std::vector<uint8_t>* MLRIndexedTriangleCloud::visibleIndexedVertices;

MLRIndexedTriangleCloud::ClassData* MLRIndexedTriangleCloud::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRIndexedTriangleCloud::InitializeClass(void)
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLRIndexedTriangleCloudClassID,
		"MidLevelRenderer::MLRIndexedTriangleCloud", MLRTriangleCloud::DefaultData);
	Register_Object(DefaultData);
	clipExtraIndex = new std::vector<uint16_t>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Pointer(clipExtraIndex);
	clipExtraTexCoords =
		new std::vector<Stuff::Vector2DScalar>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Pointer(clipExtraTexCoords);
	visibleIndexedVertices = new std::vector<uint8_t>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Pointer(visibleIndexedVertices);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRIndexedTriangleCloud::TerminateClass(void)
{
	Unregister_Pointer(clipExtraIndex);
	delete clipExtraIndex;
	Unregister_Pointer(clipExtraTexCoords);
	delete clipExtraTexCoords;
	Unregister_Pointer(visibleIndexedVertices);
	delete visibleIndexedVertices;
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRIndexedTriangleCloud::SetData(const size_t* tri_count, const size_t* point_count,
	const uint16_t* index_data, const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data,
	const Stuff::Vector2DScalar* uv_data)
{
	// Check_Pointer(this);
	usedNrOfTriangles = tri_count;
	usedNrOfPoints = point_count;
	_ASSERT(*usedNrOfTriangles <= maxNrOf);
	index = index_data;
	points = point_data;
	colors = color_data;
	texCoords = uv_data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRIndexedTriangleCloud::Draw(
	DrawEffectInformation* dInfo, GOSVertexPool* allVerticesToDraw, MLRSorter* sorter)
{
	// Check_Object(this);
	worldToEffect.Invert(*dInfo->effectToWorld);
	Stuff::Vector4D* v4 = transformedCoords->GetData();
	for (size_t k = 0; k < *usedNrOfPoints; k++, v4++)
	{
		v4->Multiply(points[k], effectToClipMatrix);
		(*clipPerVertex)[k].Clip4dVertex(v4);
	}
#if 0
	Lighting(*shape->worldToShape, dInfo->activeLights, dInfo->nrOfActiveLights);
#endif
	if (Clip(dInfo->clippingFlags, allVerticesToDraw))
	{
		sorter->AddEffect(this, dInfo->state);
	}
}

static MLRClippingState theAnd, theOr, theTest;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
uint32_t
MLRIndexedTriangleCloud::Clip(MLRClippingState clippingFlags, GOSVertexPool* vt)
{
	size_t myNumberUsedClipVertex, myNumberUsedClipIndex, myNumberUsedClipLength;
	(void)clippingFlags; // C4100
	myNumberUsedClipVertex = 0;
	myNumberUsedClipIndex = 0;
	myNumberUsedClipLength = 0;
	_ASSERT(*usedNrOfTriangles > 0);
	//
	//------------------------
	// handle the indexed case
	//------------------------
	//
	memset(visibleIndexedVertices->GetData(), 0, *usedNrOfPoints);
	//
	//-----------------------------------------------------------------
	// Step through each polygon, making sure that we don't try to clip
	// backfaced polygons
	//-----------------------------------------------------------------
	//
	size_t i, j, k, k0, k1;
	size_t* cs = (size_t*)clipPerVertex->GetData();
	uint16_t l;
	size_t index0, index1, index2, ret = 0;
	uint32_t mask;
	float a = 0.0f;
	for (i = 0, j = 0; i < *usedNrOfTriangles; j += 3, ++i)
	{
		index0 = index[j];
		index1 = index[j + 1];
		index2 = index[j + 2];
		//
		//---------------------------------------------------------------
		// Test each vertex of the polygon against the allowed clipping
		// planes, and accumulate status for which planes always clip and
		// which planes clipped at least once
		//---------------------------------------------------------------
		//
		theAnd = cs[index0];
		theAnd &= (cs[index1] & cs[index2]);
		theOr |= (cs[index1] | cs[index2]);
		theAnd = theOr = 0; // ASSUME NO CLIPPING NEEDED FOR MC2. Its just not done here!
		//
		//-------------------------------------------------------------------
		// If any bit is set for all vertices, then the polygon is completely
		// outside the viewing space and we don't have to draw it. On the
		// other hand, if no bits at all were ever set, we can do a trivial
		// accept of the polygon
		//-------------------------------------------------------------------
		//
		if (theAnd != 0)
		{
			testList[i] = 0;
#ifdef LAB_ONLY
			Set_Statistic(PolysClippedButOutside, PolysClippedButOutside + 1);
#endif
		}
		else if (theOr == 0)
		{
			testList[i] = 1;
			ret++;
			(*visibleIndexedVertices)[index0] = 1;
			(*visibleIndexedVertices)[index1] = 1;
			(*visibleIndexedVertices)[index2] = 1;
#ifdef LAB_ONLY
			Set_Statistic(PolysClippedButInside, PolysClippedButInside + 1);
#endif
		}
		//
		//-----------------------------------------------------------------
		// It is not a trivial case, so we must now do real clipping on the
		// polygon
		//-----------------------------------------------------------------
		//
		else
		{
			uint16_t numberVerticesPerPolygon = 0;
			//
			//---------------------------------------------------------------
			// handle the case of a single clipping plane by stepping through
			// the vertices and finding the edge it originates
			//---------------------------------------------------------------
			//
			bool firstIsIn;
			if (theOr.GetNumberOfSetBits() == 1)
			{
#ifdef LAB_ONLY
				Set_Statistic(PolysClippedButOnePlane, PolysClippedButOnePlane + 1);
#endif
				for (k = j; k < j + 3; k++)
				{
					k0 = index[k];
					k1 = index[(k + 1) < j + 3 ? k + 1 : j];
					//
					//----------------------------------------------------
					// If this vertex is inside the viewing space, copy it
					// directly to the clipping buffer
					//----------------------------------------------------
					//
					size_t clipped_index = myNumberUsedClipVertex + numberVerticesPerPolygon;
					theTest = cs[k0];
					if (theTest == 0)
					{
						firstIsIn = true;
						(*clipExtraCoords)[clipped_index] = (*transformedCoords)[k0];
						(*clipExtracolours)[clipped_index] = colors[k0];
						(*clipExtraTexCoords)[clipped_index] = texCoords[k0];
						numberVerticesPerPolygon++;
						clipped_index++;
						//
						//-------------------------------------------------------
						// We don't need to clip this edge if the next vertex is
						// also in the viewing space, so just move on to the
						// next vertex
						//-------------------------------------------------------
						//
						if (cs[k1] == 0)
						{
							continue;
						}
					}
					//
					//---------------------------------------------------------
					// This vertex is outside the viewing space, so if the next
					// vertex is also outside the viewing space, no clipping is
					// needed and we throw this vertex away. Since only one
					// clipping plane is involved, it must be in the same space
					// as the first vertex
					//---------------------------------------------------------
					//
					else
					{
						firstIsIn = false;
						if (cs[k1] != 0)
						{
							_ASSERT(cs[k1] == cs[k0]);
							continue;
						}
					}
					//
					//--------------------------------------------------
					// We now find the distance along the edge where the
					// clipping plane will intersect
					//--------------------------------------------------
					//
					size_t ct = 0;
					mask = 1;
					theTest |= cs[k1];
					//
					//-----------------------------------------------------
					// Find the boundary conditions that match our clipping
					// plane
					//-----------------------------------------------------
					//
					for (l = 0; l < MLRClippingState::NextBit; l++)
					{
						if (theTest.IsClipped(mask))
						{
							// GetDoubleBC(l, bc0, bc1, transformedCoords[k0],
							// transformedCoords[k1]);
							//
							//-------------------------------------------
							// Find the clipping interval from bc0 to bc1
							//-------------------------------------------
							//
							if (firstIsIn == true)
							{
								a = GetLerpFactor(
									l, (*transformedCoords)[k0], (*transformedCoords)[k1]);
							}
							else
							{
								a = GetLerpFactor(
									l, (*transformedCoords)[k1], (*transformedCoords)[k0]);
							}
							_ASSERT(a >= 0.0f && a <= 1.0f);
							ct = l;
							break;
						}
						mask <<= 1;
					}
					//
					//------------------------------
					// Lerp the homogeneous position
					//------------------------------
					//
					if (firstIsIn == true)
					{
						(*clipExtraCoords)[clipped_index].Lerp(
							(*transformedCoords)[k0], (*transformedCoords)[k1], a);
						DoClipTrick((*clipExtraCoords)[clipped_index], ct);
						//
						//----------------------------------------------------------
						// If there are colors, lerp them in screen space for
						// now as most cards do that anyway
						//----------------------------------------------------------
						//
#if COLOR_AS_DWORD
						(*clipExtracolours)[clipped_index] =
							colour_DWORD_Lerp(colors[k0], colors[k1], a);
#else
						(*clipExtracolours)[clipped_index].Lerp(colors[k0], colors[k1], a);
#endif
						//
						//-----------------------------------------------------
						// If there are texture uv's, we need to lerp them in a
						// perspective correct manner
						//-----------------------------------------------------
						//
						(*clipExtraTexCoords)[clipped_index].Lerp(texCoords[k0], texCoords[k1], a);
					}
					else
					{
						(*clipExtraCoords)[clipped_index].Lerp(
							(*transformedCoords)[k1], (*transformedCoords)[k0], a);
						DoClipTrick((*clipExtraCoords)[clipped_index], ct);
						//
						//----------------------------------------------------------
						// If there are colors, lerp them in screen space for
						// now as most cards do that anyway
						//----------------------------------------------------------
						//
#if COLOR_AS_DWORD
						(*clipExtracolours)[clipped_index] =
							colour_DWORD_Lerp(colors[k1], colors[k0], a);
#else
						(*clipExtracolours)[clipped_index].Lerp(colors[k1], colors[k0], a);
#endif
						//
						//-----------------------------------------------------
						// If there are texture uv's, we need to lerp them in a
						// perspective correct manner
						//-----------------------------------------------------
						//
						(*clipExtraTexCoords)[clipped_index].Lerp(texCoords[k1], texCoords[k0], a);
					}
					//
					//--------------------------------
					// Bump the polygon's vertex count
					//--------------------------------
					//
					numberVerticesPerPolygon++;
				}
				(*clipExtraLength)[myNumberUsedClipLength] = numberVerticesPerPolygon;
#ifdef _ARMOR
				(*clipExtraLength)[myNumberUsedClipLength] &= ~0x8000;
#endif
			}
			//
			//---------------------------------------------------------------
			// We have to handle multiple planes. We do this by creating two
			// buffers and we switch between them as we clip plane by plane
			//---------------------------------------------------------------
			//
			else
			{
#ifdef LAB_ONLY
				Set_Statistic(PolysClippedButGOnePlane, PolysClippedButGOnePlane + 1);
#endif
				ClipData2 srcPolygon, dstPolygon;
				size_t dstBuffer = 1;
				srcPolygon.coords = clipBuffer[dstBuffer].coords.GetData();
				srcPolygon.colors = clipBuffer[dstBuffer].colors.GetData();
				srcPolygon.texCoords = clipBuffer[dstBuffer].texCoords.GetData();
				srcPolygon.clipPerVertex = clipBuffer[dstBuffer].clipPerVertex.GetData();
				//
				//----------------------------------------------------------
				// unravel and copy the original data into the source buffer
				//----------------------------------------------------------
				//
				for (k = j, l = 0; k < j + 3; k++, l++)
				{
					size_t indexK = index[k];
					srcPolygon.coords[l] = (*transformedCoords)[indexK];
					srcPolygon.colors[l] = colors[indexK];
					srcPolygon.texCoords[l] = texCoords[indexK];
					srcPolygon.clipPerVertex[l] = (*clipPerVertex)[indexK];
				}
				srcPolygon.length = l;
				//
				//--------------------------------
				// Point to the destination buffer
				//--------------------------------
				//
				dstBuffer = 0;
				dstPolygon.coords = clipBuffer[dstBuffer].coords.GetData();
				dstPolygon.colors = clipBuffer[dstBuffer].colors.GetData();
				dstPolygon.texCoords = clipBuffer[dstBuffer].texCoords.GetData();
				dstPolygon.clipPerVertex = clipBuffer[dstBuffer].clipPerVertex.GetData();
				dstPolygon.length = 0;
				//
				//-----------------------------------------------------------
				// Spin through each plane that clipped the primitive and use
				// it to actually clip the primitive
				//-----------------------------------------------------------
				//
				mask = 1;
				MLRClippingState theNewOr(0);
				int32_t loop = 4;
#if HUNT_CLIP_ERROR
				for (k = 0; k < srcPolygon.length; k++)
				{
					DEBUG_STREAM << setiosflags(ios::scientific) << setprecision(20)
								 << srcPolygon.coords[k].x << " " << srcPolygon.coords[k].y << " "
								 << srcPolygon.coords[k].z << " " << srcPolygon.coords[k].w << '\n';
				}
#endif
#if HUNT_CLIP_ERROR
				DEBUG_STREAM << "TheOriginalOR: " << hex << theOr.GetClippingState() << dec << '\n';
#endif
				do
				{
					for (l = 0; l < MLRClippingState::NextBit; l++)
					{
						if (theOr.IsClipped(mask))
						{
							//
							//-----------------------------------
							// Clip each vertex against the plane
							//-----------------------------------
							//
#if HUNT_CLIP_ERROR
							DEBUG_STREAM << l << ": " << '\n';
#endif
							for (k = 0; k < srcPolygon.length; k++)
							{
								k1 = (k + 1) < srcPolygon.length ? k + 1 : 0;
								theTest = srcPolygon.clipPerVertex[k];
								//
								//----------------------------------------------------
								// If this vertex is inside the viewing space,
								// copy it directly to the clipping buffer
								//----------------------------------------------------
								//
								if (theTest.IsClipped(mask) == 0)
								{
									firstIsIn = true;
									dstPolygon.coords[dstPolygon.length] = srcPolygon.coords[k];
#if HUNT_CLIP_ERROR
									DEBUG_STREAM << k << " goes " << setiosflags(ios::scientific)
												 << setprecision(20) << srcPolygon.coords[k].x
												 << " " << srcPolygon.coords[k].y << " "
												 << srcPolygon.coords[k].z << " "
												 << srcPolygon.coords[k].w << '\n';
#endif
									dstPolygon.clipPerVertex[dstPolygon.length] =
										srcPolygon.clipPerVertex[k];
									dstPolygon.colors[dstPolygon.length] = srcPolygon.colors[k];
									dstPolygon.texCoords[dstPolygon.length] =
										srcPolygon.texCoords[k];
									dstPolygon.length++;
									//
									//-------------------------------------------------------
									// We don't need to clip this edge if the
									// next vertex is also in the viewing space,
									// so just move on to the next vertex
									//-------------------------------------------------------
									//
									if (srcPolygon.clipPerVertex[k1].IsClipped(mask) == 0)
									{
										continue;
									}
								}
								//
								//---------------------------------------------------------
								// This vertex is outside the viewing space, so
								// if the next vertex is also outside the
								// viewing space, no clipping is needed and we
								// throw this vertex away. Since only one
								// clipping plane is involved, it must be in the
								// same space as the first vertex
								//---------------------------------------------------------
								//
								else
								{
									firstIsIn = false;
									if (srcPolygon.clipPerVertex[k1].IsClipped(mask) != 0)
									{
										_ASSERT(srcPolygon.clipPerVertex[k1].IsClipped(mask) == srcPolygon.clipPerVertex[k].IsClipped(mask));
										continue;
									}
								}
								//
								//-------------------------------------------
								// Find the clipping interval from bc0 to bc1
								//-------------------------------------------
								//
								if (firstIsIn == true)
								{
									a = GetLerpFactor(
										l, srcPolygon.coords[k], srcPolygon.coords[k1]);
									_ASSERT(a >= 0.0f && a <= 1.0f);
									//
									//------------------------------
									// Lerp the homogeneous position
									//------------------------------
									//
									dstPolygon.coords[dstPolygon.length].Lerp(
										srcPolygon.coords[k], srcPolygon.coords[k1], a);
#if HUNT_CLIP_ERROR
									DEBUG_STREAM << "True " << a << " " << k << " " << k1
												 << " we get " << dstPolygon.length << '\n';
									DEBUG_STREAM << setiosflags(ios::scientific) << setprecision(20)
												 << dstPolygon.coords[dstPolygon.length].x << " "
												 << dstPolygon.coords[dstPolygon.length].y << " "
												 << dstPolygon.coords[dstPolygon.length].z << " "
												 << dstPolygon.coords[dstPolygon.length].w << '\n';
#endif
									DoClipTrick(dstPolygon.coords[dstPolygon.length], l);
									//
									//----------------------------------------------------------
									// If there are colors, lerp them in screen
									// space for now as most cards do that
									// anyway
									//----------------------------------------------------------
									//
#if COLOR_AS_DWORD
									dstPolygon.colors[dstPolygon.length] = colour_DWORD_Lerp(
										srcPolygon.colors[k], srcPolygon.colors[k1], a);
#else
									dstPolygon.colors[dstPolygon.length].Lerp(
										srcPolygon.colors[k], srcPolygon.colors[k1], a);
#endif
									//
									//-----------------------------------------------------
									// If there are texture uv's, we need to
									// lerp them in a perspective correct manner
									//-----------------------------------------------------
									//
									dstPolygon.texCoords[dstPolygon.length].Lerp(
										srcPolygon.texCoords[k], srcPolygon.texCoords[k1], a);
								}
								else
								{
									a = GetLerpFactor(
										l, srcPolygon.coords[k1], srcPolygon.coords[k]);
									_ASSERT(a >= 0.0f && a <= 1.0f);
									//
									//------------------------------
									// Lerp the homogeneous position
									//------------------------------
									//
									dstPolygon.coords[dstPolygon.length].Lerp(
										srcPolygon.coords[k1], srcPolygon.coords[k], a);
#if HUNT_CLIP_ERROR
									DEBUG_STREAM << "False " << a << " " << k << " " << k1
												 << " we get " << dstPolygon.length << '\n';
									DEBUG_STREAM << setiosflags(ios::scientific) << setprecision(20)
												 << dstPolygon.coords[dstPolygon.length].x << " "
												 << dstPolygon.coords[dstPolygon.length].y << " "
												 << dstPolygon.coords[dstPolygon.length].z << " "
												 << dstPolygon.coords[dstPolygon.length].w << '\n';
#endif
									DoClipTrick(dstPolygon.coords[dstPolygon.length], l);
									//
									//----------------------------------------------------------
									// If there are colors, lerp them in screen
									// space for now as most cards do that
									// anyway
									//----------------------------------------------------------
									//
#if COLOR_AS_DWORD
									dstPolygon.colors[dstPolygon.length] = colour_DWORD_Lerp(
										srcPolygon.colors[k1], srcPolygon.colors[k], a);
#else
									dstPolygon.colors[dstPolygon.length].Lerp(
										srcPolygon.colors[k1], srcPolygon.colors[k], a);
#endif
									//
									//-----------------------------------------------------
									// If there are texture uv's, we need to
									// lerp them in a perspective correct manner
									//-----------------------------------------------------
									//
									dstPolygon.texCoords[dstPolygon.length].Lerp(
										srcPolygon.texCoords[k1], srcPolygon.texCoords[k], a);
								}
								//
								//-------------------------------------
								// We have to generate a new clip state
								//-------------------------------------
								//
								dstPolygon.clipPerVertex[dstPolygon.length].Clip4dVertex(
									&dstPolygon.coords[dstPolygon.length]);
								//
								//----------------------------------
								// Bump the new polygon vertex count
								//----------------------------------
								//
								dstPolygon.length++;
							}
							//
							//-----------------------------------------------
							// Swap source and destination buffer pointers in
							// preparation for the next plane test
							//-----------------------------------------------
							//
							srcPolygon.coords = clipBuffer[dstBuffer].coords.GetData();
							srcPolygon.colors = clipBuffer[dstBuffer].colors.GetData();
							srcPolygon.texCoords = clipBuffer[dstBuffer].texCoords.GetData();
							srcPolygon.clipPerVertex =
								clipBuffer[dstBuffer].clipPerVertex.GetData();
							srcPolygon.length = dstPolygon.length;
							dstBuffer = size_t(!dstBuffer ? 1 : 0);
							dstPolygon.coords = clipBuffer[dstBuffer].coords.GetData();
							dstPolygon.colors = clipBuffer[dstBuffer].colors.GetData();
							dstPolygon.texCoords = clipBuffer[dstBuffer].texCoords.GetData();
							dstPolygon.clipPerVertex =
								clipBuffer[dstBuffer].clipPerVertex.GetData();
							dstPolygon.length = 0;
						}
						mask = mask << 1;
					}
					theNewOr = 0;
					for (k = 0; k < srcPolygon.length; k++)
					{
						theNewOr |= srcPolygon.clipPerVertex[k];
					}
#if HUNT_CLIP_ERROR
					DEBUG_STREAM << "TheOR: " << hex << theNewOr.GetClippingState() << dec << '\n';
#endif
					theOr = theNewOr;
				} while (theNewOr != 0 && loop--);
				//
				//--------------------------------------------------
				// could not clip this rare case, just ignore it
				//--------------------------------------------------
				//
				if (theNewOr != 0)
				{
					testList[i] = 0;
					continue;
				}
				//
				//--------------------------------------------------
				// Move the most recent polygon into the clip buffer
				//--------------------------------------------------
				//
#if HUNT_CLIP_ERROR
				DEBUG_STREAM << "Final: " << srcPolygon.length << '\n';
#endif
				for (k = 0; k < srcPolygon.length; k++)
				{
					size_t clipped_index = myNumberUsedClipVertex + k;
#if HUNT_CLIP_ERROR
					DEBUG_STREAM << setiosflags(ios::scientific) << setprecision(20)
								 << srcPolygon.coords[k].x << " " << srcPolygon.coords[k].y << " "
								 << srcPolygon.coords[k].z << " " << srcPolygon.coords[k].w << '\n';
#endif
					(*clipExtraCoords)[clipped_index] = srcPolygon.coords[k];
					(*clipExtracolours)[clipped_index] = srcPolygon.colors[k];
					(*clipExtraTexCoords)[clipped_index] = srcPolygon.texCoords[k];
				}
				numberVerticesPerPolygon = srcPolygon.length;
#if HUNT_CLIP_ERROR
				DEBUG_STREAM << "---" << '\n';
#endif
				(*clipExtraLength)[myNumberUsedClipLength] = numberVerticesPerPolygon;
#ifdef _ARMOR
				(*clipExtraLength)[myNumberUsedClipLength] |= 0x8000;
#endif
			}
			myNumberUsedClipVertex += numberVerticesPerPolygon;
			myNumberUsedClipLength++;
			ret++;
			// clip
			// dont draw the original
			testList[i] = 0;
		}
	}
	Check_Object(vt);
	gos_vertices = vt->GetActualVertexPool();
	numGOSVertices = 0;
	gos_indices = vt->GetActualIndexPool();
	numGOSIndices = 0;
	uint16_t strideIndex;
	for (j = 0, strideIndex = 0; j < *usedNrOfPoints; j++)
	{
		if ((*visibleIndexedVertices)[j] == 0)
		{
			strideIndex++;
		}
		else
		{
			indexOffset[j] = static_cast<uint16_t>(j - strideIndex);
			GOSCopyData(
				&gos_vertices[numGOSVertices], transformedCoords->GetData(), colors, texCoords, j);
#ifdef LAB_ONLY
			if (gShowClippedPolys)
			{
				gos_vertices[numGOSVertices].argb = 0xff0000ff;
				gos_vertices[numGOSVertices].u = 0.0f;
				gos_vertices[numGOSVertices].v = 0.0f;
			}
#endif
			numGOSVertices++;
		}
	}
	for (i = 0, j = 0; i < *usedNrOfTriangles; j += 3, ++i)
	{
		if (testList[i] == 0)
		{
			continue;
		}
		_ASSERT((vt->GetLastIndex() + 3 + numGOSIndices) < vt.size());
		_ASSERT(indexOffset[index[j]] < numGOSVertices);
		gos_indices[numGOSIndices] = indexOffset[index[j]];
		_ASSERT(indexOffset[index[j + 2]] < numGOSVertices);
		gos_indices[numGOSIndices + 1] = indexOffset[index[j + 2]];
		_ASSERT(indexOffset[index[j + 1]] < numGOSVertices);
		gos_indices[numGOSIndices + 2] = indexOffset[index[j + 1]];
		numGOSIndices += 3;
	}
	uint16_t stride;
	if (myNumberUsedClipLength > 0)
	{
		for (i = 0, j = 0; i < myNumberUsedClipLength; i++)
		{
#ifdef _ARMOR
			stride = static_cast<uint16_t>((*clipExtraLength)[i] & 0x7fff);
#else
			stride = static_cast<uint16_t>((*clipExtraLength)[i]);
#endif
			for (k = 1; k < size_t(stride - 1); k++)
			{
				_ASSERT((vt->GetLast() + 3 + numGOSVertices) < vt.size());
				_ASSERT((vt->GetLastIndex() + 3 + numGOSIndices) < vt.size());
				GOSCopyTriangleData(&gos_vertices[numGOSVertices], clipExtraCoords->GetData(),
					clipExtracolours->GetData(), clipExtraTexCoords->GetData(), j, j + k + 1, j + k);
#ifdef LAB_ONLY
				if (gShowClippedPolys)
				{
					if ((*clipExtraLength)[i] & 0x8000)
					{
						gos_vertices[numGOSVertices].argb = 0xffff0000;
						gos_vertices[numGOSVertices].u = 0.0f;
						gos_vertices[numGOSVertices].v = 0.0f;
						gos_vertices[numGOSVertices + 1].argb = 0xffff0000;
						gos_vertices[numGOSVertices + 1].u = 0.0f;
						gos_vertices[numGOSVertices + 1].v = 0.0f;
						gos_vertices[numGOSVertices + 2].argb = 0xffff0000;
						gos_vertices[numGOSVertices + 2].u = 0.0f;
						gos_vertices[numGOSVertices + 2].v = 0.0f;
					}
					else
					{
						gos_vertices[numGOSVertices].argb = 0xffff9999;
						gos_vertices[numGOSVertices].u = 0.0f;
						gos_vertices[numGOSVertices].v = 0.0f;
						gos_vertices[numGOSVertices + 1].argb = 0xffff9999;
						gos_vertices[numGOSVertices + 1].u = 0.0f;
						gos_vertices[numGOSVertices + 1].v = 0.0f;
						gos_vertices[numGOSVertices + 2].argb = 0xffff9999;
						gos_vertices[numGOSVertices + 2].u = 0.0f;
						gos_vertices[numGOSVertices + 2].v = 0.0f;
					}
				}
#endif
				_ASSERT((vt->GetLastIndex() + 3 + numGOSIndices) < vt.size());
				_ASSERT(numGOSIndices % 3 == 0);
				gos_indices[numGOSIndices] = (uint16_t)numGOSVertices;
				gos_indices[numGOSIndices + 1] = (uint16_t)(numGOSVertices + 1);
				gos_indices[numGOSIndices + 2] = (uint16_t)(numGOSVertices + 2);
				numGOSVertices += 3;
				numGOSIndices += 3;
			}
			j += stride;
		}
#if HUNT_CLIP_ERROR
		DEBUG_STREAM << "***" << endl
					 << endl;
#endif
	}
	vt->Increase(numGOSVertices);
	vt->IncreaseIndex(numGOSIndices);
	return numGOSIndices;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRIndexedTriangleCloud::TestInstance(void) const
{
	if (usedNrOfTriangles)
	{
		Check_Pointer(usedNrOfTriangles);
		_ASSERT(intptr_t(*usedNrOfTriangles) >= 0);
		_ASSERT(*usedNrOfTriangles <= maxNrOf);
	}
}

} // namespace MidLevelRenderer
