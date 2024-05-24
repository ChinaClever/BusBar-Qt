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
signals:

protected:
    void Startbox(QJsonObject &obj,int id);
    void Startbox_pduInfo(QJsonObject &json,int id);
    void Startbox_Alarm(QJsonObject &obj ,int id);
    void Startbox_Data(QJsonObject &obj ,int id);
    int Startbox_Status(int id);

    void Insertbox(QJsonObject &obj,int bus_id,int insert_id);
    void Insertbox_pduInfo(QJsonObject &json,int bus_id,int insert_id);
    void Insertbox_Data(QJsonObject &obj ,int bus_id,int insert_id);
    int Insertbox_Status(int bus_id,int insert_id);
private:
    sBusData *mBusData[BUS_NUM];
    sBoxData *mBoxData[BUS_NUM];
    sProgress *mPro;
    DpAlarmSlave *mAlarm;
    IpSettingDlg *mIp;
};

#endif // JSONPACK_H
