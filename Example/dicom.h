#ifndef DICOM_H
#define DICOM_H

/*!
 * \file QtM3d.h
 * \brief Main class for the QtM3DViewer application
 * \author Alexandre Abadie
 * \version 0.1
 */

#include <QtGui>

#include "ui_dicom.h"

class Dicom : public QMainWindow
  {
  Q_OBJECT
  private:

  public:
    Ui::Dicom ui;

    Dicom( QWidget *parent = 0 );

    void
    initConnections();

  public slots:
    void
    openDicomdir();
    void
    exportSerie();
    void
    preferences();
  };
#endif
