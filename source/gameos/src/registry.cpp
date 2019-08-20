/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
*******************************************************************************/
/*******************************************************************************
 registry.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include "stdinc.h"

#include "registry.hpp"
#include "debugger.hpp"
#include "profiler.hpp"
#include "perf.h"

// -----------------------------------------------------------------------------
// Global data exported from this module
uint32_t UpdatedExe;
char RegistryKey[256];

// global referenced data not listed in headers
extern uint32_t gEnableParallel;

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
void __stdcall RegistryManagerInstall();
void __stdcall RegistryManagerUninstall();
void __stdcall gos_LoadDataFromRegistry(
	PSTR valuename, PBYTE pdata, puint32_t pcbdata, bool ishklm);
void __stdcall gos_SaveDataToRegistry(PSTR keyname, PBYTE pdata, uint32_t cbdata);
void __stdcall gos_SaveStringToRegistry(PSTR keyname, PBYTE pdata, uint32_t cbdata);
PSTR __stdcall ReadRegistry(PSTR keyname, PSTR valuename, bool ishklm);
PSTR __stdcall ReadRegistryHKCU(PSTR keyname, PSTR valuename, bool ishklm);

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------

typedef struct RegDebuggerOptions
{
	uint32_t GameLogicActive;
	uint32_t ControlsActive;
	uint32_t StopSystem;
	uint32_t FrameGraph;
	uint32_t EnableParallel;
	uint32_t NoDebuggerStats;
	uint32_t ShowGraphsAsTime;
	uint32_t ShowGraphBackground;
	uint32_t ShowLFControls;
	uint32_t NoGraph;
	uint32_t TextureOverrun;
	uint32_t ShowAverage;
} RegDebuggerOptions;

/******************************************************************************/
/// <summary>
/// <c>RegistryManagerInstall</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
void __stdcall RegistryManagerInstall(void)
{
	LSTATUS status;
	SYSTEMTIME SystemTime;
	FILETIME FileTime;
	FILETIME exedate;
	HKEY hkey;
	HANDLE hFile;
	uint32_t cbdata;
	uint32_t data;
	PBYTE pdata;

	// InternalFunctionSpew("GameOS_Core", "RegistryManagerInstall()");

	if ((Platform != Platform_DLL) && (Environment.allowMultipleApps == false))
	{
		uint32_t time;

		cbdata = 4;
		data = 0;

		GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime, &FileTime);
		*(uint64_t*)&FileTime -= 10000i64 * timeGetTime();
		FileTimeToSystemTime(&FileTime, &SystemTime);
		SystemTime.wSecond = 0;
		SystemTime.wMilliseconds = 0;
		SystemTimeToFileTime(&SystemTime, &FileTime);
		time = LOWORD(FileTime.dwLowDateTime);
		if (time == 0)
			++time;
		pdata = reinterpret_cast<PBYTE>(&data);
		gos_LoadDataFromRegistry("BadShutDown", pdata, &cbdata, false);
		if (time == data && WindowsNT == false)
		{
			// PSTR resourcestr = gos_GetResourceString(gLanguageDLL, 10050u);
			// if ( InternalFunctionPause(resourcestr) )
			//	__debugbreak();
		}
		data = time; // pdata = reinterpret_cast<PBYTE>(&time);
		gos_SaveDataToRegistry("BadShutDown", pdata, 4u);
	}

	cbdata = 4;
	data = 0;
	pdata = reinterpret_cast<PBYTE>(&data);

	if ((Environment.version != nullptr) && (*Environment.version) /* ! */ && (Environment.dontClearRegistry == false))
	{
		status = ::RegOpenKeyA(HKEY_LOCAL_MACHINE, RegistryKey, &hkey);
		if (status == ERROR_SUCCESS)
		{
			status =
				::RegQueryValueExA(hkey, Environment.version, nullptr, nullptr, pdata, &cbdata);
			::RegCloseKey(hkey);
			if ((status != ERROR_SUCCESS) || (data != 1)) // ?
			{
				::RegDeleteKeyA(HKEY_LOCAL_MACHINE, RegistryKey);
			}
		}
	}

	cbdata = 4;
	data = 0;
	pdata = reinterpret_cast<PBYTE>(&data);

	hFile =
		::CreateFileA(_pgmptr, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	::GetFileTime(hFile, nullptr, nullptr, &exedate);
	::CloseHandle(hFile);

	gos_LoadDataFromRegistry("ExeDate", pdata, &cbdata, false);
	UpdatedExe = (exedate.dwLowDateTime != data)
		? 1
		: 0; // bool? Note: global UpdatedExe is defined as uint32_t

	WindowStartX = 0x80000000;
	// cbdata = 4;
	gos_LoadDataFromRegistry("Window_X", reinterpret_cast<PBYTE>(&WindowStartX), &cbdata, false);
	// cbdata = 4;
	WindowStartY = 0x80000000;
	gos_LoadDataFromRegistry("Window_Y", reinterpret_cast<PBYTE>(&WindowStartY), &cbdata, false);
	if (WindowStartX > GetDeviceCaps(DesktopDC, HORZRES)) // Width, in millimeters, of the physical screen
		WindowStartX = 0x80000000;
	if (WindowStartY > GetDeviceCaps(DesktopDC, VERTRES)) // Height, in millimeters, of the physical screen
		WindowStartY = 0x80000000;

	PerfCounterSelected = static_cast<uint32_t>(-1); // 0xffffffff
	gos_LoadDataFromRegistry(
		"PerfCounter", reinterpret_cast<PBYTE>(&PerfCounterSelected), &cbdata, false);
	if (PerfCounterSelected != static_cast<uint32_t>(-1)) // 0xffffffff
	{
		SetPerformanceMonitor(PerfCounterSelected);
		NewPerformanceRegister = true;
	}

	if (UpdatedExe == 0) // false?
	{
		memset(ProfileFlags, 0, 32);
		ProfileFlags[0] = 1;

		cbdata = 32;
		gos_LoadDataFromRegistry("DebuggerGraphs", ProfileFlags, &cbdata, false);
		// cbdata = 32;
		gos_LoadDataFromRegistry(
			"DebuggerGraphMode", reinterpret_cast<PBYTE>(&GraphMode), &cbdata, false);
		// cbdata = 32;
		gos_LoadDataFromRegistry(
			"DebuggerGraphInfo", reinterpret_cast<PBYTE>(&ShowColorInfo), &cbdata, false);
	}

	cbdata = 32;
	data = 0x1231;
	pdata = reinterpret_cast<PBYTE>(&data);
	gos_LoadDataFromRegistry("DebuggerOptions", pdata, &cbdata, false);

	gGameLogicActive = data & 1;
	gControlsActive = (data >> 1) & 1;
	gStopSystem = (data >> 2) & 1;
	gFrameGraph = (data >> 3) & 1;
	gEnableParallel = (data >> 4) & 1;
	NoDebuggerStats = (data >> 5) & 1;
	gShowGraphsAsTime = (data >> 6) & 1;
	gShowGraphBackground = (uint8_t)data >> 8;
	gShowLFControls = (data >> 9) & 1;
	gNoGraph = (data >> 10) & 1;
	gTextureOverrun = (data >> 11) & 1;
	gShowAverage = (data >> 12) & 1;
}

