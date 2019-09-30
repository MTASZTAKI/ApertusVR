@echo off
if "%1" == "" goto paramErr
if "%VSINSTALLDIR%" == "" goto envErr
if "%OGRE_HOME%" == "" goto envErr

set COMPILER=%1

rem Determine CMake generator
if "%COMPILER%" == "vc71" set GENERATOR="Visual Studio 7 .NET 2003"
if "%COMPILER%" == "vc8" set GENERATOR="Visual Studio 8 2005"
if "%COMPILER%" == "vc8_x64" set GENERATOR="Visual Studio 8 2005 Win64"
if "%COMPILER%" == "vc9" set GENERATOR="Visual Studio 9 2008"
if "%COMPILER%" == "vc9_x64" set GENERATOR="Visual Studio 9 2008 Win64"
if "%COMPILER%" == "vc10" set GENERATOR="Visual Studio 10"
if "%COMPILER%" == "vc10_x64" set GENERATOR="Visual Studio 10 Win64"

if %GENERATOR% == "" goto paramErr

rem check 7z and dot
7z > NUL
if errorlevel 1 goto 7zerror

set BUILD_DIR=%COMPILER%

if "%2" == "clean" rmdir /Q/S %BUILD_DIR%
mkdir %BUILD_DIR%
pushd %BUILD_DIR%
rem call CMake
cmake -DOgreProcedural_INSTALL_SAMPLES_SOURCE:BOOL=TRUE -DOgreProcedural_INSTALL_DOCS:BOOL=TRUE -G%GENERATOR% ..\..
if errorlevel 1 goto cmakeerror

rem Read OgreProcedural version
set /p PROCEDURALVERSION=<version.txt

rem Detect whether we're using full version of VStudio or Express
devenv /? > NUL

if errorlevel 1 goto tryexpress
set DEVENV=devenv
goto detecteddevenv
:tryexpress
set DEVENV=VCExpress
:detecteddevenv

rem build docs explicitly since INSTALL doesn't include it
%DEVENV% OgreProcedural.sln /build "Release" /project "doc"

if errorlevel 1 goto msvcerror

rem Delete unnecessary doc files
pushd docs\api\html
del /Q/F *.hhk *.hhc *.map *.md5 *.dot *.hhp *.plist
popd

echo %cd%

rem Build main binaries
%DEVENV% OgreProcedural.sln /build "Debug" /project "INSTALL"
%DEVENV% OgreProcedural.sln /build "Release" /project "INSTALL"

popd

rem Package up
set SDKNAME=ProceduralSDK_%COMPILER%_v%PROCEDURALVERSION%
rmdir /S/Q %SDKNAME%
move %BUILD_DIR%\sdk %SDKNAME%
del /Q/F %SDKNAME%.exe
rem create self-extracting 7zip archive
7z a -r -y -sfx7z.sfx %SDKNAME%.exe %SDKNAME%

echo Done! Test %SDKNAME%.exe and then release
goto end

:paramErr
echo Required: Build tool (vc71, vc8, vc8x64, vc9, vc9x64, vc10, vc10x64)
set errorlevel=1
goto end

:envErr
echo You need to run this script after running vcvars32.bat
echo You also need to set OGRE_HOME environnement variable to your Ogre SDK's directory
set errorlevel=1
goto end

:cmakeerror
popd
echo CMake not found on your path or CMake error - see above and correct
goto end

:7zerror
echo 7z.exe not found on your path, please add
goto end

:msvcerror
popd
echo Neither devenv.exe nor VCExpress are on your path, use vcvars32.bat
goto end

:end

