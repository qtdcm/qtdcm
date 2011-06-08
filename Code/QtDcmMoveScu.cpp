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
    QtDcmConvert * converter;
    QList<QString> series;
    QList<QString> filenames;
    QString outputDir;
    QString importDir;
    QString currentSerie;

    QtDcmMoveScu::mode mode;
    QString imageId;
};

QtDcmMoveScu::QtDcmMoveScu(QObject * parent) :
        d(new QtDcmMoveScuPrivate)
{
    d->manager = dynamic_cast<QtDcmManager *> (parent);
    d->converter = new QtDcmConvert(this);
    progressTotal = 0;
    progressSerie = 0;
    step = 0;

    maxPDU = ASC_DEFAULTMAXPDU;
    useMetaheader = OFTrue;
    networkTransferSyntax = EXS_Unknown;
    writeTransferSyntax = EXS_Unknown;
    groupLength = EGL_recalcGL;
    sequenceType = EET_ExplicitLength;
    paddingType = EPD_withoutPadding;
    filepad = 0;
    itempad = 0;
    bitPreserving = OFFalse;
    ignore = OFFalse;
    correctUIDPadding = OFFalse;
    repeatCount = 1;
    abortAssociation = OFFalse;
    moveDestination = NULL;
    cancelAfterNResponses = -1;
    queryModel = QMPatientRoot;
    ignorePendingDatasets = OFTrue;
    outputDirectory = ".";
    acseTimeout = 30;
    overrideKeys = NULL;
    useStoreSCP = true;
    overrideKeys = NULL;
    blockMode = DIMSE_BLOCKING;

    d->mode = QtDcmMoveScu::IMPORT;
}

QtDcmMoveScu::~QtDcmMoveScu()
{
}

void QtDcmMoveScu::setMode(QtDcmMoveScu::mode mode)
{
    d->mode = mode;
}

QtDcmMoveScu::mode QtDcmMoveScu::getMode()
{
    return d->mode;
}

