# - Find an QTDCM installation or build tree.

# When QTDCM is found, the QTDCMConfig.cmake file is sourced to setup the
# location and configuration of QTDCM.  Please read this file, or
# QtDCMConfig.cmake.in from the QTDCM source tree for the full list of
# definitions. In addition to the variables read from
# QTDCMConfig.cmake, this find module also defines
#  QTDCM_DIR  - The directory containing QtDCMConfig.cmake.  
#             This is either the root of the build tree
#
#  QTDCM_FOUND - Whether QtDcm was found.  If this is true, 
#              QTDCM_DIR is okay.

find_path(QTDCM_DIR QtDCMConfig.cmake 
  /usr/lib/cmake/qtdcm-2.0
  /usr/local/lib/cmake/qtdcm-2.0
  "C:/Program Files/QtDcm/lib/cmake/qtdcm-2.0"
  "C:/Program Files (x86)/QtDcm/lib/cmake/qtdcm-2.0"
  DOC "QTDCM directory"
)

set(QTDCM_CONFIG_FILE "${QTDCM_DIR}/QtDCMConfig.cmake")
# path is okay
if(EXISTS ${QTDCM_CONFIG_FILE})
  set(QTDCM_FOUND 1)
  include(${QTDCM_CONFIG_FILE})
else()
  set(QTDCM_FOUND 0)
  message(FATAL_ERROR "Please set QTDCM_DIR to the correct value")
endif()
