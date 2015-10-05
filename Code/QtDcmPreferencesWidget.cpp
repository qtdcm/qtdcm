/*
    QtDcm is a C++ Qt based library for communication and conversion of Dicom images.
    Copyright (C) 2011  Alexandre Abadie <Alexandre.Abadie@univ-rennes1.fr>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "QtDcmPreferencesWidget.h"
#include "QtDcmPreferences.h"
#include "QtDcmServer.h"

QtDcmPreferencesWidget::QtDcmPreferencesWidget ( QWidget* parent ) 
    : QWidget(parent)
{
    setupUi ( this );

//     treeWidget->setColumnWidth ( 1, 100 );
//     treeWidget->setColumnWidth ( 2, 100 );
//     treeWidget->setColumnWidth ( 3, 150 );
// 
//     serverNameEdit->setEnabled ( false );
//     serverAetitleEdit->setEnabled ( false );
//     serverPortEdit->setEnabled ( false );
//     serverHostnameEdit->setEnabled ( false );
//     removeButton->setEnabled ( false );
//     echoButton->setEnabled ( false );

//     this->initConnections();
    
}

QtDcmPreferencesWidget::~QtDcmPreferencesWidget()
{
}

void QtDcmPreferencesWidget::readPreferences()
{
    localDicomSettingsWidget->readPreferences();
    serversDicomSettingsWidget->readPreferences();
}

void QtDcmPreferencesWidget::updatePreferences()
{
//     d->preferences->setAetitle ( localAetitleEdit->text() );
//     d->preferences->setPort ( localPortEdit->text() );
//     d->preferences->setHostname ( localHostnameEdit->text() );
//     QTreeWidgetItem * root = treeWidget->invisibleRootItem();
// 
//     for ( int i = 0; i < d->preferences->getServers().size(); i++ )
//     {
//         d->preferences->getServers().at ( i )->setName ( root->child ( i )->data ( 0, 1 ).toString() );
//         d->preferences->getServers().at ( i )->setAetitle ( root->child ( i )->data ( 1, 1 ).toString() );
//         d->preferences->getServers().at ( i )->setPort ( root->child ( i )->data ( 2, 1 ).toString() );
//         d->preferences->getServers().at ( i )->setServer ( root->child ( i )->data ( 3, 1 ).toString() );
//     }

    localDicomSettingsWidget->updatePreferences();
    serversDicomSettingsWidget->updatePreferences();

//     d->preferences->writeSettings();
}

// void QtDcmPreferencesWidget::itemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous )
// {
//     echoButton->setEnabled ( true );
//     removeButton->setEnabled ( true );
//     serverNameEdit->setEnabled ( true );
//     serverAetitleEdit->setEnabled ( true );
//     serverPortEdit->setEnabled ( true );
//     serverHostnameEdit->setEnabled ( true );
//     serverNameEdit->setText ( current->data ( 0, 1 ).toString() );
//     serverAetitleEdit->setText ( current->data ( 1, 1 ).toString() );
//     serverPortEdit->setText ( current->data ( 2, 1 ).toString() );
//     serverHostnameEdit->setText ( current->data ( 3, 1 ).toString() );
// }
// 
// void QtDcmPreferencesWidget::serverNameChanged ( QString text )
// {
//     treeWidget->currentItem()->setText ( 0, text );
//     treeWidget->currentItem()->setData ( 0, 1, QVariant ( text ) );
// }
// 
// void QtDcmPreferencesWidget::serverAetitleChanged ( QString text )
// {
//     QRegExp rexp ( "[A-Z0-9._-]{1,50}" );
//     QRegExpValidator * valid = new QRegExpValidator ( rexp, this );
//     serverAetitleEdit->setValidator ( valid );
//     treeWidget->currentItem()->setText ( 1, text );
//     treeWidget->currentItem()->setData ( 1, 1, QVariant ( text ) );
// }
// 
// void QtDcmPreferencesWidget::serverPortChanged ( QString text )
// {
//     QIntValidator * valid = new QIntValidator ( 1000, 100000, this );
//     serverPortEdit->setValidator ( valid );
//     treeWidget->currentItem()->setText ( 2, text );
//     treeWidget->currentItem()->setData ( 2, 1, QVariant ( text ) );
// }
// 
// void QtDcmPreferencesWidget::serverHostnameChanged ( QString text )
// {
//     treeWidget->currentItem()->setText ( 3, text );
//     treeWidget->currentItem()->setData ( 3, 1, QVariant ( text ) );
// }
// 
// void QtDcmPreferencesWidget::addServer()
// {
//     QTreeWidgetItem * item = new QTreeWidgetItem ( treeWidget );
//     d->preferences->addServer();
//     item->setText ( 0, "Name" );
//     item->setData ( 0, 1, QVariant ( "Name" ) );
//     d->preferences->getServers().last()->setName ( "Name" );
//     item->setText ( 1, "AETITLE" );
//     item->setData ( 1, 1, QVariant ( "AETITLE" ) );
//     d->preferences->getServers().last()->setAetitle ( "AETITLE" );
//     item->setText ( 2, "2010" );
//     d->preferences->getServers().last()->setPort ( "2010" );
//     item->setData ( 2, 1, QVariant ( 2010 ) );
//     item->setText ( 3, "hostname" );
//     item->setData ( 3, 1, QVariant ( "hostname" ) );
//     item->setData ( 4, 1, QVariant ( d->preferences->getServers().size() - 1 ) );
//     d->preferences->getServers().last()->setServer ( "hostname" );
// }
// 
// void QtDcmPreferencesWidget::removeServer()
// {
//     QTreeWidgetItem * root = treeWidget->invisibleRootItem();
//     d->preferences->removeServer ( root->indexOfChild ( treeWidget->currentItem() ) );
// 
//     if ( root->childCount() == 0 )
//     {
//         echoButton->setEnabled ( false );
//         removeButton->setEnabled ( false );
//     }
//     else if ( root->childCount() == 1 )
//     {
//         treeWidget->reset();
//         treeWidget->clear();
//         removeButton->setEnabled ( false );
//         echoButton->setEnabled ( false );
//     }
//     else
//         root->removeChild ( treeWidget->currentItem() );
// }
