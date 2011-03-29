/*
 * QtDcmExportThread.h
 *
 *  Created on: 6 janv. 2010
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMEXPORTTHREAD_H_
#define QTDCMEXPORTTHREAD_H_

#include <QtGui>

class QtDcmExportThreadPrivate;

/*
 *
 */
class QtDcmExportThread : public QThread
  {
  Q_OBJECT
  private:
  QtDcmExportThreadPrivate * d;

  public:
    QtDcmExportThread()
      {
      }
    virtual
    ~QtDcmExportThread()
      {
      }

    void
    run();

    void
    setProgram( QString program );

    void
    setServerPacsParam( QString param );

    void
    setLocalPacsParam( QString param );

    void
    setTemporaryDir( QString dir );

    void
    setSeriesToExport( QList<QString> list );

    void
    setModality( QString modality );

    void
    setAetitle( QString ae );
  };

#endif /* QTDCMEXPORTTHREAD_H_ */
