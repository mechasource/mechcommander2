//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "stuffheaders.hpp"

#define TEST_STRING "Test String"
#define TEST_STRING_2 "Test StringTest String"
#define TEST_STRING_3 "Test StringZ"

bool Stuff::MString::TestClass()
{
#if 1
	Stuff::MString string_a;
	// _ASSERT(string_a.stringSize == 0);
	// _ASSERT(string_a.stringLength == 0);
	// _ASSERT(string_a.stringText == nullptr);
	Stuff::MString string_b(TEST_STRING);
	Stuff::MString string_c(string_b);
	_ASSERT(string_b.GetLength() == strlen(TEST_STRING));
	_ASSERT(string_b.GetLength() == strlen(string_c));
	_ASSERT(string_b == string_c);
	Stuff::MString string_d(TEST_STRING_2);
	Stuff::MString string_e;
	string_e = string_b + string_c;
	_ASSERT(string_e == string_d);
	Stuff::MString string_f(TEST_STRING_3);
	Stuff::MString string_g;
	string_g = string_b + 'Z';
	_ASSERT(string_g == string_f);
	Stuff::MString string_h(TEST_STRING);
	string_h += string_b;
	_ASSERT(string_h == string_d);
	Stuff::MString string_i(TEST_STRING);
	string_i += 'Z';
	_ASSERT(string_i == string_f);
	Stuff::MString string_j("aaa");
	Stuff::MString string_k("aab");
	Stuff::MString string_l("abb");
	Stuff::MString string_m("bbb");
	Stuff::MString string_n("aaa");
	_ASSERT(string_j < string_k);
	_ASSERT(string_l > string_k);
	_ASSERT(string_l <= string_m);
	_ASSERT(string_m >= string_j);
	_ASSERT(string_j == string_n);
	_ASSERT(string_j != string_k);
	_ASSERT(string_k.operator[](0) == 'a');
	_ASSERT(string_k.operator[](1) == 'a');
	_ASSERT(string_k.operator[](2) == 'b');
	Stuff::MString string_o("0.1,0.2,0.3");
	Stuff::MString string_p("0.1 0.2 0.3");
	Stuff::MString string_q("0.1");
	Stuff::MString string_r("0.2");
	Stuff::MString string_s("0.3");
	_ASSERT(string_o.GetNthToken(0) == string_q);
	_ASSERT(string_o.GetNthToken(1) == string_r);
	_ASSERT(string_o.GetNthToken(2) == string_s);
	_ASSERT(string_p.GetNthToken(0) == string_q);
	_ASSERT(string_p.GetNthToken(1) == string_r);
	_ASSERT(string_p.GetNthToken(2) == string_s);
#endif
	return true;
}
