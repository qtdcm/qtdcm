/*
    <one line to give the library's name and an idea of what it does.>
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
