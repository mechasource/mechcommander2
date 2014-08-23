//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRCLIPPER_HPP
#define MLR_MLRCLIPPER_HPP

#include <stuff/style.hpp>
#include <stuff/linearmatrix.hpp>
#include <mlr/mlrshape.hpp>
#include <mlr/mlrclippingstate.hpp>

namespace Stuff {
	class LinearMatrix4D;
	class RGBAColor;
	class Vector4D;
}

namespace MidLevelRenderer {

	typedef int AndyDisplay;
	class MLRSorter;
	//typedef Stuff::Vector2DOf<float> Vector2DScalar;

	class DrawShapeInformation
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		DrawShapeInformation();

		MLRShape*	shape;
		MLRState	state;
		MLRClippingState				clippingFlags;
		const Stuff::LinearMatrix4D*	shapeToWorld;
		const Stuff::LinearMatrix4D*	worldToShape;

		MLRLight* const *activeLights;
		int	nrOfActiveLights;

		void TestInstance() const {}
	};

	class DrawScalableShapeInformation : public DrawShapeInformation
	{
	public:
		DrawScalableShapeInformation(void);
		const Stuff::Vector3D*	scaling;
		const Stuff::RGBAColor*	paintMe;

		void TestInstance() const {}
	};

	class MLREffect;

	class DrawEffectInformation
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		DrawEffectInformation();

		MLREffect*						effect;
		MLRState						state;
		MLRClippingState				clippingFlags;
		const Stuff::LinearMatrix4D*	effectToWorld;

#if 0 // for the time being no lights on the effects
		MLRLight *const *activeLights;
		int	nrOfActiveLights;
#endif
		void TestInstance(void) const {}
	};

	class DrawScreenQuadsInformation
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		DrawScreenQuadsInformation(void);

		const Stuff::Vector4D*	coords;
		const Stuff::RGBAColor*	colors;
		const Vector2DScalar*	texCoords;
		const bool*				onOrOff;

		size_t	nrOfQuads;
		size_t	currentNrOfQuads;
		MLRState state;

		void TestInstance(void) const {}
	};

	//##########################################################################
	//#########################    MLRClipper   #################################
	//##########################################################################

	class MLRClipper :
		public Stuff::RegisteredClass
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		//	Camera gets attached to a film
		MLRClipper(AndyDisplay*, MLRSorter*);
		~MLRClipper();

		//	lets begin the dance
		void StartDraw(
			const Stuff::LinearMatrix4D& camera_to_world,
			const Stuff::Matrix4D& clip_matrix,
			const Stuff::RGBAColor &fog_color,		// NOT USED ANYMORE
			const Stuff::RGBAColor *background_color,
			const MLRState &default_state,
			const float *z_value
			);

		//	add another shape
		void DrawShape (DrawShapeInformation*);

		//	add another shape
		void DrawScalableShape (DrawScalableShapeInformation*);

		//	add screen quads
		void DrawScreenQuads (DrawScreenQuadsInformation*);

		//	add another effect
		void DrawEffect (DrawEffectInformation*);

		//	starts the action
		void RenderNow ()
		{ Check_Object(this); sorter->RenderNow(); }

		//	clear the film
		void Clear (uint32_t flags);

		AndyDisplay* GetDisplay () const
		{ Check_Object(this); return display; };

		// statistics and time
		uint32_t GetFrameRate () const
		{ Check_Object(this); return frameRate; }
		void SetTime (float t) 
		{ Check_Object(this); nowTime = t; }
		float GetTime () const
		{ Check_Object(this); return nowTime; }

		const Stuff::LinearMatrix4D&
			GetCameraToWorldMatrix()
		{Check_Object(this); return cameraToWorldMatrix;}
		const Stuff::LinearMatrix4D&
			GetWorldToCameraMatrix()
		{Check_Object(this); return worldToCameraMatrix;}
		const Stuff::Matrix4D&
			GetCameraToClipMatrix()
		{Check_Object(this); return cameraToClipMatrix;}

		void
			ResetSorter()
		{Check_Object(this); sorter->Reset();}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const {};

	protected:
		//	statistics and time
		uint32_t frameRate;
		float usedTime;
		float nowTime;

		//	world-to-camera matrix
		Stuff::LinearMatrix4D	worldToCameraMatrix;
		Stuff::LinearMatrix4D	cameraToWorldMatrix;
		Stuff::Matrix4D			cameraToClipMatrix;
		Stuff::Matrix4D			worldToClipMatrix;
		Stuff::Point3D			cameraPosition;

		// this is the film
		AndyDisplay*			display;

		// this defines the sort order of the draw 
		MLRSorter*				sorter;

		GOSVertexPool allVerticesToDraw;
	};

}
#endif
