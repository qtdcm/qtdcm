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

class QtDcmMoveDicomdirPrivate
{
    public:
        QtDcmManager * manager;
        QString outputDir;
        DcmItem * dcmObject;
        DcmStack dicomdirItems;
        QList<QString> filenames;
        QList<QString> series;
};

QtDcmMoveDicomdir::QtDcmMoveDicomdir(QObject * parent) :
    d(new QtDcmMoveDicomdirPrivate)
{
    d->manager = dynamic_cast<QtDcmManager *> (parent);
}

QtDcmMoveDicomdir::~QtDcmMoveDicomdir()
{
}

void
QtDcmMoveDicomdir::setDcmItem(DcmItem * item)
{
    d->dcmObject = item;
}

void
QtDcmMoveDicomdir::setSeries(QList<QString> series)
{
    d->series = series;
}

void
QtDcmMoveDicomdir::setOutputDir(QString dir)
{
    d->outputDir = dir;
}

void
QtDcmMoveDicomdir::run()
{
    int step = (int)(100.0 / d->series.size());
    int progress = 0;
    for (int s = 0; s < d->series.size(); s++) {
        QDir serieDir(d->outputDir + QDir::separator() + d->series.at(s));
        if (!serieDir.exists())
            QDir(d->outputDir).mkdir(d->series.at(s));

        d->filenames.clear();
        bool proceed = false;
        static const OFString Patient("PATIENT");
        static const OFString Study("STUDY");
        static const OFString Series("SERIES");
        static const OFString Image("IMAGE");

        // Loading all the dicomdir items in a stack
        DcmStack itemsTmp;
        if (!d->dcmObject->findAndGetElements(DCM_Item, itemsTmp).good())
            return;

        while (itemsTmp.card() > 0) {
            DcmItem * obj = (DcmItem*) itemsTmp.top();
            d->dicomdirItems.push(itemsTmp.top());
            itemsTmp.pop();
        }

        OFString strName;
        OFString strDate;
        //Unstacking and loading the different lists
        while (d->dicomdirItems.card() > 0) {
            DcmItem* lobj = (DcmItem*) d->dicomdirItems.top();
            DcmStack dirent;

            OFCondition condition = lobj->findAndGetElements(DCM_DirectoryRecordType, dirent);
            if (!condition.good()) {
                d->dicomdirItems.pop();
                continue;
            }
            while (dirent.card()) {
                DcmElement* elt = (DcmElement*) dirent.top();
                OFString cur;
                elt->getOFStringArray(cur);
                if (cur == Series) {
                    DcmElement* lelt;
                    QMap<QString, QString> infosMap;
                    if (lobj->findAndGetElement(DCM_SeriesInstanceUID, lelt).good()) {
                        OFString strID;
                        lelt->getOFStringArray(strID);
                        proceed = (QString(strID.c_str()) == d->series.at(s));
                    }
                }
                if ((cur == Image) && proceed) {
                    DcmElement* lelt;
                    QMap<QString, QString> infosMapImage;
                    if (lobj->findAndGetElement(DCM_ReferencedFileID, lelt).good()) {
                        OFString strFilename;
                        lelt->getOFStringArray(strFilename);
                        d->filenames.append(this->fixFilename(QString(strFilename.c_str())));
                    }
                }
                dirent.pop();
            }
            d->dicomdirItems.pop();
        }
        d->dicomdirItems.clear();

        for (int i = 0; i < d->filenames.size(); i++) {
            QFile image(d->filenames.at(i));
            if (image.exists()) {
                QString zeroStr;
                zeroStr.fill(QChar('0'), 5 - QString::number(i).size());
                image.copy(serieDir.absolutePath() + QDir::separator() + "ima" + zeroStr + QString::number(i));
                emit updateProgress(progress + (int)(((float) (step * (i + 1) / d->filenames.size()))));
            }
        }
        progress += step;
    }
    exit();
}

QString
QtDcmMoveDicomdir::fixFilename(QString name)
{
    QString basename = QFileInfo(d->manager->getDicomdir()).path();
    name.replace(QChar('/'), QDir::separator()).replace(QChar('\\'), QDir::separator());
    QString filename = basename + QDir::separator() + name.toUpper();
    if (!QFile(filename).exists())
        filename = basename + QDir::separator() + name.toLower();
    return filename;
}
