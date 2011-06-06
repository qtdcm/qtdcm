#include "QtDcm.h"
#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmServer.h>
#include <QtDcmImage.h>
#include <QtDcmDateDialog.h>
#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>
#include <QtDcmPreviewDialog.h>
#include <QtDcmManager.h>

class QtDcmPrivate {
public:
    int mode;
    QtDcmManager * manager; /** For managing preferences, data queries and volumes reconstructions */
    QList<QString> imagesList; /** Contains the images filenames of the current serie (i.e selected in the treewidget)*/
    QString currentSerieId; /** Id of the current selected serie */
    QDate beginDate, endDate; /** Begin and end for Q/R retrieve parameters */
    QMap<QString, QList<QString> > selectedSeries;
};

QtDCM::QtDCM ( QWidget *parent ) :
        QLabel ( parent ), d ( new QtDcmPrivate ) {
    QTextCodec::setCodecForCStrings ( QTextCodec::codecForName ( "iso" ) );
    setupUi ( this );
    d->mode = QtDCM::CD;

    //Initialize QTreeWidgetPatients
    treeWidgetPatients->setColumnWidth ( 0, 400 );
    treeWidgetPatients->setColumnWidth ( 1, 100 );
    treeWidgetPatients->setColumnWidth ( 2, 100 );
    QStringList labelsPatients;
    labelsPatients << "Patients name" << "ID" << "Birthdate" << "Sex";
    treeWidgetPatients->setHeaderLabels ( labelsPatients );
    treeWidgetPatients->setContextMenuPolicy ( Qt::CustomContextMenu );

    //Initialize QTreeWidgetSeries
    treeWidgetStudies->setColumnWidth ( 0, 200 );
    treeWidgetStudies->setColumnWidth ( 1, 100 );
    QStringList labelsStudies;
    labelsStudies << "Studies description" << "Date" << "ID";
    treeWidgetStudies->setHeaderLabels ( labelsStudies );
    treeWidgetStudies->setContextMenuPolicy ( Qt::CustomContextMenu );

    //Initialize QTreeWidgetSeries
    treeWidgetSeries->setColumnWidth ( 0, 230 );
    treeWidgetSeries->setColumnWidth ( 1, 100 );
    treeWidgetSeries->setColumnWidth ( 2, 100 );
    QStringList labelsSeries;
    labelsSeries << "Series description" << "Modality" << "Date" << "ID";
    treeWidgetSeries->setHeaderLabels ( labelsSeries );
    treeWidgetSeries->setContextMenuPolicy ( Qt::CustomContextMenu );

    //Initialize widgets
    startDateEdit->setDate ( QDate ( 1900, 01, 01 ) );
    endDateEdit->setDate ( QDate::currentDate() );
    //    previewGroupBox->hide();
    detailsFrame->hide();

    d->manager = new QtDcmManager ( this );
    d->manager->setPatientsTreeWidget ( treeWidgetPatients );
    d->manager->setStudiesTreeWidget ( treeWidgetStudies );
    d->manager->setSeriesTreeWidget ( treeWidgetSeries );
    d->manager->setProgressBar ( importProgressBar );
    importProgressBar->hide();

    d->manager->setDate1 ( startDateEdit->date().toString ( "yyyyMMdd" ) );
    d->manager->setDate2 ( endDateEdit->date().toString ( "yyyyMMdd" ) );

    this->updatePacsComboBox();
    initConnections();
}

QtDCM::~QtDCM() {
    delete d->manager;
}

QtDcmManager *
QtDCM::getManager() {
    return d->manager;
}

