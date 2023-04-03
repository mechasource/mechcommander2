//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlr.h"
#include "mlr/mlrsorter.h"
#include "mlr/mlrclipper.h"
#include "mlr/mlrngoncloud.h"

namespace MidLevelRenderer
{

//#############################################################################

extern uint32_t gShowClippedPolys;

//#############################################################################
//#######################    MLRNGonCloud    ##################################
//#############################################################################

std::vector<MLRClippingState>* MLRNGonCloud::clipPerVertex;
std::vector<Stuff::Vector4D>* MLRNGonCloud::clipExtraCoords;
std::vector<Stuff::RGBAcolour>* MLRNGonCloud::clipExtracolours;
std::vector<int32_t>* MLRNGonCloud::clipExtraLength;

MLRNGonCloud::ClassData* MLRNGonCloud::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRNGonCloud::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRNGonCloudClassID, "MidLevelRenderer::MLRNGonCloud", MLREffect::DefaultData);
	Register_Object(DefaultData);
	clipPerVertex = new std::vector<MLRClippingState>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipPerVertex);
	clipExtraCoords = new std::vector<Stuff::Vector4D>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipExtraCoords);
	clipExtracolours = new std::vector<Stuff::RGBAcolour>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipExtracolours);
	clipExtraLength = new std::vector<int32_t>(Limits::Max_Number_Primitives_Per_Frame);
	Register_Object(clipExtraLength);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRNGonCloud::TerminateClass()
{
	Unregister_Object(clipPerVertex);
	delete clipPerVertex;
	Unregister_Object(clipExtraCoords);
	delete clipExtraCoords;
	Unregister_Object(clipExtracolours);
	delete clipExtracolours;
	Unregister_Object(clipExtraLength);
	delete clipExtraLength;
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRNGonCloud::MLRNGonCloud(uint32_t vertices, uint32_t nr)
	: MLREffect(nr, DefaultData)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	usedNrOfNGons = nullptr;
	numOfVertices = vertices;
	// Check_Pointer(this);
	_ASSERT(vertices * nr <= Limits::Max_Number_Vertices_Per_Mesh);
	specialClipcolours.SetLength(vertices);
	drawMode = SortData::TriList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRNGonCloud::~MLRNGonCloud()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRNGonCloud::SetData(
	const size_t* count, const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data)
{
	// Check_Pointer(this);
	usedNrOfNGons = count;
	_ASSERT(*usedNrOfNGons <= maxNrOf);
	points = point_data;
	colors = color_data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRNGonCloud::Draw(
	DrawEffectInformation* dInfo, GOSVertexPool* allVerticesToDraw, MLRSorter* sorter)
{
	// Check_Object(this);
	worldToEffect.Invert(*dInfo->effectToWorld);
	Transform(*usedNrOfNGons, numOfVertices);
	if (Clip(dInfo->clippingFlags, allVerticesToDraw))
	{
		sorter->AddEffect(this, dInfo->state);
	}
}

static MLRClippingState theAnd, theOr, theTest;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
uint32_t
MLRNGonCloud::Clip(MLRClippingState clippingFlags, GOSVertexPool* vt)
{
	// Check_Object(this);
	size_t i, j, k;
	size_t end, len = *usedNrOfNGons, ret = 0;
#if EFECT_CLIPPED // this effect gets not clipped
	int32_t l, mask, k1, ct = 0;
	float a = 0.0f, bc0, bc1;
#endif
	Check_Object(vt);
	gos_vertices = vt->GetActualVertexPool();
	Check_Pointer(gos_vertices);
	numGOSVertices = 0;
	//
	//--------------------------------------
	// See if we don't have to draw anything
	//--------------------------------------
	//
	if (clippingFlags.GetClippingState() == 0 || len <= 0)
	{
		if (len <= 0)
		{
			visible = 0;
		}
		else
		{
			//
			//-------------------------------
			// handle the non-indexed version
			//-------------------------------
			//
			for (i = 0, j = 0; i < len; i++, j += numOfVertices)
			{
				//				if(IsOn(i) == false)
				//				{
				//					continue;
				//				}
				uint32_t incolour = GOSCopycolour(&colors[2 * i]);
				uint32_t outcolour = GOSCopycolour(&colors[2 * i + 1]);
				for (size_t z = 1; z < numOfVertices - 1; z++)
				{
					GOSCopyTriangleData(&gos_vertices[numGOSVertices], transformedCoords->GetData(),
						j, z + j + 1, z + j, true);
					gos_vertices[numGOSVertices].argb = incolour;
					gos_vertices[numGOSVertices + 1].argb = outcolour;
					gos_vertices[numGOSVertices + 2].argb = outcolour;
					numGOSVertices += 3;
				}
			}
			Check_Object(vt);
			vt->Increase(numGOSVertices);
			visible = uint32_t(numGOSVertices ? 1 : 0);
		}
		return visible;
	}
	int32_t myNumberUsedClipVertex, myNumberUsedClipIndex, myNumberUsedClipLength;
	myNumberUsedClipVertex = 0;
	myNumberUsedClipIndex = 0;
	myNumberUsedClipLength = 0;
	//
	//-------------------------------
	// handle the non-indexed version
	//-------------------------------
	//
	//
	//-----------------------------------------------------------------
	// Step through each polygon, making sure that we don't try to clip
	// backfaced polygons
	//-----------------------------------------------------------------
	//
	for (i = 0, j = 0; i < len; i++, j += numOfVertices)
	{
		//		if(IsOn(i) == false)
		//		{
		//			continue;
		//		}
		TurnVisible(i);
		//
		//---------------------------------------------------------------
		// Test each vertex of the polygon against the allowed clipping
		// planes, and accumulate status for which planes always clip and
		// which planes clipped at least once
		//---------------------------------------------------------------
		//
		theAnd.SetClippingState(0x3f);
		theOr.SetClippingState(0);
		end = j + numOfVertices;
		Stuff::Vector4D* v4d = transformedCoords->GetData() + j;
		MLRClippingState* cs = clipPerVertex->GetData() + j;
		for (k = j; k < end; k++, v4d++, cs++)
		{
			cs->Clip4dVertex(v4d);
			theAnd &= (*clipPerVertex)[k];
			theOr |= (*clipPerVertex)[k];
#ifdef LAB_ONLY
			if (*cs == 0)
			{
				Set_Statistic(NonClippedVertices, NonClippedVertices + 1);
			}
			else
			{
				Set_Statistic(ClippedVertices, ClippedVertices + 1);
			}
#endif
		}
		theAnd = theOr = 0; // ASSUME NO CLIPPING NEEDED FOR MC2.  Its just not done here!
		//
		//-------------------------------------------------------------------
		// If any bit is set for all vertices, then the polygon is completely
		// outside the viewing space and we don't have to draw it.  On the
		// other hand, if no bits at all were ever set, we can do a trivial
		// accept of the polygon
		//-------------------------------------------------------------------
		//
		if (theAnd != 0)
		{
			TurnInVisible(i);
		}
		else if (theOr == 0)
		{
			TurnVisible(i);
			uint32_t incolour = GOSCopycolour(&colors[2 * i]);
			uint32_t outcolour = GOSCopycolour(&colors[2 * i + 1]);
			for (size_t z = 1; z < numOfVertices - 1; z++)
			{
				GOSCopyTriangleData(&gos_vertices[numGOSVertices], transformedCoords->GetData(), j,
					z + j + 1, z + j, true);
				gos_vertices[numGOSVertices].argb = incolour;
				gos_vertices[numGOSVertices + 1].argb = outcolour;
				gos_vertices[numGOSVertices + 2].argb = outcolour;
#ifdef LAB_ONLY
				if (gShowClippedPolys)
				{
					gos_vertices[numGOSVertices].argb = 0xff0000ff;
					gos_vertices[numGOSVertices].u = 0.0f;
					gos_vertices[numGOSVertices].v = 0.0f;
					gos_vertices[numGOSVertices + 1].argb = 0xff0000ff;
					gos_vertices[numGOSVertices + 1].u = 0.0f;
					gos_vertices[numGOSVertices + 1].v = 0.0f;
					gos_vertices[numGOSVertices + 2].argb = 0xff0000ff;
					gos_vertices[numGOSVertices + 2].u = 0.0f;
					gos_vertices[numGOSVertices + 2].v = 0.0f;
				}
#endif
				numGOSVertices += 3;
			}
			ret++;
		}
		//
		//-----------------------------------------------------------------
		// It is not a trivial case, so we must now do real clipping on the
		// polygon
		//-----------------------------------------------------------------
		//
		else
		{
#if EFECT_CLIPPED // this effect gets not clipped
			int32_t numberVerticesPerPolygon = 0;
			//
			//---------------------------------------------------------------
			// handle the case of a single clipping plane by stepping through
			// the vertices and finding the edge it originates
			//---------------------------------------------------------------
			//
			if (theOr.GetNumberOfSetBits() == 1)
			{
				for (k = j; k < end; k++)
				{
					k1 = (k + 1 < end) ? k + 1 : j;
					//
					//----------------------------------------------------
					// If this vertex is inside the viewing space, copy it
					// directly to the clipping buffer
					//----------------------------------------------------
					//
					int32_t clipped_index = myNumberUsedClipVertex + numberVerticesPerPolygon;
					theTest = clipPerVertex[k];
					if (theTest == 0)
					{
						clipExtraCoords[clipped_index] = transformedCoords[k];
						if (k == j)
						{
							clipExtracolours[clipped_index] = colors[2 * i];
						}
						else
						{
							clipExtracolours[clipped_index] = colors[2 * i + 1];
						}
						numberVerticesPerPolygon++;
						clipped_index++;
						//
						//-------------------------------------------------------
						// We don't need to clip this edge if the next vertex is
						// also in the viewing space, so just move on to the
						// next vertex
						//-------------------------------------------------------
						//
						if (clipPerVertex[k1] == 0)
						{
							continue;
						}
					}
					//
					//---------------------------------------------------------
					// This vertex is outside the viewing space, so if the next
					// vertex is also outside the viewing space, no clipping is
					// needed and we throw this vertex away.  Since only one
					// clipping plane is involved, it must be in the same space
					// as the first vertex
					//---------------------------------------------------------
					//
					else if (clipPerVertex[k1] != 0)
					{
						_ASSERT(clipPerVertex[k1] == clipPerVertex[k]);
						continue;
					}
					//
					//--------------------------------------------------
					// We now find the distance along the edge where the
					// clipping plane will intersect
					//--------------------------------------------------
					//
					mask = 1;
					theTest |= clipPerVertex[k1];
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
							//
							//-------------------------------------------
							// Find the clipping interval from bc0 to bc1
							//-------------------------------------------
							//
							a = GetLerpFactor(l, (*transformedCoords)[k], (*transformedCoords)[k1]);
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
					clipExtraCoords[clipped_index].Lerp(
						transformedCoords[k], transformedCoords[k1], a);
					DoClipTrick(clipExtraCoords[clipped_index], ct);
					//
					//----------------------------------------------------------
					// If there are colors, lerp them in screen space for now as
					// most cards do that anyway
					//----------------------------------------------------------
					//
					clipExtracolours[clipped_index].Lerp(
						(k == j) ? colors[2 * i] : colors[2 * i + 1],
						(k1 == j) ? colors[2 * i] : colors[2 * i + 1], a);
					//
					//--------------------------------
					// Bump the polygon's vertex count
					//--------------------------------
					//
					numberVerticesPerPolygon++;
				}
			}
			//
			//---------------------------------------------------------------
			// We have to handle multiple planes.  We do this by creating two
			// buffers and we switch between them as we clip plane by plane
			//---------------------------------------------------------------
			//
			else
			{
				EffectClipData srcPolygon, dstPolygon;
				int32_t dstBuffer = 0;
				specialClipcolours[0] = colors[2 * i];
				for (l = 1; l < numOfVertices; l++)
				{
					specialClipcolours[l] = colors[2 * i + 1];
				}
				//
				//-----------------------------------------------------
				// Point the source polygon buffer at our original data
				//-----------------------------------------------------
				//
				srcPolygon.coords = &transformedCoords[j];
				srcPolygon.clipPerVertex = &clipPerVertex[j];
				srcPolygon.flags = 0;
				srcPolygon.colors = specialClipcolours.GetData();
				srcPolygon.length = numOfVertices;
				//
				//--------------------------------
				// Point to the destination buffer
				//--------------------------------
				//
				dstPolygon.coords = clipBuffer[dstBuffer].coords.GetData();
				dstPolygon.colors = clipBuffer[dstBuffer].colors.GetData();
				dstPolygon.texCoords = clipBuffer[dstBuffer].texCoords.GetData();
				dstPolygon.clipPerVertex = clipBuffer[dstBuffer].clipPerVertex.GetData();
				dstPolygon.flags = srcPolygon.flags;
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
							for (k = 0; k < srcPolygon.length; k++)
							{
								k1 = (k + 1 < srcPolygon.length) ? k + 1 : 0;
								theTest = srcPolygon.clipPerVertex[k];
								//
								//----------------------------------------------------
								// If this vertex is inside the viewing space,
								// copy it directly to the clipping buffer
								//----------------------------------------------------
								//
								if (theTest.IsClipped(mask) == 0)
								{
									dstPolygon.coords[dstPolygon.length] = srcPolygon.coords[k];
									dstPolygon.clipPerVertex[dstPolygon.length] =
										srcPolygon.clipPerVertex[k];
									dstPolygon.colors[dstPolygon.length] = srcPolygon.colors[k];
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
								// throw this vertex away.  Since only one
								// clipping plane is involved, it must be in the
								// same space as the first vertex
								//---------------------------------------------------------
								//
								else if (srcPolygon.clipPerVertex[k1].IsClipped(mask) != 0)
								{
									_ASSERT(srcPolygon.clipPerVertex[k1].IsClipped(mask) == srcPolygon.clipPerVertex[k].IsClipped(mask));
									continue;
								}
								//
								//-------------------------------------------
								// Find the clipping interval from bc0 to bc1
								//-------------------------------------------
								//
								bc0 = GetBC(l, srcPolygon.coords[k]);
								bc1 = GetBC(l, srcPolygon.coords[k1]);
								_ASSERT(!Close_Enough(bc0, bc1));
								a = bc0 / (bc0 - bc1);
								_ASSERT(a >= 0.0f && a <= 1.0f);
								//
								//------------------------------
								// Lerp the homogeneous position
								//------------------------------
								//
								dstPolygon.coords[dstPolygon.length].Lerp(
									srcPolygon.coords[k], srcPolygon.coords[k1], a);
								DoCleanClipTrick(dstPolygon.coords[dstPolygon.length], l);
								//
								//----------------------------------------------------------
								// If there are colors, lerp them in screen
								// space for now as most cards do that anyway
								//----------------------------------------------------------
								//
								dstPolygon.colors[dstPolygon.length].Lerp(
									srcPolygon.colors[k], srcPolygon.colors[k1], a);
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
							//						srcPolygon.texCoords =
							// clipBuffer[dstBuffer].texCoords.GetData();
							srcPolygon.clipPerVertex =
								clipBuffer[dstBuffer].clipPerVertex.GetData();
							srcPolygon.length = dstPolygon.length;
							dstBuffer = !dstBuffer;
							dstPolygon.coords = clipBuffer[dstBuffer].coords.GetData();
							dstPolygon.colors = clipBuffer[dstBuffer].colors.GetData();
							//						dstPolygon.texCoords =
							// clipBuffer[dstBuffer].texCoords.GetData();
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
					theOr == theNewOr;
					loop++;
				} while (theNewOr != 0 && loop--);
				_ASSERT(theNewOr == 0);
				//
				//--------------------------------------------------
				// Move the most recent polygon into the clip buffer
				//--------------------------------------------------
				//
				for (k = 0; k < srcPolygon.length; k++)
				{
					int32_t clipped_index = myNumberUsedClipVertex + k;
					if (srcPolygon.coords[k].z == srcPolygon.coords[k].w)
					{
						srcPolygon.coords[k].z -= SMALL;
					}
					clipExtraCoords[clipped_index] = srcPolygon.coords[k];
					clipExtracolours[clipped_index] = srcPolygon.colors[k];
				}
				numberVerticesPerPolygon = srcPolygon.length;
			}
			clipExtraLength[myNumberUsedClipLength] = numberVerticesPerPolygon;
			myNumberUsedClipVertex += numberVerticesPerPolygon;
			myNumberUsedClipLength++;
			ret++;
			//					clip
			//					dont draw the original
			TurnInVisible(i);
#endif
		}
	}
#if EFECT_CLIPPED // this effect gets not clipped
	if (myNumberUsedClipLength > 0)
	{
		for (i = 0, j = 0; i < myNumberUsedClipLength; i++)
		{
			int32_t stride = clipExtraLength[i];
			for (k = 1; k < stride - 1; k++)
			{
				_ASSERT((vt->GetLast() + 3 + numGOSVertices) < vt.size());
				GOSCopyTriangleData(&gos_vertices[numGOSVertices], clipExtraCoords.GetData(),
					clipExtracolours.GetData(), j, j + k + 1, j + k, true);
#ifdef LAB_ONLY
				if (gShowClippedPolys)
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
#endif
				numGOSVertices += 3;
			}
			j += stride;
		}
	}
#endif
	vt->Increase(numGOSVertices);
	visible = uint32_t(numGOSVertices ? 1 : 0);
	if (visible)
	{
	}
	else
	{
	}
	return visible;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRNGonCloud::TestInstance(void) const
{
	if (usedNrOfNGons)
	{
		Check_Pointer(usedNrOfNGons);
		_ASSERT(*usedNrOfNGons <= maxNrOf);
	}
}

} // namespace MidLevelRenderer
