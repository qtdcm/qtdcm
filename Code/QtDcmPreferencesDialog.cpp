/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>
#include "QtDcm.h"

class QtDcmPreferencesDialogPrivate
{
public:
    QtDcmPreferences * preferences;
};

QtDcmPreferencesDialog::QtDcmPreferencesDialog ( QWidget * parent ) : d(new QtDcmPreferencesDialogPrivate)
{

    d->preferences = NULL;

    setupUi ( this );
    this->setModal ( true );
    this->setParent ( parent, Qt::Dialog );

    this->initConnections();
}

void QtDcmPreferencesDialog::initConnections()
{
    QObject::connect(browseDcm2niiButton, SIGNAL(clicked()), this, SLOT(browseDcm2niiPath()));
    QObject::connect(dcm2niiCheckBox, SIGNAL(stateChanged(int)), this, SLOT(toggleDcm2niiFrame(int)));
}

QtDcmPreferencesWidget* QtDcmPreferencesDialog::getWidget()
{
    return preferencesWidget;
}

void QtDcmPreferencesDialog::setPreferences(QtDcmPreferences* prefs)
{
    d->preferences = prefs;
    dcm2niiCheckBox->setChecked(d->preferences->useDcm2nii());
    dcm2niiLineEdit->setText( d->preferences->getDcm2niiPath());
}

void QtDcmPreferencesDialog::updatePreferences()
{
    d->preferences->setDcm2niiPath(dcm2niiLineEdit->text());
    d->preferences->useDcm2nii (dcm2niiCheckBox->isChecked());

    d->preferences->writeSettings();
}

void QtDcmPreferencesDialog::browseDcm2niiPath()
{
    // Open aa QFileDialog in directory mode.
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setFileMode(QFileDialog::ExistingFile);
#ifdef Q_OS_UNIX
    dialog->setNameFilter(tr("Dcm2nii (dcm2nii)"));
#else
    dialog->setNameFilter(tr("Dcm2nii (dcm2nii.exe)"));
#endif
    dialog->setDirectory(QDir::home().dirName());
    dialog->setWindowTitle("Choose dcm2nii path");
    QString filename;
    if (dialog->exec())
    {
        filename = dialog->selectedFiles()[0];
    }
    dialog->close();
    if (!filename.isEmpty()) // A file has been chosen
    {
        // The the output directory to the manager and launch the conversion process
        d->preferences->setDcm2niiPath(filename);
        dcm2niiLineEdit->setText(filename);
    }
}

void QtDcmPreferencesDialog::toggleDcm2niiFrame(int state)
{
    dcm2niiPathFrame->setEnabled((state == Qt::Checked));
    d->preferences->useDcm2nii((state == Qt::Checked));
}

