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

QString QtDcmPatient::getId()
{
    return d->id;
}

void QtDcmPatient::setId(QString id)
{
    d->id = id;
}

QString QtDcmPatient::getName()
{
    return d->name;
}

void QtDcmPatient::setName(QString name)
{
    d->name = name;
}

QString QtDcmPatient::getBirthdate()
{
    return d->birthdate;
}

void QtDcmPatient::setBirthdate(QString birthdate)
{
    d->birthdate = birthdate;
}

QString QtDcmPatient::getSex()
{
    return d->sex;
}

void QtDcmPatient::setSex(QString sex)
{
    d->sex = sex;
}

QList<QtDcmStudy *> QtDcmPatient::getStudies()
{
    return d->studies;
}

void QtDcmPatient::setStudies(QList<QtDcmStudy *> studies)
{
    d->studies = studies;
}

void QtDcmPatient::addStudy(QtDcmStudy * study)
{
    d->studies.append(study);
}

void QtDcmPatient::removeStudy(int index)
{
    d->studies.removeAt(index);
}
