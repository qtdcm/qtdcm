cmake_minimum_required(VERSION 3.0.2)

project(qtdcm)

find_package( Qt5 REQUIRED COMPONENTS Core Widgets Network)

get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake LOCATION)
get_filename_component(QT5_BINARY_DIR ${QT5_QMAKE_EXECUTABLE} PATH)

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
    ../resources/qtdcm.qrc
)

set(${PROJECT_NAME}_HDRS
  qtdcmExports.h
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

set(${PROJECT_NAME}_HEADERS
  ${${PROJECT_NAME}_HDRS}
  ${${PROJECT_NAME}_MOC_HDRS}
  ${${PROJECT_NAME}_UI_HDRS}
)

set(${PROJECT_NAME}_SOURCES
  ${${PROJECT_NAME}_SRCS}
  ${${PROJECT_NAME}_MOC_SRCS}
  ${${PROJECT_NAME}_RCC_SRCS}
)

set(${PROJECT_NAME}_LIBRARIES
  Qt5::Core
  Qt5::Widgets
  Qt5::Network
  ${DCMTK_LIBRARIES}
  ITKIOImageBase
  ITKCommon
  ITKIOBMP
  ITKIOBioRad
  ITKIOBruker
  ITKIOHDF5
  ITKIOGDCM
  ITKIOGE
  ITKIOGIPL
  ITKIOJPEG
  ITKIOJPEG2000
  ITKIOLSM
  ITKIOMeta
  ITKIOMINC
  ITKIONIFTI
  ITKIONRRD
  ITKIOPNG
  ITKIOStimulate
  ITKIOTIFF
  ITKIOVTK
  ITKIOMRC
  ${ITKIOPhilipsREC_LIBRARIES}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${${PROJECT_NAME}_SOURCE_DIR}
  ${${PROJECT_NAME}_BINARY_DIR}
  ${ITK_INCLUDE_DIRS}
  ${DCMTK_INCLUDE_DIR}
)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})
target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARIES})

# Include header files for Visual Studio.
source_group("Header Files" REGULAR_EXPRESSION .*\\.h\$)
source_group("Generated Files" FILES ${${PROJECT_NAME}_MOC_SRCS})

if(APPLE AND NOT DCMTK_FIND_PACKAGE_USE_CONFIG_ONLY)
  include(FixDCMTKMacLink)
  FixDCMTKMacLibLink(${PROJECT_NAME})
endif()

install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include)
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT ${PROJECT_NAME}
        EXPORT QTDCMDepends)
if (WIN32)
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin
                                  LIBRARY DESTINATION lib
                                  ARCHIVE DESTINATION lib)
  # Also copy dependencies when generating an auto-installer package
  if(BUILD_PACKAGE)
    include(GetPrerequisites)
    get_prerequisites(${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll WIN32_PREREQUISITES 1 0 "" "")
    foreach(DEPENDENCY_FILE ${WIN32_PREREQUISITES})
      gp_resolve_item("${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll" "${DEPENDENCY_FILE}" "" "" resolved_file)
      install(FILES ${resolved_file} DESTINATION bin)
    endforeach()
  endif()
else ()
  install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)
endif ()
