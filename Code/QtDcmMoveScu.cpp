/*
 * QtDcmMoveScu.cpp
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
 */

#define QT_NO_CAST_TO_ASCII

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

#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QtDcmPreferences.h>
#include <QtDcmServer.h>
#include <QtDcmManager.h>
#include <QtDcmMoveScu.h>

class QtDcmMoveScuPrivate
{
    public:
        QtDcmManager * manager;
        QList<QString> series;
        QList<QString> filenames;
        QString outputDir;

        //For dcmtk stuff
        DcmDataset *overrideKeys;
        OFCondition cond;
        T_ASC_Parameters *params;
        T_ASC_Association *assoc;
        T_ASC_Network *net;
        T_ASC_PresentationContextID presId;
};

QtDcmMoveScu::QtDcmMoveScu(QObject * parent) :
    d(new QtDcmMoveScuPrivate)
{
    d->manager = dynamic_cast<QtDcmManager *> (parent);
    d->overrideKeys = NULL;
    d->net = NULL;
    d->params = NULL;
}

QtDcmMoveScu::~QtDcmMoveScu()
{
}

void
QtDcmMoveScu::setSeries(QList<QString> series)
{
    d->series = series;
}

void
QtDcmMoveScu::setOutputDir(QString dir)
{
    d->outputDir = dir;
}

bool
QtDcmMoveScu::isAssociated()
{
    return (d->assoc != NULL);
}

bool
QtDcmMoveScu::isConnected()
{
    return (d->net != NULL);
}

void
QtDcmMoveScu::run()
{
//     int step = (int) (100.0 / d->series.size());
//     int progress = 0;
    for (int i = 0; i < d->series.size(); i++) {
        QDir serieDir(d->outputDir + QDir::separator() + d->series.at(i));
        if (!serieDir.exists())
            QDir(d->outputDir).mkdir(d->series.at(i));
        this->moveScu(d->series.at(i));
        emit updateProgress((int) (100.0 * i / d->series.size()));
    }
    exit();
}

void
QtDcmMoveScu::moveScu(QString uid)
{
    this->addOverrideKey(QString("QueryRetrieveLevel=") + QString("" "SERIES" ""));
    this->addOverrideKey(QString("SeriesInstanceUID=" + uid));

    d->cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, d->manager->getPreferences()->getPort().toInt(), 5, &d->net);

    if (d->cond.bad()) {
        qDebug() << "Cannot initialize network :(";
        return;
    }

    d->cond = ASC_createAssociationParameters(&d->params, ASC_DEFAULTMAXPDU);
    if (d->cond.bad()) {
        qDebug() << "Cannot create association";
        return;
    }

    ASC_setAPTitles(d->params, d->manager->getPreferences()->getAetitle().toUtf8().data(), d->manager->getCurrentPacs()->getAetitle().toUtf8().data(), d->manager->getCurrentPacs()->getAetitle().toUtf8().data());
    ASC_setPresentationAddresses(d->params, d->manager->getPreferences()->getHostname().toUtf8().data(), QString(d->manager->getCurrentPacs()->getServer() + ":"
            + d->manager->getCurrentPacs()->getPort()).toUtf8().data());

    const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax };

    T_ASC_PresentationContextID pid = 1;
    d->cond = ASC_addPresentationContext(d->params, pid, UID_FINDPatientRootQueryRetrieveInformationModel, transferSyntaxes, 1);
    pid = 3;
    d->cond = ASC_addPresentationContext(d->params, pid, UID_MOVEPatientRootQueryRetrieveInformationModel, transferSyntaxes, 1);

    d->cond = ASC_requestAssociation(d->net, d->params, &d->assoc);
    if (d->cond.bad()) {
        if (d->cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(d->params, &rej);
            OFString temp_str;
            ASC_printRejectParameters(temp_str, &rej);
            qDebug() << "Association Rejected:" << QString(temp_str.c_str());
            return;
        }
        else {
            qDebug() << "Association Request Failed:";
            return;
        }
    }

    if (ASC_countAcceptedPresentationContexts(d->params) == 0) {
        qDebug() << "No Acceptable Presentation Contexts";
        return;
    }

    d->cond = EC_Normal;
    this->cmove();

    if (d->cond == EC_Normal) {
        d->cond = ASC_releaseAssociation(d->assoc);
        ASC_dropNetwork(&d->net);
        if (d->cond.bad()) {
            qDebug() << "Association Release Failed:";
            return;
        }
    }
    else if (d->cond == DUL_PEERREQUESTEDRELEASE) {
        qDebug() << "Protocol Error: Peer requested release (Aborting)";
        d->cond = ASC_abortAssociation(d->assoc);
        ASC_dropNetwork(&d->net);
        if (d->cond.bad()) {
            qDebug() << "Association Abort Failed: ";
            return;
        }
        return;
    }
    else if (d->cond == DUL_PEERABORTEDASSOCIATION) {
        qDebug() << "Peer Aborted Association";
        return;
    }
    else {
        qDebug() << "Move SCU Failed: Aborting Association";
        d->cond = ASC_abortAssociation(d->assoc);
        ASC_dropNetwork(&d->net);
        if (d->cond.bad()) {
            qDebug() << "Association Abort Failed: ";
            return;
        }
        return;
    }
}

