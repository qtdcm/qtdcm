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

#ifndef QTDCMIMAGE_H_
#define QTDCMIMAGE_H_

#include <QtGui>

class QtDcmSerie;
class QtDcmImagePrivate;

/**
 * This class is a representation of a dicom image. It contains dicom id and filename of the image.
 * There is also a pointer to the parent serie (not used for the moment)
 *
 */

class QtDcmImage : public QObject
{
    Q_OBJECT

private:
    QtDcmImagePrivate * d;

public:
    /**
     * Default constructor
     */
    QtDcmImage();

    /**
     * Default destructor
     */
    virtual ~QtDcmImage() {};

    /**
     * Id getter
     *
     * @return _id
     */
    QString getId();
    /**
     * Id setter
     *
     * @param id a QString
     */
    void setId ( QString id );

    /**
     * Image filename getter
     *
     * @return _filename
     */
    QString getFilename();

    /**
     * Image filename setter
     *
     * @param filename a QString containing absolute path
     */
    void setFilename ( QString filename );

    /**
     * Parent serie getter
     *
     * @return _p_serie the pointer on the parent serie object
     * @see QtDcmSerie
     */
    QtDcmSerie * getSerie();

    /**
     * Parent serie setter
     *
     * @param serie a pointer on the parent serie object
     * @see QtDcmSerie
     */
    void setSerie ( QtDcmSerie * serie );
};

#endif /* QTDCMIMAGE_H_ */
