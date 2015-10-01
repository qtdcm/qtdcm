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

#ifndef QTDCMSTUDY_H_
#define QTDCMSTUDY_H_

#include <QtGui>
#include <QList>

class QtDcmPatient;
class QtDcmSerie;

/**
 * This class is representation of a Dicom study.
 */

class QtDcmStudy
{
public:
    /**
     * Default constructor
     */
    QtDcmStudy();

    /**
     * Default destructor
     */
    virtual ~QtDcmStudy();

    /**
     * Study Id getter
     *
     * @return _id as a QString
     */
    QString id() const;

    /**
     * Study Id setter
     *
     * @param id as a QString
     */
    void setId ( const QString & id );

    /**
     * Study description getter
     *
     * @return _description as a QString
     */
    QString description() const;

    /**
     * Study description setter
     *
     * @param description as a QString
     */
    void setDescription ( const QString & description );

    /**
     * Study date getter
     *
     * @return _date as a QString
     */
    QDate date() const;

    /**
     * Study date setter
     *
     * @param date as a QString
     */
    void setDate ( const QDate & date );

    /**
     * Study time getter
     *
     * @return _time as a QString
     */
    QString time() const;

    /**
     * Study time setter
     *
     * @param time as a QString
     */
    void setTime ( const QString & time );

    /**
     * List of series in the study getter
     *
     * @return _series as a QList of series
     * @see QtDcmSerie
     */
    QList<QtDcmSerie> series() const;

    /**
     * List of series in the study setter
     *
     * @param series as a QList of series
     * @see QtDcmSerie
     */
    void setSeries ( const QList<QtDcmSerie> & series );

    /**
     * Patient getter
     *
     * @return _p_patient as a patient
     * @see QtDcmPatient
     */
    QtDcmPatient  patient() const;

    /**
     * Patient setter
     *
     * @param patient
     * @see QtDcmPatient
     */
    void setPatient ( const QtDcmPatient & patient );

    /**
     * Add serie in the list
     */
    void addSerie ( const QtDcmSerie & serie );

    /**
     * Remove serie at position index
     */
    void removeSerie ( int index );
    
private:
    class Private;
    Private * d;
};

#endif /* QTDCMSTUDY_H_ */
