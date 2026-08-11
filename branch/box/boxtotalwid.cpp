#include "boxtotalwid.h"
#include "ui_boxtotalwid.h"

BoxTotalWid::BoxTotalWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoxTotalWid)
{
    ui->setupUi(this);
    initlanguage();
    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
    //ui->AllWid->hide(); //隐藏和 y_MW_2018.4.23
    initWid();
}

BoxTotalWid::~BoxTotalWid()
{
    delete ui;
}

void BoxTotalWid::initlanguage()
{
    if(gLanguage == 0){
        ui->label->setText("输入");
        ui->label_11->setText("电压(V)");
        ui->label_4->setText("电流(A)");
        ui->label_13->setText("负载率(%)");
        ui->label_14->setText("电流\n谐波含量");
        ui->label_9->setText("视在功率");
        ui->label_10->setText("有功功率");
        ui->label_7->setText("功率因数");
        ui->label_12->setText("温度");
        ui->label_8->setText("电能(kWh)");
        ui->label_15->setText("无功功率");
        ui->volH->setText("总视在功率");
        ui->tApPowH->setText("总有功功率");
        ui->tPfH->setText("零线温度");
        ui->label_6->setText("总电能");
    }else{
        ui->label->setText("Input");
        ui->label_11->setText("Voltage(V)");
        ui->label_4->setText("Current(A)");
        ui->label_13->setText("Load rate(%)");
        ui->label_14->setText("Current\nharmonic content");
        ui->label_9->setText("Apparent power");
        ui->label_10->setText("Active power");
        ui->label_7->setText("Power factor");
        ui->label_12->setText("Temperature");
        ui->label_8->setText("Electric energy(kWh)");
        ui->label_15->setText("Reactive power");
        ui->volH->setText("Total\napparent power");
        ui->tApPowH->setText("Total\nactive power");
        ui->tPfH->setText("Neutral line\ntemperature");
        ui->label_6->setText("Total\nelectric energy");
    }
}
void BoxTotalWid::initFun(int bus, int box)
{
    sDataPacket *shm = get_share_mem();
    mBox = &(shm->data[bus].box[box]);
    mTgBox = &(shm->data[bus].box[box].tgBox);
    mLineTgBox  = &(shm->data[bus].box[box].lineTgBox);
    mEnvData = &(shm->data[bus].box[box].env);
    updateData();
}

void BoxTotalWid::timeoutDone()
{
    updateData();
    updateAll();
}

void BoxTotalWid::updateAll()
{
    QString str = "---";
//    ui->label_6->setText(str);
//    ui->volH->setText(str);
//    ui->curH->setText(str);
//    ui->tEleH->setText(str);

//    if(gLanguage == 0) str = tr("总有功功率");
//    else str = tr("Total\nactive power");
//    ui->tApPowH->setText(str);

//    if(gLanguage == 0) str = tr("零线温度");
//    else str = tr("Neutral line\ntemperature");
//    ui->tPfH->setText(str);
//    str = "---";
    ui->curH->setText(str);
    ui->tPowH->setText(str);
    ui->temH->setText(str);
    ui->tEleH->setText(str);
    if( mBox && mBox->offLine ){
        str = QString::number(mBox->totalApPow/COM_RATE_POW, 'f', 3)+"kVA";
        ui->curH->setText(str);//总视在功率
//        mBox->data.totalPow.value[0] = 0;
//        for(int i = 0 ; i < 3 ; ++i) mBox->data.totalPow.value[0] += mLineTgBox->pow[i];
//        str = QString::number(mBox->data.totalPow.value[0]/COM_RATE_POW, 'f', 3)+"kW";
        str = QString::number(mBox->totalPow.ivalue/COM_RATE_POW, 'f', 3)+"kW";
        ui->tPowH->setText(str);//总有功功率
        setLabeColor(ui->tPowH, mBox->totalPow.ialarm , mBox->totalPow.icrAlarm);

        if(mEnvData){
            sDataUnit *unit = &(mEnvData->tem);
            if(unit){
                str =  QString::number(unit->value[3]/COM_RATE_TEM) + "°C";
                ui->temH->setText(str);//零线温度
                setLabeColor(ui->temH, unit->alarm[3], unit->crAlarm[3]);
            }
        }
        str = QString::number(mBox->totalEle/COM_RATE_ELE, 'f', 1)+"kWh";
        ui->tEleH->setText(str);//总电能
    }
}

