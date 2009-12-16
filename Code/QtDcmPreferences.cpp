/*
 * QtDcmPreferences.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferences.h>

QtDcmPreferences::QtDcmPreferences()
  {
    // Check if the .qtdcm directory exists and if not, create it in the home directory
    QDir iniDir = QDir(QDir::homePath() + QDir::separator() + ".qtdcm");
    if (!iniDir.exists())
      QDir::home().mkdir(".qtdcm");

    //Check for the ini settings file
    _iniFile.setFileName(iniDir.absolutePath() + QDir::separator() + "qtdcm.ini");
    if (!_iniFile.exists())
        this->setDefaultIniFile(); //If it doesn't exist create it with default parameters
    else
      this->readSettings(); // else load the parameters
  }

void
QtDcmPreferences::addServer()
  {
    _servers.append(new QtDcmServer);
  }

void
QtDcmPreferences::removeServer( int index)
  {
    _servers.removeAt(index);
  }

void
QtDcmPreferences::readSettings()
  {
    //Instantiate a QSettings object from the ini file.
    QSettings prefs(_iniFile.fileName(), QSettings::IniFormat);
    //Load local settings
    prefs.beginGroup("LocalSettings");
    _dcm2niiPath = prefs.value("Dcm2nii").toString();
    _dcm4chePath = prefs.value("Dcm4che").toString();
    _aetitle = prefs.value("AETitle").toString();
    _port = prefs.value("Port").toString();
    _encoding = prefs.value("Encoding").toString();
    prefs.endGroup();
    //For each server load corresponding settings
    prefs.beginGroup("Servers");
    for (int i = 0; i < prefs.childGroups().size(); i++)
      {
        _servers.append(new QtDcmServer);
        _servers.at(i)->setAetitle(prefs.value("Server" + QString::number(i + 1) + "/AETitle").toString());
        _servers.at(i)->setServer(prefs.value("Server" + QString::number(i + 1) + "/Hostname").toString());
        _servers.at(i)->setPort(prefs.value("Server" + QString::number(i + 1) + "/Port").toString());
        _servers.at(i)->setName(prefs.value("Server" + QString::number(i + 1) + "/Name").toString());
      }
    prefs.endGroup();
  }

void
QtDcmPreferences::writeSettings()
  {
    //Remove settings file
    QFile ini(_iniFile.fileName());
    ini.remove();
    //Instantiate a QSettings object with the ini file.
    QSettings prefs(_iniFile.fileName(), QSettings::IniFormat);
    //Write local settings from the private attributes
    prefs.beginGroup("LocalSettings");
    prefs.setValue("Dcm2nii", _dcm2niiPath);
    prefs.setValue("Dcm4che", _dcm4chePath);
    prefs.setValue("AETitle", _aetitle);
    prefs.setValue("Port", _port);
    prefs.setValue("Encoding", _encoding);
    prefs.endGroup();
    //Do the job for each server
    prefs.beginGroup("Servers");
    for (int i = 0; i < _servers.size(); i++)
      {
        prefs.beginGroup("Server" + QString::number(i + 1));
        prefs.setValue("AETitle", _servers.at(i)->getAetitle());
        prefs.setValue("Hostname", _servers.at(i)->getServer());
        prefs.setValue("Port", _servers.at(i)->getPort());
        prefs.setValue("Name", _servers.at(i)->getName());
        prefs.endGroup();
      }
    prefs.endGroup();
  }

void
QtDcmPreferences::setDefaultIniFile()
  {
    //Create an ini file with default parameters
    _dcm2niiPath = "";
    _dcm4chePath = "";
    _aetitle = "QTDCM";
    _port = "2010";
    _encoding = "ISO_IR 100";

    QtDcmServer * server = new QtDcmServer();
    server->setAetitle("SERVER");
    server->setServer("hostname");
    server->setName("Server" + QString::number(_servers.size() + 1));
    server->setPort("11112");
    _servers.append(server);
    this->writeSettings();
  }
