cmake_minimum_required(VERSION 3.1)

project(QtDcm VERSION "2.0.2")

option(BUILD_EXAMPLE "Build qtdcm example application" OFF)
option(BUILD_DOCUMENTATION "Build QtDcm Documentation (add a Documentation target)" OFF)
option(BUILD_PACKAGE "Configure QtDcm packaging" OFF)

if(BUILD_PACKAGE)
  include(${PROJECT_SOURCE_DIR}/CMake/CPackConfig.cmake)
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
if (WIN32)
  set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
else()
  set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
endif()
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#Configure doxygen documentation
if(BUILD_DOCUMENTATION)
  add_subdirectory(Documentation/Doxygen)
endif()

set (${PROJECT_NAME}_CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_CMAKE_MODULE_PATH}")

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory(Code)

if (BUILD_EXAMPLE)
  add_subdirectory(Example)
endif()

## ###################################################################
## INSTALL RULES
## ###################################################################
if( EXISTS ${${PROJECT_NAME}_BINARY_DIR}/QtDCMConfig.cmake AND EXISTS ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)
  install(FILES
    ${${PROJECT_NAME}_BINARY_DIR}/QtDCMConfig.cmake
    ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake
    DESTINATION
    share/qtdcm)
endif()
