/*
 * QtDcmPreferencesDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmPreferences.h>
#include <QtDcmPreferencesDialog.h>

QtDcmPreferencesDialog::QtDcmPreferencesDialog ( QWidget * parent )
{
    setupUi ( this );
    this->setModal ( true );
    this->setParent ( parent, Qt::Dialog );
}

QtDcmPreferencesWidget* QtDcmPreferencesDialog::getWidget()
{
    return preferencesWidget;
}
