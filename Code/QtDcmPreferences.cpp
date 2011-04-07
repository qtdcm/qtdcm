/*
 * QtDcmPreferences.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

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

#include <QtDcmServer.h>
#include <QtDcmPreferences.h>

class QtDcmPreferencesPrivate
{
    public:
        QFile iniFile; /** File stored on the file system that contains the preferences */
        QString aetitle; /** Local aetitle of QtDcm */
        QString port; /** Local port of qtdcm */
        QString hostname;
        QString encoding; /** Local character encoding */

        QString dcm2nii; /** Absolute filename of the dcm2nii binary on the system */
        QString dcm4che; /** Absolute filename of the dcm4che binary on the system */

        QList<QtDcmServer *> servers; /** List of server that QtDcm can query */
};

QtDcmPreferences::QtDcmPreferences() :
    d(new QtDcmPreferencesPrivate)
{
    // Check if the .qtdcm directory exists and if not, create it in the home directory
    QDir iniDir = QDir(QDir::homePath() + QDir::separator() + ".qtdcm");
    if (!iniDir.exists())
        QDir::home().mkdir(".qtdcm");

    //Check for the ini settings file
    d->iniFile.setFileName(iniDir.absolutePath() + QDir::separator() + "qtdcm.ini");
    if (!d->iniFile.exists())
        this->setDefaultIniFile(); //If it doesn't exist create it with default parameters
    else
        this->readSettings(); // else load the parameters
}

void
QtDcmPreferences::sendEcho(int index)
{
    if (index > d->servers.size())
        return;

    T_ASC_Network *net; // network struct, contains DICOM upper layer FSM etc.
    ASC_initializeNetwork(NET_REQUESTOR, 0, 1000 /* timeout */, &net);

    T_ASC_Parameters *params; // parameters of association request
    ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);

    // set calling and called AE titles
    ASC_setAPTitles(params, d->aetitle.toUtf8().data(), d->servers.at(index)->getAetitle().toUtf8().data(), NULL);

    // the DICOM server accepts connections at server.nowhere.com port 104
    ASC_setPresentationAddresses(params, d->hostname.toUtf8().data(), QString(d->servers.at(index)->getServer() + ":" + d->servers.at(index)->getPort()).toAscii().data());

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
            QMessageBox * msgBox = new QMessageBox(QApplication::activeWindow());
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setText("Echo request successful !");
            msgBox->exec();
            delete msgBox;
        }
        else {
            QMessageBox * msgBox = new QMessageBox(QApplication::activeWindow());
            msgBox->setIcon(QMessageBox::Critical);
            msgBox->setText("Wrong presentation context, echo request failed");
            msgBox->exec();
            delete msgBox;
        }
    }
    else {
        QMessageBox * msgBox = new QMessageBox(QApplication::activeWindow());
        msgBox->setIcon(QMessageBox::Critical);
        msgBox->setText("Wrong dicom association, echo request failed");
        msgBox->exec();
        delete msgBox;
    }

    ASC_releaseAssociation(assoc); // release association
    ASC_destroyAssociation(&assoc); // delete assoc structure
    ASC_dropNetwork(&net); // delete net structure
}

void
QtDcmPreferences::addServer()
{
    d->servers.append(new QtDcmServer);
}

void
QtDcmPreferences::removeServer(int index)
{
    d->servers.removeAt(index);
}

