#include "temwid.h"
#include "ui_temwid.h"

TemWid::TemWid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TemWid)
{
    ui->setupUi(this);
    com_setBackColour(tr("温度"), this);
}
TemWid::~TemWid()
{
    delete ui;
}


void TemWid::updateWid(int bus, int box)
{
    sDataPacket *shm = get_share_mem();
    mData = &(shm->data[bus].box[box]);

    QString name(mData->boxName);
    ui->titleLab->setText(name);

    QString version = QString("V%1.%2.%3").arg(mData->version/100).arg(mData->version/10%10).arg(mData->version%10);
    if(mData->offLine)
        ui->version->setText(version);

    mTemMain = new TemMain(ui->tabWidget);
    mTemMain->initFun(bus,box);
}

void TemWid::on_pushButton_clicked()
{
    this->close();
    BeepThread::bulid()->beep();
}
