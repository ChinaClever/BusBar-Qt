#ifndef JSON_SEND_H
#define JSON_SEND_H
#include "json_pack.h"
#include "network.h"
#include <QThread>
#include "tcpclient.h"

class Json_Send : public QThread
{
    Q_OBJECT
    explicit Json_Send(QObject *parent = nullptr);
public:
    static Json_Send *bulid(QObject *parent = nullptr);
    void initFun();
protected:
    void sendData();
    void TcpsendData();
    void Startbox_Alarm(QJsonObject &obj ,int id);
private slots:
    void run();
private:
    Json_Pack *mJson;
    QTimer *timer;
    UdpSentSocket *mSocket;
    TcpClient *mTcp;
    int *mBoxNum[BUS_NUM];
};

#endif // JSON_SEND_H
