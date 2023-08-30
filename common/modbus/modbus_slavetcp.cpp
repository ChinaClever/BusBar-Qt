/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "modbus_slavetcp.h"

Modbus_SlaveTcp::Modbus_SlaveTcp(QObject *parent) : Modbus_SlaveObj{parent}
{

}

bool Modbus_SlaveTcp::connectModbusTcp(int port)
{
    initModbusNet(port);
    bool ret = connectDevice();
    if(ret) qDebug() << "Modbus Tcp connect" << port << ret;
    else qDebug() << "Modbus Tcp connect" << port << ret;
    return ret;
}

bool Modbus_SlaveTcp::connectTcp(int addr, int port)
{
    if(!mDev) {
        mDev = new QModbusTcpServer(this);
        //setAddress(addr);
        initUnitMap();
        initConnects();
    }setAddress(addr);

    return connectModbusTcp(port);;
}
