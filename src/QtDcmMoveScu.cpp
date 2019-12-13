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

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QtDcmPreferences.h>
#include <QtDcmServer.h>
#include <QtDcmManager.h>
#include <QtDcmMoveScu.h>

class QtDcmMoveScu::Private
{

public:
    QStringList series;
    QStringList filenames;
    QString outputDir;
    QString importDir;
    QString currentSerie;

    QtDcmMoveScu::eMoveMode mode;
    QString imageId;
    
    
    
    T_ASC_Network*        net;              /* the global DICOM network */
    T_ASC_Parameters*     params;
    T_ASC_Association*  assoc;
    T_ASC_PresentationContextID presId;

    DcmFileFormat*      file;
    char*               imageFile;

    QtDcmMoveScu::QueryModel queryModel;
    T_DIMSE_BlockingMode  blockMode;
    OFBool useMetaheader;
    OFBool bitPreserving;
    OFBool ignore;
    E_TransferSyntax writeTransferSyntax;
    OFBool correctUIDPadding;
    E_GrpLenEncoding groupLength;
    E_EncodingType sequenceType;
    E_PaddingEncoding paddingType;
    OFCmdUnsignedInt filepad;
    OFCmdUnsignedInt itempad;
    const char *moveDestination;
    int dimseTimeout;
    int acseTimeout;
    OFCmdUnsignedInt maxPDU;
    E_TransferSyntax networkTransferSyntax;
    OFCmdUnsignedInt repeatCount;
    OFBool abortAssociation;
    OFCmdSignedInt cancelAfterNResponses;
    OFBool ignorePendingDatasets;
    DcmDataset overrideKeys;
    OFString outputDirectory;

    int slicesCount;
    int progressTotal;
    int progressSerie;
    int step;
};

QtDcmMoveScu::QtDcmMoveScu ( QObject * parent ) 
    : QThread(parent), 
      d ( new QtDcmMoveScu::Private )
{
    d->progressTotal = 0;
    d->progressSerie = 0;
    d->step = 0;

    d->net = 0;
    d->assoc = 0;
    d->params = 0;
    d->file = 0;
    d->maxPDU = ASC_DEFAULTMAXPDU;
    d->useMetaheader = OFTrue;
    d->networkTransferSyntax = EXS_Unknown;
    d->writeTransferSyntax = EXS_Unknown;
    d->groupLength = EGL_recalcGL;
    d->sequenceType = EET_ExplicitLength;
    d->paddingType = EPD_withoutPadding;
    d->filepad = 0;
    d->itempad = 0;
    d->bitPreserving = OFFalse;
    d->ignore = OFFalse;
    d->correctUIDPadding = OFFalse;
    d->repeatCount = 1;
    d->abortAssociation = OFFalse;
    d->moveDestination = NULL;
    d->cancelAfterNResponses = -1;
    d->queryModel = QMPatientRoot;
    d->ignorePendingDatasets = OFTrue;
    d->outputDirectory = ".";
    d->acseTimeout = 30;
    d->blockMode = DIMSE_BLOCKING;

    d->mode = QtDcmMoveScu::IMPORT;
}

QtDcmMoveScu::~QtDcmMoveScu()
{
    delete d;
    d = NULL;
}

void QtDcmMoveScu::setMode ( QtDcmMoveScu::eMoveMode mode )
{
    d->mode = mode;
}

void QtDcmMoveScu::setImageId ( const QString & id )
{
    d->imageId = id;
}

void QtDcmMoveScu::setSeries ( const QStringList & series )
{
    d->series = series;
}

void QtDcmMoveScu::setOutputDir ( const QString & dir )
{
    d->outputDir = dir;
}

void QtDcmMoveScu::setImportDir ( const QString & dir )
{
    d->importDir = dir;
}

void QtDcmMoveScu::onStopMove()
{
    if ( this->isRunning() ) {
        ASC_dropNetwork ( &d->net );
    }
}

