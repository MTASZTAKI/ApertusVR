if(WIN32 OR APPLE)
	set(OgreProcedural_CMAKE_DIR "CMake")
else()
	set(OgreProcedural_CMAKE_DIR "${OgreProcedural_LIB_DIRECTORY}/OgreProcedural/cmake")
endif()

set(INST_FILES
  Packages/FindOIS.cmake
  Packages/FindOGRE.cmake
  Packages/FindOgreProcedural.cmake
  Utils/FindPkgMacros.cmake  
  Utils/PreprocessorUtils.cmake
  Utils/ProceduralConfigTargets.cmake
  Utils/ProceduralMacros.cmake
) 

if (OgreProcedural_INSTALL_SAMPLES_SOURCE)
set(INST_FILES ${INST_FILES}
  Templates/plugins.cfg
  Templates/plugins_d.cfg
  Templates/resources.cfg
  Templates/resources_d.cfg
)
endif()

if (WIN32)
  set(INST_FILES ${INST_FILES}
    Templates/VisualStudioUserFile.vcproj.user.in
    Templates/VisualStudioUserFile.vcxproj.user.in	
  )
endif ()

if (APPLE)
  set(INST_FILES ${INST_FILES}
    Packages/FindIOKit.cmake
  )
  set(STATIC_INST_FILES ${STATIC_INST_FILES}
    Packages/FindCarbon.cmake
    Packages/FindCocoa.cmake
    Packages/FindiPhoneSDK.cmake
  )
endif ()

install(FILES ${INST_FILES} DESTINATION ${OgreProcedural_CMAKE_DIR})
