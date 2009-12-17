/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferencesDialog.h>

QtDcmPreferencesDialog::QtDcmPreferencesDialog( QWidget * parent )
  {
    widget.setupUi(this);
    this->setModal(true);
    this->setParent(parent, Qt::Dialog);

    widget.treeWidget->setColumnWidth(1, 100);
    widget.treeWidget->setColumnWidth(2, 100);
    widget.treeWidget->setColumnWidth(3, 150);

    widget.serverNameEdit->setEnabled(false);
    widget.serverAetitleEdit->setEnabled(false);
    widget.serverPortEdit->setEnabled(false);
    widget.serverHostnameEdit->setEnabled(false);
    widget.removeButton->setEnabled(false);

    this->initConnections();
  }

void
QtDcmPreferencesDialog::initConnections()
  {
    QObject::connect(widget.treeWidget, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itemSelected(QTreeWidgetItem*, QTreeWidgetItem*)));
    QObject::connect(widget.serverNameEdit, SIGNAL(textChanged(QString)), this, SLOT(serverNameChanged(QString)));
    QObject::connect(widget.serverHostnameEdit, SIGNAL(textChanged(QString)), this, SLOT(serverHostnameChanged(QString)));
    QObject::connect(widget.serverAetitleEdit, SIGNAL(textChanged(QString)), this, SLOT(serverAetitleChanged(QString)));
    QObject::connect(widget.serverPortEdit, SIGNAL(textChanged(QString)), this, SLOT(serverPortChanged(QString)));
    QObject::connect(widget.addButton, SIGNAL(clicked()), this, SLOT(addServer()));
    QObject::connect(widget.removeButton, SIGNAL(clicked()), this, SLOT(removeServer()));
    QObject::connect(widget.dcm2niiButton, SIGNAL(clicked()), this, SLOT(setDcm2nii()));
    QObject::connect(widget.dcm4cheButton, SIGNAL(clicked()), this, SLOT(setDcm4che()));
  }

void
QtDcmPreferencesDialog::setPreferences( QtDcmPreferences * prefs )
  {
    _preferences = prefs;
    widget.dcm2niiPathEdit->setText(_preferences->getDcm2nii());
    widget.dcm4chePathEdit->setText(_preferences->getDcm4che());
    widget.localAetitleEdit->setText(_preferences->getAetitle());
    widget.localPortEdit->setText(_preferences->getPort());
    for (int i = 0; i < _preferences->getServers().size(); i++)
      {
        QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidget);
        item->setText(0, _preferences->getServers().at(i)->getName());
        item->setData(0, 1, QVariant(_preferences->getServers().at(i)->getName()));
        item->setText(1, _preferences->getServers().at(i)->getAetitle());
        item->setData(1, 1, QVariant(_preferences->getServers().at(i)->getAetitle()));
        item->setText(2, _preferences->getServers().at(i)->getPort());
        item->setData(2, 1, QVariant(_preferences->getServers().at(i)->getPort()));
        item->setText(3, _preferences->getServers().at(i)->getServer());
        item->setData(3, 1, QVariant(_preferences->getServers().at(i)->getServer()));
      }
  }

void
QtDcmPreferencesDialog::updatePreferences()
  {
    _preferences->setDcm2nii(widget.dcm2niiPathEdit->text());
    _preferences->setDcm4che(widget.dcm4chePathEdit->text());
    _preferences->setAetitle(widget.localAetitleEdit->text());
    _preferences->setPort(widget.localPortEdit->text());
    QTreeWidgetItem * root = widget.treeWidget->invisibleRootItem();
    for (int i = 0; i < _preferences->getServers().size(); i++)
      {
        _preferences->getServers().at(i)->setName(root->child(i)->data(0, 1).toString());
        _preferences->getServers().at(i)->setAetitle(root->child(i)->data(1, 1).toString());
        _preferences->getServers().at(i)->setPort(root->child(i)->data(2, 1).toString());
        _preferences->getServers().at(i)->setServer(root->child(i)->data(3, 1).toString());
      }
    _preferences->writeSettings();
  }

void
QtDcmPreferencesDialog::itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous )
  {
    widget.removeButton->setEnabled(true);
    widget.serverNameEdit->setEnabled(true);
    widget.serverAetitleEdit->setEnabled(true);
    widget.serverPortEdit->setEnabled(true);
    widget.serverHostnameEdit->setEnabled(true);
    widget.serverNameEdit->setText(current->data(0, 1).toString());
    widget.serverAetitleEdit->setText(current->data(1, 1).toString());
    widget.serverPortEdit->setText(current->data(2, 1).toString());
    widget.serverHostnameEdit->setText(current->data(3, 1).toString());
  }

