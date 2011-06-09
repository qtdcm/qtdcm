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

class QtDcmPreferences;

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

    QtDcmPreferences * getPreferences();

    void setPreferences ( QtDcmPreferences * prefs );
    void updatePreferences();

public slots:
    void itemSelected ( QTreeWidgetItem* current , QTreeWidgetItem* previous );
    void serverHostnameChanged ( QString text );
    void serverNameChanged ( QString text );
    void serverAetitleChanged ( QString text );
    void serverPortChanged ( QString text );
    void removeServer();
    void addServer();
    void setDcm2nii();
    void setDcm4che();
    void sendEcho();

signals:
    void sendEchoToPacs ( int index );

private:
    QtDcmPreferencesDialogPrivate * d;

    void initConnections();
};

#endif /* QTDCMPREFERENCESDIALOG_H_ */
