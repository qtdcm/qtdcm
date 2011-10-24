#
# Build the documentation
#
include(${CMAKE_ROOT}/Modules/Documentation.cmake)

if(BUILD_DOCUMENTATION)
#
# Configure the script and the doxyfile, then add target
#
  if(NOT DOT_PATH)
    get_filename_component(DOT_PATH ${DOT} PATH)
  endif(NOT DOT_PATH)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/doc_makeall.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/qtdcm_dox.css
    ${CMAKE_CURRENT_BINARY_DIR}/html/qtdcm_dox.css)

  add_custom_target(Documentation
    ${BASH}
    ${CMAKE_CURRENT_BINARY_DIR}/doc_makeall.sh
    DEPENDS ${DOT_PNG_DEPS}) 

endif(BUILD_DOCUMENTATION)
