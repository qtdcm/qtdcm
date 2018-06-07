message("${Esc}[32mConfiguring ITK external library${Esc}[m")

if("${ITK_DIR}" STREQUAL "")
  set(ITK_DIR ${PROJECT_BINARY_DIR}/externals/itk-build)
endif()
  
set(ITK_DIR "${ITK_DIR}" CACHE PATH "" FORCE)

ExternalProject_Add(
  itk
 
  URL https://sourceforge.net/projects/itk/files/itk/4.13/InsightToolkit-4.13.0.tar.gz

  CMAKE_GENERATOR Ninja
  
  BINARY_DIR "${ITK_DIR}"
  CMAKE_ARGS -DBUILD_SHARED_LIBS:BOOL=ON -DBUILD_EXAMPLES:BOOL=OFF -DBUILD_TESTING:BOOL=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo 
  
  INSTALL_COMMAND ""
  TEST_COMMAND ""
)

message("${Esc}[32mConfiguring ITK external library -- ${Esc}[1;32mDone${Esc}[m")
message("")
