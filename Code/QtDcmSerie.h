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

/*
 *
 */
class QtDcmSerie : public QObject
  {
  Q_OBJECT
  private:
    QString _id;
    QString _description;
    QList<QtDcmImage *> _images;
    QtDcmStudy * _p_study;

  public:
    QtDcmSerie();
    virtual
    ~QtDcmSerie();

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

    QList<QtDcmImage *>
    getImages()
      {
        return _images;
      }

    void
    setImages( QList<QtDcmImage *> images )
      {
        _images = images;
      }

    QtDcmStudy *
    getStudy()
      {
        return _p_study;
      }

    void
    setStudy( QtDcmStudy * study )
      {
        _p_study = study;
      }
  };

#endif /* QTDCMSERIE_H_ */
