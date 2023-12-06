#include "setlineitem.h"
#include "ui_setlineitem.h"

SetLineItem::SetLineItem(QWidget *parent, bool flag) :
    QWidget(parent),
    ui(new Ui::SetLineItem)
{
    ui->setupUi(this);
    mBus = mLine = 0;
    mFlag = flag;
    mPacket = get_share_mem();

    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
    connect(ui->curBar,SIGNAL(clicked()),this,SLOT(curBarClicked()));
    connect(ui->volBar,SIGNAL(clicked()),this,SLOT(volBarClicked()));
    if(!mFlag)
    {
        ui->label_1->hide();
        ui->label_2->hide();
        ui->label_14->setText(tr("零线电流"));
//        ui->volBar->hide();
//        ui->volLab->hide();
        ui->label_11->setText(tr("频率"));
        ui->nameLab->setText(tr("其他"));
    }
}

SetLineItem::~SetLineItem()
{
    delete ui;
}

void SetLineItem::timeoutDone()
{
    updateWidget(mBus, mLine);
}


/**
 * @brief 主路源变化，刷新界面
 * @param index
 */
void SetLineItem::updateWidget(int bus, int line)
{
    QString str = "L ";
    sBusData *busData = &(mPacket->data[bus]);
    int dc = busData->box[0].dc;
    if(!dc){
        int len = busData->box[0].rate.svalue ? busData->box[0].rate.svalue : 1;
        if(line>len) this->hide();
        str = "D ";
    }else{  //交流
        this->show();
    }

    sObjData  *objData = &(busData->box[0].data);
//    ui->curLab->setText(QString::number(objData ->cur.value[line]/COM_RATE_CUR,'f', 1)+"A");
//    ui->volLab->setText(QString::number(objData ->vol.value[line]/COM_RATE_VOL,'f', 0)+"V");
    if(mFlag){
        ui->curLab->setText(QString::number(objData ->cur.value[line]/COM_RATE_CUR,'f', 2)+"A");
        ui->volLab->setText(QString::number(objData->vol.value[line]/COM_RATE_VOL,'f', 1)+"V");
    }
    else{
        ui->curLab->setText(QString::number(busData->box[0].rate.svalue/COM_RATE_FREQUENCY,'f',1)+"Hz");
        ui->volLab->setText(QString::number(busData->box[0].zeroLineCur.svalue/COM_RATE_VOL,'f', 2)+"A");
        ui->nameLab->hide();
    }

    ui->nameLab->setText(str+ QString::number(mLine+1));

    if(mFlag){
        setProgressbarValue(ui->curBar,&(objData->cur),line);
        setProgressbarValue(ui->volBar,&(objData->vol),line);
        setLabeColor(ui->curLab , objData->cur.alarm[line], 0);
        setLabeColor(ui->volLab , objData->vol.alarm[line], 0);
    }
    else{
        setProgressbarOtherValue(ui->curBar , &(busData->box[0].rate));
        setProgressbarOtherValue(ui->volBar , &(busData->box[0].zeroLineCur));
        setLabeColor(ui->curLab , busData->box[0].rate.salarm, 0);
        setLabeColor(ui->volLab , busData->box[0].zeroLineCur.salarm, 0);
    }
}

void SetLineItem::setProgressbarOtherValue(QProgressBar *bar, sRtuUshortUnit *data)
{
    int max = data->smax;
    int min = data->smin;
    int value = data->svalue;
    if(max > 0 && min > 0 && max > min && value >= min && value < max)
    {
        int ret = ((value-min)*100.0/(max-min));
        if(ret > 100) ret = 100;
        bar->setValue(ret);
    }else
        bar->setValue(0);

    int alarm = data->salarm;
    if(alarm >= 1)
        setProcessBarColor(bar,"red"); //告警
    else
        setProcessBarColor(bar,"green"); //正常
}

void SetLineItem::setProgressbarValue(QProgressBar *bar, sDataUnit *data, int index)
{
    int max = data->max[index];
    int min = data->min[index];
    if(max - min > 0)
    {
        double value = data->value[index]*1.0;
        int ret = (value - min)*100/(max - min);
        bar->setValue(ret);
    }else
        bar->setValue(0);

    int cirAlarm = data->crAlarm[index];
    int alarm = data->alarm[index];
    if(alarm >= 1)
        setProcessBarColor(bar,"red"); //告警
    else if(cirAlarm == 1)
        setProcessBarColor(bar,"yellow"); //预警
    else
        setProcessBarColor(bar,"green"); //正常

}

void SetLineItem::setProcessBarColor(QProgressBar *bar, QString color)
{

    QString str = "QProgressBar{border:1px solid #000000;height:20;background:transparent;text-align:center;color:rgb(134,68,54);border-radius:10px;}";
    str += QString(" QProgressBar::chunk{border-radius:10px;border:1px solid black;background-color:%1;margin:0.5px;}").arg(color);
    bar->setStyleSheet(str);
}

void SetLineItem::curBarClicked()
{
    BeepThread::bulid()->beep();

    sThresholdItem item;
    item.bus = mBus;
    item.box = 0;
    item.num = mLine;
    if(mFlag)
        item.type = 2;
    else
        item.type = 5;

    SetThresholdDlg dlg(this);
    dlg.move(0,0);
    dlg.set(item);
    dlg.exec();
}

void SetLineItem::volBarClicked()
{
    BeepThread::bulid()->beep();

    sThresholdItem item;
    item.bus = mBus;
    item.box = 0;
    item.num = mLine;
    if(mFlag)
        item.type = 1;
    else
        item.type = 8;

    SetThresholdDlg dlg(this);
    dlg.move(0,0);
    dlg.set(item);
    dlg.exec();
}

void SetLineItem::setLabeColor(QLabel *label, int alarm, int crAlarm)
{
    QPalette pa;
    if(alarm) { // 告警
        pa.setColor(QPalette::WindowText,Qt::red);
    } else  if(crAlarm) { // 预警
        pa.setColor(QPalette::WindowText,"#CD7E80");
    } else {
        pa.setColor(QPalette::WindowText,Qt::black);
    }
    label->setPalette(pa);
}
