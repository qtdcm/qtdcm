/*
 * QtDcmExportThread.cpp
 *
 *  Created on: 6 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmExportThread.h>

void
QtDcmExportThread::run()
  {
    QDir tempDir(_temporaryDir);
    for (int i = 0; i < _seriesToExport.size(); i++)
      {
        if (!tempDir.exists(_seriesToExport.at(i)))
          {
            tempDir.mkdir(_seriesToExport.at(i));
            QString seriesId = "-qSeriesInstanceUID=" + _seriesToExport.at(i);
            QProcess * process = new QProcess();
            QStringList parameters;
            parameters << "-L" << _localPacsParam << _serverPacsParam;
            parameters << "-I" << "-cmove" << _aetitle;
            parameters << "-cstore=" + _modality << seriesId;
            // Test requete alexandre
            parameters << "-cstore PR:LE";
            parameters << "-cstoredest";
            parameters << QDir(_temporaryDir + QDir::separator() + _seriesToExport.at(i)).absolutePath();
            process->start(_program, parameters);
            process->waitForFinished();
            delete process;
          }
      }
    exit();
  }
