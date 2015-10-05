/*
    QtDcm is a C++ Qt based library for communication and conversion of Dicom images.
    Copyright (C) 2011  Alexandre Abadie <Alexandre.Abadie@univ-rennes1.fr>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef QTDCMPREFERENCES_H_
#define QTDCMPREFERENCES_H_

#include "qtdcmExports.h"
#include <QtGui>
#include <QtDcmServer.h>

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

class QTDCM_EXPORT QtDcmPreferences : public QObject
{
    Q_OBJECT

public:
    static QtDcmPreferences * instance();
    static void destroy();
    
    /**
     * Default constructor
     */
    QtDcmPreferences(QObject * parent = 0);

    /**
     * Default destructor
     */
    virtual ~QtDcmPreferences();

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

    void setIniFile ( const QString ini );

    QString iniFile ( void );

    /**
     * QtDcm local AETitle getter
     *
     * @return _aetitle as a QString
     */
    QString aetitle() const;

    /**
     * QtDcm local application TCP port getter
     *
     * @return _port as a QString
     */
    QString port() const;

    /**
     * QtDcm server list getter
     *
     * @return _servers as a QList of server
     * @see QtDcmServer
     */
    QList<QtDcmServer> servers() const;

    /**
     * QtDcm local AETitle setter
     *
     * @param _aetitle as a QString
     */
    void setHostname ( const QString & hostname );

    QString hostname() const;

    /**
     * QtDcm local AETitle setter
     *
     * @param _aetitle as a QString
     */
    void setAetitle ( const QString & aetitle );

    /**
     * QtDcm local application TCP port setter
     *
     * @param _port as a QString
     */
    void setPort ( const QString & port );


    QString dcm2niiPath() const;

    void setDcm2niiPath ( const QString & path );

    bool useDcm2nii() const;

    void setUseDcm2nii ( bool use );

    /**
     * Add server to the QList
     */
    void addServer(const QtDcmServer& server);

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
    void setServers ( const QList<QtDcmServer> & servers );

signals:
    void preferencesUpdated();

private:
    /**
     * This method create the ini file with default parameters if it doesn't exist (first execution of qtdcm)
     */
    void setDefaultIniFile();

    static QtDcmPreferences * _instance;
    QtDcmPreferencesPrivate * d;
};

#endif /* QTDCMPREFERENCES_H_ */
