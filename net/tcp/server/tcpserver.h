#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "rtuthread.h"

typedef QPair<QString,int> TcpConnect;
class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

    void init(int port, bool isVerify=true);
//    bool isConnect();

    int readData(QString &ip, char *data);

signals:

protected:
    void landVerify(QTcpSocket *socket);

private slots:
    void newConnectSlot();
    void readMessage();
    void removeUserFormList();


private:
    QTcpServer *m_tcpServer;
    QMap<TcpConnect, QTcpSocket *> m_mapClient;
    bool mIsConnect, mIsVerify;
    bool isRun;
//    uchar *mSendBuf;
};

#endif // TCPSERVER_H
