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
  Q_OBJECT
  private:
    QString _program;
    QString _serverPacsParam;
    QString _localPacsParam;
    QString _temporaryDir;
    QList<QString> _seriesToExport;
    QString _modality;
    QString _aetitle;

  public:
    QtDcmExportThread()
      {
      }
    virtual
    ~QtDcmExportThread()
      {
      }

    void
    run();

    void
    setProgram( QString program )
      {
        _program = program;
      }

    void
    setServerPacsParam( QString param )
      {
        _serverPacsParam = param;
      }

    void
    setLocalPacsParam( QString param )
      {
        _localPacsParam = param;
      }

    void
    setTemporaryDir( QString dir )
      {
        _temporaryDir = dir;
      }

    void
    setSeriesToExport( QList<QString> list )
      {
        _seriesToExport = list;
      }

    void
    setModality( QString modality )
      {
        _modality = modality;
      }

    void
    setAetitle( QString ae )
      {
        _aetitle = ae;
      }
  };

#endif /* QTDCMEXPORTTHREAD_H_ */
