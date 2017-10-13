/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 windowsdebugging.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

#include <gameos.hpp>
#include <windows.hpp>

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
MECH_IMPEXP PCSTR __stdcall GetWindowsMessage(
	uint32_t uMsg, WPARAM wParam, LPARAM lParam);

// global implemented functions not listed in headers

// local functions
static PCSTR __stdcall GetWM_WINDOWPOSCHANGEFlags(PWINDOWPOS pwinpos);
static PCSTR __stdcall GetWM_SIZEFlags(WPARAM wParam);
static PCSTR __stdcall GetWM_SYSCOMMANDFlags(WPARAM wParam);

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

static PCSTR __stdcall GetWM_WINDOWPOSCHANGEFlags(PWINDOWPOS pwinpos)
{
	/*static*/ char szBuffer[0x80];
	szBuffer[0] = 0;
	if (!(pwinpos->flags & SWP_NOMOVE))
		sprintf_s(szBuffer, _countof(szBuffer), "Top Left = %d,%d ", pwinpos->x,
			pwinpos->y);
	if (!(pwinpos->flags & SWP_NOSIZE))
		sprintf_s(szBuffer, _countof(szBuffer), "Client Size = %d,%d ",
			pwinpos->cx, pwinpos->cy);
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
	ATL_SUPPRESS_WARNING(
		4172)		 // returning address of local variable or temporary
	return szBuffer; //-V558
}

static PCSTR __stdcall GetWM_SIZEFlags(WPARAM wParam)
{
	PCSTR pszMessage;
	switch (wParam)
	{
	case SIZE_MAXHIDE:
		pszMessage = "MAXHIDE";
		break;
	case SIZE_MAXIMIZED:
		pszMessage = "MAXIMIZED";
		break;
	case SIZE_MAXSHOW:
		pszMessage = "MAXSHOW";
		break;
	case SIZE_MINIMIZED:
		pszMessage = "MINIMIZED";
		break;
	case SIZE_RESTORED:
		pszMessage = "RESTORED";
		break;
	default:
		pszMessage = "Unknown";
		break;
	}
	return pszMessage;
}

static PCSTR __stdcall GetWM_SYSCOMMANDFlags(WPARAM wParam)
{
	/*static*/ char szBuffer[30];
	PCSTR pszMessage;
	switch (wParam)
	{
	case SC_CLOSE:
		pszMessage = "CLOSE";
		break;
	case SC_HSCROLL:
		pszMessage = "HSCROLL";
		break;
	case SC_MAXIMIZE:
		pszMessage = "MAXIMIZE";
		break;
	case SC_MINIMIZE:
		pszMessage = "MINIMIZE";
		break;
	case SC_MOUSEMENU:
		pszMessage = "MOUSEMENU";
		break;
	case SC_MOVE:
		pszMessage = "MOVE";
		break;
	case SC_NEXTWINDOW:
		pszMessage = "NEXTWINDOW";
		break;
	case SC_PREVWINDOW:
		pszMessage = "PREVWINDOW";
		break;
	case SC_SIZE:
		pszMessage = "SIZE";
		break;
	case SC_VSCROLL:
		pszMessage = "VSCROLL";
		break;
	case SC_KEYMENU:
		pszMessage = "KEYMENU";
		break;
	case SC_CONTEXTHELP:
		pszMessage = "CONTEXTHELP";
		break;
	case SC_DEFAULT:
		pszMessage = "DEFAULT";
		break;
	case SC_HOTKEY:
		pszMessage = "HOTKEY";
		break;
	case SC_MONITORPOWER:
		pszMessage = "MONITORPOWER";
		break;
	case SC_RESTORE:
		pszMessage = "RESTORE";
		break;
	case SC_SCREENSAVE:
		pszMessage = "SCREENSAVE";
		break;
	case SC_TASKLIST:
		pszMessage = "TASKLIST";
		break;
	default:
		sprintf_s(szBuffer, _countof(szBuffer), "Unknown 0x%x", wParam);
		pszMessage = szBuffer;
		break;
	}
	return pszMessage;
}

