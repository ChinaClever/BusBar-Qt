#ifndef MODBUS_MASTERRTU_H
#define MODBUS_MASTERRTU_H
#include "modbus_masterobj.h"
#include <QModbusRtuSerialMaster>

class Modbus_MasterRtu : public Modbus_MasterObj
{
    Q_OBJECT
public:
    explicit Modbus_MasterRtu(QObject *parent = nullptr);
    bool connectModbusRtu(const sModbusSetting &set);
};

#endif // MODBUS_MASTERRTU_H
