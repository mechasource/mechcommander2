//--------------------------------------------------------------------------------------
// File: Mouse.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//--------------------------------------------------------------------------------------

#include "stdinc.h"
#include "Mouse.h"

#include "platformhelpers.h"

using namespace directxtk;
// using Microsoft::WRL::ComPtr;


#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)

//======================================================================================
// Win32 desktop implementation
//======================================================================================

//
// For a Win32 desktop application, in your window setup be sure to call this method:
//
// m_mouse->SetWindow(hwnd);
//
// And call this static function from your Window Message Procedure
//
// LRESULT CALLBACK WndProc(HWND hWnd, uint32_t message, WPARAM wParam, LPARAM lParam)
// {
//     switch (message)
//     {
//     case WM_ACTIVATEAPP:
//     case WM_INPUT:
//     case WM_MOUSEMOVE:
//     case WM_LBUTTONDOWN:
//     case WM_LBUTTONUP:
//     case WM_RBUTTONDOWN:
//     case WM_RBUTTONUP:
//     case WM_MBUTTONDOWN:
//     case WM_MBUTTONUP:
//     case WM_MOUSEWHEEL:
//     case WM_XBUTTONDOWN:
//     case WM_XBUTTONUP:
//     case WM_MOUSEHOVER:
//         Mouse::ProcessMessage(message, wParam, lParam);
//         break;
//
//     }
// }
//

