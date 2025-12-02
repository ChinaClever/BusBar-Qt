#include "setbreakerwid.h"

SetBreakerWid::SetBreakerWid(QWidget *parent) : ComTableWid(parent)
{
    mDc = 1;
    mBus = 0;
    mPacket =  &(get_share_mem()->data[mBus]);
    initWid();

    getSwAlramParameters(mPacket , mBus);
    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}


void SetBreakerWid::initWid()
{
    QString title;QStringList header;
    if(gLanguage == 0){title = tr("备用断路器设置");header<< tr("插接箱");}
    else {title = tr("Backup circuit breaker setting");header<< tr("Tap-off box");}

    if(mDc){ //交流9个
        for(int i = 0; i < LINE_NUM; ++i)
            header << QString((char)('A' + i%3))+ QString("%1").arg(i/3 + 1);
    }else{ //直流4个
        for(int i = 0; i < 4; i++)
            header << "D" + QString("%1").arg(i + 1);
    }

    initTableCheckboxWid(header, 1, title);
    for(int i = 0 ; i < header.size() ; i++) setTableColumnWidth(i,40); //设置宽度
}



void SetBreakerWid::checkBus(int index)
{
    //    if(mBus != index) {
    //       index = mBus;
    mPacket = &(get_share_mem()->data[index]);
    //    }

    int dc = mPacket ? mPacket->box[0].dc : 0;
    if(mDc != dc) {
        mDc = dc;
        initWid();
    }
    clearCheckboxTable();
}


int SetBreakerWid::updateDev(sBoxData *dev, int row)
{
    //if(dev->offLine)
    {
        //int flag = dev->phaseFlag;
        int flag = 1;
        setTableCheckboxRow(row, QString(dev->boxName) ,flag);
    }

    return ++row;
}

/**
 * @brief 数据更新入口函数
 */
void SetBreakerWid::updateData()
{
    int row = 0;
    for(int i=1; i<=mPacket->boxNum; ++i)
    {
        sBoxData *box = &(mPacket->box[i]);
        row = updateDev(box, row);
    }

    checkTableCheckboxRow(row);
}

void SetBreakerWid::timeoutDone()
{
    getSwAlramParameters(mPacket , mBus);
    checkBus(mBus);
    updateData();
    getCheckboxState(mPacket);
}


void SetBreakerWid::itemClicked(QTableWidgetItem *it)
{
    //if(it->text().compare("---") == 0) return;  //为空不设置
    //int column = it->column();
    //if(column > 0)
    //{
    //    //BeepThread::bulid()->beep();
    //    sThresholdItem item;
    //    item.bus = mBus;
    //    item.box = it->row()+1;
    //    item.num = column-1;
    //    item.type = 2;

    //    SetThresholdDlg dlg(this);
    //    dlg.move(0,0);
    //    dlg.set(item);
    //    dlg.exec();
    //}
}

void SetBreakerWid::onCheckBoxStateChanged(int state,sBusData * packet, int row, int col)
{
    if(packet->box[row].data.swAlarmSend[col] != state){
        packet->box[row].data.swAlarmSend[col] = state;
        QString groupStr = QString("Bus%1").arg(mBus+1);
        sw_alram_configFile_writeParam(QString("swAlram_%1_%2").arg(row+1).arg(col+1),QString::number(state),groupStr);
        bool ret = checkBuzzerStatus(packet , row);
        sThresholdItem item;
        item.bus = mBus;
        item.box = row;
        item.type = 15;
        if(ret == false){item.min = 1;mItem = item;}
        else{item.min = 0;mItem = item;}
        SetThread::bulid()->append(mItem);
    }
}

bool SetBreakerWid::checkBuzzerStatus(sBusData * packet,int row)
{
    for(int i = 0 ; i < START_LINE_NUM ; i++){
        if( packet->box[row].data.swAlarmSend[i] == 0) continue;
        else{
            return false;
        }
    }
    return true;
}
