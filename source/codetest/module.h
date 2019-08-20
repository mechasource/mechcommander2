/*******************************************************************************
 module.h - main application object definitions

 codetest

 2018-09-03 Jerker Beck, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

/// <summary>
/// <c>CMainModule</c> main application object class
/// </summary>
class CMainModule : public ATL::CAtlExeModuleT<CMainModule>
{
public:
	CMainModule(void)
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		ATLASSERT(_CrtCheckMemory());
	}
	virtual ~CMainModule(void)
	{
		// _CrtDumpMemoryLeaks();
	}

#ifndef _ATL_NO_COM_SUPPORT
	// DECLARE_LIBID(MY_LIBID)
	// DECLARE_REGISTRY_APPID_RESOURCEID(IDR_APPID, APPID_GUIDSTRING)
#endif
};

extern class CMainModule _AtlModule;
