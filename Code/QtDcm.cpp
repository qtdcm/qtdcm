#include "QtDcm.h"

QtDCM::QtDCM( QWidget *parent ) :
  QLabel(parent)
  {
    widget.setupUi(this);
    _beginDate = QDate::currentDate();
    _endDate = QDate::currentDate();

    //Initialize QTreeWidget
    widget.treeWidget->setColumnWidth(0, 250);
    widget.treeWidget->setColumnWidth(1, 100);
    widget.treeWidget->setColumnWidth(2, 120);
    QStringList labels;
    labels << "Description" << "Type" << "Date" << "ID";
    widget.treeWidget->setHeaderLabels(labels);
    widget.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    //Initialize widgets
    widget.advancedFrame->hide();
    widget.advancedButton->setCheckable(true);
    widget.dateEndButton->hide();
    widget.dateEndButton->setText(_beginDate.toString(Qt::ISODate));
    widget.labelTiret->hide();
    widget.dateBeginButton->hide();
    widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));

    _manager = new QtDcmManager(this);
    initConnections();
  }

QtDCM::~QtDCM()
  {
    delete _manager;
  }

void
QtDCM::initConnections()
  {
    // Initialize connections
    QObject::connect(widget.treeWidget, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(widget.treeWidget, SIGNAL(itemClicked (QTreeWidgetItem*, int)), this, SLOT(itemClicked(QTreeWidgetItem*, int)));
    QObject::connect(widget.treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextExportMenu(QPoint)));
    QObject::connect(widget.dateComboBox, SIGNAL(currentIndexChanged ( int ) ), this, SLOT(updateDateButtons(int)));
    QObject::connect(widget.modalityComboBox, SIGNAL(currentIndexChanged ( int ) ), this, SLOT(updateModality(int)));
    QObject::connect(widget.dateBeginButton, SIGNAL(clicked()), this, SLOT(chooseBeginDate()));
    QObject::connect(widget.dateEndButton, SIGNAL(clicked()), this, SLOT(chooseEndDate()));
    QObject::connect(widget.searchButton, SIGNAL(clicked()), this, SLOT(queryPACS()));
    QObject::connect(widget.nameEdit, SIGNAL(textChanged(QString)), this, SLOT(patientNameTextChanged(QString)));
    QObject::connect(widget.patientIdEdit, SIGNAL(textChanged(QString)), this, SLOT(patientIdTextChanged(QString)));
    QObject::connect(widget.serieDescriptionEdit, SIGNAL(textChanged(QString)), this, SLOT(serieDescriptionTextChanged(QString)));
    QObject::connect(widget.studyDescriptionEdit, SIGNAL(textChanged(QString)), this, SLOT(studyDescriptionTextChanged(QString)));
    QObject::connect(widget.advancedButton, SIGNAL(clicked()), this, SLOT(toggleAdvancedView()));
  }

void
QtDCM::clearDisplay()
  {
    _selectedSeries.clear();
    _imagesList.clear();
    widget.treeWidget->clear();
    widget.treeWidget->setAnimated(false);
    emit serieSelected(false);
  }

