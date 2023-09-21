/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mb_core.h"

sModbusSetting Mb_Core::modbusCfg;
Mb_Core::Mb_Core(QObject *parent) : QThread{parent}
{
    mRtu = new Mb_Update(this);
    mTcp = new Mb_Update(this);
    mCfg = &modbusCfg;
    connect(this, &Mb_Core::connectTcpSig, this, &Mb_Core::connectTcpSlot);
    connect(this, &Mb_Core::connectRtuSig, this, &Mb_Core::connectRtuSlot);
    mTimer = new QTimer(this); mTimer->start(1000);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(run()));
    QTimer::singleShot(135, this, SLOT(initFunSlot()));
}

Mb_Core *Mb_Core::build(QObject *parent)
{
    static Mb_Core* sington = nullptr;
    if(sington == nullptr) {
        sington = new Mb_Core(parent);
    }
    return sington;
}

void Mb_Core::initFunSlot()
{
    mCfg->enTcp = 1;
    mCfg->port = 502;
    mCfg->enRtu = 1;/////////////////

    emit connectTcpSig();
    emit connectRtuSig();
}

void Mb_Core::setAddress(int addr)
{
    mCfg->addrRtu = addr;
    mRtu->setAddress(addr);
}

void Mb_Core::setTcpAddress(int addr)
{
    mCfg->addrTcp = addr;
    mTcp->setAddress(addr);
}

void Mb_Core::setPort(int port)
{
    bool ret = mTcp->isConnectedModbus(); mCfg->port = port;
    if(ret) mTcp->setModbus(QModbusDevice::NetworkPortParameter, port);
}

void Mb_Core::setRtu(int parameter, const QVariant &value)
{
    bool ret = true;
    switch (parameter) {
    case QModbusDevice::SerialPortNameParameter: mCfg->portName = value.toString(); break;
    case QModbusDevice::SerialBaudRateParameter: mCfg->baud = value.toInt(); break;
    case QModbusDevice::SerialParityParameter: mCfg->parity = value.toInt(); break;
    case QModbusDevice::SerialDataBitsParameter: mCfg->dataBits = value.toInt(); break;
    case QModbusDevice::SerialStopBitsParameter: mCfg->stopBits = value.toInt(); break;
    default: ret = false; qDebug() << Q_FUNC_INFO; break;
    } if(ret) mRtu->setModbus(parameter, value);
}

void Mb_Core::connectTcpSlot()
{
    bool ret = false; mTcp->disconnectModbus();
    if(mCfg->enTcp) ret =mTcp->connectTcp(mCfg->addrTcp, mCfg->port);
    if(ret) mTcp->mbUpdates();
}

void Mb_Core::connectRtuSlot()
{
    bool ret = false; mRtu->disconnectModbus();
    int res = 1;
    if(res == 1) {
        ret = mRtu->connectRtu(*mCfg);
        if(ret) mRtu->mbUpdates();
        else qDebug() << mCfg->addrRtu << ret;
    }
}

void Mb_Core::run()
{
    static uint cnt = 0;
    bool ret = true;
    if(cnt++ %2)
    {
        ret = mRtu->isConnectedModbus();
        if(ret) mRtu->mbUpdates();
    }
    else
    {
        ret = mTcp->isConnectedModbus();
        if(ret) mTcp->mbUpdates();
    }
}