class Mouse::Impl
{
public:
    Impl(Mouse* owner) :
        mState{},
        mOwner(owner),
        mWindow(nullptr),
        mMode(MODE_ABSOLUTE),
        mLastX(0),
        mLastY(0),
        mRelativeX(INT32_MAX),
        mRelativeY(INT32_MAX),
        mInFocus(true)
    {
        if (s_mouse)
        {
            throw std::exception("Mouse is a singleton");
        }

        s_mouse = this;

        m_scrollwheelevent.reset(::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
        m_relativereadevent.reset(::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
        m_absolutemodeevent.reset(::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
        m_relativemodeevent.reset(::CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
        if (!m_scrollwheelevent
            || !m_relativereadevent
            || !m_absolutemodeevent
            || !m_relativemodeevent)
        {
            throw std::exception("::CreateEventEx");
        }
    }

    Impl(Impl&&) = default;
    Impl& operator= (Impl&&) = default;

    Impl(Impl const&) = delete;
    Impl& operator= (Impl const&) = delete;

    ~Impl()
    {
        s_mouse = nullptr;
    }

    void GetState(State& state) const
    {
        memcpy(&state, &mState, sizeof(State));
        state.positionMode = mMode;

        uint32_t result = WaitForSingleObjectEx(m_scrollwheelevent.get(), 0, FALSE);
        if (result == WAIT_FAILED)
            throw std::exception("WaitForSingleObjectEx");

        if (result == WAIT_OBJECT_0)
        {
            state.scrollWheelValue = 0;
        }

        if (state.positionMode == MODE_RELATIVE)
        {
            result = WaitForSingleObjectEx(m_relativereadevent.get(), 0, FALSE);

            if (result == WAIT_FAILED)
                throw std::exception("WaitForSingleObjectEx");

            if (result == WAIT_OBJECT_0)
            {
                state.x = 0;
                state.y = 0;
            }
            else
            {
                ::SetEvent(m_relativereadevent.get());
            }
        }
    }

    void ResetScrollWheelValue() noexcept
    {
        ::SetEvent(m_scrollwheelevent.get());
    }

    void SetMode(Mode mode)
    {
        if (mMode == mode)
            return;

        ::SetEvent((mode == MODE_ABSOLUTE) ? m_absolutemodeevent.get() : m_relativemodeevent.get());

        assert(mWindow != nullptr);

        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_HOVER;
        tme.hwndTrack = mWindow;
        tme.dwHoverTime = 1;
        if (!TrackMouseEvent(&tme))
        {
            throw std::exception("TrackMouseEvent");
        }
    }

    bool IsConnected() const noexcept
    {
        return GetSystemMetrics(SM_MOUSEPRESENT) != 0;
    }

    bool IsVisible() const
    {
        if (mMode == MODE_RELATIVE)
            return false;

        CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
        if (!GetCursorInfo(&info))
        {
            throw std::exception("GetCursorInfo");
        }

        return (info.flags & CURSOR_SHOWING) != 0;
    }

    void SetVisible(bool visible)
    {
        if (mMode == MODE_RELATIVE)
            return;

        CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
        if (!GetCursorInfo(&info))
        {
            throw std::exception("GetCursorInfo");
        }

        bool isvisible = (info.flags & CURSOR_SHOWING) != 0;
        if (isvisible != visible)
        {
            ShowCursor(visible);
        }
    }

    void SetWindow(HWND window)
    {
        if (mWindow == window)
            return;

        assert(window != nullptr);

        RAWINPUTDEVICE Rid;
        Rid.usUsagePage = 0x1 /* HID_USAGE_PAGE_GENERIC */;
        Rid.usUsage = 0x2 /* HID_USAGE_GENERIC_MOUSE */;
        Rid.dwFlags = RIDEV_INPUTSINK;
        Rid.hwndTarget = window;
        if (!RegisterRawInputDevices(&Rid, 1, sizeof(RAWINPUTDEVICE)))
        {
            throw std::exception("RegisterRawInputDevices");
        }

        mWindow = window;
    }

    State           mState;

    Mouse*          mOwner;

    static Mouse::Impl* s_mouse;

private:
    HWND            mWindow;
    Mode            mMode;

    wil::unique_handle    m_scrollwheelevent;
    wil::unique_handle    m_relativereadevent;
    wil::unique_handle    m_absolutemodeevent;
    wil::unique_handle    m_relativemodeevent;

    int32_t             mLastX;
    int32_t             mLastY;
    int32_t             mRelativeX;
    int32_t             mRelativeY;

    bool            mInFocus;

    friend void Mouse::ProcessMessage(uint32_t message, WPARAM wParam, LPARAM lParam);

    void ClipToWindow() noexcept
    {
        assert(mWindow != nullptr);

        RECT rect;
        GetClientRect(mWindow, &rect);

        POINT ul;
        ul.x = rect.left;
        ul.y = rect.top;

        POINT lr;
        lr.x = rect.right;
        lr.y = rect.bottom;

        MapWindowPoints(mWindow, nullptr, &ul, 1);
        MapWindowPoints(mWindow, nullptr, &lr, 1);

        rect.left = ul.x;
        rect.top = ul.y;

        rect.right = lr.x;
        rect.bottom = lr.y;

        ClipCursor(&rect);
    }
};


Mouse::Impl* Mouse::Impl::s_mouse = nullptr;


void Mouse::SetWindow(HWND window)
{
    pimpl->SetWindow(window);
}


void Mouse::ProcessMessage(uint32_t message, WPARAM wParam, LPARAM lParam)
{
    auto pimpl = Impl::s_mouse;

    if (!pimpl)
        return;

    HANDLE events[3] = { pimpl->m_scrollwheelevent.get(), pimpl->m_absolutemodeevent.get(), pimpl->m_relativemodeevent.get() };
    switch (WaitForMultipleObjectsEx(_countof(events), events, FALSE, 0, FALSE))
    {
        default:
        case WAIT_TIMEOUT:
            break;

        case WAIT_OBJECT_0:
            pimpl->mState.scrollWheelValue = 0;
            ResetEvent(events[0]);
            break;

        case (WAIT_OBJECT_0 + 1):
        {
            pimpl->mMode = MODE_ABSOLUTE;
            ClipCursor(nullptr);

            POINT point;
            point.x = pimpl->mLastX;
            point.y = pimpl->mLastY;

            // We show the cursor before moving it to support Remote Desktop
            ShowCursor(TRUE);

            if (MapWindowPoints(pimpl->mWindow, nullptr, &point, 1))
            {
                SetCursorPos(point.x, point.y);
            }
            pimpl->mState.x = pimpl->mLastX;
            pimpl->mState.y = pimpl->mLastY;
        }
        break;

        case (WAIT_OBJECT_0 + 2):
        {
            ResetEvent(pimpl->m_relativereadevent.get());

            pimpl->mMode = MODE_RELATIVE;
            pimpl->mState.x = pimpl->mState.y = 0;
            pimpl->mRelativeX = INT32_MAX;
            pimpl->mRelativeY = INT32_MAX;

            ShowCursor(FALSE);

            pimpl->ClipToWindow();
        }
        break;

        case WAIT_FAILED:
            throw std::exception("WaitForMultipleObjectsEx");
    }

    switch (message)
    {
        case WM_ACTIVATEAPP:
            if (wParam)
            {
                pimpl->mInFocus = true;

                if (pimpl->mMode == MODE_RELATIVE)
                {
                    pimpl->mState.x = pimpl->mState.y = 0;

                    ShowCursor(FALSE);

                    pimpl->ClipToWindow();
                }
            }
            else
            {
                int32_t scrollWheel = pimpl->mState.scrollWheelValue;
                memset(&pimpl->mState, 0, sizeof(State));
                pimpl->mState.scrollWheelValue = scrollWheel;

                pimpl->mInFocus = false;
            }
            return;

        case WM_INPUT:
            if (pimpl->mInFocus && pimpl->mMode == MODE_RELATIVE)
            {
                RAWINPUT raw;
                uint32_t rawSize = sizeof(raw);

                uint32_t resultData = GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &raw, &rawSize, sizeof(RAWINPUTHEADER));
                if (resultData == uint32_t(-1))
                {
                    throw std::exception("GetRawInputData");
                }

                if (raw.header.dwType == RIM_TYPEMOUSE)
                {
                    if (!(raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
                    {
                        pimpl->mState.x = raw.data.mouse.lLastX;
                        pimpl->mState.y = raw.data.mouse.lLastY;

                        ResetEvent(pimpl->m_relativereadevent.get());
                    }
                    else if (raw.data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP)
                    {
                        // This is used to make Remote Desktop sessons work
                        const int32_t width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                        const int32_t height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

                        int32_t x = static_cast<int32_t>((float(raw.data.mouse.lLastX) / 65535.0f) * float(width));
                        int32_t y = static_cast<int32_t>((float(raw.data.mouse.lLastY) / 65535.0f) * float(height));

                        if (pimpl->mRelativeX == INT32_MAX)
                        {
                            pimpl->mState.x = pimpl->mState.y = 0;
                        }
                        else
                        {
                            pimpl->mState.x = x - pimpl->mRelativeX;
                            pimpl->mState.y = y - pimpl->mRelativeY;
                        }

                        pimpl->mRelativeX = x;
                        pimpl->mRelativeY = y;

                        ResetEvent(pimpl->m_relativereadevent.get());
                    }
                }
            }
            return;

        case WM_MOUSEMOVE:
            break;

        case WM_LBUTTONDOWN:
            pimpl->mState.leftButton = true;
            break;

        case WM_LBUTTONUP:
            pimpl->mState.leftButton = false;
            break;

        case WM_RBUTTONDOWN:
            pimpl->mState.rightButton = true;
            break;

        case WM_RBUTTONUP:
            pimpl->mState.rightButton = false;
            break;

        case WM_MBUTTONDOWN:
            pimpl->mState.middleButton = true;
            break;

        case WM_MBUTTONUP:
            pimpl->mState.middleButton = false;
            break;

        case WM_MOUSEWHEEL:
            pimpl->mState.scrollWheelValue += GET_WHEEL_DELTA_WPARAM(wParam);
            return;

        case WM_XBUTTONDOWN:
            switch (GET_XBUTTON_WPARAM(wParam))
            {
                case XBUTTON1:
                    pimpl->mState.xButton1 = true;
                    break;

                case XBUTTON2:
                    pimpl->mState.xButton2 = true;
                    break;
            }
            break;

        case WM_XBUTTONUP:
            switch (GET_XBUTTON_WPARAM(wParam))
            {
                case XBUTTON1:
                    pimpl->mState.xButton1 = false;
                    break;

                case XBUTTON2:
                    pimpl->mState.xButton2 = false;
                    break;
            }
            break;

        case WM_MOUSEHOVER:
            break;

        default:
            // Not a mouse message, so exit
            return;
    }

    if (pimpl->mMode == MODE_ABSOLUTE)
    {
        // All mouse messages provide a new pointer position
        int32_t xPos = static_cast<short>(LOWORD(lParam)); // GET_X_LPARAM(lParam);
        int32_t yPos = static_cast<short>(HIWORD(lParam)); // GET_Y_LPARAM(lParam);

        pimpl->mState.x = pimpl->mLastX = xPos;
        pimpl->mState.y = pimpl->mLastY = yPos;
    }
}


#elif defined(_XBOX_ONE) && (!defined(_TITLE) || (_XDK_VER < 0x42D907D1))

//======================================================================================
// Null device
//======================================================================================

class Mouse::Impl
{
public:
    Impl(Mouse* owner) :
        mOwner(owner)
    {
        if (s_mouse)
        {
            throw std::exception("Mouse is a singleton");
        }

        s_mouse = this;
    }

    ~Impl()
    {
        s_mouse = nullptr;
    }

    void GetState(State& state) const
    {
        memset(&state, 0, sizeof(State));
    }

    void ResetScrollWheelValue() noexcept
    {
    }

    void SetMode(Mode)
    {
    }

    bool IsConnected() const
    {
        return false;
    }

    bool IsVisible() const
    {
        return false;
    }

    void SetVisible(bool)
    {
    }

    Mouse*  mOwner;

    static Mouse::Impl* s_mouse;
};

Mouse::Impl* Mouse::Impl::s_mouse = nullptr;


#else

//======================================================================================
// Windows Store or Universal Windows Platform (UWP) app implementation
//======================================================================================

//
// For a Windows Store app or Universal Windows Platform (UWP) app, add the following to your existing
// application methods:
//
// void App::SetWindow(CoreWindow^ window )
// {
//     m_mouse->SetWindow(window);
// }
// 
// void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
// {
//     m_mouse->SetDpi(sender->LogicalDpi);
// }
//

#include <Windows.Devices.Input.h>

class Mouse::Impl
{
public:
    Impl(Mouse* owner) :
        mState{},
        mOwner(owner),
        mDPI(96.f),
        mMode(MODE_ABSOLUTE),
        mPointerPressedToken{},
        mPointerReleasedToken{},
        mPointerMovedToken{},
        mPointerWheelToken{},
        mPointerMouseMovedToken{}
    {
        if (s_mouse)
        {
            throw std::exception("Mouse is a singleton");
        }

        s_mouse = this;

        m_scrollwheelevent.reset(::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
        m_relativereadevent.reset(::CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE));
        if (!m_scrollwheelevent
            || !m_relativereadevent)
        {
            throw std::exception("::CreateEventEx");
        }
    }

    ~Impl()
    {
        s_mouse = nullptr;

        RemoveHandlers();
    }

    void GetState(State& state) const
    {
        memcpy(&state, &mState, sizeof(State));

        uint32_t result = WaitForSingleObjectEx(m_scrollwheelevent.get(), 0, FALSE);
        if (result == WAIT_FAILED)
            throw std::exception("WaitForSingleObjectEx");

        if (result == WAIT_OBJECT_0)
        {
            state.scrollWheelValue = 0;
        }

        if (mMode == MODE_RELATIVE)
        {
            result = WaitForSingleObjectEx(m_relativereadevent.get(), 0, FALSE);

            if (result == WAIT_FAILED)
                throw std::exception("WaitForSingleObjectEx");

            if (result == WAIT_OBJECT_0)
            {
                state.x = 0;
                state.y = 0;
            }
            else
            {
                ::SetEvent(m_relativereadevent.get());
            }
        }

        state.positionMode = mMode;
    }

    void ResetScrollWheelValue() noexcept
    {
        ::SetEvent(m_scrollwheelevent.get());
    }

    void SetMode(Mode mode)
    {
        using namespace Microsoft::WRL;
        using namespace Microsoft::WRL::Wrappers;
        using namespace ABI::Windows::UI::Core;
        using namespace ABI::Windows::Foundation;

        if (mMode == mode)
            return;

        wil::com_ptr<ICoreWindowStatic> statics;
        HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_Core_CoreWindow).get(), statics.addressof());
        ThrowIfFailed(hr);

        wil::com_ptr<ICoreWindow> window;
        hr = statics->GetForCurrentThread(window.addressof());
        ThrowIfFailed(hr);

        if (mode == MODE_RELATIVE)
        {
            hr = window->get_PointerCursor(mCursor.put());
            ThrowIfFailed(hr);

            hr = window->put_PointerCursor(nullptr);
            ThrowIfFailed(hr);

            ::SetEvent(m_relativereadevent.get());

            mMode = MODE_RELATIVE;
        }
        else
        {
            if (!mCursor)
            {
                wil::com_ptr<ICoreCursorFactory> factory;
                hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_Core_CoreCursor).get(), factory.addressof());
                ThrowIfFailed(hr);

                hr = factory->CreateCursor(CoreCursorType_Arrow, 0, mCursor.addressof());
                ThrowIfFailed(hr);
            }

            hr = window->put_PointerCursor(mCursor.get());
            ThrowIfFailed(hr);

            mCursor.reset();

            mMode = MODE_ABSOLUTE;
        }
    }

    bool IsConnected() const
    {
        using namespace Microsoft::WRL;
        using namespace Microsoft::WRL::Wrappers;
        using namespace ABI::Windows::Devices::Input;
        using namespace ABI::Windows::Foundation;

        wil::com_ptr<IMouseCapabilities> caps;
        HRESULT hr = RoActivateInstance(HStringReference(RuntimeClass_Windows_Devices_Input_MouseCapabilities).get(), &caps);
        ThrowIfFailed(hr);

        int32_t value;
        if (SUCCEEDED(caps->get_MousePresent(&value)))
        {
            return value != 0;
        }

        return false;
    }

    bool IsVisible() const
    {
        if (mMode == MODE_RELATIVE)
            return false;

        wil::com_ptr<ABI::Windows::UI::Core::ICoreCursor> cursor;
        HRESULT hr = mWindow->get_PointerCursor(cursor.addressof());
        ThrowIfFailed(hr);

        return cursor != 0;
    }

    void SetVisible(bool visible)
    {
        using namespace Microsoft::WRL::Wrappers;
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::UI::Core;

        if (mMode == MODE_RELATIVE)
            return;

        if (visible)
        {
            if (!mCursor)
            {
                wil::com_ptr<ICoreCursorFactory> factory;
                HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_Core_CoreCursor).get(), factory.addressof());
                ThrowIfFailed(hr);

                hr = factory->CreateCursor(CoreCursorType_Arrow, 0, mCursor.addressof());
                ThrowIfFailed(hr);
            }

            HRESULT hr = mWindow->put_PointerCursor(mCursor.get());
            ThrowIfFailed(hr);
        }
        else
        {
            HRESULT hr = mWindow->put_PointerCursor(nullptr);
            ThrowIfFailed(hr);
        }
    }

    void SetWindow(ABI::Windows::UI::Core::ICoreWindow* window)
    {
        using namespace Microsoft::WRL;
        using namespace Microsoft::WRL::Wrappers;
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::Devices::Input;

        if (mWindow.get() == window)
            return;

        RemoveHandlers();

        mWindow = window;

        if (!window)
        {
            mCursor.reset();
            mMouse.reset();
            return;
        }

        wil::com_ptr<IMouseDeviceStatics> mouseStatics;
        HRESULT hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Input_MouseDevice).get(), mouseStatics.addressof());
        ThrowIfFailed(hr);

        hr = mouseStatics->GetForCurrentView(mMouse.put());
        ThrowIfFailed(hr);

        typedef __FITypedEventHandler_2_Windows__CDevices__CInput__CMouseDevice_Windows__CDevices__CInput__CMouseEventArgs MouseMovedHandler;
        hr = mMouse->add_MouseMoved(Callback<MouseMovedHandler>(MouseMovedEvent).get(), &mPointerMouseMovedToken);
        ThrowIfFailed(hr);

        typedef __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs PointerHandler;
        auto cb = Callback<PointerHandler>(PointerEvent);

        hr = window->add_PointerPressed(cb.get(), &mPointerPressedToken);
        ThrowIfFailed(hr);

        hr = window->add_PointerReleased(cb.get(), &mPointerReleasedToken);
        ThrowIfFailed(hr);

        hr = window->add_PointerMoved(cb.get(), &mPointerMovedToken);
        ThrowIfFailed(hr);

        hr = window->add_PointerWheelChanged(Callback<PointerHandler>(PointerWheel).get(), &mPointerWheelToken);
        ThrowIfFailed(hr);
    }

    State           mState;
    Mouse*          mOwner;
    float           mDPI;

    static Mouse::Impl* s_mouse;

