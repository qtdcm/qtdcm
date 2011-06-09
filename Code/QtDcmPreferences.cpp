/*
 * QtDcmPreferences.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmServer.h>
#include <QtDcmPreferences.h>

class QtDcmPreferencesPrivate
{

public:
    QFile iniFile; /** File stored on the file system that contains the preferences */
    QString aetitle; /** Local aetitle of QtDcm */
    QString port; /** Local port of qtdcm */
    QString hostname;

    QList<QtDcmServer *> servers; /** List of server that QtDcm can query */
};

QtDcmPreferences::QtDcmPreferences() : d ( new QtDcmPreferencesPrivate )
{
    // Check if the .qtdcm directory exists and if not, create it in the home directory
    QDir iniDir = QDir ( QDir::homePath() + QDir::separator() + ".qtdcm" );

    if ( !iniDir.exists() )
        QDir::home().mkdir ( ".qtdcm" );

    //Check for the ini settings file
    d->iniFile.setFileName ( iniDir.absolutePath() + QDir::separator() + "qtdcm.ini" );

    if ( !d->iniFile.exists() )
        this->setDefaultIniFile(); //If it doesn't exist create it with default parameters
    else
        this->readSettings(); // else load the parameters
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
    //Instantiate a QSettings object from the ini file.
    QSettings prefs ( d->iniFile.fileName(), QSettings::IniFormat );
    //Load local settings
    prefs.beginGroup ( "LocalSettings" );
    d->aetitle = prefs.value ( "AETitle" ).toString();
    d->port = prefs.value ( "Port" ).toString();
    d->hostname = prefs.value ( "Hostname" ).toString();
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
}

void QtDcmPreferences::setDefaultIniFile()
{
    //Create an ini file with default parameters
    d->aetitle = "QTDCM";
    d->port = "2010";
    d->hostname = "localhost";

    QtDcmServer * server = new QtDcmServer();
    server->setAetitle ( "SERVER" );
    server->setServer ( "hostname" );
    server->setName ( "Server" + QString::number ( d->servers.size() + 1 ) );
    server->setPort ( "11112" );
    d->servers.append ( server );
    this->writeSettings();
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
