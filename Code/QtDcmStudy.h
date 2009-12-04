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

/*
 *
 */
class QtDcmStudy : public QObject
  {
  Q_OBJECT
  private:
    QString _id;
    QString _description;
    QString _date;
    QString _time;
    QList<QtDcmSerie *> _series;
    QtDcmPatient * _p_patient;

  public:
    QtDcmStudy();
    virtual
    ~QtDcmStudy();

    QString
    getId()
      {
        return _id;
      }

    void
    setId( QString id )
      {
        _id = id;
      }

    QString
    getDescription()
      {
        return _description;
      }

    void
    setDescription( QString description )
      {
        _description = description;
      }

    QString
    getDate()
      {
        return _date;
      }

    void
    setDate( QString date )
      {
        _date= date;
      }

    QString
    getTime()
      {
        return _time;
      }

    void
    setTime( QString time )
      {
        _time = time;
      }

    QList<QtDcmSerie *>
    getSeries()
      {
        return _series;
      }

    void setSeries(QList<QtDcmSerie *> series) {_series = series;}

    QtDcmPatient *
    getPatient()
      {
        return _p_patient;
      }

    void
    setPatient( QtDcmPatient * patient )
      {
        _p_patient = patient;
      }
  };

#endif /* QTDCMSTUDY_H_ */
