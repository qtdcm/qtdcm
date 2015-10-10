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

#include "QtDcm.h"

#include <QFileDialog>

#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmServer.h>
#include <QtDcmImage.h>
#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>
#include <QtDcmManager.h>

class QtDcmPrivate
{

public:
    int mode;
    QList<QString> imagesList; /** Contains the images filenames of the current serie (i.e selected in the treewidget)*/
    QString currentSerieId; /** Id of the current selected serie */
    QDate beginDate, endDate; /** Begin and end for Q/R retrieve parameters */
    QMap<QString, QList<QString> > selectedSeries;
};

QtDcm::QtDcm ( QWidget *parent ) 
    : QWidget ( parent ), d ( new QtDcmPrivate )
{
    QTextCodec::setCodecForLocale( QTextCodec::codecForName ( "iso" ) );
    setupUi ( this );
    d->mode = QtDcm::CD_MODE;

    //Initialize QTreeWidgetPatients
    treeWidgetPatients->setColumnWidth ( 0, 400 );
    treeWidgetPatients->setColumnWidth ( 1, 100 );
    treeWidgetPatients->setColumnWidth ( 2, 100 );
    const QStringList labelsPatients = QStringList() 
            << "Patients name" 
            << "ID" 
            << "Birthdate" 
            << "Sex";
    
    treeWidgetPatients->setHeaderLabels ( labelsPatients );
    treeWidgetPatients->setContextMenuPolicy ( Qt::CustomContextMenu );

    //Initialize QTreeWidgetSeries
    treeWidgetStudies->setColumnWidth ( 0, 200 );
    treeWidgetStudies->setColumnWidth ( 1, 100 );
    const QStringList labelsStudies = QStringList() 
            << "Studies description" 
            << "Date" 
            << "ID";
    
    treeWidgetStudies->setHeaderLabels ( labelsStudies );
    treeWidgetStudies->setContextMenuPolicy ( Qt::CustomContextMenu );

    //Initialize QTreeWidgetSeries
    treeWidgetSeries->setColumnWidth ( 0, 230 );
    treeWidgetSeries->setColumnWidth ( 1, 100 );
    treeWidgetSeries->setColumnWidth ( 2, 100 );
    const QStringList labelsSeries = QStringList()
            << "Series description" 
            << "Modality" 
            << "Date" 
            << "ID";
    
    treeWidgetSeries->setHeaderLabels ( labelsSeries );
    treeWidgetSeries->setContextMenuPolicy ( Qt::CustomContextMenu );

    //Initialize widgets
    QDate currentDate = QDate::currentDate();
    startDateEdit->setDate (  currentDate.addYears(-1) );
    endDateEdit->setDate ( currentDate );

    QtDcmManager::instance()->setPatientsTreeWidget ( treeWidgetPatients );
    QtDcmManager::instance()->setStudiesTreeWidget ( treeWidgetStudies );
    QtDcmManager::instance()->setSeriesTreeWidget ( treeWidgetSeries );
    QtDcmManager::instance()->setStartDate ( startDateEdit->date() );
    QtDcmManager::instance()->setEndDate ( endDateEdit->date() );

    initConnections();
}

QtDcm::~QtDcm()
{
  delete d;
  d = NULL;
}

void QtDcm::initConnections()
{
    // Initialize connections
    connect ( treeWidgetPatients,    &QTreeWidget::currentItemChanged, 
              this,                  &QtDcm::onPatientItemSelected);
    connect ( treeWidgetStudies,     &QTreeWidget::currentItemChanged, 
              this,                  &QtDcm::onStudyItemSelected);
    connect ( treeWidgetSeries,      &QTreeWidget::currentItemChanged, 
              this,                  &QtDcm::onSerieItemSelected);
    connect ( treeWidgetSeries,      &QTreeWidget::itemClicked, 
              this,                  &QtDcm::onSerieItemClicked);
    connect ( nameEdit,              &QLineEdit::textChanged, 
              this,                  &QtDcm::onPatientNameTextChanged);
    connect ( serieDescriptionEdit,  &QLineEdit::textChanged, 
              this,                  &QtDcm::onSerieDescriptionTextChanged);
    connect ( studyDescriptionEdit,  &QLineEdit::textChanged, 
              this,                  &QtDcm::onStudyDescriptionTextChanged);
    connect ( searchButton,          &QPushButton::clicked, 
              this,                  &QtDcm::onPacsSearchButtonClicked);
    connect ( cdromButton,           &QPushButton::clicked, 
              this,                  &QtDcm::onDicomMediaButtonClicked);
    connect ( patientSexComboBox,    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), 
              this,                  &QtDcm::onCurrentGenderChanged);
    connect ( serieModalityComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), 
              this,                  &QtDcm::onCurrentModalityChanged);
    connect ( pacsComboBox,          static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), 
              this,                  &QtDcm::onCurrentPacsChanged);
    connect ( startDateEdit,         &QDateEdit::dateChanged, 
              this,                  &QtDcm::onStartDateChanged);
    connect ( endDateEdit,           &QDateEdit::dateChanged, 
              this,                  &QtDcm::onEndDateChanged);
}

