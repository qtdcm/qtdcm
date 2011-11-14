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


#define QT_NO_CAST_TO_ASCII

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
// From Dcmtk:
#include <dcmtk/config/osconfig.h>    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include "dcmtk/dcmnet/dfindscu.h"
#include <dcmtk/dcmdata/dcistrmz.h>    /* for dcmZlibExpectRFC1950Encoding */
// For dcm images
#include <dcmtk/dcmimgle/dcmimage.h>
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
// For color images
#include <dcmtk/dcmimage/diregist.h>

//#define INCLUDEd->CSTDLIB
//#define INCLUDEd->CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QtDcmFindCallback.h>
#include <QtDcmManager.h>
#include <QtDcmPreferences.h>
#include <QtDcmServer.h>
#include <QtDcmFindScu.h>

class QtDcmFindScuPrivate
{

public:
    QtDcmManager * manager;
};

QtDcmFindScu::QtDcmFindScu ( QObject * parent ) : d ( new QtDcmFindScuPrivate )
{
//     d->manager = dynamic_cast<QtDcmManager *> ( parent );
    d->manager = QtDcmManager::instance();
}

QtDcmFindScu::~QtDcmFindScu() {}

void QtDcmFindScu::findPatientsScu ( QString patientName )
{
    this->findPatientsScu ( patientName, "*" );
}