private:
    Mode            mMode;

    wil::com_ptr<ABI::Windows::UI::Core::ICoreWindow> mWindow;
    wil::com_ptr<ABI::Windows::Devices::Input::IMouseDevice> mMouse;
    wil::com_ptr<ABI::Windows::UI::Core::ICoreCursor> mCursor;

    wil::unique_handle    m_scrollwheelevent;
    wil::unique_handle    m_relativereadevent;

    EventRegistrationToken mPointerPressedToken;
    EventRegistrationToken mPointerReleasedToken;
    EventRegistrationToken mPointerMovedToken;
    EventRegistrationToken mPointerWheelToken;
    EventRegistrationToken mPointerMouseMovedToken;

    void RemoveHandlers()
    {
        if (mWindow)
        {
            (void)mWindow->remove_PointerPressed(mPointerPressedToken);
            mPointerPressedToken.value = 0;

            (void)mWindow->remove_PointerReleased(mPointerReleasedToken);
            mPointerReleasedToken.value = 0;

            (void)mWindow->remove_PointerMoved(mPointerMovedToken);
            mPointerMovedToken.value = 0;

            (void)mWindow->remove_PointerWheelChanged(mPointerWheelToken);
            mPointerWheelToken.value = 0;
        }

        if (mMouse)
        {
            (void)mMouse->remove_MouseMoved(mPointerMouseMovedToken);
            mPointerMouseMovedToken.value = 0;
        }
    }

    static HRESULT PointerEvent(IInspectable *, ABI::Windows::UI::Core::IPointerEventArgs*args)
    {
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::UI::Input;
        using namespace ABI::Windows::Devices::Input;

        if (!s_mouse)
            return S_OK;

        wil::com_ptr<IPointerPoint> currentPoint;
        HRESULT hr = args->get_CurrentPoint(currentPoint.addressof());
        ThrowIfFailed(hr);

        wil::com_ptr<IPointerDevice> pointerDevice;
        hr = currentPoint->get_PointerDevice(pointerDevice.addressof());
        ThrowIfFailed(hr);

        PointerDeviceType devType;
        hr = pointerDevice->get_PointerDeviceType(&devType);
        ThrowIfFailed(hr);

        if (devType == PointerDeviceType::PointerDeviceType_Mouse)
        {
            wil::com_ptr<IPointerPointProperties> props;
            hr = currentPoint->get_Properties(props.addressof());
            ThrowIfFailed(hr);

            boolean value;
            hr = props->get_IsLeftButtonPressed(&value);
            ThrowIfFailed(hr);
            s_mouse->mState.leftButton = value != 0;

            hr = props->get_IsRightButtonPressed(&value);
            ThrowIfFailed(hr);
            s_mouse->mState.rightButton = value != 0;

            hr = props->get_IsMiddleButtonPressed(&value);
            ThrowIfFailed(hr);
            s_mouse->mState.middleButton = value != 0;

            hr = props->get_IsXButton1Pressed(&value);
            ThrowIfFailed(hr);
            s_mouse->mState.xButton1 = value != 0;

            hr = props->get_IsXButton2Pressed(&value);
            ThrowIfFailed(hr);
            s_mouse->mState.xButton2 = value != 0;
        }

        if (s_mouse->mMode == MODE_ABSOLUTE)
        {
            Point pos;
            hr = currentPoint->get_Position(&pos);
            ThrowIfFailed(hr);

            float dpi = s_mouse->mDPI;

            s_mouse->mState.x = static_cast<int32_t>(pos.X * dpi / 96.f + 0.5f);
            s_mouse->mState.y = static_cast<int32_t>(pos.Y * dpi / 96.f + 0.5f);
        }

        return S_OK;
    }

    static HRESULT PointerWheel(IInspectable *, ABI::Windows::UI::Core::IPointerEventArgs*args)
    {
        using namespace ABI::Windows::Foundation;
        using namespace ABI::Windows::UI::Input;
        using namespace ABI::Windows::Devices::Input;

        if (!s_mouse)
            return S_OK;

        wil::com_ptr<IPointerPoint> currentPoint;
        HRESULT hr = args->get_CurrentPoint(currentPoint.addressof());
        ThrowIfFailed(hr);

        wil::com_ptr<IPointerDevice> pointerDevice;
        hr = currentPoint->get_PointerDevice(pointerDevice.addressof());
        ThrowIfFailed(hr);

        PointerDeviceType devType;
        hr = pointerDevice->get_PointerDeviceType(&devType);
        ThrowIfFailed(hr);

        if (devType == PointerDeviceType::PointerDeviceType_Mouse)
        {
            wil::com_ptr<IPointerPointProperties> props;
            hr = currentPoint->get_Properties(props.addressof());
            ThrowIfFailed(hr);

            int32_t value;
            hr = props->get_MouseWheelDelta(&value);
            ThrowIfFailed(hr);

            HANDLE evt = s_mouse->m_scrollwheelevent.get();
            if (WaitForSingleObjectEx(evt, 0, FALSE) == WAIT_OBJECT_0)
            {
                s_mouse->mState.scrollWheelValue = 0;
                ResetEvent(evt);
            }

            s_mouse->mState.scrollWheelValue += value;

            if (s_mouse->mMode == MODE_ABSOLUTE)
            {
                Point pos;
                hr = currentPoint->get_Position(&pos);
                ThrowIfFailed(hr);

                float dpi = s_mouse->mDPI;

                s_mouse->mState.x = static_cast<int32_t>(pos.X * dpi / 96.f + 0.5f);
                s_mouse->mState.y = static_cast<int32_t>(pos.Y * dpi / 96.f + 0.5f);
            }
        }

        return S_OK;
    }

    static HRESULT MouseMovedEvent(IInspectable *, ABI::Windows::Devices::Input::IMouseEventArgs* args)
    {
        using namespace ABI::Windows::Devices::Input;

        if (!s_mouse)
            return S_OK;

        if (s_mouse->mMode == MODE_RELATIVE)
        {
            MouseDelta delta;
            HRESULT hr = args->get_MouseDelta(&delta);
            ThrowIfFailed(hr);

            s_mouse->mState.x = delta.X;
            s_mouse->mState.y = delta.Y;

            ResetEvent(s_mouse->m_relativereadevent.get());
        }

        return S_OK;
    }
};


