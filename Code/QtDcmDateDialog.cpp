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
  }

void QtDcmDateDialog::initConnections()
  {
    QObject::connect(widget.calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(setDate(QDate)));
  }

QtDcmDateDialog::~QtDcmDateDialog()
  {
    // TODO Auto-generated destructor stub
  }

void QtDcmDateDialog::setDate(const QDate & date)
  {
    _date = date;
  }
