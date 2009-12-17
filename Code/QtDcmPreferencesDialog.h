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
#include <QtDcmPreferences.h>

/*
 *
 */
class QtDcmPreferencesDialog : public QDialog
  {
  Q_OBJECT
  private:
    QtDcmPreferences * _preferences;

    void
    initConnections();

  public:
    Ui::PreferencesDialog widget;
    QtDcmPreferencesDialog( QWidget * parent );
    virtual
    ~QtDcmPreferencesDialog()
      {
      }
    ;

    QtDcmPreferences *
    getPreferences()
      {
        return _preferences;
      }

    void
    setPreferences( QtDcmPreferences * prefs );

    void
    updatePreferences();

  public slots:
    void
    itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous );
    void
    serverHostnameChanged( QString text );
    void
    serverNameChanged( QString text );
    void
    serverAetitleChanged( QString text );
    void
    serverPortChanged( QString text );
    void
    removeServer();
    void
    addServer();
    void
    setDcm2nii();
    void
    setDcm4che();
  };

#endif /* QTDCMPREFERENCESDIALOG_H_ */
