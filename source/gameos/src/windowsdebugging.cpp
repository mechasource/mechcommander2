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
 windowsdebugging.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "windows.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
MECH_IMPEXP PSTR __stdcall GetWindowsMessage(uint32_t message, WPARAM wparam, LPARAM lparam);

// global implemented functions not listed in headers

// local functions
static PSTR __stdcall GetWM_WINDOWPOSCHANGEFlags(PWINDOWPOS pwinpos);
static PSTR __stdcall GetWM_SIZEFlags(WPARAM wparam);
static PSTR __stdcall GetWM_SYSCOMMANDFlags(WPARAM wparam);

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

static PSTR __stdcall GetWM_WINDOWPOSCHANGEFlags(PWINDOWPOS pwinpos)
{
	/*static*/ char szBuffer[0x80];
	szBuffer[0] = 0;
	if (!(pwinpos->flags & SWP_NOMOVE))
		sprintf_s(szBuffer, _countof(szBuffer), "Top Left = %d,%d ", pwinpos->x, pwinpos->y);
	if (!(pwinpos->flags & SWP_NOSIZE))
		sprintf_s(szBuffer, _countof(szBuffer), "Client Size = %d,%d ", pwinpos->cx, pwinpos->cy);
	// if (pwinpos->flags & SWP_FRAMECHANGED)
	//	strcat_s(szBuffer, _countof(szBuffer), "DRAWFRAME ");
	if (pwinpos->flags & SWP_FRAMECHANGED)
		strcat_s(szBuffer, _countof(szBuffer), "FRAMECHANGED ");
	if (pwinpos->flags & SWP_HIDEWINDOW)
		strcat_s(szBuffer, _countof(szBuffer), "HIDEWINDOW ");
	if (pwinpos->flags & SWP_NOACTIVATE)
		strcat_s(szBuffer, _countof(szBuffer), "NOACTIVATE ");
	if (pwinpos->flags & SWP_NOCOPYBITS)
		strcat_s(szBuffer, _countof(szBuffer), "NOCOPYBITS ");
	if (pwinpos->flags & SWP_NOMOVE)
		strcat_s(szBuffer, _countof(szBuffer), "NOMOVE ");
	if (pwinpos->flags & SWP_NOOWNERZORDER)
		strcat_s(szBuffer, _countof(szBuffer), "NOOWNERZORDER ");
	if (pwinpos->flags & SWP_NOREDRAW)
		strcat_s(szBuffer, _countof(szBuffer), "NOREDRAW ");
	if (pwinpos->flags & SWP_NOSENDCHANGING)
		strcat_s(szBuffer, _countof(szBuffer), "NOSENDCHANGING ");
	if (pwinpos->flags & SWP_NOSIZE)
		strcat_s(szBuffer, _countof(szBuffer), "NOSIZE ");
	if (pwinpos->flags & SWP_NOZORDER)
		strcat_s(szBuffer, _countof(szBuffer), "NOZORDER ");
	if (pwinpos->flags & SWP_SHOWWINDOW)
		strcat_s(szBuffer, _countof(szBuffer), "SHOWWINDOW ");
	MSSUPPRESS_WARNING(4172) // returning address of local variable or temporary
	return szBuffer; //-V558
}

static PSTR __stdcall GetWM_SIZEFlags(WPARAM wparam)
{
	PSTR message;
	switch (wparam)
	{
	case SIZE_MAXHIDE:
		message = "MAXHIDE";
		break;
	case SIZE_MAXIMIZED:
		message = "MAXIMIZED";
		break;
	case SIZE_MAXSHOW:
		message = "MAXSHOW";
		break;
	case SIZE_MINIMIZED:
		message = "MINIMIZED";
		break;
	case SIZE_RESTORED:
		message = "RESTORED";
		break;
	default:
		message = "Unknown";
		break;
	}
	return message;
}

