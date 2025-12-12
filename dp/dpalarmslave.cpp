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
static QStringList mAlarmJson;
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
QStringList get_alarm_json()
{
    QMutexLocker locker(&mutex);
    QStringList str = mAlarmJson;
    mAlarmJson.clear();

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



void DpAlarmSlave::saveMsg(const QString &typeStr, const QString &str , const QString &typeStrEn, const QString &strEn)
{
    QMutexLocker locker(&mutex);
    DB_Tran tran;
    DbAlarmItem item;
    item.type = typeStr;
    item.msg = str;
    DbAlarmItem itemen;
    itemen.type = typeStrEn;
    itemen.msg = strEn;
    db_alarm_obj(mBusId)->insertItem(item);
    db_alarm_obj_en(mBusId)->insertItem(itemen);
    msleep(5);


    gEmailStr += "\t" + typeStr + "\t" +str + "\n"; //邮件发送内容
}


void DpAlarmSlave::unitAlarm(QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataUnit &unit, double rate, const QString &sym)
{
    int line = START_LINE_NUM;
    if(sym.contains("°C")) line = SENSOR_NUM;
    for(int i=0; i<line; ++i)
    {
        QString str = msg, tempStr = typeStr , strEn = msgEn , tempStrEn = typeStrEn;
        if(unit.alarm[i])
        {

            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(i+1)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            tempStrEn = typeStrEn + tr(" Alarm");
            strEn += tr("%1，current value：%2%3, minimum value：%4%5, maximum value：%6%7").arg(i+1)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str, typeStrEn, strEn);
            }
        }
        else if(unit.crAlarm[i])
        {

                tempStr = typeStr +  tr("预警");
                str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(i+1)
                        .arg(unit.value[i]/rate).arg(sym)
                        .arg(unit.crMin[i]/rate).arg(sym)
                        .arg(unit.crMax[i]/rate).arg(sym);

                tempStrEn = typeStrEn +  tr(" Warning");
                strEn += tr("%1，current value：%2%3, critical lower limit：%4%5, critical upper limit：%6%7").arg(i+1)

                        .arg(unit.value[i]/rate).arg(sym)
                        .arg(unit.crMin[i]/rate).arg(sym)
                        .arg(unit.crMax[i]/rate).arg(sym);

        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << strEn;
            }
        }
    }
}

void DpAlarmSlave::unitAlarm(QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataPowUnit &unit, double rate, const QString &sym)
{
    for(int i=0; i<3; ++i)
    {
        QString str=msg, tempStr = typeStr;
        QString strEn = msgEn , tempStrEn = typeStrEn;
        if(unit.alarm[i])
        {
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(QString('A'+i))
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            tempStrEn = typeStrEn + tr(" Alarm");
            strEn += tr("%1，current value：%2%3, minimum value：%4%5, maximum value：%6%7").arg(QString('A'+i))
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str , typeStrEn , strEn);
            }
        }
        else if(unit.crAlarm[i])
        {
            tempStr = typeStr +  tr("预警");
            str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(QString('A'+i))
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);

            tempStrEn = typeStrEn +  tr(" Warning");
            strEn += tr("%1，current value：%2%3, critical lower limit：%4%5, critical upper limit：%6%7").arg(QString('A'+i))
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);
        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << strEn;
            }
        }
    }
}

