//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrshape.h"

namespace MidLevelRenderer
{

//#############################################################################
//###############################    MLRShape ##################################
//#############################################################################

MLRShape::ClassData* MLRShape::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRShape::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLRShapeClassID, "MidLevelRenderer::MLRShape", Plug::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRShape::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape::MLRShape(std::iostream stream, uint32_t version)
	: Plug(DefaultData)
{
	// Check_Pointer(this);
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	*stream >> numPrimitives;
	int32_t i;
	allPrimitives.SetLength(numPrimitives);
	for (i = 0; i < numPrimitives; i++)
	{
		MLRPrimitiveBase* pt = nullptr;
		switch (version)
		{
		case 1:
		case 2:
		{
			STOP(("Lower than version 3 is not supported anymore !"));
			/*
							MLRPrimitive *pt_old;

							RegisteredClass::ClassID class_id;
							*stream >> class_id;
							MLRPrimitive::ClassData* class_data =
								Cast_Object(MLRPrimitive::ClassData*,
			FindClassData(class_id)); pt_old =
			(*class_data->primitiveFactory)(stream, version);
							Register_Object(pt_old);

							int32_t i, nr, test;

							if(pt_old->GetReferenceState().GetLightingMode() ==
			MLRState::LightingOffMode)
							{
								test = 0;
							}
							else
							{
								test = 2;
							}

			#if COLOR_AS_DWORD
							uint32_t* colors;
			#else
							RGBAcolour *colors, testcolour(1.0f, 1.0f,
			1.0f, 1.0f); #endif Cast_Pointer(MLRIndexedPolyMesh*,
			pt_old)->GetcolourData(&colors, &nr);

							for(i=0;i<nr;i++)
							{
			#if COLOR_AS_DWORD
								if(colors[i] != 0xffffffff)
			#else
								if(colors[i] != testcolour)
			#endif
								{
									break;
								}
							}
							if(i<nr)
							{
								test |= 1;
							}

							switch (test)
							{
								case 0:
									pt = new MLR_I_PMesh;

									Cast_Pointer(MLR_I_PMesh*,
			pt)->Copy(Cast_Pointer(MLRIndexedPolyMesh*, pt_old)); break; case 1:
									pt = new MLR_I_C_PMesh;

									Cast_Pointer(MLR_I_C_PMesh*,
			pt)->Copy(Cast_Pointer(MLRIndexedPolyMesh*, pt_old)); break;
								default:
									pt = new MLR_I_L_PMesh;

									Cast_Pointer(MLR_I_L_PMesh*,
			pt)->Copy(Cast_Pointer(MLRIndexedPolyMesh*, pt_old)); break;
							}
							pt_old->DetachReference();
			*/
		}
		break;
		default:
		{
			RegisteredClass::ClassID class_id;
			*stream >> class_id;
			MLRPrimitiveBase::ClassData* class_data =
				Cast_Pointer(MLRPrimitiveBase::ClassData*, FindClassData(class_id));
			Check_Object(class_data);
			pt = (*class_data->primitiveFactory)(stream, version);
			Register_Object(pt);
			if (ConvertToTriangleMeshes == true && (pt->IsDerivedFrom(MLR_I_PMesh::DefaultData) || pt->IsDerivedFrom(MLR_I_C_PMesh::DefaultData) || pt->IsDerivedFrom(MLR_I_L_PMesh::DefaultData)))
			{
				uint8_t* length;
				int32_t i, num, threes, nonThrees;
				pt->GetSubprimitiveLengths(&length, &num);
				for (i = 0, threes = 0, nonThrees = 0; i < num; i++)
				{
					if (length[i] == 3)
					{
						threes++;
					}
					else
					{
						nonThrees++;
					}
				}
				if (threes > 0 && nonThrees == 0) // threes > 9*nonThrees)
				{
					MLRPrimitiveBase* pt_old = pt;
					if (pt->GetClassData() == MLR_I_L_PMesh::DefaultData)
					{
						pt = new MLR_I_L_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_L_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_L_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_C_PMesh::DefaultData)
					{
						pt = new MLR_I_C_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_C_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_C_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_PMesh::DefaultData)
					{
						pt = new MLR_I_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_TMesh*, pt)->Copy(Cast_Pointer(MLR_I_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_L_DT_PMesh::DefaultData)
					{
						pt = new MLR_I_L_DT_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_L_DT_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_L_DT_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_C_DT_PMesh::DefaultData)
					{
						pt = new MLR_I_C_DT_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_C_DT_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_C_DT_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_DT_PMesh::DefaultData)
					{
						pt = new MLR_I_DT_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_DT_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_DT_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_L_DeT_PMesh::DefaultData)
					{
						pt = new MLR_I_L_DeT_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_L_DeT_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_L_DeT_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_C_DeT_PMesh::DefaultData)
					{
						pt = new MLR_I_C_DeT_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_C_DeT_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_C_DeT_PMesh*, pt_old));
					}
					else if (pt->GetClassData() == MLR_I_DeT_PMesh::DefaultData)
					{
						pt = new MLR_I_DeT_TMesh;
						Register_Object(pt);
						Cast_Pointer(MLR_I_DeT_TMesh*, pt)
							->Copy(Cast_Pointer(MLR_I_DeT_PMesh*, pt_old));
					}
					pt_old->DetachReference();
				}
			}
		}
		break;
		}
		allPrimitives[i] = pt;
	}
	referenceCount = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape::MLRShape(int32_t nr)
	: Plug(DefaultData)
	, allPrimitives(nr ? nr : 4)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	numPrimitives = 0;
	referenceCount = 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape::~MLRShape()
{
	int32_t i;
	MLRPrimitiveBase* pt;
	for (i = numPrimitives - 1; i >= 0; i--)
	{
		pt = allPrimitives[i];
		allPrimitives[i] = nullptr;
		pt->DetachReference();
	}
	_ASSERT(referenceCount == 0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MLRShape::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRShape* shape = new MLRShape(stream, version);
	gos_PopCurrentHeap();
	return shape;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRShape::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	*stream << numPrimitives;
	int32_t i;
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		allPrimitives[i]->Save(stream);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRShape::Add(MLRPrimitiveBase* p)
{
	// Check_Object(this);
	if (numPrimitives >= allPrimitives.GetLength() || allPrimitives.GetLength() == 0)
	{
#ifdef _GAMEOS_HPP_
		gos_PushCurrentHeap(Heap);
#endif
		allPrimitives.SetLength(numPrimitives + 4);
		gos_PopCurrentHeap();
	}
	allPrimitives[numPrimitives] = p;
	p->AttachReference();
	numPrimitives++;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRShape::GetNumPrimitives()
{
	int32_t i, ret = 0;
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		ret += allPrimitives[i]->GetNumPrimitives();
	}
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRShape::GetNumDrawnTriangles()
{
	int32_t i, j, ret = 0;
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		if (allPrimitives[i]->IsDerivedFrom(MLRIndexedPrimitiveBase::DefaultData))
		{
			j = (Cast_Pointer(MLRIndexedPrimitiveBase*, allPrimitives[i]))->GetNumGOSIndices();
		}
		else
		{
			j = allPrimitives[i]->GetNumGOSVertices();
		}
		if (j != 0xffff)
		{
			ret += j;
		}
	}
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitiveBase*
MLRShape::Find(int32_t i)
{
	// Check_Object(this);
	_ASSERT(i < numPrimitives);
	return allPrimitives[i];
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRShape::Find(MLRPrimitiveBase* p)
{
	// Check_Object(this);
	Check_Object(p);
	int32_t i;
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		if (allPrimitives[i] == p)
		{
			return i;
		}
	}
	return -1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool MLRShape::Replace(MLRPrimitiveBase* pout, MLRPrimitiveBase* pin)
{
	// Check_Object(this);
	Check_Object(pout);
	Check_Object(pin);
	int32_t num = Find(pout);
	if (num >= 0)
	{
		pout->DetachReference();
		allPrimitives[num] = pin;
		pin->AttachReference();
	}
	else
	{
		return false;
	}
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitiveBase*
MLRShape::Remove(MLRPrimitiveBase* p)
{
	// Check_Object(this);
	Check_Object(p);
	int32_t i, nr = Find(p);
	if (nr < 0)
	{
		return nullptr;
	}
	for (i = nr; i < numPrimitives - 1; i++)
	{
		allPrimitives[i] = allPrimitives[i + 1];
	}
	allPrimitives[i] = nullptr;
	numPrimitives--;
	p->DetachReference();
	return p;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPrimitiveBase*
MLRShape::Remove(int32_t nr)
{
	// Check_Object(this);
	int32_t i;
	MLRPrimitiveBase* p = Find(nr);
	if (nr < 0 || nr >= numPrimitives)
	{
		return nullptr;
	}
	for (i = nr; i < numPrimitives - 1; i++)
	{
		allPrimitives[i] = allPrimitives[i + 1];
	}
	allPrimitives[i] = nullptr;
	numPrimitives--;
	p->DetachReference();
	return p;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLRShape::Insert(MLRPrimitiveBase* p, int32_t nr)
{
	// Check_Object(this);
	if (nr >= numPrimitives)
	{
		Add(p);
		return numPrimitives;
	}
	if (numPrimitives >= allPrimitives.GetLength() || allPrimitives.GetLength() == 0)
	{
#ifdef _GAMEOS_HPP_
		gos_PushCurrentHeap(Heap);
#endif
		allPrimitives.SetLength(numPrimitives + 4);
		gos_PopCurrentHeap();
	}
	int32_t i;
	for (i = numPrimitives; i > nr; i--)
	{
		allPrimitives[i] = allPrimitives[i - 1];
	}
	allPrimitives[i] = p;
	p->AttachReference();
	numPrimitives++;
	return numPrimitives;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRShape::InitializePrimitives(uint8_t vis, const MLRState& master, int32_t parameter)
{
	// Check_Object(this);
	int32_t i;
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		allPrimitives[i]->InitializeDrawPrimitive(vis, parameter);
		allPrimitives[i]->CombineStates(master);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// p is the eye point
void MLRShape::HurtMe(const Stuff::LinearMatrix4D& pain, float radius)
{
	for (size_t i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		allPrimitives[i]->HurtMe(pain, radius);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// p is the eye point
int32_t
MLRShape::FindBackFace(const Point3D& p)
{
	// Check_Object(this);
	int32_t i, ret = 0;
	Point3D sp;
	sp.Multiply(p, *worldToShape);
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		ret += allPrimitives[i]->FindBackFace(sp);
	}
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
void
	MLRShape::Transform(Matrix4D *mat)
{
	// Check_Object(this);

	 int32_t i;

	for(i=0;i<numPrimitives;i++)
	{
		Check_Object(allPrimitives[i]);

		allPrimitives[i]->Transform(mat);
	}
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRShape::Transform()
{
	// Check_Object(this);

	 int32_t i;

	for(i=0;i<numPrimitives;i++)
	{
		Check_Object(allPrimitives[i]);

		allPrimitives[i]->Transform(&shapeToClipMatrix);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
	MLRShape::Clip(MLRClippingState clippingFlags, GOSVertexPool *vp)
{
	// Check_Object(this);

	int32_t i, ret = 0;

	gos_GetViewport( &ViewportScalars::MulX, &ViewportScalars::MulY,
&ViewportScalars::AddX, &ViewportScalars::AddY );

	for(i=0;i<numPrimitives;i++)
	{
		Check_Object(allPrimitives[i]);

		ret += allPrimitives[i]->Clip(clippingFlags, vp);
	}

	return ret;
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRShape::Lighting(
	const LinearMatrix4D& WorldToShape, MLRLight* const* lights, int32_t nrLights)
{
	// Check_Object(this);
	Check_Object(&WorldToShape);
	if (nrLights == 0)
	{
		return;
	}
	int32_t i;
	for (i = 0; i < nrLights; i++)
	{
		lights[i]->SetLightToShapeMatrix(WorldToShape);
	}
	for (i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		allPrimitives[i]->Lighting(lights, nrLights);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool MLRShape::CastRay(Line3D* line, Normal3D* normal)
{
	// Check_Object(this);
	Check_Object(line);
	Check_Pointer(normal);
	bool result = false;
	for (size_t i = 0; i < numPrimitives; i++)
	{
		Check_Object(allPrimitives[i]);
		if (allPrimitives[i]->CastRay(line, normal))
			result = true;
	}
	return result;
}

} // namespace MidLevelRenderer
