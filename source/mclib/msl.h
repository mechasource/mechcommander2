//-------------------------------------------------------------------------------
//
// Multiple TG Shape Layer
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Replace MLR for a myriad number of reasons.
//
// Started 4/16/99
//
// FFS
//
//-------------------------------------------------------------------------------

#pragma once

#ifndef MSL_H
#define MSL_H
//-------------------------------------------------------------------------------
// Include Files

#ifndef TGL_H
#include "tgl.h"
#endif

#ifndef TXMMGR_H
#include "txmmgr.h"
#endif

// ARM
namespace Microsoft
{
namespace Xna
{
namespace Arm
{
struct IProviderEngine;
}
} // namespace Xna
} // namespace Microsoft

//-------------------------------------------------------------------------------
// Structs used by layer.
//

class TG_MultiShape;
class TG_AnimateShape;
//-------------------------------------------------------------------------------
// The meat and Potatoes part.
// TG_TypeMultiShape
class TG_TypeMultiShape
{
	friend TG_MultiShape;
	friend TG_TypeShape;
	friend TG_Shape;
	friend TG_AnimateShape;

	//---------------------------------------------------------
	// This class runs a multi-shape appearance.  Anything
	// with an ASE file of more then one shape should use this
	// Class.  Uses everything from TG_Shape except parse.
	// That is local to here to get all of the pieces.  Each
	// Shape is transformed, rendered etc. in TG_Shape.
	// This will store animation information and use it, too!

	//-------------
	// Data Members
protected:
	uint32_t numTG_TypeShapes; // Number of TG_Shapes
	uint32_t numTextures; // Total Textures for all shapes.
	TG_TypeNodePtr* listOfTypeShapes; // Memory holding all TG_TypeNodes and TypeShapes
	TG_TexturePtr listOfTextures; // List of texture Structures for all shapes.

#ifdef _DEBUG
	wchar_t* shapeName; // FileName of shape
#endif

public:
	Stuff::Vector3D maxBox; // Bounding Box Max Coords
	Stuff::Vector3D minBox; // Bounding Box Min Coords
	float extentRadius; // Bounding Sphere Coords

	//-----------------
	// Member Functions
protected:
public:
	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	void init(void)
	{
		numTG_TypeShapes = 0;
		listOfTypeShapes = nullptr;
		numTextures = 0;
		listOfTextures = nullptr;
#ifdef _DEBUG
		shapeName = nullptr;
#endif
		maxBox.x = maxBox.y = maxBox.z = -100000.0f;
		minBox.x = minBox.y = minBox.z = 100000.0f;
		extentRadius = 0.0f;
	}

	TG_TypeMultiShape(void) { init(void); }

	void destroy(void);

	~TG_TypeMultiShape(void) { destroy(void); }

	// Function returns 0 if OK.  -1 if file not found or file not ASE Format.
	// Function runs through each piece of ASE file and creates a separate
	// TG_Shape for each one.  First pass is count number of GEOMOBJECTS.
	// Second pass is load each one.
	int32_t LoadTGMultiShapeFromASE(const std::wstring_view& fileName, bool forceMakeBinary = false,
		Microsoft::Xna::Arm::IProviderEngine* armProvider = nullptr);

	// Function returns 0 if OK.  -1 if textureName is longer then nameLength-1.
	// This function digs the texture name(s) out of the ASE file so that the
	// User can load and manage them anyway they want to.
	int32_t GetTextureName(uint32_t textureNum, const std::wstring_view& textureName, int32_t nameLength);

	// Function returns 0 if OK.  -1 if textureNum is out of range of
	// numTextures.  This function takes the gosTextureHandle passed in and
	// assigns it to the  textureNum entry of the listOfTextures;
	int32_t SetTextureHandle(uint32_t textureNum, uint32_t gosTextureHandle);

