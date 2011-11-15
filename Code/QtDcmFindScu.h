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

class QtDcmFindScuPrivate;

class QtDcmFindScu : public QObject
{
    Q_OBJECT

public:
    QtDcmFindScu ( QObject * parent );
    virtual ~QtDcmFindScu();

    void findPatientsScu ( QString patientName );

    void findPatientsScu ( QString patientName, QString patientSex );

    void findStudiesScu ( QString patientName );

    void findStudiesScu ( QString patientName, QString studyDescription );

    void findStudiesScu ( QString patientName, QString studyDescription, QString startDate, QString endDate );

    void findSeriesScu ( QString patientName, QString studyUID );

    void findSeriesScu ( QString patientName, QString studyUID, QString modality );

    void findSeriesScu ( QString patientName, QString studyUID, QString studyDescription, QString modality );

    void findSeriesScu ( QString patientName, QString studyUID, QString studyDescription, QString serieDescription, QString modality );

    void findImagesScu ( QString seriesUID );

    void findImageScu ( QString imageUID);

protected:

    /**
     * test if the current selected pacs is available
     * returns false if timeout (1sec) is reached
     */
    bool checkServerConnection(int timeout = 1000);

private:
    QtDcmFindScuPrivate * d;
};

#endif /* QTDCMFINDSCU_H_ */
