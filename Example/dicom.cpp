/*!
 * \file QtM3d.cpp
 * \brief Main class for the QtM3DViewer application
 * \author Alexandre Abadie
 * \version 0.1
 */

#include "dicom.h"

#include "QtDcm.h"
#include "QtDcmManager.h"
#include "QtDcmPreferences.h"

Dicom::Dicom ( QWidget *parent ) : QMainWindow ( parent )
{
    QtDcmPreferences::instance()->setIniFile("qtdcm.ini");
    ui.setupUi ( this );
    QtDcmManager::instance()->setQtDcmWidget ( ui.qtdcm );
    QtDcmManager::instance()->setSerieInfoWidget ( ui.serieInfoWidget );
    QtDcmManager::instance()->setPreviewWidget ( ui.previewWidget );
    QtDcmManager::instance()->setImportWidget ( ui.importWidget );
    QtDcmManager::instance()->setOutputdirMode ( QtDcmManager::DIALOG );

    initConnections();
}

void
Dicom::initConnections()
{
    QObject::connect ( ui.actionOpenDicomdir, SIGNAL ( triggered() ), this, SLOT ( openDicomdir() ) );
    QObject::connect ( ui.actionExport, SIGNAL ( triggered() ), this, SLOT ( exportSerie() ) );
    QObject::connect ( ui.actionPreferences, SIGNAL ( triggered() ), this, SLOT ( preferences() ) );
}

void
Dicom::openDicomdir()
{
    ui.qtdcm->openDicomdir();
}

void
Dicom::exportSerie()
{
    QtDcmManager::instance()->importSelectedSeries();
}

void
Dicom::preferences()
{
    ui.qtdcm->editPreferences();
}
