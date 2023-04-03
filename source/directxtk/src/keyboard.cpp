//--------------------------------------------------------------------------------------
// File: Keyboard.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "Keyboard.h"

#include "platformhelpers.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;

static_assert(sizeof(Keyboard::State) == (256 / 8), "Size mismatch for State");

namespace
{
void KeyDown(int32_t key, Keyboard::State& state) noexcept
{
	if (key < 0 || key > 0xfe)
		return;

	auto ptr = reinterpret_cast<uint32_t*>(&state);

	uint32_t bf = 1u << (key & 0x1f);
	ptr[(key >> 5)] |= bf;
}

void KeyUp(int32_t key, Keyboard::State& state) noexcept
{
	if (key < 0 || key > 0xfe)
		return;

	auto ptr = reinterpret_cast<uint32_t*>(&state);

	uint32_t bf = 1u << (key & 0x1f);
	ptr[(key >> 5)] &= ~bf;
}
} // namespace

#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)

//======================================================================================
// Win32 desktop implementation
//======================================================================================

//
// For a Win32 desktop application, call this function from your Window Message Procedure
//
// LRESULT CALLBACK WndProc(HWND hWnd, uint32_t message, WPARAM wParam, LPARAM lParam)
// {
//     switch (message)
//     {
//
//     case WM_ACTIVATEAPP:
//         Keyboard::ProcessMessage(message, wParam, lParam);
//         break;
//
//     case WM_KEYDOWN:
//     case WM_SYSKEYDOWN:
//     case WM_KEYUP:
//     case WM_SYSKEYUP:
//         Keyboard::ProcessMessage(message, wParam, lParam);
//         break;
//
//     }
// }
//

class Keyboard::Impl
{
public:
	Impl(Keyboard* owner)
		: mState {}
		, mOwner(owner)
	{
		if (s_keyboard)
		{
			throw std::exception("Keyboard is a singleton");
		}

		s_keyboard = this;
	}

	Impl(Impl&&) = default;
	Impl& operator=(Impl&&) = default;

	Impl(Impl const&) = delete;
	Impl& operator=(Impl const&) = delete;

	~Impl()
	{
		s_keyboard = nullptr;
	}

	void GetState(State& state) const
	{
		memcpy(&state, &mState, sizeof(State));
	}

	void reset() noexcept
	{
		memset(&mState, 0, sizeof(State));
	}

	bool IsConnected() const
	{
		return true;
	}

	State mState;
	Keyboard* mOwner;

	static Keyboard::Impl* s_keyboard;
};

Keyboard::Impl* Keyboard::Impl::s_keyboard = nullptr;

