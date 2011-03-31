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

#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmImage.h>
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
#include "dcmtk/dcmnet/dfindscu.h"
#include <dcmtk/dcmdata/dcistrmz.h>    /* for dcmZlibExpectRFC1950Encoding */
// For dcm images
#include <dcmtk/dcmimgle/dcmimage.h>
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
// For color images
#include <dcmtk/dcmimage/diregist.h>

//#define INCLUDEd->CSTDLIB
//#define INCLUDEd->CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QtDcmFindScuSignalManager.h>

#include <QtDcmManager.h>

class QtDcmManagerPrivate
{
    public:
        QWidget * parent; /** Here the parent is corresponding to the QtDCM object */
        QProgressDialog * progress; /** Dialog window showing file copy in progress */
        QString dicomdir; /** Dicomdir absolute file path */
        QString outputDir; /** Output directory for reconstructed serie absolute path */
        QDir currentSerieDir; /** Directory containing current serie dicom slice */
        QDir tempDir; /** Qtdcm temporary directory (/tmp/qtdcm on Unix) */
        QDir logsDir; /** Directory of the reconstruction process logs file (/tmp/qtdcm/logs) */
        DcmItem * dcmObject; /** This attribute is usefull for parsing the dicomdir */
        DcmFileFormat dfile; /** This attribute is usefull for parsing the dicomdir */
        QList<QtDcmPatient *> patients; /** List that contains patients resulting of a query or read from a CD */
        QList<QString> images; /** List of image filename to export from a CD */
        QMap<QString, QList<QString> > seriesToExport;
        QList<QImage> listImages;
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

        QtDcmFindScuSignalManager * signalManager;
};

QtDcmManager::QtDcmManager() :
    d(new QtDcmManagerPrivate)
{
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
    d->patientSex="*";

    d->preferences = new QtDcmPreferences();
    d->exportThread = new QtDcmExportThread();
    d->queryThread = new QtDcmQueryThread();

    d->signalManager = new QtDcmFindScuSignalManager(this);

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
}

QtDcmManager::QtDcmManager(QWidget * parent) :
    d(new QtDcmManagerPrivate)
{
    //Initialization of the private attributes
    d->mode = "PACS";
    d->dicomdir = "";
    d->outputDir = "";
    d->patientName = "*";
    d->patientId = "*";
    d->patientSex="*";
    d->date1 = "*";
    d->date2 = "*";
    d->modality = "*";
    d->serieDescription = "*";
    d->studyDescription = "*";
    d->preferences = new QtDcmPreferences();
    d->parent = parent;

    d->exportThread = new QtDcmExportThread();
    d->queryThread = new QtDcmQueryThread();

    d->signalManager = new QtDcmFindScuSignalManager(this);

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
}

QtDcmManager::~QtDcmManager()
{
    this->deleteTemporaryDirs();
    delete d->exportThread;
    delete d->queryThread;
    delete d->preferences;
}

void
QtDcmManager::setPatientsTreeWidget(QTreeWidget * widget)
{
    d->signalManager->attachPatientsTreeWidget(widget);
}

void
QtDcmManager::setStudiesTreeWidget(QTreeWidget * widget)
{
    d->signalManager->attachStudiesTreeWidget(widget);
}

void
QtDcmManager::setSeriesTreeWidget(QTreeWidget * widget)
{
    d->signalManager->attachSeriesTreeWidget(widget);
}

void
QtDcmManager::displayErrorMessage(QString message)
{
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox(d->parent);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->setText(message);
    msgBox->exec();
    delete msgBox;
}

void
QtDcmManager::displayMessage(QString info)
{
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox(d->parent);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText(info);
    msgBox->exec();
    delete msgBox;
}

void
QtDcmManager::sendEchoRequest()
{
    T_ASC_Network *net; // network struct, contains DICOM upper layer FSM etc.
    ASC_initializeNetwork(NET_REQUESTOR, 0, 1000 /* timeout */, &net);

    T_ASC_Parameters *params; // parameters of association request
    ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);

    // set calling and called AE titles
    ASC_setAPTitles(params, d->preferences->getAetitle().toUtf8().data(), d->preferences->getServers()[0]->getAetitle().toUtf8().data(), NULL);

    // the DICOM server accepts connections at server.nowhere.com port 104
    ASC_setPresentationAddresses(params, d->preferences->getHostname().toUtf8().data(), QString(d->preferences->getServers()[0]->getServer() + ":" + d->preferences->getServers()[0]->getPort()).toAscii().data());

    // list of transfer syntaxes, only a single entry here
    const char* ts[] = { UID_LittleEndianImplicitTransferSyntax };

    // add presentation context to association request
    ASC_addPresentationContext(params, 1, UID_VerificationSOPClass, ts, 1);

    // request DICOM association
    T_ASC_Association *assoc;
    if (ASC_requestAssociation(net, params, &assoc).good()) {
        if (ASC_countAcceptedPresentationContexts(params) == 1) {
            // the remote SCP has accepted the Verification Service Class
            DIC_US id = assoc->nextMsgID++; // generate next message ID
            DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
            DcmDataset *sd = NULL; // status detail will be stored here
            // send C-ECHO-RQ and handle response
            DIMSE_echoUser(assoc, id, DIMSE_BLOCKING, 0, &status, &sd);

            delete sd; // we don't care about status detail
            this->displayMessage("Echo request successful !");

        }
        else
            this->displayErrorMessage("Wrong presentation context, echo request failed");
    }
    else
        this->displayErrorMessage("Wrong dicom association, echo request failed");

    ASC_releaseAssociation(assoc); // release association
    ASC_destroyAssociation(&assoc); // delete assoc structure
    ASC_dropNetwork(&net); // delete net structure
}

