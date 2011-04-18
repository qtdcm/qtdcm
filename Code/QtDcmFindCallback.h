/*
 * QtDcmFindCallback.h
 *
 *  Created on: 29 mars 2011
 *      Author: aabadie
 */

#ifndef QTDCMFINDCALLBACK_H_
#define QTDCMFINDCALLBACK_H_

#include <QtGui>
#include "dcmtk/dcmnet/dfindscu.h"

struct T_DIMSE_C_FindRQ;
struct T_DIMSE_C_FindRSP;
class DcmDataset;
class QtDcmManager;

class QtDcmFindCallbackPrivate;

class QtDcmFindCallback : public DcmFindSCUCallback
{
    public:
        QtDcmFindCallback();
        QtDcmFindCallback(int type);

        virtual
        ~QtDcmFindCallback() {};

        enum cbType
        {
            PATIENT, STUDY, SERIE, IMAGE
        };

        void
        setManager(QtDcmManager * manager);

        virtual void
        callback(T_DIMSE_C_FindRQ *request, int responseCount, T_DIMSE_C_FindRSP *rsp, DcmDataset *responseIdentifiers);

    private:
        QtDcmFindCallbackPrivate * d;
};

#endif /* QTDCMFINDCALLBACK_H_ */
