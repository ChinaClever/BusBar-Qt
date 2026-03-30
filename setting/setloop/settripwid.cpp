#include "settripwid.h"

SetTripWid::SetTripWid(QWidget *parent) : ComTableWid(parent)
{
    mDc = 1;
    mBus = 0;
    mPacket =  &(get_share_mem()->data[mBus]);
    initWid();

    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}


void SetTripWid::initWid()
{
    QString title;QStringList header;
    if(gLanguage == 0){title = tr("分励脱扣");header<< tr("名称")<<tr("蜂鸣器状态")<<tr("打开蜂鸣器")<<tr("关闭蜂鸣器")<<tr("控制分励脱扣");}
    else {title = tr("Shunt trip");header<< tr("Name")<<tr("Buzzer status")<<tr("On buzzer")<<tr("Off buzzer")<<tr("Control shunt trip");}



    initTableWid(header, 1, title);
    for(int i = 0 ; i < header.size() ; i++) setTableColumnWidth(i,40); //设置宽度
}



void SetTripWid::checkBus(int index)
{
    //    if(mBus != index) {
    //        mBus = index;
    mPacket = &(get_share_mem()->data[index]);
    //    }

    //int dc = mPacket ? mPacket->box[0].dc : 0;
    int num =  mPacket->boxNum;
    int rownum = tableWidget->rowCount();
    if(num+1 != rownum ) {
        //mDc = dc;
        initWid();
    }
    clearTable();
}


int SetTripWid::updateDev(sBoxData *dev, int row)
{
    if(dev->offLine){
        QStringList list;
        if(row == 0){
            list << QString(mPacket->busName);
        }else{
            list << QString(dev->boxName);
        }
        if(dev->buzzerStatus == 0) list << "ON";
        else list << "OFF";
        setTableRow(row, list);

        // 第一次创建按钮，避免重复 new
        if(tableWidget->cellWidget(row, 2) == nullptr) {
            QPushButton *btnOn  = new QPushButton("ON", tableWidget);
            QPushButton *btnOff = new QPushButton("OFF", tableWidget);
            QPushButton *btnCtl = new QPushButton("Control", tableWidget);

            tableWidget->setCellWidget(row, 2, btnOn);
            tableWidget->setCellWidget(row, 3, btnOff);
            tableWidget->setCellWidget(row, 4, btnCtl);

            // 信号槽绑定
            connect(btnOn,  &QPushButton::clicked, this, [=](){ handleButtonClick(row, 2); });
            connect(btnOff, &QPushButton::clicked, this, [=](){ handleButtonClick(row, 3); });
            connect(btnCtl, &QPushButton::clicked, this, [=](){ handleButtonClick(row, 4); });
        }
    }else{
        QStringList list;
        list<<"---"<<"---";
        setTableRow(row, list);
    }

    return ++row;
}

/**
 * @brief 数据更新入口函数
 */
void SetTripWid::updateData()
{
    int row = 0;

    for(int i=0; i<=mPacket->boxNum; ++i)
    {
        sBoxData *box = &(mPacket->box[i]);
        row = updateDev(box, row);
    }

    checkTableRow(row);
}

void SetTripWid::timeoutDone()
{
    checkBus(mBus);
    updateData();
}

void SetTripWid::handleButtonClick(int row , int col)
{
    sThresholdItem item;//设置备用断路器
    item.bus = mBus;
    item.box = row;
    if(col == 2)//on
    {
        item.type = 17;
        item.min = 0;
        SetThread::bulid()->append(item);
    }else if(col == 3)//off
    {
        item.type = 17;
        item.min = 1;
        SetThread::bulid()->append(item);
    }else if(col == 4)//control
    {
        QString str;
        QString name = QString(mPacket->busName);
        if(row != 0 ) name = QString(mPacket->box[row].boxName);
        if(gLanguage == 0) str= tr("是否控制%1分励脱扣?").arg(name);
        else str= tr("Do you want to control %1 shunt trip?").arg(name);
        QuMsgBox box(NULL, str);//传入NULL才可以禁止操作其他界面，如果传入this还可以操作其他界面
        bool ret = box.Exec();
        if(ret){
            item.insertlog = 2;
            item.txtype = 0;
            if(row == 0){
                item.type = 14;
            }else{
                item.type = 16;
            }
            item.min = 12;
            SetThread::bulid()->append(item);
        }
    }
}

//void SetTripWid::itemClicked(QTableWidgetItem *it)
//{
//    if(it->text().compare("---") == 0) return;  //为空不设置
//    int column = it->column();
//    if(column > 0)
//    {
//        //BeepThread::bulid()->beep();
//        sThresholdItem item;
//        item.bus = mBus;
//        item.box = it->row()+1;
//        item.num = column-1;
//        item.type = 2;

//        SetThresholdDlg dlg(this);
//        dlg.move(0,0);
//        dlg.set(item);
//        dlg.exec();
//    }
//}
