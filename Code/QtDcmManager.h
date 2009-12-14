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
#include <QtDcmPreferences.h>

/*
 *
 */
class QtDcmManager : QObject
  {
  Q_OBJECT
  private:
    QWidget * _parent;               /** Here the parent is corresponding to the QtDCM object */
    QProgressDialog * _progress;     /** Dialog window showing file copy in progress */
    QString _dicomdir;               /** Dicomdir absolute file path */
    QString _outputDir;              /** Output directory for reconstructed serie absolute path */
    QString _randDirName;            /** Randomly generated name where the slices of a serie are temporarily copied */
    QDir _tempRandDir;               /** Directory containing current serie dicom slice */
    QDir _tempDir;                   /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
    QDir _logsDir;                   /** Directory of the reconstruction process logs file (/tmp/qtdcm/logs) */
    DcmItem * _dcmObject;            /** This attribute is usefull for parsing the dicomdir */
    DcmFileFormat _dfile;            /** This attribute is usefull for parsing the dicomdir */
    QList<QtDcmPatient *> _patients; /** List that contains patients resulting of a query or read from a CD */
    QProcess * _process;             /** This attribute launch the reconstruction process */
    QString _dcm2niiPath;            /** Absolute path where to find dcm2nii */
    QString _dcm4chePath;            /** Absolute path where to find dcm4chee */
    QtDcmPreferences * _preferences; /** Attribute that give access to the Pacs settings */

    void
    generateRandomDir();
    void
    deleteRandomDir();
    void
    createTemporaryDirs();
    void
    deleteTemporaryDirs();

  public:
    QtDcmManager();
    QtDcmManager( QWidget * parent );
    virtual
    ~QtDcmManager();

    void
    loadDicomdir();

    void
    displayErrorMessage( QString message );

    QString
    getDicomdir() const
      {
        return _dicomdir;
      }

    void
    setDicomdir( QString _dicomdir )
      {
        this->_dicomdir = _dicomdir;
      }

    QString
    getDcm2niiPath() const
      {
        return _dcm2niiPath;
      }

    void
    setDcm2niiPath( QString path )
      {
        this->_dcm2niiPath = path;
      }

    QString
    getDcm4chePath() const
      {
        return _dcm4chePath;
      }

    void
    setDcm4chePath( QString path )
      {
        this->_dcm4chePath = path;
      }

    QString
    getOutputDirectory() const
      {
        return _outputDir;
      }

    void
    setOutputDirectory( QString directory )
      {
        this->_outputDir = directory;
      }

    QtDcmPreferences *
    getPreferences()
      {
        return _preferences;
      }

    void
    setPreferences( QtDcmPreferences * prefs )
      {
        _preferences = prefs;
      }

    QList<QtDcmPatient *>
    getPatients()
      {
        return _patients;
      }

    void
    exportSerie( QList<QString> images );

  };

#endif /* QTDCMMANAGER_H_ */
