/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Alexandre <email>

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


#include "QtDcmLocalDicomSettingsWidget.h"
#include <QtDcmPreferences.h>

class QtDcmLocalDicomSettingsWidgetPrivate
{
public:
    QtDcmPreferences * preferences;
};

QtDcmLocalDicomSettingsWidget::QtDcmLocalDicomSettingsWidget ( QWidget* parent ) : QWidget ( parent ), d ( new QtDcmLocalDicomSettingsWidgetPrivate )
{
    this->setupUi ( this );

    d->preferences = NULL;
}

QtDcmPreferences* QtDcmLocalDicomSettingsWidget::getPreferences()
{
    return d->preferences;
}

void QtDcmLocalDicomSettingsWidget::setPreferences ( QtDcmPreferences* prefs )
{
    d->preferences = prefs;

    localAetitleEdit->setText ( d->preferences->getAetitle() );
    localPortEdit->setText ( d->preferences->getPort() );
    localHostnameEdit->setText ( d->preferences->getHostname() );
}

void QtDcmLocalDicomSettingsWidget::updatePreferences()
{
    if ( d->preferences )
    {
        d->preferences->setAetitle ( localAetitleEdit->text() );
        d->preferences->setPort ( localPortEdit->text() );
        d->preferences->setHostname ( localHostnameEdit->text() );

        d->preferences->writeSettings();
    }
}

