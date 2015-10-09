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


#ifndef QTDCMSERIEINFOWIDGET_H
#define QTDCMSERIEINFOWIDGET_H

#include "qtdcmExports.h"
#include <QtGui>
#include "ui_qtdcmSerieInfoWidget.h"

class QTDCM_EXPORT QtDcmSerieInfoWidget : public QWidget, public Ui::QtDcmSerieInfoWidget
{
    Q_OBJECT
public:
    explicit QtDcmSerieInfoWidget ( QWidget * parent = 0 );
    virtual ~QtDcmSerieInfoWidget() {}
};

#endif // QTDCMSERIEINFOWIDGET_H
