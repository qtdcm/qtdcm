#include "QtDcm.h"

QtDCM::QtDCM( QWidget *parent) :
  QLabel(parent)
  {
    widget.setupUi(this);
    widget.treeWidget->setColumnWidth(0, 250);
    widget.treeWidget->setColumnWidth(1, 80);
    QStringList labels;
    labels << "Description" << "Type" << "ID";
    widget.treeWidget->setHeaderLabels(labels);
    widget.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    _manager = new QtDcmManager(this);
    initConnections();
  }

void
QtDCM::initConnections()
  {
    QObject::connect(widget.treeWidget, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(widget.treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextExportMenu(QPoint)));
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