void
QtDcmManager::findPatientScu()
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back((QString("QueryRetrieveLevel=") + QString("" "PATIENT" "")).toUtf8().data());
    overrideKeys.push_back((QString("PatientName=") + d->patientName).toUtf8().data());

    //Patient level
    overrideKeys.push_back(QString("PatientID").toUtf8().data());
    overrideKeys.push_back(QString("PatientSex=" + d->patientSex).toUtf8().data());
    overrideKeys.push_back(QString("PatientBirthDate").toUtf8().data());

    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;
    OFCondition cond;

    QtDcmFindCallback * callback = new QtDcmFindCallback();
    callback->setSignalManager(d->signalManager);

    if (findscu.initializeNetwork(30).bad())
        this->displayErrorMessage(tr("Cannot establish network connection"));

    if (findscu.performQuery(d->preferences->getServers()[0]->getServer().toUtf8().data(),
                             d->preferences->getServers()[0]->getPort().toInt(),
                             d->preferences->getAetitle().toUtf8().data(),
                             d->preferences->getServers()[0]->getAetitle().toUtf8().data(),
                             UID_FINDPatientRootQueryRetrieveInformationModel,
                             EXS_Unknown,
                             DIMSE_BLOCKING,
                             0,
                             ASC_DEFAULTMAXPDU,
                             false,
                             false,
                             1,
                             false,
                             -1,
                             &overrideKeys,
                             callback,
                             &fileNameList).bad())
        this->displayErrorMessage(tr("Cannot perform query C-FIND"));

    if (findscu.dropNetwork().bad())
        this->displayErrorMessage(tr("Cannot drop network"));
}

void
QtDcmManager::findStudiesScu(QString patientName)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back((QString("QueryRetrieveLevel=") + QString("" "STUDY" "")).toUtf8().data());
    overrideKeys.push_back((QString("PatientName=") + patientName).toUtf8().data());
    overrideKeys.push_back((QString("StudyDescription=") + d->studyDescription).toUtf8().data());
    overrideKeys.push_back(QString("StudyDate=" + d->date1 + "-" + d->date2).toUtf8().data());
    overrideKeys.push_back((QString("SeriesDescription=") + d->serieDescription).toUtf8().data());

    //Study level
    overrideKeys.push_back(QString("StudyID").toUtf8().data());
    overrideKeys.push_back(QString("StudyTime").toUtf8().data());

    //Serie level
    overrideKeys.push_back(QString("SeriesInstanceUID").toUtf8().data());
    overrideKeys.push_back(QString("Modality").toUtf8().data());
    overrideKeys.push_back(QString("InstitutionName").toUtf8().data());
    overrideKeys.push_back(QString("InstitutionAddress").toUtf8().data());
    overrideKeys.push_back(QString("PerformingPhysicianName").toUtf8().data());

    //Image level
    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;
    OFCondition cond;

    QtDcmFindCallback * callback = new QtDcmFindCallback(QtDcmFindCallback::STUDY);
    callback->setSignalManager(d->signalManager);

    if (findscu.initializeNetwork(30).bad())
        this->displayErrorMessage(tr("Cannot establish network connection"));

    if (findscu.performQuery(d->preferences->getServers()[0]->getServer().toUtf8().data(),
                             d->preferences->getServers()[0]->getPort().toInt(),
                             d->preferences->getAetitle().toUtf8().data(),
                             d->preferences->getServers()[0]->getAetitle().toUtf8().data(),
                             UID_FINDPatientRootQueryRetrieveInformationModel,
                             EXS_Unknown,
                             DIMSE_BLOCKING,
                             0,
                             ASC_DEFAULTMAXPDU,
                             false,
                             false,
                             1,
                             false,
                             -1,
                             &overrideKeys,
                             callback,
                             &fileNameList).bad())
        this->displayErrorMessage(tr("Cannot perform query C-FIND"));

    if (findscu.dropNetwork().bad())
        this->displayErrorMessage(tr("Cannot drop network"));
}

