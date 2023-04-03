/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted by Microsoft
*******************************************************************************/
/*******************************************************************************
 3dprimitives.cpp  - D3D DrawPrimitive implementations

 MechCommander 2 source code

 2014-07-24 Jerker Bäck, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "directx.hpp"
#include "dxrasterizer.hpp"
#include "debugger.hpp"
#include "3drasterizer.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data
static uint16_t SingleQuadIndexArray[] = {1, 0, 2, 3};
static float FanLength;
static uint32_t FanCalls;
static float StripLength;
static uint32_t StripCalls;

// unrecognized DirectX vertex type
enum class __3dprimitives_const
{
	vertextype_unk1 = 0x1C4,
	vertextype_unk2 = 0x2C4,
	vertextype_unk3 = 0x3C4,
};

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
#if _JUST_SOME_INFO
MECH_IMPEXP HRESULT __stdcall gos_DrawPoints(pgos_VERTEX Vertices, uint32_t NumVertices);
MECH_IMPEXP HRESULT __stdcall gos_DrawLines(pgos_VERTEX Vertices, uint32_t NumVertices);
MECH_IMPEXP HRESULT __stdcall gos_DrawTriangles(pgos_VERTEX Vertices, uint32_t NumVertices);
MECH_IMPEXP HRESULT __stdcall gos_DrawQuads(pgos_VERTEX Vertices, uint32_t NumVertices);
MECH_IMPEXP HRESULT __stdcall gos_DrawStrips(pgos_VERTEX Vertices, uint32_t NumVertices);
MECH_IMPEXP HRESULT __stdcall gos_DrawFans(pgos_VERTEX Vertices, uint32_t NumVertices);
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray(pgos_VERTEX pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices);
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray(pgos_VERTEX_2UV pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices);
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray(pgos_VERTEX_3UV pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices);
#endif

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

#pragma region gos_DrawPoints
// Draw points, pass a pointer to an array of gos_VERTEX's - every vertex is a
// new point
//  (you must set the texture to 0 if you want to disable texture mapping on the
//  points)
#pragma endregion local info
/// <summary>
/// <c>gos_DrawPoints</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="Vertices"></param>
/// <param name="NumVertices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_DrawPoints(pgos_VERTEX Vertices, uint32_t NumVertices)
{
	gos_VERTEX vVertices;
	gosASSERT(!RenderDevice && InsideBeginScene && Vertices && NumVertices);
	if (DirtyStates)
		FlushRenderStates();
	CheckVertices(Vertices, NumVertices, 1);
	if (!gDisableLinesPoints || InDebugger)
	{
		PrimitivesRendered += NumVertices;
		PointsRendered += NumVertices;
		if (ZoomMode)
		{
			for (auto i = 0u; i < NumVertices; ++i)
			{
				memcpy(&vVertices, &Vertices[i], sizeof(vVertices));
				vVertices.x = (float)(vVertices.x - (((double)DBMouseX - vVertices.x) * ZoomMode));
				vVertices.y = (float)(vVertices.y - (((double)DBMouseY - vVertices.y) * ZoomMode));
				wDrawPrimitive(d3ddevice7, D3DPT_POINTLIST, vertextype_unk1, &vVertices, 1, 0);
			}
		}
		else
		{
			wDrawPrimitive(d3ddevice7, D3DPT_POINTLIST, vertextype_unk1, Vertices, NumVertices, 0);
			GameOSFPU();
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}

#pragma region gos_DrawLines
// Draw lines, pass a pointer to an array of gos_VERTEX's - every two vertices
// is a new line
//  (you must set the texture to 0 if you want to disable texture mapping on the
//  points)
#pragma endregion local info
/// <summary>
/// <c>gos_DrawLines</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="Vertices"></param>
/// <param name="NumVertices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_DrawLines(pgos_VERTEX Vertices, uint32_t NumVertices)
{
	gos_VERTEX vVertices1;
	gos_VERTEX vVertices2;
	size_t i;
	gosASSERT(
		!RenderDevice && InsideBeginScene && Vertices && NumVertices && (NumVertices & 1) == 0);
	if (DirtyStates)
		FlushRenderStates();
	CheckVertices(Vertices, NumVertices, 1);
	if (!gDisableLinesPoints || InDebugger)
	{
		PrimitivesRendered += NumVertices >> 1;
		LinesRendered += NumVertices >> 1;
		if (ZoomMode)
		{
			for (i = 0; i < NumVertices; i += 2)
			{
				memcpy(&vVertices1, &Vertices[i], sizeof(vVertices1));
				memcpy(&vVertices2, &Vertices[i + 1], sizeof(vVertices2));
				vVertices1.x = (float)(vVertices1.x - (((double)DBMouseX - vVertices1.x) * (double)ZoomMode));
				vVertices1.y = (float)(vVertices1.y - (((double)DBMouseY - vVertices1.y) * (double)ZoomMode));
				vVertices2.x = (float)(vVertices2.x - (((double)DBMouseX - vVertices2.x) * (double)ZoomMode));
				vVertices2.y = (float)(vVertices2.y - (((double)DBMouseY - vVertices2.y) * (double)ZoomMode));
				wDrawPrimitive(d3ddevice7, D3DPT_LINELIST, vertextype_unk1, &vVertices1, 2u, 0);
			}
		}
		else
		{
			wDrawPrimitive(d3ddevice7, D3DPT_LINELIST, vertextype_unk1, Vertices, NumVertices, 0);
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}

#pragma region gos_DrawTriangles
// Draw triangles, pass a pointer to an array of gos_VERTEX's -
// every three vertices is a new triangle
#pragma endregion local info
/// <summary>
/// <c>gos_DrawTriangles</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="Vertices"></param>
/// <param name="NumVertices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_DrawTriangles(pgos_VERTEX Vertices, uint32_t NumVertices)
{
	size_t i;
	gosASSERT(InsideBeginScene && Vertices && NumVertices && (NumVertices % 3) == 0);
	if (RenderDevice)
	{
		// d3ddevice7->DrawPrimitive(
		// dptprimitivetype,dvtvertextype,pvvertices,vertexcount,flags )
		RenderDevice->DrawPrimitive(D3DPT_TRIANGLELIST, vertextype_unk1, Vertices, NumVertices, 0);
	}
	else
	{
		if (DirtyStates)
			FlushRenderStates();
		CheckVertices(Vertices, NumVertices, 0);
		if (!gDisablePrimitives || InDebugger)
		{
			PrimitivesRendered += NumVertices / 3;
			TrianglesRendered += NumVertices / 3;
			if (RenderMode)
			{
				for (i = 0; i < NumVertices; i += 3)
					DebugTriangle(&Vertices[i], &Vertices[i + 1], &Vertices[i + 2]);
			}
			else
			{
				wDrawPrimitive(
					d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk1, Vertices, NumVertices, 0);
				GameOSFPU(); // __asm { fninit }
			}
		}
		else
		{
		}
	}
	// assembly says void
	return S_OK;
}

#pragma region gos_DrawQuads
// Draw quads, pass a pointer to an array of gos_VERTEX's - every four vertices
// is a new quad
#pragma endregion local info
/// <summary>
/// <c>gos_DrawQuads</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="Vertices"></param>
/// <param name="NumVertices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_DrawQuads(pgos_VERTEX Vertices, uint32_t NumVertices)
{
	size_t i;
	gosASSERT(
		!RenderDevice && InsideBeginScene && Vertices && NumVertices && (NumVertices & 3) == 0);
	gosASSERT((NumVertices + (NumVertices >> 1)) < 128 * 6);
	if (DirtyStates)
		FlushRenderStates();
	CheckVertices(Vertices, NumVertices, 0);
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered += NumVertices >> 1;
		QuadsRendered += NumVertices >> 2;
		if (RenderMode)
		{
			for (i = 0; i < NumVertices; i += 4)
			{
				DebugTriangle(&Vertices[i], &Vertices[i + 1], &Vertices[i + 2]);
				DebugTriangle(&Vertices[i], &Vertices[i + 2], &Vertices[i + 3]);
			}
		}
		else
		{
			if (NumVertices >= 128)
			{
				do
				{
					if (NumVertices < 128)
					{
						wDrawIndexedPrimitive(d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk1,
							Vertices, NumVertices, (uint16_t*)&QuadIndex,
							NumVertices + (NumVertices >> 1), 0);
						NumVertices = 0;
					}
					else
					{
						wDrawIndexedPrimitive(d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk1,
							Vertices, 128u, (uint16_t*)&QuadIndex, 192u, 0);
						NumVertices -= 128;
						Vertices += 128;
					}
				} while (NumVertices);
			}
			else
			{
				wDrawIndexedPrimitive(d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk1, Vertices,
					NumVertices, (uint16_t*)&QuadIndex, NumVertices + (NumVertices >> 1), 0);
			}
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}

#pragma region gos_DrawStrips
// Draws a strip, pass a pointer to an array of gos_VERTEX's - every vertex
// after the 2nd is a new triangle
#pragma endregion local info
/// <summary>
/// <c>gos_DrawStrips</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="Vertices"></param>
/// <param name="NumVertices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_DrawStrips(pgos_VERTEX Vertices, uint32_t NumVertices)
{
	size_t i;
	gosASSERT(!RenderDevice && InsideBeginScene && Vertices && NumVertices >= 3);
	if (DirtyStates)
		FlushRenderStates();
	CheckVertices(Vertices, NumVertices, 0);
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered = PrimitivesRendered + NumVertices - 2;
		StripLength += (float)((double)NumVertices - 2 + StripLength);
		++StripCalls;
		if (RenderMode)
		{
			for (i = 0; i < NumVertices - 2; ++i)
				DebugTriangle(&Vertices[i], &Vertices[i + 1], &Vertices[i + 2]);
		}
		else
		{
			wDrawPrimitive(
				d3ddevice7, D3DPT_TRIANGLESTRIP, vertextype_unk1, Vertices, NumVertices, 0);
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}

#pragma region gos_DrawFans
// Draws a fan, pass a pointer to an array of gos_VERTEX's - every vertex after
// the 2nd is a new triangle
#pragma endregion local info
/// <summary>
/// <c>gos_DrawFans</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="Vertices"></param>
/// <param name="NumVertices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_DrawFans(pgos_VERTEX Vertices, uint32_t NumVertices)
{
	size_t i;
	gosASSERT(!RenderDevice && InsideBeginScene && Vertices && NumVertices >= 3);
	if (DirtyStates)
		FlushRenderStates();
	CheckVertices(Vertices, NumVertices, 0);
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered = PrimitivesRendered + NumVertices - 2;
		FanLength = (float)((double)NumVertices - 2 + FanLength);
		++FanCalls;
		if (RenderMode)
		{
			for (i = 0; i < NumVertices - 2; ++i)
				DebugTriangle(&Vertices[i + 1], &Vertices[i + 2], Vertices);
		}
		else
		{
			wDrawPrimitive(
				d3ddevice7, D3DPT_TRIANGLEFAN, vertextype_unk1, Vertices, NumVertices, 0);
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}

#pragma region gos_RenderIndexedArray
// This API allows you to pass an array of indices and an array of vertices to
// be rendered.
//
// The x,y positions must be already transformed into the viewport using the
// constants returned by gos_GetViewport.
//
// The vertex colors in the array may be changed in some wireframe modes.
// Otherwise the data is not altered.
#pragma endregion local info
/// <summary>
/// <c>gos_RenderIndexedArray</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pVertexArray"></param>
/// <param name="numbervertices"></param>
/// <param name="pwindices"></param>
/// <param name="numberindices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray1(
	pgos_VERTEX pVertexArray, uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices)
{
	size_t k;
	size_t j;
	size_t i;
	gosASSERT(
		InsideBeginScene && numbervertices > 0 && numberindices > 0 && (numberindices % 3) == 0);
	if (RenderDevice)
	{
		RenderDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertextype_unk1, pVertexArray,
			numbervertices, pwindices, numberindices, 0);
	}
	else
	{
		if (DirtyStates)
			FlushRenderStates();
		if (gShowVertexData)
		{
			for (i = 0; i < numberindices; i += 3)
				InternalFunctionSpew("GameOS_Direct3D", "Indices %d,%d,%d", pwindices[i],
					pwindices[i + 1], pwindices[i + 2]);
		}
		for (j = 0; j < numberindices; ++j)
		{
			gosASSERT(pwindices[j] < numbervertices);
			CheckVertices(&pVertexArray[pwindices[j]], 1u, 0);
		}
		if (!gDisablePrimitives || InDebugger)
		{
			PrimitivesRendered += numberindices / 3;
			IndexedTriangleLength = (float)(((double)numberindices / 3) + IndexedTriangleLength);
			++IndexedTriangleCalls;
			if (RenderMode)
			{
				for (k = 0; k < numberindices; k += 3)
					DebugTriangle(&pVertexArray[pwindices[k]], &pVertexArray[pwindices[k + 1]],
						&pVertexArray[pwindices[k + 2]]);
			}
			else
			{
				wDrawIndexedPrimitive(d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk1, pVertexArray,
					numbervertices, pwindices, numberindices, 0);
				GameOSFPU(); // __asm { fninit }
			}
		}
		else
		{
		}
	}
	// assembly says void
	return S_OK;
}

MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray2(pgos_VERTEX_2UV pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices)
{
	size_t j;
	size_t i;
	gosASSERT(!RenderDevice && InsideBeginScene && numbervertices > 0 && numberindices > 0 && (numberindices % 3) == 0);
	if (DirtyStates)
		FlushRenderStates();
	for (i = 0; i < numberindices; ++i)
	{
		gosASSERT(pwindices[i] < numbervertices);
		CheckVertices2(&pVertexArray[pwindices[i]], 1u);
	}
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered += numberindices / 3;
		IndexedTriangleLength = (float)(((double)numberindices / 3) + IndexedTriangleLength);
		++IndexedTriangleCalls;
		if (RenderMode)
		{
			for (j = 0; j < numberindices; j += 3)
				DebugTriangle_2UV(&pVertexArray[pwindices[j]], &pVertexArray[pwindices[j + 1]],
					&pVertexArray[pwindices[j + 2]]);
		}
		else
		{
			wDrawIndexedPrimitive(d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk2, pVertexArray,
				numbervertices, pwindices, numberindices, 0);
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}

MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray3(pgos_VERTEX_3UV pVertexArray,
	uint32_t numbervertices, uint16_t* pwindices, uint32_t numberindices)
{
	size_t j;
	size_t i;
	gosASSERT(!RenderDevice && InsideBeginScene && numbervertices > 0 && numberindices > 0 && (numberindices % 3) == 0);
	if (DirtyStates)
		FlushRenderStates();
	for (i = 0; i < numberindices; ++i)
	{
		gosASSERT(pwindices[i] < numbervertices);
		CheckVertices3(&pVertexArray[pwindices[i]], 1u);
	}
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered += numberindices / 3;
		IndexedTriangleLength = (float)(((double)numberindices / 3) + IndexedTriangleLength);
		++IndexedTriangleCalls;
		if (RenderMode)
		{
			for (j = 0; j < numberindices; j += 3)
				DebugTriangle_3UV(&pVertexArray[pwindices[j]], &pVertexArray[pwindices[j + 1]],
					&pVertexArray[pwindices[j + 2]]);
		}
		else
		{
			wDrawIndexedPrimitive(d3ddevice7, D3DPT_TRIANGLELIST, vertextype_unk3, pVertexArray,
				numbervertices, pwindices, numberindices, 0);
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}
