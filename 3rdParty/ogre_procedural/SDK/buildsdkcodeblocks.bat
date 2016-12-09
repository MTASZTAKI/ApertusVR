@echo off
set GENERATOR="CodeBlocks - MinGW Makefiles"

rem check 7z and dot
7z > NUL
if errorlevel 1 goto 7zerror

set BUILD_DIR=codeblocks

mkdir %BUILD_DIR%
pushd %BUILD_DIR%
rem call CMake
cmake -DOgreProcedural_INSTALL_SAMPLES_SOURCE:BOOL=TRUE -DOgreProcedural_INSTALL_DOCS:BOOL=TRUE -DOGRE_HOME:PATH=%OGRE_SDK_RELEASE% -DCMAKE_BUILD_TYPE=Release -G%GENERATOR% ..\..
if errorlevel 1 goto cmakeerror

rem Read OgreProcedural version
set /p PROCEDURALVERSION=<version.txt

echo Building docs...
rem build docs explicitly since INSTALL doesn't include it
codeblocks.exe OgreProcedural.cbp --build --target=doc

if errorlevel 1 goto msvcerror

rem Delete unnecessary doc files
pushd docs\api\html
del /Q/F *.hhk *.hhc *.map *.md5 *.dot *.hhp *.plist
popd

echo Building release binaries...
rem Build main binaries
codeblocks.exe OgreProcedural.cbp --build --target=install

popd

rem now the same for debug build...
mkdir %BUILD_DIR%_debug
pushd %BUILD_DIR%_debug
rem call CMake
cmake -DOgreProcedural_INSTALL_SAMPLES_SOURCE:BOOL=TRUE -DOGRE_HOME:PATH=%OGRE_SDK_DEBUG% -DCMAKE_BUILD_TYPE=Debug -G%GENERATOR% ..\..
if errorlevel 1 goto cmakeerror
echo Building debug binaries...
rem Build debug binaries
codeblocks.exe OgreProcedural.cbp --build --target=install
popd
pause;
rem Package up
echo Packaging...
set SDKNAME=ProceduralSDK_CB_v%PROCEDURALVERSION%
rmdir /S/Q %SDKNAME%
mkdir %SDKNAME%
xcopy /y /s %BUILD_DIR%\sdk %SDKNAME%
xcopy /y /s %BUILD_DIR%_debug\sdk %SDKNAME%
del /Q/F %SDKNAME%.exe
rem create self-extracting 7zip archive
7z a -r -y -sfx7z.sfx %SDKNAME%.exe %SDKNAME%

echo Done! Test %SDKNAME%.exe and then release
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
echo Codeblocks doesn't need to be installed
goto end

:end

