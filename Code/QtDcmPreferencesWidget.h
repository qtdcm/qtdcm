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


#ifndef QTDCMPREFERENCESWIDGET_H
#define QTDCMPREFERENCESWIDGET_H

#include <QtGui>

#include "ui_qtdcmpreferenceswidget.h"


class QtDcmPreferences;
class QtDcmPreferencesWidgetPrivate;

class QtDcmPreferencesWidget : public QWidget, public Ui::PreferencesWidget
{
    Q_OBJECT

public:
    QtDcmPreferencesWidget ( QWidget * parent = 0 );

    QtDcmPreferences * getPreferences();

    void setPreferences ( QtDcmPreferences * prefs );
    void updatePreferences();

// public slots:
//     void itemSelected ( QTreeWidgetItem* current , QTreeWidgetItem* previous );
//     void serverHostnameChanged ( QString text );
//     void serverNameChanged ( QString text );
//     void serverAetitleChanged ( QString text );
//     void serverPortChanged ( QString text );
//     void removeServer();
//     void addServer();
//     void sendEcho();
/*
signals:
    void sendEchoToPacs ( int index );*/

private:
    QtDcmPreferencesWidgetPrivate * d;

//     void initConnections();
};

#endif // QTDCMPREFERENCESWIDGET_H
