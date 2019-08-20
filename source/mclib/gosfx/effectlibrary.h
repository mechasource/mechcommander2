//==========================================================================//
// File:	 EffectLibrary.hpp												//
// Contents: Base Effect Component											//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//#include <gosfx/gosfx.hpp>
//#include <gosfx/effect.hpp>

namespace gosFX
{
class EffectLibrary
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
public:
	EffectLibrary(void);
	~EffectLibrary(void);

	void Load(std::iostream stream);
	void Save(std::iostream stream);

	enum MergeMode
	{
		OnlyAddNewEffects,
		ReplaceMatchingEffects,
		ReplaceNamedEffects
	};

	void Merge(EffectLibrary& source, MergeMode merge_mode = (MergeMode)OnlyAddNewEffects);

	static EffectLibrary* Instance;

protected:
	EffectLibrary(EffectLibrary& /*source*/) { STOP(("Shouldn't be called")); }

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Effect management
	//
public:
	std::vector<Effect::Specification*> m_effects;

	Effect::Specification* Find(std::wstring& name);
	Effect* MakeEffect(uint32_t index, uint32_t flags);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const {}
};
} // namespace gosFX