void
QtDCM::display()
  {
    //Clear view
    this->clearDisplay();
    for (int i = 0; i < _manager->getPatients().size(); i++)
      {
        if (_manager->getPatients().at(i)->getStudies().size() != 0)
          {
            QTreeWidgetItem * child = new QTreeWidgetItem(widget.treeWidget);
            QTreeWidgetItem * root = NULL;
            // Fill in the QTreeWidget the patient information
            child->setText(0, _manager->getPatients().at(i)->getName());
            child->setData(1, 1, QVariant(_manager->getPatients().at(i)->getName()));

            child->setText(1, "Patient");
            child->setData(2, 1, QVariant("PATIENT"));

            child->setText(3, _manager->getPatients().at(i)->getId());
            child->setData(4, 1, QVariant(_manager->getPatients().at(i)->getId()));

            root = child;
            //        if (_manager->getMode() == "CD")
            widget.treeWidget->expandItem(child);
            for (int j = 0; j < _manager->getPatients().at(i)->getStudies().size(); j++)
              {
                // Study information
                child = new QTreeWidgetItem(root);
                child->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getDescription());
                child->setData(1, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getDescription()));

                child->setText(1, "Study");
                child->setData(2, 1, QVariant("STUDY"));

                child->setText(2, _manager->getPatients().at(i)->getStudies().at(j)->getDate().toString(Qt::ISODate));
                child->setData(3, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getDate().toString(Qt::ISODate)));

                child->setText(3, _manager->getPatients().at(i)->getStudies().at(j)->getId());
                child->setData(4, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getId()));
                if (_manager->getMode() == "CD")
                  widget.treeWidget->expandItem(child);
                for (int k = 0; k < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().size(); k++)
                  {
                    //Serie information
                    QTreeWidgetItem * lchild = new QTreeWidgetItem(child);
                    lchild->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription());
                    lchild->setData(1, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription()));

                    lchild->setText(1, "Serie");
                    lchild->setCheckState(1, Qt::Unchecked);
                    lchild->setData(2, 1, "SERIE");

                    lchild->setText(3, _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getId());
                    lchild->setData(4, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getId()));

                    for (int l = 0; l < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().size(); l++)
                      {
                        //Images information
                        QString filename = QDir::toNativeSeparators(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().at(l)->getFilename());
                        filename.replace(QChar('\\'), QDir::separator()).replace(QChar('/'), QDir::separator());
                        QString basepath = _manager->getDicomdir();
                        basepath.truncate(basepath.lastIndexOf(QDir::separator()));
                        QString goodFilename(filename);
                        if (QFile(basepath + QDir::separator() + goodFilename.toLower()).exists())
                          filename = basepath + QDir::separator() + goodFilename.toLower();
                        else
                          filename = basepath + QDir::separator() + goodFilename.toUpper();
                        QTreeWidgetItem * llchild = new QTreeWidgetItem(lchild);
                        llchild->setText(0, filename);
                        llchild->setData(1, 1, QVariant(filename));

                        llchild->setText(1, "Image");
                        llchild->setData(2, 1, QVariant("IMAGES"));
                      }
                  }
              }
          }
      }
    widget.treeWidget->setAnimated(true);
  }

QList<QString>
QtDCM::getImagesList()
  {
    return _imagesList;
  }

void
QtDCM::itemClicked( QTreeWidgetItem* current , int column )
  {
    if (current != 0)
      {
        if (current->data(2, 1).toString() == "SERIE")
          {
            if (current->checkState(1) == Qt::Checked)
              {
                QList<QString> list;
                for (int i = 0; i < current->childCount(); i++)
                  {
                    list.append(current->child(i)->data(1, 1).toStringList());
                  }
                if (!_selectedSeries.contains(_currentSerieId))
                  _selectedSeries.insert(_currentSerieId, list);
              }
            else
              {
                if (_selectedSeries.contains(_currentSerieId))
                  _selectedSeries.remove(_currentSerieId);
              }
            emit(serieChecked(_selectedSeries.size()));
          }
      }
  }

void
QtDCM::itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous )
  {
    //Empty _imagesList;
    _imagesList.clear();
    if (current != 0) // Avoid crash when clearDisplay is called
      {
        if (current->data(2, 1).toString() == "SERIE")
          {
            // If a serie is selected, copy all images filenames in a list
            _currentSerieId = current->data(4, 1).toString();
            for (int i = 0; i < current->childCount(); i++)
              {
                _imagesList.append(current->child(i)->data(1, 1).toStringList());
              }
            emit serieSelected(true);
          }
        else
          emit serieSelected(false);
      }
  }

