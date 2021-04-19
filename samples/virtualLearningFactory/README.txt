OSX xcode setting for VLFT launcher
In XCODE click on project and choose apeVLFTLauncher target
In build phases create the following copy file phases:
1.
	Destination: Resources
	subpath: ../assets/models/avatar/
	files: every file from source/plugin/apeFilamentApplicationPlugin/resources
2.
	Destination: Resources
	subpath: ../samples
	files: the source/samples/virtualLearningFactory folder
2.
	Destination: Resources
	subpath: ../../Contents/
	files: from build/filamentapp/filament-build/samples the default_env and assets folders
3.
	Destination: Frameworks
	subpath: ./Release
	files: from build/lib/ the Debug and Release folders

Create New link binary with libraries and add:
	quartzcore
	corevideo
	metal
	cocoa
	opengl
	libcurl4
	corefoundation

Build, if successful:
go to app bundle Contents/MacOS and write in terminal:
	install_name_tool -add_rpath @executable_path/../Frameworks/Release apeVLFTLauncher
		