/*
 * QtDcmStudy.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: aabadie
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
