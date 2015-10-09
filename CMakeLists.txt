cmake_minimum_required(VERSION 3.1)

project(QTDCM VERSION "2.0.2")

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

set(${PROJECT_NAME}_CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_CMAKE_MODULE_PATH}")

include(QtDCMSettings)
if(BUILD_PACKAGE)
  include(QtDcmCPackConfig)
endif()

# External inclusion:
set(${PROJECT_NAME}_USE_FILE ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)

configure_file(${${PROJECT_NAME}_SOURCE_DIR}/cmake/UseQtDCM.cmake.in
              ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)
configure_file(${${PROJECT_NAME}_SOURCE_DIR}/cmake/QtDCMConfig.cmake.in
              ${${PROJECT_NAME}_BINARY_DIR}/QtDCMConfig.cmake)

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory(src)
if (BUILD_EXAMPLE)
  add_subdirectory(example)
endif()
if(BUILD_DOCUMENTATION)
  add_subdirectory(documentation/Doxygen)
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
