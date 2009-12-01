#ifndef QTDCM_H
#define QTDCM_H

#include <QtGui>

#include "ui_qtdcm.h"

class QtDCM : public QLabel
  {
    Q_OBJECT
  private:
  public:
    Ui::QtDCM ui;
    QtDCM(QWidget *parent = 0);
};

#endif

