cmake_minimum_required(VERSION 3.0.2)

project(dicom)

if(NOT MSVC)
  add_definitions( -Wno-deprecated -DHAVE_CONFIG_H -fPIC )
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_SOURCE_DIR}")

find_package( Qt5 REQUIRED COMPONENTS Core Widgets Network )
get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
get_filename_component(QT5_BINARY_DIR ${QT5_QMAKE_EXECUTABLE} PATH)

find_package( QtDCM REQUIRED )
find_package( ITK REQUIRED )
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif(ITK_FOUND)

set(${PROJECT_NAME}_SRCS
  main.cpp
  dicom.cpp
)

set(${PROJECT_NAME}_HDRS
  dicom.h
)

set(${PROJECT_NAME}_UIS
  dicom.ui
)

set(${PROJECT_NAME}_MOC_HDRS ${${PROJECT_NAME}_HDRS})

qt5_wrap_ui( ${PROJECT_NAME}_UI_HDRS ${${PROJECT_NAME}_UIS})
qt5_wrap_cpp( ${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_HDRS})

set(QT_LIBRARIES
  Qt5::Core
  Qt5::Widgets
  Qt5::Network
)

if(WIN32)
  list(APPEND QT_LIBRARIES Qt5::WinMain)
endif()

set(INCLUDE_DIRECTORIES
  ${QTDCM_INCLUDE_DIRS}
  ${QTDCM_BINARY_DIR}
  ${${PROJECT_NAME}_BINARY_DIR}
  ${${PROJECT_NAME}_SOURCE_DIR}
)
include_directories(${INCLUDE_DIRECTORIES})

add_executable(${PROJECT_NAME} WIN32 
               ${${PROJECT_NAME}_HDRS}  
               ${${PROJECT_NAME}_SRCS} 
               ${${PROJECT_NAME}_MOC_HDRS} 
               ${${PROJECT_NAME}_MOC_SRCS} 
               ${${PROJECT_NAME}_UI_HDRS})

target_link_libraries( ${PROJECT_NAME}
  ${QTDCM_LIBRARIES}
  ${QT_LIBRARIES}
)

if(APPLE)
  include(FixDCMTKMacLink)
  message("${QTDCM_DIR}")
  FixDCMTKMacExeLink(dicom ${QTDCM_BINARY_DIR})
endif()

install(TARGETS dicom RUNTIME DESTINATION bin)