void
QtDcmManager::findSeriesScu(QString patientName, QString studyDescription)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back((QString("QueryRetrieveLevel=") + QString("" "SERIES" "")).toUtf8().data());
    overrideKeys.push_back((QString("PatientName=") + patientName).toUtf8().data());
    overrideKeys.push_back((QString("StudyDescription=") + studyDescription).toUtf8().data());
    overrideKeys.push_back((QString("SeriesDescription=") + d->serieDescription).toUtf8().data());
    overrideKeys.push_back(QString("Modality=" + d->modality).toUtf8().data());

    //Study level
    overrideKeys.push_back(QString("StudyID").toUtf8().data());
    overrideKeys.push_back(QString("StudyDate").toUtf8().data());
    overrideKeys.push_back(QString("StudyTime").toUtf8().data());

    //Serie level
    overrideKeys.push_back(QString("SeriesInstanceUID").toUtf8().data());
    overrideKeys.push_back(QString("InstitutionName").toUtf8().data());
    overrideKeys.push_back(QString("InstitutionAddress").toUtf8().data());
    overrideKeys.push_back(QString("PerformingPhysicianName").toUtf8().data());
    overrideKeys.push_back(QString("NumberOfSlices").toUtf8().data());

    //Image level
    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;
    OFCondition cond;

    QtDcmFindCallback * callback = new QtDcmFindCallback(QtDcmFindCallback::SERIE);
    callback->setSignalManager(d->signalManager);

    if (findscu.initializeNetwork(30).bad())
        this->displayErrorMessage(tr("Cannot establish network connection"));

    if (findscu.performQuery(d->preferences->getServers()[0]->getServer().toAscii().data(),
                             d->preferences->getServers()[0]->getPort().toInt(),
                             d->preferences->getAetitle().toAscii().data(),
                             d->preferences->getServers()[0]->getAetitle().toAscii().data(),
                             UID_FINDPatientRootQueryRetrieveInformationModel,
                             EXS_Unknown,
                             DIMSE_BLOCKING,
                             0,
                             ASC_DEFAULTMAXPDU,
                             false,
                             false,
                             1,
                             false,
                             -1,
                             &overrideKeys,
                             callback,
                             &fileNameList).bad())
        this->displayErrorMessage(tr("Cannot perform query C-FIND"));

    if (findscu.dropNetwork().bad())
        this->displayErrorMessage(tr("Cannot drop network"));
}

void
QtDcmManager::findImagesScu(QString patientName, QString id, QString studyDescription)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back((QString("QueryRetrieveLevel=") + QString("" "IMAGES" "")).toUtf8().data());
    overrideKeys.push_back((QString("PatientName=") + patientName).toUtf8().data());
    overrideKeys.push_back((QString("StudyDescription=") + studyDescription).toUtf8().data());
    overrideKeys.push_back(QString("SeriesInstanceUID=" + id).toUtf8().data());

    //Study level
    overrideKeys.push_back(QString("StudyID").toUtf8().data());
    overrideKeys.push_back(QString("StudyDate").toUtf8().data());
    overrideKeys.push_back(QString("StudyTime").toUtf8().data());

    //Serie level
    overrideKeys.push_back(QString("SeriesDescription").toUtf8().data());
    overrideKeys.push_back(QString("Modality").toUtf8().data());
    overrideKeys.push_back(QString("InstitutionName").toUtf8().data());
    overrideKeys.push_back(QString("InstitutionAddress").toUtf8().data());
    overrideKeys.push_back(QString("PerformingPhysicianName").toUtf8().data());
    // pleins d'autres trucs à mettre ici !

    //Image level
    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;
    OFCondition cond;

    QtDcmFindCallback * callback = new QtDcmFindCallback(QtDcmFindCallback::IMAGE);
    callback->setSignalManager(d->signalManager);

    if (findscu.initializeNetwork(30).bad())
        this->displayErrorMessage(tr("Cannot establish network connection"));

    if (findscu.performQuery(d->preferences->getServers()[0]->getServer().toUtf8().data(),
                             d->preferences->getServers()[0]->getPort().toInt(),
                             d->preferences->getAetitle().toUtf8().data(),
                             d->preferences->getServers()[0]->getAetitle().toUtf8().data(),
                             UID_FINDPatientRootQueryRetrieveInformationModel,
                             EXS_Unknown,
                             DIMSE_BLOCKING,
                             0,
                             ASC_DEFAULTMAXPDU,
                             false,
                             false,
                             1,
                             false,
                             -1,
                             &overrideKeys,
                             callback,
                             &fileNameList).bad())
        this->displayErrorMessage(tr("Cannot perform query C-FIND"));

    if (findscu.dropNetwork().bad())
        this->displayErrorMessage(tr("Cannot drop network"));
}

