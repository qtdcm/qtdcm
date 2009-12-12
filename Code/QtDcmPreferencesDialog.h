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
    ~QtDcmPreferencesDialog();

    QtDcmPreferences *
    getPreferences()
      {
        return _preferences;
      }

    void
    setPreferences( QtDcmPreferences * prefs );

    void
    updatePreferences();
  };

#endif /* QTDCMPREFERENCESDIALOG_H_ */
