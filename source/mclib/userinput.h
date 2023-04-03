//---------------------------------------------------------------------------
//
// MechCommander 2
//
// UserInput Class -- Polls the state of keyboard, mouse, joystick
//						for this frame and stores values.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef USERINPUT_H
#define USERINPUT_H

//#include "dstd.h"
//#include "mathfunc.h"
//#include "utilities.h"

//#include <string.h>
////#include "gameos.hpp"

//---------------------------------------------------------------------------

enum mouse_input_const : uint32_t
{
	MC2_MOUSE_UP = 0,
	MC2_MOUSE_DOWN = 1,
	MOUSE_WIDTH = 32,
	MAX_MOUSE_STATES = 256,
};

enum mstate_const : uint32_t
{
	mState_MOVE = 0,
	mState_MOVE_LOS = 1,
	mState_RUN = 2,
	mState_RUN_LOS = 3,
	mState_JUMP = 4,
	mState_JUMP_LOS = 5,
	mState_WALKWAYPT = 6,
	mState_WALKWAYPT_LOS = 7,
	mState_RUNWAYPT = 8,
	mState_RUNWAYPT_LOS = 9,
	mState_JUMPWAYPT = 10,
	mState_JUMPWAYPT_LOS = 11,
	mState_LINK = 12,
	mState_GENERIC_ATTACK = 13,
	mState_ATTACK_LOS = 14,
	mState_CURPOS_ATTACK = 15,
	mState_CURPOS_ATTACK_LOS = 16,
	mState_ENERGY_WEAPONS = 17,
	mState_ENERGY_WEAPONS_LOS = 18,
	mState_AIMED_ATTACK = 19,
	mState_AIMED_ATTACK_LOS = 20,
	mState_NORMAL = 21,
	mState_INFO = 22,
	mState_LAYMINES = 23,
	mState_ROTATE_CAMERA = 24,
	mState_UNCERTAIN_AIRSTRIKE = 25,
	mState_AIRSTRIKE = 26,
	mState_SENSORSTRIKE = 27,
	mState_VEHICLE = 28,
	mState_CANTVEHICLE = 29,
	mState_DONT = 30,
	mState_REPAIR = 31,
	mState_XREPAIR = 32,
	mState_EJECT = 33,
	mState_XEJECT = 34,
	mState_CAPTURE = 35,
	mState_XCAPTURE = 36,
	mState_SALVAGE = 37,
	mState_XSALVAGE = 38,
	mState_GUARD = 39,
	mState_GUARD_LOS = 39,
	mState_GUARDTOWER = 40,
	mState_GUARDTOWER_LOS = 41,
	mState_XMINES = 42,
	mState_LOGISTICS = 43,
	mState_TUTORIALS = 44,
	mState_LONGRNG_ATTACK = 45,
	mState_SHRTRNG_ATTACK = 46,
	mState_MEDRNG_ATTACK = 47,
	mState_WALK1 = 48,
	mState_WALKLOS1 = 80,
	mState_RUN1 = 112,
	mState_RUNLOS1 = 144,
	mState_JUMP1 = 176,
	mState_JUMPLOS1 = 208,
	mState_LONGRNG_LOS = 240,
	mState_MEDRNG_LOS = 241,
	mState_SHRTRNG_LOS = 242,

	mState_NUMMOUSESTATES = 243, // Add states BETWEEN this one and above!!!
	// Used to keep from setting cursor to STUPID!!

};

constexpr const float mouseFrameRate = 0.10f;

extern volatile wchar_t mc2MouseHotSpotX;
extern volatile wchar_t mc2MouseHotSpotY;
extern volatile wchar_t mc2Mousewidth;
extern volatile wchar_t mc2Mouseheight;
extern volatile bool mc2UseAsyncMouse; // Should mouse draw and update in separate thread?
extern volatile bool mc2MouseThreadStarted; // Has the thread starting running yet?
extern volatile uint8_t* mc2MouseData;

//---------------------------------------------------------------------------
class MouseCursorData
{
protected:
	//---------------------------------------------------------------
	// Mouse textures are assumed 32x32.  Animating mice are stored
	// on the smallest texture that will hold their shape.  Thus if
	// have a mouse with four frames of animation it will be stored
	// on a 64x64.

