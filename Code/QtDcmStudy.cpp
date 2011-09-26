/*
    <one line to give the library's name and an idea of what it does.>
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

class QtDcmStudyPrivate
{

public:
    QString id; /** Dicom study identificator */
    QString description; /** Dicom study description */
    QDate date; /** Dicom study date */
    QString time; /** Dicom study time */
    QList<QtDcmSerie *> series; /** List of series in the study */
    QtDcmPatient * p_patient; /** Patient corresponding to the study */
};

QtDcmStudy::QtDcmStudy() : d ( new QtDcmStudyPrivate ) {}

QString QtDcmStudy::getId()
{
    return d->id;
}

void QtDcmStudy::setId ( QString id )
{
    d->id = id;
}

QString QtDcmStudy::getDescription()
{
    return d->description;
}

void QtDcmStudy::setDescription ( QString description )
{
    d->description = description;
}

QDate QtDcmStudy::getDate()
{
    return d->date;
}

void QtDcmStudy::setDate ( QDate date )
{
    d->date = date;
}

QString QtDcmStudy::getTime()
{
    return d->time;
}

void QtDcmStudy::setTime ( QString time )
{
    d->time = time;
}

QList<QtDcmSerie *> QtDcmStudy::getSeries()
{
    return d->series;
}

void QtDcmStudy::setSeries ( QList<QtDcmSerie *> series )
{
    d->series = series;
}

QtDcmPatient * QtDcmStudy::getPatient()
{
    return d->p_patient;
}

void QtDcmStudy::setPatient ( QtDcmPatient * patient )
{
    d->p_patient = patient;
}

void QtDcmStudy::addSerie ( QtDcmSerie * serie )
{
    d->series.append ( serie );
}

void QtDcmStudy::removeSerie ( int index )
{
    d->series.removeAt ( index );
}
