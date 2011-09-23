/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>
#include "QtDcm.h"

class QtDcmPreferencesDialogPrivate
{
public:
    QtDcmPreferences * preferences;
};

QtDcmPreferencesDialog::QtDcmPreferencesDialog ( QWidget * parent ) : d(new QtDcmPreferencesDialogPrivate)
{
    d->preferences = NULL;

    setupUi ( this );
    this->setModal ( true );
    this->setParent ( parent, Qt::Dialog );
}

QtDcmPreferencesWidget* QtDcmPreferencesDialog::getWidget()
{
    return preferencesWidget;
}

void QtDcmPreferencesDialog::setPreferences(QtDcmPreferences* prefs)
{
    d->preferences = prefs;

    dcm2niiWidget->setPreferences(d->preferences);
}

void QtDcmPreferencesDialog::updatePreferences()
{
    d->preferences->writeSettings();
}
