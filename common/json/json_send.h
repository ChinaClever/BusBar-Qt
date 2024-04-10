#ifndef JSON_SEND_H
#define JSON_SEND_H
#include "json_pack.h"

class json_send:public QThread
{
    Q_OBJECT
    explicit json_send(QObject *parent = nullptr);
public:

protected:
    void sendData();
public slots:
    void timeoutDone();
private:
    Json_Pack *mjson;
    QTimer *timer;
};

#endif // JSON_SEND_H
