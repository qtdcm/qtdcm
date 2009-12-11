/*
 * QtDcmPreferences.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferences.h>

QtDcmPreferences::QtDcmPreferences()
  {
    QDir iniDir = QDir(QDir::homePath() + QDir::separator() + ".qtdcm");
    if ( ! iniDir.exists())
      QDir::home().mkdir(".qtdcm");

    _iniFile.setFileName(iniDir.absolutePath() + QDir::separator() + "qtdcm.ini");
    if ( !_iniFile.exists())
      this->setDefaultIniFile();
    else
      this->readSettings();
  }

QtDcmPreferences::~QtDcmPreferences()
  {

  }

void QtDcmPreferences::readSettings()
  {
    QSettings settings(_iniFile.fileName(), QSettings::IniFormat);

  }

void QtDcmPreferences::setDefaultIniFile()
  {
    _aetitle = "QTDCM";
    _port = "2010";
    _encoding = "ISO_IR 100";

    QtDcmServer * server = new QtDcmServer();
    server->setAetitle("SERVER");
    server->setServer("hostname");
    server->setName("Server " + QString(_servers.size() + 1));
    server->setPort("11112");
    _servers.append(server);

    QSettings settings(_iniFile.fileName(), QSettings::IniFormat);
  }
