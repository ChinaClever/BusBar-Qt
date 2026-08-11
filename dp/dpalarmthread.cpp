/*
 * dpalarmthread.cpp
 * 告警标志位判定线程
 *
 *
 *  Created on: 2017年10月1日
 *      Author: Lzy
 */
#include "dpalarmthread.h"

DpAlarmThread::DpAlarmThread(QObject *parent) : QThread(parent)
{
    isRun = false;
    shm = get_share_mem(); // 获取共享内存
    QTimer::singleShot(15*1000,this,SLOT(initFunSLot())); //延时初始化
}

DpAlarmThread::~DpAlarmThread()
{
    isRun = false;
    wait();
}

void DpAlarmThread::initFunSLot()
{
    timer = new QTimer(this);
    timer->start(3*1000);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

void DpAlarmThread::timeoutDone()
{
    if(isRun == false) {
        start();
    }
}



void DpAlarmThread::alarmDataUnit(sDataUnit &unit, int lineNum, bool cr)
{
    for(int i=0; i<lineNum; ++i)
    {
        if((unit.value[i] < unit.min[i]) || (unit.value[i] > unit.max[i])){
            if(unit.alarm[i] == 0){
                if(unit.count[i] > 8)unit.alarm[i] = 1;
                else unit.count[i]++;
            }
        } else{
            unit.alarm[i] = 0;
            unit.count[i] = 0;
        }

        if(cr) {
            if((unit.value[i] < unit.crMin[i]) || (unit.value[i] > unit.crMax[i]))
            {
                if(unit.crAlarm[i] == 0)
                    unit.crAlarm[i] = 1;
            } else
                unit.crAlarm[i] = 0;
        }
    }
}


char DpAlarmThread::alarmFlag(sDataUnit &unit, int line, bool cr)
{
    char flag=0;

    for(int i=0; i<line; ++i) {
        flag += unit.alarm[i];
        if(flag) return 2;
    }

    for(int i=0; i<line; ++i) {
        if(cr) flag += unit.crAlarm[i];
        if(flag) return 1;
    }

    return flag;
}

void DpAlarmThread::alarmDataUnit(sDataPowUnit &unit, int lineNum, bool cr)
{
    for(int i=0; i<lineNum; ++i)
    {
        if((unit.value[i] < unit.min[i]) || (unit.value[i] > unit.max[i]))
        {
            if(unit.alarm[i] == 0)
                unit.alarm[i] = 1;
        } else
            unit.alarm[i] = 0;

        if(cr) {
            if((unit.value[i] < unit.crMin[i]) || (unit.value[i] > unit.crMax[i]))
            {
                if(unit.crAlarm[i] == 0)
                    unit.crAlarm[i] = 1;
            } else
                unit.crAlarm[i] = 0;
        }
    }
}

void DpAlarmThread::alarmDataUnit(sRtuULLintUnit &unit ,bool cr)
{

    if(unit.ivalue > unit.imax)
    {
        if(unit.ialarm == 0)
            unit.ialarm = 1;
    } else
        unit.ialarm = 0;

    if(cr) {
        if((unit.ivalue < unit.icrMin) || (unit.ivalue > unit.icrMax))
        {
            if(unit.icrAlarm == 0)
                unit.icrAlarm = 1;
        } else
            unit.icrAlarm = 0;
    }
}


char DpAlarmThread::alarmFlag(sDataPowUnit &unit, int line, bool cr)
{
    char flag=0;

    for(int i=0; i<line; ++i) {
        flag += unit.alarm[i];
        if(flag) return 2;
    }

    for(int i=0; i<line; ++i) {
        if(cr) flag += unit.crAlarm[i];
        if(flag) return 1;
    }

    return flag;
}

void DpAlarmThread::alarmOtherDataUnit(sRtuULLintUnit& box , uchar &alram)
{
    if((box.ivalue < box.imin) || (box.ivalue > box.imax))
    {
        if(alram == 0)
            alram = 1;
        box.ialarm = 1;
    } else{
        alram= 0;
        box.ialarm = 0;
    }
}

void DpAlarmThread::boxAlarm(sBoxData &box , int index )
{
    if(box.offLine > 0) {
        int lineNum = box.loopNum;
        alarmDataUnit(box.data.cur, lineNum); // 回路是否有告警
        box.boxCurAlarm = alarmFlag(box.data.cur, lineNum);

        alarmDataUnit(box.data.vol, lineNum); // 回路是否有告警
        box.boxVolAlarm = alarmFlag(box.data.vol, lineNum);

        alarmDataUnit(box.data.pow, lineNum);
        box.boxPowerAlarm =  alarmFlag(box.data.pow, lineNum);

        //--------------[限制存在才告警]----------------- By_MW 2018.3.23
        if(box.dc){ //交流
            lineNum = 4;
        }
        //---------------------------------------------

        alarmDataUnit(box.env.tem, lineNum);
        box.boxEnvAlarm =  alarmFlag(box.env.tem, lineNum);

        if(index == 0){
            if(box.data.sw[0] == 2 || box.data.sw[0] == 3){
                if(box.data.swAlarm[0] == 0) box.data.swAlarm[0] = 1;
            } else box.data.swAlarm[0] = 0;

            if((box.rate.svalue < box.rate.smin) || (box.rate.svalue > box.rate.smax)){
                if(box.HzAlarm == 0) box.HzAlarm = 1;
            } else box.HzAlarm = 0;

            if((box.totalPow.ivalue < box.totalPow.imin) || (box.totalPow.ivalue > box.totalPow.imax)){
                if(box.totalPowAlarm == 0) box.totalPowAlarm = 1;
            }else box.totalPowAlarm = 0;
            alarmOtherDataUnit(box.zeroLineCur , box.zeroLineAlarm);

            if(box.lpsAlarm == 2 && box.lpsLogAlarm == 0){
                box.lpsLogAlarm = 1;
            }else if(box.lpsAlarm == 0 || box.lpsAlarm == 1){
                box.lpsLogAlarm = 0;
            }
        }

        box.boxOffLineAlarm = 1;
        box.boxAlarm = box.boxCurAlarm + box.boxVolAlarm + box.boxEnvAlarm + box.boxPowerAlarm;
        if(index == 0){//始端箱
            box.boxAlarm += box.data.swAlarm[0];
            box.boxAlarm += box.lpsLogAlarm;
            box.boxAlarm += box.totalPowAlarm;
            box.boxAlarm += box.HzAlarm;
            box.boxAlarm += box.zeroLineAlarm;
        }else{//插接箱
            uchar breaker_num = 3;
            for(int i = 0 ; i < breaker_num ; i++){
                alarmDataUnit(box.outputXBox.outputXPow[i]);
                if(box.outputXBox.outputXPow[i].ialarm){
                    box.boxOutputPowerAlarm = 2;
                }
                box.boxAlarm += box.boxOutputPowerAlarm;
            }

            alarmDataUnit(box.totalPow);
            if(box.totalPow.ialarm){
                box.boxTotalPowerAlarm = 2;
            }
            box.boxAlarm += box.boxOutputPowerAlarm;

            if(box.phaseFlag == 0){//单相
                for(int i  = 0 ; i < box.data.lineNum ; i++){
                    if(box.data.sw[i] == 1){
                        if(box.data.swAlarm[i] == 0) box.data.swAlarm[i] = 1;
                    } else box.data.swAlarm[i] = 0;
                    box.boxAlarm += box.data.swAlarm[i];
                }
            }else if(box.phaseFlag == 1){//三相
                uchar breaker_num = (box.plugbreaker>>12)&0x0F;
                for(int i  = 0 ; i < breaker_num ; i++){
                    uchar sw = (box.plugbreaker>>i*2)&0x03;
                    if(sw == 1){
                        if(box.data.swAlarm[i] == 0) box.data.swAlarm[i] = 1;
                    } else box.data.swAlarm[i] = 0;
                    box.boxAlarm += box.data.swAlarm[i];
                }
            }
            box.boxAlarm += box.boxTotalPowerAlarm + box.boxOutputPowerAlarm;
        }
    } else {
        if(box.boxOffLineAlarm == 1) box.boxOffLineAlarm = 2;
        box.boxAlarm = 0;
    }
}

void DpAlarmThread::busAlarm(sBusData &bus)
{
    for(int i=0; i<=bus.boxNum; ++i) {
        boxAlarm(bus.box[i] , i);
    }
}


void DpAlarmThread::run()
{
    if(isRun == false)
    {
        isRun  = true;

        for(int i=0; i<BUS_NUM; ++i)
            busAlarm(shm->data[i]);

        isRun  = false;
    }
}
