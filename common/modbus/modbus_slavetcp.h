#ifndef MODBUS_SLAVETCP_H
#define MODBUS_SLAVETCP_H
#include "modbus_slaveobj.h"
#include <QHostAddress>

class Modbus_SlaveTcp : public Modbus_SlaveObj
{
    Q_OBJECT
public:
    explicit Modbus_SlaveTcp(QObject *parent = nullptr);
    bool connectTcp(int addr, int port); // 502

private:
    bool connectModbusTcp(int port);
};

#endif // MODBUS_SLAVETCP_H
