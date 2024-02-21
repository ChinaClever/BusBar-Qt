#include "temwid.h"
#include <QGridLayout>
TemWid::TemWid(QWidget *parent) : ComTableWid(parent)
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

void TemWid::initWid()
{
    QString title = tr("温度");
    QStringList header;
    header<< tr("名称");

    for(int i=0; i<SENSOR_NUM; ++i)
        header << tr("温度") + QString::number(i+1);

    initTableWid(header, 1, title);
}
void TemWid::initFun(int bus, int box)
{
    sDataPacket *shm = get_share_mem();
    mBox = &(shm->data[bus].box[box]);
    mEnvData = &(shm->data[bus].box[box].env);
    updateData();
}
void TemWid::checkBus(int index)
{
    mPacket = &(get_share_mem()->data[index]);
    clearTable();
}

/**
 * @brief 数据更新入口函数
 */
void TemWid::updateData()
{
    QString str = "---";
    if(mBox->offLine)
    {
        QStringList list;
        list << mBox->boxName;

        sDataUnit *unit = &(mEnvData->tem);
        for(int i=0; i<SENSOR_NUM; ++i)
        {
            double value = unit->value[i] / COM_RATE_TEM;
            str = QString::number(value) + "℃";
            list <<  str;
//          setItemColor(row, i+1, unit->alarm[i]);
        }
    }else{
        QStringList list;
        list << str;

        for(int i=0; i<SENSOR_NUM; ++i) list <<  str;
    }
}

void TemWid::timeoutDone()
{
    checkBus(mBus);
    updateData();
}



