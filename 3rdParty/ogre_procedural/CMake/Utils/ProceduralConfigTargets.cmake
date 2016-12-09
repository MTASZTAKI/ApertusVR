if (WIN32)
  set(OgreProcedural_RELEASE_PATH "/Release")
  set(OgreProcedural_RELWDBG_PATH "/RelWithDebInfo")
  set(OgreProcedural_MINSIZE_PATH "/MinSizeRel")
  set(OgreProcedural_DEBUG_PATH "/Debug")
  set(OgreProcedural_LIB_RELEASE_PATH "/Release")
  set(OgreProcedural_LIB_RELWDBG_PATH "/RelWithDebInfo")
  set(OgreProcedural_LIB_MINSIZE_PATH "/MinSizeRel")
  set(OgreProcedural_LIB_DEBUG_PATH "/Debug")
  set(OgreProcedural_PLUGIN_PATH "/opt")
  set(OgreProcedural_SAMPLE_PATH "/opt/samples")
elseif (UNIX)
  set(OgreProcedural_RELEASE_PATH "")
  set(OgreProcedural_RELWDBG_PATH "")
  set(OgreProcedural_MINSIZE_PATH "")
  set(OgreProcedural_DEBUG_PATH "/debug")
  if (NOT APPLE)
	set(OgreProcedural_DEBUG_PATH "")
  endif ()
  set(OgreProcedural_LIB_RELEASE_PATH "")
  set(OgreProcedural_LIB_RELWDBG_PATH "")
  set(OgreProcedural_LIB_MINSIZE_PATH "")
  set(OgreProcedural_LIB_DEBUG_PATH "")
  if(APPLE AND OgreProcedural_BUILD_PLATFORM_IPHONE)
    set(OgreProcedural_LIB_RELEASE_PATH "/Release")
  endif(APPLE AND OgreProcedural_BUILD_PLATFORM_IPHONE)
  if (APPLE)
    set(OgreProcedural_PLUGIN_PATH "/")
  else()
    set(OgreProcedural_PLUGIN_PATH "/PROCEDURAL")
  endif(APPLE)
  set(OgreProcedural_SAMPLE_PATH "/PROCEDURAL/Samples")
endif ()

# create vcproj.user file for Visual Studio to set debug working directory
function(procedural_create_vcproj_userfile TARGETNAME)
  if (MSVC)
    configure_file(
	  ${OgreProcedural_TEMPLATES_DIR}/VisualStudioUserFile.vcproj.user.in
	  ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcproj.user
	  @ONLY
	)
    configure_file(
	  ${OgreProcedural_TEMPLATES_DIR}/VisualStudioUserFile.vcxproj.user.in
	  ${CMAKE_CURRENT_BINARY_DIR}/${TARGETNAME}.vcxproj.user
	  @ONLY
	)
  endif ()
endfunction(procedural_create_vcproj_userfile)

# install targets according to current build type
function(procedural_install_target TARGETNAME)		
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "bin${OgreProcedural_RELEASE_PATH}" CONFIGURATIONS Release None ""
		RUNTIME DESTINATION "bin${OgreProcedural_RELEASE_PATH}" CONFIGURATIONS Release None ""
		LIBRARY DESTINATION "lib${OgreProcedural_LIB_RELEASE_PATH}" CONFIGURATIONS Release None ""
		ARCHIVE DESTINATION "lib${OgreProcedural_LIB_RELEASE_PATH}" CONFIGURATIONS Release None ""
		FRAMEWORK DESTINATION "lib${OgreProcedural_RELEASE_PATH}/Release" CONFIGURATIONS Release None ""
      )
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "bin${OgreProcedural_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		RUNTIME DESTINATION "bin${OgreProcedural_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		LIBRARY DESTINATION "lib${OgreProcedural_LIB_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		ARCHIVE DESTINATION "lib${OgreProcedural_LIB_RELWDBG_PATH}" CONFIGURATIONS RelWithDebInfo
		FRAMEWORK DESTINATION "lib${OgreProcedural_RELWDBG_PATH}/RelWithDebInfo" CONFIGURATIONS RelWithDebInfo
      )
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "bin${OgreProcedural_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		RUNTIME DESTINATION "bin${OgreProcedural_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		LIBRARY DESTINATION "lib${OgreProcedural_LIB_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		ARCHIVE DESTINATION "lib${OgreProcedural_LIB_MINSIZE_PATH}" CONFIGURATIONS MinSizeRel
		FRAMEWORK DESTINATION "lib${OgreProcedural_MINSIZE_PATH}/MinSizeRel" CONFIGURATIONS MinSizeRel
      )
	  install(TARGETS ${TARGETNAME}
		BUNDLE DESTINATION "bin${OgreProcedural_DEBUG_PATH}" CONFIGURATIONS Debug
		RUNTIME DESTINATION "bin${OgreProcedural_DEBUG_PATH}" CONFIGURATIONS Debug
		LIBRARY DESTINATION "lib${OgreProcedural_LIB_DEBUG_PATH}" CONFIGURATIONS Debug
		ARCHIVE DESTINATION "lib${OgreProcedural_LIB_DEBUG_PATH}" CONFIGURATIONS Debug
		FRAMEWORK DESTINATION "lib${OgreProcedural_DEBUG_PATH}/Debug" CONFIGURATIONS Debug
	  )	  
endfunction(procedural_install_target)