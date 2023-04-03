//===========================================================================//
// File:	vdata.cc                                                         //
// Contents: Implementation details for registered classes and virtual data  //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

////#include "gameos.hpp"
#include "stuff/registeredclass.h"

//// using namespace Stuff;

//#############################################################################
//#######################    RegisteredClass    ###############################
//#############################################################################

RegisteredClass::ClassData* RegisteredClass::DefaultData = nullptr;

RegisteredClass::ClassID RegisteredClass::FirstTemporaryClassID = FirstTemporaryClassID;

RegisteredClass::ClassData* RegisteredClass::ClassDataArray[__stuff_classids::ClassIDCount];

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void RegisteredClass::InitializeClass()
{
	_ASSERT(!DefaultData);
	DefaultData = new ClassData(RegisteredClassClassID, "Stuff::RegisteredClass");
	// Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void RegisteredClass::TerminateClass()
{
	// Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
RegisteredClass::RegisteredClass(ClassData* class_data)
	: classData(class_data)
{
	// Check_Object(class_data);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
std::wstring_view
RegisteredClass::GetClassString(void) const
{
	// Check_Object(this);
	RegisteredClass__ClassData* data = GetClassData();
	// Check_Object(data);
	return data->GetClassName();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void RegisteredClass::TestInstance(void) const
{ /*Check_Object(classData)*/
	;
}

//#############################################################################
//####################    RegisteredClass::ClassData    #######################
//#############################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
RegisteredClass__ClassData::RegisteredClass__ClassData(
	RegisteredClass::ClassID class_id, std::wstring_view name, RegisteredClass__ClassData* parent)
{
	//
	//--------------------------------
	// Set up the class data variables
	//--------------------------------
	//
	_ASSERT(static_cast<uint32_t>(class_id) < __stuff_classids::ClassIDCount);
	classID = class_id;
	className = name;
	firstChildClass = nullptr;
	nextSiblingClass = nullptr;
	parentClass = parent;
	_ASSERT(!RegisteredClass::ClassDataArray[class_id]);
	RegisteredClass::ClassDataArray[class_id] = this;
	//
	//--------------------------------------------------------------------
	// If this class has a parent, hook up to it, otherwise we must be the
	// root class
	//--------------------------------------------------------------------
	//
	if (parentClass)
	{
		// Check_Object(parent);
		parent->DeriveClass(this);
	}
	else
	{
		_ASSERT(classID == RegisteredClassClassID);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
RegisteredClass__ClassData::~RegisteredClass__ClassData()
{
	// Check_Object(this);
	//
	//----------------------------------------------------------------------
	// Make sure there are no children left, then if this is the root class,
	// make sure it's parent link is nullptr
	//----------------------------------------------------------------------
	//
	_ASSERT(!firstChildClass);
	if (classID == RegisteredClassClassID)
	{
		_ASSERT(!parentClass);
	}
	//
	//-----------------------------------------------------------------------
	// Otherwise, this is the a child class, so make sure it is it's parent's
	// current first child.  This ensures that the deallocation order is
	// symmetrical to the allocation order.  Then remove it from the linked
	// list
	//-----------------------------------------------------------------------
	//
	else
	{
		// Check_Object(parentClass);
		_ASSERT(parentClass->firstChildClass == this);
		parentClass->firstChildClass = nextSiblingClass;
	}
	//
	//-------------------------------------
	// Remove this from the ClassData array
	//-------------------------------------
	//
	_ASSERT(static_cast<uint32_t>(classID) < __stuff_classids::ClassIDCount);
	_ASSERT(RegisteredClass::ClassDataArray[classID] == this);
	RegisteredClass::ClassDataArray[classID] = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool RegisteredClass__ClassData::IsDerivedFrom(RegisteredClass__ClassData* parent)
{
	// Check_Object(this);
	//
	//---------------------------
	// handle null parent pointer
	//---------------------------
	//
	if (!parent)
	{
		return false;
	}
	//
	//------------------------------
	// We are descended from ourself
	//------------------------------
	//
	// Check_Object(parent);
	if (parent == this)
	{
		return true;
	}
	//
	//-----------------------------------------------
	// Spin up the chain looking for the parent class
	//-----------------------------------------------
	//
	RegisteredClass__ClassData* class_data = parentClass;
	while (class_data)
	{
		// Check_Object(class_data);
		if (class_data == parent)
		{
			return true;
		}
		class_data = class_data->parentClass;
	}
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
RegisteredClass__ClassData*
RegisteredClass__ClassData::FindClassData(std::wstring_view name)
{
	// Check_Object(this);
	if (!_stricmp(className, name))
	{
		return this;
	}
	RegisteredClass__ClassData* class_data = firstChildClass;
	RegisteredClass__ClassData* result = nullptr;
	while (class_data)
	{
		// Check_Object(class_data);
		result = class_data->FindClassData(name);
		if (result)
		{
			// Check_Object(result);
			return result;
		}
		class_data = class_data->nextSiblingClass;
	}
	return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void RegisteredClass__ClassData::DeriveClass(RegisteredClass__ClassData* child)
{
	// Check_Object(this);
	// Check_Object(child);
	_ASSERT(child->parentClass == this);
	_ASSERT(!child->nextSiblingClass);
	child->nextSiblingClass = firstChildClass;
	firstChildClass = child;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void RegisteredClass__ClassData::TestInstance()
{
	RegisteredClass__ClassData* p = this;
	RegisteredClass__ClassData* class_data = parentClass;
	while (class_data)
	{
		// Check_Signature(class_data);
		p = class_data;
		class_data = class_data->parentClass;
	}
	_ASSERT(p->classID == RegisteredClassClassID);
}
