#include "boxtotalwid.h"
#include "ui_boxtotalwid.h"

BoxTotalWid::BoxTotalWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoxTotalWid)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->start(2000);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
    //ui->AllWid->hide(); //隐藏和 y_MW_2018.4.23
    initWid();
}

BoxTotalWid::~BoxTotalWid()
{
    delete ui;
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
    QString str = "";
    ui->label_6->setText(str);
    ui->volH->setText(str);
    ui->curH->setText(str);
    ui->tEleH->setText(str);

    str = tr("总有功功率");
    ui->tApPowH->setText(str);

    str = tr("零线温度");
    ui->tPfH->setText(str);
    str = "---";
    ui->tPowH->setText(str);
    ui->temH->setText(str);
    if( mBox && mBox->offLine ){
        if(mBox->data.totalPow == 0 )
            str = QString::number(0, 'f', 2)+"kW";
        else
            str = QString::number(mBox->data.totalPow/COM_RATE_POW, 'f', 3)+"kW";
        ui->tPowH->setText(str);

        if(mEnvData){
            sDataUnit *unit = &(mEnvData->tem);
            if(unit){
                str =  QString::number(unit->value[3]/COM_RATE_TEM) + "°C";
                ui->temH->setText(str);
                setLabeColor(ui->temH, unit->alarm[3], unit->crAlarm[3]);
            }
        }
    }
}

void BoxTotalWid::initWid()
{
    QString str = "";
    ui->label_6->setText(str);
    ui->volH->setText(str);
    ui->curH->setText(str);
    ui->tEleH->setText(str);

    str = tr("总有功功率");
    ui->tApPowH->setText(str);

    str = tr("零线温度");
    ui->tPfH->setText(str);
    str = "---";
    ui->tPowH->setText(str);
    ui->temH->setText(str);
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
            str= QString::number(mLineTgBox->cur[i]/COM_RATE_CUR, 'f', 2) + "A";
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
            str =  QString::number(mLineTgBox->apPow[i]/(COM_RATE_POW*100), 'f', 3) + "kVA";
            tApPowLab[i]->setText(str);

            str =  QString::number(mLineTgBox->reactivePower[i]/(COM_RATE_POW), 'f', 3) + "kVar";
            rePowLab[i]->setText(str);

            if(mLineTgBox->apPow[i] == 0 )
                str = QString::number(0, 'f', 2);
            else
            {
                double pf = (mLineTgBox->pow[i]*100.0)/(mLineTgBox->apPow[i]);
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
