#include "boxoutputwid.h"
#include "ui_boxoutputwid.h"

BoxOutputWid::BoxOutputWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BoxOutputWid)
{
    ui->setupUi(this);
    initlanguage();
    timer = new QTimer(this);
    timer->start(2000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

BoxOutputWid::~BoxOutputWid()
{
    delete ui;
}

void BoxOutputWid::initlanguage()
{
    if(gLanguage == 0){
        ui->label->setText("输出");
        ui->label_9->setText("视在功率");
        ui->label_10->setText("有功功率");
        ui->label_8->setText("电能(kWh)");
    }else{
        ui->label->setText("Output");
        ui->label_9->setText("Apparent power");
        ui->label_10->setText("Active power");
        ui->label_8->setText("Electric energy(kWh)");
    }
}
void BoxOutputWid::initFun(int bus, int box)
{
    sDataPacket *shm = get_share_mem();
    mBox = &(shm->data[bus].box[box]);
    updateData();
}

void BoxOutputWid::timeoutDone()
{
    updateData();
}

void BoxOutputWid::updateData()
{
    QLabel *powLab[] = { ui->tPowA,  ui->tPowB,  ui->tPowC};
    QLabel *tApPowLab[] = { ui->tApPowA,  ui->tApPowB,  ui->tApPowC};
    QLabel *eleLab[] = { ui->tEleA,  ui->tEleB,  ui->tEleC};
    uchar breaker_num = (mBox->phaseFlag == 0) ? mBox->data.lineNum: (mBox->plugbreaker>>12)&0x0F;
    for(int i=0; i<3; ++i) {
        QString str = "---";
        if(mBox->offLine && i < breaker_num){
            str =  QString::number(mBox->outputXBox.outputXPow[i].ivalue/COM_RATE_POW, 'f', 3) + "kW";
            powLab[i]->setText(str);
            setLabeColor(powLab[i] , mBox->outputXBox.outputXPow[i].ialarm , mBox->outputXBox.outputXPow[i].icrAlarm);

            str =  QString::number(mBox->outputXBox.outputXApPow[i].ivalue/COM_RATE_POW, 'f', 3) + "kVA";
            tApPowLab[i]->setText(str);

            str =  QString::number(mBox->outputXBox.outputXEle[i]/COM_RATE_ELE, 'f', 1);// + "kWh";
            eleLab[i]->setText(str);

        }else{
            powLab[i]->setText(str);
            setLabeColor(powLab[i] , 0 , 0);
            tApPowLab[i]->setText(str);
            eleLab[i]->setText(str);
        }
    }
}


void BoxOutputWid::setLabeColor(QLabel *label, int alarm, int crAlarm)
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