void
QtDcmManager::loadDicomdir()
{
    d->seriesToExport.clear();
    d->mode = "CD";
    //Convenience declarations for DCMTK
    static const OFString Patient("PATIENT");
    static const OFString Image("IMAGE");
    static const OFString Series("SERIES");
    static const OFString Study("STUDY");

    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;
    if (!(status = d->dfile.loadFile(d->dicomdir.toUtf8().data())).good()) {
        return;
    }
    //Get the dicomdir dataset in a DCMTK DcmObject
    d->dcmObject = d->dfile.getDataset();

    // Loading all the dicomdir items in a stack
    DcmStack items;
    if (!d->dcmObject->findAndGetElements(DCM_Item, items).good()) {
        return;
    }
    //Using temporary lists for storing detected studies, series and images
    QList<QtDcmStudy *> tmpStudy;
    QList<QtDcmSerie *> tmpSerie;
    QList<QtDcmImage *> tmpImage;
    d->patients.clear();

    //Unstacking and loading the different lists
    while (items.card() > 0) {
        DcmItem* lobj = (DcmItem*) items.top();
        DcmStack dirent;

        OFCondition condition = lobj->findAndGetElements(DCM_DirectoryRecordType, dirent);
        if (!condition.good()) {
            items.pop();
            continue;
        }

        while (dirent.card()) {
            DcmElement* elt = (DcmElement*) dirent.top();
            OFString cur;
            elt->getOFStringArray(cur);

            if (cur == Patient) {
                d->patients.append(new QtDcmPatient());
                d->patients.last()->setStudies(tmpStudy);
                tmpStudy.clear();

                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_PatientID, lelt).good()) {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    d->patients.last()->setId(QString(strID.c_str()));
                }
                if (lobj->findAndGetElement(DCM_PatientName, lelt).good()) {
                    OFString strName;
                    lelt->getOFStringArray(strName);
                    d->patients.last()->setName(QString(strName.c_str()));
                }
                if (lobj->findAndGetElement(DCM_PatientBirthDate, lelt).good()) {
                    OFString strBirth;
                    lelt->getOFStringArray(strBirth);
                    d->patients.last()->setBirthdate(QString(strBirth.c_str()));
                }
                if (lobj->findAndGetElement(DCM_PatientSex, lelt).good()) {
                    OFString strSex;
                    lelt->getOFStringArray(strSex);
                    d->patients.last()->setSex(QString(strSex.c_str()));
                }
            }
            else if (cur == Study) {
                tmpStudy.append(new QtDcmStudy());
                tmpStudy.last()->setSeries(tmpSerie);
                tmpSerie.clear();

                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_StudyInstanceUID, lelt).good()) {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    tmpStudy.last()->setId(QString(strID.c_str()));
                }
                if (lobj->findAndGetElement(DCM_StudyDescription, lelt).good()) {
                    OFString strDescription;
                    lelt->getOFStringArray(strDescription);
                    tmpStudy.last()->setDescription(QString(strDescription.c_str()));
                }
                if (lobj->findAndGetElement(DCM_StudyDate, lelt).good()) {
                    OFString strDate;
                    lelt->getOFStringArray(strDate);
                    QString date = QString(strDate.c_str());
                    QDate qdate = QDate(date.mid(0, 4).toInt(), date.mid(4, 2).toInt(), date.mid(6, 2).toInt());
                    tmpStudy.last()->setDate(qdate);
                }
                if (lobj->findAndGetElement(DCM_StudyTime, lelt).good()) {
                    OFString strTime;
                    lelt->getOFStringArray(strTime);
                    tmpStudy.last()->setTime(QString(strTime.c_str()));
                }
            }
            else if (cur == Series) {
                tmpSerie.append(new QtDcmSerie());
                tmpSerie.last()->setImages(tmpImage);
                tmpImage.clear();

                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_SeriesInstanceUID, lelt).good()) {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    tmpSerie.last()->setId(QString(strID.c_str()));
                }
                if (lobj->findAndGetElement(DCM_SeriesDescription, lelt).good()) {
                    OFString strDescription;
                    lelt->getOFStringArray(strDescription);
                    QString description = QString(strDescription.c_str());
                    if (description == "")
                        tmpSerie.last()->setDescription("No description");
                    else
                        tmpSerie.last()->setDescription(description);
                }
            }
            else // C'est une image
            {
                tmpImage.append(new QtDcmImage());
                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_ReferencedFileID, lelt).good()) {
                    OFString str;
                    lelt->getOFStringArray(str);
                    tmpImage.last()->setFilename(QString(str.c_str()));
                }
                if (lobj->findAndGetElement(DCM_ReferencedSOPInstanceUIDInFile, lelt).good()) {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    tmpImage.last()->setId(QString(strID.c_str()));
                }
            }
            dirent.pop();
        }
        items.pop();
    }
}

