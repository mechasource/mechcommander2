//===========================================================================//
// File:	average.hpp                                                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _AVERAGE_HPP_
#define _AVERAGE_HPP_

namespace Stuff
{

//##########################################################################
//###########################    AverageOf    ##############################
//##########################################################################

template <class T>
class AverageOf
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
public:
	//
	//-----------------------------------------------------------------------
	// Constructor, Destructor, Testing
	//-----------------------------------------------------------------------
	//
	AverageOf(void);
	AverageOf(size_t size, T initial = (T)0);
	~AverageOf(void);

	void TestInstance(void) const
	{
		Check_Pointer(array);
		_ASSERT(next < size);
	}

	//
	//-----------------------------------------------------------------------
	// SetSize
	//-----------------------------------------------------------------------
	//
	void SetSize(size_t size, T initial = (T)0);

	//
	//-----------------------------------------------------------------------
	// Add
	//-----------------------------------------------------------------------
	//
	void Add(T value);

	//
	//-----------------------------------------------------------------------
	// CalculateAverage
	//-----------------------------------------------------------------------
	//
	T CalculateAverage(void);

	//
	//-----------------------------------------------------------------------
	// CalculateOlympicAverage
	//-----------------------------------------------------------------------
	//
	T CalculateOlympicAverage(void);

	//
	//-----------------------------------------------------------------------
	// Calculate Trend of Average
	//-----------------------------------------------------------------------
	//
	float CalculateTrend(void);

	//
	//-----------------------------------------------------------------------
	// Return the lowest value in the array, or,
	// Return the highest value in the array
	//-----------------------------------------------------------------------
	//
	T CalculateLowerBound(void);
	T CalculateUpperBound(void);

private:
	//
	//-----------------------------------------------------------------------
	// Private data
	//-----------------------------------------------------------------------
	//
	size_t size;
	size_t next;
	T* array;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ AverageOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
AverageOf<T>::AverageOf()
{
	array = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
AverageOf<T>::AverageOf(size_t the_size, T initial)
{
	array = nullptr;
	SetSize(the_size, initial);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
void AverageOf<T>::SetSize(size_t the_size, T initial)
{
	if (array != nullptr)
	{
		Unregister_Pointer(array);
		delete[] array;
	}
	size = the_size;
	array = new T[size];
	Register_Pointer(array);
	next = 0;
	for (size_t i = 0; i < size; i++)
	{
		Check_Pointer(array);
		_ASSERT(i < size);
		array[i] = initial;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
AverageOf<T>::~AverageOf()
{
	Unregister_Pointer(array);
	delete[] array;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
void AverageOf<T>::Add(T value)
{
	// Check_Object(this);
	Check_Pointer(array);
	_ASSERT(next < size);
	array[next] = value;
	++next;
	if (next >= size)
	{
		_ASSERT(next == size);
		next = 0;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
T AverageOf<T>::CalculateAverage()
{
	// Check_Object(this);
	size_t i;
	T accumulate;
	for (i = 0, accumulate = (T)0; i < size; i++)
	{
		Check_Pointer(array);
		_ASSERT(i < size);
		accumulate += array[i];
	}
	_ASSERT(!Small_Enough(static_cast<float>(size)));
	return (accumulate / static_cast<T>(size));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
T AverageOf<T>::CalculateOlympicAverage()
{
	// Check_Object(this);
	size_t i;
	T accumulate, min_value, max_value;
	_ASSERT(0 < size);
	min_value = array[0];
	max_value = array[0];
	for (i = 0, accumulate = (T)0; i < size; i++)
	{
		Check_Pointer(array);
		_ASSERT(i < size);
		accumulate += array[i];
		min_value = Min(array[i], min_value);
		max_value = Max(array[i], max_value);
	}
	accumulate -= min_value;
	accumulate -= max_value;
	_ASSERT(!Small_Enough(static_cast<float>(size - 2)));
	return (accumulate / static_cast<T>(size - 2));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
float AverageOf<T>::CalculateTrend()
{
	// Check_Object(this);
	size_t i;
	float f = 0.0f;
	float fx = 0.0f;
	float x1 = static_cast<float>(size);
	float x2 = x1 * x1 / 2.0f;
	float x3 = x2 + x1 * x1 * x1 / 3.0f + x1 / 6.0f;
	x2 += x1 / 2.0f;
	i = next;
	float t = 1.0f;
	do
	{
		f += array[i];
		fx += t * array[i];
		t += 1.0f;
		if (++i == size)
		{
			i = 0;
		}
	} while (i != next);
	return (x1 * fx - x2 * f) / (x1 * x3 - x2 * x2);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
T AverageOf<T>::CalculateLowerBound()
{
	// Check_Object(this);
	size_t i;
	T min_value;
	_ASSERT(0 < size);
	min_value = array[0];
	for (i = 0; i < size; i++)
	{
		Check_Pointer(array);
		_ASSERT(i < size);
		min_value = Min(array[i], min_value);
	}
	return min_value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <class T>
T AverageOf<T>::CalculateUpperBound()
{
	// Check_Object(this);
	size_t i;
	T max_value;
	_ASSERT(0 < size);
	max_value = array[0];
	for (i = 0; i < size; i++)
	{
		Check_Pointer(array);
		_ASSERT(i < size);
		max_value = Max(array[i], max_value);
	}
	return max_value;
}

//##########################################################################
//########################    StaticAverageOf    ###########################
//##########################################################################

template <class T>
class StaticAverageOf
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
public:
	//
	//-----------------------------------------------------------------------
	// Constructor, Destructor, Testing
	//-----------------------------------------------------------------------
	//
	StaticAverageOf()
	{
		size = (size_t)(T)0;
		total = (T)0;
	}
	~StaticAverageOf() { }

	void TestInstance(void) const { }

	//
	//-----------------------------------------------------------------------
	// Add
	//-----------------------------------------------------------------------
	//
	void Add(T value)
	{
		// Check_Object(this);
		size++;
		total += value;
	}

	//
	//-----------------------------------------------------------------------
	// CalculateAverage
	//-----------------------------------------------------------------------
	//
	T CalculateAverage()
	{
		// Check_Object(this);
		return (size == 0) ? ((T)0) : (total / (T)size);
	}

	//
	//-----------------------------------------------------------------------
	// GetSize
	//-----------------------------------------------------------------------
	//
	size_t GetSize()
	{
		// Check_Object(this);
		return size;
	}

private:
	//
	//-----------------------------------------------------------------------
	// Private data
	//-----------------------------------------------------------------------
	//
	size_t size;
	T total;
};
} // namespace Stuff
#endif
