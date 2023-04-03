//--------------------------------------------------------------------------------------
// File: GamePad.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#pragma once

#if (_WIN32_WINNT < 0x0A00 /*_WIN32_WINNT_WIN10*/)
#ifndef _XBOX_ONE
#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP)
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#pragma comment(lib, "xinput.lib")
#else
#pragma comment(lib, "xinput9_1_0.lib")
#endif
#endif
#endif
#endif

#include <cstdint>
#include <memory>

#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
#pragma comment(lib, "runtimeobject.lib")
#include <string>
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif

namespace directxtk
{
class GamePad
{
public:
	GamePad() noexcept(false);
	GamePad(GamePad&& moveFrom) noexcept;
	GamePad& operator=(GamePad&& moveFrom) noexcept;

	GamePad(GamePad const&) = delete;
	GamePad& operator=(GamePad const&) = delete;

	virtual ~GamePad();

#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/) || defined(_XBOX_ONE)
	static constexpr int32_t MAX_PLAYER_COUNT = 8;
#else
	static constexpr int32_t MAX_PLAYER_COUNT = 4;
#endif

	enum DeadZone
	{
		DEAD_ZONE_INDEPENDENT_AXES = 0,
		DEAD_ZONE_CIRCULAR,
		DEAD_ZONE_NONE,
	};

	struct Buttons
	{
		bool a;
		bool b;
		bool x;
		bool y;
		bool leftStick;
		bool rightStick;
		bool leftShoulder;
		bool rightShoulder;
		union
		{
			bool back;
			bool view;
		};
		union
		{
			bool start;
			bool menu;
		};
	};

	struct DPad
	{
		bool up;
		bool down;
		bool right;
		bool left;
	};

	struct ThumbSticks
	{
		float leftX;
		float leftY;
		float rightX;
		float rightY;
	};

	struct Triggers
	{
		float left;
		float right;
	};

	struct State
	{
		bool connected;
		uint64_t packet;
		Buttons buttons;
		DPad dpad;
		ThumbSticks thumbSticks;
		Triggers triggers;

		bool __cdecl IsConnected() const noexcept
		{
			return connected;
		}

		// Is the button pressed currently?
		bool __cdecl IsAPressed() const noexcept
		{
			return buttons.a;
		}
		bool __cdecl IsBPressed() const noexcept
		{
			return buttons.b;
		}
		bool __cdecl IsXPressed() const noexcept
		{
			return buttons.x;
		}
		bool __cdecl IsYPressed() const noexcept
		{
			return buttons.y;
		}

		bool __cdecl IsLeftStickPressed() const noexcept
		{
			return buttons.leftStick;
		}
		bool __cdecl IsRightStickPressed() const noexcept
		{
			return buttons.rightStick;
		}

		bool __cdecl IsLeftShoulderPressed() const noexcept
		{
			return buttons.leftShoulder;
		}
		bool __cdecl IsRightShoulderPressed() const noexcept
		{
			return buttons.rightShoulder;
		}

		bool __cdecl IsBackPressed() const noexcept
		{
			return buttons.back;
		}
		bool __cdecl IsViewPressed() const noexcept
		{
			return buttons.view;
		}
		bool __cdecl IsStartPressed() const noexcept
		{
			return buttons.start;
		}
		bool __cdecl IsMenuPressed() const noexcept
		{
			return buttons.menu;
		}

		bool __cdecl IsDPadDownPressed() const noexcept
		{
			return dpad.down;
		}
		bool __cdecl IsDPadUpPressed() const noexcept
		{
			return dpad.up;
		}
		bool __cdecl IsDPadLeftPressed() const noexcept
		{
			return dpad.left;
		}
		bool __cdecl IsDPadRightPressed() const noexcept
		{
			return dpad.right;
		}

		bool __cdecl IsLeftThumbStickUp() const noexcept
		{
			return (thumbSticks.leftY > 0.5f) != 0;
		}
		bool __cdecl IsLeftThumbStickDown() const noexcept
		{
			return (thumbSticks.leftY < -0.5f) != 0;
		}
		bool __cdecl IsLeftThumbStickLeft() const noexcept
		{
			return (thumbSticks.leftX < -0.5f) != 0;
		}
		bool __cdecl IsLeftThumbStickRight() const noexcept
		{
			return (thumbSticks.leftX > 0.5f) != 0;
		}