static PSTR __stdcall GetWM_SYSCOMMANDFlags(WPARAM wparam)
{
	/*static*/ char szBuffer[30];
	PSTR message;
	switch (wparam)
	{
	case SC_CLOSE:
		message = "CLOSE";
		break;
	case SC_HSCROLL:
		message = "HSCROLL";
		break;
	case SC_MAXIMIZE:
		message = "MAXIMIZE";
		break;
	case SC_MINIMIZE:
		message = "MINIMIZE";
		break;
	case SC_MOUSEMENU:
		message = "MOUSEMENU";
		break;
	case SC_MOVE:
		message = "MOVE";
		break;
	case SC_NEXTWINDOW:
		message = "NEXTWINDOW";
		break;
	case SC_PREVWINDOW:
		message = "PREVWINDOW";
		break;
	case SC_SIZE:
		message = "SIZE";
		break;
	case SC_VSCROLL:
		message = "VSCROLL";
		break;
	case SC_KEYMENU:
		message = "KEYMENU";
		break;
	case SC_CONTEXTHELP:
		message = "CONTEXTHELP";
		break;
	case SC_DEFAULT:
		message = "DEFAULT";
		break;
	case SC_HOTKEY:
		message = "HOTKEY";
		break;
	case SC_MONITORPOWER:
		message = "MONITORPOWER";
		break;
	case SC_RESTORE:
		message = "RESTORE";
		break;
	case SC_SCREENSAVE:
		message = "SCREENSAVE";
		break;
	case SC_TASKLIST:
		message = "TASKLIST";
		break;
	default:
		sprintf_s(szBuffer, _countof(szBuffer), "Unknown 0x%x", wparam);
		message = szBuffer;
		break;
	}
	return message;
}

