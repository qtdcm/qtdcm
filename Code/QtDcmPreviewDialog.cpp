/*
 * QtDcmPreviewDialog.cpp
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmPreviewDialog.h>

QtDcmPreviewDialog::QtDcmPreviewDialog( QWidget * parent )
  {
    // TODO Auto-generated constructor stub
    widget.setupUi(this);
    this->setModal(true);
    this->setParent(parent, Qt::Dialog);
    _scene = new QGraphicsScene();
    widget.graphicsView->setScene(_scene);
  }

void
QtDcmPreviewDialog::setImages( QList<QPixmap> images )
  {
    for (int i = 0; i < images.size(); i++)
      {
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem();
        item->setPos(QPoint((i%4)*160,(i-i%4)*40));
        item->setPixmap(images.at(i));
        _scene->addItem(item);
      }
  }
