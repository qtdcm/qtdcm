/*
 * QtDcmSerie.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmImage.h>
#include <QtDcmStudy.h>

#include <QtDcmSerie.h>

class QtDcmSeriePrivate
{

public:
    QString id; /** Serie dicom identificator */
    QString description; /** Serie description */
    QString date; /** Study date */
    QList<QtDcmImage *> images; /** List of images in the serie */
    QtDcmStudy * p_study; /** Pointer on the parent study */
};

QtDcmSerie::QtDcmSerie() : d ( new QtDcmSeriePrivate ) {}

QString QtDcmSerie::getId()
{
    return d->id;
}

void QtDcmSerie::setId ( QString id )
{
    d->id = id;
}

QString QtDcmSerie::getDate()
{
    return d->date;
}

void QtDcmSerie::setDate ( QString date )
{
    d->date = date;
}

QString QtDcmSerie::getDescription()
{
    return d->description;
}

void QtDcmSerie::setDescription ( QString description )
{
    d->description = description;
}

QList<QtDcmImage *> QtDcmSerie::getImages()
{
    return d->images;
}

void QtDcmSerie::setImages ( QList<QtDcmImage *> images )
{
    d->images = images;
}

QtDcmStudy * QtDcmSerie::getStudy()
{
    return d->p_study;
}

void QtDcmSerie::setStudy ( QtDcmStudy * study )
{
    d->p_study = study;
}


