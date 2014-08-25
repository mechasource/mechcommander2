//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

/* this source is not used */

#include "stdafx.h"
#include "mlrheaders.hpp"
#include "mlrprimitive.hpp"

//#############################################################################
//###############################    ClipPolygon    ##################################
//#############################################################################

ClipPolygon::ClipPolygon()
{
	coords.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	colors.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	texCoords.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	clipPerVertex.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
}

//#############################################################################
//###############################    MLRPrimitive    ##################################
//#############################################################################

uint32_t Statistics::MLR_TransformedVertices = 0;
uint32_t Statistics::MLR_LitVertices = 0;
uint32_t Statistics::MLR_Primitives = 0;
uint32_t Statistics::MLR_NonClippedVertices = 0;
uint32_t Statistics::MLR_ClippedVertices = 0;
uint32_t Statistics::MLR_PrimitiveKB = 0;
uint32_t Statistics::MLR_PolysClippedButOutside = 0;
uint32_t Statistics::MLR_PolysClippedButInside = 0;
uint32_t Statistics::MLR_PolysClippedButOnePlane = 0;
uint32_t Statistics::MLR_PolysClippedButGOnePlane = 0;
gos_CycleData Statistics::MLR_ClipTime;
uint32_t Statistics::MLR_NumAllIndices = 1;
uint32_t Statistics::MLR_NumAllVertices = 1;
float Statistics::MLR_Index_Over_Vertex_Ratio = 1.0f;



MLRPrimitive::ClassData*
	MLRPrimitive::DefaultData = NULL;

DynamicArrayOf<MLRClippingState>
	MLRPrimitive::clipPerVertex;
DynamicArrayOf<Vector4D>
	MLRPrimitive::clipExtraCoords;

#if COLOR_AS_DWORD
DynamicArrayOf<uint32_t>
#else
DynamicArrayOf<RGBAColor>
#endif
	MLRPrimitive::clipExtraColors;

DynamicArrayOf<Stuff::Vector2DScalar>
	MLRPrimitive::clipExtraTexCoords;

DynamicArrayOf<int32_t>
	MLRPrimitive::clipExtraIndex;

