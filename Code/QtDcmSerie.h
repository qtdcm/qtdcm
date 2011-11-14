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


#ifndef QTDCMSERIE_H_
#define QTDCMSERIE_H_

#include <QtGui>

class QtDcmSeriePrivate;
class QtDcmStudy;
class QtDcmImage;

/**
 * This class is a representation of a dicom serie
 */

class QtDcmSerie : public QObject
{
    Q_OBJECT

private:
    QtDcmSeriePrivate * d;

public:
    /**
     * Default constructor
     */
    QtDcmSerie();

    /**
     * Default destructor
     */
    virtual ~QtDcmSerie() {};

    /**
     * Serie Id getter
     *
     * @return _id a QString
     */
    QString getId();

    /**
     * Serie Id setter
     *
     * @param id is a QString
     */
    void setId ( QString id );

    /**
     * Serie date getter
     *
     * @return _date a QString
     */
    QString getDate();

    /**
     * Serie date setter
     *
     * @param date is a QString
     */
    void setDate ( QString date );

    /**
     * Serie description getter
     *
     * @return _description a QString
     */
    QString getDescription();

    /**
     * Serie description setter
     *
     * @param description a QString
     */
    void setDescription ( QString description );

    /**
     * Image list getter
     *
     * @return _images a QList of images
     * @see QtDcmImage
     */
    QList<QtDcmImage *> getImages();

    /**
     * Image list setter
     *
     * @param images a QList of images
     * @see QtDcmImage
     */
    void setImages ( QList<QtDcmImage *> images );

    /**
     * Parent study getter
     *
     * @return _p_study a pointer on the parent study
     * @see QtDcmStudy
     */
    QtDcmStudy * getStudy();

    /**
     * Parent study setter
     *
     * @param study a pointer on the parent study
     * @see QtDcmStudy
     */
    void setStudy ( QtDcmStudy * study );
};

#endif /* QTDCMSERIE_H_ */
