/*
 * QtDcmManager.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#define QT_NO_CAST_TO_ASCII
#include <QtDcmManager.h>

QtDcmManager::QtDcmManager()
  {
    //Initialization of the private attributes
    _mode = "PACS";
    _dicomdir = "";
    _outputDir = "";
    _process = new QProcess(this);
    _patientName = "*";
    _patientId = "*";
    _modality = "MR";
    _date1 = "*";
    _date2 = "*";
    _serieDescription = "*";
    _studyDescription = "*";

    _preferences = new QtDcmPreferences();
    _thread = new QtDcmExportThread();

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
  }

QtDcmManager::QtDcmManager( QWidget * parent )
  {
    //Initialization of the private attributes
    _mode = "PACS";
    _dicomdir = "";
    _outputDir = "";
    _process = new QProcess(this);
    _patientName = "*";
    _patientId = "*";
    _date1 = "*";
    _date2 = "*";
    _modality = "MR";
    _serieDescription = "*";
    _studyDescription = "*";
    _preferences = new QtDcmPreferences();
    _parent = parent;

    _thread = new QtDcmExportThread();

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
  }

QtDcmManager::~QtDcmManager()
  {
    delete _process;
    this->deleteTemporaryDirs();
  }

void
QtDcmManager::displayErrorMessage( QString message )
  {
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox(_parent);
    msgBox->setIcon(QMessageBox::Critical);
    msgBox->setText(message);
    msgBox->exec();
    delete msgBox;
  }

void
QtDcmManager::displayMessage( QString info )
  {
    //Instanciate a message from the parent i.e qtdcm
    QMessageBox * msgBox = new QMessageBox(_parent);
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText(info);
    msgBox->exec();
    delete msgBox;
  }

void
QtDcmManager::loadDicomdir()
  {
    _mode = "CD";
    //Convenience declarations for DCMTK
    static const OFString Patient("PATIENT");
    static const OFString Image("IMAGE");
    static const OFString Series("SERIES");
    static const OFString Study("STUDY");

    //Load dicomdir in a DCMTK DicomFileFormat object
    OFCondition status;
    if (!(status = _dfile.loadFile(_dicomdir.toLatin1().data())).good())
      {
        return;
      }
    //Get the dicomdir dataset in a DCMTK DcmObject
    _dcmObject = _dfile.getDataset();

    // Loading all the dicomdir items in a stack
    DcmStack items;
    if (!_dcmObject->findAndGetElements(DCM_Item, items).good())
      {
        return;
      }
    //Using temporary lists for storing detected studies, series and images
    QList<QtDcmStudy *> tmpStudy;
    QList<QtDcmSerie *> tmpSerie;
    QList<QtDcmImage *> tmpImage;
    _patients.clear();

    //Unstacking and loading the different lists
    while (items.card() > 0)
      {
        DcmItem* lobj = (DcmItem*) items.top();
        DcmStack dirent;

        OFCondition condition = lobj->findAndGetElements(DCM_DirectoryRecordType, dirent);
        if (!condition.good())
          {
            items.pop();
            continue;
          }

        while (dirent.card())
          {
            DcmElement* elt = (DcmElement*) dirent.top();
            OFString cur;
            elt->getOFStringArray(cur);

            if (cur == Patient)
              {
                _patients.append(new QtDcmPatient());
                _patients.last()->setStudies(tmpStudy);
                tmpStudy.clear();

                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_PatientID, lelt).good())
                  {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    _patients.last()->setId(QString(strID.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_PatientsName, lelt).good())
                  {
                    OFString strName;
                    lelt->getOFStringArray(strName);
                    _patients.last()->setName(QString(strName.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_PatientsBirthDate, lelt).good())
                  {
                    OFString strBirth;
                    lelt->getOFStringArray(strBirth);
                    _patients.last()->setBirthdate(QString(strBirth.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_PatientsSex, lelt).good())
                  {
                    OFString strSex;
                    lelt->getOFStringArray(strSex);
                    _patients.last()->setSex(QString(strSex.c_str()));
                  }
              }
            else if (cur == Study)
              {
                tmpStudy.append(new QtDcmStudy());
                tmpStudy.last()->setSeries(tmpSerie);
                tmpSerie.clear();

                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_StudyInstanceUID, lelt).good())
                  {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    tmpStudy.last()->setId(QString(strID.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_StudyDescription, lelt).good())
                  {
                    OFString strDescription;
                    lelt->getOFStringArray(strDescription);
                    tmpStudy.last()->setDescription(QString(strDescription.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_StudyDate, lelt).good())
                  {
                    OFString strDate;
                    lelt->getOFStringArray(strDate);
                    QString date = QString(strDate.c_str());
                    QDate qdate = QDate(date.mid(0, 4).toInt(), date.mid(4, 2).toInt(), date.mid(6, 2).toInt());
                    tmpStudy.last()->setDate(qdate);
                  }
                if (lobj->findAndGetElement(DCM_StudyTime, lelt).good())
                  {
                    OFString strTime;
                    lelt->getOFStringArray(strTime);
                    tmpStudy.last()->setTime(QString(strTime.c_str()));
                  }
              }
            else if (cur == Series)
              {
                tmpSerie.append(new QtDcmSerie());
                tmpSerie.last()->setImages(tmpImage);
                tmpImage.clear();

                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_SeriesInstanceUID, lelt).good())
                  {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    tmpSerie.last()->setId(QString(strID.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_SeriesDescription, lelt).good())
                  {
                    OFString strDescription;
                    lelt->getOFStringArray(strDescription);
                    QString description = QString(strDescription.c_str());
                    if (description == "")
                      tmpSerie.last()->setDescription("No description");
                    else
                      tmpSerie.last()->setDescription(description);
                  }
              }
            else // C'est une image
              {
                tmpImage.append(new QtDcmImage());
                DcmElement* lelt;
                if (lobj->findAndGetElement(DCM_ReferencedFileID, lelt).good())
                  {
                    OFString str;
                    lelt->getOFStringArray(str);
                    tmpImage.last()->setFilename(QString(str.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_ReferencedSOPInstanceUIDInFile, lelt).good())
                  {
                    OFString strID;
                    lelt->getOFStringArray(strID);
                    tmpImage.last()->setId(QString(strID.c_str()));
                  }
              }
            dirent.pop();
          }
        items.pop();
      }

  }

void
QtDcmManager::createTemporaryDirs()
  {
    //Creation d'un répertoire temporaire pour la série
    //    QString tmp = QDir::tempPath();
    QDir tempDir = QDir(QDir::tempPath()); //tempDir = /tmp
    if (!tempDir.exists("qtdcm"))
      tempDir.mkdir("qtdcm");

    //    tmp = tmp + QDir::separator() + "qtdcm";
    _tempDir = QDir(QDir::tempPath() + QDir::separator() + "qtdcm"); // tempDir = /tmp/qtdcm
    if (!_tempDir.exists("logs"))
      {
        if (!_tempDir.mkdir("logs"))
          qDebug() << "Repertoire logs non cree";
      }
    _logsDir = QDir(QDir::tempPath() + QDir::separator() + "qtdcm" + QDir::separator() + "logs");
  }

void
QtDcmManager::deleteTemporaryDirs()
  {
    QStringList listLogs = _logsDir.entryList(QDir::Files, QDir::Name);
    for (int i = 0; i < listLogs.size(); i++)
      {
        _logsDir.remove(listLogs.at(i));
      }
    _tempDir.rmdir("logs");
    QDir(QDir::tempPath()).rmdir("qtdcm");
  }

void
QtDcmManager::generateRandomDir()
  {
    // Calcul d'un nom aléatoire pour le répertoire temporaire
    QString acceptes = "abcdefghijklmnopqrstuvwyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
    _randDirName = "";
    for (int i = 0; i < 10; i++)
      _randDirName += acceptes[qrand() % 59];

    _tempRandDir = _tempDir.absolutePath() + QDir::separator() + _randDirName;
    _tempDir.mkdir(_randDirName);
  }

void
QtDcmManager::deleteRandomDir()
  {
    // Suppression des fichiers temporaires
    QStringList listFiles = _tempRandDir.entryList(QDir::Files, QDir::Name);
    for (int i = 0; i < listFiles.size(); i++)
      {
        _tempRandDir.remove(listFiles.at(i));
      }
    // Suppression du répertoire temporaire
    if (!_tempDir.rmdir(_randDirName))
      qDebug() << "Probleme lors de la suppression du répertoire temporaire";
  }

void
QtDcmManager::exportSerie()
  {
    _dcm2nii = _preferences->getDcm2nii();
    if (_outputDir == "")
      return;

    if (!QFile(_dcm2nii).exists())
      {
        //message d'erreur !
        this->displayErrorMessage("Impossible de trouver dcm2nii, verifiez votre installation");
        return;
      }
    this->generateRandomDir();

    if (_mode == "CD")
      this->exportSerieFromCD();
    else
      {
        this->exportSerieFromPACS();
        while (_thread->isRunning())
          {
            qApp->processEvents();
          }
        _progress->close();
        delete _progress;
      }

    QStringList listFiles = _tempRandDir.entryList(QDir::Files, QDir::Name);
    if (listFiles.size() != 0)
      {
        //Conversion de la serie avec dcm2nii
        QStringList arguments;
        arguments << "-x" << "N" << "-r" << "N" << "-g" << "N" << "-o" << _outputDir << _tempRandDir.absolutePath();
        _process->setStandardOutputFile(_logsDir.absolutePath() + QDir::separator() + "log" + _randDirName + ".txt");
        _process->start(_dcm2nii, arguments);
        _process->waitForFinished();

        this->deleteRandomDir();
      }
    else
      {
        //message d'erreur !
        this->displayErrorMessage("Pas d'images copiees, reessayez.");
      }

  }

void
QtDcmManager::exportSerieFromPACS()
  {
    QString program = _preferences->getDcm4che();
    QStringList arguments;
    QString serverPACSParam = _preferences->getServers().at(0)->getAetitle() + "@" + _preferences->getServers().at(0)->getServer() + ":" + _preferences->getServers().at(0)->getPort();
    QString localPACSParam = _preferences->getAetitle() + ":" + _preferences->getPort();
    QString seriesId = "-qSeriesInstanceUID=" + _serieId;
    arguments << "-L" << localPACSParam << serverPACSParam << "-I" << "-cmove" << _preferences->getAetitle() << seriesId << "-cstoredest" << _tempRandDir.absolutePath();
    QString command = program + " -L " + localPACSParam + " " + serverPACSParam + " -I" + " -cmove " + _preferences->getAetitle() + " -cstore=" + _modality + " " + seriesId + " -cstoredest "
        + _tempRandDir.absolutePath() + ">/dev/null";
    //    system(command.toAscii().data());

    _progress = new QProgressDialog("Export from PACS in progress...", "", 0, 0, _parent);
    _progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    _progress->setCancelButton(cancelButton);
    cancelButton->hide();
    _progress->show();
    qApp->processEvents();

    _thread->setCommand(command);
    _thread->start();
  }

void
QtDcmManager::exportSerieFromCD()
  {
    // Launch progress dialog window, to follow images copy
    _progress = new QProgressDialog("Dicom extraction in progress...", "", 0, 100, _parent);
    _progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    _progress->setCancelButton(cancelButton);
    cancelButton->hide();
    _progress->show();
    qApp->processEvents();

    //Copie des fichiers images dans le répertoire temporaire
    for (int i = 0; i < _images.size(); i++)
      {
        QFile image(_images.at(i));
        if (image.exists())
          {
            image.copy(_tempRandDir.absolutePath() + QDir::separator() + "ima" + QString::number(i));
            _progress->setValue(100 * i / _images.size());
            qApp->processEvents();
          }
      }
    _progress->setValue(100);
    qApp->processEvents();
    _progress->close();
    delete _progress;
    _images.clear();
  }

void
QtDcmManager::queryPACS()
  {
    _mode = "PACS";
    QString program = _preferences->getDcm4che();
    if (QFile(program).exists())
      {
        QStringList arguments;

        QString serverPACSParam = _preferences->getServers().at(0)->getAetitle() + "@" + _preferences->getServers().at(0)->getServer() + ":" + _preferences->getServers().at(0)->getPort();
        QString serieDescription = "-qSeriesDescription=" + _serieDescription;
        QString patientName = "-qPatientName=" + _patientName;
        QString patientId = "-qPatientId=" + _patientId;
        QString studyDescription = "-qStudyDescription=" + _studyDescription;
        QString modality = "-qModality=" + _modality;
        QString date = "-qStudyDate=" + _date1 + "-" + _date2;

        arguments << serverPACSParam << "-S" << serieDescription << patientName << studyDescription << modality << date;

        QProcess * process = new QProcess(this);
        process->start(program, arguments);

        QByteArray query;
        while (process->waitForReadyRead())
          query += process->readAll();

        delete process;
        //Parse the result of the query and store in the different lists and display in the QTreeWidget
        this->parseQueryResult(query);
      }
    else
      {
        //message d'erreur !
        this->displayErrorMessage("Impossible de trouver dcmqr, verifiez votre installation");
      }
  }

void
QtDcmManager::parseQueryResult( QString query )
  {
    _patients.clear();
    QStringList lines = query.split("\n");
    QString currentStudyId;
    int numPatient = 0;
    int cptStudy = 0;
    for (int i = 0; i < lines.size(); i++)
      {
        QRegExp rx(".*Query Response #.:.*");
        if (rx.exactMatch(lines[i])) // Detect patient name
          {
            numPatient = lines[i].split('#')[1].split(':')[0].toInt() - 1;
            // Add patient in the list
            _patients.append(new QtDcmPatient());
            QString name = lines[i + 6].section('[', 1).split(']')[0];
            _patients.last()->setName(name);
          }

        rx.setPattern(".*Query Response #. for Query Request.*");
        if (rx.exactMatch(lines[i])) //Detect new serie
          {
            numPatient = lines[i].split('#')[2].split('/')[0].toInt() - 1;
            //First get the corresponding study information
            QString studyDate = lines[i + 2].section('[', 1).split(']')[0];
            QDate qdate = QDate(studyDate.mid(0, 4).toInt(), studyDate.mid(4, 2).toInt(), studyDate.mid(6, 2).toInt());

            QString studyDesc = lines[i + 7].section('[', 1).split(']')[0];
            QString studyId = lines[i + 10].section('[', 1).split(']')[0];
            if (studyId != currentStudyId) // Is it a new study ?
              {
                cptStudy++;
                //Add the new study in the patients studies list and set study Id and description
                _patients.at(numPatient)->addStudy(new QtDcmStudy());
                _patients.at(numPatient)->getStudies().last()->setId(studyId);
                _patients.at(numPatient)->getStudies().last()->setDescription(studyDesc);
                _patients.at(numPatient)->getStudies().last()->setDate(qdate);
              }
            currentStudyId = studyId;

            //Get the found serie Id and description
            QString serieDesc = lines[i + 8].section('[', 1).split(']')[0];
            QString serieId = lines[i + 11].section('[', 1).split(']')[0];
            _patients.at(numPatient)->getStudies().last()->addSerie(new QtDcmSerie());
            //Set the description and Id of the found serie
            _patients.at(numPatient)->getStudies().last()->getSeries().last()->setId(serieId);
            _patients.at(numPatient)->getStudies().last()->getSeries().last()->setDescription(serieDesc);
            //            _patients.at(numPatient)->getStudies().last()->getSeries().last()->setDate(serieDate);
          }
      }
    //Aucune occurence = 0 study renvoyee
    if (cptStudy == 0)
      {
        //petit message d'information !
        this->displayMessage("Aucune occurence pour cette recherche");
      }
  }
