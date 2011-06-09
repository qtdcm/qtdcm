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

class QtDCM : public QLabel, public Ui::QtDCM
{
    Q_OBJECT

public:
    QtDCM ( QWidget *parent = 0 ); /** Classic constructor */
    ~QtDCM();
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

    enum mode
    {
        CD, PACS
    };

    /**
     * Get the pointer of the QtDcm manager
     *
     * @return Pointer on QtDcmManager
     */
    QtDcmManager * getManager();

    /**
     * Clear the content of the QTreeWidget
     */
    void clearDisplay();

    /**
     * Initialize SIGNAL/SLOTS connections of the different widget
     *
     */
    void initConnections();

    void importToDirectory ( QString directory );

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
     * Slot that launch a QFileDialog for choosing an output directory
     * where the current serie will be exported with dcm2nii (This is done by the _manager).
     *
     */
    //        void
    //        exportList();

    void importSelectedSeries();

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

signals:
    void serieChecked ( bool checked );
    void serieSelected ( bool selected );

private:
    void updatePacsComboBox();

    QtDcmPrivate *d;
};

#endif

