/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Alexandre Abadie <Alexandre.Abadie@univ-rennes1.fr>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef QTDCMSERVERS_H_
#define QTDCMSERVERS_H_

#include <QtGui>

/**
 * This class is a representation of a Dicom PACS server
 */

class QtDcmServer : public QObject
{
    Q_OBJECT

private:
    QString _aetitle; /** Application entity title (AETitle) of the PACS server */
    QString _server; /** The hostname of the server */
    QString _port; /** TCP port the application is listening on */
    QString _name; /** Description name of the PACS */

public:
    /**
     * Default constructor
     */
    QtDcmServer() {}

    /**
     * Default destructor
     */
    virtual ~QtDcmServer() {}

    /**
     * PACS AETitle getter
     *
     * @return _aetitle a QString
     */
    QString getAetitle() const
    {
        return _aetitle;
    }

    /**
     * Description name getter
     *
     * @return _name a QString
     */
    QString getName() const
    {
        return _name;
    }

    /**
     * PACS server port getter (QtDcm only ports between 1000 and 100000)
     *
     * @return _port as a QString
     */
    QString getPort() const
    {
        return _port;
    }

    /**
     * PACS server hostname getter
     *
     * @return _server as a QString
     */
    QString getServer() const
    {
        return _server;
    }

    /**
     * PACS AETitle setter
     *
     * @param _aetitle as a QString
     */
    void setAetitle ( QString _aetitle )
    {
        this->_aetitle = _aetitle;
    }

    /**
     * PACS Name setter
     *
     * @param _name as a QString
     */
    void setName ( QString _name )
    {
        this->_name = _name;
    }

    /**
     * PACS server port setter (QtDcm only ports between 1000 and 100000)
     *
     * @param port as a QString
     */
    void setPort ( QString port )
    {
        this->_port = port;
    }

    /**
     * PACS server hostname setter
     *
     * @param _server as a QString
     */
    void setServer ( QString _server )
    {
        this->_server = _server;
    }
};

#endif /* QTDCMSERVERS_H_ */
