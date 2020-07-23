add_subdirectory(Common)
add_subdirectory(LoadMesh)
add_subdirectory(SkinnedMesh)

add_custom_target(CopyHLMS ALL
    ${CMAKE_COMMAND} -E copy_directory ${OGRE_MEDIA_DIR}/Hlms ${PROJECT_BINARY_DIR}/Media/Hlms
)