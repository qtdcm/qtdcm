cmake_minimum_required(VERSION 2.6)

project(QTDCM)

#set(QTDCM_VERSION_MAJOR "0")
#set(QTDCM_VERSION_MINOR "1")
#set(QTDCM_VERSION_PATCH "0")

set(${PROJECT_NAME}_VERSION_MAJOR 0 CACHE STRING "${PROJECT_NAME} major version number.")
set(${PROJECT_NAME}_VERSION_MINOR 1 CACHE STRING "${PROJECT_NAME} minor version number.")
set(${PROJECT_NAME}_VERSION_BUILD 0 CACHE STRING "${PROJECT_NAME} build version number.")
set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD}")

mark_as_advanced(${PROJECT_NAME}_VERSION_MAJOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_MINOR)
mark_as_advanced(${PROJECT_NAME}_VERSION_BUILD)

add_definitions(-g -Wno-deprecated -DPACKAGE_DATE="" -DPACKAGE_VERSION="" -DPACKAGE_VERSION_NUMBER="")

option(BUILD_SHARED_LIBS "Build QtDcm with shared libraries." OFF)
option(BUILD_DOCUMENTATION "Build QtDcm Documentation" OFF)
option(BUILD_PACKAGE "Configure QtDcm packaging" OFF)

option(BUILD_SHARED_LIBS "Build Qtdcm with shared libraries." ON)

if (BUILD_PACKAGE)
  include (${QTDCM_SOURCE_DIR}/CMake/CPackConfig.cmake)
endif(BUILD_PACKAGE)

set(EXECUTABLE_OUTPUT_PATH "${QTDCM_BINARY_DIR}/bin")
if (WIN32 AND BUILD_SHARED_LIBS)
  set(LIBRARY_OUTPUT_PATH "${QTDCM_BINARY_DIR}/bin")
else (WIN32 AND BUILD_SHARED_LIBS)
  set(LIBRARY_OUTPUT_PATH "${QTDCM_BINARY_DIR}/lib")
endif (WIN32 AND BUILD_SHARED_LIBS)
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

#Configure doxygen documentation
if(BUILD_DOCUMENTATION)
  subdirs(Documentation/Doxygen)
endif(BUILD_DOCUMENTATION)

set (QTDCM_CMAKE_MODULE_PATH ${QTDCM_SOURCE_DIR}/CMake)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QTDCM_CMAKE_MODULE_PATH}")

include (${QTDCM_SOURCE_DIR}/CMake/QtDCMSettings.cmake)

find_package( Qt4 REQUIRED COMPONENTS QtCore QtGui QtNetwork)
if(QT4_FOUND)
  set(QT_USE_QTNETWORK true)
  include(${QT_USE_FILE})
endif(QT4_FOUND)

find_package(DCMTK2 REQUIRED)
include_directories(${DCMTK_INCLUDE_DIR})

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif(ITK_FOUND)
# find_package(GDCM REQUIRED)
# if(GDCM_FOUND)
#   include(${GDCM_USE_FILE})
# endif(GDCM_FOUND)

add_definitions(-DHAVE_CONFIG_H -fPIC)

set(QTDCM_RCCS
    ${QTDCM_SOURCE_DIR}/Resources/qtdcm.qrc
)

set(SAMPLE_SRCS
  ${QTDCM_SOURCE_DIR}/Code/QtDcm.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcm2.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImportWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreviewWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmManager.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmFindScu.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmFindDicomdir.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmMoveScu.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmMoveDicomdir.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmConvert.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImage.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmSerie.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmStudy.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPatient.cpp
#   ${QTDCM_SOURCE_DIR}/Code/QtDcmDateDialog.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferences.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesDialog.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServer.cpp
#   ${QTDCM_SOURCE_DIR}/Code/QtDcmExportThread.cpp
#   ${QTDCM_SOURCE_DIR}/Code/QtDcmQueryThread.cpp
#   ${QTDCM_SOURCE_DIR}/Code/QtDcmPreviewDialog.cpp
)

