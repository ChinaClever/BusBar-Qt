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
//    connect(SetThread::bulid() , SIGNAL(sendSetThreadTripSig(QString,int)) , this , SLOT(recvSetThreadTripSlot(QString,int)));
}


void SetTripWid::initWid()
{
    QString title;QStringList header;
    if(gLanguage == 0){title = tr("分励脱扣和RCA");header<< tr("名称")<<tr("MAC(ro)")<<tr("ID1")<<tr("ID2")<<tr("ID3")<<tr("蜂鸣器状态")<<tr("打开蜂鸣器")<<tr("关闭蜂鸣器")<<tr("分励脱扣控制")<<tr("RCA合闸")<<tr("RCA分闸");}
    else {title = tr("Shunt trip and RCA");header<< tr("Name")<<tr("MAC(ro)")<<tr("ID1")<<tr("ID2")<<tr("ID3")<<tr("Buzzer status")<<tr("On buzzer")<<tr("Off buzzer")<<tr("Control shunt trip")<<tr("Turn on\ntrip")<<tr("Turn off\ntrip");}

    initTableWid(header, 1, title);
    disconnect(tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
    connect(tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(itemClicked(QTableWidgetItem*)));
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
        quint8 bytes[6];
        bytes[0] = (dev->boxId[0] >> 8) & 0xFF;
        bytes[1] = dev->boxId[0] & 0xFF;
        bytes[2] = (dev->boxId[1] >> 8) & 0xFF;
        bytes[3] = dev->boxId[1] & 0xFF;
        bytes[4] = (dev->boxId[2] >> 8) & 0xFF;
        bytes[5] = dev->boxId[2] & 0xFF;

        // 拼成 MAC 地址字符串
        QString mac;
        for (int i = 0; i < 6; i++) {
            mac += QString("%1").arg(bytes[i], 2, 16, QLatin1Char('0')).toUpper();
            if (i < 5){
                mac += ":";
                if( i == 2) mac += "\n";
            }
        }
        list<<mac;
        for(int i = 0 ; i < 3 ; i++) list<< QString::number(dev->boxId[i]);
        if(row != 0){
            if(dev->buzzerStatus == 0) list << "ON";
            else list << "OFF";
        }else{
            list<<"---"<<"---"<<"---"<<"---";
        }

        setTableRow(row, list);

        // 第一次创建按钮，避免重复 new
        if(row != 0 && tableWidget->cellWidget(row, 6) == nullptr) {
            QPushButton *btnOn  = new QPushButton("ON", tableWidget);
            QPushButton *btnOff = new QPushButton("OFF", tableWidget);
            QPushButton *btnCtrl = new QPushButton("OFF", tableWidget);
            QPushButton *btnOnCtl = new QPushButton("ON\nRCA", tableWidget);
            QPushButton *btnOffCtl = new QPushButton("OFF\nRCA", tableWidget);

            tableWidget->setCellWidget(row, 6, btnOn);
            tableWidget->setCellWidget(row, 7, btnOff);
            tableWidget->setCellWidget(row, 8, btnCtrl);
            tableWidget->setCellWidget(row, 9, btnOnCtl);
            tableWidget->setCellWidget(row, 10, btnOffCtl);

            // 信号槽绑定
            connect(btnOn,  &QPushButton::clicked, this, [=](){ handleButtonClick(row, 6); });
            connect(btnOff, &QPushButton::clicked, this, [=](){ handleButtonClick(row, 7); });
            connect(btnCtrl, &QPushButton::clicked, this, [=](){ handleButtonClick(row, 8); });
            connect(btnOnCtl, &QPushButton::clicked, this, [=](){ handleButtonClick(row, 9); });
            connect(btnOffCtl, &QPushButton::clicked, this, [=](){ handleButtonClick(row, 10); });
        }
    }else{
        QStringList list;
        list<<"---"<<"---";
        setTableRow(row, list);
    }

    return ++row;
}

