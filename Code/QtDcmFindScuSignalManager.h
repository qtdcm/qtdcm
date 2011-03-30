/*
 * QtDcmFindScuSignalManager.h
 *
 *  Created on: 29 mars 2011
 *      Author: aabadie
 */

#ifndef QTDCMFINDSCUSIGNALMANAGER_H_
#define QTDCMFINDSCUSIGNALMANAGER_H_

#include <QtGui>

class QtDcmPatient;
class QtDcmStudy;
class QtDcmSerie;
class QtDcmImage;

class QtDcmFindScuSignalManagerPrivate;

class QtDcmFindScuSignalManager : public QObject
{
    Q_OBJECT
    public:
        QtDcmFindScuSignalManager(QObject * parent);
        virtual
        ~QtDcmFindScuSignalManager();

        void
        attachPatientsTreeWidget(QTreeWidget * widget);
        void
        attachStudiesTreeWidget(QTreeWidget * widget);
        void
        attachSeriesTreeWidget(QTreeWidget * widget);

        void
        foundPatient(QMap<QString, QString> infosMap);
        void
        foundStudy(QMap<QString, QString> infosMap);
        void
        foundSerie(QMap<QString, QString> infosMap);
        void
        foundImage();

    private:
        QtDcmFindScuSignalManagerPrivate * d;
};

#endif /* QTDCMFINDSCUSIGNALMANAGER_H_ */
