#include "linewid.h"
#include "ui_linewid.h"
#include "interfacechangesig.h"
#include "thdmaindlg.h"

LineWid::LineWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineWid)
{
    ui->setupUi(this);

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
        QString str;
        if(mData->box[0].dc){ //交流
            str = QString::number(mData->box[0].rate.svalue/10.0,'f',1) + "Hz";
            ui->rateLab->setText(str); //频率
            ui->label->setText("频率：");

            ui->thdBtn->setHidden(false);
            ui->widget->setHidden(false);
            mLineTable->updateData(mData->box[0]);
            ui->zeroLineLab->setText(QString::number(mData->box[0].zeroLineCur.svalue/COM_RATE_CUR,'f',2) + "A");
            QPalette pa;
            if(mData->box[0].zeroLineAlarm==1 || mData->box[0].zeroLineAlarm==2)
            {
                pa.setColor(QPalette::WindowText, Qt::red);
                ui->zeroLineLab->setPalette(pa);
            }
            else {
                pa.setColor(QPalette::WindowText, Qt::black);
                ui->zeroLineLab->setPalette(pa);
            }

            ui->lpStateLab->setText(tr("---"));
            if(mData->box[0].lpsAlarm==2&&mData->box[0].offLine)
            {
                ui->lpStateLab->setText(tr("损坏"));
                pa.setColor(QPalette::WindowText, Qt::red);
                ui->lpStateLab->setPalette(pa);
            }else if(mData->box[0].lpsAlarm==1&&mData->box[0].offLine){
                ui->lpStateLab->setText(tr("正常"));
                pa.setColor(QPalette::WindowText, Qt::black);
                ui->lpStateLab->setPalette(pa);
            }
            else {
                ui->lpStateLab->setText(tr("---"));
                pa.setColor(QPalette::WindowText, Qt::black);
                ui->lpStateLab->setPalette(pa);
            }
            updateTem();
        }else{
            str= QString::number(mData->box[0].rate.svalue) + "路";
            ui->rateLab->setText(str); //频率
            ui->label->setText("输入：");

            ui->thdBtn->setHidden(false);
            ui->widget->setHidden(true);
        }


        //------[版本号]------------
        QString version = QString("V%1.%2.%3").arg(mData->box[0].version/100).arg(mData->box[0].version/10%10).arg(mData->box[0].version%10);
        ui->version->setText(version);

        //        updateTotalWid();
        //        updatePlot();
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
    QLabel *temLab[] = {ui->temA, ui->temB, ui->temC ,ui->temD};
    QString str = "---";
    if(mData && mData->box[0].offLine){
        sDataUnit unit = mData->box[0].env.tem;

        for(int i = 0 ; i < SENSOR_NUM ; i++){
            temLab[i]->setText(QString::number(unit.value[i]/COM_RATE_TEM) + "°C");
            updateAlarmStatus(temLab[i],unit,i);
        }
        if(mData->box[0].data.totalPow.value == 0 )
            str = QString::number(0, 'f', 2)+"kW";
        else
            str = QString::number(mData->box[0].data.totalPow.value[0]/COM_RATE_POW, 'f', 3)+"kW";
        ui->totalPowLab->setText(str);
        setLabeColor(ui->rateLab , mData->box[0].HzAlarm , 0);
        int flag = 0;
        if(mData->box[0].data.totalPow.value[0] < mData->box[0].data.totalPow.min[0] || mData->box[0].data.totalPow.value[0] > mData->box[0].data.totalPow.max[0] )
        {
            flag = 2;
        }
        setLabeColor(ui->totalPowLab , flag , 0);
    }else{
        for(int i = 0 ; i < SENSOR_NUM ; i++){
            temLab[i]->setText(str);
        }
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
