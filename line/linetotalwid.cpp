#include "linetotalwid.h"
#include "ui_linetotalwid.h"

LineTotalWid::LineTotalWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineTotalWid)
{
    ui->setupUi(this);
    initLangluage();
    QTimer::singleShot(1000,this,SLOT(initFunSLot())); //延时初始化
}

LineTotalWid::~LineTotalWid()
{
    delete ui;
}
void LineTotalWid::initLangluage()
{
    if(gLanguage == 0){
        ui->label_4->setText("总电流:");
        ui->label_8->setText("总电能:");
        ui->label_7->setText("功率参数:");
        ui->label_9->setText("视在功率:");
    }else{
        ui->label_4->setText("Total current:");
        ui->label_8->setText("Total electric energy:");
        ui->label_7->setText("Power parameter:");
        ui->label_9->setText("Apparent power:");
    }

}
void LineTotalWid::initFunSLot()
{
    busChangeSlot(0);
    timer = new QTimer(this);
    timer->start(2*1000+ rand()%100);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}

void LineTotalWid::busChangeSlot(int id)
{
    sDataPacket *shm = get_share_mem();
    mTgBus = &(shm->data[id].box[0].tgBox);
    updateData();
}

void LineTotalWid::timeoutDone()
{
    updateData();
}

void LineTotalWid::updateData()
{
    QString str =  QString::number(mTgBus->cur) + "A";
    ui->tCurLab->setText(str);

    str =  QString::number(mTgBus->pow) + "kW";
    ui->tPowLab->setText(str);

    str =  QString::number(mTgBus->apPow) + "kVA";
    ui->tApPowLab->setText(str);

    str =  QString::number(mTgBus->ele) + "kWh";
    ui->tEleLab->setText(str);
}