void
QtDcmManager::createTemporaryDirs()
{
    //Creation d'un répertoire temporaire pour la série
    QDir tempDir = QDir(QDir::tempPath()); //tempDir = /tmp

    //Generer un nom de répertoire aléatoire
    QString acceptes = "abcdefghijklmnopqrstuvwyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
    QString randName;
    qsrand(time(NULL));
    for (int i = 0; i < 6; i++)
        randName += acceptes[qrand() % 59];

    if (!tempDir.exists("qtdcm" + randName))
        tempDir.mkdir("qtdcm" + randName);

    d->tempDir = QDir(QDir::tempPath() + QDir::separator() + "qtdcm" + randName); // tempDir = /tmp/qtdcm
    if (!d->tempDir.exists("logs")) {
        if (!d->tempDir.mkdir("logs"))
            qDebug() << tr("Repertoire logs non cree");
    }
    d->logsDir = QDir(d->tempDir.absolutePath() + QDir::separator() + "logs");
}

void
QtDcmManager::deleteTemporaryDirs()
{
    QStringList listLogs = d->logsDir.entryList(QDir::Files, QDir::Name);
    for (int i = 0; i < listLogs.size(); i++) {
        d->logsDir.remove(listLogs.at(i));
    }
    d->tempDir.rmdir("logs");

    QStringList listSerie = d->tempDir.entryList(QDir::Dirs, QDir::Name);
    for (int i = 0; i < listSerie.size(); i++) {
        QDir serieDir(d->tempDir.absolutePath() + QDir::separator() + listSerie.at(i));
        QStringList listFiles = serieDir.entryList(QDir::Files, QDir::Name);
        if (!(listSerie.at(i) == "." || listSerie.at(i) == "."))
            for (int j = 0; j < listFiles.size(); j++) {
                {
                    serieDir.remove(listFiles.at(j));
                }
                d->tempDir.rmdir(listSerie.at(i));
            }
    }
    QDir(QDir::tempPath()).rmdir(d->tempDir.dirName());
}

void
QtDcmManager::generateCurrentSerieDir()
{
    if (!d->serieId.isEmpty()) {
        d->currentSerieDir = d->tempDir.absolutePath() + QDir::separator() + d->serieId;
        d->tempDir.mkdir(d->serieId);
    }

}

void
QtDcmManager::deleteCurrentSerieDir()
{
    // Suppression des fichiers temporaires
    QStringList listFiles = d->currentSerieDir.entryList(QDir::Files, QDir::Name);
    for (int i = 0; i < listFiles.size(); i++) {
        d->currentSerieDir.remove(listFiles.at(i));
    }
    // Suppression du répertoire temporaire
    if (!d->tempDir.rmdir(d->serieId))
        qDebug() << tr("Probleme lors de la suppression du répertoire temporaire");
}

void
QtDcmManager::exportSeries()
{
    d->dcm2nii = d->preferences->getDcm2nii();
    if (d->outputDir == "")
        return;

    if (!QFile(d->dcm2nii).exists()) {
        //message d'erreur !
        this->displayErrorMessage(tr("Impossible de trouver dcm2nii, verifiez votre installation"));
        return;
    }

    if (d->mode == "CD")
        this->exportSerieFromCD();
    else {
        this->exportSerieFromPACS();
        while (d->exportThread->isRunning()) {
            qApp->processEvents();
        }
        d->progress->close();
        delete d->progress;
    }

    QList<QString> series = d->seriesToExport.keys();
    for (int j = 0; j < series.size(); j++) {
        d->serieId = series.at(j);
        d->currentSerieDir = QDir(d->tempDir.absolutePath() + QDir::separator() + d->serieId);
        //Conversion de la serie avec dcm2nii
        QStringList arguments;
        arguments << "-s" << "N";
        arguments << "-x" << "N";
        arguments << "-r" << "N";
        arguments << "-g" << "N";
        arguments << "-o" << d->outputDir << d->currentSerieDir.absolutePath();

        d->process = new QProcess(this);
        d->process->setStandardOutputFile(d->logsDir.absolutePath() + QDir::separator() + "log" + d->serieId + ".txt");
        d->process->start(d->dcm2nii, arguments);
        d->process->waitForFinished();
        //zeroStr.fill(QChar('0'), 5 - QString::number(i).size());

        //this->deleteCurrentSerieDir();
        delete d->process;
    }
    this->displayMessage(tr("Import termine"));
}

