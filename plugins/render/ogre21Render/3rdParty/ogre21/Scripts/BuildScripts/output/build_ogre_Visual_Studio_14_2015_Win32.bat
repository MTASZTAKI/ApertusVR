
@echo off
SETLOCAL

set OGRE_BRANCH_NAME=v2-1
set GENERATOR="Visual Studio 14 2015"
set PLATFORM=Win32

set CMAKE_BIN_x86="C:\Program Files (x86)\CMake\bin\cmake.exe"
set CMAKE_BIN_x64="C:\Program Files\CMake\bin\cmake.exe"
IF EXIST %CMAKE_BIN_x64% (
	echo CMake 64-bit detected
	set CMAKE_BIN=%CMAKE_BIN_x64%
) ELSE (
	IF EXIST %CMAKE_BIN_x86% (
		echo CMake 32-bit detected
		set CMAKE_BIN=%CMAKE_BIN_x86%
	) ELSE (
		echo Cannot detect either %CMAKE_BIN_x86% or
		echo %CMAKE_BIN_x64% make sure CMake is installed
		EXIT /B 1
	)
)
echo Using CMake at %CMAKE_BIN%

mkdir Ogre
cd Ogre
IF NOT EXIST ogredeps (
	mkdir ogredeps
	echo --- Cloning Ogredeps ---
	hg clone https://bitbucket.org/cabalistic/ogredeps ogredeps
) ELSE (
	echo --- Ogredeps repo detected. Cloning skipped ---
)
cd ogredeps
mkdir build
cd build
echo --- Building Ogredeps ---
%CMAKE_BIN% -G %GENERATOR% -A %PLATFORM% ..
%CMAKE_BIN% --build . --config Debug
%CMAKE_BIN% --build . --target install --config Debug
%CMAKE_BIN% --build . --config Release
%CMAKE_BIN% --build . --target install --config Release

cd ../../
IF NOT EXIST ogre-next (
	echo --- Cloning Ogre v2-1 ---
	git clone --branch %OGRE_BRANCH_NAME% https://github.com/OGRECave/ogre-next
)
cd ogre-next
IF NOT EXIST Dependencies (
	mklink /D Dependencies ..\ogredeps\build\ogredeps
)
mkdir build
cd build
echo --- Running CMake configure ---
%CMAKE_BIN% -D OGRE_USE_BOOST=0 -D OGRE_CONFIG_THREAD_PROVIDER=0 -D OGRE_CONFIG_THREADS=0 -D OGRE_BUILD_COMPONENT_SCENE_FORMAT=1 -D OGRE_BUILD_SAMPLES2=1 -D OGRE_BUILD_TESTS=1 -D OGRE_DEPENDENCIES_DIR=..\..\ogredeps\build\ogredeps -G %GENERATOR% -A %PLATFORM% ..
echo --- Building Ogre (Debug) ---
%CMAKE_BIN% --build . --config Debug
%CMAKE_BIN% --build . --target install --config Debug
echo --- Building Ogre (Release) ---
%CMAKE_BIN% --build . --config Release
%CMAKE_BIN% --build . --target install --config Release

echo Done!

ENDLOCAL
