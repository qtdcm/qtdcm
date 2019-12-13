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

#define QT_NO_CAST_TO_ASCII

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QTcpSocket>

#include <QtDcmFindCallback.h>
#include <QtDcmManager.h>
#include <QtDcmPreferences.h>
#include <QtDcmServer.h>
#include <QtDcmFindScu.h>

class QtDcmFindScu::Private
{
public:
    QtDcmManager * manager;
    QTcpSocket * socket;
    int networkTimeout;
};

QtDcmFindScu::QtDcmFindScu ( QObject * parent ) 
    : QObject(parent),
      d( new QtDcmFindScu::Private )
{
    d->manager = QtDcmManager::instance();
    d->socket = new QTcpSocket(this);
    d->networkTimeout = 30;
}

QtDcmFindScu::~QtDcmFindScu()
{
    delete d;
    d = NULL;
}


void QtDcmFindScu::findPatientsScu (const QString &patientName)
{
    this->findPatientsScu ( patientName, "*" );
}

void QtDcmFindScu::findPatientsScu (const QString &patientName, const QString &patientSex)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "PATIENT" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "PatientName=" ) + patientName ).toUtf8().data() );

    //Patient leqvel
    overrideKeys.push_back ( QString ( "PatientID" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "PatientSex=" + patientSex ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "PatientBirthDate" ).toUtf8().data() );

    doQuery ( overrideKeys, QtDcmFindCallback::PATIENT );

}

void QtDcmFindScu::findStudiesScu (const QString &patientName)
{
    this->findStudiesScu ( patientName, "*", QDate ( 1900,01,01 ).toString ( "yyyyMMdd" ),QDate::currentDate().toString ( "yyyyMMdd" ) );
}

void QtDcmFindScu::findStudiesScu(const QString &patientName, const QString &studyDescription)
{
    this->findStudiesScu ( patientName, studyDescription, QDate ( 1900,01,01 ).toString ( "yyyyMMdd" ),QDate::currentDate().toString ( "yyyyMMdd" ) );
}

void QtDcmFindScu::findStudiesScu (const QString &patientName, const QString &studyDescription, const QString &startDate, const QString &endDate)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "STUDY" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "PatientName=" ) + patientName ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "StudyDescription=" ) + studyDescription ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "StudyDate=" + startDate + "-" + endDate ).toUtf8().data() );
    overrideKeys.push_back(QString("StudyID").toUtf8().data());
    overrideKeys.push_back(QString("AccessionNumber").toUtf8().data());
    overrideKeys.push_back(QString("NumberOfStudyRelatedSeries").toUtf8().data());
    overrideKeys.push_back(QString("NumberOfStudyRelatedInstances").toUtf8().data());
    overrideKeys.push_back(QString("PatientID=*").toUtf8().data());

    //Study level
    overrideKeys.push_back ( QString ( "StudyInstanceUID" ).toUtf8().data() );

    doQuery ( overrideKeys, QtDcmFindCallback::STUDY );

}

void QtDcmFindScu::findSeriesScu (const QString &patientName, const QString &studyUID)
{
    this->findSeriesScu ( patientName, studyUID, "*", "*", "*" );
}

void QtDcmFindScu::findSeriesScu (const QString &patientName, const QString &studyUID, const QString &studyDescription, const QString &modality)
{
    this->findSeriesScu ( patientName, studyUID, studyDescription, "*", modality );
}

void QtDcmFindScu::findSeriesScu (const QString &patientName, const QString &studyUID, const QString &studyDescription, const QString &serieDescription, const QString &modality)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "SERIES" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "PatientName=" ) + patientName ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "StudyInstanceUID=" + studyUID ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "StudyDescription=" + studyDescription ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "SeriesDescription=" ) + serieDescription ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "Modality=" + modality ).toUtf8().data() );
    overrideKeys.push_back ( QString("PatientID=*").toUtf8().data());

    //Study level
    overrideKeys.push_back ( QString ( "StudyDate" ).toUtf8().data() );

    //Serie level
    overrideKeys.push_back ( QString ( "SeriesInstanceUID" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "InstitutionName" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "InstitutionAddress" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "PerformingPhysicianName" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "AcquisitionNumber" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "NumberOfSeriesRelatedInstances" ).toUtf8().data() );

    doQuery ( overrideKeys, QtDcmFindCallback::SERIE );
}

void QtDcmFindScu::findImagesScu (const QString &seriesUID)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "IMAGE" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "SeriesInstanceUID=" + seriesUID ).toUtf8().data() );
    overrideKeys.push_back(QString("PatientID=*").toUtf8().data());
    overrideKeys.push_back(QString("StudyInstanceUID=*").toUtf8().data());

    //Image level
    overrideKeys.push_back ( QString ( "SOPInstanceUID" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "InstanceNumber" ).toUtf8().data() );

    doQuery ( overrideKeys, QtDcmFindCallback::IMAGES );
}

void QtDcmFindScu::findImageScu (const QString &imageUID)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "IMAGE" "" ) ).toUtf8().data() );
//     overrideKeys.push_back ( QString ( "SeriesInstanceUID=" + seriesUID ).toUtf8().data() );
//     overrideKeys.push_back ( QString ( "InstanceNumber=" + instanceNumber ).toUtf8().data() );

    //Image level
    overrideKeys.push_back ( QString ( "SOPInstanceUID="+ imageUID ).toUtf8().data() );

    doQuery ( overrideKeys, QtDcmFindCallback::IMAGE );
}

bool QtDcmFindScu::checkServerConnection ( int timeout )
{
    bool result = true;
    d->socket->setSocketOption ( QAbstractSocket::LowDelayOption, 1 );
    d->socket->connectToHost ( d->manager->currentPacs().address(),  d->manager->currentPacs().port().toInt() );
    if ( d->socket->waitForConnected ( timeout ) ) {
        d->socket->disconnectFromHost();
    }
    else {
        d->manager->displayErrorMessage ( "Cannot connect to server " + d->manager->currentPacs().address() + " on port " + d->manager->currentPacs().port() + " !" );
        result = false;
    }
    
    return result;
}

bool QtDcmFindScu::doQuery ( const OFList<OFString>& overrideKeys, QtDcmFindCallback::cbType level )
{
    //Image level
    OFList<OFString> fileNameList;
    DcmFindSCU findscu;
    OFList<OFString> keys = overrideKeys;
    
    // test connection
    if ( !this->checkServerConnection(10000) ) {
        return false;
    }

    if ( findscu.initializeNetwork ( d->networkTimeout ).bad() ) {
        QtDcmManager::instance()->displayErrorMessage ( tr ( "Cannot establish network connection" ) );
        return false;
    }

    QtDcmFindCallback callback( level );
    if ( findscu.performQuery ( d->manager->currentPacs().address().toUtf8().data(),
                                d->manager->currentPacs().port().toInt(),
                                QtDcmPreferences::instance()->aetitle().toUtf8().data(),
                                d->manager->currentPacs().aetitle().toUtf8().data(),
                                UID_FINDPatientRootQueryRetrieveInformationModel, EXS_Unknown,
                                DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU, false, false, 1, false, -1, &keys, &callback, &fileNameList ).bad() ) {
        QtDcmManager::instance()->displayErrorMessage ( tr ( "Cannot perform query C-FIND" ) );
    }

    if ( findscu.dropNetwork().bad() ) {
        QtDcmManager::instance()->displayErrorMessage ( tr ( "Cannot drop network" ) );
    }
    
    return true;
}
