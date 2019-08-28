//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRNGONCLOUD_HPP
#define MLR_MLRNGONCLOUD_HPP

#include "mlr/mlreffect.h"

namespace MidLevelRenderer
{

//##########################################################################
//#####################    MLRNGonCloud    #############################
//##########################################################################

class MLRNGonCloud : public MLREffect
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
	MLRNGonCloud(uint32_t vertices, uint32_t number);
	~MLRNGonCloud(void);

	void SetData(
		const size_t* count, const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data);

	void Draw(DrawEffectInformation*, GOSVertexPool*, MLRSorter*);
	uint32_t Clip(MLRClippingState, GOSVertexPool*);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
public:
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const;

protected:
	size_t numOfVertices;
	const size_t* usedNrOfNGons;

	static std::vector<Stuff::RGBAcolour>* clipExtracolours;
	std::vector<Stuff::RGBAcolour> specialClipcolours;

	static std::vector<MLRClippingState>* clipPerVertex;

	static std::vector<Stuff::Vector4D>* clipExtraCoords;

	static std::vector<int32_t>* clipExtraLength;
};
} // namespace MidLevelRenderer
#endif
