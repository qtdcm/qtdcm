cmake_minimum_required(VERSION 3.0.2)

project(QTDCM VERSION "2.1.1")

option(BUILD_EXAMPLE       "Build qtdcm example application" OFF)
option(BUILD_DOCUMENTATION "Build QtDcm Documentation (add a Documentation target)" OFF)
option(BUILD_PACKAGE       "Configure QtDcm packaging" OFF)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
if (WIN32)
  set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
else()
  set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
endif()
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

option(DCMTK_FIND_PACKAGE_USE_CONFIG_ONLY 
  "Do not use any FindDCMTK.cmake module found in the CMAKE_MODULE_PATH to find DCMTK." 
  OFF)

if(DCMTK_FIND_PACKAGE_USE_CONFIG_ONLY)
  set(DCMTK_FIND_PACKAGE_STATEGY NO_MODULE)
endif()

set(${PROJECT_NAME}_CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_CMAKE_MODULE_PATH}")

# External project configuration
if(${CONFIGURE_EXTERNALS})
  add_subdirectory(externals)
  return()
endif()

# Includes
include(GNUInstallDirs)
include(QtDCMSettings)
if(BUILD_PACKAGE)
  include(QtDcmCPackConfig)
endif()

# External inclusion:
export(PACKAGE ${PROJECT_NAME})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtDCMConfig.cmake.in
              "${CMAKE_CURRENT_BINARY_DIR}/QtDCMConfig.cmake" @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtDCMConfig.install.cmake.in
              "${PROJECT_BINARY_DIR}/install/QtDCMConfig.cmake" @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtDCMConfigVersion.cmake.in
              "${CMAKE_CURRENT_BINARY_DIR}/QtDCMConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/install/QtDCMConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/QtDCMConfigVersion.cmake"
  DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/qtdcm-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})

install(EXPORT ${PROJECT_NAME}Depends DESTINATION  ${CMAKE_INSTALL_LIBDIR}/cmake/qtdcm-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR})

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory(src)
if (BUILD_EXAMPLE)
  set(QTDCM_DIR ${${PROJECT_NAME}_BINARY_DIR})
  add_subdirectory(example)
endif()
if(BUILD_DOCUMENTATION)
  add_subdirectory(documentation)
endif()