	// Function returns 0 if OK.  -1 if textureNum is out of range of
	// numTextures.  This function takes the gosTextureHandle passed in and
	// assigns it to the  textureNum entry of the listOfTextures;
	int32_t SetTextureAlpha(uint32_t textureNum, bool alphaFlag);

	uint32_t GetTextureHandle(uint32_t textureNum)
	{
		if (textureNum < numTextures)
			return listOfTextures[textureNum].mcTextureNodeIndex;
		return 0xffffffff;
	}

	uint32_t GetNumShapes(void) { return numTG_TypeShapes; }

	uint32_t GetNumTextures(void) { return numTextures; }

	const std::wstring_view& GetNodeId(uint32_t shapeNum)
	{
		if ((shapeNum >= 0) && (shapeNum < numTG_TypeShapes))
			return (listOfTypeShapes[shapeNum]->getNodeId());
		return nullptr;
	}

	Stuff::Point3D GetNodeCenter(uint32_t shapeNum)
	{
		Stuff::Point3D result;
		result.x = result.y = result.z = 0.0f;
		if ((shapeNum >= 0) && (shapeNum < numTG_TypeShapes))
			result = listOfTypeShapes[shapeNum]->GetNodeCenter(void);
		return result;
	}

	Stuff::Point3D GetNodeCenter(const std::wstring_view& nodeId)
	{
		Stuff::Point3D result;
		result.x = result.y = result.z = 0.0f;
		for (uint32_t i = 0; i < numTG_TypeShapes; i++)
		{
			if (_stricmp(listOfTypeShapes[i]->getNodeId(), nodeId) == 0)
				result = listOfTypeShapes[i]->GetNodeCenter(void);
		}
		return result;
	}

	// This function creates an instance of the TG_MultiShape from the Type
	TG_MultiShape* CreateFrom(void);

	void SetAlphaTest(bool flag)
	{
		for (uint32_t i = 0; i < numTG_TypeShapes; i++)
			listOfTypeShapes[i]->SetAlphaTest(flag);
	}

	void SetFilter(bool flag)
	{
		for (uint32_t i = 0; i < numTG_TypeShapes; i++)
			listOfTypeShapes[i]->SetFilter(flag);
	}

	void SetLightRGBs(uint32_t hPink, uint32_t hGreen, uint32_t hYellow)
	{
		for (uint32_t i = 0; i < numTG_TypeShapes; i++)
			listOfTypeShapes[i]->SetLightRGBs(hPink, hGreen, hYellow);
	}

	int32_t LoadBinaryCopy(const std::wstring_view& fileName);
	void SaveBinaryCopy(const std::wstring_view& fileName);
};

typedef TG_TypeMultiShape* TG_TypeMultiShapePtr;

//-------------------------------------------------------------------------------
// The meat and Potatoes part.
// TG_MultiShape
class TG_MultiShape
{
	//---------------------------------------------------------
	// This class runs a multi-shape appearance.  Anything
	// with an ASE file of more then one shape should use this
	// Class.  Uses everything from TG_Shape except parse.
	// That is local to here to get all of the pieces.  Each
	// Shape is transformed, rendered etc. in TG_Shape.
	// This will store animation information and use it, too!

	friend TG_TypeMultiShape;
	friend TG_Shape;
	friend TG_TypeShape;
	friend TG_AnimateShape;

	//-------------
	// Data Members
protected:
	TG_TypeMultiShapePtr myMultiType; // Pointer to the type
	int32_t numTG_Shapes; // Number of TG_Shapes
	TG_ShapeRecPtr listOfShapes; // Memory holding all TG_ShapeRecs
	float frameNum; // Frame number of animation
	bool d_useShadows;
	bool isHudElement;
	uint8_t alphaValue; // To fade shapes in and out
	bool isClamped; // So I can force a shape to clamp its textures

	//-----------------
	// Member Functions
protected:
public:
	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	void init(void)
	{
		myMultiType = nullptr;
		numTG_Shapes = 0;
		listOfShapes = nullptr;
		frameNum = 0.0f;
		d_useShadows = true;
		isHudElement = false;
		alphaValue = 0xff;
		isClamped = false;
	}

