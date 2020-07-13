set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Semicolon separated list" FORCE)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# use solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# cmake sanitizer
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/externals/sanitizersCmake/cmake" ${CMAKE_MODULE_PATH})