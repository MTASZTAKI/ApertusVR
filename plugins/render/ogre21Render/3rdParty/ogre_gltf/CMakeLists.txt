cmake_minimum_required(VERSION 3.8)

project(Ogre_glTF)

#to build a library
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INSTALL_PREFIX "./output")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -D_DEBUG=1") 

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "_d")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)

include(Ogre_glTF_ConfigTargets)

option(Ogre_glTF_STATIC "Static build" FALSE)

if (Ogre_glTF_STATIC)
  add_definitions(-DOgre_glTF_STATIC)
  set(Ogre_glTF_LIB_TYPE STATIC)
  set(Ogre_glTF_LIB_SUFFIX "Static")
else ()
  set(Ogre_glTF_LIB_TYPE SHARED)
  set(Ogre_glTF_LIB_SUFFIX "")
endif ()

#Get Ogre from your system. May need to set some variables for Windows folks
find_package(OGRE COMPONENTS HlmsPbs REQUIRED)
file(GLOB librarySources ./src/*.cpp ./src/private_headers/*.hpp ./include/*.hpp)

add_library(${PROJECT_NAME} ${Ogre_glTF_LIB_TYPE} ${librarySources})

target_compile_definitions(${PROJECT_NAME} PUBLIC Ogre_glTF_DLL_EXPORT_CONFIG_ON)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}${Ogre_glTF_LIB_SUFFIX})

target_include_directories( Ogre_glTF PUBLIC
	#Ogre and the physics based high level material system
	${OGRE_INCLUDE_DIRS}
	${OGRE_HlmsPbs_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIR}/Hlms/Common
	#local include directory
	./include
	./src/private_headers
	./thirdParty/tinygltf/
)

target_link_libraries(Ogre_glTF
	${OGRE_LIBRARIES}
	${OGRE_HlmsPbs_LIBRARIES}
)

add_subdirectory(Samples)

#installation

install(FILES
./include/Ogre_glTF.hpp
./include/Ogre_glTF_OgrePlugin.hpp
./include/Ogre_glTF_OgreResource.hpp
./include/Ogre_glTF_DLL.hpp
DESTINATION
"include")

install(TARGETS Ogre_glTF DESTINATION "bin")
