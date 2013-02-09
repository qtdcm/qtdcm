include (${QTDCM_SOURCE_DIR}/CMake/QtDCMSettings.cmake)

find_package( Qt4 REQUIRED COMPONENTS QtCore QtGui QtNetwork)
if(QT4_FOUND)
  set(QT_USE_QTNETWORK true)
  include(${QT_USE_FILE})
endif(QT4_FOUND)

find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif(ITK_FOUND)

set(QTDCM_RCCS
    ${QTDCM_SOURCE_DIR}/Resources/qtdcm.qrc
)

set(SAMPLE_SRCS
  ${QTDCM_SOURCE_DIR}/Code/QtDcm.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImportWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreviewWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmSerieInfoWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmLocalDicomSettingsWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServersDicomSettingsWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmDcm2niiSettingsWidget.cpp
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
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferences.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesWidget.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesDialog.cpp
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServer.cpp
)

set(SAMPLE_HDRS
  ${QTDCM_SOURCE_DIR}/Code/QtDcm.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmImportWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreviewWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmSerieInfoWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmLocalDicomSettingsWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServersDicomSettingsWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmDcm2niiSettingsWidget.h
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
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferences.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesWidget.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmPreferencesDialog.h
  ${QTDCM_SOURCE_DIR}/Code/QtDcmServer.h
)

set(SAMPLE_UIS
  ${QTDCM_SOURCE_DIR}/Code/qtdcm.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmImportWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmPreviewWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmSerieInfoWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmLocalDicomSettingsWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmServersDicomSettingsWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmDcm2niiSettingsWidget.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmpreferencesdialog.ui
  ${QTDCM_SOURCE_DIR}/Code/qtdcmpreferenceswidget.ui
)

set(SAMPLE_MOC_HDRS ${SAMPLE_HDRS})

set(QTDCM_SRCS
  ${QTDCM_SOURCE_DIR}/Code/QtDcmFindCallback.cpp
)

qt4_add_resources(QTDCM_RCC_SRCS ${QTDCM_RCCS})
qt4_wrap_ui( SAMPLE_UI_HDRS ${SAMPLE_UIS})
qt4_wrap_cpp( SAMPLE_MOC_SRCS ${SAMPLE_MOC_HDRS})

include_directories(${CMAKE_BINARY_DIR} ${QTDCM_SOURCE_DIR}/Code/ ${QTDCM_BINARY_DIR}/Code ${ITK_INCLUDE_DIRS} ${VTK_INCLUDE_DIRS} ${DCMTK_INCLUDE_DIR})

add_library( qtdcm ${QTDCM_SRCS} ${SAMPLE_SRCS} ${SAMPLE_MOC_SRCS} ${SAMPLE_UI_HDRS} ${QTDCM_RCC_SRCS})
target_link_libraries( qtdcm ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${DCMTK_LIBRARIES} ${ITK_LIBRARIES})


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
# include (${QTDCM_SOURCE_DIR}/CMake/QtDCMGenerateInstall.cmake)
# if ( NOT BUILD_PACKAGE OR BUILD_PACKAGE_BINARY OR BUILD_PACKAGE_SDK)
#   install_targets(${INSTALL_LIB} qtdcm)
# endif ( NOT BUILD_PACKAGE OR BUILD_PACKAGE_BINARY OR BUILD_PACKAGE_SDK)

set(${PROJECT_NAME}_HEADERS
  ${SAMPLE_HDRS}
  ${SAMPLE_UI_HDRS}
)

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/qtdcm)
install(TARGETS qtdcm LIBRARY DESTINATION lib)
