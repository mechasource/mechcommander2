//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrprimitivebase.h"

namespace MidLevelRenderer {

//#############################################################################

size_t clipTrick[6][2] = {{1, 1}, {1, 0}, {0, 1}, {0, 0}, {2, 0}, {2, 1}};

//#############################################################################
//#########################    ClipPolygon2    ############################
//#############################################################################

void
ClipPolygon2::Init(int32_t passes)
{
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	coords.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	colors.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	texCoords.SetLength(passes * Limits::Max_Number_Vertices_Per_Polygon);
	clipPerVertex.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
}

void
ClipPolygon2::Destroy()
{
	coords.SetLength(0);
	colors.SetLength(0);
	texCoords.SetLength(0);
	clipPerVertex.SetLength(0);
}

//#############################################################################
//#########################    MLRPrimitiveBase    ############################
//#############################################################################

MLRPrimitiveBase::ClassData* MLRPrimitiveBase::DefaultData = nullptr;

std::vector<Vector4D>* MLRPrimitiveBase::transformedCoords;

std::vector<MLRClippingState>* MLRPrimitiveBase::clipPerVertex;
std::vector<Vector4D>* MLRPrimitiveBase::clipExtraCoords;

std::vector<Stuff::Vector2DScalar>* MLRPrimitiveBase::clipExtraTexCoords;

#if COLOR_AS_DWORD
std::vector<uint32_t>
#else
std::vector<RGBAcolour>
#endif
	* MLR_I_C_PMesh::clipExtracolours;

std::vector<uint16_t>* MLRPrimitiveBase::clipExtraLength;

ClipPolygon2* MLRPrimitiveBase::clipBuffer;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLRPrimitiveBaseClassID, "MidLevelRenderer::MLRPrimitiveBase",
		RegisteredClass::DefaultData, nullptr);
	Register_Object(DefaultData);
	transformedCoords = new std::vector<Vector4D>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(transformedCoords);
	clipPerVertex = new std::vector<MLRClippingState>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipPerVertex);
	clipExtraCoords = new std::vector<Vector4D>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipExtraCoords);
	clipExtraTexCoords =
		new std::vector<Stuff::Vector2DScalar>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipExtraTexCoords);
	clipExtracolours = new std::vector<
#if COLOR_AS_DWORD
		uint32_t
#else
		RGBAcolour