/******************************************************************************/
/// <summary>
/// <c>RegistryManagerUninstall</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <returns></returns>
void __stdcall RegistryManagerUninstall(void)
{
	HANDLE hfile;
	FILETIME exedate;
	uint32_t data;

	data = 0;
	gos_SaveDataToRegistry("Window_X", reinterpret_cast<PBYTE>(&WindowStartX), 4u);
	gos_SaveDataToRegistry("Window_Y", reinterpret_cast<PBYTE>(&WindowStartY), 4u);
	gos_SaveDataToRegistry("PerfCounter", reinterpret_cast<PBYTE>(&PerfCounterSelected), 4u);
	gos_SaveDataToRegistry("BadShutDown", reinterpret_cast<PBYTE>(&data), 4u);

	RegDebuggerOptions debugoptions;
	debugoptions.GameLogicActive = gGameLogicActive != 0 ? 0x0001 : 0;
	debugoptions.ControlsActive = gControlsActive != 0 ? 0x0002 : 0;
	debugoptions.StopSystem = gStopSystem != 0 ? 0x0004 : 0;
	debugoptions.FrameGraph = gFrameGraph != 0 ? 0x0008 : 0;
	debugoptions.EnableParallel = gEnableParallel != 0 ? 0x0010 : 0;
	debugoptions.NoDebuggerStats = NoDebuggerStats != 0 ? 0x0020 : 0;
	debugoptions.ShowGraphsAsTime = gShowGraphsAsTime != 0 ? 0x0040 : 0;
	debugoptions.ShowGraphBackground = gShowGraphBackground != 0 ? 0x0100 : 0;
	debugoptions.ShowLFControls = gShowLFControls != 0 ? 0x0200 : 0;
	debugoptions.NoGraph = gNoGraph != 0 ? 0x0400 : 0;
	debugoptions.TextureOverrun = gTextureOverrun != 0 ? 0x0800 : 0;
	debugoptions.ShowAverage = gShowAverage != 0 ? 0x1000 : 0;
	gos_SaveDataToRegistry("DebuggerOptions", reinterpret_cast<PBYTE>(&debugoptions), 4u); // ?

	memset(&ProfileFlags, 0, 0x20u);
	uint32_t ix = 0;
	while (pStatistics)
	{
		if (!(pStatistics->Flags & 0x40000000))
		{
			if (pStatistics->Flags & 2)
				ProfileFlags[ix >> 3] |= 1 << (ix & 7);
			++ix;
		}
		pStatistics = pStatistics->pNext;
	}

	gos_SaveDataToRegistry("DebuggerGraphs", &ProfileFlags, 0x20u);
	gos_SaveDataToRegistry("DebuggerGraphMode", &GraphMode, 4u);
	gos_SaveDataToRegistry("DebuggerGraphInfo", &ShowColorInfo, 4u);

	hfile =
		::CreateFileA(_pgmptr, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	::GetFileTime(hfile, nullptr, nullptr, &exedate);
	::CloseHandle(hfile);
	gos_SaveDataToRegistry("ExeDate", reinterpret_cast<PBYTE>(&exedate), 4u);

	if ((Environment.version != nullptr) && *Environment.version)
	{
		data = 1;
		gos_SaveDataToRegistry(Environment.version, reinterpret_cast<PBYTE>(&data), 4u);
	}
}

/******************************************************************************/
/// <summary>
/// <c>gos_LoadDataFromRegistry</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="valuename"></param>
/// <param name="pdata"></param>
/// <param name="pcbdata"></param>
/// <param name="ishklm"></param>
/// <returns></returns>
void __stdcall gos_LoadDataFromRegistry(PSTR valuename, PBYTE pdata, puint32_t pcbdata, bool ishklm)
{
	HKEY hkey;
	LSTATUS status;

	// if ( !keyname || !pdata || !pcbdata )
	//{
	//	if ( ErrorHandler(10, "keyName && pData && szData") )
	//		__debugbreak();
	//}

	status = ::RegOpenKeyA(HKEY_LOCAL_MACHINE, &RegistryKey, &hkey);
	if (status == ERROR_SUCCESS)
	{
		status = ::RegQueryValueExA(hkey, valuename, nullptr, nullptr, pdata, pcbdata);
		::RegCloseKey(hkey);
	}
	else
		*pcbdata = 0;
}

/******************************************************************************/
/// <summary>
/// <c>gos_SaveDataToRegistry</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="keyname"></param>
/// <param name="pdata"></param>
/// <param name="cbdata"></param>
/// <returns></returns>
void __stdcall gos_SaveDataToRegistry(PSTR keyname, PBYTE pdata, uint32_t cbdata)
{
	HKEY hkey;

	// if ( (!keyname || !pdata || !cbdata) && ErrorHandler(10, "keyName && pData && szData") )
	//	__debugbreak();

	if (RegistryKey)
	{
		::RegCreateKeyA(HKEY_LOCAL_MACHINE, &RegistryKey, &hkey);
		if (cbdata == static_cast<uint32_t>(sizeof(DWORD))) // 4u
			::RegSetValueExA(hkey, keyname, 0, REG_DWORD, pdata, cbdata);
		else
			::RegSetValueExA(hkey, keyname, 0, REG_BINARY, pdata, cbdata);
		::RegCloseKey(hkey);
	}
}

/******************************************************************************/
/// <summary>
/// <c>gos_SaveStringToRegistry</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="keyname"></param>
/// <param name="pdata"></param>
/// <param name="cbdata"></param>
/// <returns></returns>
void __stdcall gos_SaveStringToRegistry(PSTR keyname, PBYTE pdata, uint32_t cbdata)
{
	HKEY hkey;

	// if ( (!keyname || !pdata || !cbdata) && ErrorHandler(10, "keyName && pData && szData") )
	//	__debugbreak();

	if (RegistryKey)
	{
		::RegCreateKeyA(HKEY_LOCAL_MACHINE, &RegistryKey, &hkey);
		::RegSetValueExA(hkey, keyname, 0, REG_SZ, pdata, cbdata);
		::RegCloseKey(hkey);
	}
}

/******************************************************************************/
/// <summary>
/// <c>ReadRegistry</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="keyname"></param>
/// <param name="valuename"></param>
/// <param name="ishklm"></param>
/// <returns>PSTR</returns>
PSTR __stdcall ReadRegistry(PSTR keyname, PSTR valuename, bool ishklm)
{
	LSTATUS status;
	uint32_t cbdata;
	HKEY hkey;
	static uint8_t Buffer[0x200];

	cbdata = 0x200;
	Buffer[0] = 0;

	status = ::RegOpenKeyA(HKEY_LOCAL_MACHINE, keyname, &hkey);
	if (status == ERROR_SUCCESS)
	{
		status = ::RegQueryValueExA(hkey, valuename, nullptr, nullptr, Buffer, &cbdata);
		::RegCloseKey(hkey);
	}

	return (status == ERROR_SUCCESS) ? static_cast<PSTR>(Buffer) : nullptr;
}

/******************************************************************************/
/// <summary>
/// <c>ReadRegistryHKCU</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="keyname"></param>
/// <param name="valuename"></param>
/// <param name="ishklm"></param>
/// <returns>PSTR</returns>
PSTR __stdcall ReadRegistryHKCU(PSTR keyname, PSTR valuename, bool ishklm)
{
	LSTATUS status;
	uint32_t cbdata;
	HKEY hkey;

	static uint8_t Buffer[0x200];

	cbdata = 0x200;
	Buffer[0] = 0;

	status = ::RegOpenKeyA(HKEY_CURRENT_USER, keyname, &hkey);
	if (status == ERROR_SUCCESS)
	{
		status = ::RegQueryValueExA(hkey, valuename, nullptr, nullptr, Buffer, &cbdata);
		::RegCloseKey(hkey);
	}

	return (status == ERROR_SUCCESS) ? static_cast<PSTR>(Buffer) : nullptr;
}
