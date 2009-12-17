/*
 * QtDcmDateDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmDateDialog.h>

QtDcmDateDialog::QtDcmDateDialog( QWidget * parent )
  {
    widget.setupUi(this);
    this->setModal(true);
    this->setParent(parent,Qt::Dialog);
    this->initConnections();
  }

void QtDcmDateDialog::initConnections()
  {
    QObject::connect(widget.calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(setDate(QDate)));
  }

void QtDcmDateDialog::setDate(const QDate & date)
  {
    _date = date;
  }
