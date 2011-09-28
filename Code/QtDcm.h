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

#ifndef QTDCM_H
#define QTDCM_H

#include "ui_qtdcm.h"
#include <QtGui>

class QtDcmPrivate;

class QtDcmManager;

/**
 * This class contains the widget for managing Dicom data. It is possible to read and display in a QTreeWidget
 * the content of a DICOMDIR. It is also possible to query and retrieve data from a PACS.
 * Once read/retrieve, each serie can be reconstructed, using the QtDcmManager.
 */

class QtDcm : public QWidget, public Ui::QtDcmWidget
{
    Q_OBJECT

public:
    QtDcm ( QWidget *parent = 0 ); /** Classic constructor */
    ~QtDcm();
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

    enum mode
    {
        CD, PACS
    };

    /**
     * Clear the content of the QTreeWidget
     */
    void clearDisplay();

    /**
     * Initialize SIGNAL/SLOTS connections of the different widget
     *
     */
    void initConnections();

public slots:
    void findSCU();

    /**
     * Slot that change the modality parameter of the query
     *
     */
    void updateModality ( int index );

    void updateSex ( int index );

    void updatePACS ( int index );

    /**
     * Slot that launch the Date selection dialog. This change the begin date (or current date) for the Dicom query
     *
     */
    void startDateChanged ( QDate date );

    /**
     * Slot that launch the Date selection dialog. This change the end date for the Dicom query (Use only in range mode)
     *
     */
    void endDateChanged ( QDate date );

    void patientItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void studyItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void serieItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void serieItemClicked ( QTreeWidgetItem* current, int );

    /**
     * Slot that query the list of server with the user specified parameters
     */
    void queryPACS();

    /**
     * Slot that launch a QFileDialog for choosing a dicomdir
     *
     *
     */
    void openDicomdir();

    void loadPatientsFromDicomdir();

    /**
     * Slot that launch the PACS preferences dialog
     *
     */
    void editPreferences();

    /**
     * Slot called when user modify patient name edit line
     */
    void patientNameTextChanged ( QString );

    /**
     * Slot called when user modify serie description edit line
     */
    void serieDescriptionTextChanged ( QString );

    /**
     * Slot called when user modify study description edit line
     */
    void studyDescriptionTextChanged ( QString );

    /**
     * Update the PACS combo box when the user modify the preferences
     */
    void updatePacsComboBox();

signals:
    void serieChecked ( bool checked );
    void serieSelected ( bool selected );

private:
    QtDcmPrivate *d;
};

#endif