void QtDcmMoveScu::run()
{
    OFCondition cond;
    d->step = ( int ) ( 100.0 / d->series.size() );
    d->progressTotal = 0;

    for ( int i = 0; i < d->series.size(); i++ ) {
        d->currentSerie = d->series.at ( i );
        const QDir serieDir ( d->outputDir + QDir::separator() + d->series.at ( i ) );

        qDebug() << "Run import" << serieDir.absolutePath();
        if ( !serieDir.exists() ) {
            QDir ( d->outputDir ).mkdir ( d->series.at ( i ) );
        }

        d->outputDirectory = QString ( d->outputDir + QDir::separator() + d->currentSerie ).toUtf8().constData();

        if ( d->mode == IMPORT ) {
            qDebug() << "plop";
            cond = this->move ( d->series.at ( i ) );
            emit updateProgress ( ( int ) ( 100.0 * ( i+1 ) / d->series.size() ) );
            d->progressTotal += d->step;
            emit serieMoved ( serieDir.absolutePath(), d->series.at ( i ), i );
        }
        else {
            cond = this->move ( d->imageId );
        }
    }
}

OFCondition QtDcmMoveScu::move ( const QString & uid )
{
    OFString temp_str;
    d->params = NULL;
    d->assoc = NULL;
    d->net = NULL;

    QuerySyntax querySyntax[3] = {
        { UID_FINDPatientRootQueryRetrieveInformationModel,
          UID_MOVEPatientRootQueryRetrieveInformationModel },
        { UID_FINDStudyRootQueryRetrieveInformationModel,
          UID_MOVEStudyRootQueryRetrieveInformationModel },
        { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
          UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
    };

    OFCondition cond;
    this->addOverrideKey(QString("PatientID=*"));
    this->addOverrideKey(QString("StudyInstanceUID=*"));

    if ( d->mode == IMPORT ) {
        this->addOverrideKey ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "SERIES" "" ) );
        this->addOverrideKey ( QString ( "SeriesInstanceUID=" + uid ) );
    }
    else {
        this->addOverrideKey ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "IMAGE" "" ) );
        this->addOverrideKey ( QString ( "SOPInstanceUID=" + uid ) );
        this->addOverrideKey (QString  ( "SeriesInstanceUID=*" ) );
    }

    cond = ASC_initializeNetwork ( NET_ACCEPTORREQUESTOR, QtDcmPreferences::instance()->port().toInt(), d->acseTimeout, &d->net );

    if ( cond.bad() ) {
        qDebug() << "Cannot create network: " << DimseCondition::dump ( temp_str, cond ).c_str();
        return cond;
    }

    cond = ASC_createAssociationParameters ( &d->params, d->maxPDU );

    if ( cond.bad() ) {
        qDebug() << "Cannot create association: " << DimseCondition::dump ( temp_str, cond ).c_str();
        return cond;
    }

    ASC_setAPTitles ( d->params, 
                      QtDcmPreferences::instance()->aetitle().toUtf8().data(), 
                      QtDcmManager::instance()->currentPacs().aetitle().toUtf8().data(), 
                      QtDcmManager::instance()->currentPacs().aetitle().toUtf8().data() );

    ASC_setPresentationAddresses ( d->params, 
                                   QtDcmPreferences::instance()->hostname().toUtf8().data(), 
                                   QString ( QtDcmManager::instance()->currentPacs().address() + ":" + QtDcmManager::instance()->currentPacs().port() ).toUtf8().data() );

    cond = addPresentationContext ( d->params, 1, querySyntax[d->queryModel].findSyntax, d->networkTransferSyntax );
    cond = addPresentationContext ( d->params, 3, querySyntax[d->queryModel].moveSyntax, d->networkTransferSyntax );

    if ( cond.bad() ) {
        qDebug() << "Wrong presentation context:" << DimseCondition::dump ( temp_str, cond ).c_str();
        T_ASC_RejectParameters rej;
        ASC_getRejectParameters ( d->params, &rej );
        ASC_printRejectParameters ( temp_str, &rej );
        
        ASC_dropNetwork ( &d->net );
        return cond;
    }

    cond = ASC_requestAssociation ( d->net, d->params, &d->assoc );

    if ( cond.bad() ) {
        if ( cond == DUL_ASSOCIATIONREJECTED ) {
            T_ASC_RejectParameters rej;
            ASC_getRejectParameters ( d->params, &rej );
            ASC_printRejectParameters ( temp_str, &rej );
        }
        else {
            qDebug() << "Association Request Failed:" << DimseCondition::dump ( temp_str,cond ).c_str();
        }
        
        ASC_abortAssociation ( d->assoc );
        ASC_dropNetwork ( &d->net );
        
        qDebug() << "Association Rejected:" << QString ( temp_str.c_str() );
        return cond;
    }

    if ( ASC_countAcceptedPresentationContexts ( d->params ) == 0 ) {
        qDebug() << "No Acceptable Presentation Contexts";
        
        ASC_abortAssociation ( d->assoc );
        ASC_dropNetwork ( &d->net );
        
        return cond;
    }

    cond = EC_Normal;

    cond = this->cmove ( d->assoc, NULL );

    d->overrideKeys.clear();
    this->addOverrideKey ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "STUDY" "" ) );

    if ( cond == EC_Normal ) {
        if ( d->abortAssociation ) {
            qDebug() << "Aborting Association";
            cond = ASC_abortAssociation ( d->assoc );
            ASC_dropNetwork ( &d->net );

            if ( cond.bad() ) {
                qDebug() << "Association Abort Failed: " << DimseCondition::dump ( temp_str,cond ).c_str();
                return cond;
            }
        }
        else {
            /* release association */
            qDebug() << "Releasing Association";
//             cond = ASC_releaseAssociation(assoc); //Problem with error message Illegal Key
            cond = ASC_dropNetwork ( &d->net );
            if (cond.bad()) {
                qDebug() << "Drop Network Failed:" << DimseCondition::dump ( temp_str,cond ).c_str();
                return cond;
            }
        }
    }
    else if ( cond == DUL_PEERREQUESTEDRELEASE ) {
        qDebug() << "Protocol Error: Peer requested release (Aborting)";
        cond = ASC_abortAssociation ( d->assoc );
        ASC_dropNetwork ( &d->net );

        if ( cond.bad() ) {
            qDebug() << "Association Abort Failed: " << DimseCondition::dump ( temp_str,cond ).c_str();
            return cond;
        }

        return cond;
    }
    else if ( cond == DUL_PEERABORTEDASSOCIATION ) {
        qDebug() << "Peer Aborted Association";
        ASC_abortAssociation ( d->assoc );
        ASC_dropNetwork ( &d->net );
        
        return cond;
    }
    else {
        qDebug() << "Move SCU Failed: Aborting Association";
        cond = ASC_abortAssociation ( d->assoc );
        ASC_dropNetwork ( &d->net );

        if ( cond.bad() ) {
            qDebug() << "Association Abort Failed: " << DimseCondition::dump ( temp_str,cond ).c_str();
            return cond;
        }
        return cond;
    }

    return cond;
}