void DpAlarmSlave::unitAlarmVA(int bus , int addr , sBoxData &box, QString &typeStr, QString &msg,QString &typeStrEn, QString &msgEn, sDataUnit &unit, double rate, const QString &sym)
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
        QString str=msg, tempStr = typeStr, strEn=msgEn, tempStrEn = typeStrEn;
        if(unit.alarm[i])
        {
            int index = 0;
            if(box.phaseFlag == 0) index = i;
            else index = i/3;
            QString cabColName, cabName;
            getCabColNameAndCabName(bus , addr , index , cabColName ,cabName);
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);
            str = tr("机柜列名称：%1,机柜名称：%2,").arg(cabColName).arg(cabName) + str;

            tempStrEn = typeStrEn + tr(" Alarm");
            strEn += tr("%1，current value：%2%3, minimum value：%4%5, maximum value：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);

            strEn = tr("cabColName:%1,cabName:%2,").arg(cabColName).arg(cabName) + strEn;

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str ,typeStrEn , strEn);
            }
        }
        else if(unit.crAlarm[i])
        {

            tempStr = typeStr +  tr("预警");
            str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);

            tempStrEn = typeStrEn +  tr(" Warning");
            strEn += tr("%1，current value：%2%3, critical lower limit：%4%5, critical upper limit：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.crMin[i]/rate).arg(sym)
                    .arg(unit.crMax[i]/rate).arg(sym);

        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << strEn;
            }
        }
    }
}

void DpAlarmSlave::unitAlarmW(int bus , int addr ,sBoxData &box, QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataPowUnit &unit, double rate, const QString &sym)
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
        QString str=msg, tempStr = typeStr , strEn = msgEn, tempStrEn = typeStrEn;
        if(unit.alarm[i])
        {
            int index = 0;
            if(box.phaseFlag == 0) index = i;
            else index = i/3;
            QString cabColName, cabName;
            getCabColNameAndCabName(bus , addr , index , cabColName ,cabName);
            tempStr = typeStr + tr("告警");
            str += tr("%1，当前值：%2%3, 最小值：%4%5, 最大值：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);
            str = tr("机柜列名称：%1,机柜名称：%2,").arg(cabColName).arg(cabName) + str;

            tempStrEn = typeStrEn + tr(" Alarm");
            strEn += tr("%1，current value：%2%3, minimum value：%4%5, maximum value：%6%7").arg(alarmStr)
                    .arg(unit.value[i]/rate).arg(sym)
                    .arg(unit.min[i]/rate).arg(sym)
                    .arg(unit.max[i]/rate).arg(sym);
            strEn = tr("cabColName:%1,cabName:%2,").arg(cabColName).arg(cabName) + strEn;

            if(unit.alarm[i] == 1){
                unit.alarm[i] = 2;
                saveMsg(typeStr, str,typeStrEn, strEn);
            }
        }
        else if(unit.crAlarm[i])
        {

                tempStr = typeStr +  tr("预警");
                str += tr("%1，当前值：%2%3, 临界下限值：%4%5, 临界上限值：%6%7").arg(alarmStr)
                        .arg(unit.value[i]/rate).arg(sym)
                        .arg(unit.crMin[i]/rate).arg(sym)
                        .arg(unit.crMax[i]/rate).arg(sym);

                tempStrEn = typeStrEn +  tr(" Warning");
                strEn += tr("%1，current value：%2%3, critical lower limit：%4%5, critical upper limit：%6%7").arg(alarmStr)
                        .arg(unit.value[i]/rate).arg(sym)
                        .arg(unit.crMin[i]/rate).arg(sym)
                        .arg(unit.crMax[i]/rate).arg(sym);
        }

        // 实时告警信息
        if((unit.alarm[i]) || (unit.crAlarm[i])) {
            mAlarmStr << shm->data[mBusId].busName;
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << str;
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << strEn;
            }
        }
    }
}

