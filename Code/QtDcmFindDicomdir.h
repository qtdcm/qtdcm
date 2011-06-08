/*
 * QtDcmFindDicomdir.h
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
 */

#ifndef QTDCMFINDDICOMDIR_H_
#define QTDCMFINDDICOMDIR_H_

#include <QtGui>

class DcmItem;
class QtDcmFindDicomdirPrivate;

class QtDcmFindDicomdir : public QObject
{
    Q_OBJECT
    public:
        QtDcmFindDicomdir(QObject * parent);
        virtual
        ~QtDcmFindDicomdir();
        
        void
        setDcmItem(DcmItem * item);

        void
        findPatients();

        void
        findStudies(QString patientName);

        void
        findSeries(QString patientName, QString studyDescription);

        void
        findImages(QString seriesUID);

    private:
        QtDcmFindDicomdirPrivate * d;
};

#endif /* QTDCMFINDDICOMDIR_H_ */
