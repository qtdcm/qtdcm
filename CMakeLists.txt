cmake_minimum_required(VERSION 2.6)

project(QTDCM)

set(QTDCM_VERSION_MAJOR "0")
set(QTDCM_VERSION_MINOR "1")
set(QTDCM_VERSION_PATCH "0")

add_definitions(-g -Wno-deprecated)

option(BUILD_SHARED_LIBS "Build QtDcm with shared libraries." OFF)
option(BUILD_DOCUMENTATION "Build QtDcm Documentation" OFF)
option(BUILD_PACKAGE "Configure QtDcm packaging" OFF)

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

find_package( Qt4 REQUIRED )
if(QT4_FOUND)
  include(${QT_USE_FILE})
endif(QT4_FOUND)

find_package(DCMTK REQUIRED)
#if(DCMTK_FOUND)
#    include(${DCMTK_USE_FILE})
#endif(DCMTK_FOUND)

find_path(DCM2NII_DIR dcm2nii
    /home/aabadie/Softs/builds/mricron
    REQUIRED
)

find_path(DCM4CHE_DIR dcmqr
    /usr/bin
    REQUIRED
)

set(SAMPLE_SRCS
  ${QTDCM_SOURCE_DIR}/Code/QtDcm.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmManager.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImage.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmSerie.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmStudy.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPatient.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmDateDialog.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferences.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesDialog.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServer.cpp
)

set(SAMPLE_HDRS
  ${QTDCM_SOURCE_DIR}/Code/QtDcm.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmManager.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImage.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmSerie.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmStudy.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPatient.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmDateDialog.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferences.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesDialog.h
    ${QTDCM_SOURCE_DIR}/Code/QtDcmServer.h
)

set(SAMPLE_UIS
  ${QTDCM_SOURCE_DIR}/Code/qtdcm.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmdatedialog.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmpreferencesdialog.ui
)

set(SAMPLE_MOC_HDRS ${SAMPLE_HDRS})

set(QTDCM_SRCS
)

qt4_wrap_ui( SAMPLE_UI_HDRS ${SAMPLE_UIS})
qt4_wrap_cpp( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS})

configure_file(${QTDCM_SOURCE_DIR}/Code/QtDcmConfig.h.in ${QTDCM_BINARY_DIR}/QtDcmConfig.h)

include_directories(${CMAKE_BINARY_DIR} ${QTDCM_SOURCE_DIR}/Code/ ${QTDCM_BINARY_DIR} ${ITK_INCLUDE_DIRS} ${VTK_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIR})
add_library( qtdcm ${QTDCM_SRCS} ${SAMPLE_SRCS} ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS})
target_link_libraries( qtdcm ${DCMTK_LIBRARIES} ${QT_LIBRARIES})


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


