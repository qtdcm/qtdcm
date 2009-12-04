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

/*
 *
 */
class QtDcmImage : public QObject
  {
  Q_OBJECT
  private:
    QString _id;
    QString _filename;
    QtDcmSerie * _p_serie;

  public:
    QtDcmImage();
    virtual
    ~QtDcmImage();

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
    getFilename()
      {
        return _filename;
      }

    void
    setFilename( QString filename )
      {
        _filename = filename;
      }

    QtDcmSerie *
    getSerie()
      {
        return _p_serie;
      }

    void
    setSerie( QtDcmSerie * serie )
      {
        _p_serie = serie;
      }

  };

#endif /* QTDCMIMAGE_H_ */
