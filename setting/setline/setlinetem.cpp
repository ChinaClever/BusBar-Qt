#include "setlinetem.h"
#include "ui_setlinetem.h"

SetLineTem::SetLineTem(QWidget *parent, bool flag) :
    QWidget(parent),
    ui(new Ui::SetLineTem)
{
    ui->setupUi(this);
    mFlag = flag;
    indexChanged(0);
    initLanguae();
//    if(mFlag){
//        ui->label->setText(tr("A相温度"));
//        ui->label_2->setText(tr("B相温度"));
//        ui->label_3->setText(tr("C相温度"));
//        ui->label_5->setText(tr("零线温度"));
//        showWid();
//    }else{
//        ui->label->setText(tr("A相功率"));
//        ui->label_2->setText(tr("B相功率"));
//        ui->label_3->setText(tr("C相功率"));
//        hideWid();
//    }
    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

SetLineTem::~SetLineTem()
{
    delete ui;
}
void SetLineTem::initLanguae()
{
    if(gLanguage == 0){
        ui->label_4->setText("始端箱:");
        ui->label_5->setText("零线温度:");
//        ui->label_6->setText(tr("环境参数:"));
        ui->label_7->setText(tr("温度:"));
        ui->label_8->setText("湿度:");
        if(mFlag){
            ui->label->setText(tr("A相温度:"));
            ui->label_2->setText(tr("B相温度:"));
            ui->label_3->setText(tr("C相温度:"));
            showWid();
        }else{
            ui->label->setText(tr("A相功率:"));
            ui->label_2->setText(tr("B相功率:"));
            ui->label_3->setText(tr("C相功率:"));
            hideWid();
        }
    }else{
        ui->label_4->setText("Feeder\nbox:");
        ui->label_5->setText("Neutral line\ntemperature:");
//        ui->label_6->setText(tr("Environmental\nparameters:"));
        ui->label_7->setText(tr("Temperature:"));
        ui->label_8->setText("Humidity:");
        if(mFlag){
            ui->label->setText(tr("Phase A\nTemperature:"));
            ui->label_2->setText(tr("Phase B\nTemperature:"));
            ui->label_3->setText(tr("phase C\nTemperature:"));
            showWid();
        }else{
            ui->label->setText(tr("Phase A\nPower:"));
            ui->label_2->setText(tr("Phase B\nPower:"));
            ui->label_3->setText(tr("Phase C\nPower:"));
            hideWid();
        }
    }
}

void SetLineTem::showWid()
{
    ui->label_5->show();
//    ui->label_6->show();
    ui->label_7->show();
    ui->temlab->show();
    ui->humlab->show();
    ui->label_8->show();
}

void SetLineTem::hideWid()
{
    ui->label_5->hide();
//    ui->label_6->hide();
    ui->label_7->hide();
    ui->temlab->hide();
    ui->humlab->hide();
    ui->label_8->hide();
}

void SetLineTem::updateWid()
{
    QPushButton *btn[] ={ui->temBtn_1, ui->temBtn_2, ui->temBtn_3, ui->temBtn_4};

    if(mFlag){
        if(share_mem_get()->data[mBus].box[0].offLine){
            for(int i=0; i<SENSOR_NUM; ++i) {
                QString str = "";
                str = QString::number(startBoxTem->value[i],10)+"℃";
                btn[i]->setText(str);
                sDataUnit *unit = &(get_share_mem()->data[mBus].box[0].env.tem);
                setBtnColor(btn[i] , unit->alarm[i], unit->crAlarm[i]);
            }
        }else{
            for(int i=0; i<SENSOR_NUM; ++i) {
                QString str = "";
                str = "---";
                btn[i]->setText(str);
                setBtnColor(btn[i] , 0, 0);
            }
        }
        sDataUnit *it = &(get_share_mem()->data[0].box[0].env.tem);
        if(it->value[7] !=0 && it->value[8] != 0){
            ui->temlab->setText(QString::number(it->value[7])+"℃");
            ui->humlab->setText(QString::number(it->value[8])+"%");
        }
    }else{
        uchar offline = share_mem_get()->data[mBus].box[0].offLine;
        for(int i=0; i<SENSOR_NUM; ++i) {
            if(i == SENSOR_NUM - 1 ) {btn[i]->hide();break;}
            QString str = "";
            str = offline ?  QString::number(startBoxPow->value[i]/COM_RATE_POW,'f', 3)+"kW" : "---";
            btn[i]->setText(str);
            sDataPowUnit *unit = &(get_share_mem()->data[mBus].box[0].data.pow);
            setBtnColor(btn[i] , offline?unit->alarm[i]:0, offline?unit->crAlarm[i]:0);
        }
    }
}

void SetLineTem::setBtnColor(QPushButton *label, int alarm, int crAlarm)
{
    QPalette pa;
    if(alarm) { // 告警
        pa.setColor(QPalette::ButtonText,Qt::red);
    } else  if(crAlarm) { // 预警
        pa.setColor(QPalette::ButtonText,"#CD7E80");
    } else {
        pa.setColor(QPalette::ButtonText,Qt::black);
    }
    label->setPalette(pa);
}

void SetLineTem::indexChanged(int index)
{
    mBus = index;
    if(mFlag)
        startBoxTem = &(share_mem_get()->data[index].box[0].env.tem); //获取共享内存
    else
        startBoxPow = &(share_mem_get()->data[index].box[0].data.pow); //获取共享内存
}


void SetLineTem::timeoutDone()
{
    updateWid();
}

void SetLineTem::setTem(int id)
{
    sThresholdItem item;
    item.bus = mBus;
    item.box = 0;
    item.num = id;
    if(mFlag)
        item.type = 3;
    else
        item.type = 4;

    SetThresholdDlg dlg(this);
    dlg.move(0,0);
    dlg.set(item);
    dlg.exec();
}

void SetLineTem::on_temBtn_1_clicked()
{
    setTem(0);
}

void SetLineTem::on_temBtn_2_clicked()
{
    setTem(1);
}

void SetLineTem::on_temBtn_3_clicked()
{
    setTem(2);
}

void SetLineTem::on_temBtn_4_clicked()
{
    setTem(3);
}
