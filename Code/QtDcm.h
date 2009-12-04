#ifndef QTDCM_H
#define QTDCM_H

#include <QtGui>

#include "ui_qtdcm.h"
#include <QtDcmManager.h>

class QtDCM : public QLabel
  {
  Q_OBJECT
  private:
    QString _mode;
    QtDcmManager * _manager;

  public:
    Ui::QtDCM widget;
    QtDCM( QWidget *parent = 0 );

    QTreeView *
    getTreeView()
      {
        return widget.treeWidget;
      }

    QtDcmManager *
    getManager()
      {
        return _manager;
      }

    void display();

  };

#endif