	int32_t numCursors;

	StaticInfo* cursorInfos; // gotta be pointer, so destructor is called before
		// texmgr quits

	wchar_t mouseHS[mouse_input_const::MAX_MOUSE_STATES][2];
	uint32_t numFrames[mouse_input_const::MAX_MOUSE_STATES];
	float frameLengths[mouse_input_const::MAX_MOUSE_STATES];

	friend class UserInput;

public:
	MouseCursorData(void)
	{
		init();
	}
	~MouseCursorData(void)
	{
		destroy();
	}

	void destroy(void);

	void init(void)
	{
		numCursors = 0;
		cursorInfos = 0;
		memset(mouseHS, 0, sizeof(mouseHS));
		memset(numFrames, 0, sizeof(numFrames));
		memset(frameLengths, 0, sizeof(frameLengths));
	}

	uint32_t getNumFrames(int32_t state)
	{
		if ((state >= 0) && (state < numCursors))
			return numFrames[state]; // no animating for now
		return 0;
	}

	wchar_t getMouseHSX(int32_t state)
	{
		if ((state >= 0) && (state < numCursors))
			return mouseHS[state][0];
		return 0;
	}

	wchar_t getMouseHSY(int32_t state)
	{
		if ((state >= 0) && (state < numCursors))
			return mouseHS[state][1];
		return 0;
	}

	void initCursors(std::wstring_view cursorFile);
};

//---------------------------------------------------------------------------
class UserInput
{
protected:
	//--------------------
	// Mouse Information
	float mouseXPosition; // Current Mouse X Position.
	float mouseYPosition; // Current Mouse Y position.

	int32_t mouseXDelta; // Amount mouse has moved in x since last poll
	int32_t mouseYDelta; // Amount mouse has moved in y since last poll

	int32_t mouseWheelDelta; // Amount Mouse wheel has moved since last poll

	uint32_t leftMouseButtonState; // Mouse Current Left Button State
	uint32_t rightMouseButtonState; // Mouse Current Right Button State
	uint32_t middleMouseButtonState; // Mouse Current Middle Button State

	bool leftClick; // Mouse has been left clicked
	bool rightClick; // Mouse has been right clicked
	bool leftDoubleClick; // Mouse has been left double clicked
	bool rightDoubleClick; // Mouse has been right double clicked
	bool middleClick; // Mouse has been middle clicked
	bool middleDoubleClick; // Mouse has been middle Double Clicked

	bool leftMouseDrag; // Mouse is left Dragging
	bool rightMouseDrag; // Mouse is right Dragging

	bool wasLeftMouseDrag; // Mouse is left Dragging
	bool wasRightMouseDrag; // Mouse is right Dragging

	float mouseDragX; // Where we started Dragging
	float mouseDragY; // Where we started Dragging;

	float mouseDragThreshold; // Distance mouse MUST move before a drag is
		// assumed.
	float mouseDblClickThreshold; // Time between clicks maximum to assume
		// double click.

	float mouseLeftUpTime; // Time since last left mouse up.
	float mouseRightUpTime; // Time since last right mouse up.
	float mouseMiddleUpTime; // Time since last right mouse up.

	float mouseLeftHeldTime; // How int32_t the mouse has been down
	float mouseRightHeldTime; // How int32_t the mouse has been down

	uint32_t lastLeftMouseButtonState; // Last Left Mouse Button State
	uint32_t lastRightMouseButtonState; // Last Right Mouse Button State
	uint32_t lastMiddleMouseButtonState; // Last Middle Mouse Button State

	float lastMouseXPosition; // Last Mouse X Position.
	float lastMouseYPosition; // Last Mouse Y Position.

	MouseCursorData* cursors; // Stores the actual cursor data and Info

	bool drawTerrainPointer; // Draw an inverse project cross
	int32_t mouseState; // Which cursor should I draw?
	int32_t mouseFrame; // current Mouse Frame
	uint32_t mouseTextureHandle; // current Mouse Texture handle.
	float mouseFrameLength; // Time in Current Mouse Frame
	float mouseScale; // Scale factor for cursor for depth cueing.

