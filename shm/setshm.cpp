#include "setshm.h"

SetShm::SetShm(QObject *parent) : QObject(parent)
{
    shm = get_share_mem(); // 获取共享内存
}

void SetShm::setThresholdUnit(int id, sThresholdItem &item, sDataUnit &unit)
{
    unit.min[id] = item.min;
    unit.max[id] = item.max;
    unit.crMin[id] = item.crmin;
    unit.crMax[id] = item.crmax;
}

void SetShm::setThresholdPowUnit(int id, sThresholdItem &item, sDataPowUnit &unit)
{
    unit.min[id] = item.min;
    unit.max[id] = item.max;
    unit.crMin[id] = item.crmin;
    unit.crMax[id] = item.crmax;
}

void SetShm::setVolAll(sThresholdItem &item)
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBusData *busData = &(shm->data[i]);
        for(int j=0; j<=busData->boxNum; ++j)
        {
            sBoxData *box = &(busData->box[j]);
            for(int k=0; k<LINE_NUM; ++k) {
                setThresholdPowUnit( k, item, box->data.cur);
            }
        }
    }
}

void SetShm::setLoopCurAll(sThresholdItem &item)
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBusData *busData = &(shm->data[i]);
        for(int j=1; j<=busData->boxNum; ++j)
        {
            sBoxData *box = &(busData->box[j]);
            for(int k=0; k<LINE_NUM; ++k) {
                setThresholdPowUnit( k, item, box->data.cur);
            }
        }
    }
}

void SetShm::setLineCurAll(sThresholdItem &item)
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBoxData *bus = &(shm->data[i].box[0]);
        for(int k=0; k<LINE_NUM; ++k)
            setThresholdPowUnit(k, item, bus->data.cur);
    }
}

void SetShm::setLineTempAll(sThresholdItem &item)
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBoxData *bus = &(shm->data[i].box[0]);
        for(int k=0; k<SENSOR_NUM; ++k)
            setThresholdUnit(k, item, bus->env.tem);
    }
}

void SetShm::setLoopTempAll(sThresholdItem &item)
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBusData *busData = &(shm->data[i]);
        for(int j=1; j<=busData->boxNum; ++j)
        {
            sBoxData *box = &(busData->box[j]);
            for(int k=0; k<SENSOR_NUM; ++k) {
                setThresholdUnit( k, item, box->env.tem);
            }
        }
    }
}

void SetShm::saveItem(sThresholdItem &item)
{
    sDataUnit *unit=NULL;
    sDataPowUnit *unitpow=NULL;

    sBusData *bus = &(shm->data[item.bus]);
    sBoxData *box = &(bus->box[item.box]);
    switch(item.type) // 阈值类型 1 电压阈值  2 电流阈值 3 温度
    {
    case 1:
        unit = &(box->data.vol);
        break;

    case 2:
        unitpow = &(box->data.cur);
        break;

    case 3:
        unit = &(box->env.tem);
        break;
    }

    if(unit) {
        setThresholdUnit(item.num, item, (*unit));
    }
    if(unitpow) {
        setThresholdPowUnit(item.num, item, (*unitpow));
    }

}

void SetShm::setItem(sThresholdItem &item)
{
    if(item.box == 0xFF) {
        switch (item.type) {
        case 1:  setVolAll(item); break;
        case 2:  setLoopCurAll(item); break;
        case 3:  setLoopTempAll(item); break;
        }
    } else if(item.bus == 0xFF) {
        switch (item.type) {
        case 1:  setVolAll(item); break;
        case 2:  setLineCurAll(item); break;
        case 3:  setLineTempAll(item); break;
        }
    }else {
        saveItem(item);
    }
}

