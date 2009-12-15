/*
 * QtDcmSerie.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMSERIE_H_
#define QTDCMSERIE_H_

#include <QtGui>

#include <QtDcmImage.h>
#include <QtDcmStudy.h>

class QtDcmStudy;
class QtDcmImage;

/**
 * This class is a representation of a dicom serie
 */
class QtDcmSerie : public QObject
  {
  Q_OBJECT
  private:
    QString _id; /** Serie dicom identificator */
    QString _description; /** Serie description */
    QList<QtDcmImage *> _images; /** List of images in the serie */
    QtDcmStudy * _p_study; /** Pointer on the parent study */

  public:
    /**
     * Default constructor
     */
    QtDcmSerie();

    /**
     * Default destructor
     */
    virtual
    ~QtDcmSerie();

    /**
     * Serie Id getter
     *
     * @return _id a QString
     */
    QString
    getId()
      {
        return _id;
      }

    /**
     * Serie Id setter
     *
     * @param is a QString
     */
    void
    setId( QString id )
      {
        _id = id;
      }

    /**
     * Serie description getter
     *
     * @return _description a QString
     */
    QString
    getDescription()
      {
        return _description;
      }

    /**
     * Serie description setter
     *
     * @param description a QString
     */
    void
    setDescription( QString description )
      {
        _description = description;
      }

    /**
     * Image list getter
     *
     * @return _images a QList of images
     * @see QtDcmImage
     */
    QList<QtDcmImage *>
    getImages()
      {
        return _images;
      }

    /**
     * Image list setter
     *
     * @param images a QList of images
     * @see QtDcmImage
     */
    void
    setImages( QList<QtDcmImage *> images )
      {
        _images = images;
      }

    /**
     * Parent study getter
     *
     * @return _p_study a pointer on the parent study
     * @see QtDcmStudy
     */
    QtDcmStudy *
    getStudy()
      {
        return _p_study;
      }

    /**
     * Parent study setter
     *
     * @param study a pointer on the parent study
     * @see QtDcmStudy
     */
    void
    setStudy( QtDcmStudy * study )
      {
        _p_study = study;
      }
  };

#endif /* QTDCMSERIE_H_ */
