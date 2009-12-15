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

/**
 * This class is a representation of a dicom patient
 */
class QtDcmPatient : public QObject
  {
  Q_OBJECT
  private:
    QString _id; /** Patient dicom identificator */
    QString _name; /** Patient name as found in the dicom format*/
    QString _birthdate; /** Patient birth date */
    QString _sex; /** Patient sex */

    QList<QtDcmStudy *> _studies; /** List of a study for this patient */

  public:
    /**
     * Default constructor
     */
    QtDcmPatient()
      {
      }

    /**
     * Default destructor
     */
    virtual
    ~QtDcmPatient()
      {
      }

    /**
     * Id getter
     *
     * @return _id a QString
     */
    QString
    getId()
      {
        return _id;
      }

    /**
     * Id setter
     *
     * @param id a QString
     */
    void
    setId( QString id )
      {
        _id = id;
      }

    /**
     * Patient name getter
     *
     * @return _name a QString
     */
    QString
    getName()
      {
        return _name;
      }

    /**
     * Patient name setter
     *
     * @param name a QString
     */
    void
    setName( QString name )
      {
        _name = name;
      }

    /**
     * Patient birthdate getter
     *
     * @return _birthdate
     */
    QString
    getBirthdate()
      {
        return _birthdate;
      }

    /**
     * Patient birthdate setter
     *
     * @param birthdate
     */
    void
    setBirthdate( QString birthdate )
      {
        _birthdate = birthdate;
      }

    /**
     * Patient sex getter
     *
     * @return _sex a QString
     */
    QString
    getSex()
      {
        return _sex;
      }

    /**
     * Patient sex setter
     *
     * @param sex a QString
     */
    void
    setSex( QString sex )
      {
        _sex = sex;
      }

    /**
     * Patient studies list getter
     *
     * @return _studies a QList of studies
     * @see QtDcmStudy
     */
    QList<QtDcmStudy *>
    getStudies()
      {
        return _studies;
      }

    /**
     * Patient studies list setter
     *
     * @param studies a QList of studies
     * @see QtDcmStudy
     */
    void
    setStudies( QList<QtDcmStudy *> studies )
      {
        _studies = studies;
      }

  };

#endif /* QTDCMPATIENT_H_ */
