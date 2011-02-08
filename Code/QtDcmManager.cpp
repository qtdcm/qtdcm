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
    _patientName = "*";
    _patientId = "*";
    _modality = "MR";
    _date1 = "*";
    _date2 = "*";
    _serieDescription = "*";
    _studyDescription = "*";

    _preferences = new QtDcmPreferences();
    _exportThread = new QtDcmExportThread();
    _queryThread = new QtDcmQueryThread();

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
  }

QtDcmManager::QtDcmManager( QWidget * parent )
  {
    //Initialization of the private attributes
    _mode = "PACS";
    _dicomdir = "";
    _outputDir = "";
    _patientName = "*";
    _patientId = "*";
    _date1 = "*";
    _date2 = "*";
    _modality = "MR";
    _serieDescription = "*";
    _studyDescription = "*";
    _preferences = new QtDcmPreferences();
    _parent = parent;

    _exportThread = new QtDcmExportThread();
    _queryThread = new QtDcmQueryThread();

    //Creation of the temporary directories (/tmp/qtdcm and /tmp/qtdcm/logs)
    this->createTemporaryDirs();
  }

QtDcmManager::~QtDcmManager()
  {
    this->deleteTemporaryDirs();
    delete _exportThread;
    delete _queryThread;
    delete _preferences;
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
    _seriesToExport.clear();
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
                if (lobj->findAndGetElement(DCM_PatientName, lelt).good())
                  {
                    OFString strName;
                    lelt->getOFStringArray(strName);
                    _patients.last()->setName(QString(strName.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_PatientBirthDate, lelt).good())
                  {
                    OFString strBirth;
                    lelt->getOFStringArray(strBirth);
                    _patients.last()->setBirthdate(QString(strBirth.c_str()));
                  }
                if (lobj->findAndGetElement(DCM_PatientSex, lelt).good())
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
    QDir tempDir = QDir(QDir::tempPath()); //tempDir = /tmp

    //Generer un nom de répertoire aléatoire
    QString acceptes = "abcdefghijklmnopqrstuvwyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789";
    QString randName;
    qsrand(time(NULL));
    for (int i = 0; i < 6; i++)
      randName += acceptes[qrand() % 59];

    if (!tempDir.exists("qtdcm" + randName))
      tempDir.mkdir("qtdcm" + randName);

    _tempDir = QDir(QDir::tempPath() + QDir::separator() + "qtdcm" + randName); // tempDir = /tmp/qtdcm
    if (!_tempDir.exists("logs"))
      {
        if (!_tempDir.mkdir("logs"))
          qDebug() << tr("Repertoire logs non cree");
      }
    _logsDir = QDir(_tempDir.absolutePath() + QDir::separator() + "logs");
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

    QStringList listSerie = _tempDir.entryList(QDir::Dirs, QDir::Name);
    for (int i = 0; i < listSerie.size(); i++)
      {
        QDir serieDir(_tempDir.absolutePath() + QDir::separator() + listSerie.at(i));
        QStringList listFiles = serieDir.entryList(QDir::Files, QDir::Name);
        if (!(listSerie.at(i) == "." || listSerie.at(i) == "."))
          for (int j = 0; j < listFiles.size(); j++)
            {
                {
                  serieDir.remove(listFiles.at(j));
                }
              _tempDir.rmdir(listSerie.at(i));
            }
      }
    QDir(QDir::tempPath()).rmdir(_tempDir.dirName());
  }

void
QtDcmManager::generateCurrentSerieDir()
  {
    if (!_serieId.isEmpty())
      {
        _currentSerieDir = _tempDir.absolutePath() + QDir::separator() + _serieId;
        _tempDir.mkdir(_serieId);
      }

  }

void
QtDcmManager::deleteCurrentSerieDir()
  {
    // Suppression des fichiers temporaires
    QStringList listFiles = _currentSerieDir.entryList(QDir::Files, QDir::Name);
    for (int i = 0; i < listFiles.size(); i++)
      {
        _currentSerieDir.remove(listFiles.at(i));
      }
    // Suppression du répertoire temporaire
    if (!_tempDir.rmdir(_serieId))
      qDebug() << tr("Probleme lors de la suppression du répertoire temporaire");
  }

void
QtDcmManager::exportSeries()
  {
    _dcm2nii = _preferences->getDcm2nii();
    if (_outputDir == "")
      return;

    if (!QFile(_dcm2nii).exists())
      {
        //message d'erreur !
        this->displayErrorMessage(tr("Impossible de trouver dcm2nii, verifiez votre installation"));
        return;
      }

    if (_mode == "CD")
      this->exportSerieFromCD();
    else
      {
        this->exportSerieFromPACS();
        while (_exportThread->isRunning())
          {
            qApp->processEvents();
          }
        _progress->close();
        delete _progress;
      }

    QList<QString> series = _seriesToExport.keys();
    for (int j = 0; j < series.size(); j++)
      {
        _serieId = series.at(j);
        _currentSerieDir = QDir(_tempDir.absolutePath() + QDir::separator() + _serieId);
        //Conversion de la serie avec dcm2nii
        QStringList arguments;
        arguments << "-x" << "N";
        arguments << "-r" << "N";
        arguments << "-g" << "N";
        arguments << "-o" << _outputDir << _currentSerieDir.absolutePath();

        _process = new QProcess(this);
        _process->setStandardOutputFile(_logsDir.absolutePath() + QDir::separator() + "log" + _serieId + ".txt");
        _process->start(_dcm2nii, arguments);
        _process->waitForFinished();

        this->deleteCurrentSerieDir();
        delete _process;
      }
    this->displayMessage(tr("Import termine"));
  }

void
QtDcmManager::exportSerieFromCD()
  {
    // Launch progress dialog window, to follow images copy
    _progress = new QProgressDialog(tr("Copie des images depuis le CD..."), "", 0, 100, _parent);
    _progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    _progress->setCancelButton(cancelButton);
    cancelButton->hide();
    _progress->setValue(0);
    qApp->processEvents();
    _progress->show();
    qApp->processEvents();

    //Copie des fichiers images dans le répertoire temporaire
    QList<QString> series = _seriesToExport.keys();
    for (int j = 0; j < series.size(); j++)
      {
        _progress->setValue(0);
        _serieId = series.at(j);
        _currentSerieDir = QDir(_tempDir.absolutePath() + QDir::separator() + _serieId);
        if (!_tempDir.exists(series.at(j)))
          {
            this->generateCurrentSerieDir();
          }
        for (int i = 0; i < _seriesToExport.value(series.at(j)).size(); i++)
          {
            QFile image(_seriesToExport.value(series.at(j)).at(i));
            if (image.exists())
              {
                QString zeroStr;
                zeroStr.fill(QChar('0'), 5 - QString::number(i).size());
                image.copy(_currentSerieDir.absolutePath() + QDir::separator() + "ima" + zeroStr + QString::number(i));
                _progress->setValue(100 * i / _seriesToExport.value(series.at(j)).size());
                qApp->processEvents();
              }
          }
        _progress->setValue(100);
        qApp->processEvents();
      }
    _progress->close();
    delete _progress;
  }

void
QtDcmManager::exportSerieFromPACS()
  {
    QString program = _preferences->getDcm4che();

    _exportThread->setProgram(_preferences->getDcm4che());
    _exportThread->setServerPacsParam(_preferences->getServers().at(0)->getAetitle() + "@" + _preferences->getServers().at(0)->getServer() + ":" + _preferences->getServers().at(0)->getPort());
    _exportThread->setLocalPacsParam(_preferences->getAetitle() + "@" + _preferences->getHostname() + ":" + _preferences->getPort());
    _exportThread->setSeriesToExport(_seriesToExport.keys());
    _exportThread->setTemporaryDir(_tempDir.absolutePath());
    _exportThread->setModality(_modality);
    _exportThread->setAetitle(_preferences->getAetitle());

    _progress = new QProgressDialog(tr("Chargement des images..."), "", 0, 0, _parent);
    _progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    _progress->setCancelButton(cancelButton);
    cancelButton->hide();
    cancelButton->hide();
    _progress->setValue(0);
    _progress->show();
    qApp->processEvents();

    _exportThread->start();
  }

void
QtDcmManager::queryPACS()
  {
    _seriesToExport.clear();
    _mode = "PACS";
    QString program = _preferences->getDcm4che();
    if (QFile(program).exists())
      {
        QStringList arguments;
        QString localDicomParam = _preferences->getAetitle() + "@" + _preferences->getHostname() + ":" + _preferences->getPort();
        QString serverPACSParam = _preferences->getServers().at(0)->getAetitle() + "@" + _preferences->getServers().at(0)->getServer() + ":" + _preferences->getServers().at(0)->getPort();
        QString serieDescription = "-qSeriesDescription=" + _serieDescription;
        QString patientName = "-qPatientName=" + _patientName;
        QString patientId = "-qPatientId=" + _patientId;
        QString studyDescription = "-qStudyDescription=" + _studyDescription;
        QString modality = "-qModality=" + _modality;
        QString date = "-qStudyDate=" + _date1 + "-" + _date2;

        arguments << "-L" << localDicomParam << serverPACSParam;
        arguments << "-S" << serieDescription;
        arguments << patientName;
        arguments << studyDescription << modality << date;

        _progress = new QProgressDialog(tr("Requete en cours..."), "", 0, 0, _parent);
        _progress->setWindowModality(Qt::WindowModal);
        QPushButton * cancel = new QPushButton("Cancel");
        _progress->setCancelButton(cancel);
        _progress->show();
        qApp->processEvents();

        _queryThread->setCommand(program);
        _queryThread->setParameters(arguments);
        _queryThread->setManager(this);
        _queryThread->start();

        while (_queryThread->isRunning())
          {
            if (_progress->isHidden())
              _queryThread->terminate();
            qApp->processEvents();
          }

        _progress->close();
        delete _progress;

        //Parse the result of the query and store in the different lists and display in the QTreeWidget
        this->parseQueryResult(_query);
      }
    else
      {
        //message d'erreur !
        this->displayErrorMessage(tr("Impossible de trouver dcmqr, verifiez votre installation"));
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
          }
      }
    //Aucune occurence = 0 study renvoyee
    if (cptStudy == 0)
      {
        //petit message d'information !
        this->displayMessage(tr("Aucune occurence pour cette recherche"));
      }
  }

