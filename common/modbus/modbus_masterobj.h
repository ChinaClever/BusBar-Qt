#ifndef MODBUS_MASTEROBJ_H
#define MODBUS_MASTEROBJ_H

#include "modbus_object.h"

class Modbus_MasterObj : public Modbus_Object
{
    Q_OBJECT
public:
    explicit Modbus_MasterObj(QObject *parent = nullptr);
    int writeCoils(int addr, int start, const vshort &data);
    int writeHoldingRegisters(int addr, int start, const vshort &data);

    QVector<quint16> readCoils(int addr, int start, int num);
    QVector<quint16> readHoldingRegisters(int addr, int start, int num);

protected:
    void initConnects();
    QModbusDevice *modbusDevice() override {return mModbus;}

private:
    void writeRequest(const QModbusDataUnit &dataunit, int serverAddress);
    int writeModbus(int addr, const QModbusDataUnit &unit);
    int writeModbus(int addr, int start, const vshort &data);
    int writeModbus(int addr, QModbusDataUnit::RegisterType table,
                    int start, const vshort &data);

    QVector<quint16> readModbus(int addr, int start, int num);
    QVector<quint16> readModbus(int addr, const QModbusDataUnit &unit);
    QVector<quint16> readModbus(int addr, QModbusDataUnit::RegisterType table, int start, int num);

protected:
    QModbusClient *mModbus;
};

#endif // MODBUS_MASTEROBJ_H
