/*
 * QtDcmImage.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmSerie.h>
#include <QtDcmImage.h>

class QtDcmImagePrivate
{
    public:
        QString id; /** Dicom ID of the image */
        QString filename; /** Filename path of the dicom image */
        QtDcmSerie * p_serie; /** Parent serie */
};

QtDcmImage::QtDcmImage() : d(new QtDcmImagePrivate) {}

QString QtDcmImage::getId()
  {
    return d->id;
  }

void QtDcmImage::setId( QString id )
  {
    d->id = id;
  }

QString QtDcmImage::getFilename()
  {
    return d->filename;
  }

void QtDcmImage::setFilename( QString filename )
  {
    d->filename = filename;
  }

QtDcmSerie * QtDcmImage::getSerie()
  {
    return d->p_serie;
  }

void QtDcmImage::setSerie( QtDcmSerie * serie )
  {
    d->p_serie = serie;
  }