void SetShm::setName(DbNameItem &item)
{
    char *name = NULL;
    QString prename , msg1 , msgen1;
    int boxNum=0, num = item.num;
    sBusData *bus = &(shm->data[item.bus]);
    QString type = tr("本机%1设置");
    QString typeen = tr("Local %1 settings");
    QString typemame = tr("母线名称");
    QString typemameen;
    switch(item.type) // 名称类型 1 母线名称   2 插接箱名称 3 回路名称
    {
    case 1:{
        name = bus->busName;
        prename = QString(bus->busName);
        typemame = tr("母线名称");typemameen = tr("the busbar name");
        break;
    }
    case 2:{
        name = bus->box[item.num].boxName;
        prename = QString(bus->box[item.num].boxName);
        typemame = tr("插接箱名称");typemameen = tr("the tap-off box name");
        break;
    }
    case 3:{
        boxNum = num / LINE_NUM ;
        num = num % LINE_NUM ;
        name = bus->box[boxNum].loopName[num];
        prename = QString(bus->box[boxNum].loopName[num]);
        typemame = tr("回路名称");typemameen = tr("the loop name");
        break;
    }
    }

//    qDebug()<<" name "<<item.name << " bus "<< item.bus << " box "<<item.box
//             <<" increment " << item.increment<<" type " << item.type<<" num " << item.num
//             <<" id " << item.id<<endl;
    if(name) {
        QByteArray ba = item.name.toLatin1();
        char *mm = ba.data();
        strcpy(name,mm);
        QString str = item.name;
        QString temp;
        if(str.length() >= 1){
            temp = str.right(1);
            str = str.remove(str.size() - 1 , 1);
        }
        int start = item.num;
        if(1 == item.increment){
            for(int i = start , j = 1; i < BOX_NUM - 1 ; i++ , j++){
                item.num = i;
                if( j > 1 ){
                    item.name = str+ QString("%1").arg(temp.toInt()+j-1);
                    strcpy(bus->box[item.num].boxName,item.name.toLatin1().data());
                }
                DbDevName::bulid()->saveItem(item);
            }
            msg1 = tr("从插接箱%1开始统一设置 ").arg(start)+typemame+" !";
            msgen1 = tr("Unified set %1 from tap-off box %2 !").arg(typemameen).arg(start);
            db_operation_obj(item.bus)->insertOperation(type.arg(typemame) , msg1);
            db_operation_obj_en(item.bus)->insertOperation(typeen.arg(typemameen) , msgen1);
        }else{
                DbDevName::bulid()->saveItem(item);
                if(prename != item.name){
                msg1 = tr("%1:将%2改成%3 ！").arg(typemame).arg(prename).arg(item.name);
                msgen1 = tr("%1:set from %2 to %3 !").arg(typemameen).arg(prename).arg(item.name);
                db_operation_obj(item.bus)->insertOperation(type.arg(typemame) , msg1);
                db_operation_obj_en(item.bus)->insertOperation(typeen.arg(typemameen) , msgen1);
            }
        }
    }
}

/**
 * @brief SetShm::setLineRatedCur  设置母线额定电流
 * @param index  母线编号
 * @param data   额定电流值
 */
void SetShm::setLineRatedCur(int index, int data)
{
    sBusData *bus = &(shm->data[index]);
    bus->box[0].ratedCur = data;
    QString str = QString::number(bus->box[0].ratedCur,10);
    QString groupStr = QString("Line%1").arg(index+1);
    sys_configFile_writeParam("rateCur",str,groupStr);
}

/**
 * @brief SetShm::setLineBoxNum  设置母线插接箱数量
 * @param index  母线编号
 * @param num    数量
 */
void SetShm::setLineBoxNum(int index, int num)
{
    sBusData *bus = &(shm->data[index]);
    bus->boxNum = num ;
    QString str = QString::number(bus->boxNum,10);
    QString groupStr = QString("Line%1").arg(index+1);
    sys_configFile_writeParam("boxNum",str,groupStr); //Line1/boxNum
}

/**
 * @brief SetShm::setCabinetNum  设置机柜数量
 * @param index  母线编号
 * @param num    数量
 */
void SetShm::setCabinetNum(int index, int num)
{
    shm->cabNum[index] = num ;
    QString str = QString::number(shm->cabNum[index],10);
    QString groupStr = QString("CabCol%1").arg(index+1);
    cab_configFile_writeParam("CabinetNum",str,groupStr); //CabCol1/2/cabinetNum
}

/**
 * @brief SetShm::setCabinetColName  设置机柜列名称
 * @param index  机柜列编号
 * @param str    机柜列名称
 */
void SetShm::setCabinetColName(int index, const QString &str)
{
    QByteArray ba = str.toLatin1();
    char *mm = ba.data();
    strcpy(shm->cabColName[index],mm);
    QString groupStr = QString("CabCol%1").arg(index+1);
    cab_configFile_writeParam("CabColName",str,groupStr); //CabCol1/2/CabColName
}

/**
 * @brief SetShm::setCabinetParameters  设置机柜名称
 * @param index  机柜列编号
 * @param box    插接箱的地址编号
 * @param str    机柜名称
 */
void SetShm::setCabinetParameters(int index,int box)
{
    QString groupStr = QString("CabCol%1_%2").arg(index+1).arg(box+1);
    cab_configFile_writeParam(tr("CabName_%1").arg(box+1),shm->cabData[index][box].cabName,groupStr); //CabCol1/2/cabinetNum
    cab_configFile_writeParam(tr("Capacity_%1").arg(box+1),QString::number(shm->cabData[index][box].capacity),groupStr);
    cab_configFile_writeParam(tr("LineA_No_%1").arg(box+1),QString::number(shm->cabData[index][box].lineA_No),groupStr);
    cab_configFile_writeParam(tr("LineA_Tapoff_No_%1").arg(box+1),QString::number(shm->cabData[index][box].lineA_Tapoff_No),groupStr);
    cab_configFile_writeParam(tr("LineA_Tapoff_Line_%1").arg(box+1),QString::number(shm->cabData[index][box].lineA_Tapoff_Line),groupStr);

    cab_configFile_writeParam(tr("LineB_No_%1").arg(box+1),QString::number(shm->cabData[index][box].lineB_No),groupStr);
    cab_configFile_writeParam(tr("LineB_Tapoff_No_%1").arg(box+1),QString::number(shm->cabData[index][box].lineB_Tapoff_No),groupStr);
    cab_configFile_writeParam(tr("LineB_Tapoff_Line_%1").arg(box+1),QString::number(shm->cabData[index][box].lineB_Tapoff_Line),groupStr);
}
