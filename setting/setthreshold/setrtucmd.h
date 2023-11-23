#ifndef SETRTUCMD_H
#define SETRTUCMD_H

#include "setshm.h"

class SetRtuCmd : public QThread
{
    Q_OBJECT
public:
    explicit SetRtuCmd(QObject *parent = nullptr);

    void send(sThresholdItem &item);
    void sendStartV3(sThresholdItem &item);
    void sendPlugV3(sThresholdItem &item);

protected:
    void sendData(int busID, int addr, ushort reg, uint len);
    void sendReg(int reg, sThresholdItem &item);
    void sendRegV3(int reg, sThresholdItem &item);
    void sendDataUintV3(int busID, int addr, ushort reg, uint val1 , uint val2);
    void sendDataUshortV3(int busID, int addr, ushort reg, uint val1 , uint val2);
    void sendDataUcharV3(int busID, int addr, ushort reg, uint val);
    int getReg(sThresholdItem &item);

};

#endif // SETRTUCMD_H
