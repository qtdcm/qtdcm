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

#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmImage.h>
#include <QtDcmManager.h>

#include <QtDcmFindCallback.h>

class QtDcmFindCallbackPrivate
{

public:
    int type;
    QtDcmManager * manager;
};

QtDcmFindCallback::QtDcmFindCallback() : d ( new QtDcmFindCallbackPrivate )
{
    d->type = QtDcmFindCallback::PATIENT;
}

QtDcmFindCallback::QtDcmFindCallback ( int type ) :
        d ( new QtDcmFindCallbackPrivate )
{
    d->type = type;
    d->manager = NULL;
}

void QtDcmFindCallback::setManager ( QtDcmManager * manager )
{
    d->manager = manager;
}

void QtDcmFindCallback::callback ( T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers )
{
    QMap<QString, QString> infosMap;

    OFString info;

    switch ( d->type )
    {

    case QtDcmFindCallback::PATIENT:
        responseIdentifiers->findAndGetOFString ( DCM_PatientName, info );
        infosMap.insert ( "Name", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_PatientID, info );
        infosMap.insert ( "ID", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_PatientSex, info );
        infosMap.insert ( "Sex", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_PatientBirthDate, info );
        infosMap.insert ( "Birthdate", QString ( info.c_str() ) );

        if ( d->manager )
            d->manager->foundPatient ( infosMap );

        break;

    case QtDcmFindCallback::STUDY:
        responseIdentifiers->findAndGetOFString ( DCM_StudyDescription, info );
        infosMap.insert ( "Description", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_StudyDate, info );
        infosMap.insert ( "Date", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_StudyID, info );
        infosMap.insert ( "ID", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_StudyInstanceUID, info );
        infosMap.insert ( "UID", QString ( info.c_str() ) );

        if ( d->manager )
            d->manager->foundStudy ( infosMap );

        break;

    case QtDcmFindCallback::SERIE:
        responseIdentifiers->findAndGetOFString ( DCM_SeriesDescription, info );
        infosMap.insert ( "Description", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_StudyDate, info );
        infosMap.insert ( "Date", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_Modality, info );
        infosMap.insert ( "Modality", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_SeriesInstanceUID, info );
        infosMap.insert ( "ID", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_InstitutionName, info );
        infosMap.insert ( "Institution", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_PerformingPhysicianName, info );
        infosMap.insert ( "Operator", QString ( info.c_str() ) );
        responseIdentifiers->findAndGetOFString ( DCM_NumberOfSeriesRelatedInstances, info );
        infosMap.insert ( "InstanceCount", QString ( info.c_str() ) );

        if ( d->manager )
            d->manager->foundSerie ( infosMap );
        break;

    case QtDcmFindCallback::IMAGE:
        responseIdentifiers->findAndGetOFString ( DCM_SOPInstanceUID, info );

        if ( d->manager )
            d->manager->setPreviewImageUID ( QString ( info.c_str() ) );
        break;

    case QtDcmFindCallback::IMAGES:
        OFString number;
        responseIdentifiers->findAndGetOFString ( DCM_SOPInstanceUID, info );
        responseIdentifiers->findAndGetOFString ( DCM_InstanceNumber, number );
                
        if (!number.length())
          number = "0";
        
        if ( d->manager )
            d->manager->foundImage ( QString ( info.c_str() ), QString ( number.c_str() ).toInt() );

//         responseIdentifiers->print ( std::cout );

        break;
        
    }
}
