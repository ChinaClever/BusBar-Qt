#include "tcpserver.h"

#define ANDROID_TCP_PORT	11283  // 案桌TCP端口号

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    //设置最大允许连接数，不设置的话默认为30
    m_tcpServer->setMaxPendingConnections(2);
    connect(m_tcpServer,SIGNAL(newConnection()),this,SLOT(newConnectSlot()));
}

void TcpServer::init(int port, bool isVerify)
{
    if(m_tcpServer->listen(QHostAddress::AnyIPv4, port)){
        qDebug() << "listen OK!";
    }else{
        qDebug() << "listen error!";
    }
    mIsVerify = isVerify;
}

void TcpServer::newConnectSlot()
{
    QTcpSocket *tcp = m_tcpServer->nextPendingConnection();
//    qDebug() <<"newConnectSlot"<<endl;
    if(mIsVerify) {
        connect(tcp,SIGNAL(readyRead()),this,SLOT(readMessage()));
        mIsConnect = false;
    } else {
        mIsConnect = true;
    }
    m_mapClient.insert(TcpConnect(tcp->peerAddress().toString() , tcp->peerPort()), tcp);
//    qDebug()<<m_mapClient.size() << "        "<<tcp->peerPort();
    connect(tcp,SIGNAL(disconnected()),this,SLOT(removeUserFormList()));
}

//bool TcpServer::isConnect()
//{
//    bool ret = false;
//    if(m_mapClient.contains(mIP))
//        ret = mIsConnect;
//    return ret;
//}

int TcpServer::readData(QString &ip, char *data)
{
    int ret=0;

    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(socket) {
        ip = socket->peerAddress().toString();
        ret = socket->read(data, 256);
    }
    return ret;
}

/**
 * 首先进行身份验证
 */
void TcpServer::landVerify(QTcpSocket *socket)
{
//    if(socket){
//    qDebug() <<socket->peerAddress().toString()<<socket;
//    }

    QByteArray by;
    if(socket){
        by = socket->readAll();
    }
    mIsConnect = true;
}

void TcpServer::readMessage()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
    if(socket){
        landVerify(socket);
    }

    //disconnect(socket,SIGNAL(readyRead()),this,SLOT(readMessage()));
}

void TcpServer::removeUserFormList()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    QMap<TcpConnect, QTcpSocket *>::iterator it;
    for(it=m_mapClient.begin();it!=m_mapClient.end();it++)
    {
        if(socket->peerAddress().toString() == it.key().first && socket->peerPort() == it.key().second)
        {
            disconnect(socket,SIGNAL(readyRead()),this,SLOT(readMessage()));
            m_mapClient.erase(it);
            break;
        }
    }
}