void
QtDcmManager::exportSerieFromCD()
{
    // Launch progress dialog window, to follow images copy
    d->progress = new QProgressDialog(tr("Copie des images depuis le CD..."), "", 0, 100, d->parent);
    d->progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    d->progress->setCancelButton(cancelButton);
    cancelButton->hide();
    d->progress->setValue(0);
    qApp->processEvents();
    d->progress->show();
    qApp->processEvents();

    //Copie des fichiers images dans le répertoire temporaire
    QList<QString> series = d->seriesToExport.keys();
    for (int j = 0; j < series.size(); j++) {
        d->progress->setValue(0);
        d->serieId = series.at(j);
        d->currentSerieDir = QDir(d->tempDir.absolutePath() + QDir::separator() + d->serieId);
        if (!d->tempDir.exists(series.at(j))) {
            this->generateCurrentSerieDir();
        }
        for (int i = 0; i < d->seriesToExport.value(series.at(j)).size(); i++) {
            QFile image(d->seriesToExport.value(series.at(j)).at(i));
            if (image.exists()) {
                QString zeroStr;
                zeroStr.fill(QChar('0'), 5 - QString::number(i).size());
                image.copy(d->currentSerieDir.absolutePath() + QDir::separator() + "ima" + zeroStr + QString::number(i));
                d->progress->setValue(100 * i / d->seriesToExport.value(series.at(j)).size());
                qApp->processEvents();
            }
        }
        d->progress->setValue(100);
        qApp->processEvents();
    }
    d->progress->close();
    delete d->progress;
}

void
QtDcmManager::exportSerieFromPACS()
{
    QString program = d->preferences->getDcm4che();

    d->exportThread->setProgram(d->preferences->getDcm4che());
    d->exportThread->setServerPacsParam(d->preferences->getServers().at(0)->getAetitle() + "@" + d->preferences->getServers().at(0)->getServer() + ":" + d->preferences->getServers().at(0)->getPort());
    d->exportThread->setLocalPacsParam(d->preferences->getAetitle() + "@" + d->preferences->getHostname() + ":" + d->preferences->getPort());
    d->exportThread->setSeriesToExport(d->seriesToExport.keys());
    d->exportThread->setTemporaryDir(d->tempDir.absolutePath());
    d->exportThread->setModality(d->modality);
    d->exportThread->setAetitle(d->preferences->getAetitle());

    d->progress = new QProgressDialog(tr("Chargement des images..."), "", 0, 0, d->parent);
    d->progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    d->progress->setCancelButton(cancelButton);
    cancelButton->hide();
    cancelButton->hide();
    d->progress->setValue(0);
    d->progress->show();
    qApp->processEvents();

    d->exportThread->start();
}

void
QtDcmManager::queryPACS()
{
    d->seriesToExport.clear();
    d->mode = "PACS";
    QString program = d->preferences->getDcm4che();
    if (QFile(program).exists()) {
        QStringList arguments;
        QString localDicomParam = d->preferences->getAetitle() + "@" + d->preferences->getHostname() + ":" + d->preferences->getPort();
        QString serverPACSParam = d->preferences->getServers().at(0)->getAetitle() + "@" + d->preferences->getServers().at(0)->getServer() + ":" + d->preferences->getServers().at(0)->getPort();
        QString serieDescription = "-qSeriesDescription=" + d->serieDescription;
        QString patientName = "-qPatientName=" + d->patientName;
        QString patientId = "-qPatientId=" + d->patientId;
        QString studyDescription = "-qStudyDescription=" + d->studyDescription;
        QString modality = "-qModality=" + d->modality;
        QString date = "-qStudyDate=" + d->date1 + "-" + d->date2;

        arguments << "-L" << localDicomParam << serverPACSParam;
        arguments << "-S" << serieDescription;
        arguments << patientName;
        arguments << studyDescription << modality << date;

        d->progress = new QProgressDialog(tr("Requete en cours..."), "", 0, 0, d->parent);
        d->progress->setWindowModality(Qt::WindowModal);
        QPushButton * cancel = new QPushButton("Cancel");
        d->progress->setCancelButton(cancel);
        d->progress->show();
        qApp->processEvents();

        d->queryThread->setCommand(program);
        d->queryThread->setParameters(arguments);
        d->queryThread->setManager(this);
        d->queryThread->start();

        while (d->queryThread->isRunning()) {
            if (d->progress->isHidden())
                d->queryThread->terminate();
            qApp->processEvents();
        }

        d->progress->close();
        delete d->progress;

        //Parse the result of the query and store in the different lists and display in the QTreeWidget
        this->parseQueryResult(d->query);
    }
    else {
        //message d'erreur !
        this->displayErrorMessage(tr("Impossible de trouver dcmqr, verifiez votre installation"));
    }
}

