/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "modbus_mastertcp.h"

Modbus_MasterTcp::Modbus_MasterTcp(QObject *parent) : Modbus_MasterObj{parent}
{
    mModbus = new QModbusTcpClient(this);
}

bool Modbus_MasterTcp::connectModbusTcp(const QString &url, int port)
{
    initConnects();
    initModbusNet(port, url);
    mModbus->setNumberOfRetries(3);
    mModbus->setTimeout(1000);
    return connectDevice();
}
