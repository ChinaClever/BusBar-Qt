#include "serialdata.h"
#include "ui_serialdata.h"
#include "datapacket.h"

Serialdata::Serialdata(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Serialdata)
{
    ui->setupUi(this);
    initLanguage();
}

Serialdata::~Serialdata()
{
    delete ui;
}
void Serialdata::initLanguage()
{
    if(gLanguage == 0){
        ui->baudLab->setText("波特率：");
        ui->databitsLab->setText("数据位：");
        ui->checkbitsLab->setText("校验位：");
        ui->stopbitsLab->setText("停止位：");
        ui->checkbitsLab_2->setText("无");
    }else{
        ui->baudLab->setText("Baud rate:");
        ui->databitsLab->setText("Data bits:");
        ui->checkbitsLab->setText("Parity bit:");
        ui->stopbitsLab->setText("Stop bit");
        ui->checkbitsLab_2->setText("NONE");
    }
}