//----- (00000AB0) --------------------------------------------------------
MECH_IMPEXP PCSTR __stdcall GetWindowsMessage(
	uint32_t uMsg, WPARAM wParam, LPARAM lParam)
{
	/*static*/ char szBuffer[0x100];
	PCSTR pszMessage = nullptr;
	szBuffer[0]		 = 0;
	if (uMsg == WM_NCHITTEST || uMsg == WM_SETCURSOR || uMsg == WM_MOUSEMOVE)
		return nullptr;
	switch (uMsg)
	{
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
		pszMessage = nullptr;
		break;
	case WM_KEYFIRST:
		sprintf_s(szBuffer, _countof(szBuffer),
			"WM_KEYDOWN w=0x%4.4x l=0x%4.4x", wParam, lParam);
		pszMessage = szBuffer;
		break;
	case WM_CANCELMODE:
		pszMessage = "WM_CANCELMODE";
		break;
	case WM_PAINT:
		pszMessage = "WM_PAINT";
		break;
	case WM_ENABLE:
		pszMessage = "WM_ENABLE";
		break;
	case WM_ERASEBKGND:
		pszMessage = "WM_ERASEBKGND";
		break;
	case WM_SIZE:
		sprintf_s(szBuffer, _countof(szBuffer),
			"WM_SIZE %ux%u %s", // "WM_SIZE %dx%d %s",
			LOWORD(lParam), HIWORD(lParam), GetWM_SIZEFlags(wParam));
		pszMessage = szBuffer;
		break;
	case WM_DISPLAYCHANGE:
		sprintf_s(szBuffer, _countof(szBuffer),
			"WM_DISPLAYCHANGE %ux%u %ubpp", //"WM_DISPLAYCHANGE %dx%d %dbpp",
			LOWORD(lParam), HIWORD(lParam), wParam);
		pszMessage = szBuffer;
		break;
	case WM_SYSCOLORCHANGE:
		pszMessage = "WM_SYSCOLORCHANGE";
		break;
	case WM_KILLFOCUS:
		pszMessage = "WM_KILLFOCUS";
		break;
	case WM_DESTROY:
		pszMessage = "WM_DESTROY";
		break;
	case WM_CLOSE:
		pszMessage = "WM_CLOSE";
		break;
	case WM_CREATE:
		pszMessage = "WM_CREATE";
		break;
	case WM_WINDOWPOSCHANGING:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_WINDOWPOSCHANGING %s",
			GetWM_WINDOWPOSCHANGEFlags(reinterpret_cast<PWINDOWPOS>(lParam)));
		pszMessage = szBuffer;
		break;
	case WM_WINDOWPOSCHANGED:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_WINDOWPOSCHANGED %s",
			GetWM_WINDOWPOSCHANGEFlags(reinterpret_cast<PWINDOWPOS>(lParam)));
		pszMessage = szBuffer;
		break;
	case WM_QUIT:
		pszMessage = "WM_QUIT";
		break;
	case WM_MOVE:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_MOVE %ux%u", LOWORD(lParam),
			HIWORD(lParam)); // "WM_MOVE %dx%d",
		pszMessage = szBuffer;
		break;
	case WM_ACTIVATEAPP:
		if (LOWORD(wParam))
			pszMessage = "Activated";
		else
			pszMessage = "Deactivated";
		sprintf_s(
			szBuffer, _countof(szBuffer), "WM_ACTIVATEAPP %s", pszMessage);
		pszMessage = szBuffer;
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam))
			pszMessage = "Activated";
		else
			pszMessage = "Deactivated";
		sprintf_s(szBuffer, _countof(szBuffer), "WM_ACTIVATE %s", pszMessage);
		pszMessage = szBuffer;
		break;
	case WM_SETFOCUS:
		pszMessage = "WM_SETFOCUS";
		break;
	case WM_NCCREATE:
		pszMessage = "WM_NCCREATE";
		break;
	case WM_NCCALCSIZE:
		pszMessage = "WM_NCCALCSIZE";
		break;
	case WM_SHOWWINDOW:
		if (wParam)
			pszMessage = "Show";
		else
			pszMessage = "Hide";
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SHOWWINDOW %s", pszMessage);
		pszMessage = szBuffer;
		break;
	case WM_NCACTIVATE:
		pszMessage = "WM_NCACTIVATE";
		break;
	case WM_NCPAINT:
		pszMessage = "WM_NCPAINT";
		break;
	case WM_NCDESTROY:
		pszMessage = "WM_NCDESTROY";
		break;
	case WM_STYLECHANGING:
		pszMessage = "WM_STYLECHANGING";
		break;
	case WM_STYLECHANGED:
		pszMessage = "WM_STYLECHANGED";
		break;
	case WM_GETTEXT:
		pszMessage = "WM_GETTEXT";
		break;
	case WM_GETMINMAXINFO:
		pszMessage = "WM_GETMINMAXINFO";
		break;
	case WM_CONTEXTMENU:
		pszMessage = "WM_CONTEXTMENU";
		break;
	case WM_POWER:
		pszMessage = "WM_POWER";
		break;
	case WM_SYNCPAINT:
		pszMessage = "WM_SYNCPAINT";
		break;
	case WM_LBUTTONDOWN:
		pszMessage = "WM_LBUTTONDOWN";
		break;
	case WM_ENTERIDLE:
		pszMessage = 0;
		break;
	case WM_KEYUP:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_KEYUP 0x%4.4x", wParam);
		pszMessage = szBuffer;
		break;
	case WM_SYSCHAR:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSCHAR 0x%4.4x", wParam);
		pszMessage = szBuffer;
		break;
	case WM_SYSKEYDOWN:
		sprintf_s(
			szBuffer, _countof(szBuffer), "WM_SYSKEYDOWN 0x%4.4x", wParam);
		pszMessage = szBuffer;
		break;
	case WM_SYSKEYUP:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSKEYUP 0x%4.4x", wParam);
		pszMessage = szBuffer;
		break;
	case WM_SYSCOMMAND:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_SYSCOMMAND %s",
			GetWM_SYSCOMMANDFlags(wParam));
		pszMessage = szBuffer;
		break;
	case WM_COMMAND:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_COMMAND 0x%4.4x,0x%x",
			wParam, lParam);
		pszMessage = szBuffer;
		break;
	case WM_CHAR:
		// wParam is character code of the key
		if (wParam >= 32 && wParam <= 127)
			sprintf_s(szBuffer, _countof(szBuffer), "WM_CHAR \"%c\"", wParam);
		else
			sprintf_s(szBuffer, _countof(szBuffer), "WM_CHAR code=%u",
				wParam); // "WM_CHAR code=%d"
		pszMessage = szBuffer;
		break;
	case WM_TIMER:
		pszMessage = "WM_TIMER";
		break;
	case WM_MENUSELECT:
		pszMessage = "WM_MENUSELECT";
		break;
	case WM_INITMENU:
		pszMessage = "WM_INITMENU";
		break;
	case WM_INITMENUPOPUP:
		pszMessage = "WM_INITMENUPOPUP";
		break;
	case WM_PALETTECHANGED:
		pszMessage = "WM_PALETTECHANGED";
		break;
	case WM_QUERYNEWPALETTE:
		pszMessage = "WM_QUERYNEWPALETTE";
		break;
	case WM_MOUSEWHEEL:
		pszMessage = "WM_MOUSEWHEEL";
		break;
	case WM_SIZING:
		pszMessage = "WM_SIZING";
		break;
	case WM_MOVING:
		pszMessage = "WM_MOVING";
		break;
	case WM_CAPTURECHANGED:
		pszMessage = "WM_CAPTURECHANGED";
		break;
	case WM_ENTERMENULOOP:
		pszMessage = "WM_ENTERMENULOOP";
		break;
	case WM_EXITMENULOOP:
		pszMessage = "WM_EXITMENULOOP";
		break;
	case WM_LBUTTONUP:
		pszMessage = "WM_LBUTTONUP";
		break;
	case WM_RBUTTONDOWN:
		pszMessage = "WM_RBUTTONDOWN";
		break;
	case WM_RBUTTONUP:
		pszMessage = "WM_RBUTTONUP";
		break;
	case WM_ENTERSIZEMOVE:
		pszMessage = "WM_ENTERSIZEMOVE";
		break;
	case WM_EXITSIZEMOVE:
		pszMessage = "WM_EXITSIZEMOVE";
		break;
	case WM_POWERBROADCAST:
		pszMessage = "WM_POWERBROADCAST";
		break;
	case WM_DEVICECHANGE:
		pszMessage = "WM_DEVICECHANGE";
		break;
	default:
		sprintf_s(szBuffer, _countof(szBuffer), "WM_%x", uMsg);
		pszMessage = szBuffer;
		break;
	}
	return (pszMessage);
}