void QtDcmMoveScu::addOverrideKey ( const QString & key )
{
    unsigned int g = 0xffff;
    unsigned int e = 0xffff;
    int n = 0;
    OFString dicName, valStr;
    OFString msg;
    char msg2[200];

    // try to parse group and element number
    OFString toParse(key.toLatin1().data());
    n = sscanf ( key.toLatin1().data(), "%x,%x=", &g, &e );

    size_t eqPos = toParse.find ( '=' );

    if ( n < 2 ) { // if at least no tag could be parsed
        // if value is given, extract it (and extrect dictname)
        if ( eqPos != OFString_npos ) {
            dicName = toParse.substr ( 0, eqPos ).c_str();
            valStr = toParse.substr ( eqPos + 1, toParse.length() );
        }
        else {
            // no value given, just dictionary name
            dicName = key.toLatin1().data(); // only dictionary name given (without value)
        }
        
        // try to lookup in dictionary
        DcmTagKey key ( 0xffff, 0xffff );
        const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
        const DcmDictEntry *dicent = globalDataDict.findEntry ( dicName.c_str() );

        dcmDataDict.unlock();

        if ( dicent != NULL ) {
            // found dictionary name, copy group and element number
            key = dicent->getKey();
            g = key.getGroup();
            e = key.getElement();
        }
        else {
            // not found in dictionary
            msg = "bad key format or dictionary name not found in dictionary: ";
            msg += dicName;
            qDebug() << QString ( msg.c_str() );
        }
    } // tag could be parsed, copy value if it exists
    else {
        if ( eqPos != OFString_npos )
            valStr = toParse.substr ( eqPos + 1, toParse.length() );
    }

    DcmTag tag ( g, e );
    if ( tag.error() != EC_Normal ) {
        sprintf ( msg2, "unknown tag: (%04x,%04x)", g, e );
        qDebug() << QString ( msg2 );
    }

    DcmElement *elem = DcmDataset::newDicomElement ( tag );
    if ( elem == NULL ) {
        sprintf ( msg2, "cannot create element for tag: (%04x,%04x)", g, e );
        qDebug() << QString ( msg2 );
    }

    if ( valStr.length() > 0 ) {
        if ( elem->putString ( valStr.c_str() ).bad() ) {
            sprintf ( msg2, "cannot put tag value: (%04x,%04x)=\"", g, e );
            msg = msg2;
            msg += valStr;
            msg += "\"";
            qDebug() << QString ( msg.c_str() );
        }
    }

    if ( d->overrideKeys.insert ( elem, OFTrue ).bad() ) {
        sprintf ( msg2, "cannot insert tag: (%04x,%04x)", g, e );
        qDebug() << QString ( msg2 );
    }
}