void QtDcmMoveScu::setImageId(QString id)
{
    d->imageId = id;
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

void
QtDcmMoveScu::setImportDir ( QString dir ) {
    d->importDir = dir;
}

void
QtDcmMoveScu::run()
{
    OFCondition cond;
    step = (int) (100.0 / d->series.size());
    progressTotal = 0;
    for (int i = 0; i < d->series.size(); i++) {
        d->currentSerie = d->series.at(i);
        QDir serieDir(d->outputDir + QDir::separator() + d->series.at(i));
        if (!serieDir.exists())
            QDir(d->outputDir).mkdir(d->series.at(i));
        outputDirectory = QString(d->outputDir + QDir::separator() + d->currentSerie).toUtf8().constData();

        if (d->mode == IMPORT)
        {
            cond = this->move(d->series.at(i));
            //Test if files are present (avoir crash of the application)
            d->converter->setInputDirectory ( serieDir.absolutePath() );
            d->converter->setOutputFilename( d->series.at(i) + ".nii" );
            d->converter->setOutputDirectory ( d->importDir );
            d->converter->convert();
            emit updateProgress((int) (100.0 * (i+1) / d->series.size()));

            progressTotal += step;
        }
        else
            cond = this->move(d->imageId);
    }
    exit();
}

OFCondition
QtDcmMoveScu::move(QString uid)
{
    OFString temp_str;
    params = NULL;
    assoc = NULL;
    net = NULL;

    QuerySyntax querySyntax[3] =  {
        { UID_FINDPatientRootQueryRetrieveInformationModel,
            UID_MOVEPatientRootQueryRetrieveInformationModel },
        { UID_FINDStudyRootQueryRetrieveInformationModel,
          UID_MOVEStudyRootQueryRetrieveInformationModel },
        { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
          UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
    };

    OFCondition cond;

    if (d->mode == IMPORT)
    {
        this->addOverrideKey(QString("QueryRetrieveLevel=") + QString("" "SERIES" ""));
        this->addOverrideKey(QString("SeriesInstanceUID=" + uid));
    }
    else
    {
        this->addOverrideKey(QString("QueryRetrieveLevel=") + QString("" "IMAGE" ""));
        this->addOverrideKey(QString("SOPInstanceUID=" + uid));
    }
    cond = ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, d->manager->getPreferences()->getPort().toInt(), acseTimeout, &net);

    if (cond.bad()) {
        qDebug() << "Cannot create network: " << DimseCondition::dump(temp_str, cond).c_str();
        return cond;
    }

    cond = ASC_createAssociationParameters(&params, maxPDU);
    if (cond.bad()) {
        qDebug() << "Cannot create association: " << DimseCondition::dump(temp_str, cond).c_str();
        return cond;
    }

    ASC_setAPTitles(params, d->manager->getPreferences()->getAetitle().toUtf8().data(), d->manager->getCurrentPacs()->getAetitle().toUtf8().data(), d->manager->getCurrentPacs()->getAetitle().toUtf8().data());
    ASC_setPresentationAddresses(params, d->manager->getPreferences()->getHostname().toUtf8().data(), QString(d->manager->getCurrentPacs()->getServer() + ":"
                                 + d->manager->getCurrentPacs()->getPort()).toUtf8().data());

    cond = addPresentationContext(params, 1, querySyntax[queryModel].findSyntax, networkTransferSyntax);
    cond = addPresentationContext(params, 3, querySyntax[queryModel].moveSyntax, networkTransferSyntax);

    if (cond.bad()) {
        qDebug() << "Wrong presentation context:" << DimseCondition::dump(temp_str, cond).c_str();
        return cond;
    }

    cond = ASC_requestAssociation(net, params, &assoc);

    if (cond.bad()) {
        if (cond == DUL_ASSOCIATIONREJECTED) {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters(params, &rej);
            ASC_printRejectParameters(temp_str, &rej);
            qDebug() << "Association Rejected:" << QString(temp_str.c_str());
            return cond;
        }
        else {
            qDebug() << "Association Request Failed:" << DimseCondition::dump(temp_str,cond).c_str();
            return cond;
        }
    }

    if (ASC_countAcceptedPresentationContexts(params) == 0) {
        qDebug() << "No Acceptable Presentation Contexts";
        return cond;
    }

    cond = EC_Normal;
    cond = this->cmove(assoc, NULL);

    overrideKeys->clear();
    this->addOverrideKey(QString("QueryRetrieveLevel=") + QString("" "STUDY" ""));

    if (cond == EC_Normal) {
        if (abortAssociation) {
            qDebug() << "Aborting Association";
            cond = ASC_abortAssociation(assoc);
            ASC_dropNetwork(&net);
            if (cond.bad()) {
                qDebug() << "Association Abort Failed: " << DimseCondition::dump(temp_str,cond).c_str();
                return cond;
            }
        } else {
            /* release association */
            qDebug() << "Releasing Association";
//             cond = ASC_releaseAssociation(assoc); //Problem with error message Illegal Key
            ASC_dropNetwork(&net);
            if (cond.bad())
            {
                qDebug() << "Association Release Failed:" << DimseCondition::dump(temp_str,cond).c_str();
                return cond;
            }
        }
    }
    else if (cond == DUL_PEERREQUESTEDRELEASE) {
        qDebug() << "Protocol Error: Peer requested release (Aborting)";
        cond = ASC_abortAssociation(assoc);
        ASC_dropNetwork(&net);
        if (cond.bad()) {
            qDebug() << "Association Abort Failed: " << DimseCondition::dump(temp_str,cond).c_str();
            return cond;
        }
        return cond;
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION) {
        qDebug() << "Peer Aborted Association";
        return cond;
    }
    else {
        qDebug() << "Move SCU Failed: Aborting Association";
        cond = ASC_abortAssociation(assoc);
        ASC_dropNetwork(&net);
        if (cond.bad()) {
            qDebug() << "Association Abort Failed: " << DimseCondition::dump(temp_str,cond).c_str();
            return cond;
        }
        return cond;
    }
    return cond;
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
    if (overrideKeys == NULL)
        overrideKeys = new DcmDataset;
    if (overrideKeys->insert(elem, OFTrue).bad()) {
        sprintf(msg2, "cannot insert tag: (%04x,%04x)", g, e);
        qDebug() << QString(msg2);
    }
}

OFCondition
QtDcmMoveScu::addPresentationContext(T_ASC_Parameters *params, T_ASC_PresentationContextID pid, const char* abstractSyntax, E_TransferSyntax preferredTransferSyntax)
{
    const char* transferSyntaxes[] = { NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch (preferredTransferSyntax) {
    case EXS_LittleEndianImplicit:
        /* we only support Little Endian Implicit */
        transferSyntaxes[0]  = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 1;
        break;
    case EXS_LittleEndianExplicit:
        /* we prefer Little Endian Explicit */
        transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    case EXS_BigEndianExplicit:
        /* we prefer Big Endian Explicit */
        transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
        transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    default:
        /* We prefer explicit transfer syntaxes.
         * If we are running on a Little Endian machine we prefer
         * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
         */
        if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
        {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        } else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }
        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext(
               params, pid, abstractSyntax,
               transferSyntaxes, numTransferSyntaxes);
}

OFCondition
QtDcmMoveScu::acceptSubAssoc(T_ASC_Network * aNet, T_ASC_Association ** assoc)
{
    const char* knownAbstractSyntaxes[] = {
        UID_VerificationSOPClass
    };
    const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int numTransferSyntaxes;

    OFCondition cond = ASC_receiveAssociation(aNet, assoc, ASC_DEFAULTMAXPDU);
    if (cond.good())
    {
        switch (EXS_Unknown)
        {
        case EXS_LittleEndianImplicit:
            /* we only support Little Endian Implicit */
            transferSyntaxes[0] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 1;
            break;
        case EXS_LittleEndianExplicit:
            /* we prefer Little Endian Explicit */
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 3;
            break;
        case EXS_BigEndianExplicit:
            /* we prefer Big Endian Explicit */
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 3;
            break;
        case EXS_JPEGProcess14SV1TransferSyntax:
            /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
            transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_JPEGProcess1TransferSyntax:
            /* we prefer JPEGBaseline (default lossy for 8 bit images) */
            transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_JPEGProcess2_4TransferSyntax:
            /* we prefer JPEGExtended (default lossy for 12 bit images) */
            transferSyntaxes[0] = UID_JPEGProcess2_4TransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_JPEG2000LosslessOnly:
            /* we prefer JPEG2000 Lossless */
            transferSyntaxes[0] = UID_JPEG2000LosslessOnlyTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_JPEG2000:
            /* we prefer JPEG2000 Lossy */
            transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_JPEGLSLossless:
            /* we prefer JPEG-LS Lossless */
            transferSyntaxes[0] = UID_JPEGLSLosslessTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_JPEGLSLossy:
            /* we prefer JPEG-LS Lossy */
            transferSyntaxes[0] = UID_JPEGLSLossyTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_MPEG2MainProfileAtMainLevel:
            /* we prefer MPEG2 MP@ML */
            transferSyntaxes[0] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
        case EXS_RLELossless:
            /* we prefer RLE Lossless */
            transferSyntaxes[0] = UID_RLELosslessTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
#ifdef WITH_ZLIB
        case EXS_DeflatedLittleEndianExplicit:
            /* we prefer Deflated Explicit VR Little Endian */
            transferSyntaxes[0] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;
#endif
        default:
            if (OFFalse)
            {
                /* we accept all supported transfer syntaxes
                 * (similar to "AnyTransferSyntax" in "storescp.cfg")
                 */
                transferSyntaxes[0] = UID_JPEG2000TransferSyntax;
                transferSyntaxes[1] = UID_JPEG2000LosslessOnlyTransferSyntax;
                transferSyntaxes[2] = UID_JPEGProcess2_4TransferSyntax;
                transferSyntaxes[3] = UID_JPEGProcess1TransferSyntax;
                transferSyntaxes[4] = UID_JPEGProcess14SV1TransferSyntax;
                transferSyntaxes[5] = UID_JPEGLSLossyTransferSyntax;
                transferSyntaxes[6] = UID_JPEGLSLosslessTransferSyntax;
                transferSyntaxes[7] = UID_RLELosslessTransferSyntax;
                transferSyntaxes[8] = UID_MPEG2MainProfileAtMainLevelTransferSyntax;
                transferSyntaxes[9] = UID_DeflatedExplicitVRLittleEndianTransferSyntax;
                if (gLocalByteOrder == EBO_LittleEndian)
                {
                    transferSyntaxes[10] = UID_LittleEndianExplicitTransferSyntax;
                    transferSyntaxes[11] = UID_BigEndianExplicitTransferSyntax;
                } else {
                    transferSyntaxes[10] = UID_BigEndianExplicitTransferSyntax;
                    transferSyntaxes[11] = UID_LittleEndianExplicitTransferSyntax;
                }
                transferSyntaxes[12] = UID_LittleEndianImplicitTransferSyntax;
                numTransferSyntaxes = 13;
            } else {
                /* We prefer explicit transfer syntaxes.
                 * If we are running on a Little Endian machine we prefer
                 * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
                 */
                if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
                {
                    transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
                    transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
                } else {
                    transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
                    transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
                }
                transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
                numTransferSyntaxes = 3;
            }
            break;

        }

        /* accept the Verification SOP Class if presented */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
                   (*assoc)->params,
                   knownAbstractSyntaxes, DIM_OF(knownAbstractSyntaxes),
                   transferSyntaxes, numTransferSyntaxes);

        if (cond.good())
        {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
                       (*assoc)->params,
                       dcmAllStorageSOPClassUIDs, numberOfAllDcmStorageSOPClassUIDs,
                       transferSyntaxes, numTransferSyntaxes);
        }
    }
    if (cond.good()) cond = ASC_acknowledgeAssociation(*assoc);
    if (cond.bad()) {
        ASC_dropAssociation(*assoc);
        ASC_destroyAssociation(assoc);
    }
    return cond;
}

