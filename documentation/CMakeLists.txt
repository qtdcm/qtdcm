#
# Build the documentation
#
find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  message(FATAL_ERROR "Doxygen is needed to build the documentation.")
endif()

set(doxyfile_in    ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in)
set(doxyfile       ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)
set(qtdcm_src_css  ${CMAKE_CURRENT_SOURCE_DIR}/qtdcm_dox.css)
set(qtdcm_gen_css  ${CMAKE_CURRENT_BINARY_DIR}/html/qtdcm_dox.css)

configure_file(${doxyfile_in} ${doxyfile} @ONLY)
configure_file(${qtdcm_src_css} ${qtdcm_gen_css} @ONLY)

add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
