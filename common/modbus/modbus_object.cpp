/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "modbus_object.h"


Modbus_Object::Modbus_Object(QObject *parent)
    : QObject{parent}
{
    m_type = QModbusDataUnit::HoldingRegisters;
}

void Modbus_Object::initModbusSerial(const sModbusSetting &set)
{
    setModbus(QModbusDevice::SerialPortNameParameter, set.portName);
    setModbus(QModbusDevice::SerialBaudRateParameter, set.baud);
    setModbus(QModbusDevice::SerialParityParameter, set.parity);
    setModbus(QModbusDevice::SerialDataBitsParameter, set.dataBits);
    setModbus(QModbusDevice::SerialStopBitsParameter, set.stopBits);

}

void Modbus_Object::setModbus(int parameter, const QVariant &value)
{
    QModbusDevice *dev = modbusDevice();
    if(dev) dev->setConnectionParameter(parameter, value);
    //else qDebug() << Q_FUNC_INFO;
}

void Modbus_Object::initModbusNet(int port, const QString &addr)
{
    QString url = addr; if(url.isEmpty())
        url = QHostAddress(QHostAddress::Any).toString();
    setModbus(QModbusDevice::NetworkPortParameter, port);
    setModbus(QModbusDevice::NetworkAddressParameter, url);
}

void Modbus_Object::throwError(const QString &err)
{
    qDebug() << err << modbusDevice()->errorString();
}

void Modbus_Object::disconnectModbus()
{
    QModbusDevice *dev = modbusDevice();
    if(dev)dev->disconnectDevice();
}

void Modbus_Object::onStateChanged(int state)
{
    QString str;
    switch (state) {
    case QModbusDevice::UnconnectedState: str = tr("Modbus Unconnected"); break;
    case QModbusDevice::ConnectingState: str = tr("Modbus Connecting"); break;
    case QModbusDevice::ConnectedState: str = tr("Modbus Connected"); break;
    case QModbusDevice::ClosingState: str = tr("Modbus Closing"); break;
    } //qDebug() << str;
}

void Modbus_Object::handleDeviceError(QModbusDevice::Error newError)
{
    if(newError != QModbusDevice::NoError)
        throwError("Error Modbus");
}

bool Modbus_Object::connectDevice()
{    
    QModbusDevice *dev = modbusDevice();
    bool ret = false; if(dev) {
        //if(dev->state() == QModbusDevice::ConnectedState) disconnectModbus();
        ret = dev->connectDevice();
        if(ret) ret = waitForState(QModbusDevice::ConnectedState);
    }
    return ret;
}

bool Modbus_Object::isConnectedModbus()
{
    bool ret = false; QModbusDevice *dev = modbusDevice();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    if(dev) if(dev->state() == QModbusDevice::ConnectedState) ret = true;
    //qDebug() << "Error: Modbus Device is not connected!";
    return ret;
}

bool Modbus_Object::waitForState(int state)
{
    bool ret = false;
    QModbusDevice *dev = modbusDevice();
    for(int i=0; i<1000; i+=1) {
        mdelay(1); if(dev->state() == state) {ret = true; break;}
    } if(!ret) throwError("Wait For State");
    return ret;
}

bool Modbus_Object::replyFinished(QModbusReply *reply)
{
    bool ret = false;
    for(int i=0; i<1000; i+=10) {
        if (reply->error() == QModbusDevice::NoError) {
            ret = reply->isFinished(); if(ret) break; else mdelay(10);
        } else {
            QModbusPdu::ExceptionCode code = reply->rawResult().exceptionCode();
            qDebug() << tr("modbus response error:%1 (modbus exception:0x%2)")
                        .arg(reply->errorString()).arg(code, -1, 16); break;
        }
    }
    return ret;
}