OFCondition QtDcmMoveScu::addPresentationContext ( T_ASC_Parameters *params, T_ASC_PresentationContextID pid, const char* abstractSyntax, E_TransferSyntax preferredTransferSyntax )
{
    const char* transferSyntaxes[] = { NULL, NULL, NULL };
    int numTransferSyntaxes = 0;

    switch ( preferredTransferSyntax )
    {
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

        if ( gLocalByteOrder == EBO_LittleEndian )  /* defined in dcxfer.h */ {
            transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
        }
        else {
            transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
        }

        transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;

        numTransferSyntaxes = 3;
        break;
    }

    return ASC_addPresentationContext (
                params, pid, abstractSyntax,
                transferSyntaxes, numTransferSyntaxes );
}

OFCondition QtDcmMoveScu::acceptSubAssoc ( T_ASC_Network * aNet, T_ASC_Association ** assoc )
{
    const char* knownAbstractSyntaxes[] = { UID_VerificationSOPClass };
    const char* transferSyntaxes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    int numTransferSyntaxes;

    OFCondition cond = ASC_receiveAssociation ( aNet, assoc, ASC_DEFAULTMAXPDU );

    if ( cond.good() ) {
        switch ( EXS_Unknown ) {

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

        case EXS_JPEGProcess14SV1:
            /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
            transferSyntaxes[0] = UID_JPEGProcess14SV1TransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;

        case EXS_JPEGProcess1:
            /* we prefer JPEGBaseline (default lossy for 8 bit images) */
            transferSyntaxes[0] = UID_JPEGProcess1TransferSyntax;
            transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
            transferSyntaxes[2] = UID_BigEndianExplicitTransferSyntax;
            transferSyntaxes[3] = UID_LittleEndianImplicitTransferSyntax;
            numTransferSyntaxes = 4;
            break;

        case EXS_JPEGProcess2_4:
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
            if ( OFFalse ) {
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

                if ( gLocalByteOrder == EBO_LittleEndian ) {
                    transferSyntaxes[10] = UID_LittleEndianExplicitTransferSyntax;
                    transferSyntaxes[11] = UID_BigEndianExplicitTransferSyntax;
                }
                else {
                    transferSyntaxes[10] = UID_BigEndianExplicitTransferSyntax;
                    transferSyntaxes[11] = UID_LittleEndianExplicitTransferSyntax;
                }

                transferSyntaxes[12] = UID_LittleEndianImplicitTransferSyntax;

                numTransferSyntaxes = 13;
            }
            else {
                /* We prefer explicit transfer syntaxes.
                 * If we are running on a Little Endian machine we prefer
                 * LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
                 */
                if ( gLocalByteOrder == EBO_LittleEndian )  /* defined in dcxfer.h */ {
                    transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
                    transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
                }
                else {
                    transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
                    transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
                }

                transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;

                numTransferSyntaxes = 3;
            }

            break;

        }

        /* accept the Verification SOP Class if presented */
        cond = ASC_acceptContextsWithPreferredTransferSyntaxes (
                    ( *assoc )->params,
                    knownAbstractSyntaxes, DIM_OF ( knownAbstractSyntaxes ),
                    transferSyntaxes, numTransferSyntaxes );

        if ( cond.good() ) {
            /* the array of Storage SOP Class UIDs comes from dcuid.h */
            cond = ASC_acceptContextsWithPreferredTransferSyntaxes (
                        ( *assoc )->params,
                        dcmAllStorageSOPClassUIDs, numberOfDcmAllStorageSOPClassUIDs,
                        transferSyntaxes, numTransferSyntaxes );
        }
    }

    if ( cond.good() ) cond = ASC_acknowledgeAssociation ( *assoc );

    if ( cond.bad() ) {
        ASC_dropAssociation ( *assoc );
        ASC_destroyAssociation ( assoc );
    }

    return cond;
}