void
QtDcmManager::parseQueryResult(QString query)
{
    d->patients.clear();
    QStringList lines = query.split("\n");
    QString currentStudyId;
    int numPatient = 0;
    int cptStudy = 0;
    for (int i = 0; i < lines.size(); i++) {
        QRegExp rx(".*Query Response #.:.*");
        if (rx.exactMatch(lines[i])) // Detect patient name
        {
            numPatient = lines[i].split('#')[1].split(':')[0].toInt() - 1;
            // Add patient in the list
            d->patients.append(new QtDcmPatient());
            QString name = lines[i + 6].section('[', 1).split(']')[0];
            d->patients.last()->setName(name);
        }

        rx.setPattern(".*Query Response #. for Query Request.*");
        if (rx.exactMatch(lines[i])) //Detect new serie
        {
            numPatient = lines[i].split('#')[2].split('/')[0].toInt() - 1;
            //First get the corresponding study information
            QString studyDate = lines[i + 2].section('[', 1).split(']')[0];
            QDate qdate = QDate(studyDate.mid(0, 4).toInt(), studyDate.mid(4, 2).toInt(), studyDate.mid(6, 2).toInt());

            QString studyDesc = lines[i + 7].section('[', 1).split(']')[0];
            QString studyId = lines[i + 10].section('[', 1).split(']')[0];
            if (studyId != currentStudyId) // Is it a new study ?
            {
                cptStudy++;
                //Add the new study in the patients studies list and set study Id and description
                d->patients.at(numPatient)->addStudy(new QtDcmStudy());
                d->patients.at(numPatient)->getStudies().last()->setId(studyId);
                d->patients.at(numPatient)->getStudies().last()->setDescription(studyDesc);
                d->patients.at(numPatient)->getStudies().last()->setDate(qdate);
            }
            currentStudyId = studyId;

            //Get the found serie Id and description
            QString serieDesc = lines[i + 8].section('[', 1).split(']')[0];
            QString serieId = lines[i + 11].section('[', 1).split(']')[0];
            d->patients.at(numPatient)->getStudies().last()->addSerie(new QtDcmSerie());
            //Set the description and Id of the found serie
            d->patients.at(numPatient)->getStudies().last()->getSeries().last()->setId(serieId);
            d->patients.at(numPatient)->getStudies().last()->getSeries().last()->setDescription(serieDesc);
        }
    }
    //Aucune occurence = 0 study renvoyee
    if (cptStudy == 0) {
        //petit message d'information !
        this->displayMessage(tr("Aucune occurence pour cette recherche"));
    }
}

void
QtDcmManager::makePreview()
{
    d->seriesToExport.insert(d->serieId, d->images);
    d->currentSerieDir = QDir(d->tempDir.absolutePath() + QDir::separator() + d->serieId);
    if (!d->tempDir.exists(d->serieId)) {
        //Utiliser l'UID de la serie pour copier les images
        if (d->mode == "CD") {
            this->exportSerieFromCD();
        }
        else {
            this->exportSerieFromPACS();
            while (d->exportThread->isRunning()) {
                qApp->processEvents();
            }
            d->progress->close();
            delete d->progress;
        }
    }
    d->progress = new QProgressDialog(tr("Creation de l'apercu..."), "", 0, 0, d->parent);
    d->progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    d->progress->setCancelButton(cancelButton);
    cancelButton->hide();
    d->progress->show();
    qApp->processEvents();
    QStringList list = d->currentSerieDir.entryList(QDir::Files, QDir::Name);
    d->listImages.clear();

    DcmRLEDecoderRegistration::registerCodecs(OFFalse, OFFalse);
    DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse);

    for (int i = 0; i < list.size(); i++) {
        qApp->processEvents();
        //QImage * current_image_ = NULL;
        // get pixeldata
        DcmFileFormat file;
        file.loadFile((d->currentSerieDir.absolutePath() + QDir::separator() + list.at(i)).toLatin1().data());
        DcmDataset * dset = file.getDataset();
        DicomImage* dcimage = new DicomImage(dset, file.getDataset()->getOriginalXfer(), CIF_MayDetachPixelData);

        dcimage->setNoDisplayFunction();
        dcimage->hideAllOverlays();
        dcimage->setNoVoiTransformation();

        //        DicomImage * dcimage = new DicomImage((d->currentSerieDir.absolutePath() + QDir::separator() + list.at(i)).toLatin1().data());
        if (dcimage != NULL) {
            if (dcimage->getStatus() == EIS_Normal) {
                Uint32 *pixelData = (Uint32 *) (dcimage->getOutputData(32 /* bits per sample */));
                if (pixelData != NULL) {
                    Uint8 *colored = new Uint8[dcimage->getWidth() * dcimage->getHeight() * 4]; //4 * dcimage->getWidth() * dcimage->getHeight() matrix
                    Uint8 *col = colored;
                    Uint32 *p = pixelData;
                    //get the highest values for RGBA, then use them to scale the pixel luminosity
                    Uint32 p_max = 0;
                    Uint32 p_min = std::numeric_limits<Uint32>::max();
                    for (unsigned i = 0; i < dcimage->getWidth(); ++i)
                        for (unsigned j = 0; j < dcimage->getHeight(); ++j, ++p) {
                            if (*p > p_max)
                                p_max = *p;
                            if (*p < p_min)
                                p_min = *p;
                        }
                    double a = 4294967295.f / ((double) p_max - (double) p_min);
                    //re-initialize 'col'
                    p = pixelData;
                    //copy the pixels in our QImage
                    for (unsigned i = 0; i < dcimage->getWidth(); ++i)
                        for (unsigned j = 0; j < dcimage->getHeight(); ++j, ++p) {
                            *col = (Uint8) ((255.f / 4294967295.f) * (a * ((double) (*p) - (double) p_min)));
                            ++col;
                            *col = (Uint8) ((255.f / 4294967295.f) * (a * ((double) (*p) - (double) p_min)));
                            ++col;
                            *col = (Uint8) ((255.f / 4294967295.f) * (a * ((double) (*p) - (double) p_min)));
                            ++col;
                            *col = 255;
                            ++col;
                        }
                    d->listImages.append(QImage(colored, dcimage->getWidth(), dcimage->getHeight(), QImage::Format_ARGB32));
                }

            }
        }
    }
    d->progress->close();
    delete d->progress;
    d->seriesToExport.clear();
}

