/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferencesDialog.h>

QtDcmPreferencesDialog::QtDcmPreferencesDialog(QWidget * parent)
  {
    widget.setupUi(this);
    this->setModal(true);
    this->setParent(parent,Qt::Dialog);

//    widget.treeWidget->setColumnWidth(0, 150);
    widget.treeWidget->setColumnWidth(1, 100);
    widget.treeWidget->setColumnWidth(2, 100);
    widget.treeWidget->setColumnWidth(3, 150);
  }

void QtDcmPreferencesDialog::initConnections()
  {

  }

void
QtDcmPreferencesDialog::setPreferences( QtDcmPreferences * prefs )
  {
    _preferences = prefs;
    widget.localAetitleEdit->setText(_preferences->getAetitle());
    widget.localPortEdit->setText(_preferences->getPort());
    for (int i = 0; i < _preferences->getServers().size(); i++)
      {
          QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidget);
          item->setText(0,_preferences->getServers().at(i)->getName());
          item->setData(0,1,QVariant(_preferences->getServers().at(i)->getName()));
          item->setText(1,_preferences->getServers().at(i)->getAetitle());
          item->setData(1,1,QVariant(_preferences->getServers().at(i)->getAetitle()));
          item->setText(2,_preferences->getServers().at(i)->getPort());
          item->setData(2,1,QVariant(_preferences->getServers().at(i)->getPort()));
          item->setText(3,_preferences->getServers().at(i)->getServer());
          item->setData(3,1,QVariant(_preferences->getServers().at(i)->getServer()));
      }
  }

void QtDcmPreferencesDialog::updatePreferences()
  {
    _preferences->setAetitle(widget.localAetitleEdit->text());
    _preferences->setPort(widget.localPortEdit->text());

    QTreeWidgetItem * root = widget.treeWidget->invisibleRootItem();
    for (int i = 0 ; i < _preferences->getServers().size() ;i++)
      {
        _preferences->getServers().at(i)->setName(root->child(i)->data(0, 1).toString());
        _preferences->getServers().at(i)->setServer(root->child(i)->data(1, 1).toString());
        _preferences->getServers().at(i)->setAetitle(root->child(i)->data(2, 1).toString());
        _preferences->getServers().at(i)->setPort(root->child(i)->data(3, 1).toString());
      }

    _preferences->writeSettings();
  }

QtDcmPreferencesDialog::~QtDcmPreferencesDialog() {}
