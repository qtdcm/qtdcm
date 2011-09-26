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


#ifndef QTDCMDCM2NIISETTINGSWIDGET_H
#define QTDCMDCM2NIISETTINGSWIDGET_H

#include <QtGui>
#include "ui_qtdcmDcm2niiSettingsWidget.h"

class QtDcmDcm2niiSettingsWidgetPrivate;
class QtDcmPreferences;

class QtDcmDcm2niiSettingsWidget : public QWidget, public Ui::QtDcmDcm2niiSettingsWidget
{
    Q_OBJECT
public:
    QtDcmDcm2niiSettingsWidget ( QWidget * parent = 0 );

    QtDcmPreferences * getPreferences();

    void setPreferences ( QtDcmPreferences * prefs );
    void updatePreferences();

public slots:
  void browseDcm2niiPath();
  void toggleDcm2niiFrame(int state);
  

private:
    QtDcmDcm2niiSettingsWidgetPrivate * d;
};

#endif // QTDCMDCM2NIISETTINGSWIDGET_H
