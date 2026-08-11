#include "setoutputpowerwid.h"
#include <QGridLayout>

SetOutputPowerWid::SetOutputPowerWid(QWidget *parent): ComTableWid(parent)
{
    mDc = 1;
    mBus = 0;
    mPacket =  &(get_share_mem()->data[mBus]);
    initWid();
//    QGridLayout *gridLayout = new QGridLayout(parent);//控制ToolBox自适应
//    gridLayout->addWidget(this);

    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

void SetOutputPowerWid::initWid()
{
    QString title; QStringList header;
    if(gLanguage == 0){title = tr("输出位功率");header<< tr("插接箱");}
    else {title = tr("Loop power");header<< tr("Tap-off box");}
    if(mDc){ //交流9个
        for(int i = 0; i < START_LINE_NUM; ++i)
            header << "Output "+QString::number(i+1);
    }

    if(gLanguage == 0){header<< tr("总有功功率");}
    else {header<< tr("Total\nactive power");}
    initTableWid(header, 1, title);
    for(int i = 0 ; i < header.size() ; i++) setTableColumnWidth(i,40); //设置宽度
}



void SetOutputPowerWid::checkBus(int index)
{
    //    if(mBus != index) {
    //        mBus = index;
    mPacket = &(get_share_mem()->data[index]);
    //    }

    int dc = mPacket ? mPacket->box[0].dc : 0;
    if(mDc != dc ) {
        mDc = dc;
        initWid();
    }
    clearTable();
}


int SetOutputPowerWid::updateDev(sBoxData *dev, int row)
{
    if(dev->offLine)
    {
        QStringList list;
        list << dev->boxName;

        int line = START_LINE_NUM + 1;
        for(int i=0; i<line; ++i)
        {
            if(i < START_LINE_NUM){
                double value = dev->outputXBox.outputXPow[i].ivalue/COM_RATE_POW;
                list << QString::number(value ,'f', 3) + "kW";
                setItemColor(row, i+1, dev->outputXBox.outputXPow[i].ialarm);
            }else{
                double value = dev->totalPow.ivalue/COM_RATE_POW;
                list << QString::number(value ,'f', 3) + "kW";
                setItemColor(row, i+1, dev->totalPow.ialarm);
            }
        }

        setTableRow(row, list);
    }

    return ++row;
}

/**
 * @brief 数据更新入口函数
 */
void SetOutputPowerWid::updateData()
{
    int row = 0;

    for(int i=1; i<=mPacket->boxNum; ++i)
    {
        sBoxData *box = &(mPacket->box[i]);
        row = updateDev(box, row);
    }

    checkTableRow(row);
}

void SetOutputPowerWid::timeoutDone()
{
    checkBus(mBus);
    updateData();
}


void SetOutputPowerWid::itemClicked(QTableWidgetItem *it)
{
    if(it->text().compare("---") == 0) return;  //为空不设置
    int column = it->column();
    if(column > 0)
    {
        sThresholdItem item;
        item.bus = mBus;
        item.box = it->row()+1;
        item.num = column-1;
        if(column <= START_LINE_NUM){
            item.type = 18;
        }else if(column > START_LINE_NUM){
            item.type = 19;
        }

        SetThresholdDlg dlg(this);
        dlg.move(0,0);
        dlg.set(item);
        dlg.exec();
    }
}
