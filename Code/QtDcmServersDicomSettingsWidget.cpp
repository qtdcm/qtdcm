/*
    <one line to give the library's name and an idea of what it does.>
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

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CSTDARG
// From Dcmtk:
#include <dcmtk/config/osconfig.h>    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/dcmdata/dctk.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include "dcmtk/dcmnet/dfindscu.h"
#include <dcmtk/dcmdata/dcistrmz.h>    /* for dcmZlibExpectRFC1950Encoding */
// For dcm images
#include <dcmtk/dcmimgle/dcmimage.h>
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmjpeg/djdecode.h"     /* for dcmjpeg decoders */
#include "dcmtk/dcmjpeg/dipijpeg.h"     /* for dcmimage JPEG plugin */
// For color images
#include <dcmtk/dcmimage/diregist.h>

//#define INCLUDEd->CSTDLIB
//#define INCLUDEd->CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"
#endif

#include <QtDcmPreferences.h>
#include <QtDcmServer.h>

class QtDcmServersDicomSettingsWidgetPrivate
{
public:
    QtDcmPreferences * preferences;
};

QtDcmServersDicomSettingsWidget::QtDcmServersDicomSettingsWidget ( QWidget* parent ) : QWidget ( parent ), d ( new QtDcmServersDicomSettingsWidgetPrivate )
{
    this->setupUi ( this );

    d->preferences = NULL;

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

void QtDcmServersDicomSettingsWidget::initConnections()
{
    QObject::connect ( treeWidget, SIGNAL ( currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT ( itemSelected ( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
    QObject::connect ( serverNameEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverNameChanged ( QString ) ) );
    QObject::connect ( serverHostnameEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverHostnameChanged ( QString ) ) );
    QObject::connect ( serverAetitleEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverAetitleChanged ( QString ) ) );
    QObject::connect ( serverPortEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverPortChanged ( QString ) ) );
    QObject::connect ( addButton, SIGNAL ( clicked() ), this, SLOT ( addServer() ) );
    QObject::connect ( removeButton, SIGNAL ( clicked() ), this, SLOT ( removeServer() ) );
    QObject::connect ( echoButton, SIGNAL ( clicked() ), this, SLOT ( sendEcho() ) );
}

QtDcmPreferences* QtDcmServersDicomSettingsWidget::getPreferences()
{
    return d->preferences;
}

void QtDcmServersDicomSettingsWidget::setPreferences ( QtDcmPreferences* prefs )
{
    d->preferences = prefs;

    for ( int i = 0; i < d->preferences->getServers().size(); i++ )
    {
        QTreeWidgetItem * item = new QTreeWidgetItem ( treeWidget );
        item->setText ( 0, d->preferences->getServers().at ( i )->getName() );
        item->setData ( 0, 1, QVariant ( d->preferences->getServers().at ( i )->getName() ) );
        item->setData ( 4, 1, QVariant ( i ) );
        item->setText ( 1, d->preferences->getServers().at ( i )->getAetitle() );
        item->setData ( 1, 1, QVariant ( d->preferences->getServers().at ( i )->getAetitle() ) );
        item->setText ( 2, d->preferences->getServers().at ( i )->getPort() );
        item->setData ( 2, 1, QVariant ( d->preferences->getServers().at ( i )->getPort() ) );
        item->setText ( 3, d->preferences->getServers().at ( i )->getServer() );
        item->setData ( 3, 1, QVariant ( d->preferences->getServers().at ( i )->getServer() ) );
    }
}

void QtDcmServersDicomSettingsWidget::updatePreferences()
{
    if ( d->preferences )
    {
        QTreeWidgetItem * root = treeWidget->invisibleRootItem();

        for ( int i = 0; i < d->preferences->getServers().size(); i++ )
        {
            d->preferences->getServers().at ( i )->setName ( root->child ( i )->data ( 0, 1 ).toString() );
            d->preferences->getServers().at ( i )->setAetitle ( root->child ( i )->data ( 1, 1 ).toString() );
            d->preferences->getServers().at ( i )->setPort ( root->child ( i )->data ( 2, 1 ).toString() );
            d->preferences->getServers().at ( i )->setServer ( root->child ( i )->data ( 3, 1 ).toString() );
        }

        d->preferences->writeSettings();
    }
}

void QtDcmServersDicomSettingsWidget::addServer()
{
    QTreeWidgetItem * item = new QTreeWidgetItem ( treeWidget );
    d->preferences->addServer();
    item->setText ( 0, "Name" );
    item->setData ( 0, 1, QVariant ( "Name" ) );
    d->preferences->getServers().last()->setName ( "Name" );
    item->setText ( 1, "AETITLE" );
    item->setData ( 1, 1, QVariant ( "AETITLE" ) );
    d->preferences->getServers().last()->setAetitle ( "AETITLE" );
    item->setText ( 2, "2010" );
    d->preferences->getServers().last()->setPort ( "2010" );
    item->setData ( 2, 1, QVariant ( 2010 ) );
    item->setText ( 3, "hostname" );
    item->setData ( 3, 1, QVariant ( "hostname" ) );
    item->setData ( 4, 1, QVariant ( d->preferences->getServers().size() - 1 ) );
    d->preferences->getServers().last()->setServer ( "hostname" );
}

void QtDcmServersDicomSettingsWidget::removeServer()
{
    QTreeWidgetItem * root = treeWidget->invisibleRootItem();
    d->preferences->removeServer ( root->indexOfChild ( treeWidget->currentItem() ) );

    if ( root->childCount() == 0 )
    {
        echoButton->setEnabled ( false );
        removeButton->setEnabled ( false );
    }
    else if ( root->childCount() == 1 )
    {
        treeWidget->reset();
        treeWidget->clear();
        removeButton->setEnabled ( false );
        echoButton->setEnabled ( false );
    }
    else
        root->removeChild ( treeWidget->currentItem() );
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

void QtDcmServersDicomSettingsWidget::serverAetitleChanged ( QString text )
{
    QRegExp rexp ( "[A-Z0-9._-]{1,50}" );
    QRegExpValidator * valid = new QRegExpValidator ( rexp, this );
    serverAetitleEdit->setValidator ( valid );
    treeWidget->currentItem()->setText ( 1, text );
    treeWidget->currentItem()->setData ( 1, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::serverHostnameChanged ( QString text )
{
    treeWidget->currentItem()->setText ( 3, text );
    treeWidget->currentItem()->setData ( 3, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::serverNameChanged ( QString text )
{
    treeWidget->currentItem()->setText ( 0, text );
    treeWidget->currentItem()->setData ( 0, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::serverPortChanged ( QString text )
{
    QIntValidator * valid = new QIntValidator ( 1000, 100000, this );
    serverPortEdit->setValidator ( valid );
    treeWidget->currentItem()->setText ( 2, text );
    treeWidget->currentItem()->setData ( 2, 1, QVariant ( text ) );
}

void QtDcmServersDicomSettingsWidget::sendEcho()
{
    if ( !d->preferences )
        return;

    if ( !treeWidget->currentItem() )
        return;

    QString aet = d->preferences->getAetitle();
    QString serverAet = treeWidget->currentItem()->data ( 1, 1 ).toString();
    QString hostname = d->preferences->getHostname();
    QString serverHostname = treeWidget->currentItem()->data ( 3, 1 ).toString();
    QString serverPort = treeWidget->currentItem()->data ( 2, 1 ).toString();

    T_ASC_Network *net; // network struct, contains DICOM upper layer FSM etc.

    OFCondition cond = ASC_initializeNetwork ( NET_REQUESTOR, 0, 5 /* timeout */, &net );
    if ( cond != EC_Normal )
    {
        QMessageBox * msgBox = new QMessageBox ( QApplication::activeWindow() );
        msgBox->setIcon ( QMessageBox::Critical );
        msgBox->setText ( "Cannot initialize network" );
        msgBox->exec();
        delete msgBox;
        return;
    }

    T_ASC_Parameters *params; // parameters of association request

    cond = ASC_createAssociationParameters ( &params, ASC_DEFAULTMAXPDU );

    // set calling and called AE titles
    cond = ASC_setAPTitles ( params, aet.toUtf8().data(), serverAet.toUtf8().data(), NULL );

    // the DICOM server accepts connections at server.nowhere.com port 104
    cond = ASC_setPresentationAddresses ( params, hostname.toUtf8().data(), QString ( serverHostname + ":" + serverPort ).toAscii().data() );

    // list of transfer syntaxes, only a single entry here
    const char* ts[] = { UID_LittleEndianImplicitTransferSyntax };

    // add presentation context to association request
    cond = ASC_addPresentationContext ( params, 1, UID_VerificationSOPClass, ts, 1 );

    // request DICOM association
    T_ASC_Association *assoc;

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
            QMessageBox * msgBox = new QMessageBox ( QApplication::activeWindow() );
            msgBox->setIcon ( QMessageBox::Information );
            msgBox->setText ( "Echo request successful !" );
            msgBox->exec();
            delete msgBox;
        }
        else
        {
            QMessageBox * msgBox = new QMessageBox ( QApplication::activeWindow() );
            msgBox->setIcon ( QMessageBox::Critical );
            msgBox->setText ( "Wrong presentation context, echo request failed" );
            msgBox->exec();
            delete msgBox;
        }
    }
    else
    {
        QMessageBox * msgBox = new QMessageBox ( QApplication::activeWindow() );
        msgBox->setIcon ( QMessageBox::Critical );
        msgBox->setText ( "Wrong dicom association, echo request failed" );
        msgBox->exec();
        delete msgBox;
    }

    ASC_releaseAssociation ( assoc ); // release association
    ASC_destroyAssociation ( &assoc ); // delete assoc structure
    ASC_dropNetwork ( &net ); // delete net structure
}
