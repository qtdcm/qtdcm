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



#include <QtDcmSerie.h>
#include <QtDcmImage.h>

class QtDcmImage::Private
{
public:
    QString id; /** Dicom ID of the image */
    QString filename; /** Filename path of the dicom image */
    QtDcmSerie serie; /** Parent serie */
};

QtDcmImage::QtDcmImage() : d(new QtDcmImage::Private) {}

QtDcmImage::~QtDcmImage()
{
    delete d;
}

QString QtDcmImage::id() const
{
    return d->id;
}

void QtDcmImage::setId( const QString & id )
{
    d->id = id;
}

QString QtDcmImage::filename() const
{
    return d->filename;
}

void QtDcmImage::setFilename( const QString & filename )
{
    d->filename = filename;
}

QtDcmSerie QtDcmImage::serie() const
{
    return d->serie;
}

void QtDcmImage::setSerie( const QtDcmSerie & serie )
{
    d->serie = serie;
}
