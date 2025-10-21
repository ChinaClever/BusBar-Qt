#include "initshm.h"
#include "dbdevname.h"

InitShm::InitShm(QObject *parent) : QThread(parent)
{
    shm = get_share_mem(); // 获取共享内存
}

/**
 * @brief 初始化插接箱数量
 */
void InitShm::initBoxNum()
{
    for(int i=0; i<BUS_NUM; ++i) {
        int boxNum = getBoxNum(i);  //插接箱数
        if(boxNum < 0) boxNum = 18;
        shm->data[i].boxNum = boxNum; // 18个插接箱

        int rateCur = getRateCur(i); // rateCur
        if(rateCur < 0) rateCur = 10000;//2600
        shm->data[i].box[0].ratedCur = rateCur; // 额定电流
    }
}

/**
 * @brief 初始化机柜数量
 */
void InitShm::initCabNum()
{
    for(int i=0; i<BUS_NUM; ++i) {
        int cabNum = getCabNum(i);  //机柜数
        if(cabNum < 0) cabNum = 18*3;
        shm->cabNum[i] = cabNum;
    }
}


void InitShm::initThresholdUnit(int id, sDataUnit &unit, int max)
{
    unit.max[id] = max;
    unit.crMax[id] = max;
}

void InitShm::initThresholdPowUnit(int id, sDataPowUnit &unit, int max)
{
    unit.max[id] = max;
    unit.crMax[id] = max;
}

void InitShm::initBoxThreshold()
{    
    for(int i=0; i<BUS_NUM; ++i) //每个母线下都有
    {
        sBusData *busData = &(shm->data[i]); //母线
        for(int j=1; j<=busData->boxNum; ++j) //值init有效位
        {
            sBoxData *box = &(busData->box[j]); //插接葙
            for(int k=0; k<LINE_NUM_MAX; ++k) //三相
            {
                initThresholdPowUnit(k, box->data.cur, 3200);
                initThresholdUnit(k, box->data.vol, 2750);//480
            }

            for(int k=0; k<SENSOR_NUM; ++k) //两个传感器
            {
                initThresholdUnit(k, box->env.tem, 99);
            }
        }
    }
}

void InitShm::initBusThreshold()
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBoxData *bus = &(shm->data[i].box[0]);
        for(int k=0; k<3; ++k)
        {
            initThresholdUnit( k, bus->data.vol, 2750);//275
            initThresholdPowUnit( k, bus->data.cur, 60000);//600
            initThresholdUnit( k, bus->env.tem, 99);
        }
    }
}

void InitShm::initThreshold()
{
    initBusThreshold();
    initBoxThreshold();
}

void InitShm::initNameUnit(int bus, int type, int num, char *buf, const QString &name)
{
    QString  str = DbDevName::bulid()->getName(bus, type, num); //数据库中取数据
    if(str.isEmpty()) //没有找到就创建并保存
    {
        DbNameItem item;
        item.bus = bus;
        item.type = type;
        item.num = num;
        item.name = name;
        DbDevName::bulid()->saveItem(item);
        str = name;
    }
    msleep(100);

    QByteArray ba = str.toLatin1();
    char *mm = ba.data();
    strcpy(buf,mm);
}


void InitShm::initBusName()
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBusData *busData = &(shm->data[i]);
        initNameUnit(i, 1, 0, busData->busName, QString("Busbar-%1").arg(i+1)); //母线名称各处统一
    }
}

void InitShm::initCabColName()
{
    for(int i=0; i<BUS_NUM/2; ++i){
        QString cabColName = getCabColStr(i);  //机柜列名称
        if(cabColName.isEmpty()) cabColName = QString("Cabinet-Column%1").arg(i+1);
        QByteArray ba = cabColName.toLatin1();
        char *mm = ba.data();
        strcpy(shm->cabColName[i],mm);
    }
}

void InitShm::initCabName()
{
    for(int i=0; i<BUS_NUM/2; ++i){
        for(int j=0; j<CABINET_NUM; ++j){
            QString cabName = "";
            bool ret = getCabNameStr( i , j , cabName);  //机柜名称
            if(!ret || cabName.isEmpty()) cabName = QString("Cabinet%1").arg(j+1);
            QByteArray ba = cabName.toLatin1();
            char *mm = ba.data();
            strcpy(shm->cabData[i][j].cabName,mm);
        }
    }
}

void InitShm::initCabParameters()
{
    for(int i=0; i<BUS_NUM/2; ++i){
        for(int j=0; j<CABINET_NUM; ++j){
            QVector<int> value = getCabParameters( i , j );  //机柜参数
            if(-1 == value[0] || 0 == value[0]) shm->cabData[i][j].capacity = 5000;
            else shm->cabData[i][j].capacity = value[0];
            if(-1 == value[1] || value[1] < 1 || value[1] > 4) shm->cabData[i][j].lineA_No = i==0?1:3;
            else shm->cabData[i][j].lineA_No = value[1];
            if(-1 == value[2] || value[2] < 1 || value[2] > 4) shm->cabData[i][j].lineB_No = i==0?2:4;
            else shm->cabData[i][j].lineB_No = value[2];
            if(-1 == value[3] || value[3] < 2 || value[3] > 19) shm->cabData[i][j].lineA_Tapoff_No = j/3 + 2;
            else shm->cabData[i][j].lineA_Tapoff_No = value[3];
            if(-1 == value[4] || value[4] < 2 || value[4] > 19) shm->cabData[i][j].lineB_Tapoff_No = j/3 + 2;
            else shm->cabData[i][j].lineB_Tapoff_No = value[4];
            if(-1 == value[5] || value[5] < 1 || value[5] > 3) shm->cabData[i][j].lineA_Tapoff_Line = j%3 + 1;
            else shm->cabData[i][j].lineA_Tapoff_Line = value[5];
            if(-1 == value[6] || value[6] < 1 || value[6] > 3) shm->cabData[i][j].lineB_Tapoff_Line = j%3 + 1;
            else shm->cabData[i][j].lineB_Tapoff_Line = value[6];
        }
    }
}

void InitShm::initBoxName()
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBusData *busData = &(shm->data[i]);
        for(int j=1; j<BOX_NUM; ++j)
        {
            sBoxData *box = &(busData->box[j]);
            initNameUnit(i, 2, j, box->boxName, QString("iBox-%1").arg(j));//插接箱名称各处统一
        }
    }
}

void InitShm::initLoopName()
{
    for(int i=0; i<BUS_NUM; ++i)
    {
        sBusData *busData = &(shm->data[i]);
        for(int j=1; j<=busData->boxNum; ++j)
        {
            sBoxData *box = &(busData->box[j]);
            for(int k=0; k<LINE_NUM; ++k) {
                char *loop = box->loopName[k];
                int num = j*LINE_NUM + k;
                initNameUnit(i, 3, num, loop, QString("loop-%1").arg(k+1)); //回路名称
            }
        }
    }
}

void InitShm::initName()
{
    initCabColName();
    initCabName();
    initBusName();
    initBoxName();
    initLoopName();
}

void InitShm::run()
{
    initBoxNum();  //统一ini 与共享内存的数组有效长度
    initCabNum();
    initCabParameters();
    //sleep(5);
    initName(); //统一SQL 与功效内存内neme
    //sleep(5);
    initThreshold(); //统一数据标准

}
