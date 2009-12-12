/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferencesDialog.h>

QtDcmPreferencesDialog::QtDcmPreferencesDialog(QWidget * parent)
  {
    widget.setupUi(this);
    this->setModal(true);
    this->setParent(parent,Qt::Dialog);
  }

void QtDcmPreferencesDialog::initConnections()
  {

  }

void
QtDcmPreferencesDialog::setPreferences( QtDcmPreferences * prefs )
  {
    _preferences = prefs;
    widget.localAetitleEdit->setText(_preferences->getAetitle());
    widget.localPortEdit->setText(_preferences->getPort());
  }

void QtDcmPreferencesDialog::updatePreferences()
  {
    _preferences->setAetitle(widget.localAetitleEdit->text());
    _preferences->setPort(widget.localPortEdit->text());
    _preferences->writeSettings();
  }

QtDcmPreferencesDialog::~QtDcmPreferencesDialog() {}
