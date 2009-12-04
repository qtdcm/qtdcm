#include "QtDcm.h"

QtDCM::QtDCM( QWidget *parent ) :
  QLabel(parent)
  {
    widget.setupUi(this);
    widget.treeWidget->setColumnCount(1);
    _manager = new QtDcmManager();
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
        for (int j = 0; j < _manager->getPatients().at(i)->getStudies().size(); j++)
          {
            child = new QTreeWidgetItem(root);
            child->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getDescription());
            widget.treeWidget->expandItem(child);
            for (int k = 0; k < _manager->getPatients().at(i)->getStudies().at(j)->getSeries().size(); k++)
              {
                QTreeWidgetItem * lchild = new QTreeWidgetItem(child);
                lchild->setText(0, _manager->getPatients().at(i)->getStudies().at(j)->getSeries().at(k)->getDescription());
                widget.treeWidget->expandItem(lchild);
              }
          }
      }
  }
