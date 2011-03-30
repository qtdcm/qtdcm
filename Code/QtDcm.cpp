#include "QtDcm.h"
#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmImage.h>
#include <QtDcmDateDialog.h>
#include <QtDcmPreferencesDialog.h>
#include <QtDcmPreviewDialog.h>
#include <QtDcmManager.h>

class QtDcmPrivate
{
    public:
        QtDcmManager * manager; /** For managing preferences, data queries and volumes reconstructions */
        QList<QString> imagesList; /** Contains the images filenames of the current serie (i.e selected in the treewidget)*/
        QString currentSerieId; /** Id of the current selected serie */
        QDate beginDate, endDate; /** Begin and end for Q/R retrieve parameters */
        QMap<QString, QList<QString> > selectedSeries;
};

QtDCM::QtDCM(QWidget *parent) :
    QLabel(parent), d(new QtDcmPrivate)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("iso"));
    setupUi(this);
    //Initialize QTreeWidgetPatients
    treeWidgetPatients->setColumnWidth(0, 400);
    treeWidgetPatients->setColumnWidth(1, 100);
    treeWidgetPatients->setColumnWidth(2, 100);
    QStringList labelsPatients;
    labelsPatients << "Patients name" << "ID" << "Birthdate" << "Sex";
    treeWidgetPatients->setHeaderLabels(labelsPatients);
    treeWidgetPatients->setContextMenuPolicy(Qt::CustomContextMenu);

    //Initialize QTreeWidgetSeries
    treeWidgetStudies->setColumnWidth(0, 200);
    treeWidgetStudies->setColumnWidth(1, 100);
    QStringList labelsStudies;
    labelsStudies << "Studies description" << "Date" << "ID";
    treeWidgetStudies->setHeaderLabels(labelsStudies);
    treeWidgetStudies->setContextMenuPolicy(Qt::CustomContextMenu);

    //Initialize QTreeWidgetSeries
    treeWidgetSeries->setColumnWidth(0, 230);
    treeWidgetSeries->setColumnWidth(1, 100);
    treeWidgetSeries->setColumnWidth(2, 100);
    QStringList labelsSeries;
    labelsSeries << "Series description" << "Modality" << "Date" << "ID";
    treeWidgetSeries->setHeaderLabels(labelsSeries);
    treeWidgetSeries->setContextMenuPolicy(Qt::CustomContextMenu);

    //Initialize widgets
    startDateEdit->setDate(QDate(1900,01,01));
    endDateEdit->setDate(QDate::currentDate());
//    previewGroupBox->hide();
    detailsFrame->hide();

    d->manager = new QtDcmManager(this);
    d->manager->setPatientsTreeWidget(treeWidgetPatients);
    d->manager->setStudiesTreeWidget(treeWidgetStudies);
    d->manager->setSeriesTreeWidget(treeWidgetSeries);
    initConnections();
}

QtDCM::~QtDCM()
{
    delete d->manager;
}

QtDcmManager *
QtDCM::getManager()
{
    return d->manager;
}

