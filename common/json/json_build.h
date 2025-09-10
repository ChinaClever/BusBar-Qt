#ifndef BUILDJSON_H
#define BUILDJSON_H
#include "datapacket.h"
#include "common.h"

#define SN_TOTAL_NUM 54
#define CABINET_COL_NUM 2

class Json_Build
{
    Json_Build();
public:
    static Json_Build *bulid();
    void getJson(QJsonObject &json);
    QVector<QString> mProduct;

protected:
    void head(QJsonObject &obj);
    void pduInfo(QJsonObject &json);
    int objData( QJsonObject &obj,QVector<QString>& devtype);
    void createDevType( QVector<QString>& devtype);
    bool saveJson(QJsonObject &json);
};

#endif // BUILDJSON_H
