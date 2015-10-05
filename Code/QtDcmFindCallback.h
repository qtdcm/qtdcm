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

#ifndef QTDCMFINDCALLBACK_H_
#define QTDCMFINDCALLBACK_H_

#include <QtGui>
#include <dcmtk/dcmnet/dfindscu.h>

struct T_DIMSE_C_FindRQ;
struct T_DIMSE_C_FindRSP;

class DcmDataset;
class QtDcmManager;
class QtDcmFindCallbackPrivate;

class QtDcmFindCallback : public DcmFindSCUCallback
{

public:
    QtDcmFindCallback ( int type = PATIENT );

    virtual ~QtDcmFindCallback();

    enum cbType {
        PATIENT, 
        STUDY, 
        SERIE, 
        IMAGES,
        IMAGE
    };

    virtual void callback ( T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers );

private:
    QtDcmFindCallbackPrivate * d;
};

#endif /* QTDCMFINDCALLBACK_H_ */
