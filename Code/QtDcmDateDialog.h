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

/*
 *
 */
class QtDcmDateDialog : public QDialog
  {
    Q_OBJECT
  private:
    QDate _date;
  public:
    Ui::DateDialog widget;
    QtDcmDateDialog( QWidget * parent );
    virtual
    ~QtDcmDateDialog();
    QCalendarWidget * getCalendarWidget() {return widget.calendarWidget;}

    QDate
    getDate()
      {
        return _date;
      }

    void initConnections();

  public slots:
    void
    setDate( const QDate & date );
  };

#endif /* QTDCMDATEDIALOG_H_ */