void
QtDCM::initConnections() {
    // Initialize connections
    QObject::connect ( treeWidgetPatients, SIGNAL ( currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT ( patientItemSelected ( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
    QObject::connect ( treeWidgetStudies, SIGNAL ( currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT ( studyItemSelected ( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
    QObject::connect ( treeWidgetSeries, SIGNAL ( currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT ( serieItemSelected ( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
    QObject::connect ( treeWidgetSeries, SIGNAL ( itemClicked ( QTreeWidgetItem*, int ) ), this, SLOT ( serieItemClicked ( QTreeWidgetItem*, int ) ) );
    QObject::connect ( nameEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( patientNameTextChanged ( QString ) ) );
    QObject::connect ( serieDescriptionEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serieDescriptionTextChanged ( QString ) ) );
    QObject::connect ( studyDescriptionEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( studyDescriptionTextChanged ( QString ) ) );
    QObject::connect ( searchButton, SIGNAL ( clicked() ), this, SLOT ( findSCU() ) );
    QObject::connect ( importButton, SIGNAL ( clicked() ), this, SLOT ( importSelectedSeries() ) );
    QObject::connect ( patientSexComboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( updateSex ( int ) ) );
    QObject::connect ( serieModalityComboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( updateModality ( int ) ) );
    QObject::connect ( pacsComboBox, SIGNAL ( currentIndexChanged ( int ) ), this, SLOT ( updatePACS ( int ) ) );
    QObject::connect ( startDateEdit, SIGNAL ( dateChanged ( QDate ) ), this, SLOT ( startDateChanged ( QDate ) ) );
    QObject::connect ( endDateEdit, SIGNAL ( dateChanged ( QDate ) ), this, SLOT ( endDateChanged ( QDate ) ) );
}

void
QtDCM::updatePacsComboBox() {
    pacsComboBox->clear();
    for ( int i = 0; i < d->manager->getPreferences()->getServers().size(); i++ )
        pacsComboBox->addItem ( d->manager->getPreferences()->getServers().at ( i )->getName() );
}

void
QtDCM::findSCU() {
    d->mode = QtDCM::PACS;
    treeWidgetPatients->clear();
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
    d->manager->findPatientsScu();
}

void
QtDCM::clearDisplay() {
    treeWidgetPatients->clear();
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
}

void
QtDCM::patientItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous ) {
    detailsFrame->hide();
    treeWidgetStudies->clear();
    if ( current != 0 ) { // Avoid crash when clearDisplay is called
        if ( d->mode == QtDCM::PACS )
            d->manager->findStudiesScu ( current->text ( 0 ) );
        else
            d->manager->findStudiesDicomdir ( current->text ( 0 ) );
    }
}

void QtDCM::studyItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous ) {
    treeWidgetSeries->clear();
    detailsFrame->hide();
    if ( current != 0 ) { // Avoid crash when clearDisplay is called
        if ( d->mode == QtDCM::PACS )
            d->manager->findSeriesScu ( treeWidgetPatients->currentItem()->text ( 0 ), current->text ( 0 ) );
        else
            d->manager->findSeriesDicomdir ( treeWidgetPatients->currentItem()->text ( 0 ), current->text ( 0 ) );
    }
}

void
QtDCM::serieItemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous ) {
    if ( current != 0 ) { // Avoid crash when clearDisplay is called
        if ( d->mode == QtDCM::CD )
//            d->manager->findImagesScu(current->text(3));
//        else
            d->manager->findImagesDicomdir ( current->text ( 3 ) );
        elementCountLabel->setText ( current->data ( 4, 0 ).toString() );
        institutionLabel->setText ( current->data ( 5, 0 ).toString() );
        operatorLabel->setText ( current->data ( 6, 0 ).toString() );
    }
    detailsFrame->show();
}

void
QtDCM::serieItemClicked ( QTreeWidgetItem * item, int column ) {
    if ( item->checkState ( 0 ) == Qt::Checked )
        d->manager->addSerieToImport ( item->text ( 3 ) );
    else
        d->manager->removeSerieToImport ( item->text ( 3 ) );
}

void
QtDCM::openDicomdir() {
    this->clearDisplay();
    d->mode = QtDCM::CD;
    // Open a QFileDialog for choosing a Dicomdir
    QFileDialog dialog ( this );
    dialog.setFileMode ( QFileDialog::ExistingFile );
    dialog.setDirectory ( QDir::home().dirName() );
    dialog.setWindowTitle ( tr ( "Open dicomdir" ) );
    dialog.setNameFilter ( tr ( "Dicomdir (dicomdir DICOMDIR)" ) );
    QString fileName;
    if ( dialog.exec() ) {
        fileName = dialog.selectedFiles() [0];
    }
    dialog.close();
    if ( !fileName.isEmpty() ) { // A file has been chosen
        d->manager->setDicomdir ( fileName );
        this->loadPatientsFromDicomdir();
    }
}

void
QtDCM::loadPatientsFromDicomdir() {
    this->clearDisplay();
    d->manager->loadDicomdir();
}

void
QtDCM::importSelectedSeries() {
    if ( d->manager->seriesToImportSize() != 0 ) {
        QFileDialog * dialog = new QFileDialog ( this );
        dialog->setFileMode ( QFileDialog::Directory );
        dialog->setOption ( QFileDialog::ShowDirsOnly, true );
        dialog->setDirectory ( QDir::home().dirName() );
        dialog->setWindowTitle ( tr ( "Export directory" ) );
        QString directory;
        if ( dialog->exec() ) {
            directory = dialog->selectedFiles() [0];
        }
        dialog->close();
        if ( !directory.isEmpty() ) { // A file has been chosen
            // Set the output directory to the manager and launch the conversion process
            d->manager->setOutputDirectory ( directory );
            d->manager->moveSelectedSeries();
        }
        delete dialog;
    }
}

void
QtDCM::importToDirectory ( QString directory ) {
    if ( d->manager->seriesToImportSize() != 0 ) {
        d->manager->setOutputDirectory ( directory );
        d->manager->moveSelectedSeries();
    }
}

void
QtDCM::queryPACS() {
    this->findSCU();
}

void
QtDCM::updateModality ( int index ) {
    switch ( index ) {
    case 0://MR
        d->manager->setModality ( "*" );
        break;
    case 1://CT
        d->manager->setModality ( "MR" );
        break;
    case 2://US
        d->manager->setModality ( "CT" );
        break;
    case 3://PET
        d->manager->setModality ( "PET" );
        break;
    }
    treeWidgetSeries->clear();
    if ( treeWidgetPatients->currentItem() && treeWidgetStudies->currentItem() ) {
        if ( d->mode == QtDCM::PACS )
            d->manager->findSeriesScu ( treeWidgetPatients->currentItem()->text ( 0 ), treeWidgetStudies->currentItem()->text ( 0 ) );
        else
            qDebug() << "recherche sur le cd";
    }
}

void
QtDCM::updateSex ( int index ) {
    switch ( index ) {
    case 0://all
        d->manager->setPatientSex ( "*" );
        this->queryPACS();
        break;
    case 1://M
        d->manager->setPatientSex ( "M" );
        this->queryPACS();
        break;
    case 2://F
        d->manager->setPatientSex ( "F" );
        this->queryPACS();
    }
}

void
QtDCM::updatePACS ( int index ) {
    d->manager->setCurrentPacs ( index );
    this->findSCU();
}

void
QtDCM::startDateChanged ( QDate date ) {
    if ( date > endDateEdit->date() ) {
        date = endDateEdit->date();
        startDateEdit->setDate ( date );
        return;
    }
    d->manager->setDate1 ( date.toString ( "yyyyMMdd" ) );

    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
    if ( treeWidgetPatients->currentItem() ) {
        if ( d->mode == QtDCM::PACS )
            d->manager->findStudiesScu ( treeWidgetPatients->currentItem()->text ( 0 ) );
        else
            qDebug() << "recherche sur le cd";
    }
}

void
QtDCM::endDateChanged ( QDate date ) {
    if ( date < startDateEdit->date() ) {
        date = startDateEdit->date();
        endDateEdit->setDate ( date );
        return;
    }
    d->manager->setDate2 ( date.toString ( "yyyyMMdd" ) );
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
    if ( treeWidgetPatients->currentItem() ) {
        if ( d->mode == QtDCM::PACS )
            d->manager->findStudiesScu ( treeWidgetPatients->currentItem()->text ( 0 ) );
        else
            qDebug() << "recherche sur le cd";
    }
}

void
QtDCM::editPreferences() {
    //Launch a dialog window for editing PACS settings
    QtDcmPreferencesDialog * dialog = new QtDcmPreferencesDialog ( this );
    dialog->setPreferences ( d->manager->getPreferences() );
    if ( dialog->exec() ) {
        dialog->updatePreferences();
        this->updatePacsComboBox();
    }
    dialog->close();
    delete dialog;
}

void
QtDCM::patientNameTextChanged ( QString name ) {
    if ( name.isEmpty() )
        d->manager->setPatientName ( "*" );
    else
        d->manager->setPatientName ( name + "*" );

    if ( d->mode == QtDCM::PACS )
        this->findSCU();
    //    else
    //        this->loadPatientsFromDicomdir();
}

void
QtDCM::studyDescriptionTextChanged ( QString desc ) {
    if ( desc.isEmpty() )
        d->manager->setStudyDescription ( "*" );
    else
        d->manager->setStudyDescription ( "*" + desc + "*" );
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
    if ( treeWidgetPatients->currentItem() ) {
        if ( d->mode == QtDCM::PACS )
            d->manager->findStudiesScu ( treeWidgetPatients->currentItem()->text ( 0 ) );
        //        else
        //            d->manager->findStudiesDicomdir(treeWidgetPatients->currentItem()->text(0));
    }
}

void
QtDCM::serieDescriptionTextChanged ( QString desc ) {
    if ( desc.isEmpty() ) {
        d->manager->setSerieDescription ( "*" );
    } else
        d->manager->setSerieDescription ( "*" + desc + "*" );
    treeWidgetSeries->clear();
    if ( treeWidgetPatients->currentItem() && treeWidgetStudies->currentItem() ) {
        if ( d->mode == QtDCM::PACS )
            d->manager->findSeriesScu ( treeWidgetPatients->currentItem()->text ( 0 ), treeWidgetStudies->currentItem()->text ( 0 ) );
        //        else
        //            d->manager->findSeriesDicomdir(treeWidgetPatients->currentItem()->text(0), treeWidgetStudies->currentItem()->text(0));
    }
}

void
QtDCM::showPreview() {
    QtDcmPreviewDialog * dialog = new QtDcmPreviewDialog ( this );
    if ( d->manager->getMode() == "CD" )
        d->manager->setImagesList ( d->imagesList );
    d->manager->setSerieId ( d->currentSerieId );
    d->manager->makePreview();
    dialog->setImages ( d->manager->getListImages() );
    dialog->updatePreview();
    dialog->exec();
    dialog->close();
    delete dialog;
}
// kate: indent-mode cstyle; space-indent on; indent-width 4; 