	float viewMulX;
	float viewAddX;
	float viewMulY;
	float viewAddY;

	bool leftMouseJustUp; // leftButtonUp message equivalent
	bool rightMouseJustUp; // right up equivalent

	uint32_t attilaIndex; // Set to 0xffffffff if no Attila Present.

public:
	// Need to have the mouse draw here
	static volatile bool drawMouse; // Should I draw the Mouse Cursor?

	UserInput(void)
	{
		init();
	}

	~UserInput(void)
	{
		destroy();
	}

	void init()
	{
		mouseScale = 1.0f;
		mouseXPosition = mouseYPosition = 0.0;
		mouseXDelta = mouseYDelta = mouseWheelDelta = 0;
		leftClick = rightClick = middleClick = false;
		leftMouseButtonState = rightMouseButtonState = middleMouseButtonState =
			(mouse_input_const::MC2_MOUSE_UP);
		leftMouseDrag = rightMouseDrag = false;
		leftDoubleClick = rightDoubleClick = middleDoubleClick = false;
		mouseDragThreshold = 0.0166667f;
		mouseDblClickThreshold = 0.2f;
		mouseLeftUpTime = mouseRightUpTime = mouseMiddleUpTime = 0.0;
		mouseDragX = mouseDragY = 0.0;
		lastLeftMouseButtonState = lastRightMouseButtonState = lastMiddleMouseButtonState =
			mouse_input_const::MC2_MOUSE_UP;
		lastMouseYPosition = lastMouseXPosition = 0.0;
		drawTerrainPointer = false;
		mouseState = -1;
		mouseFrameLength = 0.0;
		viewMulX = viewMulY = viewAddX = viewAddY = 0.0f;
		leftMouseJustUp = 0;
		rightMouseJustUp = 0;
		attilaIndex = 0xffffffff;
		mouseLeftHeldTime = 0.f;
		mouseRightHeldTime = 0.f;
		cursors = nullptr;
	}

	void initMouseCursors(std::wstring_view mouseFile);

	void destroy(void)
	{
		if (cursors)
		{
			delete cursors;
			cursors = nullptr;
		}
		init();
	}

	void setViewport(float mulX, float mulY, float addX, float addY)
	{
		viewMulX = mulX;
		viewMulY = mulY;
		viewAddX = addX;
		viewAddY = addY;
	}

	//------------------------
	// Keyboard Functionality
	bool getKeyDown(gosEnum_KeyIndex index)
	{
		if ((gos_GetKeyStatus(index) == KEY_HELD) || (gos_GetKeyStatus(index) == KEY_PRESSED))
			return true;
		return (false);
	}

	bool leftShift(void)
	{
		return (getKeyDown(KEY_LSHIFT));
	}
	bool rightShift(void)
	{
		return (getKeyDown(KEY_RSHIFT));
	}
	bool shift(void)
	{
		return (leftShift() || rightShift());
	}
	bool leftAlt(void)
	{
		return (getKeyDown(KEY_LMENU));
	}
	bool rightAlt(void)
	{
		return (getKeyDown(KEY_RMENU));
	}
	bool alt(void)
	{
		return (leftAlt() || rightAlt());
	}
	bool leftCtrl(void)
	{
		return (getKeyDown(KEY_LCONTROL));
	}
	bool rightCtrl(void)
	{
		return (getKeyDown(KEY_RCONTROL));
	}
	bool ctrl(void)
	{
		return (leftCtrl() || rightCtrl());
	}

	//---------------------------------
	// Mouse Functionality
	void setMouseDoubleClickThreshold(float time)
	{
		mouseDblClickThreshold = time;
	}

	void setMouseDragThreshold(float distance)
	{
		mouseDragThreshold = distance;
	}

	int32_t getMouseX(void)
	{
		return float2long(mouseXPosition * viewMulX);
	}
	int32_t getMouseY(void)
	{
		return float2long(mouseYPosition * viewMulY);
	}
	float realMouseX(void)
	{
		return (mouseXPosition * viewMulX);
	}
	float realMouseY(void)
	{
		return (mouseYPosition * viewMulY);
	}
	void setMousePos(float x, float y)
	{
		float xRes = 0.0f;
		float yRes = 0.0f;
		if ((fabs(viewMulX) > Stuff::SMALL) && (fabs(viewMulY) > Stuff::SMALL))
		{
			xRes = x / viewMulX;
			yRes = y / viewMulY;
		}
		gos_SetMousePosition(xRes, yRes);
	}

