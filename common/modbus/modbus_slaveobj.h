#ifndef MODBUS_SLAVEOBJ_H
#define MODBUS_SLAVEOBJ_H

#include "modbus_mastertcp.h"

class Modbus_SlaveObj : public Modbus_Object
{
    Q_OBJECT
public:
    explicit Modbus_SlaveObj(QObject *parent = nullptr);
    void setAddress(int addr) {if(mDev) mDev->setServerAddress(addr);}

    bool setCoil(quint16 reg, ushort v){return setCoils(reg, vshort{v});}
    bool setCoils(quint16 address, const vshort &values);

    bool setHoldingRegister(quint16 reg, ushort v) {return setHoldingRegisters(reg, vshort{v});}
    bool setHoldingRegisters(quint16 address, const vshort &values);

    bool setRegister(ushort reg, ushort v) {return setHoldingRegister(reg, v);}
    bool setRegisters(ushort reg, const vshort &v) {return setHoldingRegisters(reg, v);}

signals:
    void registerDataSig(int address, ushort value);
    void rcvDataSig(int address, vshort values);

private slots:
    void initRecvSlot();
    void recvDataSlot(QModbusDataUnit::RegisterType table, int address, int size);

protected:
    void initConnects();
    bool initUnitMap();
    QModbusDevice *modbusDevice() override {return mDev;}

private:
    bool setData(const QModbusDataUnit &unit);
    bool setData(quint16 address, const vshort &values);
    bool setData(QModbusDataUnit::RegisterType table, quint16 address, quint16 data);

protected:
    QModbusServer* mDev=nullptr;
};

#endif // MODBUS_SLAVEOBJ_H
