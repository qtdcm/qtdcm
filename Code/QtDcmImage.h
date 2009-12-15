/*
 * QtDcmImage.h
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMIMAGE_H_
#define QTDCMIMAGE_H_

#include <QtGui>
#include <QtDcmSerie.h>

class QtDcmSerie;

/**
 * This class is a representation of a dicom image. It contains dicom id and filename of the image.
 * There is also a pointer to the parent serie (not used for the moment)
 *
 */
class QtDcmImage : public QObject
  {
  Q_OBJECT
  private:
    QString _id; /** Dicom ID of the image */
    QString _filename; /** Filename path of the dicom image */
    QtDcmSerie * _p_serie; /** Parent serie */

  public:
    /**
     * Default constructor
     */
    QtDcmImage() {};

    /**
     * Default destructor
     */
    virtual
    ~QtDcmImage() {};

    /**
     * Id getter
     *
     * @return _id
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
     * Image filename getter
     *
     * @return _filename
     */
    QString
    getFilename()
      {
        return _filename;
      }

    /**
     * Image filename setter
     *
     * @param filename a QString containing absolute path
     */
    void
    setFilename( QString filename )
      {
        _filename = filename;
      }

    /**
     * Parent serie getter
     *
     * @return _p_serie the pointer on the parent serie object
     * @see QtDcmSerie
     */
    QtDcmSerie *
    getSerie()
      {
        return _p_serie;
      }

    /**
     * Parent serie setter
     *
     * @param serie a pointer on the parent serie object
     * @see QtDcmSerie
     */
    void
    setSerie( QtDcmSerie * serie )
      {
        _p_serie = serie;
      }

  };

#endif /* QTDCMIMAGE_H_ */
