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


#include <QtDcmImage.h>
#include <QtDcmSerie.h>
#include <QtDcmStudy.h>
#include <QtDcmPatient.h>

class QtDcmPatient::Private
{
    public:
        QString id; /** Patient dicom identificator */
        QString name; /** Patient name as found in the dicom format*/
        QString birthdate; /** Patient birth date */
        QString sex; /** Patient sex */

        QList<QtDcmStudy> studies; /** List of a study for this patient */
};

QtDcmPatient::QtDcmPatient() : d(new QtDcmPatient::Private)
{}

QtDcmPatient::~QtDcmPatient()
{
    delete d;
}

QString QtDcmPatient::id() const
{
    return d->id;
}

void QtDcmPatient::setId(const QString &id)
{
    d->id = id;
}

QString QtDcmPatient::name() const
{
    return d->name;
}

void QtDcmPatient::setName(const QString &name)
{
    d->name = name;
}

QString QtDcmPatient::birthdate() const
{
    return d->birthdate;
}

void QtDcmPatient::setBirthdate(const QString &birthdate)
{
    d->birthdate = birthdate;
}

QString QtDcmPatient::gender() const
{
    return d->sex;
}

void QtDcmPatient::setGender(const QString &sex)
{
    d->sex = sex;
}

QList<QtDcmStudy> QtDcmPatient::studies() const
{
    return d->studies;
}

void QtDcmPatient::setStudies(const QList<QtDcmStudy> &studies)
{
    d->studies = studies;
}

void QtDcmPatient::addStudy(const QtDcmStudy &study)
{
    d->studies.append(study);
}

void QtDcmPatient::removeStudy(int index)
{
    d->studies.removeAt(index);
}
