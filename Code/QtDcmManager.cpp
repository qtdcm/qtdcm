/*
 * QtDcmManager.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#include <QtDcmManager.h>

QtDcmManager::QtDcmManager()
  {
    //Initialization of the private attributes
    _dicomdir = "";
    _outputDir = "";
    _process = new QProcess(this);
    _preferences = new QtDcmPreferences();

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
  }

QtDcmManager::QtDcmManager( QWidget * parent )
  {
    //Initialization of the private attributes
    _dicomdir = "";
    _outputDir = "";
    _process = new QProcess(this);
    _preferences = new QtDcmPreferences();
    _parent = parent;

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
  }

void
QtDcmManager::loadDicomdir()
  {
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
                    tmpStudy.last()->setDate(QString(strDate.c_str()));
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
QtDcmManager::exportSerie( QList<QString> images )
  {
//If system is windows or Unix binary extension is not the same
#ifdef Q_WS_WIN
    QString program = _dcm2niiPath + QDir::separator() + "dcm2nii.exe";
#else
    QString program = _dcm2niiPath + QDir::separator() + "dcm2nii";
#endif
    //If program exist, launch the process
    if (QFile(program).exists())
      {
        // Launch progress dialog window, to follow reconstruction process
        _progress = new QProgressDialog("Dicom extraction in progress...", "", 0, 100, _parent);
        _progress->setWindowModality(Qt::WindowModal);
        QPushButton * cancelButton = new QPushButton;
        _progress->setCancelButton(cancelButton);
        cancelButton->hide();
        _progress->show();
        qApp->processEvents();

        //Vérification du repertoire de sortie
        if (_outputDir == "")
          return;

        this->generateRandomDir();

        //Copie des fichiers images dans le répertoire temporaire
        for (int i = 0; i < images.size(); i++)
          {
            QFile image(images.at(i));
            if (image.exists())
              {
                image.copy(_tempRandDir.absolutePath() + QDir::separator() + "ima" + QString::number(i));
                _progress->setValue(100 * i / images.size());
                qApp->processEvents();
              }
          }
        _progress->setValue(100);
        qApp->processEvents();
        QStringList listFiles = _tempRandDir.entryList(QDir::Files, QDir::Name);
        if (listFiles.size() != 0)
          {
            //Conversion de la serie avec dcm2nii
            QStringList arguments;
            arguments << "-x" << "N" << "-r" << "N" << "-g" << "N" << "-o" << _outputDir << _tempRandDir.absolutePath();
            _process->setStandardOutputFile(_logsDir.absolutePath() + QDir::separator() + "log" + _randDirName + ".txt");
            _process->start(program, arguments);
            _process->waitForFinished();

            this->deleteRandomDir();
          }
        else
          {
            //message d'erreur !
            this->displayErrorMessage("Pas d'images copiees, reessayez.");
          }
        _progress->close();
      }
    else
      {
        //message d'erreur !
        this->displayErrorMessage("Impossible de trouver dcm2nii, verifiez votre installation");
      }
  }
