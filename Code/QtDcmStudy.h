/*
 * QtDcmStudy.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMSTUDY_H_
#define QTDCMSTUDY_H_

#include <QtGui>
#include <QList>

class QtDcmStudyPrivate;
class QtDcmPatient;
class QtDcmSerie;

/**
 * This class is representation of a Dicom study.
 */
class QtDcmStudy : public QObject
{
    Q_OBJECT
    private:
        QtDcmStudyPrivate * d;

    public:
        /**
         * Default constructor
         */
        QtDcmStudy();

        /**
         * Default destructor
         */
        virtual
        ~QtDcmStudy()
        {
        }
        ;

        /**
         * Study Id getter
         *
         * @return _id as a QString
         */
        QString
        getId();

        /**
         * Study Id setter
         *
         * @param id as a QString
         */
        void
        setId(QString id);

        /**
         * Study description getter
         *
         * @return _description as a QString
         */
        QString
        getDescription();

        /**
         * Study description setter
         *
         * @param description as a QString
         */
        void
        setDescription(QString description);

        /**
         * Study date getter
         *
         * @return _date as a QString
         */
        QDate
        getDate();

        /**
         * Study date setter
         *
         * @param date as a QString
         */
        void
        setDate(QDate date);

        /**
         * Study time getter
         *
         * @return _time as a QString
         */
        QString
        getTime();

        /**
         * Study time setter
         *
         * @param time as a QString
         */
        void
        setTime(QString time);

        /**
         * List of series in the study getter
         *
         * @return _series as a QList of series
         * @see QtDcmSerie
         */
        QList<QtDcmSerie *>
        getSeries();

        /**
         * List of series in the study setter
         *
         * @param series as a QList of series
         * @see QtDcmSerie
         */
        void
        setSeries(QList<QtDcmSerie *> series);

        /**
         * Patient getter
         *
         * @return _p_patient as a patient
         * @see QtDcmPatient
         */
        QtDcmPatient *
        getPatient();

        /**
         * Patient setter
         *
         * @param patient
         * @see QtDcmPatient
         */
        void
        setPatient(QtDcmPatient * patient);

        /**
         * Add serie in the list
         */
        void
        addSerie(QtDcmSerie * serie);

        /**
         * Remove serie at position index
         */
        void
        removeSerie(int index);
};

#endif /* QTDCMSTUDY_H_ */
