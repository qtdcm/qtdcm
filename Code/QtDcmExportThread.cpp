/*
 * QtDcmExportThread.cpp
 *
 *  Created on: 6 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmExportThread.h>

class QtDcmExportThreadPrivate
{
    public:
        QString program;
        QString serverPacsParam;
        QString localPacsParam;
        QString temporaryDir;
        QList<QString> seriesToExport;
        QString modality;
        QString aetitle;
};

void
QtDcmExportThread::run()
{
    QDir tempDir(d->temporaryDir);
    for (int i = 0; i < d->seriesToExport.size(); i++) {
        if (!tempDir.exists(d->seriesToExport.at(i))) {
            tempDir.mkdir(d->seriesToExport.at(i));
            QString seriesId = "-qSeriesInstanceUID=" + d->seriesToExport.at(i);
            QProcess * process = new QProcess();
            QStringList parameters;
            parameters << "-L" << d->localPacsParam << d->serverPacsParam;
            parameters << "-I" << "-cmove" << d->aetitle;
            parameters << "-cstore=" + d->modality << seriesId;
            // Test requete alexandre
            parameters << "-cstore PR:LE";
            parameters << "-cstoredest";
            parameters << QDir(d->temporaryDir + QDir::separator() + d->seriesToExport.at(i)).absolutePath();
            process->start(d->program, parameters);
            process->waitForFinished();
            delete process;
        }
    }
    exit();
}

void
QtDcmExportThread::setProgram( QString program )
  {
    d->program = program;
  }

void
QtDcmExportThread::setServerPacsParam( QString param )
  {
    d->serverPacsParam = param;
  }

void
QtDcmExportThread::setLocalPacsParam( QString param )
  {
    d->localPacsParam = param;
  }

void
QtDcmExportThread::setTemporaryDir( QString dir )
  {
    d->temporaryDir = dir;
  }

void
QtDcmExportThread::setSeriesToExport( QList<QString> list )
  {
    d->seriesToExport = list;
  }

void
QtDcmExportThread::setModality( QString modality )
  {
    d->modality = modality;
  }

void
QtDcmExportThread::setAetitle( QString ae )
  {
    d->aetitle = ae;
  }
