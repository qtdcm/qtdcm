#CPack config file
if(BUILD_PACKAGE)
  message(STATUS "Configuring CPack")

  set(BUILD_TESTING OFF)
  include(InstallRequiredSystemLibraries)
  set(CPACK_GENERATOR "")
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_PACKAGE_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")
  set(CPACK_SOURCE_GENERATOR "TBZ2;ZIP")
  set(CPACK_PACKAGE_CONTACT "alexandre.abadie@inria.fr")
  set(CPACK_PACKAGE_DESCRIPTION_FILE "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CPack.QtDCMLicense.txt")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QtDCM provides Qt widgets for reading DICOM CD and communicating with DICOM PACS")
  set(CPACK_RESOURCE_FILE_LICENSE "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CPack.QtDCMLicense.txt")
  set(CPACK_RESOURCE_FILE_README "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CPack.QtDCMDescription.txt")
  set(CPACK_RESOURCE_FILE_WELCOME "${${PROJECT_NAME}_SOURCE_DIR}/cmake/CPack.QtDCMWelcome.txt")
  set(CPACK_PACKAGE_FILE_NAME "qtdcm-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "qtdcm-${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}-src")
  
  if(NOT WIN32)
  # set(CMAKE_INSTALL_PREFIX "/usr")
    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    if(BUILD_PACKAGE_DEBIAN)
      include(CPackDebConfig)
    endif()

    if(BUILD_PACKAGE_RPM)
      include(CPackRpmConfig)
    endif()

  else()

    if (APPLE)
      set(CPACK_GENERATOR PACKAGEMAKER)
    else()
      include(CPackNsisConfig)
    endif()

  endif()

  include(CPack)

endif()
