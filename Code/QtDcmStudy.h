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
#include <QtDcmSerie.h>
#include <QtDcmPatient.h>

class QtDcmPatient;
class QtDcmSerie;

/**
 * This class is representation of a Dicom study.
 */
class QtDcmStudy : public QObject
  {
  Q_OBJECT
  private:
    QString _id; /** Dicom study identificator */
    QString _description; /** Dicom study description */
    QString _date; /** Dicom study date */
    QString _time; /** Dicom study time */
    QList<QtDcmSerie *> _series; /** List of series in the study */
    QtDcmPatient * _p_patient; /** Patient corresponding to the study */

  public:
    /**
     * Default constructor
     */
    QtDcmStudy() {};

    /**
     * Default destructor
     */
    virtual
    ~QtDcmStudy() {};

    /**
     * Study Id getter
     *
     * @return _id as a QString
     */
    QString
    getId()
      {
        return _id;
      }

    /**
     * Study Id setter
     *
     * @param id as a QString
     */
    void
    setId( QString id )
      {
        _id = id;
      }

    /**
     * Study description getter
     *
     * @return _description as a QString
     */
    QString
    getDescription()
      {
        return _description;
      }

    /**
     * Study description setter
     *
     * @param description as a QString
     */
    void
    setDescription( QString description )
      {
        _description = description;
      }

    /**
     * Study date getter
     *
     * @return _date as a QString
     */
    QString
    getDate()
      {
        return _date;
      }

    /**
     * Study date setter
     *
     * @param date as a QString
     */
    void
    setDate( QString date )
      {
        _date= date;
      }

    /**
     * Study time getter
     *
     * @return _time as a QString
     */
    QString
    getTime()
      {
        return _time;
      }

    /**
     * Study time setter
     *
     * @param time as a QString
     */
    void
    setTime( QString time )
      {
        _time = time;
      }

    /**
     * List of series in the study getter
     *
     * @return _series as a QList of series
     * @see QtDcmSerie
     */
    QList<QtDcmSerie *>
    getSeries()
      {
        return _series;
      }

    /**
     * List of series in the study setter
     *
     * @param series as a QList of series
     * @see QtDcmSerie
     */
    void setSeries(QList<QtDcmSerie *> series) {_series = series;}

    /**
     * Patient getter
     *
     * @return _p_patient as a patient
     * @see QtDcmPatient
     */
    QtDcmPatient *
    getPatient()
      {
        return _p_patient;
      }

    /**
     * Patient setter
     *
     * @param patient
     * @see QtDcmPatient
     */
    void
    setPatient( QtDcmPatient * patient )
      {
        _p_patient = patient;
      }

    /**
     * Add serie in the list
     */
    void addSerie(QtDcmSerie * serie)
      {
        _series.append(serie);
      }

    /**
     * Remove serie at position index
     */
    void removeSerie(int index)
      {
        _series.removeAt(index);
      }
  };

#endif /* QTDCMSTUDY_H_ */