void
QtDCM::contextExportMenu( const QPoint point )
  {
    // Get the QTreeWidgetItem corresponding to the clic
    QTreeWidgetItem * item = 0;
    item = widget.treeWidget->itemAt(point);
    QMenu menu(widget.treeWidget);
    QAction * actionDicomdir = new QAction(this);
    QAction * actionPreview = new QAction(this);
    QAction * actionQuery = new QAction(this);
    QAction * actionExport = new QAction(this);

    if (item != 0)
      {
        // If the selected item is a SERIE
        if (item->data(2, 1) == "SERIE")
          {
            // Add preview action in the context menu
            actionPreview->setText(tr("Preview"));
            QObject::connect(actionPreview, SIGNAL(triggered()), this, SLOT(showPreview()));
            menu.addAction(actionPreview);
            menu.addSeparator();
          }
      }

    // By default the context menu contains an open dicomdir command.
    actionDicomdir->setText(tr("Open dicomdir"));
    QObject::connect(actionDicomdir, SIGNAL(triggered()), this, SLOT(openDicomdir()));
    menu.addAction(actionDicomdir);
    actionQuery->setText(tr("Query server"));
    QObject::connect(actionQuery, SIGNAL(triggered()), this, SLOT(queryPACS()));
    menu.addAction(actionQuery);
    // If no item selected (object empty)
    if (!_selectedSeries.isEmpty())
      {
        menu.addSeparator();
        // If the serie is exportable, create export command in the menu
        actionExport->setText(tr("Export"));
        QObject::connect(actionExport, SIGNAL(triggered()), this, SLOT(exportList()));
        menu.addAction(actionExport);
      }
    menu.exec(widget.treeWidget->mapToGlobal(point));
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
    if (dialog.exec())
      {
        fileName = dialog.selectedFiles()[0];
      }
    dialog.close();
    if (!fileName.isEmpty()) // A file has been chosen
      {
        // Set the choosed file to the _manager and update the display
        widget.treeWidget->setAnimated(false);
        _manager->setDicomdir(fileName);
        _manager->loadDicomdir();
        display();
      }
  }

void
QtDCM::exportList()
  {
    if (_selectedSeries.size() != 0)
      {
        // Open a QFileDialog in directory mode.
        QFileDialog * dialog = new QFileDialog(this);
        dialog->setFileMode(QFileDialog::Directory);
        dialog->setOption(QFileDialog::ShowDirsOnly, true);
        dialog->setDirectory(QDir::home().dirName());
        dialog->setWindowTitle(tr("Export directory"));
        QString directory;
        if (dialog->exec())
          {
            directory = dialog->selectedFiles()[0];
          }
        dialog->close();
        if (!directory.isEmpty()) // A file has been chosen
          {
            // Set the output directory to the manager and launch the conversion process
            _manager->setOutputDirectory(directory);
            _manager->setSeriesToExport(_selectedSeries);
            _manager->exportSeries();
          }
        delete dialog;
      }
    else
      {
        QMessageBox * msgBox = new QMessageBox(this);
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(tr("Please select at least one serie to Export"));
        msgBox->exec();
        delete msgBox;
      }
  }