	TG_MultiShape(void) { init(void); }

	void destroy(void);

	~TG_MultiShape(void) { destroy(void); }

	// This function sets the list of lights used by the TransformShape function
	// to light the shape.
	// Function returns 0 if lightList entries are all OK.  -1 otherwise.
	//
	int32_t SetLightList(TG_LightPtr* lightList, uint32_t nLights);

	// This function sets the fog values for the shape.  Straight fog right now.
	void SetFogRGB(uint32_t fRGB);

	// This function does the actual transform math, clip checks and lighting
	// math.  The matrices passed in are the translation/rotation matrix for the
	// shape and  Its inverse.  Also will animate.  Function returns -1 if all
	// vertex screen positions are off screen.  Function returns 0 if any one
	// vertex screen position is off screen.  Function returns 1 is all vertex
	// screen positions are on screen.
	// NOTE:  THIS IS NOT A RIGOROUS CLIP!!!!!!!!!
	int32_t TransformMultiShape(Stuff::Point3D* pos, Stuff::UnitQuaternion* rot);

	// This function rotates the heirarchy from this node down.  Used for torso
	// twists, arms, etc.
	// SHould only be called once this way.  This way is DAMNED SLOW!!!
	// STRICMP!  IT returns the node num Call that from then on!
	int32_t SetNodeRotation(const std::wstring_view& nodeName, Stuff::UnitQuaternion* rot);

	void SetNodeRotation(int32_t nodeNum, Stuff::UnitQuaternion* rot)
	{
		if ((nodeNum >= 0) && (nodeNum < numTG_Shapes))
			listOfShapes[nodeNum].baseRotation = *rot;
	}

	// This function takes the current listOfVisibleFaces and draws them using
	// gos_DrawTriangle.
	void Render(bool refreshTextures = false, float forceZ = -1.0f);

	// This function takes the current listOfShadowTriangles and draws them
	// using  gos_DrawTriangle.
	void RenderShadows(bool refreshTextures = false);

	int32_t GetNumShapes(void) { return numTG_Shapes; }

	void ScaleShape(float scaleFactor)
	{
		if (scaleFactor >= 0.0f)
		{
			for (size_t i = 0; i < numTG_Shapes; i++)
				listOfShapes[i].node->shapeScalar = scaleFactor;
		}
	}

	int32_t GetNumVerticesInShape(int32_t i)
	{
		if ((i >= 0) && (i < numTG_Shapes))
		{
			return myMultiType->listOfTypeShapes[i]->GetNumTypeVertices(void);
		}
		return 0;
	}

	Stuff::Vector3D GetShapeVertexInWorld(int32_t shapeNum, int32_t vertexNum, float rotation);
	Stuff::Vector3D GetShapeVertexInEditor(int32_t shapeNum, int32_t vertexNum, float rotation);

	float GetFrameNum(void) { return frameNum; }

	void SetFrameNum(float frame) { frameNum = frame; }

	void SetIsClamped(bool value) { isClamped = value; }

	// Function returns 0 if OK.  -1 if textureName is longer then nameLength-1.
	// This function digs the texture name(s) out of the ASE file so that the
	// User can load and manage them anyway they want to.
	int32_t GetTextureName(uint32_t textureNum, const std::wstring_view& textureName, int32_t nameLength)
	{
		return myMultiType->GetTextureName(textureNum, textureName, nameLength);
	}

	int32_t GetNumTextures(void) { return myMultiType->GetNumTextures(void); }

	// Function returns 0 if OK.  -1 if textureNum is out of range of
	// numTextures.  This function takes the gosTextureHandle passed in and
	// assigns it to the  textureNum entry of the listOfTextures;
	int32_t SetTextureHandle(uint32_t textureNum, uint32_t gosTextureHandle)
	{
		return myMultiType->SetTextureHandle(textureNum, gosTextureHandle);
	}

