/*
 * QtDcmMoveDicomdir.cpp
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

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#include <QtDcmManager.h>
#include <QtDcmMoveDicomdir.h>
#include <QtDcmConvert.h>

class QtDcmMoveDicomdirPrivate {
public:
    QtDcmManager * manager;
    QtDcmConvert * converter;
    QString outputDir;
    QString importDir;
    DcmItem * dcmObject;
    DcmStack dicomdirItems;
    QList<QString> filenames;
    QList<QString> series;
    QtDcmMoveDicomdir::mode mode;
    int index;
};

QtDcmMoveDicomdir::QtDcmMoveDicomdir ( QObject * parent ) :
        d ( new QtDcmMoveDicomdirPrivate ) {
    d->manager = dynamic_cast<QtDcmManager *> ( parent );
    d->converter = new QtDcmConvert ( this );
    d->mode = QtDcmMoveDicomdir::IMPORT;
}

QtDcmMoveDicomdir::~QtDcmMoveDicomdir() {
}

void QtDcmMoveDicomdir::setMode(QtDcmMoveDicomdir::mode mode)
{
    d->mode = mode;
}

QtDcmMoveDicomdir::mode QtDcmMoveDicomdir::getMode()
{
    return d->mode;
}

void
QtDcmMoveDicomdir::setDcmItem ( DcmItem * item ) {
    d->dcmObject = item;
}

void
QtDcmMoveDicomdir::setSeries ( QList<QString> series ) {
    d->series = series;
}

void QtDcmMoveDicomdir::setIndex(int index)
{
    d->index = index;
}


void
QtDcmMoveDicomdir::setOutputDir ( QString dir ) {
    d->outputDir = dir;
}

void
QtDcmMoveDicomdir::setImportDir ( QString dir ) {
    d->importDir = dir;
}

void
QtDcmMoveDicomdir::run() {
    int step = ( int ) ( 100.0 / d->series.size() );
    int progress = 0;
    for ( int s = 0; s < d->series.size(); s++ ) {
        QDir serieDir ( d->outputDir + QDir::separator() + d->series.at ( s ) );
        if ( !serieDir.exists() )
            QDir ( d->outputDir ).mkdir ( d->series.at ( s ) );

        d->filenames.clear();
        bool proceed = false;
        bool proceedIndex = false;
        static const OFString Patient ( "PATIENT" );
        static const OFString Study ( "STUDY" );
        static const OFString Series ( "SERIES" );
        static const OFString Image ( "IMAGE" );

        // Loading all the dicomdir items in a stack
        DcmStack itemsTmp;
        if ( !d->dcmObject->findAndGetElements ( DCM_Item, itemsTmp ).good() )
            return;

        while ( itemsTmp.card() > 0 ) {
            d->dicomdirItems.push ( itemsTmp.top() );
            itemsTmp.pop();
        }

        OFString strName;
        OFString strDate;
        OFString strDesc;
        //Unstacking and loading the different lists
        while ( d->dicomdirItems.card() > 0 ) {
            DcmItem* lobj = ( DcmItem* ) d->dicomdirItems.top();
            DcmStack dirent;

            OFCondition condition = lobj->findAndGetElements ( DCM_DirectoryRecordType, dirent );
            if ( !condition.good() ) {
                d->dicomdirItems.pop();
                continue;
            }
            while ( dirent.card() ) {
                DcmElement* elt = ( DcmElement* ) dirent.top();
                OFString cur;
                elt->getOFStringArray ( cur );
                if (cur ==Patient)
                {
                    DcmElement* lelt;
                    if ( lobj->findAndGetElement ( DCM_PatientName, lelt ).good() )
                        lelt->getOFStringArray ( strName );
                }
                if (cur == Study)
                {
                    DcmElement* lelt;
                    if ( lobj->findAndGetElement ( DCM_StudyDate, lelt ).good() )
                        lelt->getOFStringArray ( strDate );
                }
                if ( cur == Series ) {
                    DcmElement* lelt;
                    if ( lobj->findAndGetElement ( DCM_SeriesInstanceUID, lelt ).good() ) {
                        OFString strID;
                        lelt->getOFStringArray ( strID );
                        proceed = ( QString ( strID.c_str() ) == d->series.at ( s ) );
                    }
                    if (proceed) {
                        if ( lobj->findAndGetElement ( DCM_SeriesDescription, lelt ).good() )
                            lelt->getOFStringArray ( strDesc );
                    }
                }
                if ( ( cur == Image ) && proceed ) {
                    DcmElement* lelt;
                    if ( lobj->findAndGetElement ( DCM_InstanceNumber, lelt ).good() ) {
                        OFString strNumber;
                        lelt->getOFStringArray ( strNumber );
                        if (d->mode == QtDcmMoveDicomdir::PREVIEW)
                            proceedIndex = (QString ( strNumber.c_str() ).toInt() == d->index);
                    }
                    if ( lobj->findAndGetElement ( DCM_ReferencedFileID, lelt ).good() ) {
                        OFString strFilename;
                        lelt->getOFStringArray ( strFilename );
                        if (d->mode == QtDcmMoveDicomdir::IMPORT)
                            d->filenames.append ( this->fixFilename ( QString ( strFilename.c_str() ) ) );
                        else
                        {
                            if (proceedIndex)
                                d->filenames.append ( this->fixFilename ( QString ( strFilename.c_str() ) ) );
                        }

                    }
                    if ( lobj->findAndGetElement ( DCM_SeriesDescription, lelt ).good() )
                        lelt->getOFStringArray ( strDesc );
                }
                dirent.pop();
            }
            d->dicomdirItems.pop();
        }
        d->dicomdirItems.clear();

        if (d->mode == QtDcmMoveDicomdir::IMPORT)
        {
            for ( int i = 0; i < d->filenames.size(); i++ ) {
                QFile image ( d->filenames.at ( i ) );
                if ( image.exists() ) {
                    QString zeroStr;
                    zeroStr.fill ( QChar ( '0' ), 5 - QString::number ( i ).size() );
                    image.copy ( serieDir.absolutePath() + QDir::separator() + "ima" + zeroStr + QString::number ( i ));
                    emit updateProgress ( progress + ( int ) ( ( ( float ) ( step * ( i + 1 ) / d->filenames.size() ) ) ) );
                }
            }
            qDebug() << QString(strDesc.c_str());
            d->converter->setInputDirectory ( serieDir.absolutePath() );
            d->converter->setOutputFilename( QString(strDate.c_str()).replace(" ", "_") + "_" + QString(strName.c_str()).replace(" ", "_").replace("^", "_") + "_" + QString(strDesc.c_str()).replace(" ", "_") + ".nii" );
            d->converter->setOutputDirectory ( d->importDir );
            d->converter->convert();

            progress += step;
        }
        else
        {
            if (!d->filenames.isEmpty())
            {
                QFile filename (d->filenames.first());
                emit previewSlice(filename.fileName());
            }
        }
    }
    exit();
}

QString
QtDcmMoveDicomdir::fixFilename ( QString name ) {
    QString basename = QFileInfo ( d->manager->getDicomdir() ).path();
    name.replace ( QChar ( '/' ), QDir::separator() ).replace ( QChar ( '\\' ), QDir::separator() );
    QString filename = basename + QDir::separator() + name.toUpper();
    if ( !QFile ( filename ).exists() )
        filename = basename + QDir::separator() + name.toLower();
    return filename;
}
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
