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


#ifndef QTDCMPREVIEWWIDGET_H
#define QTDCMPREVIEWWIDGET_H

#include <QtGui>
#include "ui_qtdcmPreviewWidget.h"

class QtDcmPreviewWidgetPrivate;

class QtDcmPreviewWidget : public QWidget, public Ui::QtDcmPreviewWidget
{
     Q_OBJECT
public:
    QtDcmPreviewWidget ( QWidget * parent = 0 );

private:
    QtDcmPreviewWidgetPrivate * d;
};

#endif // QTDCMPREVIEWWIDGET_H
