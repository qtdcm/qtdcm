include (${${PROJECT_NAME}_SOURCE_DIR}/CMake/QtDCMSettings.cmake)

find_package( Qt5 REQUIRED COMPONENTS Core Widgets Network)

find_package(DCMTK REQUIRED ${DCMTK_FIND_PACKAGE_STATEGY})
include_directories(${DCMTK_INCLUDE_DIRS})

if(APPLE AND NOT DCMTK_FIND_PACKAGE_USE_CONFIG_ONLY)
  include(FixDCMTKMacInstall)
  FixDCMTKMacInstall()
endif()

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif(ITK_FOUND)

set(${PROJECT_NAME}_RCCS
    ${${PROJECT_NAME}_SOURCE_DIR}/Resources/qtdcm.qrc
)

set(${PROJECT_NAME}_HDRS
  QtDcmImage.h
  QtDcmSerie.h
  QtDcmStudy.h
  QtDcmPatient.h
  QtDcmServer.h
)

set(${PROJECT_NAME}_SRCS
  QtDcmFindCallback.cpp
)

set(${PROJECT_NAME}_UIS
  qtdcm.ui
  qtdcmImportWidget.ui
  qtdcmPreviewWidget.ui
  qtdcmSerieInfoWidget.ui
  qtdcmLocalDicomSettingsWidget.ui
  qtdcmServersDicomSettingsWidget.ui
  qtdcmDcm2niiSettingsWidget.ui
  qtdcmpreferencesdialog.ui
  qtdcmpreferenceswidget.ui
)

set(${PROJECT_NAME}_MOC_HDRS
  QtDcm.h
  QtDcmImportWidget.h
  QtDcmPreviewWidget.h
  QtDcmSerieInfoWidget.h
  QtDcmLocalDicomSettingsWidget.h
  QtDcmServersDicomSettingsWidget.h
  QtDcmDcm2niiSettingsWidget.h
  QtDcmManager.h
  QtDcmFindScu.h
  QtDcmFindDicomdir.h
  QtDcmMoveScu.h
  QtDcmMoveDicomdir.h
  QtDcmConvert.h
  QtDcmPreferences.h
  QtDcmPreferencesWidget.h
  QtDcmPreferencesDialog.h
)

set(${PROJECT_NAME}_MOC_SRCS
  QtDcm.cpp
  QtDcmImportWidget.cpp
  QtDcmPreviewWidget.cpp
  QtDcmSerieInfoWidget.cpp
  QtDcmLocalDicomSettingsWidget.cpp
  QtDcmServersDicomSettingsWidget.cpp
  QtDcmDcm2niiSettingsWidget.cpp
  QtDcmManager.cpp
  QtDcmFindScu.cpp
  QtDcmFindDicomdir.cpp
  QtDcmMoveScu.cpp
  QtDcmMoveDicomdir.cpp
  QtDcmConvert.cpp
  QtDcmImage.cpp
  QtDcmSerie.cpp
  QtDcmStudy.cpp
  QtDcmPatient.cpp
  QtDcmPreferences.cpp
  QtDcmPreferencesWidget.cpp
  QtDcmPreferencesDialog.cpp
  QtDcmServer.cpp
)

qt5_add_resources(${PROJECT_NAME}_RCC_SRCS ${${PROJECT_NAME}_RCCS})
qt5_wrap_ui( ${PROJECT_NAME}_UI_HDRS ${${PROJECT_NAME}_UIS})
qt5_wrap_cpp( ${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_HDRS})

include_directories(
  ${CMAKE_BINARY_DIR}
  ${${PROJECT_NAME}_SOURCE_DIR}/Code
  ${${PROJECT_NAME}_BINARY_DIR}/Code
  ${ITK_INCLUDE_DIRS}
  ${VTK_INCLUDE_DIRS}
  ${DCMTK_INCLUDE_DIR}
)

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

if(APPLE AND NOT DCMTK_FIND_PACKAGE_USE_CONFIG_ONLY)
  include(FixDCMTKMacLink)
  FixDCMTKMacLibLink(qtdcm)
endif()

# External inclusion stuff :
set(${PROJECT_NAME}_USE_FILE ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)

configure_file(${${PROJECT_NAME}_SOURCE_DIR}/CMake/UseQtDCM.cmake.in
              ${${PROJECT_NAME}_BINARY_DIR}/UseQtDCM.cmake)
configure_file(${${PROJECT_NAME}_SOURCE_DIR}/CMake/QtDCMConfig.cmake.in
              ${${PROJECT_NAME}_BINARY_DIR}/QtDCMConfig.cmake)

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
if (WIN32)
  install(TARGETS qtdcm RUNTIME DESTINATION bin
                        LIBRARY DESTINATION lib
                        ARCHIVE DESTINATION lib
  )

  # Also copy dependencies when generating an auto-installer package
  if(BUILD_PACKAGE)
    include(GetPrerequisites)
    get_prerequisites(${LIBRARY_OUTPUT_PATH}/qtdcm.dll WIN32_PREREQUISITES 1 0 "" "")
    foreach(DEPENDENCY_FILE ${WIN32_PREREQUISITES})
      gp_resolve_item("${LIBRARY_OUTPUT_PATH}/qtdcm.dll" "${DEPENDENCY_FILE}" "" "" resolved_file)
      install(FILES ${resolved_file} DESTINATION bin)
    endforeach()
  endif()
else ()
  install(TARGETS qtdcm LIBRARY DESTINATION lib)
endif ()
