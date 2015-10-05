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


#ifndef QTDCMMOVESCU_H_
#define QTDCMMOVESCU_H_

#include <QtGui>
#include <dcmtk/ofstd/ofstd.h>
#include <dcmtk/ofstd/ofconapp.h>
#include <dcmtk/dcmnet/dicom.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcdict.h>
#include <dcmtk/dcmdata/cmdlnarg.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/dcmdata/dcuid.h>      /* for dcmtk version name */
#include <dcmtk/dcmdata/dcdicent.h>
#include <dcmtk/dcmdata/dcostrmz.h>   /* for dcmZlibCompressionLevel */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
#define INCLUDE_CERRNO
#define INCLUDE_CCTYPE
#define INCLUDE_CSIGNAL

#include <QtDcmConvert.h>

class QtDcmMoveScu : public QThread
{
    Q_OBJECT

public:
    typedef struct
    {
        T_ASC_Association *assoc;
        T_ASC_PresentationContextID presId;
    } MyCallbackInfo;
    
    typedef enum
    {
        QMPatientRoot = 0,
        QMStudyRoot = 1,
        QMPatientStudyOnly = 2
    } QueryModel;

    typedef struct
    {
        const char *findSyntax;
        const char *moveSyntax;
    } QuerySyntax;
    
    enum eMoveMode {
        IMPORT = 0, 
        PREVIEW
    };
    
    QtDcmMoveScu ( QObject * parent = 0);

    virtual ~QtDcmMoveScu();

    void setMode ( eMoveMode eMoveMode );

    void setImageId ( const QString & id );

    void setOutputDir ( const QString & dir );

    void setImportDir ( const QString & dir );

    void setSeries ( const QStringList & series );

    void run();

public slots:
    void onStopMove();
    
signals:
    void updateProgress ( int i );
    void previewSlice ( const QString & filename );
    void serieMoved(const QString & directory, const QString & uid, int number);

protected:
    OFCondition move ( const QString & uid );

    void addOverrideKey ( const QString & key );

    OFCondition addPresentationContext ( T_ASC_Parameters *params, T_ASC_PresentationContextID pid, const char* abstractSyntax, E_TransferSyntax preferredTransferSyntax );

    OFCondition cmove ( T_ASC_Association * assoc, const char *fname );

    static OFCondition acceptSubAssoc ( T_ASC_Network * aNet, T_ASC_Association ** assoc );

    static OFCondition echoSCP ( T_ASC_Association * assoc, T_DIMSE_Message * msg, T_ASC_PresentationContextID presID );

    static OFCondition storeSCP ( T_ASC_Association *assoc, T_DIMSE_Message *msg, T_ASC_PresentationContextID presID, void* subOpCallbackData );

    static void storeSCPCallback ( void* caller, T_DIMSE_StoreProgress* progress, T_DIMSE_C_StoreRQ* req, char* imageFile, DcmDataset** imageDataSet, T_DIMSE_C_StoreRSP* rsp, DcmDataset** statusDetail );

    static OFCondition subOpSCP ( T_ASC_Association **subAssoc, void * subOpCallbackData );

    static void subOpCallback ( void * /*subOpCallbackData*/, T_ASC_Network *aNet, T_ASC_Association **subAssoc );

    static void moveCallback ( void *caller, T_DIMSE_C_MoveRQ * req, int responseCount, T_DIMSE_C_MoveRSP * rsp );

    void substituteOverrideKeys ( DcmDataset &dset );

    OFCondition moveSCU ( T_ASC_Association * assoc, const char *fname );

private:
    class Private;
    Private * d;
};

#endif /* QTDCMMOVESCU_H_ */
// kate: indent-mode cstyle; space-indent on; indent-width 0;
