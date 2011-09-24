/*!
 * \file main.cpp
 * \brief Main for the example QtM3d application
 * \author Alexandre Abadie
 * \version 0.1
 */

#include <QtGui>
#include "dicom.h"
// #include "DicomConfig.h"

//#define INCLUDE_CSTDLIB
//#define INCLUDE_CSTDIO
//#define INCLUDE_CSTRING
//#define INCLUDE_CSTDARG
//// From Dcmtk:
//#include <dcmtk/config/osconfig.h>
//#include "dcmtk/ofstd/ofstdinc.h"
//
//#include "dcmtk/dcmnet/dimse.h"
//#include "dcmtk/dcmnet/diutil.h"
//#include "dcmtk/dcmdata/dcdict.h"
//#include "dcmtk/dcmdata/dcuid.h"
//#include "dcmtk/ofstd/ofconapp.h"
//#include "dcmtk/dcmdata/dcuid.h"

int
main( int argc , char *argv[] )
  {
    QApplication app(argc, argv);

//     QString lang(argv[1]);
//     if ((lang != "fr_FR") && (lang != "en_US"))
//       lang = "fr_FR";
// 
//     QTranslator translator;
// //    translator.load("dicom_" + lang, QApplication::applicationDirPath());
//     translator.load("qtdcm_" + lang, qtdcm_dir);
//     app.installTranslator(&translator);

    Dicom dcm;
//     dcm.setWindowIcon(QIcon(icon_dir + QDir::separator() + "dicom.xpm"));
    dcm.setWindowIcon(QIcon(":Images/dicom.xpm"));
    dcm.show();
    return app.exec();
  }
