###############################################################
# Install documentation
# Build API docs, if doxygen is available
###############################################################

if(DOXYGEN_FOUND)
	add_subdirectory(manual/illustrations)
endif()

# Build and install API documentation if doxygen is available
if(DOXYGEN_FOUND)
	configure_file(${OgreProcedural_TEMPLATES_DIR}/html.cfg.in ${OgreProcedural_BINARY_DIR}/html.cfg)
	add_custom_target(OgreProceduralDoc
					COMMAND  ${DOXYGEN_EXECUTABLE} ${OgreProcedural_BINARY_DIR}/html.cfg
					WORKING_DIRECTORY ${OgreProcedural_SOURCE_DIR}/docs
					DEPENDS OgreProceduralIllustrations
					COMMENT "Building documentation" VERBATIM)
	file(WRITE ${OgreProcedural_SOURCE_DIR}/docs/api/temp.txt "This file should be deleted before installation. It is only here to force the creation of the api folder")
	file(REMOVE ${OgreProcedural_SOURCE_DIR}/docs/api/temp.txt)
	if (OgreProcedural_INSTALL_DOCS)
		install(DIRECTORY ${OgreProcedural_BINARY_DIR}/docs/api DESTINATION ${OgreProcedural_DOCS_PATH})
	endif()

	# cleanup $OgreProcedural_DOCS_PATH on "make clean"
	set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${OgreProcedural_DOCS_PATH})

	# add doxygen as dependency to OgreProceduralDoc-target
	get_target_property(DOC_TARGET OgreProceduralDoc TYPE)
	if(NOT DOC_TARGET)
		add_custom_target(OgreProceduralDoc)
	endif()
endif()