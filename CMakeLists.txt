cmake_minimum_required(VERSION 3.16)
project(stde)

# Grab all sources
file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB INCLUDES "src/*.hpp" "src/*/*.hpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/tests.cpp")

set(STDE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src")
set(STDE_LIBRARIES $<TARGET_FILE:stde>)

include(GNUInstallDirs)

add_library(stde SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src PRIVATE example)

find_package(ZLIB)
target_link_libraries(stde ZLIB::ZLIB)

install(TARGETS ${PROJECT_NAME} EXPORT stdeConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(EXPORT stdeConfig DESTINATION share/stde/cmake)

export(TARGETS ${PROJECT_NAME} FILE stdeConfig.cmake)




add_executable(tests example/tests.cpp)
target_link_libraries(tests stde)

