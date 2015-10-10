/**
 * \page integrationpage Integrating QtDcm in a Qt application
 *
 * The integration of QtDcm in a Qt application is very easy but you have to know how to configure it via QtDcmManager which is in charge of the 
 * connections between the different widgets, dicom communications, dicom images downloads and image conversions (or command line tools).
 * 
 * Important note: for the moment, QtDcm can only be integrated in Qt based applications and more precisely QtWidgets based applications.
 * 
 * \section basic_section Instanciating the objects
 *
 * In your application you have to first instanciate the widgets. Here we only show the case with the main QtDcm and the
 * QtDcmImportWidget, assuming "this" is the parent widget embedding those widgets.
 * \code
 * QtDcm * mainWidget = new QtDcm(this);
 * QtDcmImportWidget * importWidget = new QtDcmImportWidget(this);
 * \endcode
 *
 * Then you have to set the setting file. This file is stored in your personnal directory and if it doesn't exist it's created with
 * default values.
 * \code
 * QtDcmPreferences::instance()->setIniFile("mySettings");
 * \endcode
 *
 * Set the QtDcmManager class the pointers to the main widgets (QtDcm, QtDcmImportWidget for example). All the connexions are perfomed 
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
 * First there's a mode that you can set if you want to control from the code where the converted data will be stored. See QtDcmManager::outputdirmode enum.
 * If you choose QtDcmManager::DIALOG, the output directory will be set using a QFileDialog. If you choose QtDcmManager::CUSTOM, you will have
 * to set manually the output directory.
 * \code
 * QtDcmManager::instance()->setOutputdirMode(QtDcmManager::CUSTOM);
 * QtDcmManager::instance()->setOutputDirectory(myDirectory);
 * ...
 *
 * QtDcmManager::instance()->importSelectedSeries();
 * \endcode
 *
 * The other option is the choice of the provided converter : using a call to dcm2nii binary or using an ITK based conversion class.
 * \code
 * QtDcmManager::instance()->setExternalUseConverter(true);
 * ...
 * or
 * ...
 * QtDcmManager::instance()->setExternalUseConverter(false);
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
 * QtDcm * main = new QtDcm(this);
 * main->editPreferences();
 * \endcode
 * 
 * 
 * \subsection embed_settings_subsec Used the specific settings widgets
 *
 * These widgets are designed to be integrated in other applications/widgets, in fact you can just use them where you want, assuming the parent class is QWidget based. 
 * You just need to instanciate them and they will internally update the QtDcmPreferences instance but the changes won't be written to the settings file.
 * \code
 * p_servers = new QtDcmServersDicomSettingsWidget(this); // assuming p_servers is a member of the parent widget
 * \endcode
 *
 * In this way it's necessary to add somewhere in your application a way to store the settings, e.g a "Save" button connected to an onSaveClicked() slot.
 * Here is the instanciation of the button and the signal/slot connection.
 * \code
 * //instanciate a QPushButton
 * QPushButton * save = new QPushButton(this);
 * connect(save, &QPushButton::clicked, 
 *         this, &MyParentWidget::onSaveClicked);
 * \endcode
 *
 * Here is the slot implementation:
 * \code
 * void MyClass::onSaveClicked()
 * {
 *   m_servers->updatePreferences(); // Updates preferences in memory
 *   QtDcmPreferences::instance()->writeSettings(); // Effectively writes them to the settings file
 * }
 * \endcode
 * 
 */
