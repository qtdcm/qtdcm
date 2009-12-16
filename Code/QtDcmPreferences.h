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
/**
 * Class that manages the settings of QtDcm
 *
 * The Settings are organized like this :\n
 * [LocalSettings]\n
 * AETitle=""\n
 * Port=""\n
 * Encoding=""\n
 *\n
 * [Servers]\n
 * Server1\\AETitle=""\n
 * Server1\\Hostname=""\n
 * Server1\\Port=""\n
 * Server1\\Name=""\n
 * ...\n
 *\n
 *
 *
 * @todo Add path to dcm2nii and dcm4che in the preferences
 */
class QtDcmPreferences : public QObject
  {
  Q_OBJECT
  private:
    QFile _iniFile; /** File stored on the file system that contains the preferences */
    QString _aetitle; /** Local aetitle of QtDcm */
    QString _port; /** Local port of qtdcm */
    QString _encoding; /** Local character encoding */

    QString _dcm2niiPath; /** Absolute path where to find the dcm2nii binary on the system */
    QString _dcm4chePath; /** Absolute path where to find the dcm4che binary on the system */

    QList<QtDcmServer *> _servers; /** List of server that QtDcm can query */

    /**
     * This method create the ini file with default parameters if it doesn't exist (first execution of qtdcm)
     */
    void
    setDefaultIniFile();

  public:
    /**
     * Default constructor
     */
    QtDcmPreferences();

    /**
     * Default destructor
     */
    virtual
    ~QtDcmPreferences() {};

    /**
     * Read the settings from the ini file.
     * This method use the QSettings object and initialize the private attributes with the content of the ini file.
     */
    void
    readSettings();

    /**
     * Store current settings in the ini file.
     * This method use the QSettings object to store the current settings in the ini file.
     */
    void
    writeSettings();

    /**
     * QtDcm local AETitle getter
     *
     * @return _aetitle as a QString
     */
    QString
    getAetitle() const
      {
        return _aetitle;
      }

    /**
     * QtDcm local encoding getter
     *
     * @return _encoding as a QString
     */
    QString
    getEncoding() const
      {
        return _encoding;
      }

    /**
     * QtDcm local application TCP port getter
     *
     * @return _port as a QString
     */
    QString
    getPort() const
      {
        return _port;
      }

    /**
     * QtDcm server list getter
     *
     * @return _servers as a QList of server
     * @see QtDcmServer
     */
    QList<QtDcmServer *>
    getServers()
      {
        return _servers;
      }

    /**
     * Dcm2nii path getter
     *
     * @return _dcm2niiPath as a QString
     */
    QString
    getDcm2niiPath()
      {
        return _dcm2niiPath;
      }

    /**
     * Dcm4che path getter
     *
     * @return _dcm4chePath as a QString
     */
    QString
    getDcm4chePath()
      {
        return _dcm4chePath;
      }

    /**
     * Dcm2nii path setter
     *
     * @param path as a QString
     */
    void
    setDcm2niiPath(QString path)
      {
        _dcm2niiPath = path;
      }

    /**
     * Dcm4che path setter
     *
     * @param path as a QString
     */
    void
    setDcm4chePath(QString path)
      {
        _dcm4chePath = path;
      }

    /**
     * QtDcm local AETitle setter
     *
     * @param _aetitle as a QString
     */
    void
    setAetitle( QString _aetitle )
      {
        this->_aetitle = _aetitle;
      }

    /**
     * QtDcm local encoding setter
     *
     * @param encoding as a QString
     */
    void
    setEncoding( QString encoding )
      {
        this->_encoding = encoding;
      }

    /**
     * QtDcm local application TCP port setter
     *
     * @param _port as a QString
     */
    void
    setPort( QString _port )
      {
        this->_port = _port;
      }

    /**
     * Add server to the QList
     */
    void
    addServer();

    /**
     * Remove server from the QList at position i
     *
     * @param i index of the server to be removed in the list
     */
    void
    removeServer(int index);

    /**
     * QtDcm server list setter
     *
     * @param servers as a QList of servers
     * @see QtDcmServer
     */
    void
    setServers( QList<QtDcmServer *> servers )
      {
        this->_servers = servers;
      }
  };

#endif /* QTDCMPREFERENCES_H_ */
