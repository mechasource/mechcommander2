//---------------------------------------------------------------------------
//
// Vertex.h -- File contains class definitions for the Terrain Vertices
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef VERTEX_H
#define VERTEX_H
//---------------------------------------------------------------------------
// Include Files
#ifndef DVERTEX_H
#include "dvertex.h"
#endif

#include <stuff/stuff.hpp>

//---------------------------------------------------------------------------
// Macro Definitions
#define BOTTOMRIGHT			0
#define BOTTOMLEFT			1

#ifndef NO_ERROR
#define NO_ERROR				0
#endif

//---------------------------------------------------------------------------
// Classes
struct PostcompVertex
{
	//---------------------------------------------------
	// This Structure is used to store the data the 3D
	// version of the terrain uses so I don't have to 
	// recalculate it every frame!
	//
	// Replaces the pVertex pointer in Vertex Class
	
	Stuff::Vector3D		vertexNormal;		//Used for lighting

	float				elevation;			//Stored here so terrain can be locally deformed

	uint32_t				textureData;		//Top word is Overlay TXM, Bottom Word is Base TXM

	uint32_t				localRGBLight;		//aRGB format

	uint32_t				terrainType;		//terrainTypeNumber.
	uint8_t				selected;			// selection
	uint8_t				water;				//Additional Storage to pull into 16 Byte Alignment
	uint8_t				shadow;
	uint8_t				highlighted;		//Used to highlight WHOLE FACES!!!

		float getElevation (void)
		{
			return elevation;
		}

	PostcompVertex& operator=( const PostcompVertex& src );
	PostcompVertex( const PostcompVertex& );
	PostcompVertex(void);
};

typedef PostcompVertex *PostcompVertexPtr;

//---------------------------------------------------------------------------
class Vertex
{
	//Data Members
	//-------------
	public:

		PostcompVertexPtr 	pVertex;			//Pointer to PostcompVertex for this Vertex
		float				vx,vy;				//Unrotated World Coordinates of Vertex
		float				px,py;				//Screen Coordinates of vertex.
		float				pz,pw;				//Depth of vertex.
		
		int32_t				vertexNum;			//Physical Vertex Position in mapData

												//Used by new Object positioning system.
		int32_t				blockVertex;		//What terrain block is this vertex part of.vertexNumber;		
												//What vertex number in the block

		int32_t				posTile;			//Where are we on the tile!  Saves 24 divides per tile if overlay on tile!!!
												//Saves a mere 8 if no overlay!

		uint32_t				clipInfo;			//Stores data on vertex clip information.

		uint32_t				lightRGB;			//Light at this vertex.
		uint32_t				fogRGB;				//Fog at this vertex.

		float				wx,wy;				//Screen Coordinates of water face if there is one!
		float				wz,ww;				//Depth of vertex for water if there is one!
		float				wAlpha;				//Used to environment Map Sky onto water.

		uint32_t				calcThisFrame;		//Calced this vertex this frame?

		float				hazeFactor;			//Used to distance fog the terrain.
		
#ifdef _DEBUG
		bool				selected;			//Debug to show which triangle I am on.
#endif

	//Member Functions
	//-----------------
	public:

		void init (void)
		{
			pVertex = nullptr;
			px = py = 0.0f;
			pz = pw = 0.0f;

			wx = wy = 0.0f;
			wz = ww = 0.0f;

			posTile = -1;
			
			vertexNum = -1;

			lightRGB = fogRGB = 0xffffffff;
			calcThisFrame = false;
			
		}

		Vertex (void)
		{
			init(void);
		}

		void destroy (void)
		{
		}

		~Vertex (void)
		{
			destroy(void);
		}

		int32_t init (PostcompVertexPtr preVertex)
		{
			init(void);
			pVertex = preVertex;
			
			return(NO_ERROR);
		}
		
		int32_t getBlockNumber (void)
		{
			return (blockVertex>>16);
		}
		
		int32_t getVertexNumber (void)
		{
			return (blockVertex & 0x0000ffff);
		}
};

//---------------------------------------------------------------------------
extern int32_t numTerrainFaces;

//---------------------------------------------------------------------------
#endif
