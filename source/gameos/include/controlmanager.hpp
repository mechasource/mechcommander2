//===========================================================================//
// File:	 ControlManager.hpp												 //
// Contents: Control Manager routines										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _CONTROLMANAGER_HPP_
#define _CONTROLMANAGER_HPP_

//
// Mouse handling functions
//
void __stdcall CMCreateMouse(void);
void __stdcall CMUpdateMouse(void);
void __stdcall ReInitControlManager(void);
gosEnum_KeyStatus __stdcall GetStatus(gosEnum_KeyIndex index);

extern uint32_t KeyBoardBuffer[256];
extern uint32_t KeyCurrent; // Where the next key to be read is
extern uint32_t KeyPressed; // Where the next key pressed will go
extern uint32_t dbKeyBoardBuffer[256];
extern uint32_t dbKeyCurrent; // Where the next key to be read is
extern uint32_t dbKeyPressed; // Where the next key pressed will go
extern uint32_t LastWMDown; // Last WM_KEYDOWN message scan code
extern void __stdcall SaveOldKeyState(void);
extern void __stdcall DoKeyReleased(uint32_t KeyCode);
extern void __stdcall DealWithKey(uint32_t wparam, uint32_t lparam);
extern void __stdcall AddKeyEvent(uint32_t KeyEvent);

#define MAX_DI_DEVICES 16

typedef struct _cmd
{
	LPDIRECTINPUT7 m_lpDirectInput7;

	LPDIRECTINPUTDEVICE7 m_lpSysMouse7;
	LPDIRECTINPUTDEVICE7 m_lpJoystick7[MAX_DI_DEVICES];

	BOOLEAN m_disabled[MAX_DI_DEVICES];
	BOOLEAN m_validAxis[MAX_DI_DEVICES][12];
	BOOLEAN m_pollMe[MAX_DI_DEVICES];
	float m_howOften[MAX_DI_DEVICES];
	double m_nextPoll[MAX_DI_DEVICES];
	BOOLEAN m_bForceFeedback[MAX_DI_DEVICES];

	BOOLEAN m_numJoysticks;
	BOOLEAN m_numMouseButtons;
	BOOLEAN m_numMouseAxes;
	BOOLEAN m_numPOVs[MAX_DI_DEVICES];
	BOOLEAN m_numSliders[MAX_DI_DEVICES];
	BOOLEAN m_numButtons[MAX_DI_DEVICES];
	int32_t m_mousePos[3];
	int32_t m_mouseAxis[3];
	int32_t m_joyAxis[MAX_DI_DEVICES][12];

	BOOLEAN m_joyButton[MAX_DI_DEVICES][64];
	char m_joyName[MAX_DI_DEVICES][64];
	gosEnum_KeyStatus m_mouseButton[4];
	BOOLEAN m_requiresAcquire;
	HANDLE m_hMouseEvent;
} ControlManagerDataStruct;

void __stdcall CMInstall(void);
void __stdcall CMUninstall(void);
void __stdcall CMCreateKeyboard(void);
void __stdcall CMReadJoystick(uint32_t index);
void __stdcall CMReadKeyboard(void);
void __stdcall CMGetKeyState(void);
void __stdcall CMRestore(void);
void __stdcall CMReleaseControls(void);
void __stdcall CMAcquireControls(void);
void __stdcall CMUnacquireControls(void);
void __stdcall CMUpdate(void);
void __stdcall CMCreateJoysticks(void);
BOOL __stdcall CMCreateJoystick7(LPCDIDEVICEINSTANCE pdinst, PVOID pvRef);

// This data structure holds all the information necessary for setting up,
// using, and uninstalling joysticks, a keyboard, and a mouse.
extern ControlManagerDataStruct g_CMData;

#endif