	uint32_t GetTextureHandle(uint32_t textureNum)
	{
		return myMultiType->GetTextureHandle(textureNum);
	}

	float GetExtentRadius(void) { return myMultiType->extentRadius; }

	Stuff::Vector3D GetMaxBox(void) { return myMultiType->maxBox; }

	Stuff::Vector3D GetMinBox(void) { return myMultiType->minBox; }

	// Function returns 0 if OK.  -1 if textureNum is out of range of
	// numTextures.  This function takes the gosTextureHandle passed in and
	// assigns it to the  textureNum entry of the listOfTextures;
	int32_t SetTextureAlpha(uint32_t textureNum, bool alphaFlag)
	{
		return myMultiType->SetTextureAlpha(textureNum, alphaFlag);
	}

	Stuff::Point3D GetRootNodeCenter(void)
	{
		Stuff::Point3D result;
		result.x = result.y = result.z = 0.0f;
		for (size_t i = 0; i < numTG_Shapes; i++)
		{
			if (listOfShapes[i].parentNode == nullptr)
			{
				result = listOfShapes[i].node->myType->GetNodeCenter(void);
			}
		}
		return result;
	}

	Stuff::Point3D GetRootRelativeNodeCenter(void)
	{
		Stuff::Point3D result;
		result.x = result.y = result.z = 0.0f;
		for (size_t i = 0; i < numTG_Shapes; i++)
		{
			if (listOfShapes[i].parentNode == nullptr)
			{
				result = listOfShapes[i].node->myType->GetRelativeNodeCenter(void);
			}
		}
		return result;
	}

	void SetCurrentAnimation(int32_t shapeNum, TG_AnimationPtr anim)
	{
		if ((shapeNum >= 0) && (shapeNum < numTG_Shapes))
			listOfShapes[shapeNum].currentAnimation = anim;
	}

	// This function clears all of the animation pointers.
	// As such, the shape will stop animating and return to base pose.
	void ClearAnimation(void)
	{
		for (size_t i = 0; i < numTG_Shapes; i++)
			listOfShapes[i].currentAnimation = nullptr;
	}

	void SetARGBHighLight(uint32_t argb)
	{
		for (size_t i = 0; i < numTG_Shapes; i++)
			listOfShapes[i].node->SetARGBHighLight(argb);
	}

	void SetLightsOut(bool lightFlag)
	{
		for (size_t i = 0; i < numTG_Shapes; i++)
			listOfShapes[i].node->SetLightsOut(lightFlag);
	}

	Stuff::Vector3D GetTransformedNodePosition(
		Stuff::Point3D* pos, Stuff::UnitQuaternion* rot, const std::wstring_view& nodeId);
	Stuff::Vector3D GetTransformedNodePosition(
		Stuff::Point3D* pos, Stuff::UnitQuaternion* rot, int32_t nodeId);

	TG_TypeMultiShapePtr GetMultiType(void) { return myMultiType; }

	// This function takes the shape named nodeName and all of its children,
	// detaches them from the current heirarchy and stuffs them into a new
	// MultiShape which it passes back Uses are endless but for now limited to
	// blowing the arms off of the mechs!
	TG_MultiShape* Detach(const std::wstring_view& nodeName);

	// This function takes the shape named nodeName and all of its children and
	// stops processing them forever.  Since we can never re-attach a mech arm
	// in the field, this is OK!
	void StopUsing(const std::wstring_view& nodeName);

	// Tells me if the passed in nodeName is a child of the parentName.
	bool isChildOf(const std::wstring_view& nodeName, const std::wstring_view& parentName);

	const std::wstring_view& GetNodeId(int32_t shapeNum)
	{
		if ((shapeNum >= 0) && (shapeNum < numTG_Shapes))
			return (listOfShapes[shapeNum].node->getNodeName());
		return nullptr;
	}

