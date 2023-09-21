#ifndef MB_SETTING_H
#define MB_SETTING_H

#include "mb_object.h"

class Mb_Setting : public Mb_Object
{
public:
    explicit Mb_Setting(QObject *parent = nullptr);

protected:
    void mbSetUpdate();

private:
    void upSetData();

    void addrSet(ushort &v);
    void buzzerSw(ushort &v);
    void startSet(ushort addr, ushort &value);
    void timeSet(ushort addr, ushort &value);
    void datetimeSet(ushort addr, ushort &value);
    void restoreFactoryDefaults();

private slots:
    //void rcvDataSlot(int address, vshort values);
    void registerRecvSlot(int address,ushort value);
};

#endif // MB_SETTING_H
