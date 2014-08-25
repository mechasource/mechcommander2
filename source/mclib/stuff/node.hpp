//===========================================================================//
// File:	node.hh                                                          //
// Contents: Interface specification for base node class                     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <stuff/plug.hpp>

namespace Stuff {

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Node ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef Plug__ClassData Node__ClassData;

	class Node:
		public Plug
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		void TestInstance(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructor/Destructor
	//
	public:
		~Node(void);

	protected:
		explicit Node(ClassData *class_data);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
	public:
		typedef Node__ClassData ClassData;

		static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// plug detachment
	//
	public:
		virtual void
			ReleaseLinkHandler(Socket*, Plug*);
	};

}

#endif
