/*
 * QtDcmQueryThread.h
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#ifndef QTDCMQUERYTHREAD_H_
#define QTDCMQUERYTHREAD_H_

#include <QtGui>

class QtDcmManager;

class QtDcmQueryThreadPrivate;
/*
 *
 */

class QtDcmQueryThread : public QThread
{
    Q_OBJECT

private:
    QtDcmQueryThreadPrivate * d;

public:
    QtDcmQueryThread() {}

    virtual ~QtDcmQueryThread() {}

    void run();

    QProcess * getProcess();

    void setCommand ( QString command );
    void setParameters ( QStringList parameters );
    void setManager ( QtDcmManager * manager );
};

#endif /* QTDCMQUERYTHREAD_H_ */
