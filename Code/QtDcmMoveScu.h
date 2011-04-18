/*
 * QtDcmMoveScu.h
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
 */

#ifndef QTDCMMOVESCU_H_
#define QTDCMMOVESCU_H_

#include <QtGui>

class QtDcmMoveScuPrivate;

class QtDcmMoveScu : public QThread
{
    Q_OBJECT
    public:
        QtDcmMoveScu(QObject * parent);

        virtual
        ~QtDcmMoveScu();

        void
        setOutputDir(QString dir);

        void
        setSerieUID(QString uid);

        void
        run();

    private:
        QtDcmMoveScuPrivate * d;
};

#endif /* QTDCMMOVESCU_H_ */
