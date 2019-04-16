cmake_minimum_required(VERSION 3.0.3)
project(geos_wrap)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# include modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

find_package(GEOS REQUIRED)


file(GLOB_RECURSE GEOSWRAP_SOURCES src/lib/*.cpp)
file(GLOB_RECURSE GEOSWRAP_HEADERS src/include/*.h)

set_source_files_properties(${GEOSWRAP_SOURCES} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -fPIC")

set(GEOSWRAP_INCLUDE_DIR
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src/include>
)

set(GEOSWRAP_INCLUDE_DIRECTORIES
    ${GEOSWRAP_INCLUDE_DIR}
    ${GEOS_INCLUDE_DIR}
)

set(GEOSWRAP_LINK_LIBRARIES
    ${GEOS_LIBRARY}
)

add_library(geos_wrap STATIC ${GEOSWRAP_SOURCES})
set_target_properties(geos_wrap PROPERTIES PUBLIC_HEADER "${GEOSWRAP_HEADERS}")
target_include_directories(geos_wrap PUBLIC ${GEOSWRAP_INCLUDE_DIRECTORIES})
target_link_libraries(geos_wrap PUBLIC ${LINK_LIBRARIES})
set_property(TARGET geos_wrap PROPERTY CXX_STANDARD 14)
