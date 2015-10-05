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


#ifndef QTDCMDCM2NIISETTINGSWIDGET_H
#define QTDCMDCM2NIISETTINGSWIDGET_H

#include <QtGui>
#include "ui_qtdcmDcm2niiSettingsWidget.h"

class QtDcmPreferences;

class QtDcmDcm2niiSettingsWidget : public QWidget, public Ui::QtDcmDcm2niiSettingsWidget
{
    Q_OBJECT
public:
    explicit QtDcmDcm2niiSettingsWidget ( QWidget * parent = 0 );
    virtual ~QtDcmDcm2niiSettingsWidget();
    
    QtDcmPreferences * getPreferences();

    void readPreferences();
    void updatePreferences();
    
public slots:
    void onBrowseDcm2niiPathClicked();
    void onUseDcm2niiCheckChanged(int state);
  
private:
    class Private;
    Private * d;
};

#endif // QTDCMDCM2NIISETTINGSWIDGET_H
