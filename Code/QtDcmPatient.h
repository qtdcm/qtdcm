/*
 * QtDcmPatient.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMPATIENT_H_
#define QTDCMPATIENT_H_

#include <QtGui>
#include <QtDcmImage.h>
#include <QtDcmSerie.h>
#include <QtDcmStudy.h>
#include <QtDcmPatient.h>

class QtDcmStudy;

/*
 *
 */
class QtDcmPatient : public QObject
  {
  Q_OBJECT
  private:
    QString _id;
    QString _name;
    QString _birthdate;
    QString _sex;

    QList<QtDcmStudy *> _studies;

  public:
    QtDcmPatient();
    virtual
    ~QtDcmPatient();

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
    getName()
      {
        return _name;
      }
    void
    setName( QString name )
      {
        _name = name;
      }

    QString
    getBirthdate()
      {
        return _birthdate;
      }
    void
    setBirthdate( QString birthdate )
      {
        _birthdate = birthdate;
      }

    QString
    getSex()
      {
        return _sex;
      }
    void
    setSex( QString sex )
      {
        _sex = sex;
      }

    QList<QtDcmStudy *>
    getStudies()
      {
        return _studies;
      }

    void setStudies(QList<QtDcmStudy *> studies) {_studies = studies;}

  };

#endif /* QTDCMPATIENT_H_ */
