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

#define QT_NO_CAST_TO_ASCII

#include <algorithm>
#include <iostream>
#include <limits.h>
#include <stdint.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QPointer>

#include <QtDcm.h>
#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmImage.h>
#include <QtDcmServer.h>
#include <QtDcmPreferences.h>
#include <QtDcmFindCallback.h>

// For dcm images
#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dcrledrg.h>      /* for DcmRLEDecoderRegistration */
#include <dcmtk/dcmjpeg/djdecode.h>     /* for dcmjpeg decoders */
#include <dcmtk/dcmjpeg/dipijpeg.h>     /* for dcmimage JPEG plugin */
// For color images
#include <dcmtk/dcmimage/diregist.h>

#include <QtDcmFindScu.h>
#include <QtDcmFindDicomdir.h>
#include <QtDcmMoveScu.h>
#include <QtDcmMoveDicomdir.h>
#include <QtDcmConvert.h>
#include <QtDcmConvert.h>
#include <QtDcmPreviewWidget.h>
#include <QtDcmImportWidget.h>
#include <QtDcmSerieInfoWidget.h>

#include <QtDcmManager.h>

class QtDcmManagerPrivate
{

public:
    QPointer<QtDcm> mainWidget;
    QString dicomdir;                                /** Dicomdir absolute file path */
    QString outputDir;                               /** Output directory for reconstructed serie absolute path */
    QDir currentSerieDir;                            /** Directory containing current serie dicom slice */
    QDir tempDir;                                    /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
    DcmFileFormat dfile;                             /** This attribute is usefull for parsing the dicomdir */
    QList<QtDcmPatient> patients;                  /** List that contains patients resulting of a query or read from a CD */
    QStringList images;                           /** List of image filename to export from a CD */
    QStringList listImages;                       /** List of images uid in the current selected serie */
    QMap<int, QString> mapImages;                    /** Map of images (corresponding to listImages) with InstanceNumber tags used as keys */
    QStringList seriesToImport;                   /** Selected series list in the treview */
    QString serieId;                                 /** Current selected serie UID */
    QString patientName;                             /** Attribute frepresenting the patient name used to query PACS */
    QString patientId;                               /** Attribute representing the patient id used to query PACS */
    QString patientSex;                              /** Attribute representing the patient sex used to query PACS */
    QString patientBirthDate;                        /** Attribute representing the patient birthdate used to query PACS */
    QString modality;                                /** Attibute for the modality of the search (MR, US, CT, etc) */
    QString date1;                                   /** Attribute for the begin date of the query (usefull for date based queries) */
    QString date2;                                   /** Attribute for the end date of the query (usefull for date based queries) */
    QString serieDescription;                        /** Attibute representing the serie description used for query PACS */
    QString studyDescription;                        /** Attibute representing the study description used for query PACS */
    QtDcmManager::eMoveMode mode;                    /** Mode that determine the type of media (MEDIA or PACS) */
    QString dcm2nii;                                 /** Absolute filename of the dcm2nii program */

    QtDcmManager::eOutputdirMode outputdirMode;       /** Output directory mode DIALOG or CUSTOM */
    QtDcmServer currentPacs;                       /** Current pacs index in the pacs list */

    QPointer<QTreeWidget> patientsTreeWidget;                /** The pointer to the patients tree widget */
    QPointer<QTreeWidget> studiesTreeWidget;                 /** The pointer to the studies tree widget */
    QPointer<QTreeWidget> seriesTreeWidget;                  /** The pointer to the series tree widget */

    QPointer<QtDcmPreviewWidget> previewWidget;              /** The pointer to the preview widget */
    QPointer<QtDcmImportWidget> importWidget;                /** The pointer to the import widget */
    QPointer<QtDcmSerieInfoWidget> serieInfoWidget;          /** The pointer to the serie info widget */

    bool useConverter;                               /** Use a converter ? */
};

QtDcmManager * QtDcmManager::_instance = 0;


QtDcmManager * QtDcmManager::instance()
{
    if ( _instance == 0 ) {
        _instance = new QtDcmManager(0);
    }
    
    return _instance;
}

