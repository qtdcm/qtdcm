cmake_minimum_required(VERSION 2.6)

project(QTDCMEXAMPLE)

add_definitions(-g -Wno-deprecated)
add_definitions(-DHAVE_CONFIG_H)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QTDCMEXAMPLE_SOURCE_DIR}")
set (QTDCM_DIR "${QTDCM_BINARY_DIR}")

find_package(QtDCM REQUIRED)
if (QTDCM_FOUND)
  include(${QTDCM_USE_FILE})
endif (QTDCM_FOUND)

find_package(DCMTK2 REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

add_definitions(-DHAVE_CONFIG_H -fPIC)

set(SAMPLE_SRCS
  ${QTDCMEXAMPLE_SOURCE_DIR}/main.cpp
  ${QTDCMEXAMPLE_SOURCE_DIR}/dicom.cpp
)

set(SAMPLE_HDRS
  ${QTDCMEXAMPLE_SOURCE_DIR}/dicom.h
)

set(SAMPLE_UIS
  ${QTDCMEXAMPLE_SOURCE_DIR}/dicom.ui
)

set(SAMPLE_MOC_HDRS ${SAMPLE_HDRS})

qt4_wrap_ui( SAMPLE_UI_HDRS ${SAMPLE_UIS})
qt4_wrap_cpp( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS})

include_directories(${CMAKE_BINARY_DIR} ${QTDCM_INCLUDE_DIR} ${QTDCM_DIR} ${QTDCMEXAMPLE_BINARY_DIR} ${DCMTK_INCLUDE_DIR})

add_executable( dicom ${SAMPLE_SRCS} ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS})# ${qm_files})
target_link_libraries( dicom ${QTDCM_LIBS} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${DCMTK_LIBRARIES} dcmimage tiff jpeg ijg12 ijg16 ijg8)
