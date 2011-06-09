/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmServer.h>
#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>

class QtDcmPreferencesDialogPrivate
{

public:
    QtDcmPreferences * preferences;
};

QtDcmPreferencesDialog::QtDcmPreferencesDialog ( QWidget * parent ) : d ( new QtDcmPreferencesDialogPrivate )
{
    setupUi ( this );
    this->setModal ( true );
    this->setParent ( parent, Qt::Dialog );

    treeWidget->setColumnWidth ( 1, 100 );
    treeWidget->setColumnWidth ( 2, 100 );
    treeWidget->setColumnWidth ( 3, 150 );

    serverNameEdit->setEnabled ( false );
    serverAetitleEdit->setEnabled ( false );
    serverPortEdit->setEnabled ( false );
    serverHostnameEdit->setEnabled ( false );
    removeButton->setEnabled ( false );

    this->initConnections();
}

void QtDcmPreferencesDialog::initConnections()
{
    QObject::connect ( treeWidget, SIGNAL ( currentItemChanged ( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT ( itemSelected ( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
    QObject::connect ( serverNameEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverNameChanged ( QString ) ) );
    QObject::connect ( serverHostnameEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverHostnameChanged ( QString ) ) );
    QObject::connect ( serverAetitleEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverAetitleChanged ( QString ) ) );
    QObject::connect ( serverPortEdit, SIGNAL ( textChanged ( QString ) ), this, SLOT ( serverPortChanged ( QString ) ) );
    QObject::connect ( addButton, SIGNAL ( clicked() ), this, SLOT ( addServer() ) );
    QObject::connect ( removeButton, SIGNAL ( clicked() ), this, SLOT ( removeServer() ) );
    QObject::connect ( dcm2niiButton, SIGNAL ( clicked() ), this, SLOT ( setDcm2nii() ) );
    QObject::connect ( dcm4cheButton, SIGNAL ( clicked() ), this, SLOT ( setDcm4che() ) );
    QObject::connect ( echoButton, SIGNAL ( clicked() ), this, SLOT ( sendEcho() ) );
}

void QtDcmPreferencesDialog::sendEcho()
{
    if ( treeWidget->currentItem() )
        emit sendEchoToPacs ( treeWidget->currentItem()->data ( 4, 1 ).toInt() );
}

QtDcmPreferences * QtDcmPreferencesDialog::getPreferences()
{
    return d->preferences;
}

void QtDcmPreferencesDialog::setPreferences ( QtDcmPreferences * prefs )
{
    d->preferences = prefs;
    dcm2niiPathEdit->setText ( d->preferences->getDcm2nii() );
    dcm4chePathEdit->setText ( d->preferences->getDcm4che() );
    localAetitleEdit->setText ( d->preferences->getAetitle() );
    localPortEdit->setText ( d->preferences->getPort() );
    localHostnameEdit->setText ( d->preferences->getHostname() );

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

    QObject::connect ( this, SIGNAL ( sendEchoToPacs ( int ) ), d->preferences, SLOT ( sendEcho ( int ) ) );
}

void QtDcmPreferencesDialog::updatePreferences()
{
    d->preferences->setDcm2nii ( dcm2niiPathEdit->text() );
    d->preferences->setDcm4che ( dcm4chePathEdit->text() );
    d->preferences->setAetitle ( localAetitleEdit->text() );
    d->preferences->setPort ( localPortEdit->text() );
    d->preferences->setHostname ( localHostnameEdit->text() );
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

void QtDcmPreferencesDialog::itemSelected ( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
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

void QtDcmPreferencesDialog::serverNameChanged ( QString text )
{
    treeWidget->currentItem()->setText ( 0, text );
    treeWidget->currentItem()->setData ( 0, 1, QVariant ( text ) );
}

void QtDcmPreferencesDialog::serverAetitleChanged ( QString text )
{
    QRegExp rexp ( "[A-Z0-9._-]{1,50}" );
    QRegExpValidator * valid = new QRegExpValidator ( rexp, this );
    serverAetitleEdit->setValidator ( valid );
    treeWidget->currentItem()->setText ( 1, text );
    treeWidget->currentItem()->setData ( 1, 1, QVariant ( text ) );
}

void QtDcmPreferencesDialog::serverPortChanged ( QString text )
{
    QIntValidator * valid = new QIntValidator ( 1000, 100000, this );
    serverPortEdit->setValidator ( valid );
    treeWidget->currentItem()->setText ( 2, text );
    treeWidget->currentItem()->setData ( 2, 1, QVariant ( text ) );
}

void QtDcmPreferencesDialog::serverHostnameChanged ( QString text )
{
    treeWidget->currentItem()->setText ( 3, text );
    treeWidget->currentItem()->setData ( 3, 1, QVariant ( text ) );
}

void QtDcmPreferencesDialog::addServer()
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

void QtDcmPreferencesDialog::removeServer()
{
    QTreeWidgetItem * root = treeWidget->invisibleRootItem();
    d->preferences->removeServer ( root->indexOfChild ( treeWidget->currentItem() ) );

    if ( root->childCount() == 0 )
        removeButton->setEnabled ( false );
    else if ( root->childCount() == 1 )
    {
        treeWidget->reset();
        treeWidget->clear();
        removeButton->setEnabled ( false );
    }
    else
        root->removeChild ( treeWidget->currentItem() );
}

void QtDcmPreferencesDialog::setDcm2nii()
{
    // Open aa QFileDialog in directory mode.
    QFileDialog * dialog = new QFileDialog ( this );
    dialog->setFileMode ( QFileDialog::ExistingFile );
#ifdef Q_OS_UNIX
    dialog->setNameFilter ( tr ( "Dcm2nii (dcm2nii)" ) );
#else
    dialog->setNameFilter ( tr ( "Dcm2nii (dcm2nii.exe)" ) );
#endif
    dialog->setDirectory ( QDir::home().dirName() );
    dialog->setWindowTitle ( tr ( "Choose dcm2nii binary" ) );
    QString filename;

    if ( dialog->exec() )
    {
        filename = dialog->selectedFiles() [0];
    }

    dialog->close();

    if ( !filename.isEmpty() ) // A file has been chosen
    {
        // The the output directory to the manager and launch the conversion process
        d->preferences->setDcm2nii ( filename );
        dcm2niiPathEdit->setText ( filename );
    }
}

void QtDcmPreferencesDialog::setDcm4che()
{
    // Open aa QFileDialog in directory mode.
    QFileDialog * dialog = new QFileDialog ( this );
    dialog->setFileMode ( QFileDialog::ExistingFile );
#ifdef Q_OS_UNIX
    dialog->setNameFilter ( tr ( "Dcm4che (dcmqr)" ) );
#else
    dialog->setNameFilter ( tr ( "Dcm4che (dcmqr.bat)" ) );
#endif
    dialog->setDirectory ( QDir::home().dirName() );
    dialog->setWindowTitle ( tr ( "Choose dcmqr binary" ) );
    QString filename;

    if ( dialog->exec() )
    {
        filename = dialog->selectedFiles() [0];
    }

    dialog->close();

    if ( !filename.isEmpty() ) // A file has been chosen
    {
        // The the output directory to the manager and launch the conversion process
        d->preferences->setDcm4che ( filename );
        dcm4chePathEdit->setText ( filename );
    }
}
