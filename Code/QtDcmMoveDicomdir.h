/*
 * QtDcmMoveDicomdir.h
 *
 *  Created on: 15 avr. 2011
 *      Author: aabadie
 */

#ifndef QTDCMMOVEDICOMDIR_H_
#define QTDCMMOVEDICOMDIR_H_

#include <QtGui>

class DcmItem;
class QtDcmMoveDicomdirPrivate;

class QtDcmMoveDicomdir : public QThread
{
    Q_OBJECT
    public:
        QtDcmMoveDicomdir(QObject * parent);
        virtual
        ~QtDcmMoveDicomdir();

        void
        setDcmItem(DcmItem * item);

        void
        setOutputDir(QString dir);

        void
        setImportDir(QString dir);

        void
        setSeries(QList<QString> series);

        void
        run();

    signals:
        void
        updateProgress(int i);

    private:
        QString
        fixFilename(QString name);

        QtDcmMoveDicomdirPrivate * d;
};

#endif /* QTDCMMOVEDICOMDIR_H_ */
