/*
 * QtDcmManager.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMMANAGER_H_
#define QTDCMMANAGER_H_

#include <QtGui>

#include <algorithm>
#include <iostream>

#define HAVE_CLASS_TEMPLATE
#define HAVE_STL
#define HAVE_STD_NAMESPACE
#define HAVE_SSTREAM
#define USE_STD_CXX_INCLUDES
#define HAVE_CXX_BOOL

#define DCM_DICT_DEFAULT_PATH

// From Dcmtk:
#include <dcmtk/config/osconfig.h>    /* make sure OS specific configuration is included first */
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcdebug.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/dcmdata/dcuid.h>       /* for dcmtk version name */
#include <dcmtk/dcmdata/dcistrmz.h>    /* for dcmZlibExpectRFC1950Encoding */
// For dcm images
#include <dcmtk/dcmimgle/dcmimage.h>
// For color images
//#include <dcmtk/dcmimage/diregist.h>

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef WITH_ZLIB
#include <zlib.h>        /* for zlibVersion() */
#endif

#include <QtDcmPatient.h>
/*
 *
 */
class QtDcmManager : QObject
{
Q_OBJECT
private:
  QString _dicomdir;
  DcmItem * _dcmObject;
  DcmFileFormat _dfile;
  QList<QtDcmPatient *> _patients;

public:
  QtDcmManager();
  virtual
  ~QtDcmManager();

  void
  loadDicomdir();

  QString
  getDicomdir() const
  {
    return _dicomdir;
  }

  void
  setDicomdir(QString _dicomdir)
  {
    this->_dicomdir = _dicomdir;
  }

  QList<QtDcmPatient *> getPatients() {return _patients;}

};

#endif /* QTDCMMANAGER_H_ */