//need to test??????
void DpAlarmSlave::getCabColNameAndCabName(int line_no , int line_tapoff_no ,int line_tapoff_line , QString & cabCalName , QString & cabname)
{

    cabCalName = QString(shm->cabColName[line_no]);
    int cabnum = shm->cabNum[line_no];
    for(int m = 0 ; m < cabnum ; m++){
        int line_no_id_A = shm->cabData[line_no][m].lineA_No;
        int line_tapoff_no_id_A = shm->cabData[line_no][m].lineA_Tapoff_No;
        int line_tapoff_line_id_A = shm->cabData[line_no][m].lineA_Tapoff_Line;

        int line_no_id_B = shm->cabData[line_no][m].lineB_No;
        int line_tapoff_no_id_B = shm->cabData[line_no][m].lineB_Tapoff_No;
        int line_tapoff_line_id_B = shm->cabData[line_no][m].lineB_Tapoff_Line;
        if( (line_no_id_A == line_no + 1 && line_tapoff_no_id_A == line_tapoff_no + 1 && line_tapoff_line_id_A == line_tapoff_line + 1)
            ||(line_no_id_B == line_no + 1 && line_tapoff_no_id_B == line_tapoff_no + 1 && line_tapoff_line_id_B == line_tapoff_line + 1)){
            cabname = QString(shm->cabData[line_no][m].cabName);
        }
    }
}