OFCondition
QtDcmMoveScu::echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID)
{
    OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL);
    if (cond.bad())
    {
        qDebug() << "Echo SCP Failed";
    }
    return cond;
}

OFCondition
QtDcmMoveScu::storeSCP( T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID, void* subOpCallbackData )
{
    QtDcmMoveScu * self = (QtDcmMoveScu*) subOpCallbackData;

    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFile[2048];

    req = &msg->msg.CStoreRQ;

    if (OFFalse)
    {
#ifdef _WIN32
        tmpnam(imageFileName);
#else
        strcpy(imageFile, NULL_DEVICE_NAME);
#endif
    } else {
        sprintf(imageFile, "%s.%s",
                dcmSOPClassUIDToModality(req->AffectedSOPClassUID),
                req->AffectedSOPInstanceUID);
    }

    self->assoc = assoc;
    self->imageFile = imageFile;
    DcmFileFormat dcmff;
    self->file = &dcmff;

    // store SourceApplicationEntityTitle in metaheader
    if (assoc && assoc->params)
    {
        const char *aet = assoc->params->DULparams.callingAPTitle;
        if (aet) dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
    }

    DcmDataset *dset = dcmff.getDataset();

    if (OFFalse)
    {
        cond = DIMSE_storeProvider(assoc, presID, req, imageFile, OFFalse,
                                   NULL, storeSCPCallback, (void *) subOpCallbackData, DIMSE_BLOCKING, 0);
    } else {
        cond = DIMSE_storeProvider(assoc, presID, req, (char *)NULL, OFFalse,
                                   &dset, storeSCPCallback, (void *) subOpCallbackData, DIMSE_BLOCKING, 0);
    }

    return cond;
}