void BoxTotalWid::initWid()
{
    QString str = "---";
//    QString str = "";
//    ui->label_6->setText(str);
//    ui->volH->setText(str);
//    ui->curH->setText(str);
//    ui->tEleH->setText(str);

//    if(gLanguage == 0) str = tr("总有功功率");
//    else str = tr("Total\nactive power");
//    ui->tApPowH->setText(str);

//    if(gLanguage == 0) str = tr("零线温度");
//    else str = tr("Neutral line\ntemperature");
//    ui->tPfH->setText(str);
//    str = "---";
//    ui->tPowH->setText(str);
//    ui->temH->setText(str);
    ui->curH->setText(str);
    ui->tPowH->setText(str);
    ui->temH->setText(str);
    ui->tEleH->setText(str);
}


void BoxTotalWid::updateData()
{
    if(mBox->dc){ //交流
        ui->CWid->show();
    }else{
        ui->CWid->hide();
    }

    QLabel *curLab[] = { ui->curA,  ui->curB,  ui->curC};
    QLabel *volLab[] = { ui->volA,  ui->volB,  ui->volC};
    QLabel *powLab[] = { ui->tPowA,  ui->tPowB,  ui->tPowC};
    QLabel *tApPowLab[] = { ui->tApPowA,  ui->tApPowB,  ui->tApPowC};
    QLabel *pfLab[] = { ui->tPfA,  ui->tPfB,  ui->tPfC};
    QLabel *eleLab[] = { ui->tEleA,  ui->tEleB,  ui->tEleC};
    QLabel *temLab[] = {ui->temA, ui->temB, ui->temC};
    QLabel *plLab[] = { ui->plA,  ui->plB,  ui->plC};
    QLabel *thdLab[] = {ui->thdA, ui->thdB, ui->thdC};
    QLabel *rePowLab[] = {ui->tRePowA, ui->tRePowB, ui->tRePowC};

    for(int i=0; i<3; ++i) {
        QString str = "---";
        if(mBox->offLine){
//            str = QString::number(mLineTgBox->vol[i]) + "V";
            str = QString::number(mLineTgBox->vol[i]/COM_RATE_VOL, 'f',1) + "V";
            volLab[i]->setText(str);

//            str= QString::number(mLineTgBox->cur[i]/COM_RATE_CUR, 'f', 1) + "A";
            str= QString::number(mLineTgBox->cur[i]/COM_RATE_CUR, 'f', 3) + "A";
            curLab[i]->setText(str);

//            str= QString::number(mBox->data.pl[i]/COM_RATE_VOL, 'f', 1) ;
            str= QString::number(mBox->data.pl[i]/1.0, 'f', 1) ;
            plLab[i]->setText(str);

            str= QString::number(mBox->data.curThd[i]/100.0, 'f', 2) + "%";
            thdLab[i]->setText(str);

//            str =  QString::number(mLineTgBox->pow[i]/COM_RATE_POW, 'f', 3) + "kW";
            str =  QString::number(mLineTgBox->pow[i]/COM_RATE_POW, 'f', 3) + "kW";
            powLab[i]->setText(str);

//            str =  QString::number(mLineTgBox->apPow[i]/COM_RATE_POW, 'f', 3) + "kVA";
            str =  QString::number(mLineTgBox->apPow[i]/(COM_RATE_POW*1000), 'f', 3) + "kVA";
            tApPowLab[i]->setText(str);

            str =  QString::number(mLineTgBox->reactivePower[i]/(COM_RATE_POW), 'f', 3) + "kvar";
            rePowLab[i]->setText(str);

            if(mLineTgBox->apPow[i] == 0 )
                str = QString::number(0, 'f', 2);
            else
            {
                double pf = (mLineTgBox->pow[i]*1000.0)/(mLineTgBox->apPow[i]);
                if( pf > 0.99 ) str = "0.99";
                else str =  QString::number(pf, 'f', 2);
            }
            pfLab[i]->setText(str);

            str =  QString::number(mLineTgBox->ele[i]/COM_RATE_ELE, 'f', 1);// + "kWh";
            eleLab[i]->setText(str);

            sDataUnit *unit = &(mEnvData->tem);
            str =  QString::number(unit->value[i]) + "°C";
            temLab[i]->setText(str);
            setLabeColor(temLab[i], unit->alarm[i], unit->crAlarm[i]);

        }else{
            volLab[i]->setText(str);
            curLab[i]->setText(str);
            plLab[i]->setText(str);
            thdLab[i]->setText(str);
            powLab[i]->setText(str);
            tApPowLab[i]->setText(str);
            pfLab[i]->setText(str);
            eleLab[i]->setText(str);
            temLab[i]->setText(str);
            rePowLab[i]->setText(str);
        }
         if(mBox->dc == 0) thdLab[i]->setText("---");
    }
}


void BoxTotalWid::setLabeColor(QLabel *label, int alarm, int crAlarm)
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
