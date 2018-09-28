/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
*******************************************************************************/
/*******************************************************************************
 3dprimitives.cpp  - D3D DrawPrimitive implementations

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include "stdinc.h"

#include <gameos.hpp>
#include <directx.hpp>
#include <dxrasterizer.hpp>
#include <debugger.hpp>
#include <3drasterizer.hpp>

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
typedef enum __3dprimitives_const
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
	uint32_t NumberVertices, puint16_t lpwIndices, uint32_t NumberIndices);
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray(pgos_VERTEX_2UV pVertexArray,
	uint32_t NumberVertices, puint16_t lpwIndices, uint32_t NumberIndices);
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray(pgos_VERTEX_3UV pVertexArray,
	uint32_t NumberVertices, puint16_t lpwIndices, uint32_t NumberIndices);
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
	size_t i;
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
			for (i = 0; i < NumVertices; ++i)
			{
				memcpy(&vVertices, &Vertices[i], sizeof(vVertices));
				vVertices.x = (float)(vVertices.x - (((double)DBMouseX - vVertices.x) * ZoomMode));
				vVertices.y = (float)(vVertices.y - (((double)DBMouseY - vVertices.y) * ZoomMode));
				wDrawPrimitive(d3dDevice7, D3DPT_POINTLIST, vertextype_unk1, &vVertices, 1, 0);
			}
		}
		else
		{
			wDrawPrimitive(d3dDevice7, D3DPT_POINTLIST, vertextype_unk1, Vertices, NumVertices, 0);
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
				vVertices1.x =
					(float)(vVertices1.x - (((double)DBMouseX - vVertices1.x) * (double)ZoomMode));
				vVertices1.y =
					(float)(vVertices1.y - (((double)DBMouseY - vVertices1.y) * (double)ZoomMode));
				vVertices2.x =
					(float)(vVertices2.x - (((double)DBMouseX - vVertices2.x) * (double)ZoomMode));
				vVertices2.y =
					(float)(vVertices2.y - (((double)DBMouseY - vVertices2.y) * (double)ZoomMode));
				wDrawPrimitive(d3dDevice7, D3DPT_LINELIST, vertextype_unk1, &vVertices1, 2u, 0);
			}
		}
		else
		{
			wDrawPrimitive(d3dDevice7, D3DPT_LINELIST, vertextype_unk1, Vertices, NumVertices, 0);
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
		// d3dDevice7->DrawPrimitive(
		// dptPrimitiveType,dvtVertexType,lpvVertices,dwVertexCount,dwFlags )
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
					d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk1, Vertices, NumVertices, 0);
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
						wDrawIndexedPrimitive(d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk1,
							Vertices, NumVertices, (puint16_t)&QuadIndex,
							NumVertices + (NumVertices >> 1), 0);
						NumVertices = 0;
					}
					else
					{
						wDrawIndexedPrimitive(d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk1,
							Vertices, 128u, (puint16_t)&QuadIndex, 192u, 0);
						NumVertices -= 128;
						Vertices += 128;
					}
				} while (NumVertices);
			}
			else
			{
				wDrawIndexedPrimitive(d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk1, Vertices,
					NumVertices, (puint16_t)&QuadIndex, NumVertices + (NumVertices >> 1), 0);
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
				d3dDevice7, D3DPT_TRIANGLESTRIP, vertextype_unk1, Vertices, NumVertices, 0);
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
		FanLength		   = (float)((double)NumVertices - 2 + FanLength);
		++FanCalls;
		if (RenderMode)
		{
			for (i = 0; i < NumVertices - 2; ++i)
				DebugTriangle(&Vertices[i + 1], &Vertices[i + 2], Vertices);
		}
		else
		{
			wDrawPrimitive(
				d3dDevice7, D3DPT_TRIANGLEFAN, vertextype_unk1, Vertices, NumVertices, 0);
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
/// <param name="NumberVertices"></param>
/// <param name="pwIndices"></param>
/// <param name="NumberIndices"></param>
/// <returns></returns>
MECH_IMPEXP HRESULT __stdcall gos_RenderIndexedArray1(
	pgos_VERTEX pVertexArray, uint32_t NumberVertices, puint16_t pwIndices, uint32_t NumberIndices)
{
	size_t k;
	size_t j;
	size_t i;
	gosASSERT(
		InsideBeginScene && NumberVertices > 0 && NumberIndices > 0 && (NumberIndices % 3) == 0);
	if (RenderDevice)
	{
		RenderDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertextype_unk1, pVertexArray,
			NumberVertices, pwIndices, NumberIndices, 0);
	}
	else
	{
		if (DirtyStates)
			FlushRenderStates();
		if (gShowVertexData)
		{
			for (i = 0; i < NumberIndices; i += 3)
				InternalFunctionSpew("GameOS_Direct3D", "Indices %d,%d,%d", pwIndices[i],
					pwIndices[i + 1], pwIndices[i + 2]);
		}
		for (j = 0; j < NumberIndices; ++j)
		{
			gosASSERT(pwIndices[j] < NumberVertices);
			CheckVertices(&pVertexArray[pwIndices[j]], 1u, 0);
		}
		if (!gDisablePrimitives || InDebugger)
		{
			PrimitivesRendered += NumberIndices / 3;
			IndexedTriangleLength = (float)(((double)NumberIndices / 3) + IndexedTriangleLength);
			++IndexedTriangleCalls;
			if (RenderMode)
			{
				for (k = 0; k < NumberIndices; k += 3)
					DebugTriangle(&pVertexArray[pwIndices[k]], &pVertexArray[pwIndices[k + 1]],
						&pVertexArray[pwIndices[k + 2]]);
			}
			else
			{
				wDrawIndexedPrimitive(d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk1, pVertexArray,
					NumberVertices, pwIndices, NumberIndices, 0);
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
	uint32_t NumberVertices, puint16_t pwIndices, uint32_t NumberIndices)
{
	size_t j;
	size_t i;
	gosASSERT(!RenderDevice && InsideBeginScene && NumberVertices > 0 && NumberIndices > 0 &&
		(NumberIndices % 3) == 0);
	if (DirtyStates)
		FlushRenderStates();
	for (i = 0; i < NumberIndices; ++i)
	{
		gosASSERT(pwIndices[i] < NumberVertices);
		CheckVertices2(&pVertexArray[pwIndices[i]], 1u);
	}
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered += NumberIndices / 3;
		IndexedTriangleLength = (float)(((double)NumberIndices / 3) + IndexedTriangleLength);
		++IndexedTriangleCalls;
		if (RenderMode)
		{
			for (j = 0; j < NumberIndices; j += 3)
				DebugTriangle_2UV(&pVertexArray[pwIndices[j]], &pVertexArray[pwIndices[j + 1]],
					&pVertexArray[pwIndices[j + 2]]);
		}
		else
		{
			wDrawIndexedPrimitive(d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk2, pVertexArray,
				NumberVertices, pwIndices, NumberIndices, 0);
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
	uint32_t NumberVertices, puint16_t pwIndices, uint32_t NumberIndices)
{
	size_t j;
	size_t i;
	gosASSERT(!RenderDevice && InsideBeginScene && NumberVertices > 0 && NumberIndices > 0 &&
		(NumberIndices % 3) == 0);
	if (DirtyStates)
		FlushRenderStates();
	for (i = 0; i < NumberIndices; ++i)
	{
		gosASSERT(pwIndices[i] < NumberVertices);
		CheckVertices3(&pVertexArray[pwIndices[i]], 1u);
	}
	if (!gDisablePrimitives || InDebugger)
	{
		PrimitivesRendered += NumberIndices / 3;
		IndexedTriangleLength = (float)(((double)NumberIndices / 3) + IndexedTriangleLength);
		++IndexedTriangleCalls;
		if (RenderMode)
		{
			for (j = 0; j < NumberIndices; j += 3)
				DebugTriangle_3UV(&pVertexArray[pwIndices[j]], &pVertexArray[pwIndices[j + 1]],
					&pVertexArray[pwIndices[j + 2]]);
		}
		else
		{
			wDrawIndexedPrimitive(d3dDevice7, D3DPT_TRIANGLELIST, vertextype_unk3, pVertexArray,
				NumberVertices, pwIndices, NumberIndices, 0);
			GameOSFPU(); // __asm { fninit }
		}
	}
	else
	{
	}
	// assembly says void
	return S_OK;
}
