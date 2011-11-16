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
#include <limits>

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
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
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
    QtDcm * mainWidget;
    QString dicomdir; /** Dicomdir absolute file path */
    QString outputDir; /** Output directory for reconstructed serie absolute path */
    QDir currentSerieDir; /** Directory containing current serie dicom slice */
    QDir tempDir; /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
    QDir logsDir; /** Directory of the reconstruction process logs file (/tmp/qtdcm/logs) */
    DcmFileFormat dfile; /** This attribute is usefull for parsing the dicomdir */
    QList<QtDcmPatient *> patients; /** List that contains patients resulting of a query or read from a CD */
    QList<QString> images; /** List of image filename to export from a CD */
    QMap<QString, QList<QString> > seriesToExport;
    QList<QString> listImages;
    QMap<int, QString> mapImages;
    QList<QString> seriesToImport;
    QString serieId; /** Id of the serie to export from the PACS */
    QProcess * process; /** This attribute launch the reconstruction process */
    QString patientName; /** Attribute frepresenting the patient name used for query PACS */
    QString patientId; /** Attribute representing the patient id used for query PACS */
    QString patientSex;
    QString patientBirthDate;
    QString modality; /** Attibute for the modality of the search (MR, US, CT, etc) */
    QString date1; /** Attribute for the begin date of the query (usefull for date based queries) */
    QString date2; /** Attribute for the end date of the query (usefull for date based queries) */
    QString serieDescription; /** Attibute representing the serie description used for query PACS */
    QString studyDescription; /** Attibute representing the study description used for query PACS */
    QString mode; /** Mode that determine the type of media (CD or PACS) */
    QString dcm2nii; /** Absolute filename of the dcm2nii program */
    QByteArray query;
    QString previewImageUID;

    QtDcmManager::outputdirmode outputdirMode;
    QtDcmServer * currentPacs;

    QTreeWidget * patientsTreeWidget;
    QTreeWidget * studiesTreeWidget;
    QTreeWidget * seriesTreeWidget;

    QtDcmPreviewWidget * previewWidget;
    QtDcmImportWidget * importWidget;
    QtDcmSerieInfoWidget * serieInfoWidget;

    bool useConverter;
};

QtDcmManager * QtDcmManager::_instance = 0;

QtDcmManager *
QtDcmManager ::instance()
{
    if ( _instance == 0 )
        _instance = new QtDcmManager();
    return _instance;
}

QtDcmManager::QtDcmManager() : d ( new QtDcmManagerPrivate )
{
    //Initialization of the private attributes
    d->useConverter = true;
    d->mode = "PACS";
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
    d->currentPacs = NULL;
    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
}

QtDcmManager::~QtDcmManager()
{
    this->deleteTemporaryDirs();
}

