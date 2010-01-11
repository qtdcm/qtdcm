/*
 * QtDcmQueryThread.h
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#ifndef QTDCMQUERYTHREAD_H_
#define QTDCMQUERYTHREAD_H_

#include <QtGui>
#include <QtDcmManager.h>

class QtDcmManager;
/*
 *
 */
class QtDcmQueryThread : public QThread
  {
  Q_OBJECT
  private:
    QString _command;
    QStringList _parameters;
    QtDcmManager * _manager;

  public:
    QtDcmQueryThread()
      {
      }
    virtual
    ~QtDcmQueryThread()
      {
      }

    void
    run();

    void
    setCommand( QString command )
      {
        _command = command;
      }

    void
    setParameters( QStringList parameters )
      {
        _parameters = parameters;
      }

    void
    setManager( QtDcmManager * manager )
      {
        _manager = manager;
      }
  };

#endif /* QTDCMQUERYTHREAD_H_ */
