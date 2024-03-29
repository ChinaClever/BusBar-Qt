/*
 * dpalarmslave.cpp
 * 告警内容保存，告警信息提示线程
 *
 *
 *  Created on: 2017年10月1日
 *      Author: Lzy
 */
#include "dpalarmslave.h"
#include "dbalarm.h"

static QMutex mutex; // 互拆锁
static QString gEmailStr; // 告警邮件的内容
static QStringList gAlarmStr; // 实时告警内容 存储格式 母线名称，告警类型，告警内容

/**
 * @brief 获取告警邮件的内容
 * @return
 */
QString get_email_str()
{
    QMutexLocker locker(&mutex);
    QString str = gEmailStr;
    gEmailStr.clear();

    return str;
}

/**
 * @brief 获取实时告警内容
 * @return  实时告警内容 存储格式 母线名称，告警类型，告警内容
 */
QStringList get_alarm_str()
{
    QMutexLocker locker(&mutex);
    QStringList str = gAlarmStr;
    gAlarmStr.clear();

    return str;
}

int get_alarm_len()
{
    return gAlarmStr.size();
}


DpAlarmSlave::DpAlarmSlave(QObject *parent) : QThread(parent)
{
    isRun = false;
    shm = get_share_mem(); // 获取共享内存

    timer = new QTimer(this);
    timer->start(5*1000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}


DpAlarmSlave::~DpAlarmSlave()
{
    isRun = false;
    wait();
}

void DpAlarmSlave::timeoutDone()
{
    if(isRun == false) {
        start();
    }
}


void DpAlarmSlave::saveMsg(const QString &typeStr, const QString &str)
{
    DB_Tran tran;
    DbAlarmItem item;
    item.type = typeStr;
    item.msg = str;
    db_alarm_obj(mBusId)->insertItem(item);
    msleep(5);

    QMutexLocker locker(&mutex);
    gEmailStr += "\t" + typeStr + "\t" +str + "\n"; //邮件发送内容
}


void DpAlarmSlave::unitAlarm(QString &typeStr, QString &msg, sDataUnit &unit, double rate, const QString &sym)
{
    int line = START_LINE_NUM;
    if(sym.contains("°C")) line = SENSOR_NUM;
    for(int i=0; i<line; ++i)
    {
        QString str=msg, tempStr = typeStr;
        if(unit.alarm[i])
        {
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(i+1)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str);
            }
        }
        else if(unit.crAlarm[i])
        {
            tempStr = typeStr +  tr("预警");
            str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(i+1)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);
        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tempStr;
            mAlarmStr << str;
        }
    }
}

void DpAlarmSlave::unitAlarm(QString &typeStr, QString &msg, sDataPowUnit &unit, double rate, const QString &sym)
{
    for(int i=0; i<3; ++i)
    {
        QString str=msg, tempStr = typeStr;
        if(unit.alarm[i])
        {
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(i+1)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str);
            }
        }
        else if(unit.crAlarm[i])
        {
            tempStr = typeStr +  tr("预警");
            str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(i+1)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);
        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tempStr;
            mAlarmStr << str;
        }
    }
}

void DpAlarmSlave::unitAlarmVA(sBoxData &box, QString &typeStr, QString &msg, sDataUnit &unit, double rate, const QString &sym)
{
    for(int i=0; i<LINE_NUM_MAX; ++i)
    {
        //---------------------[区分日志关键字]-------------------
        QString alarmStr = "Line";
        int id = i;
        if(box.dc){
            int divisor   =  id/3; //除数
            int remainder =  id%3;//余数
            alarmStr = QString((char)('A' + remainder))+ QString("%1").arg(divisor + 1);
        }else{
            alarmStr = "D" + QString("%1").arg(id+1);
        }
        //---------------------------------------------------

        QString str=msg, tempStr = typeStr;
        if(unit.alarm[i])
        {
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str);
            }
        }
        else if(unit.crAlarm[i])
        {
            tempStr = typeStr +  tr("预警");
            str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);
        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tempStr;
            mAlarmStr << str;
        }
    }
}

