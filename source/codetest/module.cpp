/*******************************************************************************
 module.cpp - implementation of DLL entrypoint and DLL exports

 codetest

 2018-09-03 Jerker Beck, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdinc.h"
#include "module.h"

int
testresource();

/// <summary>
/// <c>WinMain</c> application entry-point function
/// </summary>
/// <remarks>override _AtlModule.WinMain and _AtlModule.Run for custom implementations
/// </remarks>
/// <param name="hInstance">application handle</param>
/// <param name="hPrevInstance">reserved</param>
/// <param name="lpCmdLine">pointer to command line arguments</param>
/// <param name="nShowCmd">initial window state</param>
/// <returns>application exit code</returns>
extern "C" int WINAPI
wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR lpCmdLine, _In_ int nShowCmd)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)lpCmdLine;
	(void)nShowCmd;

	int result = EXIT_SUCCESS;
	{
		result = testresource();
	}

	return result;
}
