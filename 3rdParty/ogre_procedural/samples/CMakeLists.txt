add_subdirectory(primitives)

add_subdirectory(extrusion)

add_subdirectory(svg)

add_subdirectory(material)

# Install sample sources
if (OgreProcedural_INSTALL_SAMPLES_SOURCE)
  if (WIN32 OR APPLE)
    set(OgreProcedural_SAMPLES_DIR Samples)
  elseif (UNIX)
    set(OgreProcedural_SAMPLES_DIR share/PROCEDURAL/Samples)
  endif ()
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION ${OgreProcedural_SAMPLES_DIR}
    REGEX "^CMakeLists.txt$" EXCLUDE
    PATTERN "Makefile.am" EXCLUDE
    #PATTERN "Media" EXCLUDE
    PATTERN "bin" EXCLUDE
    PATTERN "setup" EXCLUDE
	PATTERN "obj" EXCLUDE
	PATTERN "scripts" EXCLUDE
  )
  # install a new CMakeLists.txt file to allow building of samples
  configure_file(${OgreProcedural_TEMPLATES_DIR}/SDK_CMakeLists.txt.in ${CMAKE_CURRENT_BINARY_DIR}/../CMakeLists.txt @ONLY)
  configure_file(${OgreProcedural_TEMPLATES_DIR}/SDK_Samples_CMakeLists.txt.in ${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../CMakeLists.txt DESTINATION ${OgreProcedural_SAMPLES_DIR}/../)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeLists.txt DESTINATION ${OgreProcedural_SAMPLES_DIR})
endif ()
