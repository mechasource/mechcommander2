//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef AANIMATION_H
#define AANIMATION_H

namespace mechgui
{

class aAnimation;
class FitIniFile;

aAnimation::~aAnimation(void)
{
	destroy();
}

class aAnimation
{

public:
	aAnimation(void) noexcept = default;
	~aAnimation(void);
	aAnimation(const aAnimation&);

	aAnimation& operator=(const aAnimation& src);

	int32_t init(FitIniFile* file, std::wstring_view prependName);
	int32_t initWithBlockName(FitIniFile* file, std::wstring_view blockname);
	void destroy(void);

	void begin(void);
	void reverseBegin(void);
	void end(void);

	void update(void);

	bool isAnimating(void) const
	{
		return currentTime != -1.f;
	}
	bool isDone(void) const;

	float getXDelta(void) const;
	float getYDelta(void) const;

	float getScaleX(void) const;
	float getScaleY(void) const;

	uint32_t getcolour(void) const;
	uint32_t getcolour(float time) const;

	void setReferencePoints(float X, float Y);

	float getDirection()
	{
		return direction;
	}
	float getCurrentTime()
	{
		return currentTime;
	}
	float getMaxTime(void);

protected:
	float currentTime = -1.f;

	struct MoveInfo
	{
		float time; // in seconds
		float positionX; // x pixels
		float positionY; // pixels
		float scaleX;
		float scaleY;
		int32_t color;
	};
	float refX = 0;
	float refY = 0;
	float direction = 1.0;
	std::unique_ptr<MoveInfo> infos;
	size_t infoCount = 0;
	bool bLoops = false;

private:
	void copyData(const aAnimation&);
};

// animations for the states we use everywhere
class aAnimGroup
{
public:
	enum STATE
	{
		NORMAL = 0,
		PRESSED = 1,
		HIGHLIGHT = 2,
		PRESSEDHIGHLIGHT = 3,
		DISABLED = 4,
		MAX_ANIMATION_STATE
	};

	aAnimGroup()
	{
		curState = NORMAL;
	}

	int32_t init(FitIniFile* file, std::wstring_view blockname);

	void setState(STATE);
	STATE getState(void) const;

	int32_t getCurrentcolour(STATE) const;
	int32_t getXDelta(STATE) const;
	int32_t getYDelta(STATE) const;
	float getCurrnetScale(STATE) const;
	void update(void);

private:
	aAnimation animations[MAX_ANIMATION_STATE];
	STATE curState;
};

#endif