void QtDcmManager::setQtDcmWidget ( QtDcm* widget )
{
    d->mainWidget = widget;
    if ( d->mainWidget )
    {
        QObject::connect ( QtDcmPreferences::instance(), SIGNAL ( preferencesUpdated() ), d->mainWidget, SLOT ( updatePacsComboBox() ) );
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
    QObject::connect ( d->importWidget->importButton, SIGNAL ( clicked() ), this, SLOT ( importSelectedSeries() ) );
}

void QtDcmManager::setPreviewWidget ( QtDcmPreviewWidget* widget )
{
    d->previewWidget = widget;
}

void QtDcmManager::setSerieInfoWidget ( QtDcmSerieInfoWidget* widget )
{
    d->serieInfoWidget = widget;
}

QtDcmManager::outputdirmode QtDcmManager::getOutputdirMode()
{
    return d->outputdirMode;
}

void QtDcmManager::setOutputdirMode ( QtDcmManager::outputdirmode mode )
{
    d->outputdirMode = mode;
}

void QtDcmManager::clearSerieInfo()
{
    if ( d->serieInfoWidget )
    {
        d->serieInfoWidget->elementCountLabel->setText ( "" );
        d->serieInfoWidget->institutionLabel->setText ( "" );
        d->serieInfoWidget->operatorLabel->setText ( "" );
    }
}


void QtDcmManager::updateSerieInfo ( QString eltCount, QString institution, QString name )
{
    if ( d->serieInfoWidget )
    {
        d->serieInfoWidget->elementCountLabel->setText ( eltCount );
        d->serieInfoWidget->institutionLabel->setText ( institution );
        d->serieInfoWidget->operatorLabel->setText ( name );
    }
}

void QtDcmManager::clearPreview()
{
    if ( d->previewWidget )
        d->previewWidget->imageLabel->setPixmap ( NULL );
}


void QtDcmManager::displayErrorMessage ( QString message )
{
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox ( d->mainWidget );
    msgBox->setIcon ( QMessageBox::Critical );
    msgBox->setText ( message );
    msgBox->exec();
    delete msgBox;
}

void QtDcmManager::displayMessage ( QString info )
{
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox ( d->mainWidget );
    msgBox->setIcon ( QMessageBox::Information );
    msgBox->setText ( info );
    msgBox->exec();
    delete msgBox;
}

void QtDcmManager::findPatientsScu()
{
    if ( d->mainWidget->pacsComboBox->count() )
    {
        d->seriesToImport.clear();
        d->mode = "PACS";

        QtDcmFindScu * finder = new QtDcmFindScu ( this );
        finder->findPatientsScu ( d->patientName, d->patientSex );
        delete finder;
    }
}

void QtDcmManager::findStudiesScu ( QString patientName )
{
    d->seriesToImport.clear();

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findStudiesScu ( patientName, d->studyDescription, d->date1, d->date2 );
    delete finder;
}

void QtDcmManager::findSeriesScu ( QString patientName, QString studyUid )
{
    d->seriesToImport.clear();

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findSeriesScu ( patientName, studyUid, d->studyDescription, d->serieDescription, d->modality );
    delete finder;
}

void QtDcmManager::findImagesScu ( QString serieInstanceUID )
{
    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findImagesScu ( serieInstanceUID );
    delete finder;
}

void QtDcmManager::foundPatient ( QMap<QString, QString> infosMap )
{
    if ( d->patientsTreeWidget )
    {
        QTreeWidgetItem * patientItem = new QTreeWidgetItem ( d->patientsTreeWidget->invisibleRootItem() );
        patientItem->setText ( 0, infosMap["Name"] );
        patientItem->setText ( 1, infosMap["ID"] );
        patientItem->setText ( 2, QDate::fromString ( infosMap["Birthdate"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        patientItem->setText ( 3, infosMap["Sex"] );
    }
}

void QtDcmManager::foundStudy ( QMap<QString, QString> infosMap )
{
    if ( d->studiesTreeWidget )
    {
        QTreeWidgetItem * studyItem = new QTreeWidgetItem ( d->studiesTreeWidget->invisibleRootItem() );
        studyItem->setText ( 0, infosMap["Description"] );
        studyItem->setText ( 1, QDate::fromString ( infosMap["Date"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        studyItem->setData ( 2, 0, infosMap["UID"] );
        studyItem->setText ( 2, infosMap["UID"] );
        studyItem->setData ( 3, 0, infosMap["ID"] );
    }
}

void QtDcmManager::foundSerie ( QMap<QString, QString> infosMap )
{
    if ( d->seriesTreeWidget )
    {
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

void QtDcmManager::foundImage ( QString image, int number )
{
    d->listImages.append ( image );
    if ( number )
        d->mapImages.insert ( number, image );
}

void QtDcmManager::loadDicomdir()
{
    if ( d->dicomdir.isEmpty() )
        return;

    d->mode = "CD";

    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;

    if ( ! ( status = d->dfile.loadFile ( d->dicomdir.toUtf8().data() ) ).good() )
    {
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

void QtDcmManager::findStudiesDicomdir ( QString patientName )
{
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findStudies ( patientName );
    delete finder;
}

void QtDcmManager::findSeriesDicomdir ( QString patientName, QString studyUID )
{
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findSeries ( patientName, studyUID );
    delete finder;
}

void QtDcmManager::findImagesDicomdir ( QString uid )
{
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findImages ( uid );
    delete finder;
}

void QtDcmManager::moveSelectedSeries()
{
    if ( !d->tempDir.exists() )
        return;

    qApp->processEvents();

    if ( d->mode == "CD" )
    {
        QtDcmMoveDicomdir * mover = new QtDcmMoveDicomdir ( this );
        mover->setDcmItem ( d->dfile.getDataset() );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setImportDir ( d->outputDir );
        mover->setSeries ( d->seriesToImport );
        QObject::connect ( mover, SIGNAL ( updateProgress ( int ) ), this, SLOT ( updateProgressBar ( int ) ) );
        QObject::connect ( mover, SIGNAL ( finished() ), this, SLOT ( moveSeriesFinished() ) );
        QObject::connect ( mover, SIGNAL ( serieMoved ( QString, QString, int ) ), this, SLOT ( onSerieMoved ( QString, QString, int ) ) );
        mover->start();
    }
    else
    {
        QtDcmMoveScu * mover = new QtDcmMoveScu ( this );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( d->seriesToImport );
        mover->setImportDir ( d->outputDir );
        QObject::connect ( mover, SIGNAL ( updateProgress ( int ) ), this, SLOT ( updateProgressBar ( int ) ) );
        QObject::connect ( mover, SIGNAL ( finished() ), this, SLOT ( moveSeriesFinished() ) );
        QObject::connect ( mover, SIGNAL ( serieMoved ( QString, QString, int ) ), this, SLOT ( onSerieMoved ( QString, QString, int ) ) );
        mover->start();
    }
}

void QtDcmManager::getPreviewFromSelectedSerie ( QString uid, int elementIndex )
{
    if ( !d->tempDir.exists() )
        return;

    if ( !d->listImages.size() )
        return;

    QString imageId = d->listImages[elementIndex];
    
    if ( d->mapImages.size() && d->mapImages.contains(elementIndex))
      imageId = d->mapImages[elementIndex];
    
    if ( d->mode == "CD" )
    {
        QtDcmMoveDicomdir * mover = new QtDcmMoveDicomdir ( this );
        mover->setMode ( QtDcmMoveDicomdir::PREVIEW );
        mover->setDcmItem ( d->dfile.getDataset() );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( QStringList() << uid );
        mover->setImageId ( imageId );
        QObject::connect ( mover, SIGNAL ( previewSlice ( QString ) ), this, SLOT ( makePreview ( QString ) ) );
        mover->start();
    }
    else // mode PACS
    {
      //Check if file has already been moved
      QString modality("MR");
      if (d->seriesTreeWidget->currentItem())
        modality = d->seriesTreeWidget->currentItem()->text(1);
      QString filename(d->tempDir.absolutePath() + "/" + uid + "/" + modality + "." + imageId);

      if (QFile(filename).exists())
        makePreview(filename);
      else
      {
        emit gettingPreview();
        QtDcmMoveScu * mover = new QtDcmMoveScu ( this );
        mover->setMode ( QtDcmMoveScu::PREVIEW );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( QStringList() << uid );
        mover->setImageId ( imageId );
        QObject::connect ( mover, SIGNAL ( previewSlice ( QString ) ), this, SLOT ( makePreview ( QString ) ) );
        QObject::connect ( this, SIGNAL ( gettingPreview ( ) ), mover, SLOT ( terminate() ) );
        mover->start();
      }
    }

    return;
}


void QtDcmManager::importSelectedSeries()
{
    if ( this->useConverter() )  //Use QtDcm convertion tool (ITK or dcm2nii)
    {
        if ( this->seriesToImportSize() != 0 )
        {
            if ( this->getOutputdirMode() == QtDcmManager::DIALOG )
            {
                QFileDialog * dialog = new QFileDialog ( d->mainWidget );
                dialog->setFileMode ( QFileDialog::Directory );
                dialog->setOption ( QFileDialog::ShowDirsOnly, true );
                dialog->setDirectory ( QDir::home().dirName() );
                dialog->setWindowTitle ( tr ( "Export directory" ) );
                QString directory;

                if ( dialog->exec() )
                {
                    directory = dialog->selectedFiles() [0];
                }

                dialog->close();

                if ( !directory.isEmpty() )   // A file has been chosen
                {
                    // Set the choosen output directory to the manager and launch the conversion process
                    this->setOutputDirectory ( directory );
                    this->moveSelectedSeries();
                }

                delete dialog;
            }
            else
            {
                if ( QDir ( this->getOutputDirectory() ).exists() )
                {
                    this->moveSelectedSeries();
                }
            }
        }
    }
    else //Only copy the dicom files in a temporary directory
    {
        this->setOutputDirectory ( "" );
        this->moveSelectedSeries();
    }
}

void QtDcmManager::importToDirectory ( QString directory )
{
    if ( this->seriesToImportSize() != 0 )
    {
        this->setOutputDirectory ( directory );
        this->moveSelectedSeries();
    }
}

void QtDcmManager::onSerieMoved ( QString directory , QString serie , int number )
{
    emit serieMoved ( directory );
    if ( d->useConverter )
    {
        QtDcmConvert * converter = new QtDcmConvert ( this );
        converter->setInputDirectory ( directory );
        converter->setOutputFilename ( serie + ".nii" );
        converter->setOutputDirectory ( d->outputDir );
        converter->setTempDirectory ( d->tempDir.absolutePath() );
        converter->setSerieUID ( serie );
        converter->convert();
        delete converter;

        if ( number == this->seriesToImportSize() - 1 )
            emit importFinished();
    }
}

void QtDcmManager::moveSeriesFinished()
{
    if ( d->importWidget )
        d->importWidget->importProgressBar->setValue ( 0 );
}

void QtDcmManager::updateProgressBar ( int i )
{
    if ( d->importWidget )
        d->importWidget->importProgressBar->setValue ( i );
    qApp->processEvents();
}

void QtDcmManager::createTemporaryDirs()
{
    //Creation d'un répertoire temporaire pour la série
    QDir tempDir = QDir ( QDir::tempPath() ); //tempDir = /tmp

    //Generer un nom de répertoire aléatoire
    QString acceptes = "abcdefghijklmnopqrstuvwyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
    QString randName;
    qsrand ( time ( NULL ) );

    for ( int i = 0; i < 6; i++ )
        randName += acceptes[qrand() % 59];

    if ( !tempDir.exists ( "qtdcm" + randName ) )
        tempDir.mkdir ( "qtdcm" + randName );

    d->tempDir = QDir ( QDir::tempPath() + QDir::separator() + "qtdcm" + randName ); // tempDir = /tmp/qtdcm

    if ( !d->tempDir.exists ( "logs" ) )
    {
        if ( !d->tempDir.mkdir ( "logs" ) )
            qDebug() << tr ( "Repertoire logs non cree" );
    }

    d->logsDir = QDir ( d->tempDir.absolutePath() + QDir::separator() + "logs" );
}

void QtDcmManager::deleteTemporaryDirs()
{
    QStringList listLogs = d->logsDir.entryList ( QDir::Files, QDir::Name );

    for ( int i = 0; i < listLogs.size(); i++ )
    {
        d->logsDir.remove ( listLogs.at ( i ) );
    }

    d->tempDir.rmdir ( "logs" );

    QStringList listSerie = d->tempDir.entryList ( QDir::Dirs, QDir::Name );

    for ( int i = 0; i < listSerie.size(); i++ )
    {
        if ( ! ( listSerie.at ( i ) == "." || listSerie.at ( i ) == ".." ) )
        {
            QDir serieDir ( d->tempDir.absolutePath() + QDir::separator() + listSerie.at ( i ) );
            QStringList listFiles = serieDir.entryList ( QDir::Files, QDir::Name );

            for ( int j = 0; j < listFiles.size(); j++ )
            {
                serieDir.remove ( listFiles.at ( j ) );
            }

            d->tempDir.rmdir ( listSerie.at ( i ) );
        }
    }

    QDir ( QDir::tempPath() ).rmdir ( d->tempDir.dirName() );
}

void QtDcmManager::generateCurrentSerieDir()
{
    if ( !d->serieId.isEmpty() )
    {
        d->currentSerieDir = d->tempDir.absolutePath() + QDir::separator() + d->serieId;
        d->tempDir.mkdir ( d->serieId );
    }
}

void QtDcmManager::deleteCurrentSerieDir()
{
    // Suppression des fichiers temporaires
    QStringList listFiles = d->currentSerieDir.entryList ( QDir::Files, QDir::Name );

    for ( int i = 0; i < listFiles.size(); i++ )
    {
        d->currentSerieDir.remove ( listFiles.at ( i ) );
    }

    // Suppression du répertoire temporaire
    if ( !d->tempDir.rmdir ( d->serieId ) )
        qDebug() << tr ( "Probleme lors de la suppression du répertoire temporaire" );
}

void QtDcmManager::makePreview ( QString filename )
{
    DcmRLEDecoderRegistration::registerCodecs ( OFFalse, OFFalse );
    DJDecoderRegistration::registerCodecs ( EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse );
    DcmFileFormat file;
    file.loadFile ( filename.toLatin1().data() );
    DcmDataset * dset = file.getDataset();
    DicomImage* dcimage = new DicomImage ( dset, file.getDataset()->getOriginalXfer(), CIF_MayDetachPixelData );


    if ( dcimage != NULL )
    {
        dcimage->setNoDisplayFunction();
        dcimage->hideAllOverlays();
        dcimage->setNoVoiTransformation();

        if ( dcimage->getStatus() == EIS_Normal )
        {
            Uint32 *pixelData = ( Uint32 * ) ( dcimage->getOutputData ( 32 /* bits per sample */ ) );

            if ( pixelData != NULL )
            {
                Uint8 *colored = new Uint8[dcimage->getWidth() * dcimage->getHeight() * 4]; //4 * dcimage->getWidth() * dcimage->getHeight() matrix
                Uint8 *col = colored;
                Uint32 *p = pixelData;
                //get the highest values for RGBA, then use them to scale the pixel luminosity
                Uint32 p_max = 0;
                Uint32 p_min = std::numeric_limits<Uint32>::max();

                for ( unsigned i = 0; i < dcimage->getWidth(); ++i )
                    for ( unsigned j = 0; j < dcimage->getHeight(); ++j, ++p )
                    {
                        if ( *p > p_max )
                            p_max = *p;

                        if ( *p < p_min )
                            p_min = *p;
                    }

                double a = 4294967295.f / ( ( double ) p_max - ( double ) p_min );

                //re-initialize 'col'
                p = pixelData;
                //copy the pixels in our QImage

                for ( unsigned i = 0; i < dcimage->getWidth(); ++i )
                    for ( unsigned j = 0; j < dcimage->getHeight(); ++j, ++p )
                    {
                        *col = ( Uint8 ) ( ( 255.f / 4294967295.f ) * ( a * ( ( double ) ( *p ) - ( double ) p_min ) ) );
                        ++col;
                        *col = ( Uint8 ) ( ( 255.f / 4294967295.f ) * ( a * ( ( double ) ( *p ) - ( double ) p_min ) ) );
                        ++col;
                        *col = ( Uint8 ) ( ( 255.f / 4294967295.f ) * ( a * ( ( double ) ( *p ) - ( double ) p_min ) ) );
                        ++col;
                        *col = 255;
                        ++col;
                    }

                QImage image ( colored, dcimage->getWidth(), dcimage->getHeight(), QImage::Format_ARGB32 );

                if ( d->previewWidget )
                    d->previewWidget->imageLabel->setPixmap ( QPixmap::fromImage ( image.scaled ( 130,130 ), Qt::AutoColor ) );

                delete[] colored;

            }
        }

        delete dcimage;
    }
}

// Getters and setters
QString QtDcmManager::getDicomdir() const
{
    return d->dicomdir;
}

void QtDcmManager::setDicomdir ( QString dicomdir )
{
    d->dicomdir = dicomdir;
    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;

    if ( ! ( status = d->dfile.loadFile ( d->dicomdir.toUtf8().data() ) ).good() )
    {
        return;
    }
}

QString QtDcmManager::getOutputDirectory() const
{
    return d->outputDir;
}

void QtDcmManager::setOutputDirectory ( QString directory )
{
    d->outputDir = directory;
}

QtDcmServer * QtDcmManager::getCurrentPacs()
{
    return d->currentPacs;
}

void QtDcmManager::setCurrentPacs ( int index )
{
    if ( index < QtDcmPreferences::instance()->getServers().size() )
        d->currentPacs = QtDcmPreferences::instance()->getServers().at ( index );
}

QString QtDcmManager::getPatientName()
{
    return d->patientName;
}

void QtDcmManager::setPatientName ( QString patientName )
{
    d->patientName = patientName;
}

QString QtDcmManager::getPatientId()
{
    return d->patientId;
}

void QtDcmManager::setPatientId ( QString patientId )
{
    d->patientId = patientId;
}

QString QtDcmManager::getPatientBirthDate()
{
    QString birthdate;
    if ( d->patientsTreeWidget )
        if ( d->patientsTreeWidget->currentItem() )
            birthdate = d->patientsTreeWidget->currentItem()->data ( 2,0 ).toString();
    qDebug() << birthdate;
    return birthdate;
}

QString QtDcmManager::getPatientSex()
{
    QString sex ( "" );
    if ( d->patientsTreeWidget )
        if ( d->patientsTreeWidget->currentItem() )
            sex = d->patientsTreeWidget->currentItem()->data ( 3,0 ).toString();
    qDebug() << sex;
    return sex;
}

QString QtDcmManager::getExamDate()
{
    QString examDate;
    if ( d->studiesTreeWidget )
        if ( d->studiesTreeWidget->currentItem() )
            examDate = d->studiesTreeWidget->currentItem()->data ( 1,0 ).toString();
    qDebug() << examDate;
    return examDate;
}

void QtDcmManager::setPatientSex ( QString sex )
{
    d->patientSex = sex;
}

QString QtDcmManager::getSerieDescription()
{
    return d->serieDescription;
}

void QtDcmManager::setSerieDescription ( QString serieDescription )
{
    d->serieDescription = serieDescription;
}

QString QtDcmManager::getStudyDescription()
{
    return d->studyDescription;
}

void QtDcmManager::setStudyDescription ( QString studyDescription )
{
    d->studyDescription = studyDescription;
}

void QtDcmManager::setModality ( QString modality )
{
    d->modality = modality;
}

QString QtDcmManager::getModality()
{
    return d->modality;
}

void QtDcmManager::setDate1 ( QString date )
{
    d->date1 = date;
}

QString QtDcmManager::getDate1()
{
    return d->date1;
}

void QtDcmManager::setDate2 ( QString date )
{
    d->date2 = date;
}

QString QtDcmManager::getDate2()
{
    return d->date2;
}

void QtDcmManager::addPatient()
{
    d->patients.append ( new QtDcmPatient() );
}

QString QtDcmManager::getMode()
{
    return d->mode;
}

void QtDcmManager::setImagesList ( QList<QString> images )
{
    d->images = images;
}

QList<QString> QtDcmManager::getListImages()
{
    return d->listImages;
}

void QtDcmManager::clearListImages()
{
    d->listImages.clear();
    d->mapImages.clear();
}

void QtDcmManager::setSerieId ( QString id )
{
    d->serieId = id;
}

QString QtDcmManager::getCurrentSerieDirectory()
{
    return d->currentSerieDir.absolutePath();
}

void QtDcmManager::setSeriesToExport ( QMap<QString, QList<QString> > seriesToExport )
{
    d->seriesToExport = seriesToExport;
}

void QtDcmManager::setQuery ( QByteArray query )
{
    d->query = query;
}

void QtDcmManager::setPreviewImageUID ( QString uid )
{
    d->previewImageUID = uid;
}

void QtDcmManager::addSerieToImport ( QString uid )
{
    if ( !d->seriesToImport.contains ( uid ) )
        d->seriesToImport.append ( uid );
}

void QtDcmManager::removeSerieToImport ( QString uid )
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


