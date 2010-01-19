/*
 * QtDcmManager.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMMANAGER_H_
#define QTDCMMANAGER_H_

#include <QtGui>
#include <QtNetwork>

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
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
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
#include <QtDcmExportThread.h>
#include <QtDcmQueryThread.h>

class QtDcmQueryThread;

/**
 * This class is in charge of the different process (dcm2nii), pacs query/retrieve (dcm4chee),
 * temporary directory creation and removing, PACS server settings.
 *
 */
class QtDcmManager : public QObject
  {
  Q_OBJECT
  private:
    QWidget * _parent; /** Here the parent is corresponding to the QtDCM object */
    QProgressDialog * _progress; /** Dialog window showing file copy in progress */
    QString _dicomdir; /** Dicomdir absolute file path */
    QString _outputDir; /** Output directory for reconstructed serie absolute path */
    QDir _currentSerieDir; /** Directory containing current serie dicom slice */
    QDir _tempDir; /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
    QDir _logsDir; /** Directory of the reconstruction process logs file (/tmp/qtdcm/logs) */
    DcmItem * _dcmObject; /** This attribute is usefull for parsing the dicomdir */
    DcmFileFormat _dfile; /** This attribute is usefull for parsing the dicomdir */
    QList<QtDcmPatient *> _patients; /** List that contains patients resulting of a query or read from a CD */
    QList<QString> _images; /** List of image filename to export from a CD */
    QMap<QString, QList<QString> > _seriesToExport;
    QList<QPixmap> _listImages;
    QString _serieId; /** Id of the serie to export from the PACS */
    QProcess * _process; /** This attribute launch the reconstruction process */
    QtDcmPreferences * _preferences; /** Attribute that give access to the Pacs settings */
    QString _patientName; /** Attribute representing the patient name used for query PACS */
    QString _patientId; /** Attribute representing the patient id used for query PACS */
    QString _modality; /** Attibute for the modality of the search (MR, US, CT, etc) */
    QString _date1; /** Attribute for the begin date of the query (usefull for date based queries) */
    QString _date2; /** Attribute for the end date of the query (usefull for date based queries) */
    QString _serieDescription; /** Attibute representing the serie description used for query PACS */
    QString _studyDescription; /** Attibute representing the study description used for query PACS */
    QString _mode; /** Mode that determine the type of media (CD or PACS) */
    QString _dcm2nii; /** Absolute filename of the dcm2nii program */
    QString _dcm4che; /** Absolute filename of the dcm4che program */
    QtDcmExportThread * _exportThread;
    QtDcmQueryThread * _queryThread;
    QByteArray _query;

    void
    generateCurrentSerieDir();

    void
    deleteCurrentSerieDir();

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

    /**
     * This method parse and fill all the list with the result of C-FIND
     */
    void
    parseQueryResult( QString query );

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
     * Convenience method that display information message in a QMessageBox window.
     *
     * @param info the information message to display
     */
    void
    displayMessage( QString info );

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
     * Patient name getter
     *
     * @return _patientName as a QString
     */
    QString
    getPatientName()
      {
        return _patientName;
      }

    /**
     * Patient name setter
     *
     * @param patientName as a QString
     */
    void
    setPatientName( QString patientName )
      {
        _patientName = patientName;
      }

    /**
     * Patient id getter
     *
     * @return _patientId as a QString
     */
    QString
    getPatientId()
      {
        return _patientId;
      }

    /**
     * Patient id setter
     *
     * @param patientId as a QString
     */
    void
    setPatientId( QString patientId )
      {
        _patientId = patientId;
      }

    /**
     * Serie description getter
     *
     * @return _serieDescription as a QString
     */
    QString
    getSerieDescription()
      {
        return _serieDescription;
      }

    /**
     * Serie description setter
     *
     * @param serieDescription as a QString
     */
    void
    setSerieDescription( QString serieDescription )
      {
        _serieDescription = serieDescription;
      }

    /**
     * Study description getter
     *
     * @return _studyDescription as a QString
     */
    QString
    getStudyDescription()
      {
        return _studyDescription;
      }

    /**
     * Study description setter
     *
     * @param studyDescription as a QString
     */
    void
    setStudyDescription( QString studyDescription )
      {
        _studyDescription = studyDescription;
      }

    /**
     * Study modality setter
     *
     * @param modality as a QString
     */
    void
    setModality( QString modality )
      {
        _modality = modality;
      }

    /**
     * Study modality getter
     *
     * @return _modality as a QString
     */
    QString
    getModality()
      {
        return _modality;
      }

    /**
     * Study date setter
     *
     * @param date as a QString
     */
    void
    setDate1( QString date )
      {
        _date1 = date;
      }

    /**
     * Study date getter
     *
     * @return _date as a QString
     */
    QString
    getDate1()
      {
        return _date1;
      }

    /**
     * Study date setter
     *
     * @param date as a QString
     */
    void
    setDate2( QString date )
      {
        _date2 = date;
      }

    /**
     * Study date getter
     *
     * @return _date as a QString
     */
    QString
    getDate2()
      {
        return _date2;
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
     * Global method for exporting serie
     */
//    void
//    exportSerie();

    /**
     * Call dcm2nii in a QProcess object to reconstruct the given list of images
     *
     * @param images
     */
    void
    exportSerieFromCD();

    /**
     * Call dcm2nii in a QProcess object to reconstruct the given list of images
     *
     * @param images
     */
    void
    exportSerieFromPACS();

    /**
     * Call dcmqr on the list of server with parameters. Load the results in the patient list
     *
     */
    void
    queryPACS();

    /**
     * add patient in the list
     */
    void
    addPatient()
      {
        _patients.append(new QtDcmPatient());
      }

    /**
     * Mode getter
     */
    QString
    getMode()
      {
        return _mode;
      }

    void
    setImagesList( QList<QString> images )
      {
        _images = images;
      }

    QList<QPixmap>
    getListImages()
      {
        return _listImages;
      }

    void
    setSerieId( QString id )
      {
        _serieId = id;
      }

    QString
    getCurrentSerieDirectory()
      {
        return _currentSerieDir.absolutePath();
      }

    void
    setSeriesToExport(QMap<QString, QList<QString> > seriesToExport)
      {
        _seriesToExport = seriesToExport;
      }

    void
    exportSeries();

    void
    setQuery( QByteArray query )
      {
        _query = query;
      }

    void
    makePreview();
  };

#endif /* QTDCMMANAGER_H_ */