void QtDcmManager::destroy()
{
    if (_instance != 0) {
        delete _instance;
        _instance = 0;
    }
}

QtDcmManager::QtDcmManager(QObject *parent)
    : QObject(parent),
      d ( new QtDcmManagerPrivate )
{   
    //Initialization of the private attributes
    d->useConverter = true;
    d->mode = PACS;
    d->dicomdir = "";
    d->outputDir = "";

    d->outputdirMode = DIALOG;

    d->patientName = "*";
    d->patientId = "*";
    d->patientBirthDate = "";
    d->modality = "*";
    d->date1 = "*";
    d->date2 = "*";
    d->serieDescription = "*";
    d->studyDescription = "*";
    d->patientSex = "*";

    d->mainWidget = NULL;
    d->patientsTreeWidget = NULL;
    d->studiesTreeWidget = NULL;
    d->seriesTreeWidget = NULL;

    d->importWidget = NULL;
    d->previewWidget = NULL;
    d->serieInfoWidget = NULL;
    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
}

QtDcmManager::~QtDcmManager()
{   
    this->deleteTemporaryDirs();
    
    QtDcmPreferences::destroy();
    delete d;
}

void QtDcmManager::setQtDcmWidget ( QtDcm* widget )
{
    d->mainWidget = widget;
    if ( ! d->mainWidget.isNull() ) {
        connect(QtDcmPreferences::instance(), &QtDcmPreferences::preferencesUpdated,
                d->mainWidget.data(),         &QtDcm::updatePacsComboBox);
        d->mainWidget->updatePacsComboBox();
    }
}

void QtDcmManager::setPatientsTreeWidget ( QTreeWidget * widget )
{
    d->patientsTreeWidget = widget;
}

void QtDcmManager::setStudiesTreeWidget ( QTreeWidget * widget )
{
    d->studiesTreeWidget = widget;
}

void QtDcmManager::setSeriesTreeWidget ( QTreeWidget * widget )
{
    d->seriesTreeWidget = widget;
}

void QtDcmManager::setImportWidget ( QtDcmImportWidget* widget )
{
    d->importWidget = widget;
    QObject::connect ( d->importWidget->importButton, &QPushButton::clicked,
                       this,                          &QtDcmManager::importSelectedSeries);
}

void QtDcmManager::setPreviewWidget ( QtDcmPreviewWidget* widget )
{
    d->previewWidget = widget;
}

void QtDcmManager::setSerieInfoWidget ( QtDcmSerieInfoWidget* widget )
{
    d->serieInfoWidget = widget;
}

QtDcmManager::eOutputdirMode QtDcmManager::getOutputdirMode() const
{
    return d->outputdirMode;
}

void QtDcmManager::setOutputdirMode ( QtDcmManager::eOutputdirMode mode )
{
    d->outputdirMode = mode;
}

void QtDcmManager::clearSerieInfo()
{
    if ( d->serieInfoWidget ) {
        d->serieInfoWidget->elementCountLabel->setText ( "" );
        d->serieInfoWidget->institutionLabel->setText ( "" );
        d->serieInfoWidget->operatorLabel->setText ( "" );
    }
}


void QtDcmManager::updateSerieInfo ( const QString &eltCount, 
                                     const QString &institution, 
                                     const QString &name )
{
    if ( d->serieInfoWidget ) {
        d->serieInfoWidget->elementCountLabel->setText ( eltCount );
        d->serieInfoWidget->institutionLabel->setText ( institution );
        d->serieInfoWidget->operatorLabel->setText ( name );
    }
}

void QtDcmManager::clearPreview()
{
    if ( d->previewWidget ) {
        d->previewWidget->imageLabel->setPixmap ( QPixmap() );
    }
}


void QtDcmManager::displayErrorMessage ( const QString &message )
{
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox msgBox(d->mainWidget);
    msgBox.setIcon ( QMessageBox::Critical );
    msgBox.setText ( message );
    msgBox.exec();
}

void QtDcmManager::displayMessage ( const QString &info )
{
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox msgBox ( d->mainWidget );
    msgBox.setIcon ( QMessageBox::Information );
    msgBox.setText ( info );
    msgBox.exec();
}

