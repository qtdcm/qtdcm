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

/*
 *
 */
class QtDcmPreferencesDialog : public QDialog
  {
    Q_OBJECT
  public:
    QtDcmPreferencesDialog();
    virtual
    ~QtDcmPreferencesDialog();
  };

#endif /* QTDCMPREFERENCESDIALOG_H_ */
