/*
 * QtDcmManager.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#define QT_NO_CAST_TO_ASCII
//#define HAVE_CLASS_TEMPLATE
//#define HAVE_STL
//#define HAVE_STD_NAMESPACE
//#define HAVE_SSTREAM
//#define USE_STD_CXX_INCLUDES
//#define HAVE_CXX_BOOL
//
//#define DCM_DICT_DEFAULT_PATH
#include <algorithm>
#include <iostream>
#include <limits>

//#ifdef WITHd->ZLIB
//#include <zlib.h>        /* for zlibVersion() */
//#endif
#include <QtDcm.h>
#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmImage.h>
#include <QtDcmServer.h>
#include <QtDcmPreferences.h>
#include <QtDcmExportThread.h>
#include <QtDcmQueryThread.h>
#include <QtDcmFindCallback.h>

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
// From Dcmtk:
#include <dcmtk/config/osconfig.h>    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcistrmz.h>    /* for dcmZlibExpectRFC1950Encoding */
// For dcm images
#include <dcmtk/dcmimgle/dcmimage.h>
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
// For color images
#include <dcmtk/dcmimage/diregist.h>

#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QtDcmFindScu.h>
#include <QtDcmFindDicomdir.h>
#include <QtDcmMoveScu.h>
#include <QtDcmMoveDicomdir.h>
#include <QtDcmConvert.h>

#include <QtDcmManager.h>

class QtDcmManagerPrivate {
public:
    QWidget * parent; /** Here the parent is corresponding to the QtDCM object */
    //        QProgressDialog * progress; /** Dialog window showing file copy in progress */
    QString dicomdir; /** Dicomdir absolute file path */
    QString outputDir; /** Output directory for reconstructed serie absolute path */
    QDir currentSerieDir; /** Directory containing current serie dicom slice */
    QDir tempDir; /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
    QDir logsDir; /** Directory of the reconstruction process logs file (/tmp/qtdcm/logs) */
    DcmFileFormat dfile; /** This attribute is usefull for parsing the dicomdir */
    QList<QtDcmPatient *> patients; /** List that contains patients resulting of a query or read from a CD */
    QList<QString> images; /** List of image filename to export from a CD */
    QMap<QString, QList<QString> > seriesToExport;
    QList<QImage> listImages;
    QList<QString> seriesToImport;
    QString serieId; /** Id of the serie to export from the PACS */
    QProcess * process; /** This attribute launch the reconstruction process */
    QtDcmPreferences * preferences; /** Attribute that give access to the Pacs settings */
    QString patientName; /** Attribute representing the patient name used for query PACS */
    QString patientId; /** Attribute representing the patient id used for query PACS */
    QString patientSex;
    QString modality; /** Attibute for the modality of the search (MR, US, CT, etc) */
    QString date1; /** Attribute for the begin date of the query (usefull for date based queries) */
    QString date2; /** Attribute for the end date of the query (usefull for date based queries) */
    QString serieDescription; /** Attibute representing the serie description used for query PACS */
    QString studyDescription; /** Attibute representing the study description used for query PACS */
    QString mode; /** Mode that determine the type of media (CD or PACS) */
    QString dcm2nii; /** Absolute filename of the dcm2nii program */
    QString dcm4che; /** Absolute filename of the dcm4che program */
    QtDcmExportThread * exportThread;
    QtDcmQueryThread * queryThread;
    QByteArray query;
    QString previewImageUID;

    QtDcmServer * currentPacs;

    QTreeWidget * patientsTreeWidget;
    QTreeWidget * studiesTreeWidget;
    QTreeWidget * seriesTreeWidget;
    QProgressBar * progress;
};

QtDcmManager::QtDcmManager() :
        d ( new QtDcmManagerPrivate ) {
    //Initialization of the private attributes
    d->mode = "PACS";
    d->dicomdir = "";
    d->outputDir = "";
    d->patientName = "*";
    d->patientId = "*";
    d->modality = "*";
    d->date1 = "*";
    d->date2 = "*";
    d->serieDescription = "*";
    d->studyDescription = "*";
    d->patientSex = "*";

    d->patientsTreeWidget = NULL;
    d->studiesTreeWidget = NULL;
    d->seriesTreeWidget = NULL;
    d->progress = NULL;

    d->preferences = new QtDcmPreferences();
    d->exportThread = new QtDcmExportThread();
    d->queryThread = new QtDcmQueryThread();

    d->currentPacs = d->preferences->getServers().at ( 0 );

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
}

