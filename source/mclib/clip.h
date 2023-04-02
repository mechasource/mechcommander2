//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------------------------------
// Screen Clip code for use with video cards without guard bands and BLADE.
//
// Generally, we will only need one or two of the below routines per triangle.
//
// If a triangle is completely on screen, none of the below should be called.
//
// Each triangle can be easily checked to see which of the below should be
// called in its case.  An easy, low-end machine optimization would be to
// NOT call any of the below for TGL/MSL shapes which are partly off screen
// and simply not to draw the shape.  Profiling will indicate if this is
// necessary.
//
// colour space clipping is HUGELY painful and we are cheating until it
// becomes necessary to do otherwise.
//
// Triangles will be converted into polygons with as many as five sides.
// It will be necessary in the draw code to hand the vertices down to
// GOS/BLADE in triangles to avoid any weirdness in their code!
//
// These routines are easily optimized and assembled if and when it becomes
// necessary to do so.  Routines should be inlined for speed!
//---------------------------------------------------------------------------------------------------
#ifndef CLIP_H
#define CLIP_H
//#include "gameos.hpp"

//---------------------------------------------------------------------------------------------------
inline void
xLeftEdgeclip(gos_VERTEX* vertices, uint32_t& numvertices, float leftEdge)
{
	//------------------------------------------
	// make local copy of all point data involved
	gos_VERTEX points[6];
	memcpy(points, vertices, sizeof(gos_VERTEX) * numvertices);
	//------------------------------------------
	uint32_t first = numvertices - 1;
	uint32_t second = 0;
	//----------------------------------------------
	// Index to current vertex in clipped polygon.
	//----------------------------------------------
	int32_t work = 0;
	//------------------------------
	// final number of polygon points
	//------------------------------
	int32_t n_points = 0;
	//----------------------------
	// process points in edge order
	//----------------------------
	for (size_t s = 0; s < numvertices; s++, second++)
	{
		float clipEdgeTest = points[first].x;
		//----------------------------------------------------------------
		// if this point is not beyond the clipping Edge, store it.
		//----------------------------------------------------------------
		if (clipEdgeTest >= leftEdge)
		{
			//------------------------------------------------------------
			gos_VERTEX* workv = &(vertices[work]);
			memcpy(workv, &(points[first]), sizeof(gos_VERTEX));
			//------------------------------------------------------------
			work++;
			n_points++;
		}
		//------------------------------------------------------------
		// if the line defined by first and second intersects the
		// clipping plane, generate a new point and calculate its info.
		//------------------------------------------------------------
		if (((clipEdgeTest < leftEdge) && (points[second].x >= leftEdge)) || ((clipEdgeTest >= leftEdge) && (points[second].x < leftEdge)))
		{
			gos_VERTEX* workv = &(vertices[work]);
			float sry = points[second].y;
			float srx = points[second].x;
			float srz = points[second].z;
			float srw = points[second].rhw;
			float sru = points[second].u;
			float srv = points[second].v;
			float fry = points[first].y;
			float frx = points[first].x;
			float frz = points[first].z;
			float frw = points[first].rhw;
			float fru = points[first].u;
			float frv = points[first].v;
			float mu = (leftEdge - srx) / (frx - srx);
			workv->x = leftEdge;
			workv->y = ((fry - sry) * mu) + sry;
			workv->z = ((frz - srz) * mu) + srz;
			// Clipping color is HUGELY painful at this point.  Try cheating
			// first!
			if (clipEdgeTest < leftEdge)
			{
				workv->argb = points[first].argb;
				workv->frgb = points[first].frgb;
			}
			else
			{
				workv->argb = points[second].argb;
				workv->frgb = points[second].frgb;
			}
			workv->rhw = ((frw - srw) * mu) + srw;
			workv->u = ((fru - sru) * mu) + sru;
			workv->v = ((frv - srv) * mu) + srv;
			work++;
			n_points++;
		}
		first = second;
	}
	numvertices = n_points;
}

