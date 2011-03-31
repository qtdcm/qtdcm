/*
 * QtDcmFindScuSignalManager.cpp
 *
 *  Created on: 29 mars 2011
 *      Author: aabadie
 */

#include <QtDcmFindScuSignalManager.h>

#include <QtDcmPatient.h>
#include <QtDcmStudy.h>
#include <QtDcmSerie.h>
#include <QtDcmImage.h>

class QtDcmFindScuSignalManagerPrivate
{
    public:
        QTreeWidget * patientsTreeWidget;
        QTreeWidget * studiesTreeWidget;
        QTreeWidget * seriesTreeWidget;
};

QtDcmFindScuSignalManager::QtDcmFindScuSignalManager(QObject * parent) :
    d(new QtDcmFindScuSignalManagerPrivate)
{
    d->patientsTreeWidget = NULL;
    d->studiesTreeWidget = NULL;
    d->seriesTreeWidget = NULL;
}

QtDcmFindScuSignalManager::~QtDcmFindScuSignalManager()
{

}

void
QtDcmFindScuSignalManager::attachPatientsTreeWidget(QTreeWidget * widget)
{
    d->patientsTreeWidget = widget;
}

void
QtDcmFindScuSignalManager::attachStudiesTreeWidget(QTreeWidget * widget)
{
    d->studiesTreeWidget = widget;
}

void
QtDcmFindScuSignalManager::attachSeriesTreeWidget(QTreeWidget * widget)
{
    d->seriesTreeWidget = widget;
}

void
QtDcmFindScuSignalManager::foundPatient(QMap<QString, QString> infosMap)
{
    if (d->patientsTreeWidget) {
        QTreeWidgetItem * patientItem = new QTreeWidgetItem(d->patientsTreeWidget->invisibleRootItem());
        patientItem->setText(0, infosMap["Name"]);
        patientItem->setText(1, infosMap["ID"]);
        patientItem->setText(2, QDate::fromString(infosMap["Birthdate"], "yyyyMMdd").toString("dd/MM/yyyy"));
        patientItem->setText(3, infosMap["Sex"]);
    }
}

void
QtDcmFindScuSignalManager::foundStudy(QMap<QString, QString> infosMap)
{
    if (d->studiesTreeWidget) {
        QTreeWidgetItem * studyItem = new QTreeWidgetItem(d->studiesTreeWidget->invisibleRootItem());
        studyItem->setText(0, infosMap["Description"]);
        studyItem->setText(1, QDate::fromString(infosMap["Date"], "yyyyMMdd").toString("dd/MM/yyyy"));
        studyItem->setText(2, infosMap["ID"]);
    }
}

void
QtDcmFindScuSignalManager::foundSerie(QMap<QString, QString> infosMap)
{
    if (d->seriesTreeWidget) {
        QTreeWidgetItem * serieItem = new QTreeWidgetItem(d->seriesTreeWidget->invisibleRootItem());
        serieItem->setText(0, infosMap["Description"]);
        serieItem->setText(1, infosMap["Modality"]);
        serieItem->setText(2, QDate::fromString(infosMap["Date"], "yyyyMMdd").toString("dd/MM/yyyy"));
        serieItem->setText(3, infosMap["ID"]);
        serieItem->setData(4, 0, QVariant(infosMap["InstanceCount"]));
        serieItem->setData(5, 0, QVariant(infosMap["Institution"]));
        serieItem->setData(6, 0, QVariant(infosMap["Operator"]));
        serieItem->setCheckState(0, Qt::Unchecked);
    }
}

void
QtDcmFindScuSignalManager::foundImage(QMap<QString, QString> infosMap)
{
    if (d->seriesTreeWidget) {
        if (infosMap["InstanceCount"].toInt() > d->seriesTreeWidget->currentItem()->data(4, 0).toInt())
            d->seriesTreeWidget->currentItem()->setData(4, 0, QVariant(infosMap["InstanceCount"]));
    }
}
