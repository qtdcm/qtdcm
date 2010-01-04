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
    QObject::connect(widget.calendarWidget, SIGNAL(currentPageChanged(int,int)), this, SLOT(yearMonthChanged(int,int)));
  }

void QtDcmDateDialog::yearMonthChanged(int year, int month)
  {
    int day = _date.day();
    _date.setDate(year, month, _date.day());
    widget.calendarWidget->setSelectedDate(_date);
  }

void QtDcmDateDialog::setDate(const QDate & date)
  {
    widget.calendarWidget->setSelectedDate(date);
    _date = date;
  }