DynamicArrayOf<uint16_t>
	MLRPrimitive::clipExtraLength;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::InitializeClass()
{
	Verify(!DefaultData);
	DefaultData =
		new ClassData(
			MLRPrimitiveClassID,
			"MidLevelRenderer::MLRPrimitive",
			Plug::DefaultData,
			NULL
		);
	Register_Object(DefaultData);
	
	clipPerVertex.SetLength(Limits::Max_Number_Vertices_Per_Mesh);
	clipExtraCoords.SetLength(Limits::Max_Number_Vertices_Per_Mesh);
	clipExtraColors.SetLength(Limits::Max_Number_Vertices_Per_Mesh);
	clipExtraTexCoords.SetLength(Limits::Max_Number_Vertices_Per_Mesh);
	clipExtraIndex.SetLength(Limits::Max_Number_Vertices_Per_Mesh);
	clipExtraLength.SetLength(Limits::Max_Number_Primitives_Per_Frame);

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::TerminateClass()
{
	clipPerVertex.SetLength(0);
	clipExtraCoords.SetLength(0);
	clipExtraColors.SetLength(0);
	clipExtraTexCoords.SetLength(0);
	clipExtraIndex.SetLength(0);
	clipExtraLength.SetLength(0);

	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitive::MLRPrimitive(
	ClassData *class_data,
	MemoryStream *stream,
	uint32_t version
):
	Plug(class_data)
{
	Check_Pointer(this);
	Check_Object(stream);


	switch(version)
	{
		case 1:
		{
			actualColors = &colors;

			MemoryStreamIO_Read(stream, &coords);
			numVertices = coords.GetLength();

			transformedCoords.SetLength(numVertices);
			litColors.SetLength(numVertices);

			int32_t i, len;
#if COLOR_AS_DWORD
			Stuff::DynamicArrayOf<Stuff::RGBAColor> rgbaColors;
			MemoryStreamIO_Read(stream, &rgbaColors);

			len = rgbaColors.GetLength();

			const Stuff::RGBAColor *data = rgbaColors.GetData();

			colors.SetLength(len);

			for(i=0;i<len;i++)
			{
				colors[i] = GOSCopyColor(data+i);
			}
#else
			MemoryStreamIO_Read(stream, &colors);
#endif
			MemoryStreamIO_Read(stream, &normals);
			MemoryStreamIO_Read(stream, &texCoords);

			*stream >> visible;

			Stuff::DynamicArrayOf<int32_t>	tempLengths;

			MemoryStreamIO_Read(stream, &tempLengths);

			len = tempLengths.GetLength();
			lengths.SetLength(len);

			for(i=0;i<len;i++)
			{
				lengths[i] = (uint8_t)(tempLengths[i] & 0xff);
			}

			*stream >> visible;

			*stream >> drawMode;

			referenceState.Load(stream, version);
		}
		break;
		case 2:
		{
			actualColors = &colors;

			MemoryStreamIO_Read(stream, &coords);
			numVertices = coords.GetLength();

			transformedCoords.SetLength(numVertices);
			litColors.SetLength(numVertices);

#if COLOR_AS_DWORD
			MemoryStreamIO_Read(stream, &colors);
#else
			Stuff::DynamicArrayOf<uint32_t> smallColors;

			MemoryStreamIO_Read(stream, &smallColors);
		
			int32_t i, len = smallColors.GetLength();

			colors.SetLength(len);

			uint32_t theColor;

			for(i=0;i<len;i++)
			{
				theColor = smallColors[i];

				colors[i].blue = (theColor & 0xff) * One_Over_256;

				theColor = theColor>>8;

				colors[i].green = (theColor & 0xff) * One_Over_256;

				theColor = theColor>>8;

				colors[i].red = (theColor & 0xff) * One_Over_256;

				theColor = theColor>>8;

				colors[i].alpha = (theColor & 0xff) * One_Over_256;
			}
#endif

			MemoryStreamIO_Read(stream, &normals);
			MemoryStreamIO_Read(stream, &texCoords);

			MemoryStreamIO_Read(stream, &lengths);

			*stream >> drawMode;

			referenceState.Load(stream, version);
		}
		break;
		default:
			STOP(("ERF-Version is newer than loader !"));
		break;
	}

	referenceCount = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::Save(MemoryStream *stream)
{
	Check_Object(this);
	Check_Object(stream);

	*stream << GetClassID();
	MemoryStreamIO_Write(stream, &coords);

#if COLOR_AS_DWORD
	MemoryStreamIO_Write(stream, &colors);
#else
	Stuff::DynamicArrayOf<uint32_t> smallColors;
	int32_t i, len = colors.GetLength();

	const Stuff::RGBAColor *data = colors.GetData();

	smallColors.SetLength(len);

	for(i=0;i<len;i++)
	{
		smallColors[i] = GOSCopyColor(data+i);
	}

	MemoryStreamIO_Write(stream, &smallColors);
#endif

	MemoryStreamIO_Write(stream, &normals);
	MemoryStreamIO_Write(stream, &texCoords);

//	*stream << visible; // changed from version 1 to 2

	MemoryStreamIO_Write(stream, &lengths);

//	*stream << visible; // changed from version 1 to 2

	*stream << static_cast<int32_t>(drawMode);

	referenceState.Save(stream);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitive::MLRPrimitive(ClassData *class_data):
	Plug(class_data), lengths(0),
	colors(0), normals(0), texCoords(0), coords(0)
{
	referenceState = 0;
	
	state = 0;

	numVertices = 0;	// number of verts for stats and vert arrays

	actualColors = &colors;

	referenceCount = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitive::~MLRPrimitive()
{
	Verify(referenceCount==0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::TestInstance(void) const
{
	Verify(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::InitializeDraw()
{
#ifdef LAB_ONLY
	Statistics::MLR_NumAllIndices = 1;
	Statistics::MLR_NumAllVertices = 1;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::InitializeDrawPrimitive(int32_t vis, int32_t)
{
	gos_vertices = NULL;
	numGOSVertices = -1;

	visible = vis;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::SetSubprimitiveLengths(puint8_t data, int32_t l)
{
	Check_Object(this); 
	lengths.AssignData(data, l);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
	MLRPrimitive::GetSubprimitiveLengths(puint8_t *data)
{
	Check_Object(this); 
	*data = lengths.GetData();
	return lengths.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
	MLRPrimitive::GetSubprimitiveLength (int32_t i) const
{ 
	Check_Object(this); 
	return (lengths.GetLength() > 0 ? abs(lengths[i]) : 1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::SetCoordData(
		const Point3D *data,
		int32_t dataSize
	)
{
	Check_Object(this); 
	Check_Pointer(data);

	Verify(colors.GetLength() == 0 || dataSize == colors.GetLength());
	Verify(normals.GetLength() == 0 || dataSize == normals.GetLength());
	Verify(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());

#if defined (MAX_NUMBER_VERTICES)
	Verify(dataSize <= MAX_NUMBER_VERTICES);
#endif
	coords.AssignData(data, dataSize);
	transformedCoords.SetLength(dataSize);
	numVertices = dataSize;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::GetCoordData(
		Point3D **data,
		pint32_t dataSize
	)
{
	Check_Object(this);

	*data = coords.GetData();
	*dataSize = coords.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::SetColorData(
#if COLOR_AS_DWORD
		pcuint32_t data,
#else
		const RGBAColor *data,
#endif
		int32_t dataSize
	)
{
	Check_Object(this); 
	Check_Pointer(data);

	Verify(coords.GetLength() == 0 || dataSize == coords.GetLength());
	Verify(normals.GetLength() == 0 || dataSize == normals.GetLength());
	Verify(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());

	colors.AssignData(data, dataSize);
	litColors.SetLength(dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::GetColorData(
#if COLOR_AS_DWORD
		uint32_t **data,
#else
		RGBAColor **data,
#endif
		pint32_t dataSize
	)
{
	Check_Object(this); 
	*data = colors.GetData();
	*dataSize = colors.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::SetNormalData(
		const Vector3D *data,
		int32_t dataSize
	)
{
	Check_Object(this); 
	Check_Pointer(data);

	Verify(coords.GetLength() == 0 || dataSize == coords.GetLength());
	Verify(colors.GetLength() == 0 || dataSize == colors.GetLength());
	Verify(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());

	normals.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::GetNormalData(
		Vector3D **data,
		pint32_t dataSize
	)
{
	Check_Object(this); 
	*data = normals.GetData();
	*dataSize = normals.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::SetTexCoordData(
		const Stuff::Vector2DScalar *data,
		int32_t dataSize
	)
{
	Check_Object(this); 
	Check_Pointer(data);

	Verify(coords.GetLength() == 0 || dataSize == coords.GetLength());
	Verify(colors.GetLength() == 0 || dataSize == colors.GetLength());
	Verify(normals.GetLength() == 0 || dataSize == normals.GetLength());

	texCoords.AssignData((Vector2DScalar *)data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::GetTexCoordData(
		Vector2DScalar **data,
		pint32_t dataSize
	)
{
	Check_Object(this); 
	*data = texCoords.GetData();
	*dataSize = texCoords.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::Transform(Matrix4D *mat)
{
	Check_Object(this);

	int32_t i, len = coords.GetLength();

	for(i=0;i<len;i++)
	{
		transformedCoords[i].Multiply(coords[i], *mat);
	}

	#ifdef LAB_ONLY
		Statistics::MLR_TransformedVertices += len;
	#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::Lighting (
		MLRLight **lights,
		uint32_t nrLights
	)
{
	Check_Object(this);

	// set the to use colors to the original colors ...
	// only lighting could overwrite this;
	actualColors = &colors;

	if(nrLights == 0)
	{
		return;
	}

	if(normals.GetLength() == 0)
	{
		return;
	}

	if(lights == NULL)
	{
		return;
	}

	switch (GetCurrentState().GetLightingMode())
	{
		case MLRState::LightingOffMode:
			return;
		case MLRState::LightingClassicOnlyMode:
		{
			Verify(colors.GetLength() == litColors.GetLength());
			Verify(normals.GetLength() == colors.GetLength());
			Verify(coords.GetLength() == colors.GetLength());

			int32_t i, k, len = colors.GetLength();

			MLRVertexData vertexData;

#if COLOR_AS_DWORD
			TO_DO;
#else
			RGBAColor *color = &colors[0];
			RGBAColor *litColor = &litColors[0];

#if USE_ASSEMBLER_CODE
			_asm {
				push		esi
				push		edi

				mov			esi, color

				mov			edi, litColor
				mov         ecx, len

			_loop1:

				mov			eax, dword ptr [esi]
				mov			ebx, dword ptr [esi+4]

				mov			dword ptr [edi], eax
				mov			dword ptr [edi+ 4], ebx

				mov			eax, dword ptr [esi + 8]
				mov			ebx, dword ptr [esi + 12]

				mov			dword ptr [edi + 8], eax
				mov			dword ptr [edi + 12], ebx

				add			esi,16
				add			edi,16

				dec			ecx
				jnz			_loop1

				pop			edi
				pop			esi
			}
#else	// it doesnt know that ...
			memcpy(litColor, color, (len<<2)*sizeof(Scalar));
#endif
#endif

			//
			//-----------------------------------
			// Test each light against the vertex
			//-----------------------------------
			//
			for (i=0;i<nrLights;i++)
			{
				MLRLight *light = lights[i];

				Check_Object(light);


				vertexData.point = &coords[0];
				vertexData.color = &litColors[0];
				vertexData.normal = &normals[0];

				for(k=0;k<len;k++)
				{
					light->LightVertex(vertexData);

					vertexData.point++;
					vertexData.color++;
					vertexData.normal++;
				}
			}

			#ifdef LAB_ONLY
				Statistics::MLR_LitVertices += len*nrLights;
			#endif

			// set the to use colors to the original colors ...
			// only lighting could overwrite this;
			actualColors = &litColors;
		}
		break;
		case MLRState::LightingLightMapOnlyMode:
		{
			Verify(state.GetAlphaMode() == MLRState::OneZeroMode);
			STOP(("Lightmaps not implemented yet."));
		}
		break;
		case MLRState::LightingClassicAndLightMapMode:
		{
			Verify(state.GetAlphaMode() == MLRState::OneZeroMode);

			Verify(colors.GetLength() == litColors.GetLength());
			Verify(normals.GetLength() == colors.GetLength());
			Verify(coords.GetLength() == colors.GetLength());

			int32_t i, k, len = colors.GetLength();

			MLRVertexData vertexData;

#if COLOR_AS_DWORD
#else
			RGBAColor *color = &colors[0];
			RGBAColor *litColor = &litColors[0];

#if USE_ASSEMBLER_CODE
			_asm {
				push		esi
				push		edi

				mov			esi, color

				mov			edi, litColor
				mov         ecx, len

			_loop2:

				mov			eax, dword ptr [esi]
				mov			ebx, dword ptr [esi+4]

				mov			dword ptr [edi], eax
				mov			dword ptr [edi+ 4], ebx

				mov			eax, dword ptr [esi + 8]
				mov			ebx, dword ptr [esi + 12]

				mov			dword ptr [edi + 8], eax
				mov			dword ptr [edi + 12], ebx

				add			esi,16
				add			edi,16

				dec			ecx
				jnz			_loop2

				pop			edi
				pop			esi
			}
#else	// it doesnt know that ...
			memcpy(litColor, color, (len<<2)*sizeof(Scalar));
#endif
#endif

			//
			//-----------------------------------
			// Test each light against the vertex
			//-----------------------------------
			//
			for (i=0;i<nrLights;i++)
			{
				MLRLight *light = lights[i];

				Check_Object(light);


				vertexData.point = &coords[0];
				vertexData.color = &litColors[0];
				vertexData.normal = &normals[0];

				for(k=0;k<len;k++)
				{
					light->LightVertex(vertexData);

					vertexData.point++;
					vertexData.color++;
					vertexData.normal++;
				}
			}

			#ifdef LAB_ONLY
				Statistics::MLR_LitVertices += len*nrLights;
			#endif

			// set the to use colors to the original colors ...
			// only lighting could overwrite this;
			actualColors = &litColors;

			STOP(("Lightmaps not implemented yet."));
		}
		break;
	}

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRPrimitive::PaintMe(const Stuff::RGBAColor *paintMe)
{
	Check_Object(this);

	Verify(colors.GetLength() == litColors.GetLength());

	int32_t k, len = litColors.GetLength();

#if COLOR_AS_DWORD
	uint32_t argb = GOSCopyColor(paintMe);

	for(k=0;k<len;k++)
	{
		litColors[k] = argb;
	}
#else
	for(k=0;k<len;k++)
	{
		litColors[k] = *paintMe;
	}
#endif
	// set the to use colors to the original colors ...
	// only lighting could overwrite this;
	actualColors = &litColors;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
	MLRPrimitive::CastRay(
		Line3D *line,
		Normal3D *normal
	)
{
	Check_Object(this);
	Check_Object(line);
	Check_Pointer(normal);
	STOP(("Not implemented"));
	return false;
}

//#############################################################################
//#######################    MLRPrimitive__ClassData    #######################
//#############################################################################

void
	MLRPrimitive__ClassData::TestInstance()
{
	Verify(IsDerivedFrom(MLRPrimitive::DefaultData));
}
