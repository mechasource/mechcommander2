@echo off

:: +---debug
:: |       dllplatform.lib
:: |       gameos.lib
:: |       gameplatform.lib
:: |       mfcplatform.lib
:: |       zlib.lib
:: |
:: +---profile
:: |       dllplatform.lib
:: |       gameos.lib
:: |       gameplatform.lib
:: |       mfcplatform.lib
:: |       zlib.lib
:: |
:: +---release
::         dllplatform.lib
::         gameos.lib
::         gameplatform.lib
::         mfcplatform.lib
::         zlib.lib

:: Run the script in place to generate the tree and object files
:: Note: Object files with spaces in the name will not be found by the loop
::


@set CURDIR=%CD%
:: @set VSINSTALLDIR=E:\Development\msdev\VS2013\
:: @set VCINSTALLDIR=%VSINSTALLDIR%VC\

@set CPUDIR=x86
@set LIBDIR=debug
call :extractlibs
call :movepngzlib
goto :end

:: ========================================================================
:extractlibs
@echo.
@echo Creating directories...
@if not exist "%CPUDIR%" (@md %CPUDIR%)
@for /F "eol=;" %%i in ( libs.txt ) do @if not exist "%CPUDIR%\%%i" (@md %CPUDIR%\%%i)
@echo.
@echo Listing libraries...
@for /F "eol=;" %%i in ( libs.txt ) do @link.exe -lib -list:%CPUDIR%\%%i.txt "%LIBDIR%\%%i.lib" > nul 2>&1
@echo.
@echo Exctracting object files...
@for /F "eol=;" %%i in ( libs.txt ) do (
    @pushd %CPUDIR%\%%i
    @echo Exctracting from %LIBDIR%\%%i.lib...
    @for /F %%y in ( ..\%%i.txt ) do  @if not exist %%y (@link.exe -lib -extract:"%%y" "..\..\%LIBDIR%\%%i.lib"  > nul 2>&1)
    @popd
)
:: well the library names >.<
@echo dealing with weird object file names...

@pushd %CPUDIR%\gameos
@set GAMEOSLIB=..\..\%LIBDIR%\gameos.lib
@set FILE1=3d_tl.obj
@set FILE2=texture_api.obj
@set FILE3=texture_convert.obj
@set FILE4=texture_create.obj
@set FILE5=texture_format.obj
@set FILE6=texture_manager.obj
@set FILE7=texture_mipmap.obj
@set FILE8=texture_original.obj
@set FILE9=texture_sysmem.obj
@set FILE10=texture_update.obj
@set FILE11=texture_vidmem.obj
@set FILE12=sound_api.obj
@set FILE13=sound_ds3dchannel.obj
@set FILE14=sound_ds3dmixer.obj
@set FILE15=sound_renderer.obj
@set FILE16=sound_resource.obj

@if not exist %FILE1%  (@link.exe -lib -extract:".\Debug\3D T&L.obj"            %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE2%  (@link.exe -lib -extract:".\Debug\Texture API.obj"       %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE3%  (@link.exe -lib -extract:".\Debug\Texture Convert.obj"   %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE4%  (@link.exe -lib -extract:".\Debug\Texture Create.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE5%  (@link.exe -lib -extract:".\Debug\Texture Format.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE6%  (@link.exe -lib -extract:".\Debug\Texture Manager.obj"   %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE7%  (@link.exe -lib -extract:".\Debug\Texture MipMap.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE8%  (@link.exe -lib -extract:".\Debug\Texture Original.obj"  %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE9%  (@link.exe -lib -extract:".\Debug\Texture SysMem.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE10% (@link.exe -lib -extract:".\Debug\Texture Update.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE11% (@link.exe -lib -extract:".\Debug\Texture VidMem.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE12% (@link.exe -lib -extract:".\Debug\Sound API.obj"         %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE13% (@link.exe -lib -extract:".\Debug\Sound DS3DChannel.obj" %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE14% (@link.exe -lib -extract:".\Debug\Sound DS3DMixer.obj"   %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE15% (@link.exe -lib -extract:".\Debug\Sound Renderer.obj"    %GAMEOSLIB%  > nul 2>&1)
@if not exist %FILE16% (@link.exe -lib -extract:".\Debug\Sound Resource.obj"    %GAMEOSLIB%  > nul 2>&1)

@ren "3D T&L.obj"                   %FILE1%
@ren "Texture API.obj"              %FILE2%
@ren "Texture Convert.obj"          %FILE3%
@ren "Texture Create.obj"           %FILE4%
@ren "Texture Format.obj"           %FILE5%
@ren "Texture Manager.obj"          %FILE6%
@ren "Texture MipMap.obj"           %FILE7%
@ren "Texture Original.obj"         %FILE8%
@ren "Texture SysMem.obj"           %FILE9%
@ren "Texture Update.obj"           %FILE10%
@ren "Texture VidMem.obj"           %FILE11%
@ren "Sound API.obj"                %FILE12%
@ren "Sound DS3DChannel.obj"        %FILE13%
@ren "Sound DS3DMixer.obj"          %FILE14%
@ren "Sound Renderer.obj"           %FILE15%
@ren "Sound Resource.obj"           %FILE16%

@popd
goto :eof

:movepngzlib
@echo moving %CPUDIR% objects...
@if not exist "%CPUDIR%\gameos\pngzlib" (@md %CPUDIR%\gameos\pngzlib)
@for /F "eol=;" %%i in ( pngzlib.txt )  do (
    @if exist "%CPUDIR%\gameos\%%i"   (@move %CPUDIR%\gameos\%%i %CPUDIR%\gameos\pngzlib\%%i  > nul 2>&1)
)
goto :eof

:: ========================================================================

:end

