/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtWeb module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/

#include "qxtsshchannel_p.h"
#include "qxtsshprocess.h"
#include "qxtsshtcpsocket.h"
#include <QTimer>

/*!
    \class QxtSshClient
    \inmodule QxtNetwork
    \brief The QxtSshClient class implements a secure shell client

    QxtSshClient allows connecting to any Ssh server, login via password, pubkey,
    then opening a shell or a socket.

    You can either set the passphrase or key before connecting, or later when 
    authentificationRequired has been fired. The later allows you to
    prompt the user for a password only when it is actually required.

    QxtSShClient includes the third party library libssh2 ( http://www.libssh2.org/ )

   \code
    * Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org>
    * Copyright (c) 2005,2006 Mikhail Gusarov <dottedmag@dottedmag.net>
    * Copyright (c) 2006-2007 The Written Word, Inc.
    * Copyright (c) 2007 Eli Fant <elifantu@mail.ru>
    * Copyright (c) 2009 Daniel Stenberg
    * Copyright (C) 2008, 2009 Simon Josefsson
    * All rights reserved.
    *
    * Redistribution and use in source and binary forms,
    * with or without modification, are permitted provided
    * that the following conditions are met:
    *
    *   Redistributions of source code must retain the above
    *   copyright notice, this list of conditions and the
    *   following disclaimer.
    *
    *   Redistributions in binary form must reproduce the above
    *   copyright notice, this list of conditions and the following
    *   disclaimer in the documentation and/or other materials
    *   provided with the distribution.
    *
    *   Neither the name of the copyright holder nor the names
    *   of any other contributors may be used to endorse or
    *   promote products derived from this software without
    *   specific prior written permission.
    *
    * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
    * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
    * OF SUCH DAMAGE.
    \endcode

*/

/*! \enum QxtSshClient::AuthenticationMethod
 *
 * \value PasswordAuthentication
 * \value PublicKeyAuthentication
 */

/*! \enum QxtSshClient::Error 
 
   \value AuthenticationError
   \value HostKeyUnknownError
   \value HostKeyInvalidError
   \value HostKeyMismatchError
   \value ConnectionRefusedError
   \value UnexpectedShutdownError
   \value HostNotFoundError
   \value SocketError
   \value UnknownError
 */

/*! \enum QxtSshClient::KnownHostsFormat
 *
 * \value OpenSslFormat
 */

/*! 
 * \fn QxtSshClient::connected()
 * this signal is emmited when the connection to the ssh server is ready to open channels.
 */

/*!
 * \fn QxtSshClient::disconnected()
 * emited when the connection becomes unavailable
 */
/*!
 * \fn QxtSshClient::error ( QxtSshClient::Error error ) 
 * emited when an error ocures
 */

/*!
 * constructs a new QxtSshClient
 */
QxtSshClient::QxtSshClient(QObject * parent)
    :QObject(parent)
    ,d(new QxtSshClientPrivate){
    d->p=this;
}

/*!
 *
 */
QxtSshClient::~QxtSshClient(){
}
/*!
 *  establish an ssh connection to host on port as user
 */
void QxtSshClient::connectToHost(QString user,QString host,int port){
    d->d_hostName=host;
    d->d_userName=user;
    d->d_port=port;
    d->d_state=1;
    d->connectToHost(host,port);
}

/*!
 * disconnect the current ssh connection.
 */
void QxtSshClient::disconnectFromHost (){
    d->d_reset();
}
/*!
 * set the password for the user.
 * this is also used for the passphrase of the private key.
 */
void QxtSshClient::setPassphrase(QString pass){
    //if(d->d_passphrase!=pass){
        d->d_failedMethods.removeAll(QxtSshClient::PasswordAuthentication);
        d->d_failedMethods.removeAll(QxtSshClient::PublicKeyAuthentication);
        d->d_passphrase=pass;
        if(d->d_state>1){
            QTimer::singleShot(0,d,SLOT(d_readyRead()));
        }
        //}
}
/*!
 * set a public and private key to use to authenticate with the ssh server.
 */