void
QtDcmMoveScu::addOverrideKey(QString key)
{
    char * s = key.toUtf8().data();
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = 0;
    OFString dicName, valStr;
    OFString msg;
    char msg2[200];

    // try to parse group and element number
    n = sscanf(s, "%x,%x=", &g, &e);
    OFString toParse = s;
    size_t eqPos = toParse.find('=');
    if (n < 2) // if at least no tag could be parsed
    {
        // if value is given, extract it (and extrect dictname)
        if (eqPos != OFString_npos) {
            dicName = toParse.substr(0, eqPos).c_str();
            valStr = toParse.substr(eqPos + 1, toParse.length());
        }
        else
            // no value given, just dictionary name
            dicName = s; // only dictionary name given (without value)
        // try to lookup in dictionary
        DcmTagKey key(0xffff, 0xffff);
        const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dicent = globalDataDict.findEntry(dicName.c_str());
        dcmDataDict.unlock();
        if (dicent != NULL) {
            // found dictionary name, copy group and element number
            key = dicent->getKey();
            g = key.getGroup();
            e = key.getElement();
        }
        else {
            // not found in dictionary
            msg = "bad key format or dictionary name not found in dictionary: ";
            msg += dicName;
            qDebug() << QString(msg.c_str());
        }
    } // tag could be parsed, copy value if it exists
    else {
        if (eqPos != OFString_npos)
            valStr = toParse.substr(eqPos + 1, toParse.length());
    }
    DcmTag tag(g, e);
    if (tag.error() != EC_Normal) {
        sprintf(msg2, "unknown tag: (%04x,%04x)", g, e);
        qDebug() << QString(msg2);
    }
    DcmElement *elem = newDicomElement(tag);
    if (elem == NULL) {
        sprintf(msg2, "cannot create element for tag: (%04x,%04x)", g, e);
        qDebug() << QString(msg2);
    }
    if (valStr.length() > 0) {
        if (elem->putString(valStr.c_str()).bad()) {
            sprintf(msg2, "cannot put tag value: (%04x,%04x)=\"", g, e);
            msg = msg2;
            msg += valStr;
            msg += "\"";
            qDebug() << QString(msg.c_str());
        }
    }
    if (d->overrideKeys == NULL)
        d->overrideKeys = new DcmDataset;
    if (d->overrideKeys->insert(elem, OFTrue).bad()) {
        sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
        qDebug() << QString(msg2);
    }
}

void
QtDcmMoveScu::acceptSubAssoc()
{
    qDebug() << "In accept sub assoc";
    const char* knownAbstractSyntaxes[] = { UID_VerificationSOPClass };

    const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax };
    int numTransferSyntaxes = 1;
    d->cond = ASC_receiveAssociation(d->net, &d->assoc, ASC_DEFAULTMAXPDU);

    if (d->cond.good()) {
        /* accept the Verification SOP Class if presented */
        d->cond = ASC_acceptContextsWithPreferredTransferSyntaxes(d->assoc->params, knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);

        if (d->cond.good()) {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            d->cond = ASC_acceptContextsWithPreferredTransferSyntaxes(d->assoc->params, dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs, transferSyntaxes, numTransferSyntaxes);
        }
    }
    if (d->cond.good())
        d->cond = ASC_acknowledgeAssociation(d->assoc);
    if (d->cond.bad()) {
        ASC_dropAssociation(d->assoc);
        ASC_destroyAssociation(&d->assoc);
    }
}