//---------------------------------------------------------------------------------------------------
inline void
xRightEdgeclip(gos_VERTEX* vertices, uint32_t& numvertices, float rightEdge)
{
	//------------------------------------------
	// make local copy of all point data involved
	gos_VERTEX points[6];
	memcpy(points, vertices, sizeof(gos_VERTEX) * numvertices);
	//------------------------------------------
	uint32_t first = numvertices - 1;
	uint32_t second = 0;
	//----------------------------------------------
	// Index to current vertex in clipped polygon.
	//----------------------------------------------
	uint32_t work = 0;
	//------------------------------
	// final number of polygon points
	//------------------------------
	uint32_t n_points = 0;
	//----------------------------
	// process points in edge order
	//----------------------------
	for (size_t s = 0; s < numvertices; s++, second++)
	{
		float clipEdgeTest = points[first].x;
		//----------------------------------------------------------------
		// if this point is not beyond the clipping Edge, store it.
		//----------------------------------------------------------------
		if (clipEdgeTest < rightEdge)
		{
			//------------------------------------------------------------
			gos_VERTEX* workv = &(vertices[work]);
			memcpy(workv, &(points[first]), sizeof(gos_VERTEX));
			//------------------------------------------------------------
			work++;
			n_points++;
		}
		//------------------------------------------------------------
		// if the line defined by first and second intersects the
		// clipping plane, generate a new point and calculate its info.
		//------------------------------------------------------------
		if (((clipEdgeTest >= rightEdge) && (points[second].x < rightEdge)) || ((clipEdgeTest < rightEdge) && (points[second].x >= rightEdge)))
		{
			gos_VERTEX* workv = &(vertices[work]);
			float sry = points[second].y;
			float srx = points[second].x;
			float srz = points[second].z;
			float srw = points[second].rhw;
			float sru = points[second].u;
			float srv = points[second].v;
			float fry = points[first].y;
			float frx = points[first].x;
			float frz = points[first].z;
			float frw = points[first].rhw;
			float fru = points[first].u;
			float frv = points[first].v;
			float mu = (rightEdge - srx) / (frx - srx);
			workv->x = rightEdge - 1.0f;
			workv->y = ((fry - sry) * mu) + sry;
			workv->z = ((frz - srz) * mu) + srz;
			// Clipping color is HUGELY painful at this point.  Try cheating
			// first!
			if (clipEdgeTest >= rightEdge)
			{
				workv->argb = points[first].argb;
				workv->frgb = points[first].frgb;
			}
			else
			{
				workv->argb = points[second].argb;
				workv->frgb = points[second].frgb;
			}
			workv->rhw = ((frw - srw) * mu) + srw;
			workv->u = ((fru - sru) * mu) + sru;
			workv->v = ((frv - srv) * mu) + srv;
			work++;
			n_points++;
		}
		first = second;
	}
	numvertices = n_points;
}

//---------------------------------------------------------------------------------------------------
inline void
yTopEdgeclip(gos_VERTEX* vertices, uint32_t& numvertices, float topEdge)
{
	//------------------------------------------
	// make local copy of all point data involved
	gos_VERTEX points[6];
	memcpy(points, vertices, sizeof(gos_VERTEX) * numvertices);
	//------------------------------------------
	int32_t first = numvertices - 1;
	int32_t second = 0;
	//----------------------------------------------
	// Index to current vertex in clipped polygon.
	//----------------------------------------------
	int32_t work = 0;
	//------------------------------
	// final number of polygon points
	//------------------------------
	int32_t n_points = 0;
	//----------------------------
	// process points in edge order
	//----------------------------
	for (size_t s = 0; s < numvertices; s++, second++)
	{
		float clipEdgeTest = points[first].y;
		//----------------------------------------------------------------
		// if this point is not beyond the clipping Edge, store it.
		//----------------------------------------------------------------
		if (clipEdgeTest >= topEdge)
		{
			//------------------------------------------------------------
			gos_VERTEX* workv = &(vertices[work]);
			memcpy(workv, &(points[first]), sizeof(gos_VERTEX));
			//------------------------------------------------------------
			work++;
			n_points++;
		}
		//------------------------------------------------------------
		// if the line defined by first and second intersects the
		// clipping plane, generate a new point and calculate its info.
		//------------------------------------------------------------
		if (((clipEdgeTest < topEdge) && (points[second].y >= topEdge)) || ((clipEdgeTest >= topEdge) && (points[second].y < topEdge)))
		{
			gos_VERTEX* workv = &(vertices[work]);
			float sry = points[second].y;
			float srx = points[second].x;
			float srz = points[second].z;
			float srw = points[second].rhw;
			float sru = points[second].u;
			float srv = points[second].v;
			float fry = points[first].y;
			float frx = points[first].x;
			float frz = points[first].z;
			float frw = points[first].rhw;
			float fru = points[first].u;
			float frv = points[first].v;
			float mu = (topEdge - sry) / (fry - sry);
			workv->y = topEdge;
			workv->x = ((frx - srx) * mu) + srx;
			workv->z = ((frz - srz) * mu) + srz;
			// Clipping color is HUGELY painful at this point.  Try cheating
			// first!
			if (clipEdgeTest < topEdge)
			{
				workv->argb = points[first].argb;
				workv->frgb = points[first].frgb;
			}
			else
			{
				workv->argb = points[second].argb;
				workv->frgb = points[second].frgb;
			}
			workv->rhw = ((frw - srw) * mu) + srw;
			workv->u = ((fru - sru) * mu) + sru;
			workv->v = ((frv - srv) * mu) + srv;
			work++;
			n_points++;
		}
		first = second;
	}
	numvertices = n_points;
}

