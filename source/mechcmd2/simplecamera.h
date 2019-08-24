/*************************************************************************************************\
SimpleCamera.h		: Interface for the camera that renders a single object
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef SIMPLECAMERA_H
#define SIMPLECAMERA_H

//#include "camera.h"

class ObjectAppearance;

///////////////////////////////

class SimpleCamera : public Camera
{
public:
	SimpleCamera(void);
	~SimpleCamera(void);

	void setMech(const std::wstring_view& fileName, int32_t base = 0xffff7e00, int32_t highlight = 0xffff7e00,
		int32_t h2 = 0xffbcbcbc);
	void setComponent(const std::wstring_view& fileName);
	void setBuilding(const std::wstring_view& fileName);
	void setVehicle(const std::wstring_view& vehicle, int32_t base = 0xffff7e00, int32_t highlight = 0xffff7e00,
		int32_t h2 = 0xffbcbcbc);
	ObjectAppearance* getObjectAppearance(void) const { return pObject; }

	void init(float left, float right, float top, float bottom);

	void setObject(const std::wstring_view& fileName, int32_t type, int32_t base = 0xffff7e00,
		int32_t highlight = 0xffff7e00, int32_t h2 = 0xffbcbcbc);

	virtual void render(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual int32_t update(void);

	void setScale(float newScale);
	void setRotation(float rotation);
	void zoomIn(float howMuch); // scale for things that can't

	void setInMission(void) { bIsInMission = true; }

	void setColors(
		int32_t base = 0xffff7e00, int32_t highlight = 0xffff7e00, int32_t h2 = 0xffbcbcbc);

	float bounds[4];

private:
	ObjectAppearance* pObject;
	Camera* oldCam;
	float rotation;
	float rotationIncrement;
	float fudgeX;
	float fudgeY;
	bool bIsComponent;
	bool bIsInMission;
	float shapeScale;
};

#endif