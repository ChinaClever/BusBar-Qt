/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mb_object.h"

Mb_Object::Mb_Object(QObject *parent) : Modbus_SlaveRtu{parent}
{

}

void Mb_Object::upMasterDevInfo(sBusData *data ,int bus, int index)
{
    sBoxData *dev = &(data->box[index]);
    if(dev->offLine > 0){
        vshort vs; //initFucRegs();
        ushort dc = 0;
        if( dev->dc ) dc = 0;
        else dc = 1;
        vs << dev->proNum <<dev->version << dc << dev->curSpecification << 1; // 通讯协议版本
        vs << dev->baudRate << dev->buzzerStatus << dev->alarmTime << dev->lps << dev->iOF;
        vs << dev->isd << dev->shuntRelease << dev->reState << dev->lpsAlarm;
        for(int i = 0 ; i < 40-14 ; i++) vs << 0 ;
        for(int i = 0 ;  i < SENSOR_NUM ; i++){
            vs << dev->env.tem.value[i] << dev->env.tem.upalarm[i];
        }
        vs << (dev->totalApPow >> 16) << (dev->totalApPow & 0xffff);
        vs << (dev->totalPow.ivalue >> 16) << (dev->totalPow.ivalue & 0xffff);
        vs << dev->totalPow.ialarm << dev->reCur.svalue << dev->reCur.supalarm;
        vs << (dev->zeroLineCur.ivalue >> 16)<< (dev->zeroLineCur.ivalue & 0xffff) << dev->zeroLineCur.iupalarm;
        vs << dev->volUnbalance << dev->curUnbalance << dev->data.sw[0];
        vs << dev->rate.svalue << dev->rate.supalarm;
        for(int i = 0 ; i < 90-63 ; i++) vs << 0 ;

        sObjData *p = &(dev->data);
        for(int i = 0 ; i < START_LINE_NUM ; ++i) // 读取相 数据
        {
            vs << p->lineVol.value[i];
            vs << p->lineVol.upalarm[i];
            vs << p->vol.value[i];
            vs << p->vol.upalarm[i];
            vs << (p->cur.value[i] >> 16);
            vs << (p->cur.value[i] & 0xffff);
            vs << p->cur.upalarm[i];
            vs << (p->pow.value[i] >> 16);
            vs << (p->pow.value[i] & 0xffff);
            vs << p->pow.upalarm[i];
            vs << (p->reactivePower[i] >> 16);
            vs << (p->reactivePower[i] & 0xffff);
            vs << (p->apPow[i] >> 16);
            vs << (p->apPow[i] & 0xffff);
            vs << p->pf[i];
            vs << (p->ele[i]>> 16);
            vs << (p->ele[i] & 0xffff);
            vs << p->pl[i];
            for(int j = 0 ;  j < HARMONIC_NUM ; j++){
                vs << data->thdData.volThd[i][j];
            }
            for(int j = 0 ;  j < HARMONIC_NUM ; j++){
                vs << data->thdData.curThd[i][j];
            }
            for(int j = 0 ; j < 200-172 ; j++) vs << 0 ;
        }

        setRegs(MbMasterReg_Factory+10000*bus, vs);
    }else{//clear
        vshort vs; //initFucRegs();
        for(int i = 0 ;  i < MbMasterReg_Range ; i++){
            vs << 0;
        }
        setRegs(MbMasterReg_Factory+10000*bus, vs);
    }
}

void Mb_Object::upMasterDevRange(sBusData *data , int bus, int index)
{
    sBoxData *dev = &(data->box[index]);
    if(dev->offLine > 0){
        vshort vs;
        for(int i = 0 ;  i < SENSOR_NUM ; i++){
            vs << dev->env.tem.min[i] << dev->env.tem.max[i];
        }
        vs << dev->reCur.smax;
        vs << (dev->zeroLineCur.imax >> 16) << (dev->zeroLineCur.imax & 0xffff);
        vs << (dev->totalPow.imin >> 16) << (dev->totalPow.imin & 0xffff);
        vs << (dev->totalPow.imax >> 16) << (dev->totalPow.imax & 0xffff);
        vs << dev->rate.smin << dev->rate.smax;
        for(int i = 0 ; i < START_LINE_NUM ; i++){
            vs << dev->data.lineVol.min[i] << dev->data.lineVol.max[i];
            vs << dev->data.vol.min[i] << dev->data.vol.max[i];
            vs << (dev->data.cur.min[i] >> 16)  << (dev->data.cur.min[i] & 0xffff);
            vs << (dev->data.cur.max[i] >> 16)  << (dev->data.cur.max[i] & 0xffff);
            vs << (dev->data.pow.min[i] >> 16)  << (dev->data.pow.min[i] & 0xffff);
            vs << (dev->data.pow.max[i] >> 16)  << (dev->data.pow.max[i] & 0xffff);
        }
        setRegs(MbMasterReg_Range+10000*bus, vs);
    }else{//clear
        vshort vs; //initFucRegs();
        for(int i = MbMasterReg_Range ;  i <= MbMasterReg_End ; i++){
            vs << 0;
        }
        setRegs(MbMasterReg_Range+10000*bus, vs);
    }
}

