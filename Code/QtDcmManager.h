/*
    QtDcm is a C++ Qt based library for communication and conversion of Dicom images.
    Copyright (C) 2011  Alexandre Abadie <Alexandre.Abadie@univ-rennes1.fr>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef QTDCMMANAGER_H_
#define QTDCMMANAGER_H_

#include "qtdcmExports.h"
#include <QtGui>
#include <QtNetwork>

class QTreeWidget;
class QtDcm;
class QtDcmServer;
class QtDcmPreferences;
class QtDcmFindScuSignalManager;
class QtDcmPreviewWidget;
class QtDcmImportWidget;
class QtDcmSerieInfoWidget;

class QtDcmManagerPrivate;

/**
 * This class is in charge of the different process (dcm2nii), pacs query/retrieve (dcm4chee),
 * temporary directory creation and removing, PACS server settings.
 *
 */

class  QTDCM_EXPORT QtDcmManager : public QObject
{
    Q_OBJECT

public:
    enum eOutputdirMode {
        CUSTOM = 0,
        DIALOG
    };

    enum eConvertionMethod {
        ITK = 0,
        DCM2NII
    };
    
    enum eMoveMode {
        MEDIA = 0,
        PACS
    };

    static QtDcmManager* instance();
    static void destroy();
    
    /**
     * Default constructor. Instantiate the internal pointers and create the temporary directory.
     */
    QtDcmManager(QObject * parent = 0);

    /**
     * The default destructor
     */
    virtual ~QtDcmManager();

    /**
     * Find SCU with Dcmtk code
     */
    void findPatientsScu();
    void findStudiesScu ( const QString &patientName );
    void findSeriesScu ( const QString &patientName,
                         const QString &studyUID );
    void findImagesScu ( const QString &uid );
    void foundPatient ( const QMap<QString, QString> &infosMap );
    void foundStudy ( const QMap<QString, QString> &infosMap );
    void foundSerie ( const QMap<QString, QString> &infosMap );
//     void foundImage ( QMap<QString, QString> infosMap );
    void foundImage ( const QString &image, int number );
    void moveSelectedSeries();
    void getPreviewFromSelectedSerie ( const QString &uid, int elementCount );
//     void getPreviewFromSelectedSerie ( int elementIndex );

    void findPatientsDicomdir();
    void findStudiesDicomdir ( const QString &patientName );
    void findSeriesDicomdir ( const QString &patientName,
                              const QString &studyDescription );
    void findImagesDicomdir ( const QString &serieUID );

    void setQtDcmWidget ( QtDcm * widget );
    void setPatientsTreeWidget ( QTreeWidget * widget );
    void setStudiesTreeWidget ( QTreeWidget * widget );
    void setSeriesTreeWidget ( QTreeWidget * widget );

    void setImportWidget ( QtDcmImportWidget * widget );
    void setPreviewWidget ( QtDcmPreviewWidget * widget );
    void setSerieInfoWidget ( QtDcmSerieInfoWidget * widget );

    void setOutputdirMode ( QtDcmManager::eOutputdirMode mode );
    QtDcmManager::eOutputdirMode getOutputdirMode() const;

    void clearSerieInfo();
    void updateSerieInfo ( const QString &eltCount,
                           const QString &institution,
                           const QString &name );

    /**
     * This method read the dicomdir file and populate the patient treewidget
     *
     */
    void loadDicomdir();

    /**
     * Convenience method that display error message in a QMessageBox window.
     *
     * @param message the error message to display
     */
    void displayErrorMessage ( const QString &message );

    /**
     * Convenience method that display information message in a QMessageBox window.
     *
     * @param info the information message to display
     */
    void displayMessage ( const QString &info );

    /**
     * Return the dicomdir absolute path name
     *
     * @return _dicomdir the dicomdir file name.
     */
    QString dicomdir() const;

    /**
     * Set the dicomdir file name
     *
     * @param dicomdir the dicomdir file name
     */
    void setDicomdir ( const QString &dicomdir );

    /**
     * Return the output directory where the current serie will be reconstructed
     *
     * @return _outputdir the output directory path
     */
    QString outputDirectory() const;

    /**
     * Set the output directory
     *
     * @param directory the output directory
     */
    void setOutputDirectory ( const QString &directory );

    QtDcmServer currentPacs() const;

    void setCurrentPacs ( int index );

    /**
     * Patient name getter
     *
     * @return _patientName as a QString
     */
    QString patientName() const;

    /**
     * Patient name setter
     *
     * @param patientName as a QString
     */
    void setPatientName ( const QString &patientName );

    /**
     * Patient id getter
     *
     * @return _patientId as a QString
     */
    QString patientId() const;

    /**
     * Patient id setter
     *
     * @param patientId as a QString
     */
    void setPatientId ( const QString &patientId );

    QString patientBirthdate() const;

    QString patientGender() const;

    void setPatientSex ( const QString &sex );

    /**
     * Serie description getter
     *
     * @return _serieDescription as a QString
     */
    QString seriesDescription() const;

    /**
     * Serie description setter
     *
     * @param serieDescription as a QString
     */
    void setSerieDescription ( const QString &serieDescription );

    /**
     * Study description getter
     *
     * @return _studyDescription as a QString
     */
    QString studyDescription() const;

    /**
     * Study description setter
     *
     * @param studyDescription as a QString
     */
    void setStudyDescription ( const QString &studyDescription );

    QString examDate() const;

    /**
     * Study modality setter
     *
     * @param modality as a QString
     */
    void setModality ( const QString &modality );

    /**
     * Study modality getter
     *
     * @return _modality as a QString
     */
    QString modality() const;

    /**
     * Study date setter
     *
     * @param date as a QString
     */
    void setStartDate ( const QString &date );

    /**
     * Study date getter
     *
     * @return _date as a QString
     */
    QString startDate() const;

    /**
     * Study date setter
     *
     * @param date as a QString
     */
    void setEndDate ( const QString &date );

    /**
     * Study date getter
     *
     * @return _date as a QString
     */
    QString getEndDate() const;

    void addSerieToImport ( const QString &uid );

    void removeSerieToImport ( const QString &uid );

    void clearSeriesToImport();

    int seriesToImportSize();

    /**
     * add patient in the list
     */
    void addPatient();

    /**
     * Mode getter
     */
    eMoveMode mode() const;

    void setListOfImages ( const QStringList &images );

    QList<QString> listOfImages() const;
    void clearListOfImages();

    void setSerieId ( const QString &id );

    QString currentSeriesDirectory() const;

//     void setPreviewImageUID ( QString uid );

    bool useConverter();

    void useConverter ( bool use );

    /**
     * This method try to delete the temporary directory when closing the QtDcm widget
     * (Doesn't work for the moment)
     */
    void deleteTemporaryDirs();

public slots:
//    void onPatientFound()

    void updateProgressBar ( int i );
    void moveSeriesFinished();
    void clearPreview();
    void makePreview ( const QString &filename );
    void onSerieMoved ( const QString &directory, const QString &uid, int number );

    void importSelectedSeries();
    void importToDirectory ( const QString &directory );

signals:
    void serieMoved ( const QString &directory );
    void importFinished();
    void gettingPreview();
    
private:
    void generateCurrentSerieDir();

    void deleteCurrentSerieDir();

    /**
     * Create the temporary directory (/tmp/qtdcm on Unix) and the logging directory.
     * (/tmp/qtdcm/logs)
     */
    void createTemporaryDirs();
    
private:
    static QtDcmManager * _instance;
    QtDcmManagerPrivate *d;
};

#endif /* QTDCMMANAGER_H_ */