OFCondition QtDcmMoveScu::echoSCP ( T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID )
{
    OFCondition cond = DIMSE_sendEchoResponse ( assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, NULL );

    if ( cond.bad() ) {
        qDebug() << "Echo SCP Failed";
    }

    return cond;
}

OFCondition QtDcmMoveScu::storeSCP ( T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID, void* subOpCallbackData )
{
    QtDcmMoveScu * self = static_cast<QtDcmMoveScu * >(subOpCallbackData);
    if (!self) {
        qWarning() << "Cannot cast caller";
        return EC_IllegalCall;
    }

    OFCondition cond = EC_Normal;
    T_DIMSE_C_StoreRQ *req;
    char imageFile[2048];

    req = &msg->msg.CStoreRQ;

    if ( OFFalse ) {
#ifdef _WIN32
        tmpnam ( imageFile );
#else
        strcpy ( imageFile, NULL_DEVICE_NAME );
#endif
    }
    else {
        sprintf ( imageFile, "%s.%s",
                  dcmSOPClassUIDToModality ( req->AffectedSOPClassUID ),
                  req->AffectedSOPInstanceUID );
    }

    self->d->assoc = assoc;

    self->d->imageFile = imageFile;
    DcmFileFormat dcmff;
    self->d->file = &dcmff;

    // store SourceApplicationEntityTitle in metaheader

    if ( assoc && assoc->params ) {
        const char *aet = assoc->params->DULparams.callingAPTitle;

        if ( aet ) dcmff.getMetaInfo()->putAndInsertString ( DCM_SourceApplicationEntityTitle, aet );
    }

    DcmDataset *dset = dcmff.getDataset();

    if ( OFFalse ) {
        cond = DIMSE_storeProvider ( assoc, presID, req, imageFile, OFFalse,
                                     NULL, storeSCPCallback, ( void * ) subOpCallbackData, DIMSE_BLOCKING, 0 );
    }
    else {
        cond = DIMSE_storeProvider ( assoc, presID, req, ( char * ) NULL, OFFalse,
                                     &dset, storeSCPCallback, ( void * ) subOpCallbackData, DIMSE_BLOCKING, 0 );
    }

    return cond;
}

void QtDcmMoveScu::storeSCPCallback ( void *callbackData, T_DIMSE_StoreProgress *progress, T_DIMSE_C_StoreRQ *req, char *imageFile, DcmDataset **imageDataSet, T_DIMSE_C_StoreRSP *rsp, DcmDataset **statusDetail )
{
    QtDcmMoveScu * self = static_cast<QtDcmMoveScu * >(callbackData);
    if (!self) {
        qWarning() << "Cannot cast caller";
        return;
    }
    
    DIC_UI sopClass;
    DIC_UI sopInstance;

    if ( progress->state == DIMSE_StoreEnd ) {
        *statusDetail = NULL;
        if ( ( imageDataSet != NULL ) && ( *imageDataSet != NULL ) && !self->d->bitPreserving && !self->d->ignore ) {
            /* create full path name for the output file */
            OFString ofname;
            OFStandard::combineDirAndFilename ( ofname, self->d->outputDirectory, self->d->imageFile, OFTrue /* allowEmptyDirName */ );

            E_TransferSyntax xfer = self->d->writeTransferSyntax;

            if ( xfer == EXS_Unknown ) xfer = ( *imageDataSet )->getOriginalXfer();

            OFCondition cond = self->d->file->saveFile ( ofname.c_str(), xfer, self->d->sequenceType, self->d->groupLength,
                               self->d->paddingType, OFstatic_cast ( Uint32, self->d->filepad ), OFstatic_cast ( Uint32, self->d->itempad ),
                               ( self->d->useMetaheader ) ? EWM_fileformat : EWM_dataset );

            if ( QFile ( ofname.c_str() ).exists() ) {
                emit self->previewSlice ( QString ( ofname.c_str() ) );
            }

            if ( ( rsp->DimseStatus == STATUS_Success ) && !self->d->ignore ) {
                /* which SOP class and SOP instance ? */
                if ( !DU_findSOPClassAndInstanceInDataSet ( *imageDataSet, sopClass, sopInstance, self->d->correctUIDPadding ) ) {
                    rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
                }
                else if ( strcmp ( sopClass, req->AffectedSOPClassUID ) != 0 ) {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                }
                else if ( strcmp ( sopInstance, req->AffectedSOPInstanceUID ) != 0 ) {
                    rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
                }
            }
        }
    }

    return;
}

