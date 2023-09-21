/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "modbus_slaveobj.h"

Modbus_SlaveObj::Modbus_SlaveObj(QObject *parent) : Modbus_Object{parent}
{
    qRegisterMetaType<QModbusDataUnit::RegisterType>("sMbRt");
}

void Modbus_SlaveObj::initConnects()
{
    QTimer::singleShot(3500,this,SLOT(initRecvSlot()));
    connect(mDev, &QModbusServer::stateChanged, this, &Modbus_SlaveObj::onStateChanged);
    connect(mDev, &QModbusServer::errorOccurred, this, &Modbus_SlaveObj::handleDeviceError);
}

void Modbus_SlaveObj::initRecvSlot()
{
    connect(mDev,&QModbusServer::dataWritten,this, &Modbus_SlaveObj::recvDataSlot);
}

bool Modbus_SlaveObj::initUnitMap()
{
    QModbusDataUnitMap reg;
    //reg.insert(QModbusDataUnit::Coils, { QModbusDataUnit::Coils, 9000, 800});
    //reg.insert(QModbusDataUnit::DiscreteInputs, { QModbusDataUnit::DiscreteInputs, 0, 1999 });
    //reg.insert(QModbusDataUnit::InputRegisters, { QModbusDataUnit::InputRegisters, 0, 1999 });
    //reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 8999 });
    reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 50000 });
    bool ret = mDev->setMap(reg);
    if(!ret) throwError("Error: ModbusDataUnitMap");
    return ret;
}

bool Modbus_SlaveObj::checkWriteAddress(int address)
{
    for(int i = 0 ; i < BUS_NUM ; i++){
        for(int j = 0 ; j < BOX_NUM-1 ; j++){
            if(j == 0){
                if(MbMasterReg_Range + 10000*i <= address && MbMasterReg_End + 10000*i >= address){
                    return true;
                }
            }else{
                if(MbSlaveReg_Range + 10000*i + 1000*j <= address && MbSlaveReg_End + 10000*i + 1000*j >= address){
                    return true;
                }
            }
        }
    }
    return false;
}

void Modbus_SlaveObj::recvDataSlot(QModbusDataUnit::RegisterType table, int address, int size)
{
//    if(checkWriteAddress(address)){
//        quint16 value = 0; //cout << table << address << size;
//        for (int i = 0; i < size; ++i) {
//            switch (table) {
//            case QModbusDataUnit::Coils:
//                mDev->data(QModbusDataUnit::Coils, address + i, &value);
//                break;
//            case QModbusDataUnit::HoldingRegisters:
//                mDev->data(QModbusDataUnit::HoldingRegisters, address + i, &value);
//                break;
//            default: value = 0xFF; qDebug() << "Error: Modbus Register Type " << table;
//                continue;
//            } setData(table, address+i, value);
//            emit registerDataSig(address+i,value);
////            cout << table << address+i << size << value;
//        }

//        //QModbusDataUnit rcvData(table, address, size);
//        //if(mDev->data(&rcvData)) {
//         //   emit rcvDataSig(address, rcvData.values());
//            //qDebug() << Q_FUNC_INFO << table << address << size << rcvData.values();
//        //}
//    }
}

bool Modbus_SlaveObj::setData(const QModbusDataUnit &unit)
{
    bool ret = isConnectedModbus(); if(ret) ret = mDev->setData(unit);
    if(!ret) throwError(Q_FUNC_INFO + QString::number(unit.startAddress()));
    return ret;
}

bool Modbus_SlaveObj::setData(QModbusDataUnit::RegisterType table, quint16 address, quint16 data)
{
    return mDev->setData(table, address, data);
}

bool Modbus_SlaveObj::setData(quint16 address, const vshort &values)
{
    QModbusDataUnit unit(m_type, address, values);
    return setData(unit);
}

bool Modbus_SlaveObj::setHoldingRegisters(quint16 address, const vshort &values)
{
    return setData(address, values);
}

bool Modbus_SlaveObj::setCoils(quint16 address, const vshort &values)
{
    QModbusDataUnit unit(QModbusDataUnit::Coils, address, values);
    return setData(unit);
}