void
QtDcmPreferencesDialog::serverNameChanged( QString text )
  {
    widget.treeWidget->currentItem()->setText(0, text);
    widget.treeWidget->currentItem()->setData(0, 1, QVariant(text));
  }

void
QtDcmPreferencesDialog::serverAetitleChanged( QString text )
  {
    QRegExp rexp ("[A-Z0-9]{1,10}");
    QRegExpValidator * valid = new QRegExpValidator(rexp,this);
    widget.serverAetitleEdit->setValidator(valid);
    widget.treeWidget->currentItem()->setText(1, text);
    widget.treeWidget->currentItem()->setData(1, 1, QVariant(text));
  }

void
QtDcmPreferencesDialog::serverPortChanged( QString text )
  {
    QIntValidator * valid = new QIntValidator(1000, 100000,this);
    widget.serverPortEdit->setValidator(valid);
    widget.treeWidget->currentItem()->setText(2, text);
    widget.treeWidget->currentItem()->setData(2, 1, QVariant(text));
  }

void
QtDcmPreferencesDialog::serverHostnameChanged( QString text )
  {
    widget.treeWidget->currentItem()->setText(3, text);
    widget.treeWidget->currentItem()->setData(3, 1, QVariant(text));
  }

void
QtDcmPreferencesDialog::addServer()
  {
    QTreeWidgetItem * item = new QTreeWidgetItem(widget.treeWidget);
    _preferences->addServer();
    item->setText(0, "Name");
    item->setData(0, 1, QVariant("Name"));
    _preferences->getServers().last()->setName("Name");
    item->setText(1, "AETITLE");
    item->setData(1, 1, QVariant("AETITLE"));
    _preferences->getServers().last()->setAetitle("AETITLE");
    item->setText(2, "2010");
    _preferences->getServers().last()->setPort("2010");
    item->setData(2, 1, QVariant(2010));
    item->setText(3, "hostname");
    item->setData(3, 1, QVariant("hostname"));
    _preferences->getServers().last()->setServer("hostname");
  }

void
QtDcmPreferencesDialog::removeServer()
  {
    QTreeWidgetItem * root = widget.treeWidget->invisibleRootItem();
    _preferences->removeServer(root->indexOfChild(widget.treeWidget->currentItem()));
    qDebug() << _preferences->getServers().size();
    root->removeChild(widget.treeWidget->currentItem());
    if (root->childCount() == 0)
      {
        widget.removeButton->setEnabled(false);
      }
  }

void QtDcmPreferencesDialog::setDcm2nii()
  {
    // Open aa QFileDialog in directory mode.
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
#ifdef Q_OS_UNIX
    dialog->setNameFilter(tr("Dcm2nii (dcm2nii)"));
#else
    dialog->setNameFilter(tr("Dcm2nii (dcm2nii.exe)"));
#endif
    dialog->setDirectory(QDir::home().dirName());
    dialog->setWindowTitle("Choose dcm2nii binary");
    QString filename;
    if (dialog->exec())
      {
      filename = dialog->selectedFiles()[0];
      }
    dialog->close();
    if (!filename.isEmpty()) // A file has been chosen
      {
        // The the output directory to the manager and launch the conversion process
        _preferences->setDcm2nii(filename);
        widget.dcm2niiPathEdit->setText(filename);
      }
  }

void QtDcmPreferencesDialog::setDcm4che()
  {
    // Open aa QFileDialog in directory mode.
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
#ifdef Q_OS_UNIX
    dialog->setNameFilter(tr("Dcm4che (dcmqr)"));
#else
    dialog->setNameFilter(tr("Dcm4che (dcmqr.bat)"));
#endif
    dialog->setDirectory(QDir::home().dirName());
    dialog->setWindowTitle("Choose dcmqr binary");
    QString filename;
    if (dialog->exec())
      {
        filename = dialog->selectedFiles()[0];
      }
    dialog->close();
    if (!filename.isEmpty()) // A file has been chosen
      {
        // The the output directory to the manager and launch the conversion process
        _preferences->setDcm4che(filename);
        widget.dcm4chePathEdit->setText(filename);
      }
  }