void DpAlarmSlave::boxAlarm(int bus , sBoxData &box , int id)
{
    if(box.offLine) {
        if(box.boxAlarm){
            QString typeStr = tr("回路电流");
            QString typeStrEn = tr("Loop current");
            if(box.boxCurAlarm) {
                QString msg = tr("插接箱：%1，").arg(box.boxName);
                QString msgEn = tr("Tap-off box：%1，").arg(box.boxName);
                unitAlarmW(bus , id , box, typeStr, msg, typeStrEn, msgEn, box.data.cur, COM_RATE_CUR, "A");
            }

            typeStr = tr("回路电压");
            typeStrEn = tr("Loop voltage");
            if(box.boxVolAlarm) {
                QString msg = tr("插接箱：%1，").arg(box.boxName);
                QString msgEn = tr("Tap-off box：%1，").arg(box.boxName);
                unitAlarmVA(bus , id , box, typeStr, msg, typeStrEn , msgEn , box.data.vol, COM_RATE_VOL, "V");
            }

            typeStr = tr("回路功率");
            typeStrEn = tr("Loop power");
            if(box.boxPowerAlarm) {
                QString msg = tr("插接箱：%1，").arg(box.boxName);
                QString msgEn = tr("Tap-off box：%1，").arg(box.boxName);
                unitAlarmW(bus , id , box, typeStr, msg, typeStrEn, msgEn, box.data.pow, COM_RATE_POW , "kW");
            }

            typeStr = tr("插接箱温度");
            typeStrEn = tr("Tap-off box temperature");
            if(box.boxEnvAlarm) {
                QString msg = tr("插接箱：%1，温度").arg(box.boxName);
                QString msgEn = tr("Tap-off box：%1，temperature").arg(box.boxName);
                unitAlarm(typeStr,msg, typeStrEn , msgEn, box.env.tem, COM_RATE_TEM, "°C");
            }
            if(box.phaseFlag == 0){
                for(int i = 0 ; i < box.data.lineNum ; i++){
                    if( box.data.swAlarm[i] ) {
                        QString typeStr = tr("断路器");
                        QString str = tr("插接箱：%1").arg(box.boxName);
                        QString tempStr = typeStr + tr("告警");
                        QString statueStr = QString(tr(" 第 %1 个回路断开")).arg( i + 1 );

                        QString typeStrEn = tr("breaker");
                        QString strEn = tr("Tap-off box：%1").arg(box.boxName);
                        QString tempStrEn = typeStrEn + tr("Alarm");
                        QString tempEn = "";
                        if(i == 0) tempEn = "st";
                        else if( i == 1 ) tempEn = "nd";
                        else if( i == 2 ) tempEn = "rd";
                        else tempEn = "th";
                        QString statueStrEn = QString(tr(" %1%2 loop OFF")).arg( i + 1 ).arg(tempEn);
                        if(box.version >= 223){
                            QString cabColName, cabName;
                            getCabColNameAndCabName(bus , id , i , cabColName ,cabName);
                            str = tr("机柜列名称：%1,机柜名称：%2,").arg(cabColName).arg(cabName) + str;
                            strEn = tr("cabColName:%1,cabName:%2,").arg(cabColName).arg(cabName) + strEn;

                            statueStr = QString(tr(" 第 %1 个断路器断开")).arg( i + 1 );
                            statueStrEn = QString(tr(" %1%2 breaker OFF")).arg( i + 1 ).arg(tempEn);
                        }
                        str += statueStr;
                        strEn += statueStrEn;
                        if(box.data.swAlarm[i] == 1){
                            box.data.swAlarm[i]= 2;
                            saveMsg( typeStr , str , typeStrEn , strEn );
                        }
                        mAlarmStr << shm->data[mBusId].busName;
                        if(gLanguage == 0){
                            mAlarmStr << tempStr;
                            mAlarmStr << str;
                        }else{
                            mAlarmStr << tempStrEn;
                            mAlarmStr << strEn;
                        }
                    }
                }
            }else if(box.phaseFlag == 1){
                uchar breaker_num = (box.plugbreaker>>12)&0x0F;
                for(int i = 0 ; i < breaker_num ; i++){
                    if( box.data.swAlarm[i] ) {
                        QString cabColName, cabName;
                        getCabColNameAndCabName(bus , id , i , cabColName ,cabName);
                        QString typeStr = tr("断路器");
                        QString str = tr("插接箱：%1").arg(box.boxName);
                        QString tempStr = typeStr + tr("告警");
                        QString statueStr = QString(tr(" 第 %1 个断路器断开")).arg( i + 1 );
                        QString typeStrEn = tr("breaker");
                        QString strEn = tr("Tap-off box：%1").arg(box.boxName);
                        QString tempStrEn = typeStrEn + tr("Alarm");
                        QString tempEn = "";
                        if(i == 0) tempEn = "st";
                        else if( i == 1 ) tempEn = "nd";
                        else if( i == 2 ) tempEn = "rd";
                        else tempEn = "th";
                        QString statueStrEn = QString(tr(" %1%2 breaker OFF")).arg( i + 1 ).arg(tempEn);
                        str += statueStr;
                        strEn += statueStrEn;
                        str = tr("机柜列名称：%1,机柜名称：%2,").arg(cabColName).arg(cabName) + str;
                        strEn = tr("cabColName:%1,cabName:%2,").arg(cabColName).arg(cabName) + strEn;
                        if(box.data.swAlarm[i] == 1){
                            box.data.swAlarm[i]= 2;
                            saveMsg( typeStr , str , typeStrEn , strEn );
                        }
                        mAlarmStr << shm->data[mBusId].busName;
                        if(gLanguage == 0){
                            mAlarmStr << tempStr;
                            mAlarmStr << str;
                        }else{
                            mAlarmStr << tempStrEn;
                            mAlarmStr << strEn;
                        }
                    }//box.data.swAlarm[i]
                }//for breaker_num
            }//else if(box.phaseFlag == 1)
        }//box.boxAlarm
    } else {
        if(box.boxOffLineAlarm == 2){
            QString tempStr ,str , tempStrEn ,strEn;
            tempStr = tr("离线告警");
            str += shm->data[mBusId].busName+tr("插接箱离线")+tr("插接箱：%1 已离线").arg(box.boxName);

            tempStrEn = tr("Offline alarm");
            strEn += shm->data[mBusId].busName+tr("Tap-off box offline")+tr("Tap-off box：%1 is offline").arg(box.boxName);

            mAlarmStr << shm->data[mBusId].busName;

            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << tr("插接箱：%1 已离线").arg(box.boxName);
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << tr("Tap-off box：%1 is offline").arg(box.boxName);
            }
            saveMsg(tempStr,str,tempStrEn,strEn);
            box.boxOffLineAlarm = 3;
        }else if(box.boxOffLineAlarm == 3){
            QString tempStr ,str , tempStrEn ,strEn;
            tempStr = tr("离线告警");
            str += shm->data[mBusId].busName+tr("插接箱离线")+tr("插接箱：%1 已离线").arg(box.boxName);

            tempStrEn = tr("Offline alarm");
            strEn += shm->data[mBusId].busName+tr("Tap-off box offline")+tr("Tap-off box：%1 is offline").arg(box.boxName);

            mAlarmStr << shm->data[mBusId].busName;
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << tr("插接箱：%1 已离线").arg(box.boxName);
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << tr("Tap-off box：%1 is offline").arg(box.boxName);
            }
        }
    }
}

