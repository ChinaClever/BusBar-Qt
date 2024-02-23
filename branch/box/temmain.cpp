#include "temmain.h"
#include <QGridLayout>
TemMain::TemMain(QWidget *parent) : ComTableWid(parent)
{
    mBus = 0;
    mPacket =  &(get_share_mem()->data[mBus]);
    initWid();
    QGridLayout *gridLayout = new QGridLayout(parent);//控制ToolBox自适应
    gridLayout->addWidget(this);

    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

void TemMain::initWid()
{
    QString title = tr("温度模块");
    QStringList header;


    for(int i=0; i<SENSOR_NUM; ++i)
        header << tr("温度") + QString::number(i+1);

    initTableWid(header, 1, title);
}
void TemMain::initFun(int bus, int box)
{
    sDataPacket *shm = get_share_mem();
    mBox = &(shm->data[bus].box[box]);
    mEnvData = &(shm->data[bus].box[box].env);
    updateData();
}
void TemMain::checkBus(int index)
{
    mPacket = &(get_share_mem()->data[index]);
    clearTable();
}

/**
 * @brief 数据更新入口函数
 */
void TemMain::updateData()
{
    QString str = "---";
    QStringList list;
    if(mBox->offLine)
    {
        sDataUnit *unit = &(mEnvData->tem);
        for(int i=0; i<SENSOR_NUM; ++i)
        {
            double value = unit->value[i] / COM_RATE_TEM;
            str = QString::number(value) + "℃";
            list <<  str;
        }
    }else{
        for(int i=0; i<SENSOR_NUM; ++i) list <<  str;
    }
    setTableRow(0, list);
}

void TemMain::timeoutDone()
{
    checkBus(mBus);
    updateData();
}



