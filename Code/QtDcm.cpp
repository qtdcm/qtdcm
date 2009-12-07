#include "QtDcm.h"

QtDCM::QtDCM( QWidget *parent ) :
  QLabel(parent)
  {
    widget.setupUi(this);
    widget.treeWidget->setColumnCount(1);
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
        child->setText(0, _manager->getPatients().at(i)->getName());
        root = child;
        widget.treeWidget->expandItem(child);
        for (int j = 0; j < _manager->getPatients().at(i)->getStudies().size(); j++)
          {
            child = new QTreeWidgetItem(root);
            child->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getDescription());
            widget.treeWidget->expandItem(child);
            for (int k = 0; k < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().size(); k++)
              {
                QTreeWidgetItem * lchild = new QTreeWidgetItem(child);
                lchild->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription());

                QVariant v(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription());
                lchild->setData(1, 1, v);

                for (int l = 0; l < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().size(); l++)
                  {
                    QString filename = QDir::toNativeSeparators(_manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getImages().at(l)->getFilename());
                    QString basepath = _manager->getDicomdir();
                    basepath.truncate(basepath.lastIndexOf(QDir::separator()));
                    filename = QDir::toNativeSeparators(basepath + QDir::separator() + filename.replace(QChar('\\'), QDir::separator()).replace(QChar('/'), QDir::separator()));

                    QTreeWidgetItem * llchild = new QTreeWidgetItem(lchild);
                    llchild->setText(0, filename);

                    QVariant v(filename);
                    llchild->setData(1, 1, v);
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
    QVariant v;
    for (int i = 0; i < current->childCount(); i++)
      {
        _imagesList.append(current->child(i)->data(1, 1).toStringList());
      }
  }

void
QtDCM::contextExportMenu( const QPoint point )
  {
    QTreeWidgetItem * item = 0;
    item = widget.treeWidget->itemAt(point);
    if (_imagesList.size() != 0)
      {
        QMenu menu(widget.treeWidget);
        QAction * action = new QAction(this);
        action->setText("Export");
        QObject::connect(action, SIGNAL(activated()), this, SLOT(exportList()));
        menu.addAction(action);
        menu.exec(widget.treeWidget->mapToGlobal(point));
      }
  }

void
QtDCM::exportList()
  {
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setDirectory(QDir::home().dirName());
    dialog.setWindowTitle("Export directory");
    QString directory;
    if (dialog.exec())
      {
        directory = dialog.selectedFiles()[0];
      }
    dialog.close();
    if (!directory.isEmpty()) // A file has been chosen
      {
        _manager->setOutputDirectory(directory);
        _manager->exportSerie(_imagesList);
      }
  }
