/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mb_setting.h"
#include "mb_core.h"
#include "setthreshold/setthread.h"


Mb_Setting::Mb_Setting(QObject *parent) : Mb_Object{parent}
{
    connect(this, &Modbus_SlaveObj::registerDataSig, this, &Mb_Setting::registerRecvSlot);
    QDateTime t = QDateTime::currentDateTime();
    for(int i = 0 ; i < BUS_NUM ; i++){
        for(int j = 0 ; j < BOX_NUM ; j++){
            mPreTime[i][j] = t;
            mCount[i][j] = 0;
        }
    }
}

void Mb_Setting::mbSetUpdate()
{
    upSetData();
}

void Mb_Setting::upSetData()
{
//    vshort vs;
//    vs << Mb_Core::modbusCfg.addrRtu;
//    vs << mDevData->cfg.param.buzzerSw;
//    setRegs(MbReg_Setting+1, vs);

    //qint64 timestamp = QDateTime::currentSecsSinceEpoch();
    //vs << timestamp/0xffff; vs << timestamp%0xffff;
    //vs << mDevData->cfg.param.drySw;
    //setRegs(MbReg_SetTime+1, vs);
}


void Mb_Setting::addrSet(ushort &v)
{
//    Cfg_Com::bulid()->writeCfg("addr", v, "modbus");
//    Mb_Core::modbusCfg.addrRtu = v; setAddress(v);
//    mDevData->cfg.param.modbusRtuAddr = v;
//    cout << "modbus set addr OK" << v;
}

void Mb_Setting::buzzerSw(ushort &v)
{
//    mDevData->cfg.param.buzzerSw = v;
//    cout << "modbus set Buzzer switch OK" << v;
//    Cfg_Core::bulid()->devParamWrite("buzzerSw", v, "devParams");
}


void Mb_Setting::startSet(ushort addr, ushort &value)
{
    switch (addr) {
//    case MbReg_SetAddr: addrSet(value); break;
//    case MbReg_SetBuzzer: buzzerSw(value); break;
//    case MbReg_SetEle: break;
    //case MbReg_SetTime: case MbReg_SetTime+1: timeSet(addr, value); break;
    }
}


void Mb_Setting::timeSet(ushort addr, ushort &value)
{
//    static uint t = 0;
//    if(addr%2) {
//        t = value << 16;
//    } else {
//        t += value;
//        QDateTime dt = QDateTime::fromTime_t(t);
//        QString str = dt.toString("yyyy-MM-dd hh:mm:ss");
//        App_Core::bulid()->ntp_time(str); //cout << str;
//    }
}


void Mb_Setting::restoreFactoryDefaults()
{
//    Set_Core::bulid()->factoryRestore();
}

void Mb_Setting::registerRecvSlot(int address, ushort value)
{
//    qDebug()<<"address "<<address <<"value "<<value;
    if( address >= 40000) return;
    sThresholdItem item;
    QDateTime t = QDateTime::currentDateTime();
    item.txtype = 1;
    if(address % 500 == 6){
        item.type = 17;
        if( value > 1 ) return;
        item.bus = address / 10000;
        item.box = (address % 10000) / 500;
        item.min = value;
        SetThread::bulid()->append(item);
        return ;
    }

    if(mPreTime[address / 10000][(address % 10000) / 500].secsTo(t) < 2*60){
        if(mCount[address / 10000][(address % 10000) / 500] == 0){
//            qDebug()<<(address / 10000)<<((address % 10000) / 500)<<"  "<<t.toString("yyyy-MM-dd hh:mm:ss.zzz");
            mCount[address / 10000][(address % 10000) / 500]++;
            mPreTime[address / 10000][(address % 10000) / 500] = t;
        }else{
            if(((address % 10000) / 500 == 0) && address % 500 == 14){
                item.type = 14;
                item.insertlog = 2;
                if( value != 12) return;
            }else if(((address % 10000) / 500 >= 1) && address % 500 == 400){
                item.type = 16;
                item.insertlog = 2;
                if( value != 12) return;
            }
            item.bus = address / 10000;
            item.box = (address % 10000) / 500;
            item.min = value;
//            qDebug()<<t.toString("yyyy-MM-dd hh:mm:ss.zzz")<<"address "<<address <<"value "<<value<<" item.bus "<<item.bus<<
//                " item.type "<<item.type<<" item.box "<<item.box;
//            qDebug()<<mPreTime[address / 10000][(address % 10000) / 500].secsTo(t);
            SetThread::bulid()->append(item);
            mCount[address / 10000][(address % 10000) / 500] = 0;
            mPreTime[address / 10000][(address % 10000) / 500] = t;
        }
    }else{
        mCount[address / 10000][(address % 10000) / 500] = 1;
        mPreTime[address / 10000][(address % 10000) / 500] = t;
    }
}
