/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "modbus_slavertu.h"

Modbus_SlaveRtu::Modbus_SlaveRtu(QObject *parent) : Modbus_SlaveTcp{parent}
{

}

bool Modbus_SlaveRtu::connectRtu(const sModbusSetting &cfg)
{
    bool ret = true; if(!mDev) {
        mDev = new QModbusRtuSerialSlave(this);
        ret = initUnitMap();
        if(ret) initConnects();
    }

    if(ret) initModbusSerial(cfg);
    if(ret) setAddress(cfg.addrRtu);
    if(ret) ret = connectDevice();
    return ret;
}
