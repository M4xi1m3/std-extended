cmake_minimum_required(VERSION 3.16)
project(stde)

# Grab all sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Grab all headers
file(GLOB INCLUDES "src/*.hpp" "src/*/*.hpp")

# Use standard install dirs
include(GNUInstallDirs)

# this is the "object library" target: compiles the sources only once
add_library(OBJECTS OBJECT ${SOURCES})

# shared libraries need PIC
set_property(TARGET OBJECTS PROPERTY POSITION_INDEPENDENT_CODE 1)

# Create shared library
# add_library(stde SHARED ${SOURCES})

# shared and static libraries built from the same object files
add_library(stde SHARED $<TARGET_OBJECTS:OBJECTS>)
add_library(stde_static STATIC $<TARGET_OBJECTS:OBJECTS>)

# Set include directory for the library and the examples
target_include_directories(OBJECTS PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)


# Add zlib
find_package(ZLIB)
target_link_libraries(stde ZLIB::ZLIB)
target_link_libraries(stde_static ZLIB::ZLIB)

# Add install for shared lib
install(TARGETS ${PROJECT_NAME} EXPORT stdeConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

# Add install for includes
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

# Add install for config
install(EXPORT stdeConfig DESTINATION share/stde/cmake)
export(TARGETS ${PROJECT_NAME} FILE stdeConfig.cmake)

# Add tests executable
add_executable(tests example/tests.cpp)

target_include_directories(tests PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE example)

target_link_libraries(tests stde_static)

