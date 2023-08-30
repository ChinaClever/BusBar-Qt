/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "mb_env.h"

Mb_Env::Mb_Env(QObject *parent) : Mb_Object{parent}
{

}


void Mb_Env::env_dataUpdate()
{
//    vshort vs; int size = SENOR_NUM;
//    sEnvData *obj = &(mDevData->env);
//    for(int i=0; i<size; ++i) {
//        vs << obj->tem.value[i];
//        vs << obj->hum.value[i];
//    }setRegs(MbReg_EnvData, vs);
}

void Mb_Env::env_alarmUpdate()
{
//    vshort vs; int size = SENOR_NUM;
//    sEnvData *obj = &(mDevData->env);
//    for(int i=0; i<size; ++i) {
//        vs << obj->tem.alarm[i];
//        vs << obj->hum.alarm[i];
//    }setRegs(MbReg_EnvAlarm, vs);
}


//void Mb_Env::env_thresholdObj(const sAlarmUnit &unit, int id, vshort &vs)
//{
//    vs << unit.en[id];
//    vs << unit.max[id];
//    vs << unit.crMax[id];
//    vs << unit.crMin[id];
//    vs << unit.min[id];
//}


void Mb_Env::env_thresholdUpdate()
{
//    vshort vs; int size = SENOR_NUM; //obj.size;
//    sEnvData *obj = &(mDevData->env);
//    for(int i=0; i<size; ++i) {
//        env_thresholdObj(obj->tem, i, vs);
//        env_thresholdObj(obj->hum, i, vs);
//    } setRegs(MbReg_EnvThreshol, vs);
}

void Mb_Env::env_sensorUpdate()
{
//    vshort vs;
//    sEnvData *obj = &(mDevData->env);
//    vs << obj->door[0];
//    vs << obj->door[1];
//    vs << obj->water[0];
//    vs << obj->smoke[0];
//    setRegs(MbReg_EnvSensor, vs);
}

void Mb_Env::env_update()
{
    env_dataUpdate();
    env_alarmUpdate();
    env_sensorUpdate();
    env_thresholdUpdate();
}



void Mb_Env::env_setting(ushort addr, ushort value)
{
//    ushort reg = addr - MbReg_EnvThreshol;
//    sEnvData *obj = &(mDevData->env);
//    sAlarmUnit *unit = nullptr;
//    uint *ptr = nullptr;
//    int id = reg/10;

//    switch (reg%10/5) {
//    case 0: unit = &(obj->tem); break;
//    case 1: unit = &(obj->hum); break;
//    default: cout << addr; return;
//    }

//    reg = reg % 5; switch (reg) {
//    case 0: ptr = unit->en; break;
//    case 1: ptr = unit->max; break;
//    case 2: ptr = unit->crMax; break;
//    case 3: ptr = unit->crMin; break;
//    case 4: ptr = unit->min; break;
//    default: cout << addr; break;
//    }

//    bool ret = alarmUnitCheck(reg, id, unit, value);
//    if(ptr && ret) ptr[id] = value;
}

