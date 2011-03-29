/*
 * QtDcmFindCallback.cpp
 *
 *  Created on: 29 mars 2011
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

#include <QtDcmFindCallback.h>

class QtDcmFindCallbackPrivate
{
    public:
        int type;
};

QtDcmFindCallback::QtDcmFindCallback() :
    d(new QtDcmFindCallbackPrivate)
{
    d->type = QtDcmFindCallback::PATIENT;
}

QtDcmFindCallback::QtDcmFindCallback(int type) :
    d(new QtDcmFindCallbackPrivate)
{
    d->type = type;
}

void
QtDcmFindCallback::callback(T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers)
{
    OFString info;

    responseIdentifiers->print(std::cout);
//    responseIdentifiers->findAndGetOFString(DCM_PatientAge, info);
//    qDebug() << "Patient Age : " << QString(info.c_str());
//
//    responseIdentifiers->findAndGetOFString(DCM_PatientID, info);
//    qDebug() << "Patient ID : " << QString(info.c_str());
}
