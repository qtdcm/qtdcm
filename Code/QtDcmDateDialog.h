/*
 * QtDcmDateDialog.h
 *
 *  Created on: 10 déc. 2009
 *      Author: aabadie
 */

#ifndef QTDCMDATEDIALOG_H_
#define QTDCMDATEDIALOG_H_
#include <QtGui>
#include <ui_qtdcmdatedialog.h>

class QtDcmDateDialogPrivate;

/*
 *
 */

class QtDcmDateDialog : public QDialog, public Ui::DateDialog
{
    Q_OBJECT

private:
    QtDcmDateDialogPrivate * d;

public:
    QtDcmDateDialog ( QWidget * parent );
    virtual
    ~QtDcmDateDialog() {};

    QCalendarWidget * getCalendarWidget()
    {
        return calendarWidget;
    }

    /**
     * Call this method to get the selected date in the calendar widget
     */
    QDate getDate();

    void initConnections();

public slots:
    void setDate ( const QDate & date );
    void yearMonthChanged ( int year , int month );
};

#endif /* QTDCMDATEDIALOG_H_ */