		bool __cdecl IsRightThumbStickUp() const noexcept
		{
			return (thumbSticks.rightY > 0.5f) != 0;
		}
		bool __cdecl IsRightThumbStickDown() const noexcept
		{
			return (thumbSticks.rightY < -0.5f) != 0;
		}
		bool __cdecl IsRightThumbStickLeft() const noexcept
		{
			return (thumbSticks.rightX < -0.5f) != 0;
		}
		bool __cdecl IsRightThumbStickRight() const noexcept
		{
			return (thumbSticks.rightX > 0.5f) != 0;
		}

		bool __cdecl IsLeftTriggerPressed() const noexcept
		{
			return (triggers.left > 0.5f) != 0;
		}
		bool __cdecl IsRightTriggerPressed() const noexcept
		{
			return (triggers.right > 0.5f) != 0;
		}
	};

	struct Capabilities
	{
		enum Type
		{
			UNKNOWN = 0,
			GAMEPAD,
			WHEEL,
			ARCADE_STICK,
			FLIGHT_STICK,
			DANCE_PAD,
			GUITAR,
			GUITAR_ALTERNATE,
			DRUM_KIT,
			GUITAR_BASS = 11,
			ARCADE_PAD = 19,
		};

		bool connected;
		Type gamepadType;
#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
		std::wstring id;
#else
		uint64_t id;
#endif
		uint16_t vid;
		uint16_t pid;

		Capabilities() noexcept
			: connected(false)
			, gamepadType(UNKNOWN)
			, id {}
			, vid(0)
			, pid(0)
		{
		}

		bool __cdecl IsConnected() const noexcept
		{
			return connected;
		}
	};

	class ButtonStateTracker
	{
	public:
		enum ButtonState
		{
			UP = 0, // Button is up
			HELD = 1, // Button is held down
			RELEASED = 2, // Button was just released
			PRESSED = 3, // Buton was just pressed
		};

		ButtonState a;
		ButtonState b;
		ButtonState x;
		ButtonState y;

		ButtonState leftStick;
		ButtonState rightStick;

		ButtonState leftShoulder;
		ButtonState rightShoulder;

		union
		{
			ButtonState back;
			ButtonState view;
		};

		union
		{
			ButtonState start;
			ButtonState menu;
		};

		ButtonState dpadUp;
		ButtonState dpadDown;
		ButtonState dpadLeft;
		ButtonState dpadRight;

		ButtonState leftStickUp;
		ButtonState leftStickDown;
		ButtonState leftStickLeft;
		ButtonState leftStickRight;

		ButtonState rightStickUp;
		ButtonState rightStickDown;
		ButtonState rightStickLeft;
		ButtonState rightStickRight;

		ButtonState leftTrigger;
		ButtonState rightTrigger;

#pragma prefast(suppress : 26495, "reset() performs the initialization")
		ButtonStateTracker() noexcept
		{
			reset();
		}

		void __cdecl Update(const State& state) noexcept;

		void __cdecl reset() noexcept;

		State __cdecl GetLastState() const noexcept
		{
			return lastState;
		}

	private:
		State lastState;
	};

	// Retrieve the current state of the gamepad of the associated player index
	State __cdecl GetState(int32_t player, DeadZone deadZoneMode = DEAD_ZONE_INDEPENDENT_AXES);

	// Retrieve the current capabilities of the gamepad of the associated player index
	Capabilities __cdecl GetCapabilities(int32_t player);

	// Set the vibration motor speeds of the gamepad
	bool __cdecl SetVibration(int32_t player, float leftMotor, float rightMotor, float leftTrigger = 0.f, float rightTrigger = 0.f) noexcept;

	// Handle suspending/resuming
	void __cdecl Suspend() noexcept;
	void __cdecl Resume() noexcept;

#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/) || defined(_XBOX_ONE)
	void __cdecl RegisterEvents(void* ctrlChanged, void* userChanged) noexcept;
#endif

	// Singleton
	static GamePad& __cdecl Get();

private:
	// Private implementation.
	class Impl;

	std::unique_ptr<Impl> pimpl;
};
} // namespace directxtk

#ifdef __clang__
#pragma clang diagnostic pop
#endif
