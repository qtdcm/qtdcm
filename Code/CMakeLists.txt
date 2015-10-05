include (${${PROJECT_NAME}_SOURCE_DIR}/CMake/QtDCMSettings.cmake)

find_package( Qt5 REQUIRED COMPONENTS Core Widgets Network)

find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})
IF(APPLE)
  include(FixDCMTKMacInstall)
  FixDCMTKMacInstall()
ENDIF(APPLE)

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif(ITK_FOUND)

set(${PROJECT_NAME}_RCCS
    ${${PROJECT_NAME}_SOURCE_DIR}/Resources/qtdcm.qrc
)

set(${PROJECT_NAME}_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmImage.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmSerie.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmStudy.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPatient.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmServer.h
)

set(${PROJECT_NAME}_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmFindCallback.cpp
)


set(${PROJECT_NAME}_UIS
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcm.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmImportWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmPreviewWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmSerieInfoWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmLocalDicomSettingsWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmServersDicomSettingsWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmDcm2niiSettingsWidget.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmpreferencesdialog.ui
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/qtdcmpreferenceswidget.ui
)

set(${PROJECT_NAME}_MOC_HDRS   
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcm.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmImportWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreviewWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmSerieInfoWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmLocalDicomSettingsWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmServersDicomSettingsWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmDcm2niiSettingsWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmManager.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmFindScu.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmFindDicomdir.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmMoveScu.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmMoveDicomdir.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmConvert.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreferences.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreferencesWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreferencesDialog.h
)

set(${PROJECT_NAME}_MOC_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcm.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmImportWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreviewWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmSerieInfoWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmLocalDicomSettingsWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmServersDicomSettingsWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmDcm2niiSettingsWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmManager.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmFindScu.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmFindDicomdir.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmMoveScu.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmMoveDicomdir.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmConvert.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmImage.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmSerie.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmStudy.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPatient.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreferences.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreferencesWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmPreferencesDialog.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/Code/QtDcmServer.cpp
)

qt5_add_resources(${PROJECT_NAME}_RCC_SRCS ${${PROJECT_NAME}_RCCS})
qt5_wrap_ui( ${PROJECT_NAME}_UI_HDRS ${${PROJECT_NAME}_UIS})
qt5_wrap_cpp( ${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_HDRS})

include_directories(${CMAKE_BINARY_DIR} ${${PROJECT_NAME}_SOURCE_DIR}/Code ${${PROJECT_NAME}_BINARY_DIR}/Code ${ITK_INCLUDE_DIRS} ${VTK_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIR})

set(${PROJECT_NAME}_LIBRARIES
  Qt5::Core
  Qt5::Widgets
  Qt5::Network
  ${DCMTK_LIBRARIES}
  ITKCommon
  ITKIOBMP
  ITKIOBioRad
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGE
  ITKIOGIPL
  ITKIOJPEG
  ITKIOLSM
  ITKIOMeta
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOVTK
  ITKIOMRC
)

add_library(qtdcm SHARED ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_HDRS} ${${PROJECT_NAME}_MOC_SRCS} ${${PROJECT_NAME}_UI_HDRS} ${${PROJECT_NAME}_RCC_SRCS})
target_link_libraries(qtdcm
  ${${PROJECT_NAME}_LIBRARIES}
)

if(APPLE)
  include(FixDCMTKMacLink)
  FixDCMTKMacLibLink(qtdcm)
endif(APPLE)

# External inclusion stuff :
set(${PROJECT_NAME}_USE_FILE ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)
set(${PROJECT_NAME}_LIBRARY_DEPENDS_FILE ${${PROJECT_NAME}_BINARY_DIR}/QtDCMLibraryDepends.cmake)
set(${PROJECT_NAME}_BUILD_SETTINGS_FILE ${${PROJECT_NAME}_BINARY_DIR}/QtDCMBuildSettings.cmake)
# Copy the UseQtDCM.cmake file to the binary tree for backward compatability.
configure_file(${${PROJECT_NAME}_SOURCE_DIR}/CMake/UseQtDCM.cmake.in
              ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)
configure_file(${${PROJECT_NAME}_SOURCE_DIR}/CMake/QtDCMConfig.cmake.in
              ${${PROJECT_NAME}_BINARY_DIR}/QtDCMConfig.cmake)

# Save the compiler settings so another project can import them.
include(${CMAKE_ROOT}/Modules/CMakeExportBuildSettings.cmake)
cmake_export_build_settings(${${PROJECT_NAME}_BINARY_DIR}/QtDCMBuildSettings.cmake)
export_library_dependencies(${${PROJECT_NAME}_BINARY_DIR}/QtDCMLibraryDepends.cmake)

# Create the QtDCMConfig.cmake file containing the QtDCM configuration used with make install and CPack.
# include (${${PROJECT_NAME}_SOURCE_DIR}/CMake/QtDCMGenerateInstall.cmake)
# if ( NOT BUILD_PACKAGE OR BUILD_PACKAGE_BINARY OR BUILD_PACKAGE_SDK)
#   install_targets(${INSTALL_LIB} qtdcm)
# endif ( NOT BUILD_PACKAGE OR BUILD_PACKAGE_BINARY OR BUILD_PACKAGE_SDK)

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HDRS}
  ${${PROJECT_NAME}_MOC_HDRS}
  ${${PROJECT_NAME}_UI_HDRS}
)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/qtdcm)
if (WIN32 AND BUILD_SHARED_LIBS)
  install(TARGETS qtdcm RUNTIME DESTINATION lib)
else ()
  install(TARGETS qtdcm LIBRARY DESTINATION lib)
endif ()
