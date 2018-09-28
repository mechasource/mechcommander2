/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
*******************************************************************************/
/*******************************************************************************
 winproc.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include "stdinc.h"

#include <gameos.hpp>
#include <controlmanager.hpp>
#include <directx.hpp>
#include <dxrasterizer.hpp>
#include <debugger.hpp>
#include <windows.hpp>
#include <winproc.hpp>

// -----------------------------------------------------------------------------
// Global data exported from this module

MECH_IMPEXP HICON ArrowCursor;
MECH_IMPEXP uint32_t HitScrollLock;

// global referenced data not listed in headers
MECH_IMPEXP bool WindowsPause;

// local data
bool ControlDown;
bool volatile mc2HasLostFocus;
LPARAM LastlParam;

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
MECH_IMPEXP LRESULT __stdcall GameOSWinProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam);

// global implemented functions not listed in headers
MECH_IMPEXP void __stdcall UpdateCursor(void);

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
extern bool ChangingModes;
extern uint32_t DebuggerMouse;
extern uint32_t GlobalGotoFullScreen;
extern uint32_t GlobalGotoWindowMode;
extern uint32_t gCaptureScreen;
extern uint32_t gPrintScreen;
extern bool g_DDperformFlip;
extern uint32_t g_DDstate;

LRESULT __stdcall ProcessIMEMessages(
	HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam, bool* pbHandled);

#pragma region UpdateCursor
/*
 */
