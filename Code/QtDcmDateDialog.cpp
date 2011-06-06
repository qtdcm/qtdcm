/*
 * QtDcmDateDialog.cpp
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#include <QtDcmDateDialog.h>

class QtDcmDateDialogPrivate
{
    public:
        QDate date;
};

QtDcmDateDialog::QtDcmDateDialog(QWidget * parent) :
    d(new QtDcmDateDialogPrivate)
{
    this->setupUi(this);
    this->setModal(true);
    this->setParent(parent, Qt::Dialog);
    this->initConnections();
}

void
QtDcmDateDialog::initConnections()
{
    QObject::connect(calendarWidget, SIGNAL(clicked(QDate)), this, SLOT(setDate(QDate)));
    QObject::connect(calendarWidget, SIGNAL(currentPageChanged(int,int)), this, SLOT(yearMonthChanged(int,int)));
}

void
QtDcmDateDialog::yearMonthChanged(int year, int month)
{
//     int day = d->date.day();
    d->date.setDate(year, month, d->date.day());
    calendarWidget->setSelectedDate(d->date);
}

QDate
QtDcmDateDialog::getDate()
{
    return d->date;
}

void
QtDcmDateDialog::setDate(const QDate & date)
{
    calendarWidget->setSelectedDate(date);
    d->date = date;
}