QtDcmManager::QtDcmManager ( QWidget * parent ) :
        d ( new QtDcmManagerPrivate ) {
    //Initialization of the private attributes
    d->mode = "PACS";
    d->dicomdir = "";
    d->outputDir = "";
    d->patientName = "*";
    d->patientId = "*";
    d->patientSex = "*";
    d->date1 = "*";
    d->date2 = "*";
    d->modality = "*";
    d->serieDescription = "*";
    d->studyDescription = "*";
    d->preferences = new QtDcmPreferences();
    d->parent = dynamic_cast<QtDCM *> (parent);

    d->patientsTreeWidget = NULL;
    d->studiesTreeWidget = NULL;
    d->seriesTreeWidget = NULL;
    d->progress = NULL;

    d->exportThread = new QtDcmExportThread();
    d->queryThread = new QtDcmQueryThread();

    d->currentPacs = d->preferences->getServers().at ( 0 );

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
}

QtDcmManager::~QtDcmManager() {
    this->deleteTemporaryDirs();
    delete d->exportThread;
    delete d->queryThread;
    delete d->preferences;
}

void
QtDcmManager::setPatientsTreeWidget ( QTreeWidget * widget ) {
    d->patientsTreeWidget = widget;
}

void
QtDcmManager::setStudiesTreeWidget ( QTreeWidget * widget ) {
    d->studiesTreeWidget = widget;
}

void
QtDcmManager::setSeriesTreeWidget ( QTreeWidget * widget ) {
    d->seriesTreeWidget = widget;
}

void
QtDcmManager::setProgressBar ( QProgressBar * progress ) {
    d->progress = progress;
}

void
QtDcmManager::displayErrorMessage ( QString message ) {
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox ( d->parent );
    msgBox->setIcon ( QMessageBox::Critical );
    msgBox->setText ( message );
    msgBox->exec();
    delete msgBox;
}

void
QtDcmManager::displayMessage ( QString info ) {
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox ( d->parent );
    msgBox->setIcon ( QMessageBox::Information );
    msgBox->setText ( info );
    msgBox->exec();
    delete msgBox;
}

void
QtDcmManager::findPatientsScu() {
    d->seriesToImport.clear();
    d->mode = "PACS";

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findPatientsScu ( d->patientName, d->patientSex );
    delete finder;
}

void
QtDcmManager::findStudiesScu ( QString patientName ) {
    d->seriesToImport.clear();

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findStudiesScu ( patientName, d->studyDescription, d->date1, d->date2 );
    delete finder;
}

void
QtDcmManager::findSeriesScu ( QString patientName, QString studyDescription ) {
    d->seriesToImport.clear();

    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findSeriesScu ( patientName, studyDescription, d->serieDescription, d->modality );
    delete finder;
}

void
QtDcmManager::findImagesScu ( QString serieInstanceUID ) {
    QtDcmFindScu * finder = new QtDcmFindScu ( this );
    finder->findImagesScu ( serieInstanceUID );
    delete finder;
}