Mouse::Impl* Mouse::Impl::s_mouse = nullptr;


void Mouse::SetWindow(ABI::Windows::UI::Core::ICoreWindow* window)
{
    pimpl->SetWindow(window);
}


void Mouse::SetDpi(float dpi)
{
    auto pimpl = Impl::s_mouse;

    if (!pimpl)
        return;

    pimpl->mDPI = dpi;
}

#endif

#pragma warning( disable : 4355 )

// Public constructor.
Mouse::Mouse() noexcept(false)
    : pimpl(std::make_unique<Impl>(this))
{
}


// Move constructor.
Mouse::Mouse(Mouse&& moveFrom) noexcept
    : pimpl(std::move(moveFrom.pimpl))
{
    pimpl->mOwner = this;
}


// Move assignment.
Mouse& Mouse::operator= (Mouse&& moveFrom) noexcept
{
    pimpl = std::move(moveFrom.pimpl);
    pimpl->mOwner = this;
    return *this;
}


// Public destructor.
Mouse::~Mouse()
{
}


Mouse::State Mouse::GetState() const
{
    State state;
    pimpl->GetState(state);
    return state;
}


void Mouse::ResetScrollWheelValue() noexcept
{
    pimpl->ResetScrollWheelValue();
}


void Mouse::SetMode(Mode mode)
{
    pimpl->SetMode(mode);
}


