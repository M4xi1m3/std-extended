cmake_minimum_required(VERSION 3.16)
project(stde)

# Grab all sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

# Grab all headers
file(GLOB INCLUDES "src/*.hpp" "src/*/*.hpp")

# Use standard install dirs
include(GNUInstallDirs)

# Create shared library
add_library(stde SHARED ${SOURCES})

# Set include directory for the library and the examples
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src PRIVATE example)

# Add zlib
find_package(ZLIB)
target_link_libraries(stde ZLIB::ZLIB)

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
target_link_libraries(tests stde)