OFCondition QtDcmMoveScu::subOpSCP ( T_ASC_Association **subAssoc, void * subOpCallbackData )
{
    T_DIMSE_Message     msg;
    T_ASC_PresentationContextID presID;

    if ( !ASC_dataWaiting ( *subAssoc, 0 ) ) { /* just in case */
        return DIMSE_NODATAAVAILABLE;
    }

    OFCondition cond = DIMSE_receiveCommand ( *subAssoc, DIMSE_BLOCKING, 0, &presID, &msg, NULL );

    if ( cond == EC_Normal ) {
        switch ( msg.CommandField )
        {
        case DIMSE_C_STORE_RQ:
            cond = storeSCP ( *subAssoc, &msg, presID, subOpCallbackData );
            break;
            
        case DIMSE_C_ECHO_RQ:
            cond = echoSCP ( *subAssoc, &msg, presID );
            break;

        default:
            cond = DIMSE_BADCOMMANDTYPE;
            break;
        }
    }

    /* clean up on association termination */
    if ( cond == DUL_PEERREQUESTEDRELEASE ) {
        cond = ASC_acknowledgeRelease ( *subAssoc );
        ASC_dropSCPAssociation ( *subAssoc );
        ASC_destroyAssociation ( subAssoc );
        return cond;
    }
    else if ( cond == DUL_PEERABORTEDASSOCIATION ) {
    }
    else if ( cond != EC_Normal ) {
        OFString temp_str;
        qDebug() << "DIMSE failure (aborting sub-association): " << DimseCondition::dump ( temp_str, cond ).c_str();
        /* some kind of error so abort the association */
        cond = ASC_abortAssociation ( *subAssoc );
    }

    if ( cond != EC_Normal ) {
        ASC_dropAssociation ( *subAssoc );
        ASC_destroyAssociation ( subAssoc );
    }

    return cond;
}

void QtDcmMoveScu::subOpCallback ( void * caller, T_ASC_Network *aNet, T_ASC_Association **subAssoc )
{
    if ( !caller )
        return;

    QtDcmMoveScu * self = static_cast<QtDcmMoveScu * >(caller);
    if (!self) {
        qWarning() << "Cannot cast caller";
        return;
    }
    
    if ( self->d->slicesCount ) {
        emit self->updateProgress ( self->d->progressTotal + ( int ) ( ( ( float ) ( self->d->step * ( self->d->progressSerie ) / self->d->slicesCount ) ) ) );
    }

    if ( aNet == NULL ) return; /* help no net ! */

    if ( *subAssoc == NULL ) {
        /* negotiate association */
        acceptSubAssoc ( aNet, subAssoc );
    }
    else {
        /* be a service class provider */
        subOpSCP ( subAssoc, caller );
    }

    self->d->progressSerie ++;
}

