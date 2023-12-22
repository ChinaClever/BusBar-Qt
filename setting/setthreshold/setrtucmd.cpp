#include "setrtucmd.h"
#include "rtuthread.h"
extern RtuThread *rtu[5];

SetRtuCmd::SetRtuCmd(QObject *parent) : QThread(parent)
{

}

void SetRtuCmd::sendData(int busID, int addr, ushort reg, uint len)
{
#if (SI_RTUWIFI == 0)
    if((busID == 0xff) || (addr == 0xff)){
        for(int i=0; i<4; ++i) {
            if(rtu[i]) rtu[i]->sendData(addr, reg, len);
        }
    } else {
        if(rtu[busID]) rtu[busID]->sendData(addr, reg, len);
    }
#elif (SI_RTUWIFI == 1)
      insertBusCmd(busID, addr, reg, len);
#endif
}

void SetRtuCmd::sendReg(int reg, sThresholdItem &item)
{
    sendData(item.bus, item.box, reg, item.max);
    sendData(item.bus, item.box, reg+1, item.min);
}

void SetRtuCmd::sendRegV3(int reg, sThresholdItem &item)
{
    if(item.type == 4)
        sendDataUintV3(item.bus, item.box, reg, item.min , item.max);
    else if(item.type == 11 || item.type == 14 )
        sendDataUcharV3(item.bus, item.box, reg, item.min);
    else
        sendDataUshortV3(item.bus, item.box, reg, item.min , item.max);
}

void SetRtuCmd::sendDataUintV3(int busID, int addr, ushort reg, uint val1, uint val2)
{
    if(rtu[busID]) rtu[busID]->sendDataUintV3(addr, reg, val1 , val2);
}

void SetRtuCmd::sendDataUshortV3(int busID, int addr, ushort reg, uint val1, uint val2)
{
    if(rtu[busID]) rtu[busID]->sendDataUshortV3(addr, reg, val1 , val2);
}

void SetRtuCmd::sendDataUcharV3(int busID, int addr, ushort reg, uint val)
{
    if(rtu[busID]) rtu[busID]->sendDataUcharV3(addr, reg, val);
}

void SetRtuCmd::send(sThresholdItem &item)
{
    int reg=0;
//    switch (item.type) {
//    case 1: reg = VoltageMAX_L1 + item.num*2; break;
//    case 2: reg = CurrentMAX_L1 + item.num*2; item.max*=10; item.min*=10; break;
//    case 3: reg = temperatureMAX_1 + item.num*2; break;
//    }
    switch (item.type) {
    case 1: reg = VoltageMAX_L1 + item.num*2; /*item.max*=10; item.min*=10;*/ break;
    case 2: reg = CurrentMAX_L1 + item.num*2; /*item.max*=100; item.min*=100;*/ break;
    case 3: reg = temperatureMAX_1 + item.num*2; break;
    case 4: reg = PowerMAX_L1 + item.num*2; break;
    case 5: reg = SetHzMAX ; break;
    }
    sendReg(reg, item);
}

void SetRtuCmd::sendStartV3(sThresholdItem &item)
{
    int reg=0;
    switch (item.type) {
    case 1: reg = StartVoltageMIN_L1 + item.num*10; /*item.max*=10; item.min*=10;*/ break;
    case 2: reg = StartCurrentMIN_L1 + item.num*10; /*item.max*=100; item.min*=100;*/ break;
    case 3: reg = StartTemperatureMIN_1 + item.num*2; break;
    case 4: reg = StartPowerMIN_L1_1 + item.num*10;break;
    case 5: reg = StartSetHzMIN ; break;
    case 8: reg = StartZoneCurMIN ; break;
    case 11: reg = SetStartRelease ; break;
    case 14: reg = SetStartControlRelease ; break;
    }
    sendRegV3(reg, item);
}

void SetRtuCmd::sendPlugV3(sThresholdItem &item)
{
    int reg=0;
    switch (item.type) {
    case 1: reg = PlugVoltageMIN_L1 + item.num*8; /*item.max*=10; item.min*=10;*/break;
    case 2: reg = PlugCurrentMIN_L1 + item.num*8; /*item.max*=100; item.min*=100;*/break;
    case 3: reg = PlugTemperatureMIN_1 + item.num*2; break;
    case 4: reg = PlugPowerMIN_L1_1 + item.num*8;break;
    case 8: reg = PlugZeroLineMIN;break;
    }
    sendRegV3(reg, item);
}
