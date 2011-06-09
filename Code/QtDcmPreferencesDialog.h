/*
 * QtDcmPreferencesDialog.h
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#ifndef QTDCMPREFERENCESDIALOG_H_
#define QTDCMPREFERENCESDIALOG_H_

#include <QtGui>

#include <ui_qtdcmpreferencesdialog.h>

class QtDcmPreferencesWidget;
class QtDcmPreferencesDialogPrivate;

/*
 *
 */

class QtDcmPreferencesDialog : public QDialog, public Ui::PreferencesDialog
{
    Q_OBJECT

public:
    QtDcmPreferencesDialog ( QWidget * parent );
    virtual ~QtDcmPreferencesDialog() {};

    QtDcmPreferencesWidget * getWidget();
    
//     void setPreferences ( QtDcmPreferences * prefs );
//     void updatePreferences();
};

#endif /* QTDCMPREFERENCESDIALOG_H_ */
