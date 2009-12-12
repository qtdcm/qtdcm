#ifndef QTDCM_H
#define QTDCM_H

#include <QtGui>

#include "ui_qtdcm.h"
#include <QtDcmDateDialog.h>
#include <QtDcmPreferencesDialog.h>
#include <QtDcmManager.h>

class QtDCM : public QLabel
  {
  Q_OBJECT
  private:
    QString _mode;
    QtDcmManager * _manager;
    QList<QString> _imagesList;
    QDate _beginDate, _endDate;


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
    updateDateButtons( int index );
    void
    chooseBeginDate();
    void
    chooseEndDate();
    void
    itemSelected( QTreeWidgetItem* current , QTreeWidgetItem* previous );
    //    void progressCopy(int i);
    void
    contextExportMenu( const QPoint point );
    void
    exportList();
    void
    openDicomdir();
    void
    editPreferences();
  };

#endif

