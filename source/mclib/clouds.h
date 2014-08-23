//---------------------------------------------------------------------------
//
// Clouds.h -- File contains class definitions for the Clouds
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CLOUDS_H
#define CLOUDS_H
//---------------------------------------------------------------------------
// Include Files
#ifndef DCAMERA_H
#include "dcamera.h"
#endif

#ifndef GAMEOS_HPP
#include <gameos.hpp>
#endif

#ifndef STUFF_HPP
#include <stuff/stuff.hpp>
#endif

//---------------------------------------------------------------------------
// Macro Definitions

typedef struct _CloudVertex
{
	float				vx,vy;				//Unrotated World Coordinates of Vertex
	float				px,py;				//Screen Coordinates of vertex.
	float				pz,pw;				//Depth of vertex.
	float				pu,pv;				//Scrolling UVs
	bool				clipInfo;			//Is this vertex visible
	ULONG				fogRGB;				//Haze ULONG
} CloudVertex;

typedef CloudVertex *CloudVertexPtr;

//---------------------------------------------------------------------------
class Clouds
{
	// Data Members
	//--------------
	protected:
		ULONG			mcTextureNodeIndex;			//Pointer to MCTextureNode which is used to cache handles if necessary
		ULONG			gosTextureHandle;
		bool			renderClouds;
		int32_t			gridSize;
		float			scrollU;
		float			scrollV;
		
		CloudVertexPtr	cloudVertices;

	public:

		void init (void)
		{
			mcTextureNodeIndex = gosTextureHandle = 0xffffffff;
			
			renderClouds = false;
			
			scrollU = scrollV = 0.0f;
			
			cloudVertices = NULL;
			gridSize = 0;
		}
		
		Clouds (void)
		{
			init();
		}
		
		void destroy (void);
		
		~Clouds (void)
		{
			destroy();
		}
		
		void init (PSTR textureName, int32_t gSize);
		
		void update (void);
		void render (void);
};

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
//---------------------------------------------------------------------------

