/*!
 * \file dicom.cpp
 * \brief Main class for the example
 * \author Alexandre Abadie
 * \version 0.1
 */

#include "dicom.h"

#include "QtDcm.h"
#include "QtDcmManager.h"
#include "QtDcmPreferences.h"

Dicom::Dicom ( QWidget *parent ) : QMainWindow ( parent )
{
    ui.setupUi ( this );
    QtDcmPreferences::instance()->setIniFile("qtdcm.ini");
    QtDcmManager::instance()->setQtDcmWidget ( ui.qtdcm );
    QtDcmManager::instance()->setSerieInfoWidget ( ui.serieInfoWidget );
    QtDcmManager::instance()->setPreviewWidget ( ui.previewWidget );
    QtDcmManager::instance()->setImportWidget ( ui.importWidget );
    QtDcmManager::instance()->setOutputdirMode ( QtDcmManager::DIALOG );

    QObject::connect ( ui.actionOpenDicomdir, &QAction::triggered, 
                       this,                  &Dicom::openDicomdir);
    QObject::connect ( ui.actionExport,       &QAction::triggered, 
                       this,                  &Dicom::exportSerie);
    QObject::connect ( ui.actionPreferences,  &QAction::triggered, 
                       this,                  &Dicom::preferences);
}

Dicom::~Dicom()
{
}

void Dicom::openDicomdir()
{
    ui.qtdcm->openDicomdir();
}

void Dicom::exportSerie()
{
    QtDcmManager::instance()->importSelectedSeries();
}

void Dicom::preferences()
{
    ui.qtdcm->editPreferences();
}