void DpAlarmSlave::busAlarm(int id)
{
    mBusId = id;
    sBusData *bus = &(shm->data[id]);
    sBoxData *busBox = &(bus->box[0]);
    //----------------[区分日志关键字]-------------------
    QString alarmStr , alarmStrEn;

    alarmStr = "输入 ";
    if(!busBox->dc){ // 直流
        alarmStr = "输入 ";
    }
    alarmStrEn = "input ";
    if(!busBox->dc){ // 直流
        alarmStrEn = "input ";
    }
    //--------------------------------------------------
    if(busBox->offLine){
        if(busBox->boxAlarm) {
            if(busBox->boxCurAlarm) { // 总线电流告警
                QString typeStr = tr("主路电流");
                QString msg = tr("母线：%1，%2 ").arg(bus->busName).arg(alarmStr);

                QString typeStrEn = tr("Input current");
                QString msgEn = tr("Busbar：%1，%2 ").arg(bus->busName).arg(alarmStrEn);
                unitAlarm(typeStr, msg, typeStrEn, msgEn, busBox->data.cur, COM_RATE_CUR, "A");
            }

            if(busBox->boxVolAlarm) { // 总线电压告警
                QString typeStr = tr("主路电压");
                QString msg = tr("母线：%1，%2 ").arg(bus->busName).arg(alarmStr);

                QString typeStrEn = tr("Input voltage");
                QString msgEn = tr("Busbar：%1，%2 ").arg(bus->busName).arg(alarmStrEn);
                unitAlarm(typeStr, msg, typeStrEn, msgEn, busBox->data.vol, COM_RATE_VOL, "V");
            }

            if(busBox->boxEnvAlarm) { // 温度
                QString typeStr = tr("主路温度");
                QString msg = tr("母线：%1，温度").arg(bus->busName);

                QString typeStrEn = tr("Input temperature");
                QString msgEn = tr("Busbar：%1，temperature").arg(bus->busName);
                unitAlarm(typeStr, msg, typeStrEn, msgEn, busBox->env.tem, COM_RATE_TEM, "°C");
            }

            if(busBox->boxPowerAlarm){
                QString typeStr = tr("主路功率");
                QString msg = tr("母线：%1，功率").arg(bus->busName);

                QString typeStrEn = tr("Input power");
                QString msgEn = tr("Busbar：%1，power").arg(bus->busName);
                unitAlarm(typeStr, msg, typeStrEn, msgEn, busBox->data.pow, COM_RATE_POW , "kW");
            }

            if( busBox->HzAlarm ) {
                QString typeStr = tr("主路频率");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr(" 当前值：%2%3, 最小值：%4%5, 最大值：%6%7")
                           .arg(QString::number(busBox->rate.svalue/COM_RATE_FREQUENCY,'f',1)).arg("Hz")
                           .arg(QString::number(busBox->rate.smin/COM_RATE_FREQUENCY,'f',1)).arg("Hz")
                        .arg(QString::number(busBox->rate.smax/COM_RATE_FREQUENCY,'f',1)).arg("Hz");

                QString typeStrEn = tr("Input frequency");
                QString strEn = tr("Busbar：%1").arg(bus->busName);
                QString tempStrEn = typeStrEn + tr("Alarm");
                strEn += tr(" current value：%2%3, minimum value：%4%5, maximum value：%6%7")
                           .arg(QString::number(busBox->rate.svalue/COM_RATE_FREQUENCY,'f',1)).arg("Hz")
                           .arg(QString::number(busBox->rate.smin/COM_RATE_FREQUENCY,'f',1)).arg("Hz")
                        .arg(QString::number(busBox->rate.smax/COM_RATE_FREQUENCY,'f',1)).arg("Hz");
                if(busBox->HzAlarm  == 1){
                    busBox->HzAlarm = 2;
                    saveMsg( typeStr , str , typeStrEn , strEn);
                }
                mAlarmStr << shm->data[mBusId].busName;
                if(gLanguage == 0){
                    mAlarmStr << tempStr;
                    mAlarmStr << str;
                }else{
                    mAlarmStr << tempStrEn;
                    mAlarmStr << strEn;
                }
            }
            if( busBox->totalPowAlarm ) {
                QString typeStr = tr("主路总功率");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr(" 当前值：%2%3, 最小值：%4%5, 最大值：%6%7")
                           .arg(QString::number(busBox->totalPow.ivalue/COM_RATE_POW,'f',3)).arg("kW")
                           .arg(QString::number(busBox->rate.smin/COM_RATE_FREQUENCY,'f',3)).arg("kW")
                        .arg(QString::number(busBox->rate.smax/COM_RATE_FREQUENCY,'f',3)).arg("kW");

                QString typeStrEn = tr("Input total power");
                QString strEn = tr("Busbar：%1").arg(bus->busName);
                QString tempStrEn = typeStrEn + tr("Alarm");
                strEn += tr(" current value：%2%3, minimum value：%4%5, maximum value：%6%7")
                           .arg(QString::number(busBox->totalPow.ivalue/COM_RATE_POW,'f',3)).arg("kW")
                           .arg(QString::number(busBox->rate.smin/COM_RATE_FREQUENCY,'f',3)).arg("kW")
                        .arg(QString::number(busBox->rate.smax/COM_RATE_FREQUENCY,'f',3)).arg("kW");
                if(busBox->totalPowAlarm == 1){
                    busBox->totalPowAlarm = 2;
                    saveMsg( typeStr , str , typeStrEn , strEn);
                }
                mAlarmStr << shm->data[mBusId].busName;
                if(gLanguage == 0){
                    mAlarmStr << tempStr;
                    mAlarmStr << str;
                }else{
                    mAlarmStr << tempStrEn;
                    mAlarmStr << strEn;
                }
            }
            if( busBox->lpsLogAlarm ) {
                QString typeStr = tr("主路防雷 ");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr("防雷损坏");
                QString typeStrEn = tr("Input lightning protection ");
                QString strEn = tr("Busbar：%1 ").arg(bus->busName);
                QString tempStrEn = typeStrEn + tr("Alarm");
                strEn += tr("lightning protection damage");
                if(busBox->lpsLogAlarm == 1){
                    busBox->lpsLogAlarm = 2;
                    saveMsg( typeStr , str , typeStrEn , strEn);
                }
                mAlarmStr << shm->data[mBusId].busName;
                if(gLanguage == 0){
                    mAlarmStr << tempStr;
                    mAlarmStr << str;
                }else{
                    mAlarmStr << tempStrEn;
                    mAlarmStr << strEn;
                }
            }
            if( busBox->zeroLineAlarm ) {
                QString typeStr = tr("主路零线电流");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                str += tr(" 当前值：%2%3, 最小值：%4%5, 最大值：%6%7")
                           .arg(QString::number(busBox->zeroLineCur.ivalue/COM_RATE_CUR,'f',3)).arg("A")
                           .arg(QString::number(busBox->zeroLineCur.imin/COM_RATE_CUR,'f',3)).arg("A")
                        .arg(QString::number(busBox->zeroLineCur.imax/COM_RATE_CUR,'f',3)).arg("A");
                QString typeStrEn = tr("Input neutral line current");
                QString strEn = tr("Busbar：%1").arg(bus->busName);
                QString tempStrEn = typeStrEn + tr("Alarm");
                strEn += tr(" current value:%2%3, minimum value：%4%5, maximum value：%6%7")
                           .arg(QString::number(busBox->zeroLineCur.ivalue/COM_RATE_CUR,'f',3)).arg("A")
                           .arg(QString::number(busBox->zeroLineCur.imin/COM_RATE_CUR,'f',3)).arg("A")
                        .arg(QString::number(busBox->zeroLineCur.imax/COM_RATE_CUR,'f',3)).arg("A");
                if(busBox->zeroLineAlarm == 1){
                    busBox->zeroLineAlarm = 2;
                    saveMsg( typeStr , str , typeStrEn , strEn);
                }
                mAlarmStr << shm->data[mBusId].busName;
                if(gLanguage == 0){
                    mAlarmStr << tempStr;
                    mAlarmStr << str;
                }else{
                    mAlarmStr << tempStrEn;
                    mAlarmStr << strEn;
                }
            }

            if( busBox->data.swAlarm[0] ) {
                QString typeStr = tr("主路断路器");
                QString str = tr("母线：%1").arg(bus->busName);
                QString tempStr = typeStr + tr("告警");
                QString statueStr = tr("");
                QString typeStrEn = tr("Input breaker");
                QString strEn = tr("Busbar：%1").arg(bus->busName);
                QString tempStrEn = typeStrEn + tr("Alarm");
                QString statueStrEn = tr("");
                if(busBox->data.sw[0] == 2){ statueStr = tr("断开");statueStrEn = tr("OFF");}
                else if(busBox->data.sw[0] == 3){ statueStr = tr("跳闸");statueStrEn = tr("TRIP");}
                str += tr(" %1 ").arg(statueStr);
                strEn += tr(" %1 ").arg(statueStrEn);
                if(busBox->data.swAlarm[0] == 1){
                    busBox->data.swAlarm[0]= 2;
                    saveMsg( typeStr , str , typeStrEn , strEn);
                }
                mAlarmStr << shm->data[mBusId].busName;
                if(gLanguage == 0){
                    mAlarmStr << tempStr;
                    mAlarmStr << str;
                }else{
                    mAlarmStr << tempStrEn;
                    mAlarmStr << strEn;
                }
            }
        }
    }else{
        if(busBox->boxOffLineAlarm == 2){
            mAlarmStr << shm->data[mBusId].busName;
            QString tempStr ,str , tempStrEn ,strEn;
            tempStr = tr("离线告警");
            tempStrEn = tr("Offline alarm");
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << tr("始端箱离线");
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << tr("Feeder box offline");
            }
            str += shm->data[mBusId].busName+tr("始端箱离线");
            strEn += shm->data[mBusId].busName+tr("Feeder box offline");
            saveMsg( tempStr , str , tempStrEn , strEn);
            busBox->boxOffLineAlarm = 3;
        }else if(busBox->boxOffLineAlarm == 3){
            mAlarmStr << shm->data[mBusId].busName;
            QString tempStr ,str , tempStrEn ,strEn;
            tempStr = tr("离线告警");
            tempStrEn = tr("Offline alarm");
            if(gLanguage == 0){
                mAlarmStr << tempStr;
                mAlarmStr << tr("始端箱离线");
            }else{
                mAlarmStr << tempStrEn;
                mAlarmStr << tr("Feeder box offline");
            }
            str += shm->data[mBusId].busName+tr("始端箱离线");
            strEn += shm->data[mBusId].busName+tr("Feeder box offline");
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
        boxAlarm(id , bus->box[i] , i);
    }

}

void DpAlarmSlave::checkAlarm()
{
    for(int i=0; i<BUS_NUM; ++i)
        busAlarm(i);

    QMutexLocker locker(&mutex);
    gAlarmStr = mAlarmStr;
    mAlarmJson = mAlarmStr;
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