void
QtDCM::exportToDirectory( QString directory )
  {
    if (_selectedSeries.size() != 0)
      {
        _manager->setOutputDirectory(directory);
        _manager->setSeriesToExport(_selectedSeries);
        _manager->exportSeries();
      }
    else
      {
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
    _manager->queryPACS();
    this->display();
  }

void
QtDCM::updateModality( int index )
  {
    switch (index)
      {
      case 0://MR
        _manager->setModality("MR");
        this->queryPACS();
        break;
      case 1://CT
        _manager->setModality("CT");
        this->queryPACS();
        break;
      case 2://US
        _manager->setModality("US");
        this->queryPACS();
      case 3://PET
        _manager->setModality("PET");
        this->queryPACS();
      }
  }

void
QtDCM::updateDateButtons( int index )
  {
    switch (index)
      {
      //No date is used in the query
      case 0:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->hide();
        _manager->setDate1("*");
        _manager->setDate2("*");
        this->queryPACS();
        break;
        //Query on current date Dicom data
      case 1:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->setText(QDate::currentDate().toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(false);
        widget.dateBeginButton->setFlat(true);
        widget.dateBeginButton->show();
        _beginDate = _endDate = QDate::currentDate();
        _manager->setDate1(_beginDate.toString(Qt::ISODate).replace("-", ""));
        _manager->setDate2(_beginDate.toString(Qt::ISODate).replace("-", ""));
        this->queryPACS();
        break;
        //Query on yesterday date Dicom data
      case 2:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->setText(QDate::currentDate().addDays(-1).toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(false);
        widget.dateBeginButton->setFlat(true);
        widget.dateBeginButton->show();
        _beginDate = _endDate = QDate::currentDate().addDays(-1);
        _manager->setDate1(_beginDate.toString(Qt::ISODate).replace("-", ""));
        _manager->setDate2(_beginDate.toString(Qt::ISODate).replace("-", ""));
        this->queryPACS();
        break;
        //Query on specified date (use date dialog window)
      case 3:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(true);
        widget.dateBeginButton->setFlat(false);
        widget.dateBeginButton->show();
        _manager->setDate1(_beginDate.toString(Qt::ISODate).replace("-", ""));
        _manager->setDate2(_beginDate.toString(Qt::ISODate).replace("-", ""));
        break;
      case 4:
        //Query on a range of date (use date dialog window)
        widget.dateEndButton->setText(_beginDate.toString(Qt::ISODate));
        widget.dateEndButton->show();
        widget.labelTiret->show();
        widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(true);
        widget.dateBeginButton->setFlat(false);
        widget.dateBeginButton->show();
        _manager->setDate1(_beginDate.toString(Qt::ISODate).replace("-", ""));
        _manager->setDate2(_endDate.toString(Qt::ISODate).replace("-", ""));
        break;
      }
  }

void
QtDCM::chooseBeginDate()
  {
    //Launch a dialog window with QCalendarWidget
    QtDcmDateDialog * dialog = new QtDcmDateDialog(this);
    dialog->setDate(_beginDate);
    QDate date;
    if (dialog->exec())
      {
        _beginDate = dialog->getDate();
        widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));
        if (_endDate < _beginDate)
          {
            _endDate = _beginDate;
            widget.dateEndButton->setText(_beginDate.toString(Qt::ISODate));
          }
        if (widget.dateComboBox->currentIndex() == 3)
          {
            _endDate = _beginDate;
          }
        _manager->setDate2(_endDate.toString(Qt::ISODate).replace("-", ""));
        _manager->setDate1(_beginDate.toString(Qt::ISODate).replace("-", ""));
        this->queryPACS();
      }
    dialog->close();
    delete dialog;
  }

void
QtDCM::chooseEndDate()
  {
    //Launch a dialog window with QCalendarWidget
    QtDcmDateDialog * dialog = new QtDcmDateDialog(this);
    dialog->setDate(_endDate);
    QDate date;
    if (dialog->exec())
      {
        _endDate = dialog->getDate();
        widget.dateEndButton->setText(_endDate.toString(Qt::ISODate));
        if (_endDate < _beginDate)
          {
            _beginDate = _endDate;
            widget.dateBeginButton->setText(_endDate.toString(Qt::ISODate));
          }
        _manager->setDate2(_endDate.toString(Qt::ISODate).replace("-", ""));
        _manager->setDate1(_beginDate.toString(Qt::ISODate).replace("-", ""));
        this->queryPACS();
      }
    dialog->close();
    delete dialog;
  }

void
QtDCM::editPreferences()
  {
    //Launch a dialog window for editing PACS settings
    QtDcmPreferencesDialog * dialog = new QtDcmPreferencesDialog(this);
    dialog->setPreferences(_manager->getPreferences());
    if (dialog->exec())
      {
        dialog->updatePreferences();
      }
    dialog->close();
    delete dialog;
  }

void
QtDCM::patientNameTextChanged( QString name )
  {
    if (name.isEmpty())
      {
        _manager->setPatientName("*");
      }
    else
      _manager->setPatientName("*" + name + "*");
  }

void
QtDCM::patientIdTextChanged( QString id )
  {
    if (id.isEmpty())
      {
        _manager->setPatientId("*");
      }
    else
      _manager->setPatientId("*" + id + "*");
  }

void
QtDCM::serieDescriptionTextChanged( QString desc )
  {
    if (desc.isEmpty())
      {
        _manager->setSerieDescription("*");
      }
    else
      _manager->setSerieDescription("*" + desc + "*");
  }

void
QtDCM::studyDescriptionTextChanged( QString desc )
  {
    if (desc.isEmpty())
      {
        _manager->setStudyDescription("*");
      }
    else
      _manager->setStudyDescription("*" + desc + "*");
  }

void
QtDCM::toggleAdvancedView()
  {
    if (widget.advancedFrame->isHidden())
      {
        widget.advancedButton->setText("Normal");
        widget.advancedFrame->show();
      }
    else
      {
        widget.advancedButton->setText("Advanced");
        widget.advancedFrame->hide();
        widget.studyDescriptionEdit->setText("");
        widget.serieDescriptionEdit->setText("");
        widget.patientIdEdit->setText("");
      }
  }

void
QtDCM::showPreview()
  {
    QtDcmPreviewDialog * dialog = new QtDcmPreviewDialog(this);
    if (_manager->getMode() == "CD")
      _manager->setImagesList(_imagesList);
    _manager->setSerieId(_currentSerieId);
    _manager->makePreview();
    dialog->setImages(_manager->getListImages());
    dialog->exec();
    dialog->close();
    delete dialog;
  }
