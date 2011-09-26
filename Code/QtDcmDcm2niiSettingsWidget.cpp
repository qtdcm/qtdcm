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


#include "QtDcmDcm2niiSettingsWidget.h"
#include <QtDcmPreferences.h>

class QtDcmDcm2niiSettingsWidgetPrivate
{
public:
    QtDcmPreferences * preferences;
};

QtDcmDcm2niiSettingsWidget::QtDcmDcm2niiSettingsWidget(QWidget* parent): QWidget(parent), d(new QtDcmDcm2niiSettingsWidgetPrivate)
{
    this->setupUi(this);
    d->preferences = NULL;

    QObject::connect(browseDcm2niiButton, SIGNAL(clicked()), this, SLOT(browseDcm2niiPath()));
    QObject::connect(dcm2niiCheckBox, SIGNAL(stateChanged(int)), this, SLOT(toggleDcm2niiFrame(int)));
}

QtDcmPreferences* QtDcmDcm2niiSettingsWidget::getPreferences()
{
    return d->preferences;
}

void QtDcmDcm2niiSettingsWidget::setPreferences(QtDcmPreferences* prefs)
{
    d->preferences = prefs;

    dcm2niiCheckBox->setChecked(d->preferences->useDcm2nii());
    dcm2niiLineEdit->setText( d->preferences->getDcm2niiPath());
}

void QtDcmDcm2niiSettingsWidget::updatePreferences()
{
    d->preferences->setDcm2niiPath(dcm2niiLineEdit->text());
    d->preferences->useDcm2nii (dcm2niiCheckBox->isChecked());

    d->preferences->writeSettings();
}

void QtDcmDcm2niiSettingsWidget::browseDcm2niiPath()
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

void QtDcmDcm2niiSettingsWidget::toggleDcm2niiFrame(int state)
{
    dcm2niiPathFrame->setEnabled((state == Qt::Checked));
    d->preferences->useDcm2nii((state == Qt::Checked));
}



