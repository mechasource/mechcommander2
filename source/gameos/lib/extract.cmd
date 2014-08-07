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

@set CURDIR=%CD%
@set VSINSTALLDIR=E:\Development\msdev\VS2013\
@set VCINSTALLDIR=%VSINSTALLDIR%VC\

@set CPUDIR=x86
@set LIBDIR=debug
call :extractlibs

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
    @for /F %%y in ( ..\%%i.txt ) do  @if not exist %%y (@link.exe -lib -extract:%%y "..\..\%LIBDIR%\%%i.lib" > nul 2>&1)
    @popd
)

:: ========================================================================

:end