void
QtDcmMoveScu::storeSCPCallback(void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req, char *imageFile, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *rsp, DcmDataset **statusDetail)
{
    QtDcmMoveScu* self = (QtDcmMoveScu*) callbackData;

    DIC_UI sopClass;
    DIC_UI sopInstance;

    if (progress->state == DIMSE_StoreEnd)
    {
        *statusDetail = NULL;

        if ((imageDataSet != NULL) && (*imageDataSet != NULL) && !self->bitPreserving && !self->ignore)
        {
            /* create full path name for the output file */
            OFString ofname;
            OFStandard::combineDirAndFilename(ofname, self->outputDirectory, self->imageFile, OFTrue /* allowEmptyDirName */);

            E_TransferSyntax xfer = self->writeTransferSyntax;
            if (xfer == EXS_Unknown) xfer = (*imageDataSet)->getOriginalXfer();

            OFCondition cond = self->file->saveFile(ofname.c_str(), xfer, self->sequenceType, self->groupLength,
                                                    self->paddingType, OFstatic_cast(Uint32, self->filepad), OFstatic_cast(Uint32, self->itempad),
                                                    (self->useMetaheader) ? EWM_fileformat : EWM_dataset);

            if (QFile(ofname.c_str()).exists())
                emit self->previewSlice(QString(ofname.c_str()));

            if ((rsp->DimseStatus == STATUS_Success) && !self->ignore)
            {
                /* which SOP class and SOP instance ? */
                if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet, sopClass, sopInstance, self->correctUIDPadding))
                {
                    rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                }
                else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0)
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                }
                else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0)
                {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                }
            }
        }
    }
    return;
}