void QtDcmFindScu::findPatientsScu ( QString patientName, QString patientSex )
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "PATIENT" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "PatientName=" ) + patientName ).toUtf8().data() );

    //Patient leqvel
    overrideKeys.push_back ( QString ( "PatientID" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "PatientSex=" + patientSex ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "PatientBirthDate" ).toUtf8().data() );

    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;

    QtDcmFindCallback * callback = new QtDcmFindCallback();
    callback->setManager ( d->manager );

    if ( findscu.initializeNetwork ( 30 ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot establish network connection" ) );

    if ( findscu.performQuery ( d->manager->getCurrentPacs()->getServer().toUtf8().data(), d->manager->getCurrentPacs()->getPort().toInt(), QtDcmPreferences::instance()->getAetitle().toUtf8().data(), d->manager->getCurrentPacs()->getAetitle().toUtf8().data(), UID_FINDPatientRootQueryRetrieveInformationModel, EXS_Unknown, DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU, false, false, 1, false, -1, &overrideKeys, callback, &fileNameList ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot perform query C-FIND" ) );

    if ( findscu.dropNetwork().bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot drop network" ) );
}

void QtDcmFindScu::findStudiesScu ( QString patientName )
{
    this->findStudiesScu ( patientName, "*", QDate ( 1900,01,01 ).toString ( "yyyyMMdd" ),QDate::currentDate().toString ( "yyyyMMdd" ) );
}

void QtDcmFindScu::findStudiesScu ( QString patientName, QString studyDescription )
{
    this->findStudiesScu ( patientName, studyDescription, QDate ( 1900,01,01 ).toString ( "yyyyMMdd" ),QDate::currentDate().toString ( "yyyyMMdd" ) );
}

void QtDcmFindScu::findStudiesScu ( QString patientName, QString studyDescription, QString startDate, QString endDate )
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "STUDY" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "PatientName=" ) + patientName ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "StudyDescription=" ) + studyDescription ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "StudyDate=" + startDate + "-" + endDate ).toUtf8().data() );

    //Study level
    overrideKeys.push_back ( QString ( "StudyInstanceUID" ).toUtf8().data() );

    //Image level
    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;

    QtDcmFindCallback * callback = new QtDcmFindCallback ( QtDcmFindCallback::STUDY );
    callback->setManager ( d->manager );

    if ( findscu.initializeNetwork ( 30 ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot establish network connection" ) );

    if ( findscu.performQuery ( d->manager->getCurrentPacs()->getServer().toUtf8().data(), d->manager->getCurrentPacs()->getPort().toInt(), QtDcmPreferences::instance()->getAetitle().toUtf8().data(), d->manager->getCurrentPacs()->getAetitle().toUtf8().data(), UID_FINDPatientRootQueryRetrieveInformationModel, EXS_Unknown, DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU, false, false, 1, false, -1, &overrideKeys, callback, &fileNameList ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot perform query C-FIND" ) );

    if ( findscu.dropNetwork().bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot drop network" ) );
}

void QtDcmFindScu::findSeriesScu ( QString patientName, QString studyDescription )
{
    this->findSeriesScu ( patientName, studyDescription, "*", "*" );
}

void QtDcmFindScu::findSeriesScu ( QString patientName, QString studyDescription, QString modality )
{
    this->findSeriesScu ( patientName, studyDescription, "*", modality );
}

void QtDcmFindScu::findSeriesScu ( QString patientName, QString studyDescription, QString serieDescription, QString modality )
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "SERIES" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "PatientName=" ) + patientName ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "StudyDescription=" + studyDescription ).toUtf8().data() );
    overrideKeys.push_back ( ( QString ( "SeriesDescription=" ) + serieDescription ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "Modality=" + modality ).toUtf8().data() );

    //Study level
    overrideKeys.push_back ( QString ( "StudyDate" ).toUtf8().data() );

    //Serie level
    overrideKeys.push_back ( QString ( "SeriesInstanceUID" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "InstitutionName" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "InstitutionAddress" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "PerformingPhysicianName" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "AcquisitionNumber" ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "NumberOfSeriesRelatedInstances" ).toUtf8().data() );

    //Image level
    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;

    QtDcmFindCallback * callback = new QtDcmFindCallback ( QtDcmFindCallback::SERIE );
    callback->setManager ( d->manager );

    if ( findscu.initializeNetwork ( 30 ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot establish network connection" ) );

    if ( findscu.performQuery ( d->manager->getCurrentPacs()->getServer().toAscii().data(), d->manager->getCurrentPacs()->getPort().toInt(), QtDcmPreferences::instance()->getAetitle().toAscii().data(), d->manager->getCurrentPacs()->getAetitle().toAscii().data(), UID_FINDPatientRootQueryRetrieveInformationModel, EXS_Unknown, DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU, false, false, 1, false, -1, &overrideKeys, callback, &fileNameList ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot perform query C-FIND" ) );

    if ( findscu.dropNetwork().bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot drop network" ) );
}

void QtDcmFindScu::findImagesScu ( QString seriesUID )
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "IMAGE" "" ) ).toUtf8().data() );
    overrideKeys.push_back ( QString ( "SeriesInstanceUID=" + seriesUID ).toUtf8().data() );

    //Image level
    overrideKeys.push_back ( QString ( "SOPInstanceUID" ).toUtf8().data() );
    

    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;

    QtDcmFindCallback * callback = new QtDcmFindCallback ( QtDcmFindCallback::IMAGES );
    callback->setManager ( d->manager );

    if ( findscu.initializeNetwork ( 30 ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot establish network connection" ) );

    if ( findscu.performQuery ( d->manager->getCurrentPacs()->getServer().toAscii().data(), d->manager->getCurrentPacs()->getPort().toInt(), QtDcmPreferences::instance()->getAetitle().toAscii().data(), d->manager->getCurrentPacs()->getAetitle().toAscii().data(), UID_FINDPatientRootQueryRetrieveInformationModel, EXS_Unknown, DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU, false, false, 1, false, -1, &overrideKeys, callback, &fileNameList ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot perform query C-FIND" ) );

    if ( findscu.dropNetwork().bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot drop network" ) );
}

void QtDcmFindScu::findImageScu ( QString imageUID)
{
    OFList<OFString> overrideKeys;
    overrideKeys.push_back ( ( QString ( "QueryRetrieveLevel=" ) + QString ( "" "IMAGE" "" ) ).toUtf8().data() );
//     overrideKeys.push_back ( QString ( "SeriesInstanceUID=" + seriesUID ).toUtf8().data() );
//     overrideKeys.push_back ( QString ( "InstanceNumber=" + instanceNumber ).toUtf8().data() );

    //Image level
    overrideKeys.push_back ( QString ( "SOPInstanceUID="+ imageUID ).toUtf8().data() );

    OFList<OFString> fileNameList;
    OFString temp_str;
    DcmFindSCU findscu;

    QtDcmFindCallback * callback = new QtDcmFindCallback ( QtDcmFindCallback::IMAGE );
    callback->setManager ( d->manager );

    if ( findscu.initializeNetwork ( 30 ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot establish network connection" ) );

    if ( findscu.performQuery ( d->manager->getCurrentPacs()->getServer().toAscii().data(), d->manager->getCurrentPacs()->getPort().toInt(), QtDcmPreferences::instance()->getAetitle().toAscii().data(), d->manager->getCurrentPacs()->getAetitle().toAscii().data(), UID_FINDPatientRootQueryRetrieveInformationModel, EXS_Unknown, DIMSE_BLOCKING, 0, ASC_DEFAULTMAXPDU, false, false, 1, false, -1, &overrideKeys, callback, &fileNameList ).bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot perform query C-FIND" ) );

    if ( findscu.dropNetwork().bad() )
        d->manager->displayErrorMessage ( tr ( "Cannot drop network" ) );
}
