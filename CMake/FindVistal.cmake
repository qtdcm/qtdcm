# - Find an Vistal installation or build tree.

# When Vistal is found, the VistalConfig.cmake file is sourced to setup the
# location and configuration of Vistal.  Please read this file, or
# VistalConfig.cmake.in from the Vistal source tree for the full list of
# definitions.  Of particular interest is Vistal_USE_FILE, a CMake source file
# that can be included to set the include directories, library directories,
# and preprocessor macros.  In addition to the variables read from
# VistalConfig.cmake, this find module also defines
#  VISTAL_DIR  - The directory containing VistalConfig.cmake.  
#             This is either the root of the build tree
#   
#  VISTAL_FOUND - Whether Vistal was found.  If this is true, 
#              VISTAL_DIR is okay.
#
#  USE_VISTAL_FILE - The full path to the UseVistal.cmake file.  
#                 This is provided for backward 
#                 compatability.  Use VISTAL_USE_FILE
#                 instead.

FIND_PATH(VISTAL_DIR VistalConfig.cmake
  /usr/local/vistal 
  DOC "Vistal directory"
)

SET(VISTAL_CONFIG_FILE ${VISTAL_DIR}/VistalConfig.cmake)
# path is okay
IF (EXISTS ${VISTAL_CONFIG_FILE})

  SET(VISTAL_FOUND 1)
  INCLUDE (${VISTAL_CONFIG_FILE})

  IF (EXISTS ${VISTAL_USE_FILE})
    INCLUDE (${VISTAL_USE_FILE})
  ENDIF (EXISTS ${VISTAL_USE_FILE})

ELSE (EXISTS ${VISTAL_CONFIG_FILE})

  SET(VISTAL_FOUND 0)
  IF(VISTAL_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Please set VISTAL_DIR to the correct value")
  ENDIF(VISTAL_FIND_REQUIRED)

ENDIF (EXISTS ${VISTAL_CONFIG_FILE})
