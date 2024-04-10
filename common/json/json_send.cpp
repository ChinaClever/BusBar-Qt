#include "json_send.h"

json_send::json_send(QObject *parent): QThread{parent}
{
    mjson = new Json_Pack(this);
    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

void json_send::timeoutDone()
{
    sendData();
}
void json_send::sendData()
{

}
