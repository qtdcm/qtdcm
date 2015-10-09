# - Find an QtM3d installation or build tree.

# When QtM3d is found, the QtM3dConfig.cmake file is sourced to setup the
# location and configuration of QtM3d.  Please read this file, or
# VistalConfig.cmake.in from the QtM3d source tree for the full list of
# definitions.  Of particular interest is QtM3d_USE_FILE, a CMake source file
# that can be included to set the include directories, library directories,
# and preprocessor macros.  In addition to the variables read from
# QtM3dConfig.cmake, this find module also defines
#  QTDCM_DIR  - The directory containing VistalConfig.cmake.  
#             This is either the root of the build tree
#
#  QtM3d_FOUND - Whether Vistal was found.  If this is true, 
#              QtM3d_DIR is okay.
#
#  USE_QTDCM_FILE - The full path to the UseVistal.cmake file.  
#                 This is provided for backward 
#                 compatability.  Use QtM3d_USE_FILE
#                 instead.

find_path(QTDCM_DIR UseQtDCM.cmake 
  /usr/lib/qtdcm-0.1 
  /usr/local/lib/qtdcm-0.1
  "C:/Program Files/QtDcm/share/qtdcm"
  "C:/Program Files (x86)/QtDcm/share/qtdcm"
  DOC "QTDCM directory"
)

set(QTDCM_CONFIG_FILE "${QTDCM_DIR}/QtDCMConfig.cmake")
# path is okay
if(EXISTS ${QTDCM_CONFIG_FILE})
  set(QTDCM_FOUND 1)
  include(${QTDCM_CONFIG_FILE})
else()
  set(QTDCM_FOUND 0)
  if(QTDCM_FIND_REQUIRED)
    message(FATAL_ERROR "Please set QTDCM_DIR to the correct value")
  endif()
endif()
