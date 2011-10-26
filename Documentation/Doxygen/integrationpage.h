/**
 * \page integrationpage Integrating QtDcm in a Qt application
 *
 * The integration of QtDcm in a Qt application is very easy but you have to know how to configure the QtDcmManager class. This
 * class manage the connections between widgets, dicom communication classes and conversion classes (or command line tools).
 * 
 * \section basic_section Instanciating the objects
 *
 * In your application you have to first instanciate the widgets. Here we only show the case with the main QtDcm and the
 * QtDcmImportWidget.
 * \code
 * QtDcm * mainWidget = new QtDcm;
 * QtDcmImportWidget * importWidget = new QtDcmImportWidget;
 * \endcode
 *
 * Then you have to set the setting file. This file is stored in your personnal directory and if it doesn't exist it's created with
 * default values.
 * \code
 * QtDcmPreferences::instance()->setIniFile("mySettings");
 * \endcode
 *
 * Then you have to give the QtDcmManager class (note that this is a singleton) the pointers to the previous widgets. QtDcmManager
 * should always be called using the QtDcmManager::instance() method. When setting the widgets, all the connexions are perfomed
 * internally by QtDcmManager.
 *
 * \code
 * QtDcmManager::instance()->setQtDcmWidget(mainWidget);
 * QtDcmManager::instance()->setImportWidget(importWidget());
 * \endcode
 *
 * \section manager_conf_sec QtDcmManager configuration options
 *
 * Depending on what you planned to do with this library, it's possible to set behaviour options to QtDcmManger. This behaviour mostly relies on
 * the way dicom data are converted and where they are stored.
 * First there's a mode that you can set if you want to control in the code where the converted data are stored. See QtDcmManager::outputdirmode enum.
 * If you choose QtDcmManager::DIALOG, the output directory will be set using a QFileDialog. If you choose QtDcmManager::CUSTOM, you will also have
 * to set manually the output directory.
 * \code
 * QtDcmManager::instance()->setOutputdirMode(QtDcmManager::CUSTOM);
 * QtDcmManager::instance()->setOutputDirectory(myDirectory);
 * ...
 *
 * QtDcmManager::instance()->importSelectedSeries();
 * \endcode
 *
 * The other useful option is the choice between using the provided converter (call to dcm2nii binary or ITK based conversion class).
 * \code
 * QtDcmManager::instance()->useConverter(true);
 * ...
 * or
 * ...
 * QtDcmManager::instance()->useConverter(false);
 * \endcode
 * 
 * 
 * \section setting_sec Playing with the settings widgets
 * There a lots of way to play with settings. By default, the settings are used to configure dicom related stuff, like local dicom settings
 * (AETitle, port, IP) or available dicom servers. There are also settings for the Dcm2nii conversion tool (if you want to use it).
 *
 * \subsection easy_settings_subsec The easiest way: calling the QtDcmPreferencesDialog class
 * You just need to call the slot QtDcm::editPreferences()
 * \code
 * QtDcm * main = new QtDcm;
 * main->editPreferences();
 * \endcode
 * 
 * 
 * \subsection embed_settings_subsec Used the specific settings widgets
 *
 * Theses widgets are designed to be integrate in large applications, in fact you can just add where you want. First you need to instanciate
 * them and to give them he pointer to the instance of QtDcmPreferences.
 * \code
 * QtDcmServersDicomSettingsWidget * servers = new QtDcmServersDicomSettingsWidget;
 * servers->setPreferences(QtDcmPreferences::instance()); 
 * \endcode
 *
 * In this way it's necessary to add somewhere in your application a way to store the settings. For example a "Save" button. The clicked()
 * signal of this button will be connected with a onSaveClicked() slot that will be in charge to update the settings.
 * Here is the instanciation of the button and the signal/slot connection.
 * \code
 * //instanciate a QPushButton
 * QPushButton * save = new QPushButton;
 * QObject::connect(save, SIGNAL(clicked), this, SLOT(onSaveClicked()));
 * \endcode
 *
 * Here is the slot implementation:
 * \code
 * void MyClass::onSaveClicked()
 * {
 *  this->servers->updatePreferences(); //"This" is a bit tricky but this is just an example...
 * }
 * \endcode
 * 
 */