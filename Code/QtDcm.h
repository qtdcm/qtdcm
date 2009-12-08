#ifndef QTDCM_H
#define QTDCM_H

#include <QtGui>

#include "ui_qtdcm.h"
#include <QtDcmManager.h>
//QString dcm2nii;
//#include <QtDcmConfig.h>

class QtDCM : public QLabel
  {
  Q_OBJECT
  private:
    QString _mode;
    QtDcmManager * _manager;
    QList<QString> _imagesList;

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

    void
    display();
    void
    initConnections();
    QList<QString>
    getImagesList();

  public slots:
    void
    itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous );
    //    void progressCopy(int i);
    void
    contextExportMenu( const QPoint point );
    void
    exportList();
    void
    openDicomdir();
  };

#endif

