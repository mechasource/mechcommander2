/*******************************************************************************
 module.cpp - implementation of DLL entrypoint and DLL exports

 codetest

 2018-09-03 Jerker Back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdinc.h"
#include "module.h"

HRESULT testresource();

/// <summary>
/// <c>WinMain</c> application entry-point function
/// </summary>
/// <remarks>override _AtlModule.WinMain and _AtlModule.Run for custom implementations
/// </remarks>
/// <param name="hinstance">application handle</param>
/// <param name="hPrevInstance">reserved</param>
/// <param name="commandline">pointer to command line arguments</param>
/// <param name="nShowCmd">initial window state</param>
/// <returns>application exit code</returns>
extern "C" int32_t WINAPI
wWinMain(_In_ HINSTANCE hinstance,_In_opt_ HINSTANCE notused,
	_In_ PWSTR commandline,_In_ int32_t windowshow)
{
	(void)hinstance;
	(void)notused;
	(void)commandline;
	(void)windowshow;

	int ir = EXIT_SUCCESS;
	{
		ir = testresource();
	}

	return ir;
}