void
QtDCM::initConnections()
{
    // Initialize connections
    QObject::connect(treeWidgetPatients, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(patientItemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(treeWidgetStudies, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(studyItemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(treeWidgetSeries, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(serieItemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(patientNameTextChanged(QString)));
    QObject::connect(serieDescriptionEdit, SIGNAL(textChanged(QString)), this, SLOT(serieDescriptionTextChanged(QString)));
    QObject::connect(studyDescriptionEdit, SIGNAL(textChanged(QString)), this, SLOT(studyDescriptionTextChanged(QString)));
    QObject::connect(echoButton, SIGNAL(clicked()), this, SLOT(sendEcho()));
    QObject::connect(searchButton, SIGNAL(clicked()), this, SLOT(findSCU()));
}

void
QtDCM::sendEcho()
{
    d->manager->sendEchoRequest();
}

void
QtDCM::findSCU()
{
    treeWidgetPatients->clear();
    treeWidgetStudies->clear();
    treeWidgetSeries->clear();
    d->manager->findPatientScu();
}

void
QtDCM::clearDisplay()
{
//    d->selectedSeries.clear();
//    d->imagesList.clear();
//    treeWidget->clear();
//    treeWidget->setAnimated(false);
//    emit serieSelected(false);
}

void
QtDCM::display()
{
//    //Clear view
//    this->clearDisplay();
//    for (int i = 0; i < d->manager->getPatients().size(); i++) {
//        if (d->manager->getPatients().at(i)->getStudies().size() != 0) {
//            QTreeWidgetItem * child = new QTreeWidgetItem(treeWidget);
//            QTreeWidgetItem * root = NULL;
//            // Fill in the QTreeWidget the patient information
//            child->setText(0, d->manager->getPatients().at(i)->getName());
//            child->setData(1, 1, QVariant(d->manager->getPatients().at(i)->getName()));
//
//            child->setText(1, "Patient");
//            child->setData(2, 1, QVariant("PATIENT"));
//
//            child->setText(3, d->manager->getPatients().at(i)->getId());
//            child->setData(4, 1, QVariant(d->manager->getPatients().at(i)->getId()));
//
//            root = child;
//            //        if (d->manager->getMode() == "CD")
//            treeWidget->expandItem(child);
//            for (int j = 0; j < d->manager->getPatients().at(i)->getStudies().size(); j++) {
//                // Study information
//                child = new QTreeWidgetItem(root);
//                child->setText(0, d->manager->getPatients().at(i)->getStudies().at(j)->getDescription());
//                child->setData(1, 1, QVariant(d->manager->getPatients().at(i)->getStudies().at(j)->getDescription()));
//
//                child->setText(1, "Study");
//                child->setData(2, 1, QVariant("STUDY"));
//
//                child->setText(2, d->manager->getPatients().at(i)->getStudies().at(j)->getDate().toString(Qt::ISODate));
//                child->setData(3, 1, QVariant(d->manager->getPatients().at(i)->getStudies().at(j)->getDate().toString(Qt::ISODate)));
//
//                child->setText(3, d->manager->getPatients().at(i)->getStudies().at(j)->getId());
//                child->setData(4, 1, QVariant(d->manager->getPatients().at(i)->getStudies().at(j)->getId()));
//                if (d->manager->getMode() == "CD")
//                    treeWidget->expandItem(child);
//                for (int k = 0; k < d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().size(); k++) {
//                    //Serie information
//                    QTreeWidgetItem * lchild = new QTreeWidgetItem(child);
//                    lchild->setText(0, d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription());
//                    lchild->setData(1, 1, QVariant(d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription()));
//
//                    lchild->setText(1, "Serie");
//                    lchild->setCheckState(1, Qt::Unchecked);
//                    lchild->setData(2, 1, "SERIE");
//
//                    lchild->setText(3, d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getId());
//                    lchild->setData(4, 1, QVariant(d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getId()));
//
//                    for (int l = 0; l < d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().size(); l++) {
//                        //Images information
//                        QString filename = QDir::toNativeSeparators(d->manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().at(l)->getFilename());
//                        filename.replace(QChar('\\'), QDir::separator()).replace(QChar('/'), QDir::separator());
//                        QString basepath = d->manager->getDicomdir();
//                        basepath.truncate(basepath.lastIndexOf(QDir::separator()));
//                        QString goodFilename(filename);
//                        if (QFile(basepath + QDir::separator() + goodFilename.toLower()).exists())
//                            filename = basepath + QDir::separator() + goodFilename.toLower();
//                        else
//                            filename = basepath + QDir::separator() + goodFilename.toUpper();
//                        QTreeWidgetItem * llchild = new QTreeWidgetItem(lchild);
//                        llchild->setText(0, filename);
//                        llchild->setData(1, 1, QVariant(filename));
//
//                        llchild->setText(1, "Image");
//                        llchild->setData(2, 1, QVariant("IMAGES"));
//                    }
//                }
//            }
//        }
//    }
//    treeWidget->setAnimated(true);
}

QList<QString>
QtDCM::getImagesList()
{
    return d->imagesList;
}

void
QtDCM::itemClicked(QTreeWidgetItem* current, int column)
{
//    if (current != 0) {
//        if (current->data(2, 1).toString() == "SERIE") {
//            if (current->checkState(1) == Qt::Checked) {
//                QList<QString> list;
//                for (int i = 0; i < current->childCount(); i++) {
//                    list.append(current->child(i)->data(1, 1).toStringList());
//                }
//                if (!d->selectedSeries.contains(d->currentSerieId))
//                    d->selectedSeries.insert(d->currentSerieId, list);
//            }
//            else {
//                if (d->selectedSeries.contains(d->currentSerieId))
//                    d->selectedSeries.remove(d->currentSerieId);
//            }
//            emit(serieChecked(d->selectedSeries.size()));
//        }
//    }
}

void
QtDCM::itemSelected(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
//    //Empty d->imagesList;
//    d->imagesList.clear();
//    if (current != 0) // Avoid crash when clearDisplay is called
//    {
//        if (current->data(2, 1).toString() == "SERIE") {
//            // If a serie is selected, copy all images filenames in a list
//            d->currentSerieId = current->data(4, 1).toString();
//            for (int i = 0; i < current->childCount(); i++) {
//                d->imagesList.append(current->child(i)->data(1, 1).toStringList());
//            }
//            emit serieSelected(true);
//        }
//        else
//            emit serieSelected(false);
//    }
}

void
QtDCM::patientItemSelected(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    detailsFrame->hide();
    treeWidgetStudies->clear();
    if (current != 0) // Avoid crash when clearDisplay is called
        d->manager->findStudiesScu(current->text(0));
}

void
QtDCM::studyItemSelected(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
    treeWidgetSeries->clear();
    detailsFrame->hide();
    if (current != 0) // Avoid crash when clearDisplay is called
    {
        qDebug() << current->text(0);
        d->manager->findSeriesScu(treeWidgetPatients->currentItem()->text(0), current->text(0));
    }
}

void
QtDCM::serieItemSelected(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
//    QRegExp reg("[àâéèêîôùû]");
    if (current != 0) // Avoid crash when clearDisplay is called
    {
        // ici je remplis les champs infos !
        elementCountLabel->setText(current->data(4,0).toString());
        institutionLabel->setText(current->data(5,0).toString());
        operatorLabel->setText(current->data(6,0).toString());
    }
    detailsFrame->show();
}


void
QtDCM::contextExportMenu(const QPoint point)
{
//    // Get the QTreeWidgetItem corresponding to the clic
//    QTreeWidgetItem * item = 0;
//    item = treeWidget->itemAt(point);
//    QMenu menu(treeWidget);
//    QAction * actionDicomdir = new QAction(this);
//    QAction * actionPreview = new QAction(this);
//    QAction * actionQuery = new QAction(this);
//    QAction * actionExport = new QAction(this);
//
//    if (item != 0) {
//        // If the selected item is a SERIE
//        if (item->data(2, 1) == "SERIE") {
//            // Add preview action in the context menu
//            actionPreview->setText(tr("Preview"));
//            QObject::connect(actionPreview, SIGNAL(triggered()), this, SLOT(showPreview()));
//            menu.addAction(actionPreview);
//            menu.addSeparator();
//        }
//    }
//
//    // By default the context menu contains an open dicomdir command.
//    actionDicomdir->setText(tr("Open dicomdir"));
//    QObject::connect(actionDicomdir, SIGNAL(triggered()), this, SLOT(openDicomdir()));
//    menu.addAction(actionDicomdir);
//    actionQuery->setText(tr("Query server"));
//    QObject::connect(actionQuery, SIGNAL(triggered()), this, SLOT(queryPACS()));
//    menu.addAction(actionQuery);
//    // If no item selected (object empty)
//    if (!d->selectedSeries.isEmpty()) {
//        menu.addSeparator();
//        // If the serie is exportable, create export command in the menu
//        actionExport->setText(tr("Export"));
//        QObject::connect(actionExport, SIGNAL(triggered()), this, SLOT(exportList()));
//        menu.addAction(actionExport);
//    }
//    menu.exec(treeWidget->mapToGlobal(point));
}

void
QtDCM::openDicomdir()
{
    // Open a QFileDialog for choosing a Dicomdir
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::home().dirName());
    dialog.setWindowTitle(tr("Open dicomdir"));
    dialog.setNameFilter(tr("Dicomdir (dicomdir DICOMDIR)"));
    QString fileName;
    if (dialog.exec()) {
        fileName = dialog.selectedFiles()[0];
    }
    dialog.close();
    if (!fileName.isEmpty()) // A file has been chosen
    {
        // Set the choosed file to the d->manager and update the display
        //treeWidget->setAnimated(false);
        d->manager->setDicomdir(fileName);
        d->manager->loadDicomdir();
        display();
    }
}

void
QtDCM::exportList()
{
    if (d->selectedSeries.size() != 0) {
        // Open a QFileDialog in directory mode.
        QFileDialog * dialog = new QFileDialog(this);
        dialog->setFileMode(QFileDialog::Directory);
        dialog->setOption(QFileDialog::ShowDirsOnly, true);
        dialog->setDirectory(QDir::home().dirName());
        dialog->setWindowTitle(tr("Export directory"));
        QString directory;
        if (dialog->exec()) {
            directory = dialog->selectedFiles()[0];
        }
        dialog->close();
        if (!directory.isEmpty()) // A file has been chosen
        {
            // Set the output directory to the manager and launch the conversion process
            d->manager->setOutputDirectory(directory);
            d->manager->setSeriesToExport(d->selectedSeries);
            d->manager->exportSeries();
        }
        delete dialog;
    }
    else {
        QMessageBox * msgBox = new QMessageBox(this);
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(tr("Please select at least one serie to Export"));
        msgBox->exec();
        delete msgBox;
    }
}

void
QtDCM::exportToDirectory(QString directory)
{
    if (d->selectedSeries.size() != 0) {
        d->manager->setOutputDirectory(directory);
        d->manager->setSeriesToExport(d->selectedSeries);
        d->manager->exportSeries();
    }
    else {
        QMessageBox * msgBox = new QMessageBox(this);
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(tr("Please select at least one serie to Export"));
        msgBox->exec();
        delete msgBox;
    }
}

void
QtDCM::queryPACS()
{
    d->manager->queryPACS();
    this->display();
}

void
QtDCM::updateModality(int index)
{
    switch (index) {
        case 0://MR
            d->manager->setModality("MR");
            this->queryPACS();
            break;
        case 1://CT
            d->manager->setModality("CT");
            this->queryPACS();
            break;
        case 2://US
            d->manager->setModality("US");
            this->queryPACS();
        case 3://PET
            d->manager->setModality("PET");
            this->queryPACS();
    }
}

void
QtDCM::updateDateButtons(int index)
{
//    switch (index) {
//        //No date is used in the query
//        case 0:
//            dateEndButton->hide();
//            labelTiret->hide();
//            dateBeginButton->hide();
//            d->manager->setDate1("*");
//            d->manager->setDate2("*");
//            this->queryPACS();
//            break;
//            //Query on current date Dicom data
//        case 1:
//            dateEndButton->hide();
//            labelTiret->hide();
//            dateBeginButton->setText(QDate::currentDate().toString(Qt::ISODate));
//            dateBeginButton->setEnabled(false);
//            dateBeginButton->setFlat(true);
//            dateBeginButton->show();
//            d->beginDate = d->endDate = QDate::currentDate();
//            d->manager->setDate1(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            d->manager->setDate2(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            this->queryPACS();
//            break;
//            //Query on yesterday date Dicom data
//        case 2:
//            dateEndButton->hide();
//            labelTiret->hide();
//            dateBeginButton->setText(QDate::currentDate().addDays(-1).toString(Qt::ISODate));
//            dateBeginButton->setEnabled(false);
//            dateBeginButton->setFlat(true);
//            dateBeginButton->show();
//            d->beginDate = d->endDate = QDate::currentDate().addDays(-1);
//            d->manager->setDate1(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            d->manager->setDate2(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            this->queryPACS();
//            break;
//            //Query on specified date (use date dialog window)
//        case 3:
//            dateEndButton->hide();
//            labelTiret->hide();
//            dateBeginButton->setText(d->beginDate.toString(Qt::ISODate));
//            dateBeginButton->setEnabled(true);
//            dateBeginButton->setFlat(false);
//            dateBeginButton->show();
//            d->manager->setDate1(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            d->manager->setDate2(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            break;
//        case 4:
//            //Query on a range of date (use date dialog window)
//            dateEndButton->setText(d->beginDate.toString(Qt::ISODate));
//            dateEndButton->show();
//            labelTiret->show();
//            dateBeginButton->setText(d->beginDate.toString(Qt::ISODate));
//            dateBeginButton->setEnabled(true);
//            dateBeginButton->setFlat(false);
//            dateBeginButton->show();
//            d->manager->setDate1(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//            d->manager->setDate2(d->endDate.toString(Qt::ISODate).replace("-", ""));
//            break;
//    }
}

void
QtDCM::chooseBeginDate()
{
    //Launch a dialog window with QCalendarWidget
//    QtDcmDateDialog * dialog = new QtDcmDateDialog(this);
//    dialog->setDate(d->beginDate);
//    QDate date;
//    if (dialog->exec()) {
//        d->beginDate = dialog->getDate();
//        dateBeginButton->setText(d->beginDate.toString(Qt::ISODate));
//        if (d->endDate < d->beginDate) {
//            d->endDate = d->beginDate;
//            dateEndButton->setText(d->beginDate.toString(Qt::ISODate));
//        }
//        if (dateComboBox->currentIndex() == 3) {
//            d->endDate = d->beginDate;
//        }
//        d->manager->setDate2(d->endDate.toString(Qt::ISODate).replace("-", ""));
//        d->manager->setDate1(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//        this->queryPACS();
//    }
//    dialog->close();
//    delete dialog;
}

void
QtDCM::chooseEndDate()
{
    //Launch a dialog window with QCalendarWidget
//    QtDcmDateDialog * dialog = new QtDcmDateDialog(this);
//    dialog->setDate(d->endDate);
//    QDate date;
//    if (dialog->exec()) {
//        d->endDate = dialog->getDate();
//        dateEndButton->setText(d->endDate.toString(Qt::ISODate));
//        if (d->endDate < d->beginDate) {
//            d->beginDate = d->endDate;
//            dateBeginButton->setText(d->endDate.toString(Qt::ISODate));
//        }
//        d->manager->setDate2(d->endDate.toString(Qt::ISODate).replace("-", ""));
//        d->manager->setDate1(d->beginDate.toString(Qt::ISODate).replace("-", ""));
//        this->queryPACS();
//    }
//    dialog->close();
//    delete dialog;
}

void
QtDCM::editPreferences()
{
    //Launch a dialog window for editing PACS settings
    QtDcmPreferencesDialog * dialog = new QtDcmPreferencesDialog(this);
    dialog->setPreferences(d->manager->getPreferences());
    if (dialog->exec()) {
        dialog->updatePreferences();
    }
    dialog->close();
    delete dialog;
}

void
QtDCM::patientNameTextChanged(QString name)
{
    if (name.isEmpty()) {
        d->manager->setPatientName("*");
    }
    else
        d->manager->setPatientName(name + "*");
}

void
QtDCM::patientIdTextChanged(QString id)
{
    if (id.isEmpty()) {
        d->manager->setPatientId("*");
    }
    else
        d->manager->setPatientId(id + "*");
}

void
QtDCM::serieDescriptionTextChanged(QString desc)
{
    if (desc.isEmpty()) {
        d->manager->setSerieDescription("*");
    }
    else
        d->manager->setSerieDescription("*" + desc + "*");
}

void
QtDCM::studyDescriptionTextChanged(QString desc)
{
    if (desc.isEmpty()) {
        d->manager->setStudyDescription("*");
    }
    else
        d->manager->setStudyDescription("*" + desc + "*");
}

void
QtDCM::toggleAdvancedView()
{
//    if (advancedFrame->isHidden()) {
//        advancedButton->setText("Normal");
//        advancedFrame->show();
//    }
//    else {
//        advancedButton->setText("Advanced");
//        advancedFrame->hide();
//        studyDescriptionEdit->setText("");
//        serieDescriptionEdit->setText("");
//        patientIdEdit->setText("");
//    }
}

void
QtDCM::showPreview()
{
    QtDcmPreviewDialog * dialog = new QtDcmPreviewDialog(this);
    if (d->manager->getMode() == "CD")
        d->manager->setImagesList(d->imagesList);
    d->manager->setSerieId(d->currentSerieId);
    d->manager->makePreview();
    dialog->setImages(d->manager->getListImages());
    dialog->updatePreview();
    dialog->exec();
    dialog->close();
    delete dialog;
}
