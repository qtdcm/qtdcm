/*
 * QtDcmPreviewDialog.h
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#ifndef QTDCMPREVIEWDIALOG_H_
#define QTDCMPREVIEWDIALOG_H_

#include <QtGui>
#include <ui_qtdcmpreviewdialog.h>

/*
 *
 */
class QtDcmPreviewDialog : public QDialog
  {
   Q_OBJECT
  private:
   QGraphicsScene * _scene;
   QList<QPixmap> _images;

  public:
    Ui::PreviewDialog widget;
    QtDcmPreviewDialog(QWidget * parent);
    virtual
    ~QtDcmPreviewDialog() {}

    void
    setImages(QList<QPixmap> images);
  };

#endif /* QTDCMPREVIEWDIALOG_H_ */
