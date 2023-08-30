/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "modbus_masterrtu.h"

Modbus_MasterRtu::Modbus_MasterRtu(QObject *parent) : Modbus_MasterObj{parent}
{
    mModbus = new QModbusRtuSerialMaster(nullptr);
}

bool Modbus_MasterRtu::connectModbusRtu(const sModbusSetting &set)
{    
    initConnects();
    initModbusSerial(set);
    mModbus->setNumberOfRetries(set.numberOfRetries);
    mModbus->setTimeout(set.responseTime);
    return connectDevice();
}
