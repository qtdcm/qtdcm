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

#include <QtDcmWidget.h>
#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>

#include "QtDcm2.h"


QtDcm2 * QtDcm2::_instance = 0;


class QtDcm2Private
{
public:
    QtDcmWidget * widget;
};

QtDcm2::QtDcm2()
{

}

void QtDcm2::attachQtDcmWidget ( QtDcmWidget* widget )
{
    d->widget = widget;
}


QtDcm2 * QtDcm2::instance()
{
    if ( _instance == 0 )
        _instance = new QtDcm2();
    return _instance;
}


