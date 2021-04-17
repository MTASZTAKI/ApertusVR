OSX xcode setting for VLFT launcher
In XCODE click on project and choose apeVLFTLauncher target
In build phases create the following copy file phases:
1.
	Destination: Resources
	subpath: ./CharacterGLB
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
	subpath: 
	files: from build/lib/ the Debug and Release folders
		