void
QtDcmMoveScu::storeSCP(T_DIMSE_Message * msg)
{
    qDebug() << "Store SCP";
    //      OFCondition cond = EC_Normal;
    //      T_DIMSE_C_StoreRQ *req;
    //      char imageFileName[2048];
    //
    //      req = &msg->msg.CStoreRQ;
    //
    //      if (opt_ignore)
    //      {
    //  #ifdef _WIN32
    //          tmpnam(imageFileName);
    //  #else
    //          strcpy(imageFileName, NULL_DEVICE_NAME);
    //  #endif
    //      } else {
    //          sprintf(imageFileName, "%s.%s",
    //              dcmSOPClassUIDToModality(req->AffectedSOPClassUID),
    //              req->AffectedSOPInstanceUID);
    //      }
    //
    //      OFString temp_str;
    //      OFLOG_INFO(movescuLogger, "Received Store Request: MsgID " << req->MessageID << ", ("
    //          << dcmSOPClassUIDToModality(req->AffectedSOPClassUID, "OT") << ")");
    //      OFLOG_DEBUG(movescuLogger, DIMSE_dumpMessage(temp_str, *req, DIMSE_INCOMING, NULL, presID));
    //
    //      StoreCallbackData callbackData;
    //      callbackData.assoc = assoc;
    //      callbackData.imageFileName = imageFileName;
    //      DcmFileFormat dcmff;
    //      callbackData.dcmff = &dcmff;
    //
    //      // store SourceApplicationEntityTitle in metaheader
    //      if (assoc && assoc->params)
    //      {
    //        const char *aet = assoc->params->DULparams.callingAPTitle;
    //        if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
    //      }
    //
    //      DcmDataset *dset = dcmff.getDataset();
    //
    //      if (opt_bitPreserving)
    //      {
    //        cond = DIMSE_storeProvider(assoc, presID, req, imageFileName, opt_useMetaheader,
    //          NULL, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), opt_blockMode, opt_dimse_timeout);
    //      } else {
    //        cond = DIMSE_storeProvider(assoc, presID, req, NULL, opt_useMetaheader,
    //          &dset, storeSCPCallback, OFreinterpret_cast(void*, &callbackData), opt_blockMode, opt_dimse_timeout);
    //      }
    //
    //      if (cond.bad())
    //      {
    //        OFLOG_ERROR(movescuLogger, "Store SCP Failed: " << DimseCondition::dump(temp_str, cond));
    //        /* remove file */
    //        if (!opt_ignore)
    //        {
    //          if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName);
    //        }
    //  #ifdef _WIN32
    //      } else if (opt_ignore) {
    //          if (strcmp(imageFileName, NULL_DEVICE_NAME) != 0) unlink(imageFileName); // delete the temporary file
    //  #endif
    //      }
    //
    //      if (opt_sleepAfter > 0)
    //      {
    //        OFStandard::sleep(OFstatic_cast(unsigned int, opt_sleepDuring));
    //      }
    //      return cond;
}

void
QtDcmMoveScu::storeSCPCallback(void *caller, char *imageFileName, DcmDataset **imageDataSet, DcmDataset **statusDetail)
{

}

