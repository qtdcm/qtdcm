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
    _process = new QProcess();
    _process->start(_command, _parameters);

    QByteArray query;
    while (_process->waitForReadyRead())
      query += _process->readAll();

    _manager->setQuery(query);

    delete _process;

    exit();
  }
