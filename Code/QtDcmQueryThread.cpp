/*
 * QtDcmQueryThread.cpp
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmQueryThread.h>
#include <QtDcmManager.h>

class QtDcmQueryThreadPrivate
{
    public:
        QString command;
        QStringList parameters;
        QtDcmManager * manager;
        QProcess * process;
};

void
QtDcmQueryThread::run()
  {
    // Use dcmqr
    d->process = new QProcess();
    d->process->start(d->command, d->parameters);

    QByteArray query;
    while (d->process->waitForReadyRead())
      query += d->process->readAll();

//    qDebug() << query;
    d->manager->setQuery(query);

    delete d->process;

    exit();
  }

QProcess *
QtDcmQueryThread::getProcess()
  {
    return d->process;
  }

void
QtDcmQueryThread::setCommand( QString command )
  {
    d->command = command;
  }

void
QtDcmQueryThread::setParameters( QStringList parameters )
  {
    d->parameters = parameters;
  }

void
QtDcmQueryThread::setManager( QtDcmManager * manager )
  {
    d->manager = manager;
  }
