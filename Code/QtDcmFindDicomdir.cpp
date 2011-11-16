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

#include <QtDcmManager.h>
#include <QtDcmFindDicomdir.h>

class QtDcmFindDicomdirPrivate
{

public:
    DcmItem * dcmObject;
    DcmStack dicomdirItems;
};

QtDcmFindDicomdir::QtDcmFindDicomdir ( QObject * parent ) : d ( new QtDcmFindDicomdirPrivate )
{
  d->dcmObject = NULL;
}

QtDcmFindDicomdir::~QtDcmFindDicomdir()
{
  delete d;
  d = NULL;
}

void QtDcmFindDicomdir::setDcmItem ( DcmItem * item )
{
    d->dcmObject = item;
}

void QtDcmFindDicomdir::findPatients()
{
    static const OFString Patient ( "PATIENT" );

    // Loading all the dicomdir items in a stack
    DcmStack itemsTmp;

    if ( !d->dcmObject->findAndGetElements ( DCM_Item, itemsTmp ).good() )
    {
        return;
    }

    while ( itemsTmp.card() > 0 )
    {
        d->dicomdirItems.push ( itemsTmp.top() );
        itemsTmp.pop();
    }

    //Unstacking and loading the different lists
    while ( d->dicomdirItems.card() > 0 )
    {
        DcmItem* lobj = ( DcmItem* ) d->dicomdirItems.top();
        DcmStack dirent;

        OFCondition condition = lobj->findAndGetElements ( DCM_DirectoryRecordType, dirent );

        if ( !condition.good() )
        {
            d->dicomdirItems.pop();
            continue;
        }

        while ( dirent.card() )
        {
            DcmElement* elt = ( DcmElement* ) dirent.top();
            OFString cur;
            elt->getOFStringArray ( cur );

            if ( cur == Patient )
            {
                DcmElement* lelt;
                QMap<QString, QString> infosMap;

                if ( lobj->findAndGetElement ( DCM_PatientName, lelt ).good() )
                {
                    OFString strName;
                    lelt->getOFStringArray ( strName );
                    infosMap.insert ( "Name", QString ( strName.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_PatientID, lelt ).good() )
                {
                    OFString strID;
                    lelt->getOFStringArray ( strID );
                    infosMap.insert ( "ID", QString ( strID.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_PatientBirthDate, lelt ).good() )
                {
                    OFString strBirth;
                    lelt->getOFStringArray ( strBirth );
                    infosMap.insert ( "Birthdate", QString ( strBirth.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_PatientSex, lelt ).good() )
                {
                    OFString strSex;
                    lelt->getOFStringArray ( strSex );
                    infosMap.insert ( "Sex", QString ( strSex.c_str() ) );
                }

                QtDcmManager::instance()->foundPatient ( infosMap );
            }

            dirent.pop();
        }

        d->dicomdirItems.pop();
    }

    d->dicomdirItems.clear();
}

void QtDcmFindDicomdir::findStudies ( QString patientName )
{
    bool proceed = false;
    static const OFString Patient ( "PATIENT" );
    static const OFString Study ( "STUDY" );

    // Loading all the dicomdir items in a stack
    DcmStack itemsTmp;

    if ( !d->dcmObject->findAndGetElements ( DCM_Item, itemsTmp ).good() )
        return;

    while ( itemsTmp.card() > 0 )
    {
        d->dicomdirItems.push ( itemsTmp.top() );
        itemsTmp.pop();
    }

    //Unstacking and loading the different lists
    while ( d->dicomdirItems.card() > 0 )
    {
        DcmItem* lobj = ( DcmItem* ) d->dicomdirItems.top();
        DcmStack dirent;

        OFCondition condition = lobj->findAndGetElements ( DCM_DirectoryRecordType, dirent );

        if ( !condition.good() )
        {
            d->dicomdirItems.pop();
            continue;
        }

        while ( dirent.card() )
        {
            DcmElement* elt = ( DcmElement* ) dirent.top();
            OFString cur;
            elt->getOFStringArray ( cur );

            if ( cur == Patient )
            {
                DcmElement* lelt;

                if ( lobj->findAndGetElement ( DCM_PatientName, lelt ).good() )
                {
                    OFString strName;
                    lelt->getOFStringArray ( strName );
                    proceed = ( QString ( strName.c_str() ) == patientName );
                }
            }

            if ( ( cur == Study ) && proceed )
            {
                DcmElement* lelt;
                QMap<QString, QString> infosMap;

                if ( lobj->findAndGetElement ( DCM_StudyInstanceUID, lelt ).good() )
                {
                    OFString strUID;
                    lelt->getOFStringArray ( strUID );
                    infosMap.insert ( "UID", QString ( strUID.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_StudyID, lelt ).good() )
                {
                    OFString strID;
                    lelt->getOFStringArray ( strID );
                    infosMap.insert ( "ID", QString ( strID.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_StudyDescription, lelt ).good() )
                {
                    OFString strDescription;
                    lelt->getOFStringArray ( strDescription );
                    infosMap.insert ( "Description", QString ( strDescription.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_StudyDate, lelt ).good() )
                {
                    OFString strDate;
                    lelt->getOFStringArray ( strDate );
                    infosMap.insert ( "Date", QString ( strDate.c_str() ) );
                }
                QtDcmManager::instance()->foundStudy ( infosMap );
            }

            dirent.pop();
        }

        d->dicomdirItems.pop();
    }

    d->dicomdirItems.clear();
}

void QtDcmFindDicomdir::findSeries ( QString patientName, QString studyUID )
{
    bool proceed = false;
    static const OFString Patient ( "PATIENT" );
    static const OFString Study ( "STUDY" );
    static const OFString Series ( "SERIES" );

    // Loading all the dicomdir items in a stack
    DcmStack itemsTmp;

    if ( !d->dcmObject->findAndGetElements ( DCM_Item, itemsTmp ).good() )
        return;

    while ( itemsTmp.card() > 0 )
    {
        d->dicomdirItems.push ( itemsTmp.top() );
        itemsTmp.pop();
    }

    OFString strName;

    OFString strDate;
    //Unstacking and loading the different lists

    while ( d->dicomdirItems.card() > 0 )
    {
        DcmItem* lobj = ( DcmItem* ) d->dicomdirItems.top();
        DcmStack dirent;

        OFCondition condition = lobj->findAndGetElements ( DCM_DirectoryRecordType, dirent );

        if ( !condition.good() )
        {
            d->dicomdirItems.pop();
            continue;
        }

        while ( dirent.card() )
        {
            DcmElement* elt = ( DcmElement* ) dirent.top();
            OFString cur;
            elt->getOFStringArray ( cur );

            if ( cur == Patient )
            {
                DcmElement* lelt;

                if ( lobj->findAndGetElement ( DCM_PatientName, lelt ).good() )
                    lelt->getOFStringArray ( strName );
            }

            if ( cur == Study )
            {
                DcmElement* lelt;

                if ( lobj->findAndGetElement ( DCM_StudyInstanceUID, lelt ).good() )
                {
                    OFString strUid;
                    lelt->getOFStringArray ( strUid );
                    proceed = ( ( QString ( strName.c_str() ) == patientName ) && ( QString ( strUid.c_str() ) == studyUID ) );
                }

                if ( lobj->findAndGetElement ( DCM_StudyDate, lelt ).good() )
                {
                    lelt->getOFStringArray ( strDate );
                }
            }

            if ( ( cur == Series ) && proceed )
            {
                DcmElement* lelt;
                QMap<QString, QString> infosMap;

                if ( lobj->findAndGetElement ( DCM_SeriesInstanceUID, lelt ).good() )
                {
                    OFString strID;
                    lelt->getOFStringArray ( strID );
                    infosMap.insert ( "ID", QString ( strID.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_SeriesDescription, lelt ).good() )
                {
                    OFString strDescription;
                    lelt->getOFStringArray ( strDescription );
                    infosMap.insert ( "Description", QString ( strDescription.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_Modality, lelt ).good() )
                {
                    OFString strModality;
                    lelt->getOFStringArray ( strModality );
                    infosMap.insert ( "Modality", QString ( strModality.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_InstitutionName, lelt ).good() )
                {
                    OFString strInstitution;
                    lelt->getOFStringArray ( strInstitution );
                    infosMap.insert ( "Institution", QString ( strInstitution.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_AcquisitionNumber, lelt ).good() )
                {
                    OFString strCount;
                    lelt->getOFStringArray ( strCount );
                    infosMap.insert ( "InstanceCount", QString ( strCount.c_str() ) );
                }

                if ( lobj->findAndGetElement ( DCM_PerformingPhysicianName, lelt ).good() )
                {
                    OFString strOperator;
                    lelt->getOFStringArray ( strOperator );
                    infosMap.insert ( "Operator", QString ( strOperator.c_str() ) );
                }

                infosMap.insert ( "Date", QString ( strDate.c_str() ) );

                QtDcmManager::instance()->foundSerie ( infosMap );
            }

            dirent.pop();
        }

        d->dicomdirItems.pop();
    }

    d->dicomdirItems.clear();
}

void QtDcmFindDicomdir::findImages ( QString seriesUID )
{
    bool proceed = false;
    static const OFString Patient ( "PATIENT" );
    static const OFString Study ( "STUDY" );
    static const OFString Series ( "SERIES" );
    static const OFString Image ( "IMAGE" );

    // Loading all the dicomdir items in a stack
    DcmStack itemsTmp;

    if ( !d->dcmObject->findAndGetElements ( DCM_Item, itemsTmp ).good() )
        return;

    while ( itemsTmp.card() > 0 )
    {
        d->dicomdirItems.push ( itemsTmp.top() );
        itemsTmp.pop();
    }

    OFString strName;

    OFString strDate;
    //Unstacking and loading the different lists

    while ( d->dicomdirItems.card() > 0 )
    {
        DcmItem* lobj = ( DcmItem* ) d->dicomdirItems.top();
        DcmStack dirent;

        OFCondition condition = lobj->findAndGetElements ( DCM_DirectoryRecordType, dirent );

        if ( !condition.good() )
        {
            d->dicomdirItems.pop();
            continue;
        }

        while ( dirent.card() )
        {
            DcmElement* elt = ( DcmElement* ) dirent.top();
            OFString cur;
            elt->getOFStringArray ( cur );

            if ( cur == Series )
            {
                DcmElement* lelt;
                QMap<QString, QString> infosMap;

                if ( lobj->findAndGetElement ( DCM_SeriesInstanceUID, lelt ).good() )
                {
                    OFString strID;
                    lelt->getOFStringArray ( strID );
                    proceed = ( QString ( strID.c_str() ) == seriesUID );
                }
            }

            if ( ( cur == Image ) && proceed )
            {
                DcmElement* lelt;

                OFString strUID;
                OFString strNumber ( "0" );
                if ( lobj->findAndGetElement ( DCM_InstanceNumber, lelt ).good() )
                    lelt->getOFStringArray ( strNumber );

                if ( lobj->findAndGetElement ( DCM_ReferencedSOPInstanceUIDInFile, lelt ).good() )
                {
                    lelt->getOFStringArray ( strUID );
                }
                QtDcmManager::instance()->foundImage ( QString ( strUID.c_str() ), QString ( strNumber.c_str() ).toInt() );
            }

            dirent.pop();
        }

        d->dicomdirItems.pop();
    }

    d->dicomdirItems.clear();
}
