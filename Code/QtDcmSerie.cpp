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


