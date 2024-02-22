#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QObject>
#include <QtNetwork>
#include <QDebug>

#include "mysocket.h"
#include "rtuthread.h"
//#include "tcpserver.h"

class Socket;

class serverThread : public QThread
{
    Q_OBJECT
public:
    serverThread(int sockDesc, QObject *parent = Q_NULLPTR);
    ~serverThread();

private:
    void run(void);

signals:
    void dataReady(const QString &ip, const QByteArray &data);

private slots:   
    void recvDataSlot(const QString &ip, const QByteArray &data);
    void disconnectToHost(void);

private:
    MySocket *m_socket;

    int m_sockDesc;
    uchar *mBuf;
};

#endif // SERVERTHREAD_H