void
QtDcmMoveScu::subOpSCP()
{
    qDebug() << "subopscp";

    T_DIMSE_Message msg;
    T_ASC_PresentationContextID presId;

    if (!ASC_dataWaiting(d->assoc, 0)) /* just in case */
        d->cond = DIMSE_NODATAAVAILABLE;

    d->cond = DIMSE_receiveCommand(d->assoc, DIMSE_BLOCKING, 30, &presId, &msg, NULL);
    qDebug() << "After receive command";
    qDebug() << d->cond.text();


    if (d->cond == EC_Normal) {
        switch (msg.CommandField) {
            case DIMSE_C_STORE_RQ:
                qDebug() << "Store request";
                this->storeSCP(&msg);
                break;
            case DIMSE_C_ECHO_RQ:
                qDebug() << "Echo request";
//                d->cond = echoSCP(*subAssoc, &msg, presID);
                break;
            default:
                qDebug() << "Bad command type";
                d->cond = DIMSE_BADCOMMANDTYPE;
//                qDebug() << "cannot handle command: 0x" << STD_NAMESPACEhex << OFstatic_cast(unsigned, msg.CommandField);
                break;
            }
        }
        /* clean up on association termination */
        if (d->cond == DUL_PEERREQUESTEDRELEASE) {
            d->cond = ASC_acknowledgeRelease(d->assoc);
            ASC_dropSCPAssociation(d->assoc);
            ASC_destroyAssociation(&d->assoc);
            return;
        }
        else if (d->cond == DUL_PEERABORTEDASSOCIATION) {
        }
        else if (d->cond != EC_Normal) {
            OFString temp_str;
            DimseCondition::dump(temp_str, d->cond);
            qDebug() << "DIMSE failure (aborting sub-association): " << QString(temp_str.c_str());
            /* some kind of error so abort the association */
            d->cond = ASC_abortAssociation(d->assoc);
        }

        if (d->cond != EC_Normal) {
            ASC_dropAssociation(d->assoc);
            ASC_destroyAssociation(&d->assoc);
        }
    }

void
QtDcmMoveScu::subOpCallback(void * caller, T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{
    if (!static_cast<QtDcmMoveScu*> (caller)->isConnected())
        return; /* help no net ! */

    if (!static_cast<QtDcmMoveScu*> (caller)->isAssociated()) {
        /* negotiate association */
        static_cast<QtDcmMoveScu*> (caller)->acceptSubAssoc();
    }
    else {
        /* be a service class provider */
        static_cast<QtDcmMoveScu*> (caller)->subOpSCP();
    }
}

void
QtDcmMoveScu::moveCallback(void *callbackData, T_DIMSE_C_MoveRQ * req, int responseCount, T_DIMSE_C_MoveRSP * rsp)
{
    OFCondition cond = EC_Normal;
    OFString temp_str;
    DIMSE_dumpMessage(temp_str, *rsp, DIMSE_INCOMING);
    qDebug() << "Move Response " << responseCount << ":";
//    qDebug() << QString(temp_str.c_str());
}

void
QtDcmMoveScu::substituteOverrideKeys(DcmDataset *dset)
{

}

void
QtDcmMoveScu::moveSCU()
{
    T_DIMSE_C_MoveRQ req;
    T_DIMSE_C_MoveRSP rsp;
    DIC_US msgId = d->assoc->nextMsgID++;
    DcmDataset *rspIds = NULL;
    DcmDataset *statusDetail = NULL;
    //    MyCallbackInfo callbackData;

    DcmDataset * dset(d->overrideKeys);

    dset->print(std::cout);

    /* which presentation context should be used */
    d->presId = ASC_findAcceptedPresentationContextID(d->assoc, UID_MOVEPatientRootQueryRetrieveInformationModel);
    if (d->presId == 0)
        d->cond = DIMSE_NOVALIDPRESENTATIONCONTEXTID;

    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, UID_MOVEPatientRootQueryRetrieveInformationModel);
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;

    ASC_getAPTitles(d->assoc->params, req.MoveDestination, NULL, NULL);

    d->cond = DIMSE_moveUser(d->assoc, d->presId, &req, dset, moveCallback, (void*) this, DIMSE_BLOCKING, 0, d->net, subOpCallback, (void*) this, &rsp, &statusDetail, &rspIds, OFTrue);

    if (d->cond == EC_Normal) {
        OFString temp_str;
        DIMSE_dumpMessage(temp_str, rsp, DIMSE_INCOMING);
        if (rspIds != NULL) {
            qDebug() << "Response Identifiers:";
            DcmDataset(*rspIds).print(std::cout);
        }
    }
    else {
        OFString temp_str;
        qDebug() << "Move Request Failed: " << QString(temp_str.c_str());
    }
    if (statusDetail != NULL) {
        qDebug() << "Status Detail:";
        DcmDataset(*statusDetail).print(std::cout);
        delete statusDetail;
    }

    if (rspIds != NULL)
        delete rspIds;
}

void
QtDcmMoveScu::cmove()
{
    d->cond = EC_Normal;
    int n = OFstatic_cast(int, 1);
    while (d->cond.good() && n--)
        this->moveSCU();
}
