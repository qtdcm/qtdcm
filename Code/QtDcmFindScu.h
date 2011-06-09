/*
 * QtDcmFindScu.h
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
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

    void findSeriesScu ( QString patientName, QString studyDescription );

    void findSeriesScu ( QString patientName, QString studyDescription, QString modality );

    void findSeriesScu ( QString patientName, QString studyDescription, QString serieDescription, QString modality );

    void findImagesScu ( QString seriesUID );

    void
    findImageScu ( QString seriesUID, QString instanceNumber );

private:
    QtDcmFindScuPrivate * d;
};

#endif /* QTDCMFINDSCU_H_ */
