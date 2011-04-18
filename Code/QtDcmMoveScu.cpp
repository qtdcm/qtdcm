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

#include <QtDcmManager.h>
#include <QtDcmMoveScu.h>



class QtDcmMoveScuPrivate
{
    public:
        QtDcmManager * manager;
        QString uid;
        QString outputDir;
};

QtDcmMoveScu::QtDcmMoveScu(QObject * parent) : d(new QtDcmMoveScuPrivate)
{
    d->manager = dynamic_cast<QtDcmManager *> (parent);
}

QtDcmMoveScu::~QtDcmMoveScu()
{
}

void
QtDcmMoveScu::setSerieUID(QString uid)
{
    d->uid = uid;
}

void
QtDcmMoveScu::setOutputDir(QString dir)
{
    d->outputDir = dir;
}

void
QtDcmMoveScu::run()
{
    qDebug() << "Start thread !!!";
}