void QtDcm::updatePacsComboBox()
{
    pacsComboBox->blockSignals ( true );
    pacsComboBox->clear();

    for ( int i = 0; i < QtDcmPreferences::instance()->servers().size(); i++ ) {
        pacsComboBox->addItem ( QtDcmPreferences::instance()->servers().at ( i ).name() );
    }
    
    pacsComboBox->blockSignals ( false );
}

void QtDcm::onPacsSearchButtonClicked()
{
    d->mode = QtDcm::PACS_MODE;
    treeWidgetPatients->clear();
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
    QtDcmManager::instance()->setCurrentPacs ( pacsComboBox->currentIndex() );
    QtDcmManager::instance()->findPatientsScu();
}

void QtDcm::clearDisplay()
{
    treeWidgetPatients->clear();
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
}

void QtDcm::onPatientItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    if (!current) {
        return;
    }
    
    QtDcmManager::instance()->clearSerieInfo();
    QtDcmManager::instance()->clearPreview();

    treeWidgetStudies->clear();

    if ( d->mode == QtDcm::PACS_MODE ) {
        QtDcmManager::instance()->findStudiesScu ( current->text ( 0 ) );
    }
    else {
        QtDcmManager::instance()->findStudiesDicomdir ( current->text ( 0 ) );
    }
    
    QtDcmManager::instance()->clearPreview();
}

void QtDcm::onStudyItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    if (!current) {
        return;
    }
    
    treeWidgetSeries->clear();

    QtDcmManager::instance()->clearSerieInfo();
    QtDcmManager::instance()->clearPreview();

    if ( d->mode == QtDcm::PACS_MODE ) {
        QtDcmManager::instance()->findSeriesScu ( treeWidgetPatients->currentItem()->text ( 0 ), current->data ( 2, 0 ).toString() );
    }
    else {
        QtDcmManager::instance()->findSeriesDicomdir ( treeWidgetPatients->currentItem()->text ( 0 ), current->data ( 2, 0 ).toString() );
    }
    
    QtDcmManager::instance()->clearPreview();
}

void QtDcm::onSerieItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    if (!current)  {
        return;
    }
        
    QtDcmManager::instance()->clearListOfImages();
    
    if ( d->mode == QtDcm::CD_MODE )
        QtDcmManager::instance()->findImagesDicomdir ( current->text ( 3 ) );
    else
        QtDcmManager::instance()->findImagesScu ( current->text ( 3 ) );
    
    int elementCount = QtDcmManager::instance()->listOfImages().size();
    QString institution = current->data ( 5, 0 ).toString();
    QString opName = current->data ( 6, 0 ).toString();
    
    QtDcmManager::instance()->updateSerieInfo ( QString::number ( elementCount ), institution, opName );
//         QtDcmManager::instance()->clearPreview();
    QtDcmManager::instance()->getPreviewFromSelectedSerie ( current->text ( 3 ), elementCount / 2 );
}

void QtDcm::onSerieItemClicked ( QTreeWidgetItem * item, int column )
{
    if ( item->checkState ( 0 ) == Qt::Checked ) {
        QtDcmManager::instance()->addSerieToImport ( item->text ( 3 ) );
    }
    else {
        QtDcmManager::instance()->removeSerieToImport ( item->text ( 3 ) );
    }
}

void QtDcm::onDicomMediaButtonClicked()
{
    this->openDicomdir();
}

void QtDcm::loadPatientsFromDicomdir()
{
    this->clearDisplay();
    QtDcmManager::instance()->loadDicomdir();
}

void QtDcm::queryPACS()
{
    this->onPacsSearchButtonClicked();
}

void QtDcm::onCurrentModalityChanged ( int index )
{
    switch ( index )
    {
    case ALL_MODALITIES://*
        QtDcmManager::instance()->setModality ( "*" );
        break;

    case MR_MODALITY:
        QtDcmManager::instance()->setModality ( "MR" );
        break;

    case CT_MODALITY:
        QtDcmManager::instance()->setModality ( "CT" );
        break;

    case PET_MODALITY:
        QtDcmManager::instance()->setModality ( "PET" );
        break;
    default:
        qWarning() << "Modality not supported: " << serieModalityComboBox->currentText();
        break;
    }

    treeWidgetSeries->clear();

    if ( treeWidgetPatients->currentItem() && treeWidgetStudies->currentItem() ) {
        if ( d->mode == QtDcm::PACS_MODE ) {
            QtDcmManager::instance()->findSeriesScu ( treeWidgetPatients->currentItem()->text ( 0 ), treeWidgetStudies->currentItem()->data ( 2, 0 ).toString() );
        }
        else {
            QtDcmManager::instance()->findSeriesDicomdir ( treeWidgetPatients->currentItem()->text ( 0 ), treeWidgetStudies->currentItem()->data ( 2, 0 ).toString() );
        }
    }
}

