#ifndef JSONPACK_H
#define JSONPACK_H
#include "common.h"
#include <QJsonValue>
#include <QThread>
#include "dpalarmslave.h"
#include "setsystem/ipsettingdlg.h"

class Json_Pack:public QThread
{
    Q_OBJECT
    explicit Json_Pack(QObject *parent = nullptr);
public:
    static Json_Pack *bulid(QObject *parent = nullptr);
    void getStart_Json(QJsonObject &json, QByteArray &ba, int id);
    void getInsert_Json(QJsonObject &json , QByteArray &ba, int id, int num);
    bool delay(int s);
signals:

protected:
    void Startbox(QJsonObject &obj,int id);
    void Startbox_pduInfo(QJsonObject &json,int id);
    bool Startbox_Alarm(QJsonObject &obj ,int id);
    void Startbox_Data(QJsonObject &obj ,int id);
    int Startbox_Status(QJsonObject &obj, int id);

    void Insertbox(QJsonObject &obj,int bus_id,int insert_id);
    void Insertbox_pduInfo(QJsonObject &json,int bus_id,int insert_id);
    void Insertbox_Data(QJsonObject &obj ,int bus_id,int insert_id);
    int Insertbox_Status(int bus_id,int insert_id);
private:
    sBusData *mBusData[BUS_NUM];
    sBoxData *mBoxData[BUS_NUM];
    sProgress *mPro;
    IpSettingDlg *mIp;
    QString m_buslist[BUS_NUM];
};

#endif // JSONPACK_H
