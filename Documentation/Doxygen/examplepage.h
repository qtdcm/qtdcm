/**
 * \page examplepage A simple Qt example
 *
 * Here we describe the complete example available in the Example directory. The example use a gui generated with Qt Designer.
 *
 * We define a class called Dicom (why not ?)
 * 
 * \code
 *
 * class Dicom : public QMainWindow
 * {
 * Q_OBJECT
 * private:
 *
 * public:
 *   Ui::Dicom ui;
 *
 *  Dicom( QWidget *parent = 0 );
 *
 *    void
 *    initConnections();
 *
 *  public slots:
 *    void
 *    openDicomdir();
 *    void
 *    exportSerie();
 *    void
 *    preferences();
 *  };
 *
 * \endcode
 *
 * Let's take a look at the constructor:
 *
 * \code
Dicom::Dicom ( QWidget *parent ) : QMainWindow ( parent )
{
    ui.setupUi ( this );
    QtDcmPreferences::instance()->setIniFile("qtdcm.ini");
    QtDcmManager::instance()->setQtDcmWidget ( ui.qtdcm );
    QtDcmManager::instance()->setSerieInfoWidget ( ui.serieInfoWidget );
    QtDcmManager::instance()->setPreviewWidget ( ui.previewWidget );
    QtDcmManager::instance()->setImportWidget ( ui.importWidget );
    QtDcmManager::instance()->setOutputdirMode ( QtDcmManager::DIALOG );

    initConnections();
}
 * \endcode
 *
 * Now let's detail the constructor:
 *
 * First we instanciate the gui objects.
 * \code
 * ui.setupUi(this);
 * \endcode
 *
 * Then we set the settings file of qtdcm. From this file QtDcmManager will instanciate the QtDcmPreferences class.
 * \code
 * QtDcmPreferences::instance()->setIniFile("qtdcm.ini");
 * \endcode
 *
 * Then we give QtDcmManager the pointers to the specific widgets: QtDcm, QtDcmSerieInfoWidget, QtDcmPreviewWidget and QtDcmImportWidget 
 * \code  
 * QtDcmManager::instance()->setQtDcmWidget ( ui.qtdcm );
 * QtDcmManager::instance()->setSerieInfoWidget ( ui.serieInfoWidget );
 * QtDcmManager::instance()->setPreviewWidget ( ui.previewWidget );
 * QtDcmManager::instance()->setImportWidget ( ui.importWidget );
 * \endcode
 *
 * Finally, we configure the manager to set the output directory (for conversion to nifti) using a QFileDialog box. The other mode available
 * is QtDcmManager::CUSTOM
 * \code
 *  QtDcmManager::instance()->setOutputdirMode ( QtDcmManager::DIALOG );
 * \endcode
 *
 *
 * Now let's detail the class slots. I think we are ok with the initConnection() method.
 *
 * The Dicom::openDicomdir() slot is very simple because we directly call the method QtDcm::openDicomdir(). The dicomdir path can
 * be set manually using QtDcmManager::setDicomdir() and the dicomdir can be loaded using QtDcm::loadPatientsFromDicomdir()
 * \code
void
Dicom::openDicomdir()
{
    ui.qtdcm->openDicomdir();
}
 * \endcode
 *
 * The Dicom::exportSerie() slot is also very simple. We directly use the QtDcmManager::importSelectedSeries()
 *
 * \code
void
Dicom::exportSerie()
{
    QtDcmManager::instance()->importSelectedSeries();
}
 * \endcode
 *
 *
 * Finally the Dicom::preferences() slot displays a QDialog for setting the application preferences. The called method is QtDcm::editPreferences().
 * It also possible to set the preferences from an embedded widget, see QtDcmPreferencesWidget, QtDcmLocalDicomSettingsWidget and
 * QtDcmServersDicomSettingsWidget classes.
 * \code
void
Dicom::preferences()
{
    ui.qtdcm->editPreferences();
}
 * \endcode
 *
 *
 * To watch the full code, see dicom.h and dicom.cpp
 */