void QtDcm::onCurrentGenderChanged ( int index )
{
    switch ( index )
    {
    case ALL_GENDER:
        QtDcmManager::instance()->setPatientGender( "*" );
        this->queryPACS();
        break;

    case M_GENDER:
        QtDcmManager::instance()->setPatientGender( "M" );
        this->queryPACS();
        break;

    case F_GENDER:
        QtDcmManager::instance()->setPatientGender( "F" );
        this->queryPACS();
        break;
    default:
        qWarning() << "Gender not supported: " << patientSexComboBox->currentText();
        break;
    }
}

void QtDcm::onCurrentPacsChanged ( int index )
{
    QtDcmManager::instance()->setCurrentPacs ( index );
    this->onPacsSearchButtonClicked();
}

void QtDcm::onStartDateChanged(const QDate &startdate)
{   
    if ( startdate > endDateEdit->date() ) {
        startDateEdit->blockSignals(true);
        startDateEdit->setDate ( endDateEdit->date() );
        startDateEdit->blockSignals(false);
    }

    QtDcmManager::instance()->setStartDate ( startDateEdit->date() );

    treeWidgetStudies->clear();
    treeWidgetSeries->clear();

    if ( treeWidgetPatients->currentItem() ) {
        if ( d->mode == QtDcm::PACS_MODE ) {
            QtDcmManager::instance()->findStudiesScu ( treeWidgetPatients->currentItem()->text ( 0 ) );
        }
        else {
            qDebug() << "Date filtering not available in CD-Rom mode";
        }
    }
}

void QtDcm::onEndDateChanged (const QDate &enddate)
{   
    if ( enddate < startDateEdit->date() ) {
        endDateEdit->blockSignals(true);
        endDateEdit->setDate(startDateEdit->date() );
        endDateEdit->blockSignals(false);
    }

    QtDcmManager::instance()->setEndDate ( endDateEdit->date() );

    treeWidgetStudies->clear();
    treeWidgetSeries->clear();

    if ( treeWidgetPatients->currentItem() ) {
        if ( d->mode == QtDcm::PACS_MODE ) {
            QtDcmManager::instance()->findStudiesScu ( treeWidgetPatients->currentItem()->text ( 0 ) );
        }
        else {
            qDebug() << "Date filtering not available in CD-Rom mode";
        }
    }
}

void QtDcm::editPreferences()
{
    //Launch a dialog window for editing PACS settings
    QtDcmPreferencesDialog dialog( this );
    dialog.readPreferences();
    if ( dialog.exec() ) {
        dialog.updatePreferences();
    }
}

void QtDcm::openDicomdir()
{
    this->clearDisplay();
    d->mode = QtDcm::CD_MODE;
    // Open a QFileDialog for choosing a Dicomdir
    QFileDialog dialog(this);
    dialog.setWindowTitle ( tr ( "Open dicomdir" ) );
    dialog.setNameFilters(QStringList() << "Dicomdir files (dicomdir* DICOMDIR*)");
    
    // Trying to open directly on one of the available drives
    if (!QDir::drives().isEmpty()) {
        dialog.setDirectory ( QDir::drives().first().absoluteDir() );
    }
    
    QString fileName;
    if ( dialog.exec() ) {
        fileName = dialog.selectedFiles() [0];
    }

    if ( !fileName.isEmpty() ) {  // A file has been chosen{
        if (QString::compare(fileName, "dicomdir", Qt::CaseInsensitive)) {
            QtDcmManager::instance()->setDicomdir ( fileName );
            this->loadPatientsFromDicomdir();
        }
    }
}

void QtDcm::onPatientNameTextChanged (const QString &pName)
{
    if (pName.isEmpty() ) {
        QtDcmManager::instance()->setPatientName ( "*" );
    }
    else {
        QtDcmManager::instance()->setPatientName ( pName + "*" );
    }

    if ( d->mode == QtDcm::PACS_MODE ) {
        this->onPacsSearchButtonClicked();
    }
}

void QtDcm::onStudyDescriptionTextChanged (const QString &description)
{
    if ( description.isEmpty() ) {
        QtDcmManager::instance()->setStudyDescription ( "*" );
    }
    else {
        QtDcmManager::instance()->setStudyDescription ( "*" + description + "*" );
    }

    if ( d->mode == QtDcm::PACS_MODE ) {
        treeWidgetStudies->clear();
        treeWidgetSeries->clear();
    }
    
    if ( treeWidgetPatients->currentItem() && d->mode == QtDcm::PACS_MODE ) {
        QtDcmManager::instance()->findStudiesScu ( treeWidgetPatients->currentItem()->text ( 0 ) );
    }
}

void QtDcm::onSerieDescriptionTextChanged (const QString &description)
{
    if ( description.isEmpty() ) {
        QtDcmManager::instance()->setSerieDescription ( "*" );
    }
    else {
        QtDcmManager::instance()->setSerieDescription ( "*" + description + "*" );
    }

    if ( d->mode == QtDcm::PACS_MODE ) {
        treeWidgetSeries->clear();
    }

    if ( treeWidgetPatients->currentItem() && treeWidgetStudies->currentItem() && d->mode == QtDcm::PACS_MODE) {
        QtDcmManager::instance()->findSeriesScu ( treeWidgetPatients->currentItem()->text ( 0 ), treeWidgetStudies->currentItem()->data ( 2, 0 ).toString() );
    }
}
