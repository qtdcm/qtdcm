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
}

void QtDcmPreferencesDialog::browseDcm2niiPath()
{
    qDebug() << "Browse dcm2nii path";
}

void QtDcmPreferencesDialog::toggleDcm2niiFrame(int state)
{
    dcm2niiPathFrame->setEnabled((state == Qt::Checked));
    d->preferences->useDcm2nii((state == Qt::Checked));
}

