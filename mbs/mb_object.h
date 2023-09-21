#ifndef MB_OBJECT_H
#define MB_OBJECT_H
#include "modbus_slavertu.h"

class Mb_Object : public Modbus_SlaveRtu
{
public:
    explicit Mb_Object(QObject *parent = nullptr);
    //bool alarmUnitCheck(int reg, int id, sAlarmUnit *unit, ushort v);

protected:
    void upMasterDevInfo(sBusData *data, int bus, int index);
    void upMasterDevRange(sBusData *data, int bus, int index);
    void upSlaveDevInfo(sBusData *data, int bus, int index);
    void upSlaveDevRange(sBusData *data, int bus, int index);
    bool setReg(ushort reg, const char *str);
    bool setReg(ushort reg, ushort v) {return setRegister(reg, v);}
    bool setRegs(ushort reg, const vshort &v) {return setRegisters(reg, v);}
    bool setReg2(ushort reg, uchar h, uchar l) {return setRegister(reg, getShort(h, l));}

private:
    vshort strToShort(const char *str);
    ushort getShort(uchar h, uchar l) {return ((h<<8) +l);}

protected:
    template<typename T>
    void appendData(uchar size, const T v[], vshort &vs) {
        for(int i=0; i<size; ++i) vs << v[i];
    }

//    void appendAlarm(const sAlarmUnit &unit, vshort &vs) {
//        for(int i=0; i<unit.size; ++i) vs << unit.max[i] << unit.min[i];
//    }

    void appendData2(uchar size, const uint v[], vshort &vs) {
        for(int i=0; i<size; ++i) vs << v[i] / 65536  << v[i] % 65536;
    }

protected:
    sBoxData *dev;
};

#endif // MB_OBJECT_H
