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
    QString title;
    QStringList header;
    if(gLanguage == 0) title = tr("温度模块");
    else title = tr("Temperature module");

    for(int i=0; i<SENSOR_NUM; ++i){
        if( i <= 2 ){
            if(gLanguage == 0) header << QString('A'+i) + tr("相温度");
            else header << QString(tr("Phase %1 temperature")).arg(QString('A'+i)) ;
        }else{
            if(gLanguage == 0) header << tr("零线温度");
            else header << tr("Neutral line temperature") ;
        }
    }

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



