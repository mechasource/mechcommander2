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

	class aAnimation
	{

	public:
		aAnimation(void);
		~aAnimation(void);
		aAnimation(const aAnimation&);

		aAnimation& operator=(const aAnimation& src);

		int32_t	init(FitIniFile* file, PCSTR prependName);
		int32_t	initWithBlockName(FitIniFile* file, PCSTR blockName);
		void	destroy(void);

		void	begin(void);
		void	reverseBegin(void);
		void	end(void);

		void	update(void);

		bool	isAnimating(void) const
		{
			return currentTime != -1.f;
		}
		bool	isDone(void) const;

		float	getXDelta(void) const;
		float	getYDelta(void) const;

		float	getScaleX(void) const;
		float	getScaleY(void) const;

		uint32_t getColor(void) const;
		uint32_t getColor(float time) const;

		void setReferencePoints(float X, float Y);

		float	getDirection()
		{
			return direction;
		}
		float	getCurrentTime()
		{
			return currentTime;
		}
		float	getMaxTime(void);




	protected:

		float currentTime;

		struct MoveInfo
		{
			float time; // in seconds
			float positionX; // x pixels
			float positionY; // pixels
			float scaleX;
			float scaleY;
			int32_t color;
		};

		MoveInfo*	infos;
		size_t		infoCount;
		float		refX;
		float		refY;
		float		direction;
		bool		bLoops;

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

		int32_t init(FitIniFile* file, PCSTR blockName);

		void setState(STATE);
		STATE getState(void) const;

		int32_t getCurrentColor(STATE) const;
		int32_t getXDelta(STATE) const;
		int32_t getYDelta(STATE) const;
		float getCurrnetScale(STATE) const;
		void update(void);

	private:
		aAnimation		animations[MAX_ANIMATION_STATE];
		STATE			curState;
	};


#endif

