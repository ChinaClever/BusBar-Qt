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
    QString command = QString("date -s \'%1\'")
                          .arg(QString(data));
    int ret = system(command.toLatin1().data());
    ret = system("hwclock -f /dev/rtc0 -w");
}

void serverThread::disconnectToHost(void)
{
    m_socket->disconnectFromHost();
    disconnect(m_socket, SIGNAL(dataReady(const QString&, const QByteArray&)),
               this, SLOT(recvDataSlot(const QString&, const QByteArray&)));
    this->quit();
}
