/*
 * QtDcmServers.h
 *
 *  Created on: 11 déc. 2009
 *      Author: aabadie
 */

#ifndef QTDCMSERVERS_H_
#define QTDCMSERVERS_H_

#include <QtGui>

/**
 * This class is a representation of a Dicom PACS server
 */
class QtDcmServer : public QObject
  {
  Q_OBJECT
  private:
    QString _aetitle; /** Application entity title (AETitle) of the PACS server */
    QString _server; /** The hostname of the server */
    QString _port; /** TCP port the application is listening on */
    QString _name; /** Description name of the PACS */
  public:
    /**
     * Default constructor
     */
    QtDcmServer()
      {
      }

    /**
     * Default destructor
     */
    virtual
    ~QtDcmServer()
      {
      }

    /**
     * PACS AETitle getter
     *
     * @return _aetitle a QString
     */
    QString
    getAetitle() const
      {
        return _aetitle;
      }

    /**
     * Description name getter
     *
     * @return _name a QString
     */
    QString
    getName() const
      {
        return _name;
      }

    /**
     * PACS server port getter (QtDcm only ports between 1000 and 100000)
     *
     * @return _port as a QString
     */
    QString
    getPort() const
      {
        return _port;
      }

    /**
     * PACS server hostname getter
     *
     * @return _server as a QString
     */
    QString
    getServer() const
      {
        return _server;
      }

    /**
     * PACS AETitle setter
     *
     * @param _aetitle as a QString
     */
    void
    setAetitle( QString _aetitle )
      {
        this->_aetitle = _aetitle;
      }

    /**
     * PACS Name setter
     *
     * @param _name as a QString
     */
    void
    setName( QString _name )
      {
        this->_name = _name;
      }

    /**
     * PACS server port setter (QtDcm only ports between 1000 and 100000)
     *
     * @param port as a QString
     */
    void
    setPort( QString port )
      {
        this->_port = port;
      }

    /**
     * PACS server hostname setter
     *
     * @param _server as a QString
     */
    void
    setServer( QString _server )
      {
        this->_server = _server;
      }
  };

#endif /* QTDCMSERVERS_H_ */