	int32_t GetNodeNameId(const std::wstring_view& nodeId)
	{
		for (size_t i = 0; i < numTG_Shapes; i++)
		{
			//-----------------------------------------------------------------
			// Scan through the list of shapes and dig out the number needed.
			// DO NOT UPDATE THE HEIRARCHY!!!!
			// This thing may not have updated itself this turn yet!!!
			if (_stricmp(listOfShapes[i].node->myType->getNodeId(), nodeId) == 0)
				return i;
		}
		return -1;
	}

	void SetIsHudElement(void) { isHudElement = true; }

	void SetAlphaValue(uint8_t aVal) { alphaValue = aVal; }

	uint8_t GetAlphaValue(void) { return alphaValue; }

	bool PerPolySelect(int32_t mouseX, int32_t mouseY)
	{
		float mx = mouseX;
		float my = mouseY;
		for (size_t i = 0; i < numTG_Shapes; i++)
		{
			if (listOfShapes[i].node->numVisibleFaces && listOfShapes[i].node->PerPolySelect(mx, my))
				return true;
		}
		return false;
	}

	void SetRecalcShadows(bool flag)
	{
		for (size_t i = 0; i < numTG_Shapes; i++)
		{
			listOfShapes[i].node->SetRecalcShadows(flag);
		}
	}

	void SetUseShadow(bool flag) { d_useShadows = flag; }
};

typedef TG_MultiShape* TG_MultiShapePtr;

//-------------------------------------------------------------------------------
// The meat and Potatoes part of the animation system
// TG_AnimShape
class TG_AnimateShape
{
	friend TG_TypeMultiShape;
	friend TG_MultiShape;
	//--------------------------------------------------------------------
	// This class stores a single gesture worth of animation data.
	// Works by loading the animation data for each NODE in the multiShape
	// From an ASE file.  When we change an animation gesture, this class
	// will shove the correct pointers to the data down to the TG_ShapeRecs.
	// Other then data storage, parsing, and the pointer sets, does zipp!
protected:
	TG_AnimationPtr listOfAnimation;
	int32_t count;
	int32_t actualCount;
	bool shapeIdsSet;

public:
	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	void init(void)
	{
		listOfAnimation = nullptr;
		count = actualCount = 0;
		shapeIdsSet = false;
	}

	TG_AnimateShape(void) { init(void); }

	~TG_AnimateShape(void) { destroy(void); }

	int32_t LoadBinaryCopy(const std::wstring_view& fileName);
	void SaveBinaryCopy(const std::wstring_view& fileName);

	// This function frees all of the RAM allocated by this class and resets
	// vars to initial state.
	void destroy(void);

	// This function loads the animation data contained in the file passed in.
	// It sets up a pointer to the multi-shape so that animation data for each
	// Node in the Multi-Shape can be loaded.
	// It mallocs memory.
	int32_t LoadTGMultiShapeAnimationFromASE(const std::wstring_view& filename, TG_TypeMultiShapePtr shape,
		bool skipIfBinary = false, bool forceMakeBinary = false);

	// This function copies the pointers to the animation data in this class to
	// the  TG_MultiShape being drawn.  Nothing else happens.
	void SetAnimationState(TG_MultiShapePtr mShape);

	float GetFrameRate(void)
	{
		if (listOfAnimation)
			return listOfAnimation[0].frameRate;
		return 30.0f;
	}

	void SetFrameRate(float nFrameRate)
	{
		if (listOfAnimation)
			listOfAnimation[0].frameRate = nFrameRate;
	}

	void ReverseFrameRate(void)
	{
		if (listOfAnimation)
			listOfAnimation[0].frameRate = -listOfAnimation[0].frameRate;
	}

	int32_t GetNumFrames(void)
	{
		if (listOfAnimation)
			return listOfAnimation[0].numFrames;
		return 0;
	}

	void resetShapeIds(void) { shapeIdsSet = false; }
};

typedef TG_AnimateShape* TG_AnimateShapePtr;
//-------------------------------------------------------------------------------
#endif
