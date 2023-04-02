/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted to Microsoft
*******************************************************************************/
/*******************************************************************************
 winproc.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "controlmanager.hpp"
#include "directx.hpp"
#include "dxrasterizer.hpp"
#include "debugger.hpp"
#include "windows.hpp"
#include "winproc.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module

MECH_IMPEXP HICON ArrowCursor;
MECH_IMPEXP uint32_t HitScrollLock;

// global referenced data not listed in headers
MECH_IMPEXP BOOLEAN WindowsPause;

// local data
BOOLEAN ControlDown;
BOOLEAN volatile mc2HasLostFocus;
LPARAM LastlParam;

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
MECH_IMPEXP LRESULT __stdcall GameOSWinProc(HWND hwnd, uint32_t message, WPARAM wparam, LPARAM lparam);

// global implemented functions not listed in headers
MECH_IMPEXP void __stdcall UpdateCursor(void);

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
extern BOOLEAN ChangingModes;
extern uint32_t DebuggerMouse;
extern uint32_t GlobalGotoFullScreen;
extern uint32_t GlobalGotoWindowMode;
extern uint32_t gCaptureScreen;
extern uint32_t gPrintScreen;
extern BOOLEAN g_DDperformFlip;
extern uint32_t g_DDstate;

LRESULT __stdcall ProcessIMEMessages(
	HWND hwnd, uint32_t message, WPARAM wparam, LPARAM lparam, BOOLEAN* pbHandled);

/******************************************************************************/
/// <summary>
/// <c>UpdateCursor</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
void __stdcall UpdateCursor(void)
{
	if (gActive && gGotFocus && (ProcessingError == 0) && (static_cast<uint16_t>(LastlParam) == 1 || (Environment.fullScreen == true)))
	{
		if (!gHardwareMouse && !DebuggerMouse || Environment.fullScreen || Compatibility3D & 0x400)
			SetCursor(0);
		else
			SetCursor(ArrowCursor);
	}
	else
	{
		SetCursor(ArrowCursor);
	}
}

