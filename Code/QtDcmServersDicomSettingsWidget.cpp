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


#include "QtDcmServersDicomSettingsWidget.h"

#include <dcmtk/dcmnet/dimse.h>

#include <QMessageBox>
#include <QTcpSocket>

#include <QtDcmPreferences.h>
#include <QtDcmServer.h>

QtDcmServersDicomSettingsWidget::QtDcmServersDicomSettingsWidget ( QWidget* parent ) 
    : QWidget ( parent )
{
    this->setupUi ( this );

    treeWidget->setColumnWidth ( 1, 100 );
    treeWidget->setColumnWidth ( 2, 100 );
    treeWidget->setColumnWidth ( 3, 150 );

    serverNameEdit->setEnabled ( false );
    serverAetitleEdit->setEnabled ( false );
    serverPortEdit->setEnabled ( false );
    serverHostnameEdit->setEnabled ( false );
    removeButton->setEnabled ( false );
    echoButton->setEnabled ( false );


    this->initConnections();
}

QtDcmServersDicomSettingsWidget::~QtDcmServersDicomSettingsWidget()
{
}

void QtDcmServersDicomSettingsWidget::initConnections()
{
    QObject::connect ( treeWidget, &QTreeWidget::currentItemChanged, 
                       this,       &QtDcmServersDicomSettingsWidget::itemSelected);
    QObject::connect ( serverNameEdit,     &QLineEdit::textChanged, 
                       this,               &QtDcmServersDicomSettingsWidget::serverNameChanged);
    QObject::connect ( serverHostnameEdit, &QLineEdit::textChanged, 
                       this,               &QtDcmServersDicomSettingsWidget::serverHostnameChanged );
    QObject::connect ( serverAetitleEdit,  &QLineEdit::textChanged, 
                       this,               &QtDcmServersDicomSettingsWidget::serverAetitleChanged );
    QObject::connect ( serverPortEdit,     &QLineEdit::textChanged, 
                       this,               &QtDcmServersDicomSettingsWidget::serverPortChanged );
    QObject::connect ( addButton,    &QPushButton::clicked, 
                       this,         &QtDcmServersDicomSettingsWidget::addServer);
    QObject::connect ( removeButton, &QPushButton::clicked, 
                       this,         &QtDcmServersDicomSettingsWidget::removeServer);
    QObject::connect ( echoButton,   &QPushButton::clicked, 
                       this,         &QtDcmServersDicomSettingsWidget::sendEcho);
}

void QtDcmServersDicomSettingsWidget::readPreferences()
{
    QtDcmPreferences* prefs = QtDcmPreferences::instance();
    treeWidget->clear();
    
    for ( int i = 0; i < prefs->servers().size(); i++ )
    {
        QTreeWidgetItem * item = new QTreeWidgetItem ( treeWidget );
        item->setText ( 0, prefs->servers().at ( i ).name() );
        item->setData ( 0, 1, QVariant ( prefs->servers().at ( i ).name() ) );
        
        item->setText ( 1, prefs->servers().at ( i ).aetitle() );
        item->setData ( 1, 1, QVariant ( prefs->servers().at ( i ).aetitle() ) );
        
        item->setText ( 2, prefs->servers().at ( i ).port() );
        item->setData ( 2, 1, QVariant ( prefs->servers().at ( i ).port() ) );
        
        item->setText ( 3, prefs->servers().at ( i ).address() );
        item->setData ( 3, 1, QVariant ( prefs->servers().at ( i ).address() ) );
        
        item->setData ( 4, 1, QVariant ( i ) );
    }
}

void QtDcmServersDicomSettingsWidget::updatePreferences()
{   
    QtDcmPreferences* prefs = QtDcmPreferences::instance();
    QTreeWidgetItem * root = treeWidget->invisibleRootItem();
    
    QList<QtDcmServer> servers;
    for (int i = 0; i < root->childCount() ; i++) {
        QtDcmServer server;
        server.setName(root->child ( i )->data ( 0, 1 ).toString());
        server.setAetitle(root->child ( i )->data ( 1, 1 ).toString());
        server.setPort(root->child ( i )->data ( 2, 1 ).toString());
        server.setAddress(root->child ( i )->data ( 3, 1 ).toString());
        servers << server;
    }
    
    prefs->setServers(servers);
}

void QtDcmServersDicomSettingsWidget::addServer()
{
    QtDcmPreferences* prefs = QtDcmPreferences::instance();
    QTreeWidgetItem * item = new QTreeWidgetItem ( treeWidget );
    QtDcmServer server;
    server.setName("Name");
    server.setAetitle("AETITLE");
    server.setPort("2010");
    server.setAddress("hostname");
    
    item->setText ( 0, server.name() );
    item->setData ( 0, 1, QVariant ( server.name() ) );
    
    item->setText ( 1, server.aetitle() );
    item->setData ( 1, 1, QVariant ( server.aetitle() ) );
    
    item->setText ( 2, server.port() );
    item->setData ( 2, 1, QVariant ( server.port().toInt() ) );
    
    item->setText ( 3, server.address() );
    item->setData ( 3, 1, QVariant ( server.address() ) );
    
    item->setData ( 4, 1, QVariant ( prefs->servers().size() - 1 ) );
    
    prefs->addServer(server);
}

void QtDcmServersDicomSettingsWidget::removeServer()
{
    QTreeWidgetItem * root = treeWidget->invisibleRootItem();
    if (!treeWidget->currentItem()) {
        return;
    }
    
    const int index = root->indexOfChild ( treeWidget->currentItem());
    QTreeWidgetItem * item = treeWidget->currentItem();
    root->removeChild(item);
    delete item;
    
    QtDcmPreferences::instance()->removeServer ( index );

    if ( root->childCount() == 0 ) {
        echoButton->setEnabled ( false );
        removeButton->setEnabled ( false );
    }
}

