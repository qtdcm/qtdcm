message("${Esc}[32mConfiguring DCMTK external library${Esc}[m")

if("${DCMTK_DIR}" STREQUAL "")
  set(DCMTK_DIR ${PROJECT_BINARY_DIR}/externals/dcmtk-build)
endif()
if("${DCMTK_SOURCE_DIR}" STREQUAL "")
  set(DCMTK_SOURCE_DIR ${PROJECT_BINARY_DIR}/externals/dcmtk)
endif()
set(DCMTK_SOURCE_DIR "${DCMTK_SOURCE_DIR}" CACHE PATH "" FORCE)
set(DCMTK_DIR "${DCMTK_DIR}" CACHE PATH "" FORCE)

ExternalProject_Add(
  dcmtk 
 
  GIT_REPOSITORY git://git.dcmtk.org/dcmtk.git
  GIT_TAG DCMTK-3.6.2

  CMAKE_GENERATOR Ninja
  
  SOURCE_DIR "${DCMTK_SOURCE_DIR}"
  BINARY_DIR "${DCMTK_DIR}"
  CMAKE_ARGS -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_APPS:BOOL=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo -DDCMTK_WITH_ZLIB:BOOL=OFF 
  
  INSTALL_COMMAND ""
  TEST_COMMAND ""
)

message("${Esc}[32mConfiguring DCMTK external library -- ${Esc}[1;32mDone${Esc}[m")
message("")
