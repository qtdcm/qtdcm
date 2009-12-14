/*
 * QtDcmPreferences.h
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#ifndef QTDCMPREFERENCES_H_
#define QTDCMPREFERENCES_H_

#include <QtGui>
#include <QtDcmServer.h>
/*
 * On va utiliser la classe QSettings pour gérer les préférences de l'application
 */
class QtDcmPreferences : public QObject
  {
  Q_OBJECT
  private:
    QFile _iniFile;
    QString _aetitle;
    QString _port;
    QString _encoding;
    QList<QtDcmServer *> _servers;

    void
    setDefaultIniFile();

  public:
    QtDcmPreferences();
    virtual
    ~QtDcmPreferences();

    void
    readSettings();
    void
    writeSettings();

    QString
    getAetitle() const
      {
        return _aetitle;
      }

    QString
    getEncoding() const
      {
        return _encoding;
      }

    QString
    getPort() const
      {
        return _port;
      }

    QList<QtDcmServer *>
    getServers()
      {
        return _servers;
      }

    void
    setAetitle( QString _aetitle )
      {
        this->_aetitle = _aetitle;
      }

    void
    setEncoding( QString encoding )
      {
        this->_encoding = encoding;
      }

    void
    setPort( QString _port )
      {
        this->_port = _port;
      }

    void
    addServer();

    void
    removeServer(int index);

    void
    setServers( QList<QtDcmServer *> servers )
      {
        this->_servers = servers;
      }
  };

#endif /* QTDCMPREFERENCES_H_ */
