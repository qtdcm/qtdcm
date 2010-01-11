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
        if (! tempDir.exists(_seriesToExport.at(i)))
          {
            tempDir.mkdir(_seriesToExport.at(i));
            QString seriesId = "-qSeriesInstanceUID=" + _seriesToExport.at(i);
            QString command = _program + " -L " + _localPacsParam + " " + _serverPacsParam + " -I" + " -cmove " + _aetitle + " -cstore=" + _modality + " " + seriesId + " -cstoredest " + QDir(
                _temporaryDir + QDir::separator() + _seriesToExport.at(i)).absolutePath() + ">/dev/null";

            system(command.toAscii().data());
          }
      }
    exit();
  }
