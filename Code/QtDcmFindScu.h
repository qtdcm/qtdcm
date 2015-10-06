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

#ifndef QTDCMFINDSCU_H_
#define QTDCMFINDSCU_H_

#include <QtGui>
#include "QtDcmFindCallback.h"

class QtDcmFindScu : public QObject
{
    Q_OBJECT

public:
    explicit QtDcmFindScu ( QObject * parent = 0);
    virtual ~QtDcmFindScu();

    void findPatientsScu ( const QString & patientName );

    void findPatientsScu ( const QString & patientName, 
                           const QString & patientSex );

    void findStudiesScu ( const QString & patientName );

    void findStudiesScu ( const QString & patientName, 
                          const QString & studyDescription );

    void findStudiesScu ( const QString & patientName, 
                          const QString & studyDescription, 
                          const QString & startDate, 
                          const QString & endDate );

    void findSeriesScu ( const QString & patientName, 
                         const QString & studyUID );

    void findSeriesScu ( const QString & patientName, 
                         const QString & studyUID, 
                         const QString & modality );

    void findSeriesScu ( const QString & patientName, 
                         const QString & studyUID, 
                         const QString & studyDescription, 
                         const QString & modality );

    void findSeriesScu ( const QString & patientName, 
                         const QString & studyUID, 
                         const QString & studyDescription, 
                         const QString & serieDescription, 
                         const QString & modality );

    void findImagesScu ( const QString & seriesUID );

    void findImageScu ( const QString & imageUID);

protected:

    bool doQuery(const OFList<OFString>& overrideKeys, QtDcmFindCallback::cbType level);

    /**
     * test if the current selected pacs is available
     * returns false if timeout (10sec) is reached
     */
    bool checkServerConnection(int timeout = 10000);

private:
    class Private;
    Private * d;
};

#endif /* QTDCMFINDSCU_H_ */
