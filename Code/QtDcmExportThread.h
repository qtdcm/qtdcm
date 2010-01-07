/*
 * QtDcmExportThread.h
 *
 *  Created on: 6 janv. 2010
 *      Author: Alexandre Abadie
 */

#ifndef QTDCMEXPORTTHREAD_H_
#define QTDCMEXPORTTHREAD_H_

#include <QtGui>

/*
 *
 */
class QtDcmExportThread : public QThread
  {
  private:
    QString _command;

  public:
    QtDcmExportThread();
    virtual
    ~QtDcmExportThread()
      {
      }

    void
    run();

    QString
    getCommand()
      {
        return _command;
      }

    void
    setCommand( QString command )
      {
        _command = command;
      }
  };

#endif /* QTDCMEXPORTTHREAD_H_ */
