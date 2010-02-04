/*
 * QtDcmQueryThread.cpp
 *
 *  Created on: 7 janv. 2010
 *      Author: aabadie
 */

#include <QtDcmQueryThread.h>

void
QtDcmQueryThread::run()
  {

//    DcmDataset query;
//    DcmElement* e = NULL;
//
//    e = newDicomElement(DCM_QueryRetrieveLevel);
//    e->putString("SERIES");
//    query.insert(e);
//
//    e = newDicomElement(DCM_SpecificCharacterSet);
//    query.insert(e);
//
//    e = newDicomElement(DCM_StudyInstanceUID);
//    e->putString("*");
//    query.insert(e);
//
//    e = newDicomElement(DCM_SeriesInstanceUID);
//    query.insert(e);
//
//    e = newDicomElement(DCM_SeriesNumber);
//    query.insert(e);
//
//    e = newDicomElement(DCM_Modality);
//    query.insert(e);
//
//    e = newDicomElement(DCM_SeriesDescription);
//    query.insert(e);
//
//    e = newDicomElement(DCM_SeriesTime);
//    query.insert(e);
//
//    e = newDicomElement(DCM_StationName);
//    query.insert(e);
//
//    e = newDicomElement(DCM_NumberOfSeriesRelatedInstances);
//    query.insert(e);
//
//    std::cout << "NEW QUERY:" << std::endl;
//    query.print(COUT);
//
//    T::SetAcceptLossyImages(s->m_lossy);
//
//    T::Create(s->m_aet, s->m_hostname, s->m_port, local_aet, syntax);
//
//    bool r = T::Connect(&net).good();
//
//    if (r == true)
//      {
//        std::cout << "T::SendObject()" << std::endl;
//        r = T::SendObject(query).good();
//      }
//
//    std::cout << "T::Drop()" << std::endl;
//    T::Drop();
//    std::cout << "T::Destroy()" << std::endl;
//    T::Destroy();
//
//    DcmStack* result = T::GetResultStack();
//    if (r && result != NULL && result->card() > 0)
//      {
//        std::cout << "signal_server_result('" << server << "')" << std::endl;
//        signal_server_result(result, server);
//      }
//
//
//    NetClient<FindAssociation> a;
//    a.QueryServer(&query, server, local_aet, UID_FINDStudyRootQueryRetrieveInformationModel);
//
//    DcmStack* result = a.GetResultStack();
//    for (unsigned int i = 0; i < result->card(); i++)
//      {
//        DcmDataset* dset = (DcmDataset*) result->elem(i);
//        dset->print(COUT);
//        resultslot(create_query_series(dset));
//      }

    // Use dcmqr
    _process = new QProcess();
    _process->start(_command, _parameters);

    QByteArray query;
    while (_process->waitForReadyRead())
      query += _process->readAll();

//    qDebug() << query;
    _manager->setQuery(query);

    delete _process;

    exit();
  }