//---------------------------------------------------------------------------------------------------
inline void
yBottomEdgeclip(gos_VERTEX* vertices, uint32_t& numvertices, float bottomEdge)
{
	//------------------------------------------
	// make local copy of all point data involved
	gos_VERTEX points[6];
	memcpy(points, vertices, sizeof(gos_VERTEX) * numvertices);
	//------------------------------------------
	int32_t first = numvertices - 1;
	int32_t second = 0;
	//----------------------------------------------
	// Index to current vertex in clipped polygon.
	//----------------------------------------------
	int32_t work = 0;
	//------------------------------
	// final number of polygon points
	//------------------------------
	int32_t n_points = 0;
	//----------------------------
	// process points in edge order
	//----------------------------
	for (size_t s = 0; s < numvertices; s++, second++)
	{
		float clipEdgeTest = points[first].y;
		//----------------------------------------------------------------
		// if this point is not beyond the clipping Edge, store it.
		//----------------------------------------------------------------
		if (clipEdgeTest < bottomEdge)
		{
			//------------------------------------------------------------
			gos_VERTEX* workv = &(vertices[work]);
			memcpy(workv, &(points[first]), sizeof(gos_VERTEX));
			//------------------------------------------------------------
			work++;
			n_points++;
		}
		//------------------------------------------------------------
		// if the line defined by first and second intersects the
		// clipping plane, generate a new point and calculate its info.
		//------------------------------------------------------------
		if (((clipEdgeTest >= bottomEdge) && (points[second].y < bottomEdge)) || ((clipEdgeTest < bottomEdge) && (points[second].y >= bottomEdge)))
		{
			gos_VERTEX* workv = &(vertices[work]);
			float sry = points[second].y;
			float srx = points[second].x;
			float srz = points[second].z;
			float srw = points[second].rhw;
			float sru = points[second].u;
			float srv = points[second].v;
			float fry = points[first].y;
			float frx = points[first].x;
			float frz = points[first].z;
			float frw = points[first].rhw;
			float fru = points[first].u;
			float frv = points[first].v;
			float mu = (bottomEdge - sry) / (fry - sry);
			workv->y = bottomEdge - 1.0f;
			workv->x = ((frx - srx) * mu) + srx;
			workv->z = ((frz - srz) * mu) + srz;
			// Clipping color is HUGELY painful at this point.  Try cheating
			// first!
			if (clipEdgeTest >= bottomEdge)
			{
				workv->argb = points[first].argb;
				workv->frgb = points[first].frgb;
			}
			else
			{
				workv->argb = points[second].argb;
				workv->frgb = points[second].frgb;
			}
			workv->rhw = ((frw - srw) * mu) + srw;
			workv->u = ((fru - sru) * mu) + sru;
			workv->v = ((frv - srv) * mu) + srv;
			work++;
			n_points++;
		}
		first = second;
	}
	numvertices = n_points;
}

//---------------------------------------------------------------------------------------------------
#endif