OSX xcode settings for VLFT launcher
In XCODE click on project and choose apeVLFTLauncher target

First build apeFilamentApplicationPlugin before building the apeVLFTLauncher.

In build settings check if other c++ flags contains std=c++17, if not add it

In build phases create the following copy file phases:
1.
	Destination: Resources
	subpath: assets/models/avatar/
	files: from source/plugin/apeFilamentApplicationPlugin/resources every file except the VLFT_3Dlogo files
2.
	Destination: Resources
	subpath: ../samples
	files: the source/samples/virtualLearningFactory folder
3.
	Destination: Resources
	subpath: ../../Contents/
	files: from build/filamentapp/filament-build/samples the default_env and assets folders
In Debug mode
4.
	Destination: Frameworks
	subpath: ./Debug
	files: from build/lib/Debug everything
In Release mode
4.
	Destination: Frameworks
	subpath: ./Release
	files: from build/lib/Release everything

5.
	Destination: Resources
	subpath: assets/models/logo/
	files: from source/plugin/apeFilamentApplicationPlugin/resources the VLFT_3Dlogo.gltf and VLFT_3Dlogo.bin

Create New link binary with libraries and add:
	quartzcore
	corevideo
	metal
	cocoa
	opengl
	libcurl4
	corefoundation

After this, you can either build and run apeVLFTlauncher itself, or if it cannot copy the dylibs from build/lib run ALL_BUILD first.
		