set(SAMPLE_HDRS
  ${QTDCM_SOURCE_DIR}/Code/QtDcm.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcm2.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImportWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreviewWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmManager.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmFindScu.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmFindDicomdir.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmMoveScu.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmMoveDicomdir.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmConvert.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImage.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmSerie.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmStudy.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPatient.h
#   ${QTDCM_SOURCE_DIR}/Code/QtDcmDateDialog.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferences.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesDialog.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServer.h
)

set(SAMPLE_UIS
  ${QTDCM_SOURCE_DIR}/Code/qtdcm.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmwidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmImportWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmPreviewWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmpreferencesdialog.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmpreferenceswidget.ui
)

set(SAMPLE_MOC_HDRS ${SAMPLE_HDRS})

set(QTDCM_SRCS
  ${QTDCM_SOURCE_DIR}/Code/QtDcmFindCallback.cpp
#     ${QTDCM_SOURCE_DIR}/Code/QtDcmConvert.cpp
)

qt4_add_resources(QTDCM_RCC_SRCS ${QTDCM_RCCS})
qt4_wrap_ui( SAMPLE_UI_HDRS ${SAMPLE_UIS})
qt4_wrap_cpp( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS})

#set(LANGUAGES fr_FR en_US)

#set(LANGUAGE_TS_FILES)
#foreach(LANGUAGE ${LANGUAGES})
#set(TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/qtdcm_${LANGUAGE}.ts")
#set(LANGUAGE_TS_FILES ${LANGUAGE_TS_FILES} ${TS_FILE})
#endforeach(LANGUAGE ${LANGUAGES})
#IF(LANGUAGE_TS_FILES)
#  qt4_create_translation(qm_files ${QTDCM_SRCS} ${SAMPLE_SRCS} ${SAMPLE_HDRS} ${SAMPLE_UI_HDRS} ${SAMPLE_MOC_SRCS} ${LANGUAGE_TS_FILES})
#ENDIF(LANGUAGE_TS_FILES)

message(STATUS ${DCMTK_LIBRARIES})
link_directories(${DCMTK_DIR}/lib/)

include_directories(${CMAKE_BINARY_DIR} ${QTDCM_SOURCE_DIR}/Code/ ${QTDCM_BINARY_DIR} ${ITK_INCLUDE_DIRS} ${VTK_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIR})
add_library( qtdcm ${QTDCM_SRCS} ${SAMPLE_SRCS} ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS} ${QTDCM_RCC_SRCS})# ${qm_files})
target_link_libraries( qtdcm ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${DCMTK_LIBRARIES} ITKIO dcmimage)# jpeg ijg12 ijg16 ijg8)# tiff /lib/libwrap.so.0)


# External inclusion stuff :
set(QTDCM_USE_FILE ${QTDCM_BINARY_DIR}/UseQtDCM.cmake)
set(QTDCM_LIBRARY_DEPENDS_FILE ${QTDCM_BINARY_DIR}/QtDCMLibraryDepends.cmake)
set(QTDCM_BUILD_SETTINGS_FILE ${QTDCM_BINARY_DIR}/QtDCMBuildSettings.cmake)
# Copy the UseQtDCM.cmake file to the binary tree for backward compatability.
configure_file(${QTDCM_SOURCE_DIR}/CMake/UseQtDCM.cmake.in
              ${QTDCM_BINARY_DIR}/UseQtDCM.cmake)
configure_file(${QTDCM_SOURCE_DIR}/CMake/QtDCMConfig.cmake.in
              ${QTDCM_BINARY_DIR}/QtDCMConfig.cmake)

# Save the compiler settings so another project can import them.
include(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
cmake_export_build_settings(${QTDCM_BINARY_DIR}/QtDCMBuildSettings.cmake)
export_library_dependencies(${QTDCM_BINARY_DIR}/QtDCMLibraryDepends.cmake)

# Create the QtDCMConfig.cmake file containing the QtDCM configuration used with make install and CPack.
include (${QTDCM_SOURCE_DIR}/CMake/QtDCMGenerateInstall.cmake)
if ( NOT BUILD_PACKAGE OR BUILD_PACKAGE_BINARY OR BUILD_PACKAGE_SDK)
  install_targets(${INSTALL_LIB} qtdcm)
endif ( NOT BUILD_PACKAGE OR BUILD_PACKAGE_BINARY OR BUILD_PACKAGE_SDK)


