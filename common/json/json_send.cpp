#include "json_send.h"

Json_Send::Json_Send(QObject *parent) : QThread(parent)
{
    mJson = Json_Pack::bulid();
    mSocket = new UdpSentSocket();
    mTcp = new TcpClient();

    sDataPacket *shm = get_share_mem();
    for(int i = 0;i <BUS_NUM; i++)
    {
        mBoxNum[i] = &(shm->data[i].boxNum);
    }
    timer = new QTimer(this);
    timer->start(5*1000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(run()));
}

Json_Send *Json_Send::bulid(QObject *parent)
{
    static Json_Send* sington = NULL;
    if(sington == NULL) {
        sington = new Json_Send(parent);
    }
    return sington;
}

void Json_Send::initFun()
{
    sendData();

}

void Json_Send::run()
{
//    sendData();
    TcpsendData();
}

void Json_Send::TcpsendData()
{
    QString mHost = "192.168.1.44";
    int port = 2222;
    bool ret = mTcp->newConnect(mHost,port);
    if(ret) {
        QJsonObject bar_json, box_json ; QByteArray ba;

        for(int i = 0;i < BUS_NUM;i++)
        {
            mJson->getStart_Json(bar_json, ba, i);
            ret = mTcp->sentMessage(ba);
            if(!ret) break;
            bar_json.empty(); ba.clear();

            for(int j = 1;j < *(mBoxNum[i]) + 1;j++)
            {
                mJson->getInsert_Json(box_json, ba, i, j);
                ret = mTcp->sentMessage(ba);
                if(!ret) break;
                box_json.empty(); ba.clear();
            }
        }

    }
}
void Json_Send::sendData()
{
    QHostAddress address; bool ret;
    address.setAddress(QString("192.168.1.151"));
    QJsonObject bar_json, box_json ; QByteArray ba;

    for(int i = 0;i < BUS_NUM;i++)
    {
        mJson->getStart_Json(bar_json, ba, i);
        ret = mSocket->sentData(address, ba ,20010);
        if(!ret) break;
        bar_json.empty(); ba.clear();

        for(int j = 1;j < *(mBoxNum[i]) + 1;j++)
        {
            mJson->getInsert_Json(box_json, ba, i, j);
            ret = mSocket->sentData(address, ba ,20010);
            if(!ret) break;
            box_json.empty(); ba.clear();

        }
    }
}