#pragma endregion UpdateCursor
/// <summary>
/// <c>UpdateCursor</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
void __stdcall UpdateCursor(void)
{
	if (gActive && gGotFocus && !ProcessingError &&
		((unsigned __int16)LastlParam == 1 || Environment.fullScreen))
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

#pragma region GameOSWinProc
/*
 */
#pragma endregion GameOSWinProc
/// <summary>
/// <c>GameOSWinProc</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="hWnd"></param>
/// <param name="uMsg"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns>LRESULT __stdcall</returns>
MECH_IMPEXP LRESULT __stdcall GameOSWinProc(HWND hWnd, uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bActive;
	bool bHandled;
	LRESULT lResult;
	WINDOWPLACEMENT wndpl;
	PWINDOWPOS winpos;
	HDC hdcSrc;
	HDC hdc;
	PAINTSTRUCT paint;
	RECT rect;
	PSTR pszMessage;
	if ((uMsg == WM_NCHITTEST) || (uMsg == WM_MOUSEMOVE))
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	if (uMsg == WM_SETCURSOR)
	{
		if (Platform != Platform_Game)
		{
			lResult = DefWindowProcA(hWnd, WM_SETCURSOR, wParam, lParam);
		}
		else
		{
			LastlParam = lParam;
			UpdateCursor();
			lResult = 1;
		}
		return lResult;
	}
	pszMessage = GetWindowsMessage(uMsg, wParam, lParam);
	if (pszMessage)
		InternalFunctionSpew("Windows_Messages", pszMessage);
	if (uMsg <= WM_KEYFIRST) // uMsg <= 0x100
	{
		if (uMsg != WM_KEYFIRST)
		{
			return lResult;
		}
		switch (wParam)
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
		if (wParam != VK_CANCEL)
			DealWithKey(wParam, lParam);
		if (!Environment.Key_FullScreen || ProcessingError || wParam != Environment.Key_FullScreen)
		{
			if (!Environment.Key_SwitchMonitors || ProcessingError ||
				wParam != Environment.Key_SwitchMonitors)
			{
				if (Environment.Key_Exit && !ProcessingError && wParam == Environment.Key_Exit)
				{
					InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - Esc");
					if (Environment.fullScreen)
					{
						DoUpdateWindow		 = 1;
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
					if ((unsigned int)Environment.FullScreenDevice >= NumDevices)
						Environment.FullScreenDevice = 0;
					DoUpdateWindow		 = 1;
					GlobalGotoFullScreen = 1;
				}
				else
				{
					DoUpdateWindow		 = 1;
					GlobalGotoFullScreen = 1;
				}
			}
		}
		else
		{
			InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - F4");
			if (Environment.fullScreen)
			{
				DoUpdateWindow		 = 1;
				GlobalGotoWindowMode = 1;
			}
			else
			{
				DoUpdateWindow		 = 1;
				GlobalGotoFullScreen = 1;
			}
		}
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}
	if (uMsg > WM_SYSCOMMAND)
	{
		switch (uMsg)
		{
		case WM_ENTERMENULOOP:
			WindowsPause = 1;
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_EXITMENULOOP:
			WindowsPause = 0;
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_LBUTTONDOWN:
			DealWithKey(1u, 0);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_LBUTTONUP:
			DoKeyReleased(1u);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_RBUTTONDOWN:
			DealWithKey(2u, 0);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_RBUTTONUP:
			DoKeyReleased(2u);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_MBUTTONDOWN:
			DealWithKey(3u, 0);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_MBUTTONUP:
			DoKeyReleased(3u);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_XBUTTONDOWN:
			DealWithKey(HIWORD(wParam) + 3, 0);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		case WM_XBUTTONUP:
			DoKeyReleased(HIWORD(wParam) + 3);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		default:
		$LN2:
			bHandled = 0;
			lResult  = ProcessIMEMessages(hWnd, uMsg, wParam, lParam, &bHandled);
			if (!bHandled)
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			lResult = lResult;
			break;
		}
	}
	else
	{
		if (uMsg != WM_SYSCOMMAND)
		{
			switch (uMsg)
			{
			case WM_SYSKEYUP:
				if (wParam == VK_RETURN)
					return 0;
				DoKeyReleased(wParam);
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			case WM_SYSKEYDOWN:
				if (wParam == VK_RETURN && !ProcessingError)
				{
					InternalFunctionSpew("GameOS_DirectDraw", "Mode change requested - Alt Enter");
					if (Environment.fullScreen)
					{
						DoUpdateWindow		 = 1;
						GlobalGotoWindowMode = 1;
					}
					else
					{
						DoUpdateWindow		 = 1;
						GlobalGotoFullScreen = 1;
					}
					return 0;
				}
				if (wParam == VK_SCROLL)
				{
					if (Environment.fullScreen)
						EnterWindowMode();
					ENTER_DEBUGGER;
					JUMPOUT(loc_818);
				}
				DealWithKey(wParam, lParam);
				break;
			case WM_SYSCHAR:
				if (wParam != VK_F10)
					return DefWindowProcA(hWnd, uMsg, wParam, lParam);
				return 0;
			case WM_KEYUP:
				if (wParam != VK_CANCEL)
					DoKeyReleased(wParam);
				if (wParam == VK_CONTROL)
					ControlDown = 0;
				if (wParam == VK_SNAPSHOT && gPrintScreen)
					gCaptureScreen = 1;
				return DefWindowProcA(hWnd, uMsg, wParam, lParam);
			case WM_CHAR:
				if (wParam != VK_F16 && (wParam != VK_CANCEL || lParam != 0x1460001))
				{
					if (DebuggerActive && ((dbKeyPressed + 1) != dbKeyCurrent))
					{
						dbKeyBoardBuffer[dbKeyPressed] = wParam + LastWMDown;
						dbKeyPressed				   = (dbKeyPressed + 1);
					}
					if (!DebuggerActive || DebuggerActive && gControlsActive && !gStopSystem)
						AddKeyEvent(wParam + LastWMDown);
					LastWMDown = 0;
					lResult	= 0;
				}
				else
				{
					lResult = 0;
				}
				return lResult;
			default:
				goto $LN2;
			}
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		}
		if ((wParam & 0xFFF0) == SC_KEYMENU)
			return 0;
		if ((wParam & 0xFFF0) == SC_SCREENSAVE || (wParam & 0xFFF0) == SC_MONITORPOWER)
			return 0;
		if ((wParam & 0xFFF0) == SC_RESTORE)
		{
			InternalFunctionSpew(
				"GameOS_DirectDraw", "Mode change requested - WM_SYSCOMMAND - SC_RESTORE");
			if (Environment.fullScreen)
			{
				DoUpdateWindow		 = 1;
				GlobalGotoFullScreen = 1;
			}
			else
			{
				DoUpdateWindow		 = 1;
				GlobalGotoWindowMode = 1;
			}
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		}
		if ((wParam & 0xFFF0) != SC_MAXIMIZE)
		{
			if ((wParam & 0xFFF0) == SC_CLOSE)
			{
				WindowClosed = true;
				return 0;
			}
			if (Environment.fullScreen)
			{
				InternalFunctionSpew(
					"GameOS_DirectDraw", "Mode change requested - WM_SYSCOMMAND - other");
				DoUpdateWindow		 = 1;
				GlobalGotoWindowMode = 1;
			}
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
		}
		if (!Environment.fullScreen && !ProcessingError)
		{
			InternalFunctionSpew(
				"GameOS_DirectDraw", "Mode change requested - WM_SYSCOMMAND - SC_MAXIMIZE");
			DoUpdateWindow		 = 1;
			GlobalGotoFullScreen = 1;
		}
		lResult = 0;
	}
	return lResult;
	switch (uMsg)
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
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_PAINT:
		if (InUpdateRenderers || g_DDstate != 1 || !g_DDperformFlip || Environment.fullScreen ||
			status != Running && status != GameInit)
			goto LABEL_35;
		if (FrontBufferSurface)
		{
			rect.left   = clientToScreen.x;
			rect.right  = Environment.screenWidth + clientToScreen.x;
			rect.top	= clientToScreen.y;
			rect.bottom = Environment.screenHeight + clientToScreen.y;
			if (Environment.fullScreen)
			{
				rect.left   = 0;
				rect.right  = Environment.screenWidth;
				rect.top	= 0;
				rect.bottom = Environment.screenHeight;
			}
			AllowFail = true;
			wBlt(FrontBufferSurface, &rect, BackBufferSurface, 0, DDBLT_WAIT, 0);
			__asm { fninit} // GameOSFPU(void);
			AllowFail = false;
		LABEL_35:
			ValidateRect(hWnd, 0);
			lResult = 0;
		}
		else
		{
			hdc = BeginPaint(hWnd, &paint);
			wGetDC(BackBufferSurface, &hdcSrc);
			if (DesktopBpp == 8)
			{
				SetStretchBltMode(hdc, HALFTONE);
				StretchBlt(hdc, 0, 0, Environment.screenWidth, Environment.screenHeight, hdcSrc, 0,
					0, Environment.screenWidth, Environment.screenHeight, SRCCOPY);
			}
			else
			{
				BitBlt(hdc, 0, 0, Environment.screenWidth, Environment.screenHeight, hdcSrc, 0, 0,
					SRCCOPY);
			}
			wReleaseDC(BackBufferSurface, hdcSrc);
			EndPaint(hWnd, &paint);
			lResult = 0;
		}
		return lResult;
	case WM_ACTIVATE:
		bActive = (unsigned __int16)wParam == WA_ACTIVE ||
			(unsigned __int16)wParam == WA_CLICKACTIVE; // low order word
		gActive = bActive;
		if (gActive)
			mc2HasLostFocus = false;
		else
			mc2HasLostFocus = true;
		if (!(wParam & 0xFFFF0000) && gActive) // high order word. A nonzero
											   // value indicates the window is
											   // minimized.
			CMAcquireControls();
		if (Environment.fullScreen && !(_WORD)wParam)
			ClipCursor(0);
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_WINDOWPOSCHANGING:
		if (!ChangingModes)
		{
			if (!Environment.fullScreen)
			{
				winpos = (WINDOWPOS*)lParam;
				if (!(*(_DWORD*)(lParam + 24) & SWP_NOMOVE)) // WINDOWPOS.flags
				{
					wndpl.length = 0x2Cu;
					GetWindowPlacement(hWindow, &wndpl);
					if (wndpl.showCmd == 1)
					{
						WindowStartX = winpos->x;
						WindowStartY = winpos->y;
					}
				}
			}
		}
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_ACTIVATEAPP:
		if (!IsIconic(hWindow) && wParam)
			CMAcquireControls();
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_KILLFOCUS:
		gGotFocus = false;
		CMUnacquireControls();
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_SETFOCUS:
		gGotFocus = true;
		CMAcquireControls();
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_MOVE:
		clientToScreen.x = 0;
		clientToScreen.y = 0;
		ClientToScreen(hWnd, &clientToScreen);
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	case WM_ERASEBKGND:
		if (Platform || status == Uninitialized)
		{
			ValidateRect(hWnd, 0);
			return DefWindowProcA(hWnd, uMsg, wParam, lParam);
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
