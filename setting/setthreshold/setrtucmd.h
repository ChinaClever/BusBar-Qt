#ifndef SETRTUCMD_H
#define SETRTUCMD_H

#include "setshm.h"

class SetRtuCmd : public QThread
{
    Q_OBJECT
public:
    explicit SetRtuCmd(QObject *parent = nullptr);

    void send(sThresholdItem &item);
    int sendStartV3(sThresholdItem &item);
    int sendPlugV3(sThresholdItem &item);

//signals:
//    void sendTripSig(QString mac, int val);

protected:
    void sendData(int busID, int addr, ushort reg, uint len);
    void sendReg(int reg, sThresholdItem &item);
    int sendRegV3(int reg, sThresholdItem &item);
    void sendDataUintV3(int busID, int addr, ushort reg, uint val1 , uint val2);
    void sendDataUshortV3(int busID, int addr, ushort reg, uint val1 , uint val2);
    void sendDataUcharV3(int busID, int addr, ushort reg, uint val);
    int sendDataUcharControlV3(int busID, int addr, ushort reg, uint val, uint mac1, uint mac2, uint mac3);
    int getReg(sThresholdItem &item);

};

#endif // SETRTUCMD_H