void
QtDcmPreferences::readSettings()
{
    //Instantiate a QSettings object from the ini file.
    QSettings prefs(d->iniFile.fileName(), QSettings::IniFormat);
    //Load local settings
    prefs.beginGroup("LocalSettings");
    d->dcm2nii = prefs.value("Dcm2nii").toString();
    d->dcm4che = prefs.value("Dcm4che").toString();
    d->aetitle = prefs.value("AETitle").toString();
    d->port = prefs.value("Port").toString();
    d->hostname = prefs.value("Hostname").toString();
    d->encoding = prefs.value("Encoding").toString();
    prefs.endGroup();
    //For each server load corresponding settings
    prefs.beginGroup("Servers");
    for (int i = 0; i < prefs.childGroups().size(); i++) {
        d->servers.append(new QtDcmServer);
        d->servers.at(i)->setAetitle(prefs.value("Server" + QString::number(i + 1) + "/AETitle").toString());
        d->servers.at(i)->setServer(prefs.value("Server" + QString::number(i + 1) + "/Hostname").toString());
        d->servers.at(i)->setPort(prefs.value("Server" + QString::number(i + 1) + "/Port").toString());
        d->servers.at(i)->setName(prefs.value("Server" + QString::number(i + 1) + "/Name").toString());
    }
    prefs.endGroup();
}

void
QtDcmPreferences::writeSettings()
{
    //Remove settings file
    QFile ini(d->iniFile.fileName());
    ini.remove();
    //Instantiate a QSettings object with the ini file.
    QSettings prefs(d->iniFile.fileName(), QSettings::IniFormat);
    //Write local settings from the private attributes
    prefs.beginGroup("LocalSettings");
    prefs.setValue("Dcm2nii", d->dcm2nii);
    prefs.setValue("Dcm4che", d->dcm4che);
    prefs.setValue("AETitle", d->aetitle);
    prefs.setValue("Port", d->port);
    prefs.setValue("Hostname", d->hostname);
    prefs.setValue("Encoding", d->encoding);
    prefs.endGroup();
    //Do the job for each server
    prefs.beginGroup("Servers");
    for (int i = 0; i < d->servers.size(); i++) {
        prefs.beginGroup("Server" + QString::number(i + 1));
        prefs.setValue("AETitle", d->servers.at(i)->getAetitle());
        prefs.setValue("Hostname", d->servers.at(i)->getServer());
        prefs.setValue("Port", d->servers.at(i)->getPort());
        prefs.setValue("Name", d->servers.at(i)->getName());
        prefs.endGroup();
    }
    prefs.endGroup();
}

void
QtDcmPreferences::setDefaultIniFile()
{
    //Create an ini file with default parameters
    d->dcm2nii = "";
    d->dcm4che = "";
    d->aetitle = "QTDCM";
    d->port = "2010";
    d->hostname = "localhost";
    d->encoding = "ISOd->IR 100";

    QtDcmServer * server = new QtDcmServer();
    server->setAetitle("SERVER");
    server->setServer("hostname");
    server->setName("Server" + QString::number(d->servers.size() + 1));
    server->setPort("11112");
    d->servers.append(server);
    this->writeSettings();
}

QString
QtDcmPreferences::getAetitle() const
{
    return d->aetitle;
}

QString
QtDcmPreferences::getEncoding() const
{
    return d->encoding;
}

QString
QtDcmPreferences::getPort() const
{
    return d->port;
}

QList<QtDcmServer *>
QtDcmPreferences::getServers()
{
    return d->servers;
}

QString
QtDcmPreferences::getDcm2nii()
{
    return d->dcm2nii;
}

QString
QtDcmPreferences::getDcm4che()
{
    return d->dcm4che;
}

void
QtDcmPreferences::setDcm2nii(QString path)
{
    d->dcm2nii = path;
}

void
QtDcmPreferences::setDcm4che(QString path)
{
    d->dcm4che = path;
}

void
QtDcmPreferences::setHostname(QString hostname)
{
    d->hostname = hostname;
}

QString
QtDcmPreferences::getHostname()
{
    return d->hostname;
}

void
QtDcmPreferences::setAetitle(QString aetitle)
{
    d->aetitle = aetitle;
}

void
QtDcmPreferences::setEncoding(QString encoding)
{
    d->encoding = encoding;
}

void
QtDcmPreferences::setPort(QString port)
{
    d->port = port;
}

void
QtDcmPreferences::setServers(QList<QtDcmServer *> servers)
{
    d->servers = servers;
}
