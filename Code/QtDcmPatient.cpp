/*
 * QtDcmPatient.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmImage.h>
#include <QtDcmSerie.h>
#include <QtDcmStudy.h>
#include <QtDcmPatient.h>

class QtDcmPatientPrivate
{
    public:
        QString id; /** Patient dicom identificator */
        QString name; /** Patient name as found in the dicom format*/
        QString birthdate; /** Patient birth date */
        QString sex; /** Patient sex */

        QList<QtDcmStudy *> studies; /** List of a study for this patient */
};

QtDcmPatient::QtDcmPatient() : d(new QtDcmPatientPrivate)
{}

QString
QtDcmPatient::getId()
{
    return d->id;
}

void
QtDcmPatient::setId(QString id)
{
    d->id = id;
}

QString
QtDcmPatient::getName()
{
    return d->name;
}

void
QtDcmPatient::setName(QString name)
{
    d->name = name;
}

QString
QtDcmPatient::getBirthdate()
{
    return d->birthdate;
}

void
QtDcmPatient::setBirthdate(QString birthdate)
{
    d->birthdate = birthdate;
}

QString
QtDcmPatient::getSex()
{
    return d->sex;
}

void
QtDcmPatient::setSex(QString sex)
{
    d->sex = sex;
}

QList<QtDcmStudy *>
QtDcmPatient::getStudies()
{
    return d->studies;
}

void
QtDcmPatient::setStudies(QList<QtDcmStudy *> studies)
{
    d->studies = studies;
}

void
QtDcmPatient::addStudy(QtDcmStudy * study)
{
    d->studies.append(study);
}

void
QtDcmPatient::removeStudy(int index)
{
    d->studies.removeAt(index);
}
