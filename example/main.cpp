/*!
 * \file main.cpp
 * \brief Main for the example application
 * \author Alexandre Abadie
 * \version 2.0
 */

#include <QApplication>
#include <QtGui>
#include "dicom.h"
#include <QtDcmManager.h>

#define NO_COLOR       "\x1b[0m"
#define DEBUG_COLOR    "\033[33m"
#define INFO_COLOR     "\033[36m"
#define WARNING_COLOR  "\033[33m"
#define CRITICAL_COLOR "\033[31;1m"
#define MESSAGE_COLOR  "\033[0m"

int main( int argc , char *argv[] )
{
    QString messagePattern = QString("%1[%{time yyyyMMdd h:mm:ss.zzz}] "
                                     "%{if-debug}%2DEB %{endif}%{if-info}%3%{endif}%{if-warning}%4WAR %{endif}%{if-critical}%5CRIT%{endif} %1"
                                     "%3 %{file}:%{line} %6 %{message} %1")
            .arg(NO_COLOR)
            .arg(DEBUG_COLOR)
            .arg(INFO_COLOR)
            .arg(WARNING_COLOR)
            .arg(CRITICAL_COLOR)
            .arg(MESSAGE_COLOR);
    
    qSetMessagePattern(messagePattern);
    QApplication app(argc, argv);
    
    Dicom dcm;
    dcm.setWindowIcon(QIcon(":Images/dicom.xpm"));
    dcm.show();
    
    int ret = app.exec();
    QtDcmManager::destroy();
    
    return ret;
}
