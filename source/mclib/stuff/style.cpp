//===========================================================================//
// File:	verify.cc                                                        //
// Contents: verification fail routines                                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"
//#include "gameos.hpp"
#include "stuff/style.h"

//
//#############################################################################
//#############################################################################
//
static wchar_t Error_Message_Buffer[400];

int32_t Stuff::ArmorLevel = 4;

float Stuff::SMALL = 1e-4f;

//
//#############################################################################
//#############################################################################
//
#if defined(_ARMOR)
// Signature::Signature()
//{
//	mark = Valid;
//}

// Signature::~Signature()
//{
//	mark = Destroyed;
//}

void Stuff::Is_Signature_Bad(const volatile Signature* p)
{
	if ((p) && reinterpret_cast<intptr_t>(p) != Stuff::SNAN_NEGATIVE_LONG)
	{
		_ASSERT(!(reinterpret_cast<intptr_t>(p) & 3));
		if (p->mark == Signature::Destroyed)
			PAUSE(("Object has been destroyed"));
		else if (p->mark != Signature::Valid)
			PAUSE(("Object has been corrupted"));
	}
	else
		PAUSE(("Bad object pointer: %x", p));
}

#endif
