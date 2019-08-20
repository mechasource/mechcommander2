//==========================================================================//
// File:	 gosFX_Effect.cpp												//
// Contents: Base gosFX::Effect Component									//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "gosFXHeaders.hpp"

gosFX::EffectLibrary* gosFX::EffectLibrary::Instance = nullptr;

//------------------------------------------------------------------------------
//
void
gosFX::EffectLibrary::InitializeClass()
{
	_ASSERT(!Instance);
}

//------------------------------------------------------------------------------
//
void
gosFX::EffectLibrary::TerminateClass()
{
	if (Instance)
	{
		Unregister_Object(Instance);
		delete Instance;
		Instance = nullptr;
	}
}

//------------------------------------------------------------------------------
//
gosFX::EffectLibrary::EffectLibrary()
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
}

//------------------------------------------------------------------------------
//
gosFX::EffectLibrary::~EffectLibrary()
{
	for (uint32_t i = 0; i < m_effects.GetLength(); ++i)
	{
		if (m_effects[i])
		{
			Unregister_Object(m_effects[i]);
			delete m_effects[i];
		}
	}
}

//------------------------------------------------------------------------------
//
void
gosFX::EffectLibrary::Load(std::iostream stream)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	_ASSERT(!m_effects.GetLength());
	uint32_t version = ReadGFXVersion(stream);
	uint32_t len;
	*stream >> len;
	m_effects.SetLength(len);
	for (uint32_t i = 0; i < len; ++i)
	{
		m_effects[i] = gosFX::Effect::Specification::Create(stream, version);
		Check_Object(m_effects[i]);
		m_effects[i]->m_effectID = i;
	}
}

//------------------------------------------------------------------------------
//
void
gosFX::EffectLibrary::Save(std::iostream stream)
{
	WriteGFXVersion(stream);
	*stream << m_effects.GetLength();
	for (uint32_t i = 0; i < m_effects.GetLength(); ++i)
	{
		Check_Object(m_effects[i]);
		m_effects[i]->Save(stream);
	}
}

//------------------------------------------------------------------------------
//
gosFX::Effect::Specification*
gosFX::EffectLibrary::Find(std::wstring& name)
{
	for (auto i = 0; i < m_effects.GetLength(); ++i)
	{
		gosFX::Effect::Specification* spec = m_effects[i];
		if (spec)
		{
			Check_Object(spec);
			if (spec->m_name == name) // if(!_stricmp(spec->m_name, name))
			{
				_ASSERT(spec->m_effectID == i);
				return spec;
			}
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
//
gosFX::Effect*
gosFX::EffectLibrary::MakeEffect(uint32_t index, uint32_t flags)
{
	gosFX::Effect::Specification* spec = m_effects[index];
	Check_Object(spec);
	gosFX::Effect::ClassData* data = Cast_Pointer(
		gosFX::Effect::ClassData*, Stuff::RegisteredClass::FindClassData(spec->GetClassID()));
	Check_Object(data);
	Check_Pointer(data->effectFactory);
	return (*data->effectFactory)(spec, flags);
}
