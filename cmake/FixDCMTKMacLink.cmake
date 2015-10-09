MACRO(FixDCMTKMacLibLink lib_name)

  foreach(lib dcmdata dcmimage dcmimgle dcmjpeg dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)
    if (EXISTS ${DCMTK_DIR}/lib/lib${lib}.dylib)
      add_custom_command(TARGET ${lib_name}
	    POST_BUILD
	    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${lib}.dylib ${DCMTK_DIR}/lib/lib${lib}.dylib lib/lib${lib_name}.dylib
          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      )
    endif()
  endforeach()

ENDMACRO(FixDCMTKMacLibLink)

MACRO(FixDCMTKMacExeLink exe_name)

  foreach(lib dcmdata dcmimage dcmimgle dcmjpeg dcmnet dcmpstat dcmqrdb dcmsr dcmtls ijg12 ijg16 ijg8 oflog ofstd)
    if (EXISTS ${DCMTK_DIR}/lib/lib${lib}.dylib)
      add_custom_command(TARGET ${exe_name}
	    POST_BUILD
	    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -change lib${lib}.dylib ${DCMTK_DIR}/lib/lib${lib}.dylib ${exe_name}
          WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
      )
    endif()
  endforeach()

ENDMACRO(FixDCMTKMacExeLink)
