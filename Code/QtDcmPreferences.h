/*
 * QtDcmPreferences.h
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#ifndef QTDCMPREFERENCES_H_
#define QTDCMPREFERENCES_H_

#include <QtGui>

class QtDcmServer;

class QtDcmPreferencesPrivate;
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
    void readSettings();

    /**
     * Store current settings in the ini file.
     * This method use the QSettings object to store the current settings in the ini file.
     */
    void writeSettings();

    /**
     * QtDcm local AETitle getter
     *
     * @return _aetitle as a QString
     */
    QString getAetitle() const;

    /**
     * QtDcm local encoding getter
     *
     * @return _encoding as a QString
     */
    QString getEncoding() const;

    /**
     * QtDcm local application TCP port getter
     *
     * @return _port as a QString
     */
    QString getPort() const;

    /**
     * QtDcm server list getter
     *
     * @return _servers as a QList of server
     * @see QtDcmServer
     */
    QList<QtDcmServer *> getServers();

    /**
     * Dcm2nii  getter
     *
     * @return _dcm2nii as a QString
     */
    QString getDcm2nii();

    /**
     * Dcm4che getter
     *
     * @return _dcm4che as a QString
     */
    QString getDcm4che();

    /**
     * Dcm2nii path setter
     *
     * @param as a QString
     */
    void setDcm2nii ( QString path );

    /**
     * Dcm4che setter
     *
     * @param as a QString
     */
    void setDcm4che ( QString path );

    /**
     * QtDcm local AETitle setter
     *
     * @param _aetitle as a QString
     */
    void setHostname ( QString hostname );

    QString getHostname();

    /**
     * QtDcm local AETitle setter
     *
     * @param _aetitle as a QString
     */
    void setAetitle ( QString aetitle );

    /**
     * QtDcm local encoding setter
     *
     * @param encoding as a QString
     */
    void setEncoding ( QString encoding );

    /**
     * QtDcm local application TCP port setter
     *
     * @param _port as a QString
     */
    void setPort ( QString port );

    /**
     * Add server to the QList
     */
    void addServer();

    /**
     * Remove server from the QList at position i
     *
     * @param i index of the server to be removed in the list
     */
    void removeServer ( int index );

    /**
     * QtDcm server list setter
     *
     * @param servers as a QList of servers
     * @see QtDcmServer
     */
    void setServers ( QList<QtDcmServer *> servers );

public slots:
    void sendEcho ( int index );

private:
    /**
     * This method create the ini file with default parameters if it doesn't exist (first execution of qtdcm)
     */
    void setDefaultIniFile();

    QtDcmPreferencesPrivate * d;
};

#endif /* QTDCMPREFERENCES_H_ */
