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


#ifndef QTDCMPATIENT_H_
#define QTDCMPATIENT_H_

#include <QtGui>

class QtDcmStudy;

/**
 * This class is a representation of a dicom patient
 */
class QtDcmPatient
{
public:
    /**
     * Default constructor
     */
    QtDcmPatient();

    /**
     * Default destructor
     */
    virtual ~QtDcmPatient();

    /**
     * Id getter
     *
     * @return _id a QString
     */
    QString id() const;

    /**
     * Id setter
     *
     * @param id a QString
     */
    void setId( const QString& id );

    /**
     * Patient name getter
     *
     * @return _name a QString
     */
    QString name() const;

    /**
     * Patient name setter
     *
     * @param name a QString
     */
    void setName( const QString& name );

    /**
     * Patient birthdate getter
     *
     * @return _birthdate
     */
    QString birthdate() const;

    /**
     * Patient birthdate setter
     *
     * @param birthdate
     */
    void setBirthdate( const QString & birthdate );

    /**
     * Patient sex getter
     *
     * @return _sex a QString
     */
    QString gender() const;

    /**
     * Patient sex setter
     *
     * @param sex a QString
     */
    void setGender( const QString& sex );

    /**
     * Patient studies list getter
     *
     * @return _studies a QList of studies
     * @see QtDcmStudy
     */
    QList<QtDcmStudy> studies() const;

    /**
     * Patient studies list setter
     *
     * @param studies a QList of studies
     * @see QtDcmStudy
     */
    void setStudies( const QList<QtDcmStudy>& studies );

    /**
     * Add study in the list
     */
    void addStudy(const QtDcmStudy & study);

    /**
     * Remove study at position index
     */
    void removeStudy(int index);
    
    
private:
    class Private;
    Private *d;
  };

#endif /* QTDCMPATIENT_H_ */
