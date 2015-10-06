cmake_minimum_required(VERSION 2.6)

project(dicom)

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
  set(QT_LIBRARIES ${QT_LIBRARIES}
    Qt5::WinMain
  )
endif()

include_directories(${CMAKE_BINARY_DIR} ${QTDCM_INCLUDE_DIR} ${QTDCM_DIR} ${${PROJECT_NAME}_BINARY_DIR})

add_executable( ${PROJECT_NAME} WIN32 ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS}  ${${PROJECT_NAME}_MOC_HDRS} ${${PROJECT_NAME}_MOC_SRCS} ${${PROJECT_NAME}_UI_HDRS})
target_link_libraries( ${PROJECT_NAME}
  qtdcm
  ${QT_LIBRARIES}
)

if(WIN32)
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS")
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
   set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif()

if(APPLE)
  include(FixDCMTKMacLink)
  message("${QTDCM_DIR}")
  FixDCMTKMacExeLink(dicom ${QTDCM_BINARY_DIR})
endif(APPLE)

install(TARGETS dicom RUNTIME DESTINATION bin)
