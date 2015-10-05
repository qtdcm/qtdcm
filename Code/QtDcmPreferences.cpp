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


#include <QtDcmPreferences.h>
#include <QtDcmManager.h>

class QtDcmPreferencesPrivate
{
public:
    QFile iniFile;        /** File stored on the file system that contains the preferences */
    QString aetitle;      /** Local aetitle of QtDcm */
    QString port;         /** Local port of qtdcm */
    QString hostname;     /** Local hostname of qtdcm */

    bool useDcm2nii;      /** Use dcm2nii as a conversion tool */
    QString dcm2niiPath;  /** The dcm2nii binary path */

    QList<QtDcmServer> servers; /** List of server that QtDcm can query */
};

QtDcmPreferences * QtDcmPreferences::_instance = 0;

QtDcmPreferences::QtDcmPreferences(QObject * parent) 
    : QObject(parent),
      d ( new QtDcmPreferencesPrivate )
{
}

QtDcmPreferences::~QtDcmPreferences()
{   
    delete d;
}

QtDcmPreferences* QtDcmPreferences::instance()
{
    if ( _instance == 0 ) {
        _instance = new QtDcmPreferences();
    }
    
    return _instance;
}

void QtDcmPreferences::destroy()
{
    if (_instance != 0) {
        delete _instance;
        _instance = 0;
    }
}

void QtDcmPreferences::addServer(const QtDcmServer &server)
{
    d->servers.append ( server );
}

void QtDcmPreferences::removeServer ( int index )
{
    d->servers.removeAt ( index );
}

void QtDcmPreferences::readSettings()
{
    if ( !d->iniFile.exists() ) {
        this->setDefaultIniFile();
    }
    
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
        QtDcmServer server;
        server.setAetitle ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/AETitle" ).toString() );
        server.setAddress ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/Hostname" ).toString() );
        server.setPort ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/Port" ).toString() );
        server.setName ( prefs.value ( "Server" + QString::number ( i + 1 ) + "/Name" ).toString() );
        d->servers.append ( server );
    }
    prefs.endGroup();

    emit preferencesUpdated();
}

void QtDcmPreferences::writeSettings()
{   
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
    for ( int i = 0; i < d->servers.size(); i++ ) {
        QtDcmServer server = d->servers[i];
        prefs.beginGroup ( "Server" + QString::number ( i + 1 ) );
        prefs.setValue ( "AETitle", server.aetitle() );
        prefs.setValue ( "Hostname", server.address() );
        prefs.setValue ( "Port", server.port() );
        prefs.setValue ( "Name", server.name() );
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

    QtDcmServer server;
    server.setAetitle ( "SERVER" );
    server.setAddress ( "hostname" );
    server.setName ( "Server" + QString::number ( d->servers.size() + 1 ) );
    server.setPort ( "11112" );
    d->servers.append ( server );
    this->writeSettings();
}

QString QtDcmPreferences::iniFile ( void )
{
    return d->iniFile.fileName();
}

void QtDcmPreferences::setIniFile ( const QString ini )
{
    QDir iniDir = QDir ( QDir::homePath() + QDir::separator() + ".qtdcm" );

    if ( !iniDir.exists() )
        QDir::home().mkdir ( ".qtdcm" );

    d->iniFile.setFileName ( iniDir.absolutePath() + QDir::separator() + ini );

    if ( !d->iniFile.exists() ) {
        qWarning() << "Configuration not found, using default settings";
        this->setDefaultIniFile(); //If it doesn't exist create it with default parameters
    }
    else {
        qDebug() << "Reading configuration file" << d->iniFile.fileName();
        this->readSettings();
    }

    emit preferencesUpdated();
}

QString QtDcmPreferences::aetitle() const
{
    return d->aetitle;
}

QString QtDcmPreferences::port() const
{
    return d->port;
}

QList<QtDcmServer> QtDcmPreferences::servers() const
{
    return d->servers;
}

void QtDcmPreferences::setHostname ( const QString & hostname )
{
    d->hostname = hostname;
}

QString QtDcmPreferences::hostname() const
{
    return d->hostname;
}

void QtDcmPreferences::setAetitle ( const QString & aetitle )
{
    d->aetitle = aetitle;
}

void QtDcmPreferences::setPort ( const QString & port )
{
    d->port = port;
}

void QtDcmPreferences::setServers ( const QList<QtDcmServer> & servers )
{
    d->servers = servers;
}

QString QtDcmPreferences::dcm2niiPath() const
{
    return d->dcm2niiPath;
}

void QtDcmPreferences::setDcm2niiPath ( const QString & path )
{
    d->dcm2niiPath = path;
}

bool QtDcmPreferences::useDcm2nii() const
{
    return d->useDcm2nii;
}

void QtDcmPreferences::setUseDcm2nii ( bool use )
{
    d->useDcm2nii = use;
}