void Mb_Object::upSlaveDevInfo(sBusData *data ,int bus, int index)
{
    sBoxData *dev = &(data->box[index]);
    if(dev->offLine > 0){
        vshort vs; //initFucRegs();
        vs << dev->version << dev->proNum << dev->loopNum << (index+1) << dev->baudRate;
        vs << dev->iOF << dev->buzzerStatus << dev->alarmTime<<dev->boxType;
        vs << 0 << 0 << 0 << 0 << 0 << 0 << 0;
        sObjData *p = &(dev->data);
        for(int i = 0 ; i < LOOP_NUM_MAX ; i++)
        {
            if( i < dev->loopNum ){
                vs << p->vol.value[i] << p->vol.upalarm[i];
                vs << p->cur.value[i] << p->cur.upalarm[i];
                vs << ( p->pow.value[i] >> 16 ) << ( p->pow.value[i] & 0xffff);
                vs << p->pow.upalarm[i];
                vs << ( p->apPow[i] >> 16 ) << ( p->apPow[i] & 0xffff);
                vs << ( p->reactivePower[i] >> 16 ) << ( p->reactivePower[i] & 0xffff);
                vs << p->pf[i] << p->sw[i];
                vs << ( p->ele[i] >> 16 ) << ( p->ele[i] & 0xffff);
            }else{
                vs << 0 << 0;
                vs << 0 << 0;
                vs << 0 << 0;
                vs << 0;
                vs << 0 << 0;
                vs << 0 << 0;
                vs << 0 << 0;
                vs << 0 << 0;
            }
        }

        vs << dev->data.curThd[0] << dev->data.curThd[1] << dev->data.curThd[2];
        vs << dev->data.pl[0] << dev->data.pl[1] << dev->data.pl[2];
        for(int i = 0 ;  i < SENSOR_NUM ; i++){
            vs << dev->env.tem.value[i] << dev->env.tem.upalarm[i];
        }
        setRegs(MbSlaveReg_Factory+10000*bus+500*index, vs);
    }else{//clear
        vshort vs; //initFucRegs();
        for(int i = 0 ;  i < MbSlaveReg_Range ; i++){
            vs << 0;
        }
        setRegs(MbSlaveReg_Factory+10000*bus+500*index, vs);
    }
}

void Mb_Object::upSlaveDevRange(sBusData *data , int bus, int index)
{
    sBoxData *dev = &(data->box[index]);
    if(dev->offLine > 0){
        vshort vs;
        for(int i = 0 ;  i < SENSOR_NUM ; i++){
            vs << dev->env.tem.min[i] << dev->env.tem.max[i];
        }
        for(int i = 0 ; i < LOOP_NUM_MAX ; i++){
            if( i < dev->loopNum ){
                vs << dev->data.vol.min[i] << dev->data.vol.max[i];
                vs << dev->data.cur.min[i] << dev->data.cur.max[i];
                vs << (dev->data.pow.min[i] >> 16)  << (dev->data.pow.min[i] & 0xffff);
                vs << (dev->data.pow.max[i] >> 16)  << (dev->data.pow.max[i] & 0xffff);
            }else{
                vs << 0 << 0;
                vs << 0 << 0;
                vs << 0  << 0;
                vs << 0  << 0;
            }
        }
        setRegs(MbSlaveReg_Range+10000*bus+500*index, vs);
    }else{//clear
        vshort vs; //initFucRegs();
        for(int i = MbSlaveReg_Range ;  i <= MbSlaveReg_End ; i++){
            vs << 0;
        }
        setRegs(MbSlaveReg_Range+10000*bus+500*index, vs);
    }
}

bool Mb_Object::setReg(ushort reg, const char *str)
{
    vshort values = strToShort(str);
    return setHoldingRegisters(reg, values);
}

vshort Mb_Object::strToShort(const char *str)
{
    vshort res; ushort buf[32] = {0};
    int size = strlen(str) + 1;
    memcpy(buf, str, size);
    uchar *ptr = (uchar *)str;

    for(int i=0; i<size; i+=2) {
        buf[i] = (ptr[i]<<8) + ptr[i+1];
        res.append(buf[i]);
    }

    return res;
}


//bool Mb_Object::alarmUnitCheck(int reg, int id, sAlarmUnit *unit, ushort v)
//{
//    bool ret = true; switch (reg) {
//    case 1: if((v > unit->rated[id]*1.3) || (v < unit->crMax[id])) ret = false; break;
//    case 2: if((v > unit->max[id]) || (v < unit->crMin[id])) ret = false; break;
//    case 3: if((v > unit->crMax[id]) || (v < unit->min[id])) ret = false; break;
//    case 4: if(v > unit->crMin[id]) ret = false; break;
//    } if(id >= unit->size) ret = false;

//    return ret;
//}