void DpAlarmSlave::unitAlarmW(sBoxData &box, QString &typeStr, QString &msg, sDataPowUnit &unit, double rate, const QString &sym)
{
    for(int i=0; i<LINE_NUM_MAX; ++i)
    {
        //---------------------[区分日志关键字]-------------------
        QString alarmStr = "Line";
        int id = i;
        if(box.dc){
            int divisor   =  id/3; //除数
            int remainder =  id%3;//余数
            alarmStr = QString((char)('A' + remainder))+ QString("%1").arg(divisor + 1);
        }else{
            alarmStr = "D" + QString("%1").arg(id+1);
        }
        //---------------------------------------------------

        QString str=msg, tempStr = typeStr;
        if(unit.alarm[i])
        {
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str);
            }
        }
        else if(unit.crAlarm[i])
        {
            tempStr = typeStr +  tr("预警");
            str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);
        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tempStr;
            mAlarmStr << str;
        }
    }
}


void DpAlarmSlave::boxAlarm(sBoxData &box)
{
    if(box.offLine) {
        if(box.boxAlarm)  {
            QString typeStr = tr("回路电流");
            if(box.boxCurAlarm) {
                QString msg = tr("插接箱：%1，").arg(box.boxName);
                unitAlarmVA(box, typeStr, msg, box.data.cur, COM_RATE_CUR, "A");
            }

            typeStr = tr("回路电压");
            if(box.boxVolAlarm) {
                QString msg = tr("插接箱：%1，").arg(box.boxName);
                unitAlarmVA(box, typeStr, msg, box.data.vol, COM_RATE_VOL, "V");
            }

            typeStr = tr("回路功率");
            if(box.boxPowerAlarm) {
                QString msg = tr("插接箱：%1，").arg(box.boxName);
                unitAlarmW(box, typeStr, msg, box.data.pow, COM_RATE_POW , "kW");
            }

            typeStr = tr("插接箱温度");
            if(box.boxEnvAlarm) {
                QString msg = tr("插接箱：%1，温度").arg(box.boxName);
                unitAlarm(typeStr, msg, box.env.tem, COM_RATE_TEM, "°C");
            }
        }
    } else {
        if(box.boxOffLineAlarm == 2){
            QString tempStr ,str;
            tempStr = tr("离线告警");
            str += shm->data[mBusId].busName+tr("插接箱离线")+tr("插接箱：%1 已离线").arg(box.boxName);
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tr("插接箱离线");
            mAlarmStr << tr("插接箱：%1 已离线").arg(box.boxName);
            saveMsg(tempStr,str);
            box.boxOffLineAlarm = 3;
        }else if(box.boxOffLineAlarm == 3){
            QString tempStr ,str;
            tempStr = tr("离线告警");
            str += shm->data[mBusId].busName+tr("插接箱离线")+tr("插接箱：%1 已离线").arg(box.boxName);
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tr("插接箱离线");
            mAlarmStr << tr("插接箱：%1 已离线").arg(box.boxName);
        }
    }
}

