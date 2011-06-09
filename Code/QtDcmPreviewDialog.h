/*
 * QtDcmPreviewDialog.h
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#ifndef QTDCMPREVIEWDIALOG_H_
#define QTDCMPREVIEWDIALOG_H_

#include <QtGui>
#include <ui_qtdcmpreviewdialog.h>

/*
 *
 */

class QtDcmPreviewDialog : public QDialog
{
    Q_OBJECT

private:
    QGraphicsScene * _scene;
    QList<QImage> _images;
    int _mode;

public:
    Ui::PreviewDialog widget;
    QtDcmPreviewDialog ( QWidget * parent );
    virtual ~QtDcmPreviewDialog() {}

    void setImages ( QList<QImage> images )
    {
        _images = images;
    }

    void initConnections();
    void showMode1();
    void showMode2();
    void showMode4();
    void showMode8();

public slots:
    void updatePreview();
    void tool1ButtonClicked();
    void tool2ButtonClicked();
    void tool4ButtonClicked();
    void tool8ButtonClicked();
};

#endif /* QTDCMPREVIEWDIALOG_H_ */
