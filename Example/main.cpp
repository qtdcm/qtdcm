/*!
 * \file main.cpp
 * \brief Main for the example application
 * \author Alexandre Abadie
 * \version 2.0
 */

#include <QtGui>
#include "dicom.h"

int
main( int argc , char *argv[] )
  {
    QApplication app(argc, argv);

    Dicom dcm;
    dcm.setWindowIcon(QIcon(":Images/dicom.xpm"));
    dcm.show();
    return app.exec();
  }
