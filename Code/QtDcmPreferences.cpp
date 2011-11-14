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


#include <QtDcmServer.h>
#include <QtDcmPreferences.h>
#include <QtDcmManager.h>

class QtDcmPreferencesPrivate
{
public:
    QFile iniFile; /** File stored on the file system that contains the preferences */
    QString aetitle; /** Local aetitle of QtDcm */
    QString port; /** Local port of qtdcm */
    QString hostname;

    bool useDcm2nii;
    QString dcm2niiPath;

    QList<QtDcmServer *> servers; /** List of server that QtDcm can query */
};

QtDcmPreferences * QtDcmPreferences::_instance = 0;

QtDcmPreferences::QtDcmPreferences() : d ( new QtDcmPreferencesPrivate )
{
}

QtDcmPreferences* QtDcmPreferences::instance()
{
    if ( _instance == 0 )
        _instance = new QtDcmPreferences();
    return _instance;
}

void QtDcmPreferences::addServer()
{
    d->servers.append ( new QtDcmServer );
}

void QtDcmPreferences::removeServer ( int index )
{
    d->servers.removeAt ( index );
}

void QtDcmPreferences::readSettings()
{
    if ( !d->iniFile.exists() )
        this->setDefaultIniFile();
    //Instantiate a QSettings object from the ini file.
    QSettings prefs ( d->iniFile.fileName(), QSettings::IniFormat );
    //Load local settings
    prefs.beginGroup ( "LocalSettings" );
    d->aetitle = prefs.value ( "AETitle" ).toString();
    d->port = prefs.value ( "Port" ).toString();
    d->hostname = prefs.value ( "Hostname" ).toString();
    prefs.endGroup();

    prefs.beginGroup ( "Converter" );
    d->useDcm2nii = prefs.value ( "UseDcm2nii" ).toBool();
    d->dcm2niiPath = prefs.value ( "Dcm2nii" ).toString();
    prefs.endGroup();

    //For each server load corresponding settings
    prefs.beginGroup ( "Servers" );
    for ( int i = 0; i < prefs.childGroups().size(); i++ )
    {
        d->servers.append ( new QtDcmServer );
        d->servers.at ( i )->setAetitle ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/AETitle" ).toString() );
        d->servers.at ( i )->setServer ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/Hostname" ).toString() );
        d->servers.at ( i )->setPort ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/Port" ).toString() );
        d->servers.at ( i )->setName ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/Name" ).toString() );
    }
    prefs.endGroup();

    emit preferencesUpdated();
}

void QtDcmPreferences::writeSettings()
{
    //Remove settings file
    QFile ini ( d->iniFile.fileName() );
    ini.remove();
    //Instantiate a QSettings object with the ini file.
    QSettings prefs ( d->iniFile.fileName(), QSettings::IniFormat );
    //Write local settings from the private attributes
    prefs.beginGroup ( "LocalSettings" );
    prefs.setValue ( "AETitle", d->aetitle );
    prefs.setValue ( "Port", d->port );
    prefs.setValue ( "Hostname", d->hostname );
    prefs.endGroup();

    prefs.beginGroup ( "Converter" );
    prefs.setValue ( "Dcm2nii", d->dcm2niiPath );
    prefs.setValue ( "UseDcm2nii", d->useDcm2nii );
    prefs.endGroup();

    //Do the job for each server
    prefs.beginGroup ( "Servers" );
    for ( int i = 0; i < d->servers.size(); i++ )
    {
        prefs.beginGroup ( "Server" + QString::number ( i + 1 ) );
        prefs.setValue ( "AETitle", d->servers.at ( i )->getAetitle() );
        prefs.setValue ( "Hostname", d->servers.at ( i )->getServer() );
        prefs.setValue ( "Port", d->servers.at ( i )->getPort() );
        prefs.setValue ( "Name", d->servers.at ( i )->getName() );
        prefs.endGroup();
    }

    prefs.endGroup();

    emit preferencesUpdated();
}

void QtDcmPreferences::setDefaultIniFile()
{
    //Create an ini file with default parameters
    d->aetitle = "QTDCM";
    d->port = "2010";
    d->hostname = "localhost";

    d->dcm2niiPath = "";
    d->useDcm2nii = 0;

    QtDcmServer * server = new QtDcmServer();
    server->setAetitle ( "SERVER" );
    server->setServer ( "hostname" );
    server->setName ( "Server" + QString::number ( d->servers.size() + 1 ) );
    server->setPort ( "11112" );
    d->servers.append ( server );
    this->writeSettings();
}

QString QtDcmPreferences::getIniFile ( void )
{
    return d->iniFile.fileName();
}

void QtDcmPreferences::setIniFile ( const QString ini )
{
    QDir iniDir = QDir ( QDir::homePath() + QDir::separator() + ".qtdcm" );

    if ( !iniDir.exists() )
        QDir::home().mkdir ( ".qtdcm" );

    d->iniFile.setFileName ( iniDir.absolutePath() + QDir::separator() + ini );

    if ( !d->iniFile.exists() )
        this->setDefaultIniFile(); //If it doesn't exist create it with default parameters
    else
        this->readSettings();

    emit preferencesUpdated();
}

QString QtDcmPreferences::getAetitle() const
{
    return d->aetitle;
}

QString QtDcmPreferences::getPort() const
{
    return d->port;
}

QList<QtDcmServer *> QtDcmPreferences::getServers()
{
    return d->servers;
}

void QtDcmPreferences::setHostname ( QString hostname )
{
    d->hostname = hostname;
}

QString QtDcmPreferences::getHostname()
{
    return d->hostname;
}

void QtDcmPreferences::setAetitle ( QString aetitle )
{
    d->aetitle = aetitle;
}

void QtDcmPreferences::setPort ( QString port )
{
    d->port = port;
}

void QtDcmPreferences::setServers ( QList<QtDcmServer *> servers )
{
    d->servers = servers;
}

QString QtDcmPreferences::getDcm2niiPath()
{
    return d->dcm2niiPath;
}

void QtDcmPreferences::setDcm2niiPath ( QString path )
{
    d->dcm2niiPath = path;
}

bool QtDcmPreferences::useDcm2nii()
{
    return d->useDcm2nii;
}

void QtDcmPreferences::useDcm2nii ( bool use )
{
    d->useDcm2nii = use;
}

