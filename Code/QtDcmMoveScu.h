/*
 * QtDcmMoveScu.h
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
 */

#ifndef QTDCMMOVESCU_H_
#define QTDCMMOVESCU_H_

#include <QtGui>
#include "dcmtk/dcmnet/dimse.h"

struct T_DIMSE_C_MOVERQ;
struct T_DIMSE_C_MOVERSP;
struct T_ASC_Network;
struct T_ASC_Association;
class DcmDataset;
class QtDcmMoveScuPrivate;

class QtDcmMoveScu : public QThread
{
    Q_OBJECT
    public:
        QtDcmMoveScu(QObject * parent);

        virtual
        ~QtDcmMoveScu();

        void
        setOutputDir(QString dir);

        void
        setSeries(QList<QString> series);

        bool isAssociated();

        bool isConnected();

        void
        run();

    signals:
        void
        updateProgress(int i);

    private:
        void
        moveScu(QString uid);

        void
        addOverrideKey(QString key);

        void
        cmove();

        void
        acceptSubAssoc();

        void
        storeSCP(T_DIMSE_Message * msg);

        void
        storeSCPCallback(void *callbackData, char *imageFileName, DcmDataset **imageDataSet, DcmDataset **statusDetail);

        void
        subOpSCP();

        static void
        subOpCallback(void * /*subOpCallbackData*/, T_ASC_Network *aNet, T_ASC_Association **subAssoc);

        static void
        moveCallback(void *callbackData, T_DIMSE_C_MoveRQ * req, int responseCount, T_DIMSE_C_MoveRSP * rsp);

        void
        substituteOverrideKeys(DcmDataset *dset);

        void
        moveSCU();

        QtDcmMoveScuPrivate * d;
};

#endif /* QTDCMMOVESCU_H_ */