//----- (00000AB0) --------------------------------------------------------
MECH_IMPEXP PSTR __stdcall GetWindowsMessage(uint32_t message, WPARAM wparam, LPARAM lparam)
{
	/*static*/ char szBuffer[0x100];
	PSTR message = nullptr;
	szBuffer[0] = 0;
	if (message == WM_NCHITTEST || message == WM_SETCURSOR || message == WM_MOUSEMOVE)
		return nullptr;
	switch (message)
	{
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
		message = nullptr;
		break;
	case WM_KEYFIRST:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_KEYDOWN w=0x%4.4x l=0x%4.4x", wparam, lparam);
		message = szBuffer;
		break;
	case WM_CANCELMODE:
		message = "WM_CANCELMODE";
		break;
	case WM_PAINT:
		message = "WM_PAINT";
		break;
	case WM_ENABLE:
		message = "WM_ENABLE";
		break;
	case WM_ERASEBKGND:
		message = "WM_ERASEBKGND";
		break;
	case WM_SIZE:
		sprintf_s(szBuffer, _countof(szBuffer),
			"WM_SIZE %ux%u %s", // "WM_SIZE %dx%d %s",
			LOWORD(lparam), HIWORD(lparam), GetWM_SIZEFlags(wparam));
		message = szBuffer;
		break;
	case WM_DISPLAYCHANGE:
		sprintf_s(szBuffer, _countof(szBuffer),
			"WM_DISPLAYCHANGE %ux%u %ubpp", //"WM_DISPLAYCHANGE %dx%d %dbpp",
			LOWORD(lparam), HIWORD(lparam), wparam);
		message = szBuffer;
		break;
	case WM_SYSCOLORCHANGE:
		message = "WM_SYSCOLORCHANGE";
		break;
	case WM_KILLFOCUS:
		message = "WM_KILLFOCUS";
		break;
	case WM_DESTROY:
		message = "WM_DESTROY";
		break;
	case WM_CLOSE:
		message = "WM_CLOSE";
		break;
	case WM_CREATE:
		message = "WM_CREATE";
		break;
	case WM_WINDOWPOSCHANGING:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_WINDOWPOSCHANGING %s",
			GetWM_WINDOWPOSCHANGEFlags(reinterpret_cast<PWINDOWPOS>(lparam)));
		message = szBuffer;
		break;
	case WM_WINDOWPOSCHANGED:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_WINDOWPOSCHANGED %s",
			GetWM_WINDOWPOSCHANGEFlags(reinterpret_cast<PWINDOWPOS>(lparam)));
		message = szBuffer;
		break;
	case WM_QUIT:
		message = "WM_QUIT";
		break;
	case WM_MOVE:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_MOVE %ux%u", LOWORD(lparam),
			HIWORD(lparam)); // "WM_MOVE %dx%d",
		message = szBuffer;
		break;
	case WM_ACTIVATEAPP:
		if (LOWORD(wparam))
			message = "Activated";
		else
			message = "Deactivated";
		sprintf_s(szBuffer, _countof(szBuffer), "WM_ACTIVATEAPP %s", message);
		message = szBuffer;
		break;
	case WM_ACTIVATE:
		if (LOWORD(wparam))
			message = "Activated";
		else
			message = "Deactivated";
		sprintf_s(szBuffer, _countof(szBuffer), "WM_ACTIVATE %s", message);
		message = szBuffer;
		break;
	case WM_SETFOCUS:
		message = "WM_SETFOCUS";
		break;
	case WM_NCCREATE:
		message = "WM_NCCREATE";
		break;
	case WM_NCCALCSIZE:
		message = "WM_NCCALCSIZE";
		break;
	case WM_SHOWWINDOW:
		if (wparam)
			message = "Show";
		else
			message = "Hide";
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SHOWWINDOW %s", message);
		message = szBuffer;
		break;
	case WM_NCACTIVATE:
		message = "WM_NCACTIVATE";
		break;
	case WM_NCPAINT:
		message = "WM_NCPAINT";
		break;
	case WM_NCDESTROY:
		message = "WM_NCDESTROY";
		break;
	case WM_STYLECHANGING:
		message = "WM_STYLECHANGING";
		break;
	case WM_STYLECHANGED:
		message = "WM_STYLECHANGED";
		break;
	case WM_GETTEXT:
		message = "WM_GETTEXT";
		break;
	case WM_GETMINMAXINFO:
		message = "WM_GETMINMAXINFO";
		break;
	case WM_CONTEXTMENU:
		message = "WM_CONTEXTMENU";
		break;
	case WM_POWER:
		message = "WM_POWER";
		break;
	case WM_SYNCPAINT:
		message = "WM_SYNCPAINT";
		break;
	case WM_LBUTTONDOWN:
		message = "WM_LBUTTONDOWN";
		break;
	case WM_ENTERIDLE:
		message = 0;
		break;
	case WM_KEYUP:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_KEYUP 0x%4.4x", wparam);
		message = szBuffer;
		break;
	case WM_SYSCHAR:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSCHAR 0x%4.4x", wparam);
		message = szBuffer;
		break;
	case WM_SYSKEYDOWN:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSKEYDOWN 0x%4.4x", wparam);
		message = szBuffer;
		break;
	case WM_SYSKEYUP:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSKEYUP 0x%4.4x", wparam);
		message = szBuffer;
		break;
	case WM_SYSCOMMAND:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSCOMMAND %s", GetWM_SYSCOMMANDFlags(wparam));
		message = szBuffer;
		break;
	case WM_COMMAND:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_COMMAND 0x%4.4x,0x%x", wparam, lparam);
		message = szBuffer;
		break;
	case WM_CHAR:
		// wparam is character code of the key
		if (wparam >= 32 && wparam <= 127)
			sprintf_s(szBuffer, _countof(szBuffer), "WM_CHAR \"%c\"", wparam);
		else
			sprintf_s(szBuffer, _countof(szBuffer), "WM_CHAR code=%u",
				wparam); // "WM_CHAR code=%d"
		message = szBuffer;
		break;
	case WM_TIMER:
		message = "WM_TIMER";
		break;
	case WM_MENUSELECT:
		message = "WM_MENUSELECT";
		break;
	case WM_INITMENU:
		message = "WM_INITMENU";
		break;
	case WM_INITMENUPOPUP:
		message = "WM_INITMENUPOPUP";
		break;
	case WM_PALETTECHANGED:
		message = "WM_PALETTECHANGED";
		break;
	case WM_QUERYNEWPALETTE:
		message = "WM_QUERYNEWPALETTE";
		break;
	case WM_MOUSEWHEEL:
		message = "WM_MOUSEWHEEL";
		break;
	case WM_SIZING:
		message = "WM_SIZING";
		break;
	case WM_MOVING:
		message = "WM_MOVING";
		break;
	case WM_CAPTURECHANGED:
		message = "WM_CAPTURECHANGED";
		break;
	case WM_ENTERMENULOOP:
		message = "WM_ENTERMENULOOP";
		break;
	case WM_EXITMENULOOP:
		message = "WM_EXITMENULOOP";
		break;
	case WM_LBUTTONUP:
		message = "WM_LBUTTONUP";
		break;
	case WM_RBUTTONDOWN:
		message = "WM_RBUTTONDOWN";
		break;
	case WM_RBUTTONUP:
		message = "WM_RBUTTONUP";
		break;
	case WM_ENTERSIZEMOVE:
		message = "WM_ENTERSIZEMOVE";
		break;
	case WM_EXITSIZEMOVE:
		message = "WM_EXITSIZEMOVE";
		break;
	case WM_POWERBROADCAST:
		message = "WM_POWERBROADCAST";
		break;
	case WM_DEVICECHANGE:
		message = "WM_DEVICECHANGE";
		break;
	default:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_%x", message);
		message = szBuffer;
		break;
	}
	return (message);
}
