#include "QtDcm.h"

QtDCM::QtDCM( QWidget *parent ) :
  QLabel(parent)
  {
    widget.setupUi(this);
    _beginDate = QDate::currentDate();
    _endDate = QDate::currentDate();

    //Initialisation of QTreeWidget
    widget.treeWidget->setColumnWidth(0, 250);
    widget.treeWidget->setColumnWidth(1, 80);
    QStringList labels;
    labels << "Description" << "Type" << "ID";
    widget.treeWidget->setHeaderLabels(labels);
    widget.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    //Initialisation des widgets
    widget.dateEndButton->hide();
    widget.dateEndButton->setText(_beginDate.toString(Qt::ISODate));
    widget.labelTiret->hide();
    widget.dateBeginButton->hide();
    widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));

    _manager = new QtDcmManager(this);
    initConnections();
  }

void
QtDCM::initConnections()
  {
    QObject::connect(widget.treeWidget, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(widget.treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextExportMenu(QPoint)));
    QObject::connect(widget.dateComboBox, SIGNAL(currentIndexChanged ( int ) ), this, SLOT(updateDateButtons(int)));
    QObject::connect(widget.dateBeginButton, SIGNAL(clicked()), this, SLOT(chooseBeginDate()));
    QObject::connect(widget.dateEndButton, SIGNAL(clicked()), this, SLOT(chooseEndDate()));
  }

void
QtDCM::display()
  {
    QTreeWidgetItem * child = new QTreeWidgetItem(widget.treeWidget);
    QTreeWidgetItem * root = NULL;

    for (int i = 0; i < _manager->getPatients().size(); i++)
      {
        // Fill in the QTreeWidget the patient information
        child->setText(0, _manager->getPatients().at(i)->getName());
        child->setData(1, 1, QVariant(_manager->getPatients().at(i)->getName()));

        child->setText(1, "Patient");
        child->setData(2, 1, QVariant("PATIENT"));

        child->setText(2, _manager->getPatients().at(i)->getId());
        child->setData(3, 1, QVariant(_manager->getPatients().at(i)->getId()));
        root = child;
        widget.treeWidget->expandItem(child);
        for (int j = 0; j < _manager->getPatients().at(i)->getStudies().size(); j++)
          {
            // Study information
            child = new QTreeWidgetItem(root);
            child->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getDescription());
            child->setData(1, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getDescription()));

            child->setText(1, "Study");
            child->setData(2, 1, QVariant("STUDY"));

            child->setText(2, _manager->getPatients().at(i)->getStudies().at(j)->getId());
            child->setData(3, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getId()));
            widget.treeWidget->expandItem(child);
            for (int k = 0; k < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().size(); k++)
              {
                //Serie information
                QTreeWidgetItem * lchild = new QTreeWidgetItem(child);
                lchild->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription());
                lchild->setData(1, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription()));

                lchild->setText(1, "Serie");
                lchild->setData(2, 1, "SERIE");

                lchild->setText(2, _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getId());
                lchild->setData(3, 1, QVariant(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getId()));

                for (int l = 0; l < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().size(); l++)
                  {
                    //Images information
                    QString filename = QDir::toNativeSeparators(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().at(l)->getFilename());
                    QString basepath = _manager->getDicomdir();
                    basepath.truncate(basepath.lastIndexOf(QDir::separator()));
                    filename = QDir::toNativeSeparators(basepath + QDir::separator() + filename.replace(QChar('\\'), QDir::separator()).replace(QChar('/'), QDir::separator()));
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

QList<QString>
QtDCM::getImagesList()
  {
    return _imagesList;
  }

void
QtDCM::itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous )
  {
    _imagesList.clear();
    if (current->data(2, 1).toString() == "SERIE")
      {
        // If a serie is selected, copy all images filenames in a list
        for (int i = 0; i < current->childCount(); i++)
          {
            _imagesList.append(current->child(i)->data(1, 1).toStringList());
          }
      }
  }

void
QtDCM::contextExportMenu( const QPoint point )
  {
    QTreeWidgetItem * item = 0;
    item = widget.treeWidget->itemAt(point);
    QMenu menu(widget.treeWidget);
    QAction * actionDicomdir = new QAction(this);
    QAction * actionExp = new QAction(this);
    if (item != 0)
      {
        if (item->data(2, 1) == "SERIE")
          {
            if (_imagesList.size() != 0)
              {
                actionExp->setText("Export");
                QObject::connect(actionExp, SIGNAL(activated()), this, SLOT(exportList()));
                menu.addAction(actionExp);
              }
          }
      }
    actionDicomdir->setText("Open dicomdir");
    QObject::connect(actionDicomdir, SIGNAL(activated()), this, SLOT(openDicomdir()));
    menu.addAction(actionDicomdir);
    menu.exec(widget.treeWidget->mapToGlobal(point));
  }

void
QtDCM::openDicomdir()
  {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setDirectory(QDir::home().dirName());
    dialog.setWindowTitle("Open dicomdir");
    dialog.setNameFilter(tr("Dicomdir (dicomdir DICOMDIR)"));
    QString fileName;
    if (dialog.exec())
      {
        fileName = dialog.selectedFiles()[0];
      }
    dialog.close();
    if (!fileName.isEmpty()) // A file has been chosen
      {
        _manager->setDicomdir(fileName);
        _manager->loadDicomdir();
        display();
      }
  }

void
QtDCM::exportList()
  {
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
    dialog->setDirectory(QDir::home().dirName());
    dialog->setWindowTitle("Export directory");
    QString directory;
    if (dialog->exec())
      {
        directory = dialog->selectedFiles()[0];
      }
    dialog->close();
    if (!directory.isEmpty()) // A file has been chosen
      {
        _manager->setOutputDirectory(directory);
        _manager->exportSerie(_imagesList);
      }
  }

void
QtDCM::updateDateButtons( int index )
  {
    switch (index)
      {
      case 0:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->hide();
        break;
      case 1:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->setText(QDate::currentDate().toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(false);
        widget.dateBeginButton->setFlat(true);
        widget.dateBeginButton->show();
        _beginDate = _endDate = QDate::currentDate();
        break;
      case 2:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->setText(QDate::currentDate().addDays(-1).toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(false);
        widget.dateBeginButton->setFlat(true);
        widget.dateBeginButton->show();
        _beginDate = _endDate = QDate::currentDate().addDays(-1);
        break;
      case 3:
        widget.dateEndButton->hide();
        widget.labelTiret->hide();
        widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(true);
        widget.dateBeginButton->setFlat(false);
        widget.dateBeginButton->show();
        break;
      case 4:
        widget.dateEndButton->setText(_beginDate.toString(Qt::ISODate));
        widget.dateEndButton->show();
        widget.labelTiret->show();
        widget.dateBeginButton->setText(_beginDate.toString(Qt::ISODate));
        widget.dateBeginButton->setEnabled(true);
        widget.dateBeginButton->setFlat(false);
        widget.dateBeginButton->show();
        break;
      }
  }

void
QtDCM::chooseBeginDate()
  {
    QtDcmDateDialog * dialog = new QtDcmDateDialog(this);
    dialog->getCalendarWidget()->setSelectedDate(_beginDate);
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
      }
    dialog->close();
    delete dialog;
  }

void
QtDCM::chooseEndDate()
  {
    QtDcmDateDialog * dialog = new QtDcmDateDialog(this);
    dialog->getCalendarWidget()->setSelectedDate(_endDate);
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
      }
    dialog->close();
    delete dialog;
  }
