#ifndef JSONPACK_H
#define JSONPACK_H
#include "common.h"
#include <QJsonValue>

class Json_Pack:public QObject
{
    Q_OBJECT
    explicit Json_Pack(QObject *parent = nullptr);
public:
    static Json_Pack *bulid(QObject *parent = nullptr);
    void getJson(QJsonObject &json, QByteArray &ba, int id, int num);

signals:

protected:
    void Startbox(QJsonObject &obj,int id);
    void Startbox_pduInfo(QJsonObject &json,int id);
    void Startbox_ConfigData(QJsonObject &obj,int id);
    void Startbox_StateData(QJsonObject &obj,int id);
    void Startbox_Data(QJsonObject &obj ,int id);

    void Insertbox(QJsonObject &obj,int bus_id,int insert_id);
    void Insertbox_pduInfo(QJsonObject &json,int bus_id,int insert_id);
    void Insertbox_ConfigData(QJsonObject &obj,int bus_id,int insert_id);
    void Insertbox_StateData(QJsonObject &obj,int bus_id,int insert_id);
    void Insertbox_Data(QJsonObject &obj ,int bus_id,int insert_id);
private:
    sBusData *mBusData[BUS_NUM];
    sBoxData *mBoxData[BUS_NUM];
    sProgress *startPro;
    sProgress *insertPro;

};

#endif // JSONPACK_H
