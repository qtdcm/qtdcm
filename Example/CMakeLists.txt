cmake_minimum_required(VERSION 2.6)

project(QTDCMEXAMPLE)

if(NOT MSVC)
  add_definitions( -Wno-deprecated -DHAVE_CONFIG_H -fPIC )
endif()

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_SOURCE_DIR}")
set (QTDCM_DIR "${QTDCM_BINARY_DIR}")

find_package(QtDCM REQUIRED)
if (QTDCM_FOUND)
  include(${QTDCM_USE_FILE})
endif (QTDCM_FOUND)

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif(ITK_FOUND)

find_package( Qt5 REQUIRED COMPONENTS Core Widgets Network)

set(${PROJECT_NAME}_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/main.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/dicom.cpp
)

set(${PROJECT_NAME}_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/dicom.h
)

set(${PROJECT_NAME}_UIS
  ${${PROJECT_NAME}_SOURCE_DIR}/dicom.ui
)

set(${PROJECT_NAME}_MOC_HDRS ${${PROJECT_NAME}_HDRS})

qt5_wrap_ui( ${PROJECT_NAME}_UI_HDRS ${${PROJECT_NAME}_UIS})
qt5_wrap_cpp( ${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_HDRS})

include_directories(${CMAKE_BINARY_DIR} ${QTDCM_INCLUDE_DIR} ${QTDCM_DIR} ${${PROJECT_NAME}_BINARY_DIR})

add_executable( dicom ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS}  ${${PROJECT_NAME}_MOC_HDRS} ${${PROJECT_NAME}_MOC_SRCS} ${${PROJECT_NAME}_UI_HDRS})
target_link_libraries( dicom
  qtdcm
  Qt5::Core
  Qt5::Widgets
  Qt5::Network
)

if(APPLE)
  include(FixDCMTKMacLink)
  message("${QTDCM_DIR}")
  FixDCMTKMacExeLink(dicom ${QTDCM_BINARY_DIR})
endif(APPLE)
