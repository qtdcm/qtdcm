/*
 * QtDcmSerie.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMSERIE_H_
#define QTDCMSERIE_H_

#include <QtGui>

class QtDcmSeriePrivate;
class QtDcmStudy;
class QtDcmImage;

/**
 * This class is a representation of a dicom serie
 */
class QtDcmSerie : public QObject
  {
  Q_OBJECT
  private:
   QtDcmSeriePrivate * d;

  public:
    /**
     * Default constructor
     */
    QtDcmSerie();

    /**
     * Default destructor
     */
    virtual
    ~QtDcmSerie() {};

    /**
     * Serie Id getter
     *
     * @return _id a QString
     */
    QString
    getId();

    /**
     * Serie Id setter
     *
     * @param id is a QString
     */
    void
    setId( QString id );

    /**
     * Serie date getter
     *
     * @return _date a QString
     */
    QString
    getDate();

    /**
     * Serie date setter
     *
     * @param date is a QString
     */
    void
    setDate( QString date );

    /**
     * Serie description getter
     *
     * @return _description a QString
     */
    QString
    getDescription();

    /**
     * Serie description setter
     *
     * @param description a QString
     */
    void
    setDescription( QString description );

    /**
     * Image list getter
     *
     * @return _images a QList of images
     * @see QtDcmImage
     */
    QList<QtDcmImage *>
    getImages();

    /**
     * Image list setter
     *
     * @param images a QList of images
     * @see QtDcmImage
     */
    void
    setImages( QList<QtDcmImage *> images );

    /**
     * Parent study getter
     *
     * @return _p_study a pointer on the parent study
     * @see QtDcmStudy
     */
    QtDcmStudy *
    getStudy();

    /**
     * Parent study setter
     *
     * @param study a pointer on the parent study
     * @see QtDcmStudy
     */
    void
    setStudy( QtDcmStudy * study );
  };

#endif /* QTDCMSERIE_H_ */
