/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/

/*!
    \class QxtXmlRpcClient
    \inmodule QxtNetwork
    \brief The QxtXmlRpcClient class implements an XML-RPC Client.

    Implements a Client that can communicate with services implementing the XML-RPC spec
    http://www.xmlrpc.com/spec

    \section2 Type Conversion

    \table 80%
    \header \o XML-RPC Type \o Qt Type
    \row  \o array \o QVariantList
    \row  \o base64 \o QByteArray   (decoded from base64 to raw)
    \row  \o boolean \o bool
    \row  \o dateTime \o QDateTime
    \row  \o double \o double
    \row  \o int \o int
    \row  \o i4 \o int
    \row  \o string \o QString
    \row  \o struct \o QVariantMap
    \row  \o nil \o QVariant()
    \endtable

*/

#include "qxtxmlrpcclient.h"
#include "qxtxmlrpccall.h"
#include "qxtxmlrpc_p.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

struct QxtXmlRpcClient::Private
{
    QUrl url;
    QNetworkAccessManager * networkManager;
    void serialize(QVariant);
};

QxtXmlRpcClient::QxtXmlRpcClient(QObject * parent)
        : QObject(parent)
        , d(new Private())
{
    d->networkManager = new QNetworkAccessManager(this);
}

QxtXmlRpcClient::~QxtXmlRpcClient() = default;

/*!
  returns the url of the remote service
 */
QUrl QxtXmlRpcClient::serviceUrl() const
{
    return d->url;
}

/*!
  sets the url of the remote service to \a url
 */
void QxtXmlRpcClient::setServiceUrl(QUrl url)
{
    d->url = url;
}
/*!
  returns the QNetworkAccessManager used for connecting to the remote service
 */
QNetworkAccessManager * QxtXmlRpcClient::networkManager() const
{
    return d->networkManager;
}
/*!
  sets the QNetworkAccessManager used for connecting to the remote service to \a manager
 */

void QxtXmlRpcClient::setNetworkManager(QNetworkAccessManager * manager)
{
    delete d->networkManager;
    d->networkManager = manager;
}

/*!
  calls the remote \a method with \a arguments and returns a QxtXmlRpcCall wrapping it.
  you can connect to QxtXmlRpcCall's signals to retreive the status of the call.
 */
QxtXmlRpcCall * QxtXmlRpcClient::call(QString method, QVariantList arguments)
{
    QByteArray data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><methodCall><methodName>" + method.toUtf8() + "</methodName><params>";
    foreach(QVariant i, arguments)
    {
        data += "<param><value>" + QxtXmlRpc::serialize(i).toUtf8() + "</value></param>";
    }
    data += "</params></methodCall>";


    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    request.setRawHeader("Connection", "close");
    request.setUrl(d->url);

    return new QxtXmlRpcCall(d->networkManager->post(request, data));
}
