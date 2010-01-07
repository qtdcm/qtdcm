/*
 * QtDcmQueryThread.cpp
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmQueryThread.h>

void
QtDcmQueryThread::run()
  {
    QProcess * process = new QProcess(this);
    process->start(_command, _parameters);

    QByteArray query;
    while (process->waitForReadyRead())
      query += process->readAll();

    _manager->setQuery(query);

    delete process;

    exit();
  }
