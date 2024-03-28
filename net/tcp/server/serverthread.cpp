#include "serverthread.h"

serverThread::serverThread(int sockDesc, QObject *parent) :
    QThread(parent),
    m_sockDesc(sockDesc)
{

}

serverThread::~serverThread()
{
    m_socket->close();
    if(m_socket) {delete m_socket;m_socket = nullptr;}
}

void serverThread::run(void)
{
    m_socket = new MySocket(m_sockDesc);

    if (!m_socket->setSocketDescriptor(m_sockDesc)) {
        return ;
    }

    connect(m_socket, &MySocket::disconnected, this, &serverThread::disconnectToHost);
    connect(m_socket, SIGNAL(dataReady(const QString&, const QByteArray&)),
            this, SLOT(recvDataSlot(const QString&, const QByteArray&)));

    this->exec();
}

void serverThread::recvDataSlot(const QString &ip, const QByteArray &data)
{
    //qDebug()<<"recv date "<<QString(data);
    QDateTime t1 = QDateTime::fromString(QString(data) , "yyyy-MM-dd HH:mm:ss");

    QString command = QString("date -s \'%1\'")
                          .arg(QString(data));
    int ret = system(command.toLatin1().data());
    ret = system("hwclock -f /dev/rtc0 -w");

    QDateTime t = QDateTime::currentDateTime();
    if( qAbs(t.secsTo(t1)) <= 60 ){
        m_socket->write((char*)"OK");
        m_socket->flush();
    }else{
        m_socket->write((char*)"FAIL");
        m_socket->flush();
    }
}

void serverThread::disconnectToHost(void)
{
    m_socket->disconnectFromHost();
    disconnect(m_socket, SIGNAL(dataReady(const QString&, const QByteArray&)),
               this, SLOT(recvDataSlot(const QString&, const QByteArray&)));
    this->quit();
}
