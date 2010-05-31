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
    _mode = 4;
    this->initConnections();
  }

void
QtDcmPreviewDialog::initConnections()
  {
    QObject::connect(widget.tool1Button, SIGNAL(clicked()), this, SLOT(tool1ButtonClicked()));
    QObject::connect(widget.tool2Button, SIGNAL(clicked()), this, SLOT(tool2ButtonClicked()));
    QObject::connect(widget.tool4Button, SIGNAL(clicked()), this, SLOT(tool4ButtonClicked()));
    QObject::connect(widget.tool8Button, SIGNAL(clicked()), this, SLOT(tool8ButtonClicked()));
  }

void
QtDcmPreviewDialog::showMode1()
  {
    _scene->clear();
    for (int i = 0; i < _images.size(); i++)
      {
        QImage image = _images.at(i);
        image = image.scaledToWidth(512);
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem();
        item->setPos(QPoint(20, 34 + i * (34 + image.height())));
        item->setPixmap(QPixmap::fromImage(image));
        _scene->addItem(item);
      }
  }

void
QtDcmPreviewDialog::showMode2()
  {
    _scene->clear();
    for (int i = 0; i < _images.size(); i++)
      {
        QImage image = _images.at(i);
        image = image.scaledToWidth(512);
        image = image.scaledToWidth(image.width() / 2);
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem();
        item->setPos(QPoint(16 + (i % 2) * (image.width() + 6), 16 + (i - i % 2) * (3 + image.height() / 2)));
        item->setPixmap(QPixmap::fromImage(image));
        _scene->addItem(item);
      }
  }

void
QtDcmPreviewDialog::showMode4()
  {
    _scene->clear();
    _scene->update(QRectF(0,0,0,0));
    for (int i = 0; i < _images.size(); i++)
      {
        QImage image = _images.at(i);
        image = image.scaledToWidth(512);
        image = image.scaledToWidth(image.width() / 4);
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem();
        item->setPos(QPointF((i % 4) * (image.width() + 12), (i - i % 4) * (3 + image.height() / 4) ));
        item->setPixmap(QPixmap::fromImage(image));
        _scene->addItem(item);
      }
  }

void
QtDcmPreviewDialog::showMode8()
  {
    _scene->clear();
    for (int i = 0; i < _images.size(); i++)
      {
        QImage image = _images.at(i);
        image = image.scaledToWidth(512);
        image = image.scaledToWidth(image.width() / 8);
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem();
        item->setPos(QPoint((i % 8) * (image.width() + 6), (i - i % 8) *  (1 + image.height() / 8)));
        item->setPixmap(QPixmap::fromImage(image));
        _scene->addItem(item);
      }
  }

void
QtDcmPreviewDialog::updatePreview()
  {
    switch (_mode)
      {
      case 1:
        this->showMode1();
        break;
      case 2:
        this->showMode2();
        break;
      case 8:
        this->showMode8();
        break;
      default:
        this->showMode4();
      }
  }

void
QtDcmPreviewDialog::tool1ButtonClicked()
  {
    _mode = 1;
    this->updatePreview();
  }

void
QtDcmPreviewDialog::tool2ButtonClicked()
  {
    _mode = 2;
    this->updatePreview();
  }

void
QtDcmPreviewDialog::tool4ButtonClicked()
  {
    _mode = 4;
    this->updatePreview();
  }

void
QtDcmPreviewDialog::tool8ButtonClicked()
  {
    _mode = 8;
    this->updatePreview();
  }