bool Mouse::IsConnected() const
{
    return pimpl->IsConnected();
}

bool Mouse::IsVisible() const
{
    return pimpl->IsVisible();
}

void Mouse::SetVisible(bool visible)
{
    pimpl->SetVisible(visible);
}

Mouse& Mouse::Get()
{
    if (!Impl::s_mouse || !Impl::s_mouse->mOwner)
        throw std::exception("Mouse is a singleton");

    return *Impl::s_mouse->mOwner;
}



//======================================================================================
// ButtonStateTracker
//======================================================================================

#define UPDATE_BUTTON_STATE(field) field = static_cast<ButtonState>( ( !!state.field ) | ( ( !!state.field ^ !!lastState.field ) << 1 ) );

void Mouse::ButtonStateTracker::Update(const Mouse::State& state) noexcept
{
    UPDATE_BUTTON_STATE(leftButton)

    assert((!state.leftButton && !lastState.leftButton) == (leftButton == UP));
    assert((state.leftButton && lastState.leftButton) == (leftButton == HELD));
    assert((!state.leftButton && lastState.leftButton) == (leftButton == RELEASED));
    assert((state.leftButton && !lastState.leftButton) == (leftButton == PRESSED));

    UPDATE_BUTTON_STATE(middleButton)
    UPDATE_BUTTON_STATE(rightButton)
    UPDATE_BUTTON_STATE(xButton1)
    UPDATE_BUTTON_STATE(xButton2)

    lastState = state;
}

#undef UPDATE_BUTTON_STATE


void Mouse::ButtonStateTracker::reset() noexcept
{
    memset(this, 0, sizeof(ButtonStateTracker));
}
