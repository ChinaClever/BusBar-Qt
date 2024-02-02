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

void TemWid::checkBus(int index)
{
    mPacket = &(get_share_mem()->data[index]);
    clearTable();
}

int TemWid::updateDev(sBoxData *dev, int row)
{
    if(dev->offLine)
    {
        QStringList list;
        list << dev->boxName;

        sDataUnit *unit = &(dev->env.tem);
        for(int i=0; i<SENSOR_NUM; ++i)
        {
            double value = unit->value[i] / COM_RATE_TEM;
            list <<  QString::number(value) + "℃";
            setItemColor(row, i+1, unit->alarm[i]);
        }
        setTableRow(row, list);
    }

    return ++row;
}

/**
 * @brief 数据更新入口函数
 */
void TemWid::updateData()
{
    int row = 0;

    for(int i=1; i<=mPacket->boxNum; ++i)
    {
        sBoxData *box = &(mPacket->box[i]);
        row = updateDev(box, row);
    }

    checkTableRow(row);
}

void TemWid::timeoutDone()
{
    checkBus(mBus);
    updateData();
}



