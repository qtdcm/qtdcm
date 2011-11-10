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


#ifndef QTDCMPATIENT_H_
#define QTDCMPATIENT_H_

#include <QtGui>

class QtDcmPatientPrivate;
class QtDcmStudy;

/**
 * This class is a representation of a dicom patient
 */
class QtDcmPatient : public QObject
{

  Q_OBJECT

private:
    QtDcmPatientPrivate *d;

public:
    /**
     * Default constructor
     */
    QtDcmPatient();

    /**
     * Default destructor
     */
    virtual ~QtDcmPatient() {};

    /**
     * Id getter
     *
     * @return _id a QString
     */
    QString getId();

    /**
     * Id setter
     *
     * @param id a QString
     */
    void setId( QString id );

    /**
     * Patient name getter
     *
     * @return _name a QString
     */
    QString getName();

    /**
     * Patient name setter
     *
     * @param name a QString
     */
    void setName( QString name );

    /**
     * Patient birthdate getter
     *
     * @return _birthdate
     */
    QString getBirthdate();

    /**
     * Patient birthdate setter
     *
     * @param birthdate
     */
    void setBirthdate( QString birthdate );

    /**
     * Patient sex getter
     *
     * @return _sex a QString
     */
    QString getSex();

    /**
     * Patient sex setter
     *
     * @param sex a QString
     */
    void setSex( QString sex );

    /**
     * Patient studies list getter
     *
     * @return _studies a QList of studies
     * @see QtDcmStudy
     */
    QList<QtDcmStudy *> getStudies();

    /**
     * Patient studies list setter
     *
     * @param studies a QList of studies
     * @see QtDcmStudy
     */
    void setStudies( QList<QtDcmStudy *> studies );

    /**
     * Add study in the list
     */
    void addStudy(QtDcmStudy * study);

    /**
     * Remove study at position index
     */
    void removeStudy(int index);
  };

#endif /* QTDCMPATIENT_H_ */
