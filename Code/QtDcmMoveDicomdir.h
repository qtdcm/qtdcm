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
    QtDcmMoveDicomdir ( QObject * parent );
    virtual ~QtDcmMoveDicomdir();

    enum mode
    {
        IMPORT, PREVIEW
    };

    void setMode ( mode mode );

    QtDcmMoveDicomdir::mode getMode();

    void setDcmItem ( DcmItem * item );

    void setOutputDir ( QString dir );

    void setImportDir ( QString dir );

    void setSeries ( QList<QString> series );

    void setIndex ( int index );

    void run();

signals:
    void updateProgress ( int i );
    void previewSlice ( QString filename );
    void serieMoved(QString directory);

private:
    QString fixFilename ( QString name );

    QtDcmMoveDicomdirPrivate * d;
};

#endif /* QTDCMMOVEDICOMDIR_H_ */
