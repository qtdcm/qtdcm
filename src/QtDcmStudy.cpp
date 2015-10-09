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

#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmPatient.h>

class QtDcmStudy::Private
{
public:
    QString id; /** Dicom study identificator */
    QString description; /** Dicom study description */
    QDate date; /** Dicom study date */
    QString time; /** Dicom study time */
    QList<QtDcmSerie> series; /** List of series in the study */
    QtDcmPatient patient; /** Patient corresponding to the study */
};

QtDcmStudy::QtDcmStudy() : d ( new QtDcmStudy::Private ) {}

QtDcmStudy::~QtDcmStudy()
{
    delete d;
}

QString QtDcmStudy::id() const
{
    return d->id;
}

void QtDcmStudy::setId ( const QString & id )
{
    d->id = id;
}

QString QtDcmStudy::description() const
{
    return d->description;
}

void QtDcmStudy::setDescription ( const QString & description )
{
    d->description = description;
}

QDate QtDcmStudy::date() const
{
    return d->date;
}

void QtDcmStudy::setDate ( const QDate & date )
{
    d->date = date;
}

QString QtDcmStudy::time() const
{
    return d->time;
}

void QtDcmStudy::setTime ( const QString & time )
{
    d->time = time;
}

QList<QtDcmSerie> QtDcmStudy::series() const
{
    return d->series;
}

void QtDcmStudy::setSeries ( const QList<QtDcmSerie> & series )
{
    d->series = series;
}

QtDcmPatient QtDcmStudy::patient() const
{
    return d->patient;
}

void QtDcmStudy::setPatient ( const QtDcmPatient & patient )
{
    d->patient = patient;
}

void QtDcmStudy::addSerie ( const QtDcmSerie & serie )
{
    d->series.append ( serie );
}

void QtDcmStudy::removeSerie ( int index )
{
    d->series.removeAt ( index );
}
