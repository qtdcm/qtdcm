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


#ifndef QTDCMWIDGET_H
#define QTDCMWIDGET_H

#include <QtGui>
#include "ui_qtdcmwidget.h"

class QtDcmWidgetPrivate;

class QtDcmWidget : public QWidget, public Ui::QtDcmWidget
{
    Q_OBJECT

public:
    QtDcmWidget ( QWidget * parent = 0 );

    enum mode
    {
        CD, PACS
    };

    /**
     * Get the pointer of the treewidget
     *
     * @return the pointer on the QTreeWidget
     */
    QTreeView * getPatientsTreeView()
    {
        return treeWidgetPatients;
    }

    QTreeView * getStudiesTreeView()
    {
        return treeWidgetStudies;
    }

    QTreeView * getSeriesTreeView()
    {
        return treeWidgetSeries;
    }

    QLabel * getPreviewLabel()
    {
        return imageLabel;
    }

public slots:
//     void patientItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );
// 
//     void studyItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );
// 
//     void serieItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );
// 
//     void serieItemClicked ( QTreeWidgetItem* current, int );
// 
//     /**
//      * Slot called when user modify patient name edit line
//      */
//     void patientNameTextChanged ( QString );
// 
//     /**
//      * Slot called when user modify serie description edit line
//      */
//     void serieDescriptionTextChanged ( QString );
// 
//     /**
//      * Slot called when user modify study description edit line
//      */
//     void studyDescriptionTextChanged ( QString );
// 
//     /**
//      * Slot that change the modality parameter of the query
//      *
//      */
//     void modalityChanged ( int index );
// 
//     /**
//      * Slot called when user modify sex combo box
//      */
//     void sexChanged ( int index );
// 
//     /**
//      * Slot called when user modify sex combo box
//      */
//     void pacsChanged ( int index );
// 
//     /**
//      * Slot that launch the Date selection dialog. This change the begin date (or current date) for the Dicom query
//      *
//      */
//     void startDateChanged ( QDate date );
// 
//     /**
//      * Slot that launch the Date selection dialog. This change the end date for the Dicom query (Use only in range mode)
//      *
//      */
//     void endDateChanged ( QDate date );


signals:
    void patientsQuery ( QMap<QString,QString> map );
    void studiesQuery ( QMap<QString,QString> map );
    void seriesQuery ( QMap<QString,QString> map );
    void imageQuery ( QMap<QString, QString> map );

private:
    QtDcmWidgetPrivate * d;

    /**
     * Initialize SIGNAL/SLOTS connections of the different widget
     *
     */
    void initConnections();
};

#endif // QTDCMWIDGET_H
