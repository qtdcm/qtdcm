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

#ifndef QTDCM_H
#define QTDCM_H

#include "qtdcmExports.h"
#include "ui_qtdcm.h"
#include <QtGui>

class QtDcmPrivate;
class QtDcmManager;

/**
 * This class contains the widget for managing Dicom data. It is possible to read and display in a QTreeWidget
 * the content of a DICOMDIR. It is also possible to query and retrieve data from a PACS.
 * Once read/retrieve, each serie can be reconstructed, using the QtDcmManager.
 */

class QTDCM_EXPORT QtDcm : public QWidget, public Ui::QtDcmWidget
{
    Q_OBJECT

public:
    enum eMode {
        CD_MODE = 0, 
        PACS_MODE
    };
    
    enum eGender {
        ALL_GENDER = 0,
        M_GENDER,
        F_GENDER
    };
    
    enum eModalities {
        ALL_MODALITIES = 0,
        MR_MODALITY,
        CT_MODALITY,
        PET_MODALITY
    };
    
    explicit QtDcm ( QWidget *parent = 0 ); /** constructor */
    virtual ~QtDcm();
    
    /**
     * Get the pointer of the treewidget
     *
     * @return the pointer on the QTreeWidget
     */
    QTreeView *patientsTreeView(){return treeWidgetPatients;}

    QTreeView *studiesTreeView() {return treeWidgetStudies;}

    QTreeView *seriesTreeView() {return treeWidgetSeries;}
    
    /**
     * @brief Updates the PACS list in combo box
     */
    void updatePacsComboBox();
    
    /**
     * @brief Opens the PACS preferences dialog
     */
    void editPreferences();
    
    /**
     * @brief Opens the dicomdir selectiondialog
     */
    void openDicomdir();

    
signals:
    void serieChecked ( bool checked );
    void serieSelected ( bool selected );
    
protected slots:
    void onPacsSearchButtonClicked();

    void onCurrentModalityChanged ( int index );

    void onCurrentGenderChanged ( int index );

    void onCurrentPacsChanged ( int index );

    void onStartDateChanged ( const QDate & startdate );

    void onEndDateChanged ( const QDate & enddate );

    void onPatientItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void onStudyItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void onSerieItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous );

    void onSerieItemClicked ( QTreeWidgetItem* current, int index );

    void onDicomMediaButtonClicked();

    void onPatientNameTextChanged ( const QString & pName);

    void onSerieDescriptionTextChanged ( const QString & description );

    void onStudyDescriptionTextChanged ( const QString & description );

protected:
    void loadPatientsFromDicomdir();
    void queryPACS();
    void clearDisplay();
    void initConnections();
    
private:
    QtDcmPrivate *d;
};

#endif

