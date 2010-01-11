#ifndef QTDCM_H
#define QTDCM_H

#include <QtGui>

#include "ui_qtdcm.h"
#include <QtDcmDateDialog.h>
#include <QtDcmPreferencesDialog.h>
#include <QtDcmPreviewDialog.h>
#include <QtDcmManager.h>

/**
 * This class contains the widget for managing Dicom data. It is possible to read and display in a QTreeWidget
 * the content of a DICOMDIR. It is also possible to query and retrieve data from a PACS.
 * Once read/retrieve, each serie can be reconstructed, using the QtDcmManager.
 */
class QtDCM : public QLabel
  {
  Q_OBJECT
  private:
    QtDcmManager * _manager; /** For managing preferences, data queries and volumes reconstructions */
    QList<QString> _imagesList; /** Contains the images filenames of the current serie (i.e selected in the treewidget)*/
    QString _currentSerieId; /** Id of the current selected serie */
    QDate _beginDate, _endDate; /** Begin and end for Q/R retrieve parameters */
    QMap<QString, QList<QString> > _selectedSeries;

  public:
    Ui::QtDCM widget; /** Global widget generating by Designer*/
    QtDCM( QWidget *parent = 0 ); /** Classic constructor */
    ~QtDCM();
    /**
     * Get the pointer of the treewidget
     *
     * @return the pointer on the QTreeWidget
     */
    QTreeView *
    getTreeView()
      {
        return widget.treeWidget;
      }

    /**
     * Get the pointer of the QtDcm manager
     *
     * @return Pointer on QtDcmManager
     */
    QtDcmManager *
    getManager()
      {
        return _manager;
      }

    /**
     * Clear the content of the QTreeWidget
     */
    void
    clearDisplay();

    /**
     * Display in the QTreeWidget the content of the list of patients from the manager
     *
     */
    void
    display();

    /**
     * Initialize SIGNAL/SLOTS connections of the different widget
     *
     */
    void
    initConnections();

    /**
     * Get the images filenames list of the current serie
     *
     */
    QList<QString>
    getImagesList();

    void
    exportToDirectory( QString directory );

  public slots:

    /**
     * Slot called when dateComboBox is changed. This slots change the display parameters of the 2 date buttons
     *
     */
    void
    updateDateButtons( int index );

    /**
     * Slot that change the modality parameter of the query
     *
     */
    void
    updateModality( int index );

    /**
     * Slot that launch the Date selection dialog. This change the begin date (or current date) for the Dicom query
     *
     */
    void
    chooseBeginDate();

    /**
     * Slot that launch the Date selection dialog. This change the end date for the Dicom query (Use only in range mode)
     *
     */
    void
    chooseEndDate();

    /**
     * Slot called when a selected is made on the QTreeWidget. If a serie is selected, corresponding images filenames
     * are copied in _imagesList
     *
     */
    void
    itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous );

    void
    itemClicked( QTreeWidgetItem* current , int );

    /**
     * Slot called when right clicking on the QTreeWidget. User can export or open a dicomdir from the context menu
     *
     */
    void
    contextExportMenu( const QPoint point );

    /**
     * Slot that launch a QFileDialog for choosing an output directory
     * where the current serie will be exported with dcm2nii (This is done by the _manager).
     *
     */
    void
    exportList();

    /**
     * Slot that query the list of server with the user specified parameters
     */
    void
    queryPACS();

    /**
     * Slot that launch a QFileDialog for choosing a dicomdir
     *
     *
     */
    void
    openDicomdir();

    /**
     * Slot that launch the PACS preferences dialog
     *
     */
    void
    editPreferences();

    /**
     * Slot called when user modify patient name edit line
     */
    void
    patientNameTextChanged( QString );

    /**
     * Slot called when user modify patient id edit line
     */
    void
    patientIdTextChanged( QString );

    /**
     * Slot called when user modify serie description edit line
     */
    void
    serieDescriptionTextChanged( QString );

    /**
     * Slot called when user modify study description edit line
     */
    void
    studyDescriptionTextChanged( QString );

    /**
     * Slot called when advanced button is clicked
     */
    void
    toggleAdvancedView();

    void
    showPreview();
  };

#endif

