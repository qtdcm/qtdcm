#include "QtDcm.h"

QtDCM::QtDCM( QWidget *parent ) :
  QLabel(parent)
  {
    widget.setupUi(this);
    widget.treeWidget->setColumnCount(1);
    _manager = new QtDcmManager();
    initConnections();
  }

void
QtDCM::initConnections()
  {
    QObject::connect(widget.treeWidget, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
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
    for (int i=0; i < current->childCount(); i++)
      {
        _imagesList.append(current->child(i)->data(1, 1).toStringList());
      }
  }