#endif
		>(Limits::Max_Number_Primitives_Per_Frame);
	Register_Object(clipExtracolours);
	clipExtraLength = new std::vector<uint16_t>(Limits::Max_Number_Primitives_Per_Frame);
	Register_Object(clipExtraLength);
	clipBuffer = new ClipPolygon2[2];
	Register_Pointer(clipBuffer);
	clipBuffer[0].Init(Limits::Max_Number_Of_Multitextures);
	clipBuffer[1].Init(Limits::Max_Number_Of_Multitextures);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::TerminateClass()
{
	clipBuffer[1].Destroy();
	clipBuffer[0].Destroy();
	Unregister_Pointer(clipBuffer);
	delete[] clipBuffer;
	Unregister_Object(transformedCoords);
	delete transformedCoords;
	Unregister_Object(clipPerVertex);
	delete clipPerVertex;
	Unregister_Object(clipExtraCoords);
	delete clipExtraCoords;
	Unregister_Object(clipExtraTexCoords);
	delete clipExtraTexCoords;
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
MLRPrimitiveBase::MLRPrimitiveBase(ClassData* class_data, std::iostream stream, uint32_t version) :
	RegisteredClass(class_data)
{
	// Check_Pointer(this);
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	switch (version)
	{
	case 1:
	case 2:
	{
		STOP(("This class got created only after version 2 !"));
	}
	break;
	default:
	{
		MemoryStreamIO_Read(stream, &coords);
		MemoryStreamIO_Read(stream, &texCoords);
		MemoryStreamIO_Read(stream, &lengths);
		*stream >> drawMode;
		referenceState.Load(stream, version);
	}
	break;
	}
	passes = 1;
	referenceCount = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	*stream << GetClassID();
	MemoryStreamIO_Write(stream, &coords);
	MemoryStreamIO_Write(stream, &texCoords);
	MemoryStreamIO_Write(stream, &lengths);
	*stream << static_cast<int32_t>(drawMode);
	referenceState.Save(stream);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitiveBase::MLRPrimitiveBase(ClassData* class_data) :
	RegisteredClass(class_data), lengths(0), texCoords(0), coords(0)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	referenceState = 0;
	state = 0;
	passes = 1;
	referenceCount = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitiveBase::~MLRPrimitiveBase()
{
	_ASSERT(referenceCount == 0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::InitializeDraw()
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::InitializeDrawPrimitive(uint8_t vis, int32_t)
{
	gos_vertices = nullptr;
	numGOSVertices = -1;
	visible = vis;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::SetSubprimitiveLengths(uint8_t* data, int32_t l)
{
	// Check_Object(this);
	lengths.AssignData(data, l);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::GetSubprimitiveLengths(uint8_t** data, int32_t* len)
{
	// Check_Object(this);
	*data = lengths.GetData();
	*len = lengths.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRPrimitiveBase::GetSubprimitiveLength(int32_t i) const
{
	// Check_Object(this);
	return (lengths.GetLength() > 0 ? abs(lengths[i]) : 1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::SetCoordData(const Point3D* data, size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	_ASSERT(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
#if defined(MAX_NUMBER_VERTICES)
	_ASSERT(dataSize <= MAX_NUMBER_VERTICES);
#endif
	coords.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::GetCoordData(Point3D** data, size_t* dataSize)
{
	// Check_Object(this);
	*data = coords.GetData();
	*dataSize = coords.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::SetTexCoordData(const Stuff::Vector2DScalar* data, size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	_ASSERT(coords.GetLength() == 0 || dataSize == coords.GetLength());
	texCoords.AssignData((Vector2DScalar*)data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::GetTexCoordData(Stuff::Vector2DScalar** data, size_t* dataSize)
{
	// Check_Object(this);
	*data = texCoords.GetData();
	*dataSize = texCoords.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::Transform(Matrix4D* mat)
{
	// Check_Object(this);
	int32_t i, len = coords.GetLength();
	for (i = 0; i < len; i++)
	{
		(*transformedCoords)[i].Multiply(coords[i], *mat);
	}
#ifdef LAB_ONLY
	Set_Statistic(TransformedVertices, TransformedVertices + len);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPrimitiveBase::GetExtend(Stuff::ExtentBox* box)
{
	// Check_Object(this);
	Check_Object(box);
	if (coords.GetLength() == 0)
	{
		return;
	}
	box->minX = box->maxX = coords[0].x;
	box->minY = box->maxY = coords[0].y;
	box->minZ = box->maxZ = coords[0].z;
	for (size_t i = 0; i < coords.GetLength(); i++)
	{
		if (coords[i].x < box->minX)
		{
			box->minX = coords[i].x;
		}
		if (coords[i].y < box->minY)
		{
			box->minY = coords[i].y;
		}
		if (coords[i].z < box->minZ)
		{
			box->minZ = coords[i].z;
		}
		if (coords[i].x > box->maxX)
		{
			box->maxX = coords[i].x;
		}
		if (coords[i].y > box->maxY)
		{
			box->maxY = coords[i].y;
		}
		if (coords[i].z > box->maxZ)
		{
			box->maxZ = coords[i].z;
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
MLRPrimitiveBase::CastRay(Line3D* line, Normal3D* normal)
{
	// Check_Object(this);
	Check_Object(line);
	Check_Pointer(normal);
	STOP(("Not implemented"));
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MidLevelRenderer::CreateIndexedIcosahedron(
	IcoInfo& icoInfo, std::vector<MLRState>* states)
{
	switch (icoInfo.type)
	{
	case MLR_I_PMeshClassID:
		return CreateIndexedIcosahedron_Nocolour_NoLit(icoInfo, &(*states)[0]);
		break;
	case MLR_I_C_PMeshClassID:
		return CreateIndexedIcosahedron_colour_NoLit(icoInfo, &(*states)[0]);
		break;
	case MLR_I_L_PMeshClassID:
		return CreateIndexedIcosahedron_colour_Lit(icoInfo, &(*states)[0]);
		break;
	case MLR_I_DT_PMeshClassID:
		return CreateIndexedIcosahedron_Nocolour_NoLit_2Tex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_C_DT_PMeshClassID:
		return CreateIndexedIcosahedron_colour_NoLit_2Tex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_L_DT_PMeshClassID:
		return CreateIndexedIcosahedron_colour_Lit_2Tex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_MT_PMeshClassID:
		return CreateIndexedIcosahedron_Nocolour_NoLit_MultiTexture(icoInfo, states);
		break;
	case MLR_I_DeT_PMeshClassID:
		return CreateIndexedIcosahedron_Nocolour_NoLit_DetTex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_C_DeT_PMeshClassID:
		return CreateIndexedIcosahedron_colour_NoLit_DetTex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_L_DeT_PMeshClassID:
		return CreateIndexedIcosahedron_colour_Lit_DetTex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_TMeshClassID:
		return CreateIndexedTriIcosahedron_Nocolour_NoLit(icoInfo, &(*states)[0]);
		break;
	case MLR_I_C_TMeshClassID:
		return CreateIndexedTriIcosahedron_colour_NoLit(icoInfo, &(*states)[0]);
		break;
	case MLR_I_L_TMeshClassID:
		return CreateIndexedTriIcosahedron_colour_Lit(icoInfo, &(*states)[0]);
		break;
	case MLR_I_DeT_TMeshClassID:
		return CreateIndexedTriIcosahedron_Nocolour_NoLit_DetTex(
			icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_C_DeT_TMeshClassID:
		return CreateIndexedTriIcosahedron_colour_NoLit_DetTex(
			icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_L_DeT_TMeshClassID:
		return CreateIndexedTriIcosahedron_colour_Lit_DetTex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_DT_TMeshClassID:
		return CreateIndexedTriIcosahedron_Nocolour_NoLit_2Tex(
			icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_C_DT_TMeshClassID:
		return CreateIndexedTriIcosahedron_colour_NoLit_2Tex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_I_L_DT_TMeshClassID:
		return CreateIndexedTriIcosahedron_colour_Lit_2Tex(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	case MLR_TerrainClassID:
		return CreateIndexedTriIcosahedron_TerrainTest(icoInfo, &(*states)[0], &(*states)[1]);
		break;
	}
	return nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
const std::wstring_view&
MidLevelRenderer::IcoInfo::GetTypeName()
{
	switch (type)
	{
	case MLR_I_PMeshClassID:
		return "not colored, unlit mesh";
		break;
	case MLR_I_C_PMeshClassID:
		return "colored, unlit mesh";
		break;
	case MLR_I_L_PMeshClassID:
		return "colored, lit mesh";
		break;
	case MLR_I_DT_PMeshClassID:
		return "not colored, unlit mesh w/ 2 Tex";
		break;
	case MLR_I_C_DT_PMeshClassID:
		return "colored, unlit mesh w/ 2 Tex";
		break;
	case MLR_I_L_DT_PMeshClassID:
		return "colored, lit mesh w/ 2 Tex";
		break;
	case MLR_I_MT_PMeshClassID:
		return "not colored, unlit mesh w/ 4 Tex";
		break;
	case MLR_I_DeT_PMeshClassID:
		return "not colored, unlit mesh w/ DetTex";
		break;
	case MLR_I_C_DeT_PMeshClassID:
		return "colored, unlit mesh w/ DetTex";
		break;
	case MLR_I_L_DeT_PMeshClassID:
		return "colored, lit mesh w/ DetTex";
		break;
	case MLR_I_TMeshClassID:
		return "not colored, unlit tri. mesh";
		break;
	case MLR_I_C_TMeshClassID:
		return "colored, unlit tri. mesh";
		break;
	case MLR_I_L_TMeshClassID:
		return "colored, lit tri. mesh";
		break;
	case MLR_I_DeT_TMeshClassID:
		return "not colored, unlit tri. mesh w/ DetTex";
		break;
	case MLR_I_C_DeT_TMeshClassID:
		return "colored, unlit tri. mesh w/ DetTex";
		break;
	case MLR_I_L_DeT_TMeshClassID:
		return "colored, lit tri. mesh w/ DetTex";
		break;
	case MLR_I_DT_TMeshClassID:
		return "not colored, unlit tri. mesh w/ 2 Tex";
		break;
	case MLR_I_C_DT_TMeshClassID:
		return "colored, unlit tri. mesh w/ 2 Tex";
		break;
	case MLR_I_L_DT_TMeshClassID:
		return "colored, lit tri. mesh w/ 2 Tex";
		break;
	case MLR_TerrainClassID:
		return "not colored, unlit terrain w/ DetTex";
		break;
	}
	return "mesh";
}

//#############################################################################
//#####################    MLRPrimitiveBase__ClassData    #####################
//#############################################################################

void
MLRPrimitiveBase__ClassData::TestInstance()
{
	_ASSERT(IsDerivedFrom(MLRPrimitiveBase::DefaultData));
}

} // namespace MidLevelRenderer