OFCondition
QtDcmMoveScu::subOpSCP(T_ASC_Association **subAssoc, void * subOpCallbackData)
{
    T_DIMSE_Message     msg;
    T_ASC_PresentationContextID presID;

    if (!ASC_dataWaiting(*subAssoc, 0)) /* just in case */
        return DIMSE_NODATAAVAILABLE;

    OFCondition cond = DIMSE_receiveCommand(*subAssoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL);

    if (cond == EC_Normal) {
        switch (msg.CommandField)
        {
        case DIMSE_C_STORE_RQ:
            cond = storeSCP(*subAssoc, &msg, presID, subOpCallbackData);
            break;
        case DIMSE_C_ECHO_RQ:
            cond = echoSCP(*subAssoc, &msg, presID);
            break;
        default:
            cond = DIMSE_BADCOMMANDTYPE;
            break;
        }
    }
    /* clean up on association termination */
    if (cond == DUL_PEERREQUESTEDRELEASE)
    {
        cond = ASC_acknowledgeRelease(*subAssoc);
        ASC_dropSCPAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
        return cond;
    }
    else if (cond == DUL_PEERABORTEDASSOCIATION)
    {
    }
    else if (cond != EC_Normal)
    {
        OFString temp_str;
        qDebug() << "DIMSE failure (aborting sub-association): " << DimseCondition::dump(temp_str, cond).c_str();
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation(*subAssoc);
    }

    if (cond != EC_Normal)
    {
        ASC_dropAssociation(*subAssoc);
        ASC_destroyAssociation(subAssoc);
    }
    return cond;
}

void
QtDcmMoveScu::subOpCallback(void * caller, T_ASC_Network *aNet, T_ASC_Association **subAssoc)
{
    if (!caller)
        return;

    QtDcmMoveScu * self = (QtDcmMoveScu*) caller;
    emit self->updateProgress ( self->progressTotal + ( int ) ( ( ( float ) ( self->step * ( self->progressSerie ) / self->slicesCount ) ) ) );

    if (aNet == NULL) return;   /* help no net ! */

    if (*subAssoc == NULL) {
        /* negotiate association */
        acceptSubAssoc(aNet, subAssoc);
    } else {
        /* be a service class provider */
        subOpSCP(subAssoc, caller);
    }
    self->progressSerie ++;
}