// Getters and setters
QString
QtDcmManager::getDicomdir() const
{
    return d->dicomdir;
}

void
QtDcmManager::setDicomdir(QString dicomdir)
{
    d->dicomdir = dicomdir;
}

QString
QtDcmManager::getOutputDirectory() const
{
    return d->outputDir;
}

void
QtDcmManager::setOutputDirectory(QString directory)
{
    d->outputDir = directory;
}

QtDcmPreferences *
QtDcmManager::getPreferences()
{
    return d->preferences;
}

void
QtDcmManager::setPreferences(QtDcmPreferences * prefs)
{
    d->preferences = prefs;
}

QString
QtDcmManager::getPatientName()
{
    return d->patientName;
}

void
QtDcmManager::setPatientName(QString patientName)
{
    d->patientName = patientName;
}

QString
QtDcmManager::getPatientId()
{
    return d->patientId;
}

void
QtDcmManager::setPatientId(QString patientId)
{
    d->patientId = patientId;
}

void
QtDcmManager::setPatientSex(QString sex)
{
    d->patientSex = sex;
}

QString
QtDcmManager::getSerieDescription()
{
    return d->serieDescription;
}

void
QtDcmManager::setSerieDescription(QString serieDescription)
{
    d->serieDescription = serieDescription;
}

QString
QtDcmManager::getStudyDescription()
{
    return d->studyDescription;
}

void
QtDcmManager::setStudyDescription(QString studyDescription)
{
    d->studyDescription = studyDescription;
}

void
QtDcmManager::setModality(QString modality)
{
    d->modality = modality;
}

QString
QtDcmManager::getModality()
{
    return d->modality;
}

void
QtDcmManager::setDate1(QString date)
{
    d->date1 = date;
}

QString
QtDcmManager::getDate1()
{
    return d->date1;
}

void
QtDcmManager::setDate2(QString date)
{
    d->date2 = date;
}

QString
QtDcmManager::getDate2()
{
    return d->date2;
}

QList<QtDcmPatient *>
QtDcmManager::getPatients()
{
    return d->patients;
}

void
QtDcmManager::addPatient()
{
    d->patients.append(new QtDcmPatient());
}

QString
QtDcmManager::getMode()
{
    return d->mode;
}

void
QtDcmManager::setImagesList(QList<QString> images)
{
    d->images = images;
}

QList<QImage>
QtDcmManager::getListImages()
{
    return d->listImages;
}

void
QtDcmManager::setSerieId(QString id)
{
    d->serieId = id;
}

QString
QtDcmManager::getCurrentSerieDirectory()
{
    return d->currentSerieDir.absolutePath();
}

void
QtDcmManager::setSeriesToExport(QMap<QString, QList<QString> > seriesToExport)
{
    d->seriesToExport = seriesToExport;
}

void
QtDcmManager::setQuery(QByteArray query)
{
    d->query = query;
}