void QtDcmManager::findPatientsScu()
{
    if ( d->mainWidget->pacsComboBox->count() ) {
        d->seriesToImport.clear();
        d->mode = PACS;

        QtDcmFindScu * finder = new QtDcmFindScu ( this );
        finder->findPatientsScu ( d->patientName, d->patientSex );
        delete finder;
    }
}

void QtDcmManager::findStudiesScu ( const QString &patientName )
{
    d->seriesToImport.clear();

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findStudiesScu ( patientName, d->studyDescription, d->date1, d->date2 );
    delete finder;
}

void QtDcmManager::findSeriesScu ( const QString &patientName, const QString &studyUid )
{
    d->seriesToImport.clear();

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findSeriesScu ( patientName, studyUid, d->studyDescription, d->serieDescription, d->modality );
    delete finder;
}

void QtDcmManager::findImagesScu ( const QString &serieInstanceUID )
{
    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findImagesScu ( serieInstanceUID );
    delete finder;
}

void QtDcmManager::foundPatient ( const QMap<QString, QString> &infosMap )
{
    if ( !d->patientsTreeWidget.isNull() ) {
        QTreeWidgetItem * patientItem = new QTreeWidgetItem ( d->patientsTreeWidget->invisibleRootItem() );
        patientItem->setText ( 0, infosMap["Name"] );
        patientItem->setText ( 1, infosMap["ID"] );
        patientItem->setText ( 2, QDate::fromString ( infosMap["Birthdate"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        patientItem->setText ( 3, infosMap["Sex"] );
    }
}

void QtDcmManager::foundStudy ( const QMap<QString, QString> &infosMap )
{
    if ( !d->studiesTreeWidget.isNull() ) {
        QTreeWidgetItem * studyItem = new QTreeWidgetItem ( d->studiesTreeWidget->invisibleRootItem() );
        studyItem->setText ( 0, infosMap["Description"] );
        studyItem->setText ( 1, QDate::fromString ( infosMap["Date"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        studyItem->setData ( 2, 0, infosMap["UID"] );
        studyItem->setText ( 2, infosMap["UID"] );
        studyItem->setData ( 3, 0, infosMap["ID"] );
    }
}

void QtDcmManager::foundSerie ( const QMap<QString, QString> &infosMap )
{
    if ( !d->seriesTreeWidget.isNull() ) {
        QTreeWidgetItem * serieItem = new QTreeWidgetItem ( d->seriesTreeWidget->invisibleRootItem() );
        serieItem->setText ( 0, infosMap["Description"] );
        serieItem->setText ( 1, infosMap["Modality"] );
        serieItem->setText ( 2, QDate::fromString ( infosMap["Date"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        serieItem->setText ( 3, infosMap["ID"] );
        serieItem->setData ( 4, 0, QVariant ( infosMap["InstanceCount"] ) );
        serieItem->setData ( 5, 0, QVariant ( infosMap["Institution"] ) );
        serieItem->setData ( 6, 0, QVariant ( infosMap["Operator"] ) );
        serieItem->setCheckState ( 0, Qt::Unchecked );
    }
}

void QtDcmManager::foundImage ( const QString &image, int number )
{
    d->listImages.append ( image );
    if ( number ) {
        d->mapImages.insert ( number, image );
    }
}

void QtDcmManager::loadDicomdir()
{
    if ( d->dicomdir.isEmpty() ) {
        return;
    }

    d->mode = MEDIA;

    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;
    if ( ! ( status = d->dfile.loadFile ( d->dicomdir.toUtf8().data() ) ).good() ) {
        return;
    }

    this->findPatientsDicomdir();
}

void QtDcmManager::findPatientsDicomdir()
{
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findPatients();
    delete finder;
}

void QtDcmManager::findStudiesDicomdir ( const QString &patientName )
{
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findStudies ( patientName );
    delete finder;
}

void QtDcmManager::findSeriesDicomdir ( const QString &patientName, 
                                        const QString &studyUID )
{
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findSeries ( patientName, studyUID );
    delete finder;
}

void QtDcmManager::findImagesDicomdir ( const QString &uid )
{
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findImages ( uid );
    delete finder;
}

void QtDcmManager::moveSelectedSeries()
{
    if ( !d->tempDir.exists() ) {
        return;
    }

    // Uhh
    qApp->processEvents();

    switch (d->mode) {
    case MEDIA:
    {
        QtDcmMoveDicomdir * mover = new QtDcmMoveDicomdir ( this );
        mover->setDcmItem ( d->dfile.getDataset() );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setImportDir ( d->outputDir );
        mover->setSeries ( d->seriesToImport );
        connect ( mover, &QtDcmMoveDicomdir::updateProgress,
                  this,  &QtDcmManager::updateProgressBar);
        connect ( mover, &QtDcmMoveDicomdir::serieMoved,
                  this,  &QtDcmManager::onSerieMoved);
        connect ( mover, &QtDcmMoveDicomdir::finished,
                  this,  &QtDcmManager::moveSeriesFinished);
        connect ( mover, &QtDcmMoveDicomdir::finished,
                  mover, &QtDcmMoveDicomdir::deleteLater);
        mover->start();
    }
        break;
    case PACS: 
    {
        QtDcmMoveScu * mover = new QtDcmMoveScu ( this );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( d->seriesToImport );
        mover->setImportDir ( d->outputDir );
        connect ( mover, &QtDcmMoveScu::updateProgress,
                  this,  &QtDcmManager::updateProgressBar);
        connect ( mover, &QtDcmMoveScu::serieMoved,
                  this,  &QtDcmManager::onSerieMoved);
        connect ( mover, &QtDcmMoveScu::finished,
                  this,  &QtDcmManager::moveSeriesFinished);
        connect ( mover, &QtDcmMoveScu::finished,
                  mover, &QtDcmMoveScu::deleteLater);
        mover->start();
    }
        break;
    default:
        qWarning() << "Move mode not supported";
        break;
    }
}

void QtDcmManager::getPreviewFromSelectedSerie ( const QString &uid, int elementIndex )
{
    if ( !d->tempDir.exists() ) {
        return;
    }

    if ( !d->listImages.size() ) {
        return;
    }

    if ( d->listImages.size() < elementIndex ) {
        return;
    }

    QString imageId = d->listImages[elementIndex];

    if ( d->mapImages.size() && d->mapImages.contains ( elementIndex ) ) {
        imageId = d->mapImages[elementIndex];
    }

    switch(d->mode) {
    case MEDIA:
    {
        QtDcmMoveDicomdir * mover = new QtDcmMoveDicomdir ( this );
        mover->setMode ( QtDcmMoveDicomdir::PREVIEW );
        mover->setDcmItem ( d->dfile.getDataset() );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( QStringList() << uid );
        mover->setImageId ( imageId );
        connect(mover, &QtDcmMoveDicomdir::previewSlice ,
                this,  &QtDcmManager::makePreview);
        connect(mover, &QtDcmMoveDicomdir::finished,
                mover, &QtDcmMoveDicomdir::deleteLater);
        mover->start();
    }
        break;
    case PACS:
    {
        //Check if file has already been moved
        this->clearPreview();
        emit gettingPreview();

        QString modality ( "MR" );
        if ( d->seriesTreeWidget->currentItem() ) {
            modality = d->seriesTreeWidget->currentItem()->text ( 1 );
        }
        
        QString filename ( d->tempDir.absolutePath() + "/" + uid + "/" + modality + "." + imageId );
        if ( QFile ( filename ).exists() ) {
            makePreview ( filename );
        }
        else {
            QtDcmMoveScu * mover = new QtDcmMoveScu ( this );
            mover->setMode ( QtDcmMoveScu::PREVIEW );
            mover->setOutputDir ( d->tempDir.absolutePath() );
            mover->setSeries ( QStringList() << uid );
            mover->setImageId ( imageId );
            connect(mover, &QtDcmMoveScu::previewSlice,
                    this,  &QtDcmManager::makePreview);
            connect(mover, &QtDcmMoveScu::finished,
                    mover, &QtDcmMoveScu::deleteLater);
            mover->start();
        }
    }
        break;
    default:
        qWarning() <<  "Move mode not supported";
        break;
    }

    return;
}


void QtDcmManager::importSelectedSeries()
{
    if ( this->useConverter() ) { //Use QtDcm convertion tool (ITK or dcm2nii)
        if ( this->seriesToImportSize() != 0 ) {
            if ( this->getOutputdirMode() == QtDcmManager::DIALOG ) {
                QFileDialog dialog( d->mainWidget );
                dialog.setFileMode ( QFileDialog::Directory );
                dialog.setOption ( QFileDialog::ShowDirsOnly, true );
                
                // Trying to open directly on one of the available drives
                if (!QDir::drives().isEmpty()) {
                    dialog.setDirectory ( QDir::drives().first().absoluteDir() );
                }
                
                dialog.setWindowTitle ( tr ( "Export directory" ) );
                QString directory;
                if ( dialog.exec() ) {
                    directory = dialog.selectedFiles() [0];
                }

                if ( !directory.isEmpty() ) { // A file has been chosen
                    // Set the choosen output directory to the manager and launch the conversion process
                    this->setOutputDirectory ( directory );
                    this->moveSelectedSeries();
                }
            }
            else {
                if ( QDir ( this->outputDirectory() ).exists() ) {
                    this->moveSelectedSeries();
                }
            }
        }
    }
    else { //Only copy the dicom files in a temporary directory
        this->setOutputDirectory ( "" );
        this->moveSelectedSeries();
    }
}

void QtDcmManager::importToDirectory ( const QString &directory )
{
    if ( this->seriesToImportSize() != 0 ) {
        this->setOutputDirectory ( directory );
        this->moveSelectedSeries();
    }
}

void QtDcmManager::onSerieMoved ( const QString &directory , const QString &serie , int number )
{
    if ( d->useConverter ) {
        qDebug() << "Starting reconstruction of series" << serie;
        
        QtDcmConvert converter( this );
        converter.setInputDirectory ( directory );
        converter.setOutputFilename ( serie + ".nii" );
        converter.setOutputDirectory ( d->outputDir );
        converter.setTempDirectory ( d->tempDir.absolutePath() );
        converter.setSerieUID ( serie );
        converter.convert();
        qDebug() << "Conversion complete";
        
        if ( number == this->seriesToImportSize() - 1 ) {
            emit importFinished();
        }
    }
}

void QtDcmManager::moveSeriesFinished()
{
    if ( d->importWidget ) {
        d->importWidget->importProgressBar->setValue ( 0 );
    }
}

void QtDcmManager::updateProgressBar ( int i )
{
    if ( d->importWidget ) {
        d->importWidget->importProgressBar->setValue ( i );
    }
    qApp->processEvents();
}

void QtDcmManager::createTemporaryDirs()
{
    //Creation d'un répertoire temporaire pour la série
    QDir tempDir = QDir ( QDir::tempPath() );

    //Use Quuid to generate the temporary directory
    const QString randName = QUuid::createUuid().toString();

    if ( !tempDir.exists ( "qtdcm" ) )
        tempDir.mkdir ( "qtdcm" );

    QDir qtdcmDir = QDir ( QDir::tempPath() + QDir::separator() + "qtdcm" );

    if ( !qtdcmDir.exists ( randName ) ) {
        qtdcmDir.mkdir ( randName );
    }

    d->tempDir = QDir ( qtdcmDir.absolutePath() + QDir::separator() + randName );
}

void QtDcmManager::deleteTemporaryDirs()
{   
    QDir qtdcmTmpDir(QDir::tempPath() + QDir::separator() + "qtdcm");
    if(!qtdcmTmpDir.removeRecursively()) {
        qWarning() << "Cannot remove recursively temporary directory" << qtdcmTmpDir.absolutePath();
    }
}

void QtDcmManager::generateCurrentSerieDir()
{
    if ( !d->serieId.isEmpty() ) {
        d->currentSerieDir = d->tempDir.absolutePath() + QDir::separator() + d->serieId;
        d->tempDir.mkdir ( d->serieId );
    }
}

void QtDcmManager::deleteCurrentSerieDir()
{
    // Suppression des fichiers temporaires
    const QStringList listFiles = d->currentSerieDir.entryList ( QDir::Files, QDir::Name );

    for ( int i = 0; i < listFiles.size(); i++ ) {
        d->currentSerieDir.remove ( listFiles.at ( i ) );
    }

    // Suppression du répertoire temporaire
    if ( !d->tempDir.rmdir ( d->serieId ) ) {
        qDebug() << tr ( "Probleme lors de la suppression du répertoire temporaire" );
    }
}

void QtDcmManager::makePreview ( const QString &filename )
{
    DcmRLEDecoderRegistration::registerCodecs ( OFFalse, OFFalse );
    DJDecoderRegistration::registerCodecs ( EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse );
    DcmFileFormat file;
    file.loadFile ( filename.toLatin1().data() );
    DcmDataset * dset = file.getDataset();
    DicomImage* dcimage = new DicomImage ( dset, file.getDataset()->getOriginalXfer(), CIF_MayDetachPixelData );


    if ( dcimage != NULL ) {
        dcimage->setNoDisplayFunction();
        dcimage->hideAllOverlays();
        dcimage->setNoVoiTransformation();

        if ( dcimage->getStatus() == EIS_Normal ) {
            Uint32 *pixelData = ( Uint32 * ) ( dcimage->getOutputData ( 32 /* bits per sample */ ) );

            if ( pixelData != NULL ) {
                Uint8 *colored = new Uint8[dcimage->getWidth() * dcimage->getHeight() * 4]; //4 * dcimage->getWidth() * dcimage->getHeight() matrix
                Uint8 *col = colored;
                Uint32 *p = pixelData;
                //get the highest values for RGBA, then use them to scale the pixel luminosity
                Uint32 p_max = 0;
#ifdef WIN32                
                Uint32 p_min = UINT_LEAST32_MAX;
#else
                Uint32 p_min = std::numeric_limits<Uint32>::max();
#endif                

                for ( unsigned i = 0; i < dcimage->getWidth(); ++i ) {
                    for ( unsigned j = 0; j < dcimage->getHeight(); ++j, ++p ) {
                        if ( *p > p_max ) {
                            p_max = *p;
                        }
                        
                        if ( *p < p_min ) {
                            p_min = *p;
                        }
                    }
                }

                double a = 4294967295.f / ( ( double ) p_max - ( double ) p_min );

                //re-initialize 'col'
                p = pixelData;
                //copy the pixels in our QImage

                for ( unsigned i = 0; i < dcimage->getWidth(); ++i ) {
                    for ( unsigned j = 0; j < dcimage->getHeight(); ++j, ++p ) {
                        *col = ( Uint8 ) ( ( 255.f / 4294967295.f ) * ( a * ( ( double ) ( *p ) - ( double ) p_min ) ) );
                        ++col;
                        *col = ( Uint8 ) ( ( 255.f / 4294967295.f ) * ( a * ( ( double ) ( *p ) - ( double ) p_min ) ) );
                        ++col;
                        *col = ( Uint8 ) ( ( 255.f / 4294967295.f ) * ( a * ( ( double ) ( *p ) - ( double ) p_min ) ) );
                        ++col;
                        *col = 255;
                        ++col;
                    }
                }

                QImage image ( colored, dcimage->getWidth(), dcimage->getHeight(), QImage::Format_ARGB32 );

                if ( d->previewWidget ) {
                    d->previewWidget->imageLabel->setPixmap ( QPixmap::fromImage ( image.scaled ( 130,130 ), Qt::AutoColor ) );
                }

                delete[] colored;

            }
        }

        delete dcimage;
    }
    
    DcmRLEDecoderRegistration::cleanup();
    DJDecoderRegistration::cleanup();
}

// Getters and setters
QString QtDcmManager::dicomdir() const
{
    return d->dicomdir;
}

void QtDcmManager::setDicomdir ( const QString &dicomdir )
{
    d->dicomdir = dicomdir;
    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;

    if ( ! ( status = d->dfile.loadFile ( d->dicomdir.toUtf8().data() ) ).good() ) {
        return;
    }
}

QString QtDcmManager::outputDirectory() const
{
    return d->outputDir;
}

void QtDcmManager::setOutputDirectory ( const QString &directory )
{
    d->outputDir = directory;
}

QtDcmServer QtDcmManager::currentPacs() const
{
    return d->currentPacs;
}

void QtDcmManager::setCurrentPacs ( int index )
{
    if ( index < QtDcmPreferences::instance()->servers().size() ) {
        d->currentPacs = QtDcmPreferences::instance()->servers().at ( index );
    }
}

QString QtDcmManager::patientName() const
{
    return d->patientName;
}

void QtDcmManager::setPatientName ( const QString &patientName )
{
    d->patientName = patientName;
}

QString QtDcmManager::patientId() const
{
    return d->patientId;
}

void QtDcmManager::setPatientId ( const QString &patientId )
{
    d->patientId = patientId;
}

QString QtDcmManager::patientBirthdate() const 
{
    QString birthdate;
    if ( d->patientsTreeWidget && d->patientsTreeWidget->currentItem() ) {
        birthdate = d->patientsTreeWidget->currentItem()->data ( 2,0 ).toString();
    }
    qDebug() << birthdate;
    return birthdate;
}

QString QtDcmManager::patientGender() const 
{
    QString sex ( "" );
    if ( d->patientsTreeWidget && d->patientsTreeWidget->currentItem() ) {
        sex = d->patientsTreeWidget->currentItem()->data ( 3,0 ).toString();
    }
    
    qDebug() << sex;
    return sex;
}

QString QtDcmManager::examDate() const 
{ 
    QString examDate;
    if ( d->studiesTreeWidget && d->studiesTreeWidget->currentItem() ) {
        examDate = d->studiesTreeWidget->currentItem()->data ( 1,0 ).toString();
    }
    
    qDebug() << examDate;
    return examDate;
}

void QtDcmManager::setPatientSex ( const QString &sex )
{
    d->patientSex = sex;
}

QString QtDcmManager::seriesDescription() const 
{
    return d->serieDescription;
}

void QtDcmManager::setSerieDescription ( const QString &serieDescription )
{
    d->serieDescription = serieDescription;
}

QString QtDcmManager::studyDescription() const 
{
    return d->studyDescription;
}

void QtDcmManager::setStudyDescription ( const QString &studyDescription )
{
    d->studyDescription = studyDescription;
}

void QtDcmManager::setModality ( const QString &modality )
{
    d->modality = modality;
}

QString QtDcmManager::modality() const 
{
    return d->modality;
}

void QtDcmManager::setStartDate ( const QString &date )
{
    d->date1 = date;
}

QString QtDcmManager::startDate() const 
{
    return d->date1;
}

void QtDcmManager::setEndDate ( const QString &date )
{
    d->date2 = date;
}

QString QtDcmManager::getEndDate() const 
{
    return d->date2;
}

void QtDcmManager::addPatient()
{
    d->patients.append ( QtDcmPatient() );
}

QtDcmManager::eMoveMode QtDcmManager::mode() const 
{
    return d->mode;
}

void QtDcmManager::setListOfImages ( const QStringList &images )
{
    d->images = images;
}

QList<QString> QtDcmManager::listOfImages() const
{
    return d->listImages;
}

void QtDcmManager::clearListOfImages()
{
    d->listImages.clear();
    d->mapImages.clear();
}

void QtDcmManager::setSerieId ( const QString &id )
{
    d->serieId = id;
}

QString QtDcmManager::currentSeriesDirectory() const 
{
    return d->currentSerieDir.absolutePath();
}

void QtDcmManager::addSerieToImport ( const QString &uid )
{
    if ( !d->seriesToImport.contains ( uid ) ) {
        d->seriesToImport.append ( uid );
    }
}

void QtDcmManager::removeSerieToImport ( const QString &uid )
{
    if ( d->seriesToImport.contains ( uid ) )
        d->seriesToImport.removeOne ( uid );
}

void QtDcmManager::clearSeriesToImport()
{
    d->seriesToImport.clear();
}

int QtDcmManager::seriesToImportSize()
{
    return d->seriesToImport.size();
}

bool QtDcmManager::useConverter()
{
    return d->useConverter;
}

void QtDcmManager::useConverter ( bool use )
{
    d->useConverter = use;
}