void QtDcmServersDicomSettingsWidget::itemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    echoButton->setEnabled ( true );
    removeButton->setEnabled ( true );
    serverNameEdit->setEnabled ( true );
    serverAetitleEdit->setEnabled ( true );
    serverPortEdit->setEnabled ( true );
    serverHostnameEdit->setEnabled ( true );
    serverNameEdit->setText ( current->data ( 0, 1 ).toString() );
    serverAetitleEdit->setText ( current->data ( 1, 1 ).toString() );
    serverPortEdit->setText ( current->data ( 2, 1 ).toString() );
    serverHostnameEdit->setText ( current->data ( 3, 1 ).toString() );
}

void QtDcmServersDicomSettingsWidget::serverAetitleChanged ( const QString & text )
{
    QRegExp rexp ( "[A-Z0-9._-]{1,50}" );
    QRegExpValidator * valid = new QRegExpValidator ( rexp, this );
    serverAetitleEdit->setValidator ( valid );
    treeWidget->currentItem()->setText ( 1, text );
    treeWidget->currentItem()->setData ( 1, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::serverHostnameChanged (const QString &text)
{
    treeWidget->currentItem()->setText ( 3, text );
    treeWidget->currentItem()->setData ( 3, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::serverNameChanged (const QString &text)
{
    treeWidget->currentItem()->setText ( 0, text );
    treeWidget->currentItem()->setData ( 0, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::serverPortChanged (const QString &text)
{
    QIntValidator * valid = new QIntValidator ( 1000, 100000, this );
    serverPortEdit->setValidator ( valid );
    treeWidget->currentItem()->setText ( 2, text );
    treeWidget->currentItem()->setData ( 2, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::sendEcho()
{
    if ( !treeWidget->currentItem() )
        return;

    QtDcmPreferences* prefs = QtDcmPreferences::instance();
    
    const QString aet = prefs->aetitle();
    const QString serverAet = treeWidget->currentItem()->data ( 1, 1 ).toString();
    const QString hostname = prefs->hostname();
    const QString serverHostname = treeWidget->currentItem()->data ( 3, 1 ).toString();
    const QString serverPort = treeWidget->currentItem()->data ( 2, 1 ).toString();

    T_ASC_Network *net = 0; // network struct, contains DICOM upper layer FSM etc.

    OFCondition cond = ASC_initializeNetwork ( NET_REQUESTOR, 0, 30 /* timeout */, &net );
    if ( cond != EC_Normal )
    {
        QMessageBox msgBox( QApplication::activeWindow() );
        msgBox.setIcon ( QMessageBox::Critical );
        msgBox.setText ( "Cannot initialize network" );
        msgBox.exec();
        
        ASC_dropNetwork ( &net );
        
        return;
    }

    QTcpSocket * socket = new QTcpSocket(this);
    socket->connectToHost(serverHostname, serverPort.toInt());
    if (!socket->waitForConnected(1000)) {
        QMessageBox msgBox( QApplication::activeWindow() );
        msgBox.setIcon ( QMessageBox::Information );
        msgBox.setText ( "Cannot connect to server " + serverHostname + " on port " + serverPort + " !" );
        msgBox.exec();
        
        ASC_dropNetwork ( &net );
        
        return;
    }
    
    socket->disconnectFromHost();
    delete socket;
    
    T_ASC_Parameters *params; // parameters of association request

    cond = ASC_createAssociationParameters ( &params, ASC_DEFAULTMAXPDU );

    // set calling and called AE titles
    cond = ASC_setAPTitles ( params, aet.toUtf8().data(), serverAet.toUtf8().data(), NULL );

    // the DICOM server accepts connections at server.nowhere.com port 104
    cond = ASC_setPresentationAddresses ( params, hostname.toUtf8().data(), QString ( serverHostname + ":" + serverPort ).toLatin1().data() );

    // list of transfer syntaxes, only a single entry here
    const char* ts[] = { UID_LittleEndianImplicitTransferSyntax };

    // add presentation context to association request
    cond = ASC_addPresentationContext ( params, 1, UID_VerificationSOPClass, ts, 1 );

    // request DICOM association
    T_ASC_Association *assoc = 0;

    if ( ASC_requestAssociation ( net, params, &assoc ).good() )
    {
        if ( ASC_countAcceptedPresentationContexts ( params ) == 1 )
        {
            // the remote SCP has accepted the Verification Service Class
            DIC_US id = assoc->nextMsgID++; // generate next message ID
            DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
            DcmDataset *sd = NULL; // status detail will be stored here
            // send C-ECHO-RQ and handle response
            DIMSE_echoUser ( assoc, id, DIMSE_BLOCKING, 0, &status, &sd );
            delete sd; // we don't care about status detail
            
            QMessageBox msgBox( QApplication::activeWindow() );
            msgBox.setIcon ( QMessageBox::Information );
            msgBox.setText ( "Echo request successful !" );
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox( QApplication::activeWindow() );
            msgBox.setIcon ( QMessageBox::Critical );
            msgBox.setText ( "Wrong presentation context, echo request failed" );
            msgBox.exec();
        }
    }
    else
    {
        QMessageBox msgBox( QApplication::activeWindow() );
        msgBox.setIcon ( QMessageBox::Critical );
        msgBox.setText ( "Wrong dicom association, echo request failed" );
        msgBox.exec();
    }

    ASC_releaseAssociation ( assoc ); // release association
    ASC_destroyAssociation ( &assoc ); // delete assoc structure
    ASC_dropNetwork ( &net ); // delete net structure
    
    net = 0;
    assoc = 0;
}
