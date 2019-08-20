//===========================================================================//
// File: vdata.hh                                                            //
// Contents: Interface specifications for virtual data                       //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _REGISTEREDCLASS_HPP_
#define _REGISTEREDCLASS_HPP_

//#include <stuff/stuff.hpp>
#include <stuff/style.hpp>
#include <mechclassids.h>

namespace Stuff
{

class RegisteredClass__ClassData;

//##########################################################################
//#######################    RegisteredClass    ############################
//##########################################################################

class RegisteredClass
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	friend RegisteredClass__ClassData;

public:
	static void InitializeClass(void);
	static void TerminateClass(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// ClassID enumeration
	//
public:
	typedef uint32_t ClassID;
	typedef RegisteredClass__ClassData ClassData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Construction, destruction, saving
	//
public:
	virtual ~RegisteredClass(void) {}

protected:
	explicit RegisteredClass(ClassData* class_data);

private:
	RegisteredClass(const RegisteredClass&);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
public:
	ClassData* GetClassData(void) const
	{
		// Check_Object(this);
		return classData;
	}
	bool IsDerivedFrom(ClassData* parent) const;
	bool IsDerivedFrom(ClassID class_id) const;
	bool IsDerivedFrom(PCSTR parent) const;

	static ClassData* FindClassData(PCSTR name);
	static ClassData* FindClassData(ClassID class_id)
	{
		_ASSERT(class_id < static_cast<ClassID>(__stuff_classids::ClassIDCount));
		return ClassDataArray[class_id];
	}
	static ClassData* DefaultData;

protected:
	ClassData* classData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Accessors
	//
public:
	ClassID GetClassID(void) const;
	PCSTR GetClassString(void) const;
	static ClassID AllocateTemporaryClassID(void)
	{
		_ASSERT(FirstTemporaryClassID < static_cast<ClassID>(__stuff_classids::ClassIDCount));
		return FirstTemporaryClassID++;
	}

private:
	static ClassData* ClassDataArray[__stuff_classids::ClassIDCount];
	static ClassID FirstTemporaryClassID;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Test support
	//
public:
	void TestInstance(void) const;
};

//##########################################################################
//#####################    Receiver::ClassData    ##########################
//##########################################################################
class RegisteredClass__ClassData
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	friend class RegisteredClass;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Construction, destruction, testing
	//
public:
	RegisteredClass__ClassData(RegisteredClass::ClassID class_id, PCSTR class_name,
		RegisteredClass__ClassData* parent = nullptr);

	~RegisteredClass__ClassData(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Inheritance stuff
	//
public:
	bool IsDerivedFrom(RegisteredClass__ClassData* parent);

	RegisteredClass::ClassID GetClassID(void)
	{
		// Check_Object(this);
		return classID;
	}
	PCSTR GetClassName(void)
	{
		// Check_Object(this);
		return className;
	}
	RegisteredClass__ClassData* GetParentClass(void)
	{
		// Check_Object(this);
		return parentClass;
	}

protected:
	void DeriveClass(RegisteredClass__ClassData* child);
	RegisteredClass__ClassData* FindClassData(PCSTR name);

	RegisteredClass__ClassData* parentClass;
	RegisteredClass__ClassData* firstChildClass;
	RegisteredClass__ClassData* nextSiblingClass;

	PCSTR className;
	union {
		RegisteredClass::ClassID classID;
		size_t padding;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Test support
	//
public:
	void TestInstance(void);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
inline bool
RegisteredClass::IsDerivedFrom(RegisteredClass__ClassData* parent) const
{
	// Check_Object(this);
	// Check_Object(classData);
	return classData->IsDerivedFrom(parent);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
inline bool
RegisteredClass::IsDerivedFrom(ClassID class_id) const
{
	// Check_Object(this);
	// Check_Object(classData);
	return classData->IsDerivedFrom(FindClassData(class_id));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
inline bool
RegisteredClass::IsDerivedFrom(PCSTR parent) const
{
	// Check_Object(this);
	// Check_Object(classData);
	return classData->IsDerivedFrom(FindClassData(parent));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
inline RegisteredClass::ClassID
RegisteredClass::GetClassID(void) const
{
	// Check_Object(this);
	RegisteredClass__ClassData* data = GetClassData();
	// Check_Object(data);
	return data->GetClassID();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
inline RegisteredClass::ClassData*
RegisteredClass::FindClassData(PCSTR name)
{
	return DefaultData->FindClassData(name);
}
} // namespace Stuff
#endif