void Keyboard::ProcessMessage(uint32_t message, WPARAM wParam, LPARAM lParam)
{
	auto pimpl = Impl::s_keyboard;

	if (!pimpl)
		return;

	bool down = false;

	switch (message)
	{
	case WM_ACTIVATEAPP:
		pimpl->reset();
		return;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		down = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;

	default:
		return;
	}

	int32_t vk = static_cast<int32_t>(wParam);
	switch (vk)
	{
	case VK_SHIFT:
		vk = static_cast<int32_t>(
			MapVirtualKey((static_cast<uint32_t>(lParam) & 0x00ff0000) >> 16u,
				MAPVK_VSC_TO_VK_EX));
		if (!down)
		{
			// Workaround to ensure left vs. right shift get cleared when both were pressed at same time
			KeyUp(VK_LSHIFT, pimpl->mState);
			KeyUp(VK_RSHIFT, pimpl->mState);
		}
		break;

	case VK_CONTROL:
		vk = (static_cast<uint32_t>(lParam) & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vk = (static_cast<uint32_t>(lParam) & 0x01000000) ? VK_RMENU : VK_LMENU;
		break;
	}

	if (down)
	{
		KeyDown(vk, pimpl->mState);
	}
	else
	{
		KeyUp(vk, pimpl->mState);
	}
}

#else

//======================================================================================
// Windows Store or Universal Windows Platform (UWP) app implementation
//======================================================================================

//
// For a Windows Store app or Universal Windows Platform (UWP) app, add the following:
//
// void App::SetWindow(CoreWindow^ window )
// {
//     m_keyboard->SetWindow(window);
// }
//

#include <Windows.Devices.Input.h>

class Keyboard::Impl
{
public:
	Impl(Keyboard* owner)
		: mState {}
		, mOwner(owner)
		, mAcceleratorKeyToken {}
		, mActivatedToken {}
	{
		if (s_keyboard)
		{
			throw std::exception("Keyboard is a singleton");
		}

		s_keyboard = this;
	}

	~Impl()
	{
		s_keyboard = nullptr;

		RemoveHandlers();
	}

	void GetState(State& state) const
	{
		memcpy(&state, &mState, sizeof(State));
	}

	void reset() noexcept
	{
		memset(&mState, 0, sizeof(State));
	}

	bool IsConnected() const
	{
		using namespace Microsoft::WRL;
		using namespace Microsoft::WRL::Wrappers;
		using namespace ABI::Windows::Devices::Input;
		using namespace ABI::Windows::Foundation;

		wil::com_ptr<IKeyboardCapabilities> caps;
		HRESULT hr = RoActivateInstance(HStringReference(RuntimeClass_Windows_Devices_Input_KeyboardCapabilities).get(), &caps);
		ThrowIfFailed(hr);

		int32_t value;
		if (SUCCEEDED(caps->get_KeyboardPresent(&value)))
		{
			return value != 0;
		}

		return false;
	}

	void SetWindow(ABI::Windows::UI::Core::ICoreWindow* window)
	{
		using namespace Microsoft::WRL;
		using namespace Microsoft::WRL::Wrappers;
		using namespace ABI::Windows::UI::Core;

		if (mWindow.get() == window)
			return;

		RemoveHandlers();

		mWindow = window;

		if (!window)
			return;

		typedef __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CWindowActivatedEventArgs ActivatedHandler;
		HRESULT hr = window->add_Activated(Callback<ActivatedHandler>(Activated).get(), &mActivatedToken);
		ThrowIfFailed(hr);

		wil::com_ptr<ICoreDispatcher> dispatcher;
		hr = window->get_Dispatcher(dispatcher.addressof());
		ThrowIfFailed(hr);

		wil::com_ptr<ICoreAcceleratorKeys> keys;
		hr = dispatcher.query(&keys);
		ThrowIfFailed(hr);

		typedef __FITypedEventHandler_2_Windows__CUI__CCore__CCoreDispatcher_Windows__CUI__CCore__CAcceleratorKeyEventArgs AcceleratorKeyHandler;
		hr = keys->add_AcceleratorKeyActivated(Callback<AcceleratorKeyHandler>(AcceleratorKeyEvent).get(), &mAcceleratorKeyToken);
		ThrowIfFailed(hr);
	}

	State mState;
	Keyboard* mOwner;

	static Keyboard::Impl* s_keyboard;

private:
	wil::com_ptr<ABI::Windows::UI::Core::ICoreWindow> mWindow;

	EventRegistrationToken mAcceleratorKeyToken;
	EventRegistrationToken mActivatedToken;

	void RemoveHandlers()
	{
		if (mWindow)
		{
			using namespace ABI::Windows::UI::Core;

			wil::com_ptr<ICoreDispatcher> dispatcher;
			HRESULT hr = mWindow->get_Dispatcher(dispatcher.addressof());
			ThrowIfFailed(hr);

			(void)mWindow->remove_Activated(mActivatedToken);
			mActivatedToken.value = 0;

			wil::com_ptr<ICoreAcceleratorKeys> keys;
			hr = dispatcher.query(&keys);
			ThrowIfFailed(hr);

			(void)keys->remove_AcceleratorKeyActivated(mAcceleratorKeyToken);
			mAcceleratorKeyToken.value = 0;
		}
	}

	static HRESULT Activated(IInspectable*, ABI::Windows::UI::Core::IWindowActivatedEventArgs*)
	{
		auto pimpl = Impl::s_keyboard;

		if (!pimpl)
			return S_OK;

		pimpl->reset();

		return S_OK;
	}

	static HRESULT AcceleratorKeyEvent(IInspectable*, ABI::Windows::UI::Core::IAcceleratorKeyEventArgs* args)
	{
		using namespace ABI::Windows::System;
		using namespace ABI::Windows::UI::Core;

		auto pimpl = Impl::s_keyboard;

		if (!pimpl)
			return S_OK;

		CoreAcceleratorKeyEventType evtType;
		HRESULT hr = args->get_EventType(&evtType);
		ThrowIfFailed(hr);

		bool down = false;

		switch (evtType)
		{
		case CoreAcceleratorKeyEventType_KeyDown:
		case CoreAcceleratorKeyEventType_SystemKeyDown:
			down = true;
			break;

		case CoreAcceleratorKeyEventType_KeyUp:
		case CoreAcceleratorKeyEventType_SystemKeyUp:
			break;

		default:
			return S_OK;
		}

		CorePhysicalKeyStatus status;
		hr = args->get_KeyStatus(&status);
		ThrowIfFailed(hr);

		VirtualKey virtualKey;
		hr = args->get_VirtualKey(&virtualKey);
		ThrowIfFailed(hr);

		int32_t vk = static_cast<int32_t>(virtualKey);

		switch (vk)
		{
		case VK_SHIFT:
			vk = (status.ScanCode == 0x36) ? VK_RSHIFT : VK_LSHIFT;
			if (!down)
			{
				// Workaround to ensure left vs. right shift get cleared when both were pressed at same time
				KeyUp(VK_LSHIFT, pimpl->mState);
				KeyUp(VK_RSHIFT, pimpl->mState);
			}
			break;

		case VK_CONTROL:
			vk = (status.IsExtendedKey) ? VK_RCONTROL : VK_LCONTROL;
			break;

		case VK_MENU:
			vk = (status.IsExtendedKey) ? VK_RMENU : VK_LMENU;
			break;
		}

		if (down)
		{
			KeyDown(vk, pimpl->mState);
		}
		else
		{
			KeyUp(vk, pimpl->mState);
		}

		return S_OK;
	}
};

Keyboard::Impl* Keyboard::Impl::s_keyboard = nullptr;

void Keyboard::SetWindow(ABI::Windows::UI::Core::ICoreWindow* window)
{
	pimpl->SetWindow(window);
}

#endif

#pragma warning(disable : 4355)

// Public constructor.
Keyboard::Keyboard() noexcept(false)
	: pimpl(std::make_unique<Impl>(this))
{
}

// Move constructor.
Keyboard::Keyboard(Keyboard&& moveFrom) noexcept
	: pimpl(std::move(moveFrom.pimpl))
{
	pimpl->mOwner = this;
}

// Move assignment.
Keyboard& Keyboard::operator=(Keyboard&& moveFrom) noexcept
{
	pimpl = std::move(moveFrom.pimpl);
	pimpl->mOwner = this;
	return *this;
}

// Public destructor.
Keyboard::~Keyboard()
{
}

Keyboard::State Keyboard::GetState() const
{
	State state;
	pimpl->GetState(state);
	return state;
}

void Keyboard::reset() noexcept
{
	pimpl->reset();
}

bool Keyboard::IsConnected() const
{
	return pimpl->IsConnected();
}

Keyboard& Keyboard::Get()
{
	if (!Impl::s_keyboard || !Impl::s_keyboard->mOwner)
		throw std::exception("Keyboard is a singleton");

	return *Impl::s_keyboard->mOwner;
}

//======================================================================================
// KeyboardStateTracker
//======================================================================================

void Keyboard::KeyboardStateTracker::Update(const State& state) noexcept
{
	auto currPtr = reinterpret_cast<const uint32_t*>(&state);
	auto prevPtr = reinterpret_cast<const uint32_t*>(&lastState);
	auto releasedPtr = reinterpret_cast<uint32_t*>(&released);
	auto pressedPtr = reinterpret_cast<uint32_t*>(&pressed);
	for (auto j = 0u; j < (256 / 32); ++j)
	{
		*pressedPtr = *currPtr & ~(*prevPtr);
		*releasedPtr = ~(*currPtr) & *prevPtr;

		++currPtr;
		++prevPtr;
		++releasedPtr;
		++pressedPtr;
	}

	lastState = state;
}

void Keyboard::KeyboardStateTracker::reset() noexcept
{
	memset(this, 0, sizeof(KeyboardStateTracker));
}
