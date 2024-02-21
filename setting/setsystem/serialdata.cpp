#include "serialdata.h"
#include "ui_serialdata.h"

Serialdata::Serialdata(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Serialdata)
{
    ui->setupUi(this);
    this->setStyleSheet(BTN_FOCUS_BACK_COLOR);
//    QTimer::singleShot(10,this,SLOT(initFunSLot())); //延时初始化
}

Serialdata::~Serialdata()
{
    delete ui;
}
void Serialdata::initWid()
{
    sDataPacket *shm = get_share_mem();
    mData = &(shm->data[0].box[0]);//始端箱
    ui->baudEdit->setText(QString::number(mData->baudRate));
}
void Serialdata::initFunSLot()
{
    initWid();
}