	int32_t getMouseXDelta(void)
	{
		return mouseXDelta;
	}
	int32_t getMouseYDelta(void)
	{
		return mouseYDelta;
	}
	int32_t getMouseWheelDelta(void)
	{
		return mouseWheelDelta;
	}
	uint32_t getMouseLeftButtonState(void)
	{
		return leftMouseButtonState;
	}
	uint32_t getMouseRightButtonState(void)
	{
		return rightMouseButtonState;
	}
	bool isLeftDrag(void)
	{
		return leftMouseDrag;
	}
	bool isRightDrag(void)
	{
		return rightMouseDrag;
	}
	bool wasLeftDrag(void)
	{
		return wasLeftMouseDrag;
	}
	bool wasRightDrag(void)
	{
		return wasRightMouseDrag;
	}
	float getMouseDragX(void)
	{
		return mouseDragX * viewMulX;
	}
	float getMouseDragY(void)
	{
		return mouseDragY * viewMulY;
	}
	bool isLeftClick(void)
	{
		return (leftClick);
	}
	bool isRightClick(void)
	{
		return (rightClick);
	}

	// bool isLeftHeld() {
	//	return isLeftClick && lastLeftMouseButtonState == MC2_MOUSE_DOWN;
	// }

	bool isLeftDoubleClick(void)
	{
		return leftDoubleClick;
	}

	bool isRightDoubleClick(void)
	{
		return rightDoubleClick;
	}

	bool isMiddleClick(void)
	{
		return middleClick;
	}

	bool isMiddleDoubleClick(void)
	{
		return middleDoubleClick;
	}

	void setMouseCursor(int32_t state);

	int32_t getMouseCursor(void)
	{
		return (mouseState);
	}

	void setMouseFrame(int32_t frameNum)
	{
		mouseFrame = frameNum;
	}

	int32_t getMouseFrame(void)
	{
		return (mouseFrame);
	}

	void mouseOn(void); // Draw Mouse Cursor
	void mouseOff(void); // Don't Draw Mouse Cursor

	void pointerOn(void)
	{
		drawTerrainPointer = true;
	}

	void pointerOff(void)
	{
		drawTerrainPointer = false;
	}

	bool leftMouseReleased()
	{
		return leftMouseJustUp;
	}

	bool rightMouseReleased()
	{
		return rightMouseJustUp;
	}

	//------------------------------------------------------------------------------------
	// Used to make mouse move off into distance in perspective to help depth
	// perception
	void setMouseScale(float scaleFactor);

	//----------------------------------------------------------
	// Attila Functionality.
	void addAttila(uint32_t joyIndex)
	{
		attilaIndex = joyIndex;
	}

	float getAttilaXAxis(void) // Left/Right Scroll
	{
		float result = 0.0f;
		if (attilaIndex != 0xffffffff)
			result = gosJoystick_GetAxis(attilaIndex, JOY_XAXIS);
		return result;
	}

	float getAttilaYAxis(void) // Up/Down Scroll
	{
		float result = 0.0f;
		if (attilaIndex != 0xffffffff)
			result = gosJoystick_GetAxis(attilaIndex, JOY_YAXIS);
		return result;
	}

	float getAttilaRAxis(void) // Camera Rotation
	{
		float result = 0.0f;
		if (attilaIndex != 0xffffffff)
			result = gosJoystick_GetAxis(attilaIndex, JOY_RZAXIS);
		return result;
	}

	float getMouseRightHeld()
	{
		return mouseRightHeldTime;
	}

	float getMouseLeftHeld()
	{
		return mouseLeftHeldTime;
	}

	//------------------------
	// Poller
	void update(void);

	void render(void); // Last thing rendered.  Draws Mouse.
};

extern UserInput* userInput;
//---------------------------------------------------------------------------
#endif