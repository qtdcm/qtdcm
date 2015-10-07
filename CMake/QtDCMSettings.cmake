# SETTING installation directories
set(INSTALL_CMAKE /lib/QtDCM-${QTDCM_VERSION_MAJOR}.${QTDCM_VERSION_MINOR}/)
set(INSTALL_BIN /bin/)
set(INSTALL_LIB /lib/)
set(INSTALL_INC /include/qtdcm)
set(INSTALL_DOC /doc/)
set(INSTALL_MAN /man/)
set(INSTALL_SHARE /share/)

## SETTING THE INCLUDE DIRS
set(QTDCM_INCLUDE_DIRS
## Modules include dirs
  ${PROJECT_SOURCE_DIR}/Code
)

## SETTING QTDCM LIBRARY DIRS
set(QTDCM_LIBRARY_DIRS
  ${LIBRARY_OUTPUT_PATH}
)

include_directories(${QTDCM_INCLUDE_DIRS})

## SETTING QTDCM LIBS
set(QTDCM_LIBS
  qtdcm
)