void QxtSshClient::setKeyFiles(QString publicKey,QString privateKey){
    //if(d->d_publicKey!=publicKey ||  d->d_privateKey!=privateKey){
        d->d_failedMethods.removeAll(QxtSshClient::PublicKeyAuthentication);
        d->d_publicKey=publicKey;
        d->d_privateKey=privateKey;
        if(d->d_state>1){
            QTimer::singleShot(0,d,SLOT(d_readyRead()));
        }
        //}
}
/*!
 * load known hosts from a file.
 */
bool QxtSshClient::loadKnownHosts(QString file,KnownHostsFormat c){
    Q_UNUSED(c);
    return (libssh2_knownhost_readfile(d->d_knownHosts, qPrintable(file),
                                      LIBSSH2_KNOWNHOST_FILE_OPENSSH)==0);
}
/*!
 * save known hosts to a file
 */
bool QxtSshClient::saveKnownHosts(QString file,KnownHostsFormat c) const{
    Q_UNUSED(c);
    return (libssh2_knownhost_writefile(d->d_knownHosts, qPrintable(file),
                                LIBSSH2_KNOWNHOST_FILE_OPENSSH)==0);
}
/*!
 * add a known host
 */
bool QxtSshClient::addKnownHost(QString hostname,QxtSshKey key){
    int typemask=LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW;
    switch (key.type){
        case QxtSshKey::Dss:
            typemask|=LIBSSH2_KNOWNHOST_KEY_SSHDSS;
            break;
        case QxtSshKey::Rsa:
            typemask|=LIBSSH2_KNOWNHOST_KEY_SSHRSA;
            break;
        case QxtSshKey::UnknownType:
            return false;
    };


    return(libssh2_knownhost_add(d->d_knownHosts, qPrintable(hostname),
                                 NULL, key.key.data(), key.key.size(),
                                 typemask,  NULL));

}

/*!
 * key used by the ssh server currently connected to
 */
QxtSshKey QxtSshClient::hostKey() const{
    return d->d_hostKey;
}
/*!
 * hostname of the ssh server currently connected to
 */
QString   QxtSshClient::hostName() const{
    return d->d_hostName;
}
/*!
 * execute a process on the ssh server and connect a channel to it.
 *
 * returns NULL when not connected to an ssh server.
 */
QxtSshProcess * QxtSshClient::openProcessChannel(){
    if(d->d_state!=6){
        qWarning("cannot open channel before connected()");
        return NULL;
    }
    QxtSshProcess * s=new QxtSshProcess(this);
    d->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),d,SLOT(d_channelDestroyed()));
    return s;
}
/*!
 * Establish a tcp connection to a remote host, tunneling through this ssh channel.
 * Traffic from the ssh server to host is unencrypted, while traffic from this machine to 
 * the ssh server remains encrypted.
 *
 * returns NULL when not connected to an ssh server.
 */
QxtSshTcpSocket * QxtSshClient::openTcpSocket(QString hostName,quint16 port){
    if(d->d_state!=6){
        qWarning("cannot open channel before connected()");
        return NULL;
    }
    QxtSshTcpSocket * s=new QxtSshTcpSocket(this);
    d->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),d,SLOT(d_channelDestroyed()));
    s->d->openTcpSocket(hostName,port);
    return s;
}


static ssize_t qxt_p_libssh_recv(int socket,void *buffer, size_t length,int flags, void **abstract){
    Q_UNUSED(socket);
    Q_UNUSED(flags);
    QTcpSocket* c=reinterpret_cast<QTcpSocket*>(*abstract);
    int r=c->read(reinterpret_cast<char*>(buffer),length);
    if(r==0)
        return -EAGAIN;
    return r;
}

static ssize_t qxt_p_libssh_send(int socket,const void *buffer, size_t length,int flags, void **abstract){
    Q_UNUSED(socket);
    Q_UNUSED(flags);
    QTcpSocket* c=reinterpret_cast<QTcpSocket*>(*abstract);
    int r=c->write(reinterpret_cast<const char*>(buffer),length);
    if(r==0)
        return -EAGAIN;
    return r;
}