/******************************************************************************/
/// <summary>
/// <c>GameOSWinProc</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="hwnd"></param>
/// <param name="message"></param>
/// <param name="wparam"></param>
/// <param name="lparam"></param>
/// <returns>LRESULT</returns>
MECH_IMPEXP LRESULT __stdcall GameOSWinProc(HWND hwnd, uint32_t message, WPARAM wparam, LPARAM lparam)
{
	BOOLEAN bActive;
	BOOLEAN bHandled;
	LRESULT lResult;
	WINDOWPLACEMENT wndpl;
	PWINDOWPOS winpos;
	HDC hdcsrc;
	HDC hdc;
	PAINTSTRUCT paint;
	RECT rect;
	PSTR message;
	if ((message == WM_NCHITTEST) || (message == WM_MOUSEMOVE))
		return DefWindowProcA(hwnd, message, wparam, lparam);
	if (message == WM_SETCURSOR)
	{
		if (Platform != Platform_Game)
		{
			lResult = DefWindowProcA(hwnd, WM_SETCURSOR, wparam, lparam);
		}
		else
		{
			LastlParam = lparam;
			UpdateCursor();
			lResult = 1;
		}
		return lResult;
	}
	message = GetWindowsMessage(message, wparam, lparam);
	if (message)
		InternalFunctionSpew("Windows_Messages", message);
	if (message <= WM_KEYFIRST) // message <= 0x100
	{
		if (message != WM_KEYFIRST)
		{
			return lResult;
		}
		switch (wparam)
		{
		case VK_SCROLL:
			HitScrollLock = 1;
			if (InternalFunctionPause("Scroll Lock pressed"))
				ENTER_DEBUGGER;
			HitScrollLock = 0;
			return 0;
		case VK_CONTROL:
			ControlDown = true;
			break;
		case VK_BACK:
			if (ControlDown == false)
				break;
			gFrameGraph ^= 1u;
			return 0;
		case VK_CANCEL:
		case VK_PAUSE:
			if (ControlDown)
			{
				DebuggerActive ^= true;
				if (DebuggerActive == false)
					EndRenderMode();
			}
			break;
		default:
			break;
		}
		if (wparam != VK_CANCEL)
			DealWithKey(wparam, lparam);
		if (!Environment.Key_FullScreen || ProcessingError || wparam != Environment.Key_FullScreen)
		{
			if (!Environment.Key_SwitchMonitors || ProcessingError || wparam != Environment.Key_SwitchMonitors)
			{
				if (Environment.Key_Exit && !ProcessingError && wparam == Environment.Key_Exit)
				{
					InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - Esc");
					if (Environment.fullScreen)
					{
						DoUpdateWindow = 1;
						GlobalGotoWindowMode = 1;
					}
					else
					{
						WindowClosed = 1;
					}
				}
			}
			else
			{
				InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - F5");
				if (Environment.fullScreen)
				{
					++Environment.FullScreenDevice;
					if ((uint32_t)Environment.FullScreenDevice >= NumDevices)
						Environment.FullScreenDevice = 0;
					DoUpdateWindow = 1;
					GlobalGotoFullScreen = 1;
				}
				else
				{
					DoUpdateWindow = 1;
					GlobalGotoFullScreen = 1;
				}
			}
		}
		else
		{
			InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - F4");
			if (Environment.fullScreen)
			{
				DoUpdateWindow = 1;
				GlobalGotoWindowMode = 1;
			}
			else
			{
				DoUpdateWindow = 1;
				GlobalGotoFullScreen = 1;
			}
		}
		return DefWindowProcA(hwnd, message, wparam, lparam);
	}
	if (message > WM_SYSCOMMAND)
	{
		switch (message)
		{
		case WM_ENTERMENULOOP:
			WindowsPause = 1;
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_EXITMENULOOP:
			WindowsPause = 0;
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_LBUTTONDOWN:
			DealWithKey(1u, 0);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_LBUTTONUP:
			DoKeyReleased(1u);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_RBUTTONDOWN:
			DealWithKey(2u, 0);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_RBUTTONUP:
			DoKeyReleased(2u);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_MBUTTONDOWN:
			DealWithKey(3u, 0);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_MBUTTONUP:
			DoKeyReleased(3u);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_XBUTTONDOWN:
			DealWithKey(HIWORD(wparam) + 3, 0);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_XBUTTONUP:
			DoKeyReleased(HIWORD(wparam) + 3);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		default:
		$LN2:
			bHandled = 0;
			lResult = ProcessIMEMessages(hwnd, message, wparam, lparam, &bHandled);
			if (!bHandled)
				return DefWindowProcA(hwnd, message, wparam, lparam);
			lResult = lResult;
			break;
		}
	}
	else
	{
		if (message != WM_SYSCOMMAND)
		{
			switch (message)
			{
			case WM_SYSKEYUP:
				if (wparam == VK_RETURN)
					return 0;
				DoKeyReleased(wparam);
				return DefWindowProcA(hwnd, message, wparam, lparam);
			case WM_SYSKEYDOWN:
				if (wparam == VK_RETURN && !ProcessingError)
				{
					InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - Alt Enter");
					if (Environment.fullScreen)
					{
						DoUpdateWindow = 1;
						GlobalGotoWindowMode = 1;
					}
					else
					{
						DoUpdateWindow = 1;
						GlobalGotoFullScreen = 1;
					}
					return 0;
				}
				if (wparam == VK_SCROLL)
				{
					if (Environment.fullScreen)
						EnterWindowMode();
					ENTER_DEBUGGER;
					JUMPOUT(loc_818);
				}
				DealWithKey(wparam, lparam);
				break;
			case WM_SYSCHAR:
				if (wparam != VK_F10)
					return DefWindowProcA(hwnd, message, wparam, lparam);
				return 0;
			case WM_KEYUP:
				if (wparam != VK_CANCEL)
					DoKeyReleased(wparam);
				if (wparam == VK_CONTROL)
					ControlDown = 0;
				if (wparam == VK_SNAPSHOT && gPrintScreen)
					gCaptureScreen = 1;
				return DefWindowProcA(hwnd, message, wparam, lparam);
			case WM_CHAR:
				if (wparam != VK_F16 && (wparam != VK_CANCEL || lparam != 0x1460001))
				{
					if (DebuggerActive && ((dbKeyPressed + 1) != dbKeyCurrent))
					{
						dbKeyBoardBuffer[dbKeyPressed] = wparam + LastWMDown;
						dbKeyPressed = (dbKeyPressed + 1);
					}
					if (!DebuggerActive || DebuggerActive && gControlsActive && !gStopSystem)
						AddKeyEvent(wparam + LastWMDown);
					LastWMDown = 0;
					lResult = 0;
				}
				else
				{
					lResult = 0;
				}
				return lResult;
			default:
				goto $LN2;
			}
			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		if ((wparam & 0xFFF0) == SC_KEYMENU)
			return 0;
		if ((wparam & 0xFFF0) == SC_SCREENSAVE || (wparam & 0xFFF0) == SC_MONITORPOWER)
			return 0;
		if ((wparam & 0xFFF0) == SC_RESTORE)
		{
			InternalFunctionSpew(
				"GameOS_DirectDraw", "Mode change requested - WM_SYSCOMMAND - SC_RESTORE");
			if (Environment.fullScreen)
			{
				DoUpdateWindow = 1;
				GlobalGotoFullScreen = 1;
			}
			else
			{
				DoUpdateWindow = 1;
				GlobalGotoWindowMode = 1;
			}
			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		if ((wparam & 0xFFF0) != SC_MAXIMIZE)
		{
			if ((wparam & 0xFFF0) == SC_CLOSE)
			{
				WindowClosed = true;
				return 0;
			}
			if (Environment.fullScreen)
			{
				InternalFunctionSpew(
					"GameOS_DirectDraw", "Mode change requested - WM_SYSCOMMAND - other");
				DoUpdateWindow = 1;
				GlobalGotoWindowMode = 1;
			}
			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		if (!Environment.fullScreen && !ProcessingError)
		{
			InternalFunctionSpew(
				"GameOS_DirectDraw", "Mode change requested - WM_SYSCOMMAND - SC_MAXIMIZE");
			DoUpdateWindow = 1;
			GlobalGotoFullScreen = 1;
		}
		lResult = 0;
	}
	return lResult;
	switch (message)
	{
	case WM_SYSCOLORCHANGE:
	case WM_DISPLAYCHANGE:
		Compatibility3D = Compatibility3D;
		if (!(Compatibility3D & 0x800000) && !ChangingModes)
		{
			DoUpdateWindow = 1;
			InternalFunctionSpew("GameOS_DirectDraw",
				"Mode change requested - "
				"WM_SYSCOLORCHANGE or "
				"WM_DISPLAYCHANGE");
		}
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_PAINT:
		if (InUpdateRenderers || g_DDstate != 1 || !g_DDperformFlip || Environment.fullScreen || status != Running && status != GameInit)
			goto LABEL_35;
		if (FrontBufferSurface)
		{
			rect.left = clientToScreen.x;
			rect.right = Environment.screenwidth + clientToScreen.x;
			rect.top = clientToScreen.y;
			rect.bottom = Environment.screenheight + clientToScreen.y;
			if (Environment.fullScreen)
			{
				rect.left = 0;
				rect.right = Environment.screenwidth;
				rect.top = 0;
				rect.bottom = Environment.screenheight;
			}
			AllowFail = true;
			wBlt(FrontBufferSurface, &rect, BackBufferSurface, 0, DDBLT_WAIT, 0);
			__asm { fninit} // GameOSFPU(void);
			AllowFail = false;
		LABEL_35:
			ValidateRect(hwnd, 0);
			lResult = 0;
		}
		else
		{
			hdc = BeginPaint(hwnd, &paint);
			wGetDC(BackBufferSurface, &hdcsrc);
			if (DesktopBpp == 8)
			{
				SetStretchBltMode(hdc, HALFTONE);
				StretchBlt(hdc, 0, 0, Environment.screenwidth, Environment.screenheight, hdcsrc, 0,
					0, Environment.screenwidth, Environment.screenheight, SRCCOPY);
			}
			else
			{
				BitBlt(hdc, 0, 0, Environment.screenwidth, Environment.screenheight, hdcsrc, 0, 0,
					SRCCOPY);
			}
			wReleaseDC(BackBufferSurface, hdcsrc);
			EndPaint(hwnd, &paint);
			lResult = 0;
		}
		return lResult;
	case WM_ACTIVATE:
		bActive = (unsigned __int16)wparam == WA_ACTIVE || (unsigned __int16)wparam == WA_CLICKACTIVE; // low order word
		gActive = bActive;
		if (gActive)
			mc2HasLostFocus = false;
		else
			mc2HasLostFocus = true;
		if (!(wparam & 0xFFFF0000) && gActive) // high order word. A nonzero
			// value indicates the window is
			// minimized.
			CMAcquireControls();
		if (Environment.fullScreen && !(_WORD)wparam)
			ClipCursor(0);
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_WINDOWPOSCHANGING:
		if (!ChangingModes)
		{
			if (!Environment.fullScreen)
			{
				winpos = (WINDOWPOS*)lparam;
				if (!(winpos->flags & SWP_NOMOVE)) // WINDOWPOS.flags
				{
					wndpl.length = sizeof(wndpl);
					GetWindowPlacement(hwnd, &wndpl);
					if (wndpl.showCmd == SW_SHOW 1)
					{
						WindowStartX = winpos->x;
						WindowStartY = winpos->y;
					}
				}
			}
		}
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_ACTIVATEAPP:
		if (!IsIconic(hwnd) && wparam)
			CMAcquireControls();
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_KILLFOCUS:
		gGotFocus = false;
		CMUnacquireControls();
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_SETFOCUS:
		gGotFocus = true;
		CMAcquireControls();
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_MOVE:
		clientToScreen.x = 0;
		clientToScreen.y = 0;
		ClientToScreen(hwnd, &clientToScreen);
		return DefWindowProcA(hwnd, message, wparam, lparam);
	case WM_ERASEBKGND:
		if (Platform || status == Uninitialized)
		{
			ValidateRect(hwnd, 0);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		lResult = 1;
		break;
	case WM_CLOSE:
		WindowClosed = true;
		return 0;
	default:
		goto $LN2;
	}
}
