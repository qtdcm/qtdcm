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

#include "QtDcmDcm2niiSettingsWidget.h"

#include <QFileDialog>

#include <QtDcmPreferences.h>

QtDcmDcm2niiSettingsWidget::QtDcmDcm2niiSettingsWidget(QWidget* parent)
    : QWidget(parent)
{
    this->setupUi(this);

    QObject::connect(browseDcm2niiButton, SIGNAL(clicked()), this, SLOT(onBrowseDcm2niiPathClicked()));
    QObject::connect(dcm2niiCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onUseDcm2niiCheckChanged(int)));
}

QtDcmDcm2niiSettingsWidget::~QtDcmDcm2niiSettingsWidget()
{
}

void QtDcmDcm2niiSettingsWidget::readPreferences()
{
    dcm2niiCheckBox->setChecked(QtDcmPreferences::instance()->useDcm2nii());
    dcm2niiLineEdit->setText( QtDcmPreferences::instance()->dcm2niiPath());
}

void QtDcmDcm2niiSettingsWidget::updatePreferences()
{
    QtDcmPreferences::instance()->setDcm2niiPath(dcm2niiLineEdit->text());
    QtDcmPreferences::instance()->setUseDcm2nii(dcm2niiCheckBox->isChecked());
}

void QtDcmDcm2niiSettingsWidget::onBrowseDcm2niiPathClicked()
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
        QtDcmPreferences::instance()->setDcm2niiPath(filename);
        dcm2niiLineEdit->setText(filename);
    }
}

void QtDcmDcm2niiSettingsWidget::onUseDcm2niiCheckChanged(int state)
{
    dcm2niiPathFrame->setEnabled((state == Qt::Checked));
    QtDcmPreferences::instance()->setUseDcm2nii((state == Qt::Checked));
}