void QtDcmMoveScu::moveCallback ( void *caller, T_DIMSE_C_MoveRQ * req, int responseCount, T_DIMSE_C_MoveRSP * rsp )
{
    if ( !caller )
        return;

    QtDcmMoveScu * self = static_cast<QtDcmMoveScu * >(caller);
    if (!self) {
        qWarning() << "Cannot cast caller";
        return;
    }

    if ( responseCount == 1 ) {
        self->d->progressSerie = 0;
    }

    self->d->slicesCount = rsp->NumberOfRemainingSubOperations + rsp->NumberOfFailedSubOperations + rsp->NumberOfWarningSubOperations + rsp->NumberOfCompletedSubOperations;

    OFString temp_str;

    DIMSE_dumpMessage ( temp_str, *rsp, DIMSE_INCOMING );

    qDebug() << "Move Response " << responseCount << ":";
    foreach (const QString &msg, QString ( temp_str.c_str() ).split('\n')) {
        qDebug() << msg;   
    }
}

void QtDcmMoveScu::substituteOverrideKeys ( DcmDataset & dset )
{
    if ( d->overrideKeys.isEmpty()) {
        return; /* nothing to do */
    }

    /* copy the override keys */
    DcmDataset keys ( d->overrideKeys );

    /* put the override keys into dset replacing existing tags */
    unsigned long elemCount = keys.card();

    for ( unsigned long i = 0; i < elemCount; i++ ) {
        DcmElement *elem = keys.remove ( ( unsigned long ) 0 );
        dset.insert ( elem, OFTrue );
    }
}

OFCondition QtDcmMoveScu::moveSCU ( T_ASC_Association * assoc, const char *fname )
{
    T_DIMSE_C_MoveRQ    req;
    T_DIMSE_C_MoveRSP   rsp;
    DIC_US              msgId = assoc->nextMsgID++;
    DcmDataset          *rspIds = NULL;
    const char          *sopClass;
    DcmDataset          *statusDetail = NULL;

    QuerySyntax querySyntax[3] =
    {
        { UID_FINDPatientRootQueryRetrieveInformationModel,
          UID_MOVEPatientRootQueryRetrieveInformationModel },
        { UID_FINDStudyRootQueryRetrieveInformationModel,
          UID_MOVEStudyRootQueryRetrieveInformationModel },
        { UID_RETIRED_FINDPatientStudyOnlyQueryRetrieveInformationModel,
          UID_RETIRED_MOVEPatientStudyOnlyQueryRetrieveInformationModel }
    };


    DcmFileFormat file;
    if ( fname != NULL ) {
        if ( file.loadFile ( fname ).bad() ) {
            qDebug() << "bad DICOM file: " << QString ( fname );
            return DIMSE_BADDATA;
        }
    }

    /* replace specific keys by those in overrideKeys */
    substituteOverrideKeys ( *file.getDataset() );

    sopClass = querySyntax[d->queryModel].moveSyntax;

    /* which presentation context should be used */
    d->presId = ASC_findAcceptedPresentationContextID ( d->assoc, sopClass );

    if ( d->presId == 0 ) return DIMSE_NOVALIDPRESENTATIONCONTEXTID;

    req.MessageID = msgId;

    strcpy ( req.AffectedSOPClassUID, sopClass );

    req.Priority = DIMSE_PRIORITY_MEDIUM;

    req.DataSetType = DIMSE_DATASET_PRESENT;

    if ( d->moveDestination == NULL ) {
        /* set the destination to be me */
        ASC_getAPTitles ( assoc->params, req.MoveDestination,
                          NULL, NULL );
    }
    else {
        strcpy( req.MoveDestination, d->moveDestination );
    }


    const OFCondition cond = DIMSE_moveUser ( assoc, 
                                              d->presId, 
                                              &req, 
                                              file.getDataset(), 
                                              moveCallback, 
                                              ( void* ) this, 
                                              d->blockMode, 
                                              d->dimseTimeout, 
                                              d->net, 
                                              subOpCallback, 
                                              ( void* ) this, 
                                              &rsp, 
                                              &statusDetail, 
                                              &rspIds, 
                                              d->ignorePendingDatasets );
    
    if ( rspIds != NULL ) delete rspIds;

    return cond;
}


OFCondition QtDcmMoveScu::cmove ( T_ASC_Association * assoc, const char *fname )
{
    OFCondition cond = EC_Normal;
    int n = ( int ) d->repeatCount;

    while ( cond.good() && n-- ) {
        cond = moveSCU ( assoc, fname );
    }

    return cond;
}
