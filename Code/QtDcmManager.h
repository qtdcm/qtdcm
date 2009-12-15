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

/**
 * This class is in charge of the different process (dcm2nii), pacs query/retrieve (dcm4chee),
 * temporary directory creation and removing, PACS server settings.
 *
 */
class QtDcmManager : QObject
  {
  Q_OBJECT
  private:
    QWidget * _parent; /** Here the parent is corresponding to the QtDCM object */
    QProgressDialog * _progress; /** Dialog window showing file copy in progress */
    QString _dicomdir; /** Dicomdir absolute file path */
    QString _outputDir; /** Output directory for reconstructed serie absolute path */
    QString _randDirName; /** Randomly generated name where the slices of a serie are temporarily copied */
    QDir _tempRandDir; /** Directory containing current serie dicom slice */
    QDir _tempDir; /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
    QDir _logsDir; /** Directory of the reconstruction process logs file (/tmp/qtdcm/logs) */
    DcmItem * _dcmObject; /** This attribute is usefull for parsing the dicomdir */
    DcmFileFormat _dfile; /** This attribute is usefull for parsing the dicomdir */
    QList<QtDcmPatient *> _patients; /** List that contains patients resulting of a query or read from a CD */
    QProcess * _process; /** This attribute launch the reconstruction process */
    QString _dcm2niiPath; /** Absolute path where to find dcm2nii */
    QString _dcm4chePath; /** Absolute path where to find dcm4chee */
    QtDcmPreferences * _preferences; /** Attribute that give access to the Pacs settings */

    /**
     * Generate random directory name and create it
     */
    void
    generateRandomDir();

    /**
     * Delete random directory after data has been reconstructed
     */
    void
    deleteRandomDir();

    /**
     * Create the temporary directory (/tmp/qtdcm on Unix) and the logging directory.
     * (/tmp/qtdcm/logs)
     */
    void
    createTemporaryDirs();

    /**
     * This method try to delete the temporary directory when closing the QtDcm widget
     * (Doesn't work for the moment)
     */
    void
    deleteTemporaryDirs();

  public:
    /**
     * Default constructor. Instantiate the internal pointers and create the temporary directory.
     */
    QtDcmManager();

    /**
     * Default constructor. Instantiate the internal pointers and create the temporary directory.
     * @param parent the parent widget (the qtdcm object)
     */
    QtDcmManager( QWidget * parent );

    /**
     * The default destructor
     */
    virtual
    ~QtDcmManager();

    /**
     * This method read the dicomdir file and populate the different lists (Patients, Studies, Series and Images)
     *
     * @see QtDcmPatient, QtDcmStudy, QtDcmSerie, QtDcmImage
     */
    void
    loadDicomdir();

    /**
     * Convenience method that display error message in a QMessageBox window.
     *
     * @param message the error message to display
     */
    void
    displayErrorMessage( QString message );

    /**
     * Return the dicomdir absolute path name
     *
     * @return _dicomdir the dicomdir file name.
     */
    QString
    getDicomdir() const
      {
        return _dicomdir;
      }

    /**
     * Set the dicomdir file name
     *
     * @param dicomdir the dicomdir file name
     */
    void
    setDicomdir( QString dicomdir )
      {
        this->_dicomdir = dicomdir;
      }

    /**
     * Return the path where is located the dcm2nii binary
     *
     * @return _dcm2niiPath the dcm2nii path
     */
    QString
    getDcm2niiPath() const
      {
        return _dcm2niiPath;
      }

    /**
     * Set the dcm2nii path
     *
     * @param path the dcm2nii path
     */
    void
    setDcm2niiPath( QString path )
      {
        this->_dcm2niiPath = path;
      }

    /**
     * Return the dcm4che binary path
     *
     * @return _dcm4chePath the dcm4chee path
     */
    QString
    getDcm4chePath() const
      {
        return _dcm4chePath;
      }

    /**
     * Sets the dcm4che path
     *
     * @param path the dcm4che path
     */
    void
    setDcm4chePath( QString path )
      {
        this->_dcm4chePath = path;
      }

    /**
     * Return the output directory where the current serie will be reconstructed
     *
     * @return _outputdir the output directory path
     */
    QString
    getOutputDirectory() const
      {
        return _outputDir;
      }

    /**
     * Set the output directory
     *
     * @param directory the output directory
     */
    void
    setOutputDirectory( QString directory )
      {
        this->_outputDir = directory;
      }

    /**
     * Return the pointer to the QtDcmPreferences instance
     *
     * @return _preferences
     * @see QtDcmPreferences
     */
    QtDcmPreferences *
    getPreferences()
      {
        return _preferences;
      }

    /**
     * Set the pointer to the QtDcmPreferences instance
     *
     * @param prefs
     * @see QtDcmPreferences
     */
    void
    setPreferences( QtDcmPreferences * prefs )
      {
        _preferences = prefs;
      }

    /**
     * Return the current patient list
     *
     * @return QList<QtDcmPatient *> the list of patient loaded
     * @see QtDcmPatient
     */
    QList<QtDcmPatient *>
    getPatients()
      {
        return _patients;
      }

    /**
     * Call dcm2nii in a QProcess object to reconstruct the given list of images
     *
     * @param images
     */
    void
    exportSerie( QList<QString> images );

  };

#endif /* QTDCMMANAGER_H_ */
