/*
 * QtDcmManager.cpp
 *
 *  Created on: 2 déc. 2009
 *      Author: Alexandre Abadie
 */

#include <QtDcmManager.h>

QtDcmManager::QtDcmManager()
  {
    // TODO Auto-generated constructor stub
    _dicomdir = "";
    _outputDir = "";
    _process = new QProcess(this);
  }

QtDcmManager::~QtDcmManager()
  {
    // TODO Auto-generated destructor stub
  }

void
QtDcmManager::loadDicomdir()
  {
    static const OFString Patient("PATIENT");
    static const OFString Image("IMAGE");
    static const OFString Series("SERIES");
    static const OFString Study("STUDY");

    OFCondition status;
    if (!(status = _dfile.loadFile(_dicomdir.toLatin1().data())).good())
      {
        return;
      }
    _dcmObject = _dfile.getDataset();

    // Find the item containing patients
    DcmStack items;
    if (!_dcmObject->findAndGetElements(DCM_Item, items).good())
      {
        return;
      }
    QList<QtDcmStudy *> tmpStudy;
    QList<QtDcmSerie *> tmpSerie;
    QList<QtDcmImage *> tmpImage;
    _patients.clear();

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
                    tmpSerie.last()->setDescription(QString(strDescription.c_str()));
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
QtDcmManager::exportSerie( QList<QString> images )
  {
    //Vérification du repertoire de sortie
    if (_outputDir == "")
      return;

    //Creation d'un répertoire temporaire pour la série
    QString tmp = QDir::tempPath();
    QDir tempDir = QDir(tmp);
    tmp = tmp + QDir::separator() + "qtdcm";
    tempDir.mkdir("qtdcm");

    tempDir = QDir(tmp);

    QString acceptes = "abcdefghijklmnopqrstuvwyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    QString randdir = "";
    qsrand(time(0));
    for (int i = 0; i < 10; i++)
      {
        int pos = qrand() % 62;//j'ai 26*2 +10 caractères dans acceptes
        randdir += acceptes[pos];
      }
    tmp += QDir::separator() + randdir;
    tempDir.mkdir(randdir);

    //Copie des fichiers images dans le répertoire temporaire
    for (int i = 0; i < images.size(); i++)
      {
        QFile image(images.at(i));
        image.copy(tmp + QDir::separator() + "ima" + QString::number(i));
      }

    QDir::setCurrent(tmp);

    //Conversion de la serie avec dcm2nii
    QString program = "/home/aabadie/Softs/builds/mricron/dcm2nii";
    QString arguments = "-g N -o " + _outputDir + " " + tmp + QDir::separator() +  "\*";
//    QStringList arguments;
//    arguments << "-g" << "N" << "-o" << _outputDir << "\"" + tmp + QDir::separator() + "*\"";
//    arguments << "-g" << "N" << "-o" << _outputDir << "ima*";
    QString command = program + " " + arguments;
//    QProcess::execute("ls .");
//    _process->setStandardOutputFile("/home/aabadie/Bureau/temp.txt");
//    _process->start(program, arguments);
//    _process->waitForFinished();
    system(command.toAscii().data());

//    qDebug() << "Command : " << command;


    //Suppression des fichiers temporaires


  }
