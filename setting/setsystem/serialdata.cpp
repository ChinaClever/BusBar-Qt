#include "serialdata.h"
#include "ui_serialdata.h"
#include "datapacket.h"
#include "sysconfigfile.h"

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
    QPixmap pix(1,50);
    pix.fill(Qt::transparent);
    QIcon icon(pix);

    if(gLanguage == 0){
        ui->baudLab->setText("波特率：");
        ui->databitsLab->setText("数据位：");
        ui->paritybitLab->setText("校验位：");
        ui->stopbitsLab->setText("停止位：");
        ui->paritybitBox->setItemText( 0 , "无");
        ui->paritybitBox->setItemText( 1 , "偶");
        ui->paritybitBox->setItemText( 2 , "奇");
        ui->saveBtn->setText("保存");
    }else{
        ui->baudLab->setText("Baud rate:");
        ui->databitsLab->setText("Data bits:");
        ui->paritybitLab->setText("Parity bit:");
        ui->stopbitsLab->setText("Stop bits:");
        ui->paritybitBox->setItemText( 0 , "None");
        ui->paritybitBox->setItemText( 1 , "Even");
        ui->paritybitBox->setItemText( 2 , "Odd");
        ui->saveBtn->setText("Save");
    }
    ui->baudBox->setIconSize(QSize(1,50));
    ui->databitsBox->setIconSize(QSize(1,50));
    ui->paritybitBox->setIconSize(QSize(1,50));
    ui->stopbitsBox->setIconSize(QSize(1,50));
    for(int i = 0 ; i < 5 ; i++)
        ui->baudBox->setItemIcon(i , icon);
    for(int i = 0 ; i < 4 ; i++)
        ui->databitsBox->setItemIcon(i , icon);
    for(int i = 0 ; i < 3 ; i++)
        ui->paritybitBox->setItemIcon(i , icon);
    for(int i = 0 ; i < 2 ; i++)
        ui->stopbitsBox->setItemIcon(i , icon);
    initComParameter();
}

int Serialdata::baudToIndex(int baud)
{
    int index = 0;
    switch(baud)
    {
        case 9600:index = 0;break;
        case 19200:index = 1;break;
        case 38400:index = 2;break;
        case 57600:index = 3;break;
        case 115200:index = 4;break;
        default:index = 0;break;
    }
    return index;
}

int Serialdata::parityToIndex(int parity)
{
    int index = 0;
    switch(parity)
    {
    case 0:index = 0;break;
    case 2:index = 1;break;
    case 3:index = 2;break;
    default:index = 0;break;
    }
    return index;
}

int Serialdata::databitsToIndex(int databits)
{
    int index = 3;
    switch(databits)
    {
    case 5:index = 0;break;
    case 6:index = 1;break;
    case 7:index = 2;break;
    case 8:index = 3;break;
    default:index = 3;break;
    }
    return index;
}

int Serialdata::stopbitsToIndex(int stopbits)
{
    int index = 0;
    switch(stopbits)
    {
    case 1:index = 0;break;
    case 2:index = 1;break;
    default:index = 0;break;
    }
    return index;
}


void Serialdata::initComParameter()
{
    bool ret = sys_configFile_open();
    ret = sys_configFile_contains("baudrate");

    if(ret){
        int baud = sys_configFile_readInt("baudrate");
        ui->baudBox->setCurrentIndex(baudToIndex(baud));
    }else{
        ui->baudBox->setCurrentIndex(0);
    }
    ret = sys_configFile_contains("parity");
    if(ret){
        int parity = sys_configFile_readInt("parity");
        ui->paritybitBox->setCurrentIndex(parityToIndex(parity));
    }else{
        ui->paritybitBox->setCurrentIndex(0);
    }
    ret = sys_configFile_contains("databits");
    if(ret){
        int databits = sys_configFile_readInt("databits");
        ui->databitsBox->setCurrentIndex(databitsToIndex(databits));
    }else{
        ui->databitsBox->setCurrentIndex(3);
    }
    ret = sys_configFile_contains("stopbits");
    if(ret){
        int stopbits = sys_configFile_readInt("stopbits");
        ui->stopbitsBox->setCurrentIndex(stopbitsToIndex(stopbits));
    }else{
        ui->stopbitsBox->setCurrentIndex(0);
    }
    sys_configFile_close();
}

QString Serialdata::transformerBaud()
{
    int index = ui->baudBox->currentIndex();
    QString str = "9600";
    switch(index)
    {
        case 0 :str = "9600";break;
        case 1 :str = "19200";break;
        case 2 :str = "38400";break;
        case 3 :str = "57600";break;
        case 4 :str = "115200";break;
        default: str = "9600";break;
    }
    return str;
}

QString Serialdata::transformerParity()
{
    int index = ui->paritybitBox->currentIndex();
    QString str = "0";
    switch(index)
    {
    case 0 :str = "0";break;
    case 1 :str = "2";break;
    case 2 :str = "3";break;
    default: str = "0";break;
    }
    return str;
}

QString Serialdata::transformerDatabits()
{
    int index = ui->databitsBox->currentIndex();
    QString str = "8";
    switch(index)
    {
    case 0 :str = "5";break;
    case 1 :str = "6";break;
    case 2 :str = "7";break;
    case 3 :str = "8";break;
    default: str = "8";break;
    }
    return str;
}

QString Serialdata::transformerStopbits()
{
    int index = ui->stopbitsBox->currentIndex();
    QString str = "1";
    switch(index)
    {
    case 0 :str = "1";break;
    case 1 :str = "2";break;
    default: str = "1";break;
    }
    return str;
}


void Serialdata::on_saveBtn_clicked()
{
    bool ret = sys_configFile_open();
    if(ret){
        sys_configFile_write("baudrate" , transformerBaud());
    }
    if(ret){
        sys_configFile_write("parity" , transformerParity());
    }
    if(ret){
        sys_configFile_write("databits" , transformerDatabits());
    }
    if(ret){
        sys_configFile_write("stopbits" , transformerStopbits());
    }
    sys_configFile_close();
}

