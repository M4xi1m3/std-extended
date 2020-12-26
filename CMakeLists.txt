cmake_minimum_required(VERSION 3.16)
project(stde)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Grab all sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Grab all headers
file(GLOB INCLUDES "src/*.hpp" "src/*/*.hpp")

# Use standard install dirs
include(GNUInstallDirs)

# this is the "object library" target: compiles the sources only once
add_library(STDE_OBJECTS OBJECT ${SOURCES})

# shared libraries need PIC
set_property(TARGET STDE_OBJECTS PROPERTY POSITION_INDEPENDENT_CODE 1)

# Create shared library
# add_library(stde SHARED ${SOURCES})

# shared and static libraries built from the same object files
add_library(stde SHARED $<TARGET_OBJECTS:STDE_OBJECTS>)
add_library(stde_static STATIC $<TARGET_OBJECTS:STDE_OBJECTS>)

# Set include directory for the library and the examples
target_include_directories(STDE_OBJECTS PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/stde>
    $<INSTALL_INTERFACE:include/stde>
    PRIVATE src)

# Add zlib
find_package(ZLIB)

if(ZLIB_FOUND)
    target_include_directories(STDE_OBJECTS PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(stde ZLIB::ZLIB)
    target_link_libraries(stde_static ZLIB::ZLIB)
else()
    message(WARNING "System doesn't provde ZLib, downloading...")
    configure_file(CMakeLists.txt.in zlib-download/CMakeLists.txt)

    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/zlib-download"
    )
    execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/zlib-download"
    )

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/zlib)

    target_include_directories(STDE_OBJECTS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external/zlib ${CMAKE_CURRENT_BINARY_DIR}/external/zlib)
    target_link_libraries(stde zlib)
    target_link_libraries(stde_static zlib)
endif()

if(WIN32)
    target_link_libraries(stde wsock32 ws2_32)
    target_link_libraries(stde_static wsock32 ws2_32)
endif()

# Add tests executable
add_executable(stdetests example/tests.cpp)

target_include_directories(stdetests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE example)

target_link_libraries(stdetests stde_static)