void
QtDcmMoveScu::moveCallback(void *caller, T_DIMSE_C_MoveRQ * req, int responseCount, T_DIMSE_C_MoveRSP * rsp)
{
    if (!caller)
        return;

    QtDcmMoveScu * self = (QtDcmMoveScu*) caller;

    self->progressSerie = 0;
    self->slicesCount = rsp->NumberOfRemainingSubOperations + rsp->NumberOfFailedSubOperations + rsp->NumberOfWarningSubOperations + rsp->NumberOfCompletedSubOperations;

    OFCondition cond = EC_Normal;
    OFString temp_str;
    DIMSE_dumpMessage(temp_str, *rsp, DIMSE_INCOMING);
    qDebug() << "Move Response " << responseCount << ":";
    qDebug() << QString(temp_str.c_str());
}

void
QtDcmMoveScu::substituteOverrideKeys(DcmDataset *dset)
{
    if (overrideKeys == NULL) {
        return; /* nothing to do */
    }

    /* copy the override keys */
    DcmDataset keys(*overrideKeys);

    /* put the override keys into dset replacing existing tags */
    unsigned long elemCount = keys.card();
    for (unsigned long i = 0; i < elemCount; i++) {
        DcmElement *elem = keys.remove((unsigned long)0);

        dset->insert(elem, OFTrue);
    }
}

OFCondition
QtDcmMoveScu::moveSCU(T_ASC_Association * assoc, const char *fname)
{
    T_DIMSE_C_MoveRQ    req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US              msgId = assoc->nextMsgID++;
    DcmDataset          *rspIds = NULL;
    const char          *sopClass;
    DcmDataset          *statusDetail = NULL;

    QuerySyntax querySyntax[3] =  {
        { UID_FINDPatientRootQueryRetrieveInformationModel,
            UID_MOVEPatientRootQueryRetrieveInformationModel },
        { UID_FINDStudyRootQueryRetrieveInformationModel,
          UID_MOVEStudyRootQueryRetrieveInformationModel },
        { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
          UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
    };


    DcmFileFormat file;

    if (fname != NULL) {
        if (file.loadFile(fname).bad()) {
            qDebug() << "bad DICOM file: " << QString(fname);
            return DIMSE_BADDATA;
        }
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys(file.getDataset());

    sopClass = querySyntax[queryModel].moveSyntax;

    /* which presentation context should be used */
    presId = ASC_findAcceptedPresentationContextID(assoc, sopClass);
    if (presId == 0) return DIMSE_NOVALIDPRESENTATIONCONTEXTID;

    req.MessageID = msgId;
    strcpy(req.AffectedSOPClassUID, sopClass);
    req.Priority = DIMSE_PRIORITY_MEDIUM;
    req.DataSetType = DIMSE_DATASET_PRESENT;
    if (moveDestination == NULL) {
        /* set the destination to be me */
        ASC_getAPTitles(assoc->params, req.MoveDestination,
                        NULL, NULL);
    } else {
        strcpy(req.MoveDestination, moveDestination);
    }

    OFCondition cond;
    if (useStoreSCP)
        cond = DIMSE_moveUser(assoc, presId, &req, file.getDataset(), moveCallback, (void*) this, blockMode, dimseTimeout, net, subOpCallback, (void*) this, &rsp, &statusDetail, &rspIds, ignorePendingDatasets);
    else
        cond = DIMSE_moveUser(assoc, presId, &req, file.getDataset(), moveCallback, (void*) this, blockMode, dimseTimeout, net, NULL, (void*) this, &rsp, &statusDetail, &rspIds, ignorePendingDatasets);

    if (rspIds != NULL) delete rspIds;

    return cond;
}


OFCondition
QtDcmMoveScu::cmove(T_ASC_Association * assoc, const char *fname)
{
    OFCondition cond = EC_Normal;
    int n = (int)repeatCount;
    while (cond.good() && n--)
        cond = moveSCU(assoc, fname);
    return cond;
}
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
