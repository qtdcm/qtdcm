/*
 * QtDcmServers.h
 *
 *  Created on: 11 déc. 2009
 *      Author: aabadie
 */

#ifndef QTDCMSERVERS_H_
#define QTDCMSERVERS_H_

#include <QtGui>

/*
 *
 */
class QtDcmServer : public QObject
  {
  Q_OBJECT
  private:
    QString _aetitle;
    QString _server;
    QString _port;
    QString _name;
  public:
    QtDcmServer() {};
    virtual
    ~QtDcmServer() {};

    QString
    getAetitle() const
      {
        return _aetitle;
      }

    QString
    getName() const
      {
        return _name;
      }

    QString
    getPort() const
      {
        return _port;
      }

    QString
    getServer() const
      {
        return _server;
      }

    void
    setAetitle( QString _aetitle )
      {
        this->_aetitle = _aetitle;
      }

    void
    setName( QString _name )
      {
        this->_name = _name;
      }

    void
    setPort( QString _port )
      {
        this->_port = _port;
      }

    void
    setServer( QString _server )
      {
        this->_server = _server;
      }
  };

#endif /* QTDCMSERVERS_H_ */