QxtSshClientPrivate::QxtSshClientPrivate()
    :d_session(0)
    ,d_knownHosts(0)
    ,d_state(0)
    ,d_errorCode(0)
{
    connect(this,SIGNAL(connected()),this,SLOT(d_connected()));
    connect(this,SIGNAL(disconnected()),this,SLOT(d_disconnected()));
    connect(this,SIGNAL(readyRead()),this,SLOT(d_readyRead()));

    Q_ASSERT(libssh2_init (0)==0);

    d_reset();

}

QxtSshClientPrivate::~QxtSshClientPrivate(){
    d_reset();
    if(d_session){
        libssh2_knownhost_free(d_knownHosts);
        libssh2_session_free(d_session);
    }
}

void QxtSshClientPrivate::d_connected(){
    d_state=2;
    d_readyRead();
}

void QxtSshClientPrivate::d_readyRead(){
    if(d_state==2){
        int sock=socketDescriptor();
        int ret=0;

        //1) initalise ssh session. exchange banner and stuff.

        if((ret = libssh2_session_startup(d_session, sock)) ==LIBSSH2_ERROR_EAGAIN){
            return;
        }
        if (ret) {
            qWarning("Failure establishing SSH session: %d", ret);
            emit p->error(QxtSshClient::UnexpectedShutdownError);
            d_reset();
            return;
        }


        //2) make sure remote is safe.
        size_t len;
        int type;
        const char * fingerprint = libssh2_session_hostkey(d_session, &len, &type);
        d_hostKey.key=QByteArray(fingerprint,len);
        d_hostKey.hash=QByteArray(libssh2_hostkey_hash(d_session,LIBSSH2_HOSTKEY_HASH_MD5),16);
        switch (type){
            case LIBSSH2_HOSTKEY_TYPE_RSA:
                d_hostKey.type=QxtSshKey::Rsa;
                break;
            case LIBSSH2_HOSTKEY_TYPE_DSS:
                d_hostKey.type=QxtSshKey::Dss;
                break;
            default:
                d_hostKey.type=QxtSshKey::UnknownType;
        }
        if(fingerprint) {
            struct libssh2_knownhost *host;
            int check = libssh2_knownhost_check(d_knownHosts, qPrintable(d_hostName),
                                                (char *)fingerprint, len,
                                                LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                                LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                &host);

            switch(check){
                case LIBSSH2_KNOWNHOST_CHECK_MATCH:
                    d_state=3;
                    d_readyRead();
                    return;
                case LIBSSH2_KNOWNHOST_CHECK_FAILURE:
                    d_delaydError=QxtSshClient::HostKeyInvalidError;
                    break;
                case LIBSSH2_KNOWNHOST_CHECK_MISMATCH:
                    d_delaydError=QxtSshClient::HostKeyMismatchError;
                    break;
                case LIBSSH2_KNOWNHOST_CHECK_NOTFOUND:
                    d_delaydError=QxtSshClient::HostKeyUnknownError;
                    break;
            }
        }else{
            d_delaydError=QxtSshClient::HostKeyInvalidError;
        }
        d_getLastError();
        d_reset();
        disconnectFromHost ();
        QTimer::singleShot(0,this,SLOT(d_delaydErrorEmit()));
        return;

    }else if(d_state==3){
        //3) try auth type "none" and get a list of other methods
        //   in the likely case that the server doesnt like "none"

        QByteArray username=d_userName.toLocal8Bit();
        char * alist=libssh2_userauth_list(d_session, username.data(),username.length());
        if(alist==NULL){
            if(libssh2_userauth_authenticated(d_session)){
                //null auth ok
                emit p->connected();
                d_state=5;
                return;
            }else if(libssh2_session_last_error(d_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN) {
                return;
            }else{
                d_getLastError();
                emit p->error(QxtSshClient::UnexpectedShutdownError);
                d_reset();
                emit p->disconnected();
                return;
            }
        }

        foreach(QByteArray m,QByteArray(alist).split(',')){
            if(m=="publickey"){
                d_availableMethods<<QxtSshClient::PublicKeyAuthentication;
            }
            else if(m=="password"){
                d_availableMethods<<QxtSshClient::PasswordAuthentication;
            }
        }
        d_state=4;
        d_readyRead();
    }else if(d_state==4){
        qDebug("looking for auth option");
        if(d_availableMethods.contains(QxtSshClient::PublicKeyAuthentication) &&
           !d_privateKey.isNull() &&
           !d_failedMethods.contains(QxtSshClient::PublicKeyAuthentication)){

            d_currentAuthTry=QxtSshClient::PublicKeyAuthentication;
            d_state=5;
            d_readyRead();
            return;
        }
        if(d_availableMethods.contains(QxtSshClient::PasswordAuthentication) &&
           !d_passphrase.isNull() &&
           !d_failedMethods.contains(QxtSshClient::PasswordAuthentication)){

            d_currentAuthTry=QxtSshClient::PasswordAuthentication;
            d_state=5;
            d_readyRead();
            return;
        }
        emit p->authenticationRequired(d_availableMethods);
    }else if(d_state==5){
        int ret;
        qDebug()<<"trying"<<d_currentAuthTry;
        if(d_currentAuthTry==QxtSshClient::PasswordAuthentication){
            ret=libssh2_userauth_password(d_session, qPrintable(d_userName),
                                          qPrintable(d_passphrase));

        }else if(d_currentAuthTry==QxtSshClient::PublicKeyAuthentication){
            ret=libssh2_userauth_publickey_fromfile(d_session,
                                                       qPrintable(d_userName),
                                                       qPrintable(d_publicKey),
                                                       qPrintable(d_privateKey),
                                                       qPrintable(d_passphrase));
        }
        if(ret==LIBSSH2_ERROR_EAGAIN ){
            return;
        }else if(ret==0){
            d_state=6;
            emit p->connected();
        }else{
            d_getLastError();
            emit p->error(QxtSshClient::AuthenticationError);
            d_failedMethods.append(d_currentAuthTry);
            d_state=4;
            d_readyRead();
        }
    }else if(d_state==6){
        QList<QxtSshChannel*>::const_iterator i;
        for (i = d_channels.constBegin(); i != d_channels.constEnd(); ++i){
            bool ret=(*i)->d->activate();
            if(!ret){
                d_getLastError();
            }
        }
    }else{
        qDebug("did not expect to receive data in this state");
    }
}

void QxtSshClientPrivate::d_reset(){
    qDebug("reset");

    //teardown
    if(d_knownHosts){
        libssh2_knownhost_free(d_knownHosts);
    }
    if(d_state>1){
        libssh2_session_disconnect(d_session,"good bye!");
    }
    if(d_session){
        libssh2_session_free(d_session);
    }

    d_state=0;
    d_errorCode=0;
    d_errorMessage=QString();
    d_failedMethods.clear();
    d_availableMethods.clear();


    //buildup
    d_session=libssh2_session_init_ex(NULL,NULL,NULL,reinterpret_cast<void*>(this));
    libssh2_session_callback_set(d_session,LIBSSH2_CALLBACK_RECV,reinterpret_cast<void*>(&qxt_p_libssh_recv));
    libssh2_session_callback_set(d_session,LIBSSH2_CALLBACK_SEND,reinterpret_cast<void*>(&qxt_p_libssh_send));
    Q_ASSERT(d_session);

    d_knownHosts= libssh2_knownhost_init(d_session);
    Q_ASSERT(d_knownHosts);

    libssh2_session_set_blocking(d_session,0);


}

void QxtSshClientPrivate::d_disconnected (){
    if(d_state!=0){
        qWarning("unexpected shutdown");
        d_reset();
    }
}

void QxtSshClientPrivate::d_getLastError(){
    char * msg;
    int len=0;
    d_errorCode=libssh2_session_last_error(d_session, &msg, &len,0);
    d_errorMessage=QString::fromLocal8Bit(QByteArray::fromRawData(msg,len));
}



void QxtSshClientPrivate::d_channelDestroyed(){
    QxtSshChannel* channel=  qobject_cast<QxtSshChannel*>(sender());
    d_channels.removeAll(channel);
}

void QxtSshClientPrivate::d_delaydErrorEmit(){
    emit p->error(d_delaydError);
}
