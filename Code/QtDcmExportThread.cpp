/*
 * QtDcmExportThread.cpp
 *
 *  Created on: 6 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmExportThread.h>

QtDcmExportThread::QtDcmExportThread()
  {
    _command = "";
  }

void
QtDcmExportThread::run()
  {
    if (!_command.isEmpty())
      {
        system(_command.toAscii().data());
        exit();
      }
  }
