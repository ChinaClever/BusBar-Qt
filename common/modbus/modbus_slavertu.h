#ifndef MODBUS_SLAVERTU_H
#define MODBUS_SLAVERTU_H
#include "modbus_slavetcp.h"

class Modbus_SlaveRtu : public Modbus_SlaveTcp
{
    Q_OBJECT
public:
    explicit Modbus_SlaveRtu(QObject *parent = nullptr);
    bool connectRtu(const sModbusSetting &cfg);
};

#endif // MODBUS_SLAVERTU_H