QString SetTripWid::transformerMac(sBoxData *dev)
{
    quint8 bytes[6];
    bytes[0] = (dev->boxId[0] >> 8) & 0xFF;
    bytes[1] = dev->boxId[0] & 0xFF;
    bytes[2] = (dev->boxId[1] >> 8) & 0xFF;
    bytes[3] = dev->boxId[1] & 0xFF;
    bytes[4] = (dev->boxId[2] >> 8) & 0xFF;
    bytes[5] = dev->boxId[2] & 0xFF;

    // 拼成 MAC 地址字符串
    QString mac;
    for (int i = 0; i < 6; i++) {
        mac += QString("%1").arg(bytes[i], 2, 16, QLatin1Char('0')).toUpper();
        if (i < 5){
            mac += ":";
        }
    }
    return mac;
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
    if(col == 6)//on
    {
        item.type = 17;
        item.min = 0;
        SetThread::bulid()->append(item);
    }else if(col == 7)//off
    {
        item.type = 17;
        item.min = 1;
        SetThread::bulid()->append(item);
    }else if(col == 8)//off trip control
    {
        QString str;
        QString name = QString(transformerMac(&mPacket->box[row]));
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
            item.crmin = mPacket->box[row].boxId[0];
            item.crmax = mPacket->box[row].boxId[1];
            item.max = mPacket->box[row].boxId[2];
            SetThread::bulid()->append(item);//old
            SetThread::bulid()->append(item);//old
        }

    }else if(col == 9)//turn off trip control
    {
        QString str;
        QString name = QString(transformerMac(&mPacket->box[row]));
        if(gLanguage == 0) str= tr("是否控制%1RCA合闸?").arg(name);
        else str= tr("Do you want to trun on %1 shunt trip?").arg(name);
        QuMsgBox box(NULL, str);//传入NULL才可以禁止操作其他界面，如果传入this还可以操作其他界面
        bool ret = box.Exec();
        if(ret){
            item.insertlog = 2;
            item.txtype = 0;
            item.type = 21;
            item.min = 8;
            item.crmin = mPacket->box[row].boxId[0];
            item.crmax = mPacket->box[row].boxId[1];
            item.max = mPacket->box[row].boxId[2];
            SetThread::bulid()->append(item);//new 605
        }
    }else if(col == 10)//turn off trip control
    {
        QString str;
        QString name = QString(transformerMac(&mPacket->box[row]));
        if(gLanguage == 0) str= tr("是否控制%1RCA分闸?").arg(name);
        else str= tr("Do you want to trun off %1 shunt trip?").arg(name);
        QuMsgBox box(NULL, str);
        bool ret = box.Exec();
        if(ret){
            item.insertlog = 2;
            item.txtype = 0;
            item.type = 21;
            item.min = 7;
            item.crmin = mPacket->box[row].boxId[0];
            item.crmax = mPacket->box[row].boxId[1];
            item.max = mPacket->box[row].boxId[2];
            SetThread::bulid()->append(item);//new 605
        }
    }
}

void SetTripWid::itemClicked(QTableWidgetItem *it)
{
    if(it->text().compare("---") == 0) return;  //为空不设置
    int column = it->column();
    if(column == 2||column == 3||column == 4)
    {
        sThresholdItem item;
        item.bus = mBus;
        item.box = it->row();
        item.num = column-2;
        item.type = 20;

        SetThresholdDlg dlg(this);
        dlg.move(0,0);
        dlg.set(item);
        dlg.exec();
    }
}

//void SetTripWid::recvSetThreadTripSlot(QString mac, int val)
//{
//    QString str;
//    if(gLanguage == 0) str= tr("MAC：%1\n%2失败!").arg(mac).arg(val==7?"RCA分闸":"RCA合闸");
//    else str= tr("MAC：%1\n%2 failure?").arg(mac).arg(val==7?"trun off RCA":"trun on RCA");
//    QuMsgBox box(NULL, str);
//    bool ret = box.Exec();
//}
