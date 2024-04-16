#include "linewid.h"
#include "ui_linewid.h"
#include "interfacechangesig.h"
#include "thdmaindlg.h"

LineWid::LineWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineWid)
{
    ui->setupUi(this);
    initLanguage();
    mIndex = 0;
    initFun();
    initWid();

    connect(this, SIGNAL(busChangedSig(int)), this, SLOT(indexChanged(int)));
    connect(InterfaceChangeSig::get(), SIGNAL(typeSig(int)), this,SLOT(interfaceChangedSlot(int)));
}

LineWid::~LineWid()
{
    delete ui;
}

void LineWid::initLanguage()
{
    if(gLanguage == 0){
        ui->label_10->setText("零线\n电流:");
        ui->label_11->setText("断路器:");
        ui->label_15->setText("零线\n温度:");
        ui->label_16->setText("总有功\n功率:");

        ui->label_17->setText("防雷:");
        ui->label_2->setText("始端箱\n版本:");
        ui->label_5->setText("输入");
        ui->label_13->setText("电压");
        ui->label_9->setText("电流");
        ui->label_3->setText("过载电流");
        ui->label_8->setText("有功功率");
        ui->label_6->setText("功率因数");
        ui->label_4->setText("无功功率");
        ui->label_7->setText("电能");
        ui->thdBtn->setText("谐波分析");
    }else{
        ui->label_10->setText("Zero line\ncurrent:");
        ui->label_11->setText("Breaker:");
        ui->label_15->setText("Zero line\ntemperature:");
        ui->label_16->setText("Total\nactive power:");

        ui->label_17->setText("Lightning\nprotection:");
        ui->label_2->setText("Start box\nversion:");
        ui->label_5->setText("Input");
        ui->label_13->setText("Voltage");
        ui->label_9->setText("Current");
        ui->label_3->setText("Overload current");
        ui->label_8->setText("Active power");
        ui->label_6->setText("Power factor");
        ui->label_4->setText("Reactive power");
        ui->label_7->setText("Electric energy");
        ui->thdBtn->setText("Harmonic\nanalysis");
    }
}
void LineWid::initFun()
{
    sDataPacket *shm = get_share_mem();
    mData = &(shm->data[mIndex]);

    isRun = false;
    timer = new QTimer(this);
    timer->start(3*1000 + rand()%100);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

void LineWid::initWid()
{
    //    mTotalWid = new LineTotalWid(ui->totalWid);
    //    connect(this, SIGNAL(busChangedSig(int)), mTotalWid, SLOT(busChangeSlot(int)));
    //    initTotalWid(); //表盘界面
    ui->totalWid->hide(); //隐藏和 By_MW_2018.4.23

    mLineTable = new LineTable(ui->widget) ;
    //三总线界面
    mLine[0] = new LineRoadWid(ui->line1Wid);
    mLine[1] = new LineRoadWid(ui->line2Wid);
    mLine[2] = new LineRoadWid(ui->line3Wid);

    for(int i=0; i<3; ++i) {
        LineRoadWid *line  = mLine[i];
        line->initLine(i);
        connect(this, SIGNAL(busChangedSig(int)), line, SLOT(busChangeSlot(int)));
    }

//    ui->lpsLab->hide();
//    ui->label_10->hide();
}


void LineWid::interfaceChangedSlot(int id)
{
    if(id == 2) {
        isRun = true;
    } else {
        isRun = false;
    }
}

void LineWid::timeoutDone()
{
    if(isRun) {
        uchar offline = mData->box[0].offLine;
        QString str;
        if(mData->box[0].dc){ //交流
            str = QString::number(mData->box[0].rate.svalue/10.0,'f',1) + "Hz";
            ui->rateLab->setText(str); //频率
            if(gLanguage == 0)ui->label->setText("频率：");
            else ui->label->setText("Frequency：");

            ui->thdBtn->setHidden(false);
            ui->widget->setHidden(false);
            mLineTable->updateData(mData->box[0]);
            ui->zeroLineLab->setText(QString::number(offline==0?0:
                                             mData->box[0].zeroLineCur.ivalue/COM_RATE_CUR,'f',3) + "A");
            QPalette pa;
            if((mData->box[0].zeroLineAlarm==1 || mData->box[0].zeroLineAlarm==2)&&offline)
            {
                pa.setColor(QPalette::WindowText, Qt::red);
                ui->zeroLineLab->setPalette(pa);
            }
            else {
                pa.setColor(QPalette::WindowText, Qt::black);
                ui->zeroLineLab->setPalette(pa);
            }

            ui->lpStateLab->setText(tr("---"));
            if(mData->box[0].lpsAlarm==2&&offline)
            {
                if(gLanguage == 0)ui->lpStateLab->setText(tr("损坏"));
                else ui->lpStateLab->setText(tr("damage"));
                pa.setColor(QPalette::WindowText, Qt::red);
                ui->lpStateLab->setPalette(pa);
            }else if(mData->box[0].lpsAlarm==1&&offline){
                if(gLanguage == 0)ui->lpStateLab->setText(tr("正常"));
                else ui->lpStateLab->setText(tr("normal"));
                pa.setColor(QPalette::WindowText, Qt::black);
                ui->lpStateLab->setPalette(pa);
            }
            else {
                ui->lpStateLab->setText(tr("---"));
                pa.setColor(QPalette::WindowText, Qt::black);
                ui->lpStateLab->setPalette(pa);
            }
            updateBreak();

            updateTem();
        }else{
            if(gLanguage == 0) {str= QString::number(mData->box[0].rate.svalue) + "路";ui->label->setText("输入：");}
            else {str= QString::number(mData->box[0].rate.svalue) + "load";ui->label->setText("Input：");}
            ui->rateLab->setText(str); //频率


            ui->thdBtn->setHidden(false);
            ui->widget->setHidden(true);
        }


        //------[版本号]------------
        QString version = QString("V%1.%2.%3").arg(offline==0?0:mData->box[0].version/100)
                              .arg(offline==0?0:mData->box[0].version/10%10)
                              .arg(offline==0?0:mData->box[0].version%10);
        ui->version->setText(version);

        //        updateTotalWid();
        //        updatePlot();
    }
}

void LineWid::updateBreak()
{
    QPalette pa;
    ui->breakLab->setText(tr("---"));
    uchar offline = mData->box[0].offLine;
    if(mData->box[0].data.sw[0]&&offline){
        if(mData->box[0].data.sw[0] == 1){
            if(gLanguage == 0)ui->breakLab->setText(tr("闭合"));
            else ui->breakLab->setText(tr("ON"));
            pa.setColor(QPalette::WindowText, Qt::black);
        }else if(mData->box[0].data.sw[0] == 2){
            if(gLanguage == 0)ui->breakLab->setText(tr("断开"));
            else ui->breakLab->setText(tr("OFF"));
            pa.setColor(QPalette::WindowText, Qt::red);
        }else if(mData->box[0].data.sw[0] == 3){
            if(gLanguage == 0)ui->breakLab->setText(tr("跳闸"));
            else ui->breakLab->setText(tr("TRIP"));
            pa.setColor(QPalette::WindowText, Qt::red);
        }
        ui->breakLab->setPalette(pa);
    }else{
        pa.setColor(QPalette::WindowText, Qt::black);
        ui->breakLab->setPalette(pa);
    }
}

void LineWid::initTotalWid()
{    
    QHBoxLayout *layout = new QHBoxLayout(ui->totalWid);

    mCurPlot = new CustomDialPlot(ui->totalWid);
    mVolPlot = new CustomDialPlot(ui->totalWid);
    mPwPlot = new CustomDialPlot(ui->totalWid);
    // mPfPlot = new CustomDialPlot(ui->totalWid);

    layout->addWidget(mVolPlot);
    layout->addWidget(mCurPlot);
    layout->addWidget(mPwPlot);
    // layout->addWidget(mPfPlot);

    mVolPlot->setUnit("V");
    mCurPlot->setUnit("A");
    mPwPlot->setUnit("kW");

    // mPfPlot->setUnit("");
    // mPfPlot->setRange(0,1);
}

void LineWid::updateTotalWid()
{
    int max = 0;
    for(int i=0; i<3; ++i) max += mData->box[0].data.cur.max[i];
    if(max<1000) max = 1000;
    mCurPlot->setRange(0,max/COM_RATE_CUR + 1);

    max *= 275;
    mPwPlot->setRange(0,max/COM_RATE_POW);
    mVolPlot->setRange(0,275);
}

void LineWid::updateTem()
{
    QString str = "---";
    if(mData && mData->box[0].offLine){
        sDataUnit unit = mData->box[0].env.tem;

        ui->temD->setText(QString::number(unit.value[SENSOR_NUM-1]/COM_RATE_TEM) + "°C");
        updateAlarmStatus(ui->temD,unit,SENSOR_NUM-1);
        if(mData->box[0].totalPow.ivalue == 0 )
            str = QString::number(0, 'f', 2)+"kW";
        else
            str = QString::number(mData->box[0].totalPow.ivalue/COM_RATE_POW, 'f', 3)+"kW";
        ui->totalPowLab->setText(str);
        setLabeColor(ui->rateLab , mData->box[0].HzAlarm , 0);
        int flag = 0;
        if(mData->box[0].totalPow.ivalue < mData->box[0].totalPow.imin || mData->box[0].totalPow.ivalue > mData->box[0].totalPow.imax )
        {
            flag = 2;
        }
        setLabeColor(ui->totalPowLab , flag , 0);
    }else{
        ui->temD->setText(str);
        ui->totalPowLab->setText(str);
        ui->rateLab->setText(str);
    }
}

void LineWid::updatePlot()
{    
    sDataPacket *shm = get_share_mem();
    sTgObjData *tgBusData = &(shm->data[mIndex].box[0].tgBox);
    mVolPlot->setValue(tgBusData->vol);        //表盘更新 V
    mCurPlot->setValue(tgBusData->cur/COM_RATE_CUR); //A
    mPwPlot->setValue(tgBusData->pow/COM_RATE_POW);  //W
    //    mPfPlot->setValue(tgBusData->pf/COM_RATE_PF);
}

void LineWid::indexChanged(int index)
{
    mIndex = index;
    //-------[未随着更新]------------ 故ADD—— By_MW
    sDataPacket *shm = get_share_mem();
    mData = &(shm->data[mIndex]);
    //-------------------------------------------
}


void LineWid::on_thdBtn_clicked()
{
    ThdMainDlg dlg(this);
    dlg.initBus(mIndex);
    dlg.exec();
}

void LineWid::updateAlarmStatus(QLabel *lab, sDataUnit &unit , int id)
{
    QPalette pe;

    if(unit.alarm[id])
        pe.setColor(QPalette::WindowText,Qt::red);
    else  if(unit.crAlarm[id])
        pe.setColor(QPalette::WindowText,"#CD7E80");
    else
        pe.setColor(QPalette::WindowText,Qt::black);

    lab->setPalette(pe);
}

void LineWid::setLabeColor(QLabel *label, int alarm, int crAlarm)
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
