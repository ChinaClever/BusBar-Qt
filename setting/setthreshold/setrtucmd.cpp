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

int SetRtuCmd::sendRegV3(int reg, sThresholdItem &item)
{
    int ret = 0;
    if(item.type == 4 || (item.box == 0 && item.type == 2) || (item.box == 0 && item.type == 8))
        sendDataUintV3(item.bus, item.box, reg, item.min , item.max);
    else if(item.type == 11 || item.type == 14 || item.type == 15 || item.type == 17 || item.type == 20)
        sendDataUcharV3(item.bus, item.box, reg, item.min);
    else if(item.type == 16 || item.type == 21)
        ret = sendDataUcharControlV3(item.bus, item.box, reg, item.min , item.crmin , item.crmax , item.max);
    else if(item.type == 2 && item.curSpec == 1)
        sendDataUintV3(item.bus, item.box, reg, item.min , item.max);
    else
        sendDataUshortV3(item.bus, item.box, reg, item.min , item.max);
    return ret;
}

void SetRtuCmd::sendDataUintV3(int busID, int addr, ushort reg, uint val1, uint val2)
{
    if((busID == 0xff) || (addr == 0xff - 1)){
        for(int i=0; i<4; ++i) {
            if(rtu[i]) rtu[i]->sendDataUintV3(addr, reg, val1 , val2);
        }
    }else{
        if(rtu[busID]) rtu[busID]->sendDataUintV3(addr, reg, val1 , val2);
    }
}

void SetRtuCmd::sendDataUshortV3(int busID, int addr, ushort reg, uint val1, uint val2)
{
    if((busID == 0xff) || (addr == 0xff - 1)){
        for(int i=0; i<4; ++i) {
            if(rtu[i]) rtu[i]->sendDataUshortV3(addr, reg, val1 , val2);
        }
    }else{
        if(rtu[busID]) rtu[busID]->sendDataUshortV3(addr, reg, val1 , val2);
    }
}

void SetRtuCmd::sendDataUcharV3(int busID, int addr, ushort reg, uint val)
{
    if((busID == 0xff) || (addr == 0xff - 1)){
        for(int i=0; i<4; ++i) {
            if(rtu[i]) rtu[i]->sendDataUcharV3(addr, reg, val);
        }
    }else{
        if(rtu[busID]) rtu[busID]->sendDataUcharV3(addr, reg, val);
    }
}

int SetRtuCmd::sendDataUcharControlV3(int busID, int addr, ushort reg, uint val , uint mac1 , uint mac2 , uint mac3)
{
    int ret = 0;
    if(rtu[busID]) ret = rtu[busID]->sendDataUcharControlV3(addr, reg, val);
    if(ret != 6){
        quint8 bytes[6];
        bytes[0] = (mac1 >> 8) & 0xFF;
        bytes[1] = mac1 & 0xFF;
        bytes[2] = (mac2 >> 8) & 0xFF;
        bytes[3] = mac2 & 0xFF;
        bytes[4] = (mac3 >> 8) & 0xFF;
        bytes[5] = mac3 & 0xFF;

        // 拼成 MAC 地址字符串
        QString mac;
        for (int i = 0; i < 6; i++) {
            mac += QString("%1").arg(bytes[i], 2, 16, QLatin1Char('0')).toUpper();
            if (i < 5){
                mac += ":";
            }
        }
        emit sendTripSig(mac, val);
    }
    return ret;
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

int SetRtuCmd::sendStartV3(sThresholdItem &item)
{
    int reg=0;
    switch (item.type) {
    case 1: reg = StartVoltageMIN_L1 + item.num*12; /*item.max*=10; item.min*=10;*/ break;
    case 2: reg = StartCurrentMIN_L1_1 + item.num*12; /*item.max*=100; item.min*=100;*/ break;
    case 3: reg = StartTemperatureMIN_1 + item.num*2; break;
    case 4: reg = StartPowerMIN_L1_1 + item.num*12;break;
    case 5: reg = StartSetHzMIN ; break;
    case 8: reg = StartZoneCurMAX_1 ; break;
    case 11: reg = SetStartRelease ; break;
    case 14: reg = SetStartControlRelease ; break;
    case 17: reg = SetStartBuzzer;break;
    case 20: reg = SetStartID + item.num;break;
    }
    return sendRegV3(reg, item);
}

int SetRtuCmd::sendPlugV3(sThresholdItem &item)
{
    int reg=0;
    switch (item.type) {
    case 1: reg = PlugVoltageMIN_L1 + item.num*8; /*item.max*=10; item.min*=10;*/break;
    case 2:{
        if(item.curSpec==0)
            reg = PlugCurrentMIN_L1 + item.num*8;
        else
            reg = PlugCurrentMIN_HIGH_L1 + item.num*4;/*item.max*=100; item.min*=100;*/
        }break;
    case 3: reg = PlugTemperatureMIN_1 + item.num*2; break;
    case 4: reg = PlugPowerMIN_L1_1 + item.num*8;break;
    case 15: reg = SetPlugBackupBreaker;break;
    case 16: reg = PlugShuntRelease;break;
    case 17: reg = SetPlugBuzzer;break;
    case 20: reg = SetPlugBoxID + item.num;break;
    case 21: reg = PlugShuntReleaseLegrandRCA;break;
    }
    return sendRegV3(reg, item);
}