void
QtDcmManager::makePreview()
  {
    _seriesToExport.insert(_serieId, _images);
    _currentSerieDir = QDir(_tempDir.absolutePath() + QDir::separator() + _serieId);
    if (!_tempDir.exists(_serieId))
      {
        //Utiliser l'UID de la serie pour copier les images
        if (_mode == "CD")
          {
            this->exportSerieFromCD();
          }
        else
          {
            this->exportSerieFromPACS();
            while (_exportThread->isRunning())
              {
                qApp->processEvents();
              }
            _progress->close();
            delete _progress;
          }
      }
    _progress = new QProgressDialog(tr("Creation de l'apercu..."), "", 0, 0, _parent);
    _progress->setWindowModality(Qt::WindowModal);
    QPushButton * cancelButton = new QPushButton;
    _progress->setCancelButton(cancelButton);
    cancelButton->hide();
    _progress->show();
    qApp->processEvents();
    QStringList list = _currentSerieDir.entryList(QDir::Files, QDir::Name);
    _listImages.clear();

    DcmRLEDecoderRegistration::registerCodecs(OFFalse, OFFalse);
    DJDecoderRegistration::registerCodecs(EDC_photometricInterpretation, EUC_default, EPC_default, OFFalse);

    for (int i = 0; i < list.size(); i++)
      {
        qApp->processEvents();
        QImage * current_image_ = NULL;
        // get pixeldata
        DcmFileFormat file;
        file.loadFile((_currentSerieDir.absolutePath() + QDir::separator() + list.at(i)).toLatin1().data());
        DcmDataset * dset = file.getDataset();
        DicomImage* dcimage = new DicomImage(dset, file.getDataset()->getOriginalXfer(), CIF_MayDetachPixelData);

        dcimage->setNoDisplayFunction();
        dcimage->hideAllOverlays();
        dcimage->setNoVoiTransformation();

        //        DicomImage * dcimage = new DicomImage((_currentSerieDir.absolutePath() + QDir::separator() + list.at(i)).toLatin1().data());
        if (dcimage != NULL)
          {
            if (dcimage->getStatus() == EIS_Normal)
              {
                Uint32 *pixelData = (Uint32 *) (dcimage->getOutputData(32 /* bits per sample */));
                if (pixelData != NULL)
                  {
                    Uint8 *colored = new Uint8[dcimage->getWidth() * dcimage->getHeight() * 4]; //4 * dcimage->getWidth() * dcimage->getHeight() matrix
                    Uint8 *col = colored;
                    Uint32 *p = pixelData;
                    //get the highest values for RGBA, then use them to scale the pixel luminosity
                    Uint32 p_max = 0;
					Uint32 p_min = std::numeric_limits<Uint32>::max();
                    for (unsigned i = 0; i < dcimage->getWidth(); ++i)
                      for (unsigned j = 0; j < dcimage->getHeight(); ++j, ++p)
                        {
                          if (*p > p_max)
                            p_max = *p;
                          if (*p < p_min)
                            p_min = *p;
                        }
                    double a = 4294967295.f / ((double) p_max - (double) p_min);
                    //re-initialize 'col'
                    p = pixelData;
                    //copy the pixels in our QImage
                    for (unsigned i = 0; i < dcimage->getWidth(); ++i)
                      for (unsigned j = 0; j < dcimage->getHeight(); ++j, ++p)
                        {
                          *col = (Uint8) ((255.f / 4294967295.f) * (a * ((double) (*p) - (double) p_min)));
                          ++col;
                          *col = (Uint8) ((255.f / 4294967295.f) * (a * ((double) (*p) - (double) p_min)));
                          ++col;
                          *col = (Uint8) ((255.f / 4294967295.f) * (a * ((double) (*p) - (double) p_min)));
                          ++col;
                          *col = 255;
                          ++col;
                        }
                    _listImages.append(QImage(colored, dcimage->getWidth(), dcimage->getHeight(), QImage::Format_ARGB32));
                  }

              }
          }
      }
    _progress->close();
    delete _progress;
    _seriesToExport.clear();
  }