void
QtDcmManager::foundPatient ( QMap<QString, QString> infosMap ) {
    if ( d->patientsTreeWidget ) {
        QTreeWidgetItem * patientItem = new QTreeWidgetItem ( d->patientsTreeWidget->invisibleRootItem() );
        patientItem->setText ( 0, infosMap["Name"] );
        patientItem->setText ( 1, infosMap["ID"] );
        patientItem->setText ( 2, QDate::fromString ( infosMap["Birthdate"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        patientItem->setText ( 3, infosMap["Sex"] );
    }
}

void
QtDcmManager::foundStudy ( QMap<QString, QString> infosMap ) {
    if ( d->studiesTreeWidget ) {
        QTreeWidgetItem * studyItem = new QTreeWidgetItem ( d->studiesTreeWidget->invisibleRootItem() );
        studyItem->setText ( 0, infosMap["Description"] );
        studyItem->setText ( 1, QDate::fromString ( infosMap["Date"], "yyyyMMdd" ).toString ( "dd/MM/yyyy" ) );
        studyItem->setText ( 2, infosMap["ID"] );
    }
}

void
QtDcmManager::foundSerie ( QMap<QString, QString> infosMap ) {
    if ( d->seriesTreeWidget ) {
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

void
QtDcmManager::foundImage ( QMap<QString, QString> infosMap ) {
//     QtDcmManager::foundImage ( QString uid) {
    if ( d->seriesTreeWidget ) {
        if ( infosMap["InstanceCount"].toInt() > d->seriesTreeWidget->currentItem()->data ( 4, 0 ).toInt() )
            d->seriesTreeWidget->currentItem()->setData ( 4, 0, QVariant ( infosMap["InstanceCount"] ) );
    }
}

void
QtDcmManager::loadDicomdir() {
    if ( d->dicomdir.isEmpty() )
        return;
    d->mode = "CD";
    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;
    if ( ! ( status = d->dfile.loadFile ( d->dicomdir.toUtf8().data() ) ).good() ) {
        return;
    }
    this->findPatientsDicomdir();
}

void
QtDcmManager::findPatientsDicomdir() {
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findPatients();
    delete finder;
}

void
QtDcmManager::findStudiesDicomdir ( QString patientName ) {
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findStudies ( patientName );
    delete finder;
}

void
QtDcmManager::findSeriesDicomdir ( QString patientName, QString studyDescription ) {
    d->seriesToImport.clear();
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findSeries ( patientName, studyDescription );
    delete finder;
}

void
QtDcmManager::findImagesDicomdir ( QString uid ) {
    QtDcmFindDicomdir * finder = new QtDcmFindDicomdir ( this );
    finder->setDcmItem ( d->dfile.getDataset() );
    finder->findImages ( uid );
    delete finder;
}

void
QtDcmManager::moveSelectedSeries() {
    if ( !d->tempDir.exists() )
        return;
    d->progress->show();
    qApp->processEvents();
    if ( d->mode == "CD" ) {
        QtDcmMoveDicomdir * mover = new QtDcmMoveDicomdir ( this );
        mover->setMode(QtDcmMoveDicomdir::IMPORT);
        mover->setDcmItem ( d->dfile.getDataset() );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setImportDir(d->outputDir);
        mover->setSeries ( d->seriesToImport );
        QObject::connect ( mover, SIGNAL ( updateProgress ( int ) ), this, SLOT ( updateProgressBar ( int ) ) );
        QObject::connect ( mover, SIGNAL ( finished() ), this, SLOT ( moveSeriesFinished() ) );
        mover->start();
    } else {
        QtDcmMoveScu * mover = new QtDcmMoveScu ( this );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( d->seriesToImport );
        mover->setImportDir(d->outputDir);
        QObject::connect ( mover, SIGNAL ( updateProgress ( int ) ), this, SLOT ( updateProgressBar ( int ) ) );
        QObject::connect ( mover, SIGNAL ( finished() ), this, SLOT ( moveSeriesFinished() ) );
        mover->start();
    }
}

void QtDcmManager::getPreviewFromSelectedSerie(QString uid, int elementIndex)
{
    if ( !d->tempDir.exists() )
        return;
    if ( d->mode == "CD" ) {
        QtDcmMoveDicomdir * mover = new QtDcmMoveDicomdir ( this );
        mover->setMode(QtDcmMoveDicomdir::PREVIEW);
        mover->setDcmItem ( d->dfile.getDataset() );
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( QStringList() << uid );
        mover->setIndex(elementIndex);
        QObject::connect ( mover, SIGNAL ( previewSlice(QString) ), this, SLOT ( makePreview(QString) ) );
        mover->start();
    }
    else
    {
        QtDcmFindScu * find = new QtDcmFindScu(this);
        find->findImageScu(uid, QString::number(elementIndex));
        delete find;
        QtDcmMoveScu * mover = new QtDcmMoveScu ( this );
        mover->setMode(QtDcmMoveScu::PREVIEW);
        mover->setOutputDir ( d->tempDir.absolutePath() );
        mover->setSeries ( QStringList() << uid );
        mover->setImageId(d->previewImageUID);
        QObject::connect ( mover, SIGNAL ( previewSlice(QString) ), this, SLOT ( makePreview(QString) ) );
        mover->start();
    }
    return;
}

void
QtDcmManager::moveSeriesFinished() {
    d->progress->setValue ( 100 );
    d->progress->hide();
    d->progress->setValue ( 0 );
}

void
QtDcmManager::updateProgressBar ( int i ) {
    d->progress->setValue ( i );
    qApp->processEvents();
}

void
QtDcmManager::createTemporaryDirs() {
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
    if ( !d->tempDir.exists ( "logs" ) ) {
        if ( !d->tempDir.mkdir ( "logs" ) )
            qDebug() << tr ( "Repertoire logs non cree" );
    }
    d->logsDir = QDir ( d->tempDir.absolutePath() + QDir::separator() + "logs" );
}

void
QtDcmManager::deleteTemporaryDirs() {
    QStringList listLogs = d->logsDir.entryList ( QDir::Files, QDir::Name );
    for ( int i = 0; i < listLogs.size(); i++ ) {
        d->logsDir.remove ( listLogs.at ( i ) );
    }
    d->tempDir.rmdir ( "logs" );
    QStringList listSerie = d->tempDir.entryList ( QDir::Dirs, QDir::Name );
    for ( int i = 0; i < listSerie.size(); i++ ) {
        if ( ! ( listSerie.at ( i ) == "." || listSerie.at ( i ) == ".." ) ) {
            QDir serieDir ( d->tempDir.absolutePath() + QDir::separator() + listSerie.at ( i ) );
            QStringList listFiles = serieDir.entryList ( QDir::Files, QDir::Name );
            for ( int j = 0; j < listFiles.size(); j++ ) {
                serieDir.remove ( listFiles.at ( j ) );
            }
            d->tempDir.rmdir ( listSerie.at ( i ) );
        }
    }
    QDir ( QDir::tempPath() ).rmdir ( d->tempDir.dirName() );
}

void
QtDcmManager::generateCurrentSerieDir() {
    if ( !d->serieId.isEmpty() ) {
        d->currentSerieDir = d->tempDir.absolutePath() + QDir::separator() + d->serieId;
        d->tempDir.mkdir ( d->serieId );
    }
}

void
QtDcmManager::deleteCurrentSerieDir() {
    // Suppression des fichiers temporaires
    QStringList listFiles = d->currentSerieDir.entryList ( QDir::Files, QDir::Name );
    for ( int i = 0; i < listFiles.size(); i++ ) {
        d->currentSerieDir.remove ( listFiles.at ( i ) );
    }
    // Suppression du répertoire temporaire
    if ( !d->tempDir.rmdir ( d->serieId ) )
        qDebug() << tr ( "Probleme lors de la suppression du répertoire temporaire" );
}

void
QtDcmManager::makePreview(QString filename) {
    DcmRLEDecoderRegistration::registerCodecs ( OFFalse, OFFalse );
    DJDecoderRegistration::registerCodecs ( EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse );
    DcmFileFormat file;
    file.loadFile ( filename.toLatin1().data() );
    DcmDataset * dset = file.getDataset();
    DicomImage* dcimage = new DicomImage ( dset, file.getDataset()->getOriginalXfer(), CIF_MayDetachPixelData );

    dcimage->setNoDisplayFunction();
    dcimage->hideAllOverlays();
    dcimage->setNoVoiTransformation();

    //        DicomImage * dcimage = new DicomImage((d->currentSerieDir.absolutePath() + QDir::separator() + list.at(i)).toLatin1().data());
    if ( dcimage != NULL ) {
        if ( dcimage->getStatus() == EIS_Normal ) {
            Uint32 *pixelData = ( Uint32 * ) ( dcimage->getOutputData ( 32 /* bits per sample */ ) );
            if ( pixelData != NULL ) {
                Uint8 *colored = new Uint8[dcimage->getWidth() * dcimage->getHeight() * 4]; //4 * dcimage->getWidth() * dcimage->getHeight() matrix
                Uint8 *col = colored;
                Uint32 *p = pixelData;
                //get the highest values for RGBA, then use them to scale the pixel luminosity
                Uint32 p_max = 0;
                Uint32 p_min = std::numeric_limits<Uint32>::max();
                for ( unsigned i = 0; i < dcimage->getWidth(); ++i )
                    for ( unsigned j = 0; j < dcimage->getHeight(); ++j, ++p ) {
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
                QImage image(colored, dcimage->getWidth(), dcimage->getHeight(), QImage::Format_ARGB32 );
                dynamic_cast<QtDCM*>(d->parent)->getPreviewLabel()->setPixmap(QPixmap::fromImage(image.scaled(130,130), Qt::AutoColor));
            }
        }
    }
}
// Getters and setters
QString
QtDcmManager::getDicomdir() const {
    return d->dicomdir;
}

void
QtDcmManager::setDicomdir ( QString dicomdir ) {
    d->dicomdir = dicomdir;
    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;
    if ( ! ( status = d->dfile.loadFile ( d->dicomdir.toUtf8().data() ) ).good() ) {
        return;
    }
}

QString
QtDcmManager::getOutputDirectory() const {
    return d->outputDir;
}

void
QtDcmManager::setOutputDirectory ( QString directory ) {
    d->outputDir = directory;
}

QtDcmPreferences *
QtDcmManager::getPreferences() {
    return d->preferences;
}

void
QtDcmManager::setPreferences ( QtDcmPreferences * prefs ) {
    d->preferences = prefs;
}

QtDcmServer *
QtDcmManager::getCurrentPacs() {
    return d->currentPacs;
}

void
QtDcmManager::setCurrentPacs ( int index ) {
    if ( index < d->preferences->getServers().size() )
        d->currentPacs = d->preferences->getServers().at ( index );
}

QString
QtDcmManager::getPatientName() {
    return d->patientName;
}

void
QtDcmManager::setPatientName ( QString patientName ) {
    d->patientName = patientName;
}

QString
QtDcmManager::getPatientId() {
    return d->patientId;
}

void
QtDcmManager::setPatientId ( QString patientId ) {
    d->patientId = patientId;
}

void
QtDcmManager::setPatientSex ( QString sex ) {
    d->patientSex = sex;
}

QString
QtDcmManager::getSerieDescription() {
    return d->serieDescription;
}

void
QtDcmManager::setSerieDescription ( QString serieDescription ) {
    d->serieDescription = serieDescription;
}

QString
QtDcmManager::getStudyDescription() {
    return d->studyDescription;
}

void
QtDcmManager::setStudyDescription ( QString studyDescription ) {
    d->studyDescription = studyDescription;
}

void
QtDcmManager::setModality ( QString modality ) {
    d->modality = modality;
}

QString
QtDcmManager::getModality() {
    return d->modality;
}

void
QtDcmManager::setDate1 ( QString date ) {
    d->date1 = date;
}

QString
QtDcmManager::getDate1() {
    return d->date1;
}

void
QtDcmManager::setDate2 ( QString date ) {
    d->date2 = date;
}

QString
QtDcmManager::getDate2() {
    return d->date2;
}

void
QtDcmManager::addPatient() {
    d->patients.append ( new QtDcmPatient() );
}

QString
QtDcmManager::getMode() {
    return d->mode;
}

void
QtDcmManager::setImagesList ( QList<QString> images ) {
    d->images = images;
}

QList<QImage>
QtDcmManager::getListImages() {
    return d->listImages;
}

void
QtDcmManager::setSerieId ( QString id ) {
    d->serieId = id;
}

QString
QtDcmManager::getCurrentSerieDirectory() {
    return d->currentSerieDir.absolutePath();
}

void
QtDcmManager::setSeriesToExport ( QMap<QString, QList<QString> > seriesToExport ) {
    d->seriesToExport = seriesToExport;
}

void
QtDcmManager::setQuery ( QByteArray query ) {
    d->query = query;
}

void QtDcmManager::setPreviewImageUID(QString uid)
{
    d->previewImageUID = uid;
}

void
QtDcmManager::addSerieToImport ( QString uid ) {
    if ( !d->seriesToImport.contains ( uid ) )
        d->seriesToImport.append ( uid );
}

void
QtDcmManager::removeSerieToImport ( QString uid ) {
    if ( d->seriesToImport.contains ( uid ) )
        d->seriesToImport.removeOne ( uid );
}

void
QtDcmManager::clearSeriesToImport() {
    d->seriesToImport.clear();
}

int
QtDcmManager::seriesToImportSize() {
    return d->seriesToImport.size();
}
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