void DpAlarmSlave::busAlarm(int id)
{
    mBusId = id;
    sBusData *bus = &(shm->data[id]);
    sBoxData *busBox = &(bus->box[0]);
    //----------------[区分日志关键字]-------------------
    QString alarmStr = "输入";
    if(!busBox->dc){ // 直流
        alarmStr = "输入";
    }
    //--------------------------------------------------
    if(busBox->offLine){
        if(busBox->boxAlarm) {
            if(busBox->boxCurAlarm) { // 总线电流告警
                QString typeStr = tr("主路电流");
                QString msg = tr("母线：%1，%2 ").arg(bus->busName).arg(alarmStr);
                unitAlarm(typeStr, msg, busBox->data.cur, COM_RATE_CUR, "A");
            }

            if(busBox->boxVolAlarm) { // 总线电压告警
                QString typeStr = tr("主路电压");
                QString msg = tr("母线：%1，%2 ").arg(bus->busName).arg(alarmStr);
                unitAlarm(typeStr, msg, busBox->data.vol, COM_RATE_VOL, "V");
            }

            if(busBox->boxEnvAlarm) { // 温度
                QString typeStr = tr("主路温度");
                QString msg = tr("母线：%1，温度").arg(bus->busName);
                unitAlarm(typeStr, msg, busBox->env.tem, COM_RATE_TEM, "°C");
            }

            if(busBox->boxPowerAlarm){
                QString typeStr = tr("主路功率");
                QString msg = tr("母线：%1，功率").arg(bus->busName);
                unitAlarm(typeStr, msg, busBox->data.pow, COM_RATE_POW , "kW");
            }

            if( busBox->HzAlarm == 1) {
                busBox->HzAlarm = 2;
                QString typeStr = tr("主路频率");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr(" 当前值：%2%3, 最小值：%4%5, 最大值：%6%7")
                           .arg(QString::number(busBox->rate.svalue/COM_RATE_FREQUENCY,'f',1)).arg("Hz")
                           .arg(QString::number(busBox->rate.smin/COM_RATE_FREQUENCY,'f',1)).arg("Hz")
                        .arg(QString::number(busBox->rate.smax/COM_RATE_FREQUENCY,'f',1)).arg("Hz");
                saveMsg( typeStr , str );
                mAlarmStr << shm->data[mBusId].busName;
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }
            if( busBox->lpsLogAlarm == 1) {
                busBox->lpsLogAlarm = 2;
                QString typeStr = tr("主路防雷");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr("损坏");
                saveMsg( typeStr , str );
                mAlarmStr << shm->data[mBusId].busName;
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }
            if( busBox->zeroLineAlarm == 1) {
                busBox->zeroLineAlarm = 2;
                QString typeStr = tr("主路零线电流");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr(" 当前值：%2%3, 最小值：%4%5, 最大值：%6%7")
                           .arg(QString::number(busBox->zeroLineCur.svalue/COM_RATE_CUR,'f',2)).arg("A")
                           .arg(QString::number(busBox->zeroLineCur.smin/COM_RATE_CUR,'f',2)).arg("A")
                        .arg(QString::number(busBox->zeroLineCur.smax/COM_RATE_CUR,'f',2)).arg("A");
                saveMsg( typeStr , str );
                mAlarmStr << shm->data[mBusId].busName;
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }
        }
    }else{
        if(busBox->boxOffLineAlarm == 2){
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tr("始端箱离线");
            QString tempStr ,str;
            tempStr = tr("离线告警");
            str += shm->data[mBusId].busName+tr("始端箱离线");
            saveMsg(tempStr,str);
            busBox->boxOffLineAlarm = 3;
        }else if(busBox->boxOffLineAlarm == 3){
            mAlarmStr << shm->data[mBusId].busName;
            mAlarmStr << tr("始端箱离线");
            QString tempStr ,str;
            tempStr = tr("离线告警");
            str += shm->data[mBusId].busName+tr("始端箱离线");
        }
    }

//    if( busBox->lps != 0 )
//    {
//        if( busBox->lpsAlarm == 0 )
//        {
//            busBox->lpsAlarm = 2;
//            QString typeStr = tr("主路防雷");
//            QString str = tr("母线：%1").arg(bus->busName);
//            QString tempStr = typeStr + tr("告警");
//            str += tr("防雷装置损坏");
//            saveMsg( typeStr , str );
//            mAlarmStr << shm->data[mBusId].busName;
//            mAlarmStr << tempStr;
//            mAlarmStr << str;
//        }
//    }
//    else
//        busBox->lpsAlarm = 0;



    for(int i=1; i<=bus->boxNum; ++i) {
        boxAlarm(bus->box[i]);
    }

}

void DpAlarmSlave::checkAlarm()
{
    for(int i=0; i<BUS_NUM; ++i)
        busAlarm(i);

    QMutexLocker locker(&mutex);
    gAlarmStr = mAlarmStr;
    mAlarmStr.clear();
}

void DpAlarmSlave::run()
{
    if(isRun == false)
    {
        isRun  = true;
        checkAlarm();
        isRun  = false;
    }
}
