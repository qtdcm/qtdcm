/*
 * QtDcmMoveScu.h
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
 */

#ifndef QTDCMMOVESCU_H_
#define QTDCMMOVESCU_H_

#include <QtGui>
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#define INCLUDE_CCTYPE
#define INCLUDE_CSIGNAL

#include <QtDcmConvert.h>

class QtDcmMoveScuPrivate;

class QtDcmMoveScu : public QThread
{
     Q_OBJECT
public:
    QtDcmMoveScu(QObject * parent);

    virtual
    ~QtDcmMoveScu();

    void
    setOutputDir(QString dir);

    void
    setImportDir(QString dir);

    void
    setSeries(QList<QString> series);

    void
    run();

signals:
    void
    updateProgress(int i);

protected:
    typedef struct {
        T_ASC_Association *assoc;
        T_ASC_PresentationContextID presId;
    } MyCallbackInfo;

        typedef enum {
        QMPatientRoot = 0,
        QMStudyRoot = 1,
        QMPatientStudyOnly = 2
    } QueryModel;

    typedef struct {
        const char *findSyntax;
        const char *moveSyntax;
    } QuerySyntax;

    OFCondition
    move(QString uid);

    void
    addOverrideKey(QString key);

    OFCondition
    addPresentationContext(T_ASC_Parameters *params, T_ASC_PresentationContextID pid, const char* abstractSyntax, E_TransferSyntax preferredTransferSyntax);

    OFCondition
    cmove(T_ASC_Association * assoc, const char *fname);

    static OFCondition
    acceptSubAssoc(T_ASC_Network * aNet, T_ASC_Association ** assoc);

    static OFCondition
    echoSCP(T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID);

    static OFCondition
    storeSCP(T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID, void* subOpCallbackData);

    static void
    storeSCPCallback(void* callbackData, T_DIMSE_StoreProgress* progress, T_DIMSE_C_StoreRQ* req, char* imageFile, DcmDataset** imageDataSet, T_DIMSE_C_StoreRSP* rsp, DcmDataset** statusDetail);

    static OFCondition
    subOpSCP(T_ASC_Association **subAssoc, void * subOpCallbackData);

    static void
    subOpCallback(void * /*subOpCallbackData*/, T_ASC_Network *aNet, T_ASC_Association **subAssoc);

    static void
    moveCallback(void *callbackData, T_DIMSE_C_MoveRQ * req, int responseCount, T_DIMSE_C_MoveRSP * rsp);

    void
    substituteOverrideKeys(DcmDataset *dset);

    OFCondition
    moveSCU(T_ASC_Association * assoc, const char *fname);

private:
    QtDcmMoveScuPrivate * d;

    //For dcmtk stuff
    T_ASC_Network*        net;              /* the global DICOM network */
    T_ASC_Parameters*     params;
    T_ASC_Association*  assoc;
    T_ASC_PresentationContextID presId;
    
    DcmFileFormat*      file;
    char*               imageFile;

    QueryModel queryModel;
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
    OFBool useStoreSCP;
    DcmDataset *overrideKeys;
    OFString outputDirectory;

    int slicesCount;
    int progressTotal;
    int progressSerie;
    int step;
};

#endif /* QTDCMMOVESCU_H_ */
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
