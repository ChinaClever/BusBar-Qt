#include "autosetaddress.h"
#include "ui_autosetaddress.h"

AutoSetAddress::AutoSetAddress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AutoSetAddress)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->progressBar_2->setValue(0);
    ui->progressBar_3->setValue(0);
    ui->progressBar_4->setValue(0);
    setProcessBarColor(ui->progressBar,"green");
    setProcessBarColor(ui->progressBar_2,"green");
    setProcessBarColor(ui->progressBar_3,"green");
    setProcessBarColor(ui->progressBar_4,"green");
    for(int i = 0 ; i < 4 ; ++i) mCurBoxNum[i] = 0;
}

AutoSetAddress::~AutoSetAddress()
{
    delete ui;
}

void AutoSetAddress::on_autoSet1Btn_clicked()
{
    ui->autoSet1Btn->setEnabled(false);
    ui->progressBar->setValue(0);
    emit autoSetBus1Sig(1);
}


void AutoSetAddress::on_autoSet2Btn_clicked()
{
    ui->autoSet2Btn->setEnabled(false);
    ui->progressBar_2->setValue(0);
    emit autoSetBus2Sig(2);
}

void AutoSetAddress::on_autoSet3Btn_clicked()
{
    ui->autoSet3Btn->setEnabled(false);
    ui->progressBar_3->setValue(0);
    emit autoSetBus3Sig(3);
}


void AutoSetAddress::on_autoSet4Btn_clicked()
{
    ui->autoSet4Btn->setEnabled(false);
    ui->progressBar_4->setValue(0);
    emit autoSetBus4Sig(4);
}

void AutoSetAddress::setProgressbarPowValue(QProgressBar *bar,int value, int index)
{
    int min = 2;
    int max = getBoxNum(index)==-1?18:getBoxNum(index);
    max += 2;
    if(max - min > 0)
    {
        int ret = (value - min)*100.0/(max - min);
        bar->setValue(ret);
        if( max == value ) {
            switch(index+1)
            {
            case 1:{
                    ui->autoSet1Btn->setEnabled(true);
                    QPalette pa;
                    pa.setColor(QPalette::WindowText, Qt::green);
                    ui->statuslab1->setPalette(pa);
                }break;
            case 2:{
                    ui->autoSet2Btn->setEnabled(true);
                    QPalette pa;
                    pa.setColor(QPalette::WindowText, Qt::green);
                    ui->statuslab2->setPalette(pa);
            }break;
            case 3:{
                    ui->autoSet3Btn->setEnabled(true);
                    QPalette pa;
                    pa.setColor(QPalette::WindowText, Qt::green);
                    ui->statuslab3->setPalette(pa);
            }break;
            case 4:{
                    ui->autoSet4Btn->setEnabled(true);
                    QPalette pa;
                    pa.setColor(QPalette::WindowText, Qt::green);
                    ui->statuslab4->setPalette(pa);
            }break;
            }
        }
        setProcessBarColor(bar , "green");
    }else
        bar->setValue(0);
}

void AutoSetAddress::setProcessBarColor(QProgressBar *bar, QString color)
{
    QString str = "QProgressBar{border:1px solid #000000;height:20;background:transparent;text-align:center;color:rgb(134,68,54);border-radius:10px;}";
    str += QString(" QProgressBar::chunk{border-radius:10px;border:1px solid black;background-color:%1;margin:0.5px;}").arg(color);
    bar->setStyleSheet(str);
}

void AutoSetAddress::getNumAndIndexSlot(int index , int curnum)
{
    int num = curnum >= 2 ? curnum : 2;
    int in = index + 1;
    switch(in)
    {
    case 1:
    {
        setProgressbarPowValue(ui->progressBar , num , index);
        mCurBoxNum[0] = num;
        if(num > 2){
            ui->statuslab1->setText(QString(tr("设置地址%1成功")).arg(num-1));
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->statuslab4->setPalette(pa);
        }
    }break;
    case 2:
    {
        setProgressbarPowValue(ui->progressBar_2 , num , index);
        mCurBoxNum[1] = num;
        if(num > 2){
            ui->statuslab2->setText(QString(tr("设置地址%1成功")).arg(num-1));
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->statuslab4->setPalette(pa);
        }
    }break;
    case 3:
    {
        setProgressbarPowValue(ui->progressBar_3 , num , index);
        mCurBoxNum[2] = num;
        if(num > 2){
            ui->statuslab3->setText(QString(tr("设置地址%1成功")).arg(num-1));
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->statuslab4->setPalette(pa);
        }
    }break;
    case 4:
    {
        setProgressbarPowValue(ui->progressBar_4 , num , index);
        mCurBoxNum[3] = num;
        if(num > 2){
            ui->statuslab4->setText(QString(tr("设置地址%1成功")).arg(num-1));
            QPalette pa;
            pa.setColor(QPalette::WindowText, Qt::black);
            ui->statuslab4->setPalette(pa);
        }
    }break;
    }
}

void AutoSetAddress::getDelaySlot(int index)
{
    int in = index + 1;
    switch(in)
    {
    case 1:
    {
        setProcessBarColor(ui->progressBar , "red");
        if(mCurBoxNum[0] == 0){
            ui->statuslab1->setText(QString(tr("始端箱未发出命令")));
        }else if(mCurBoxNum[0] >= 2 && mCurBoxNum[0] <= 20 ){
            ui->statuslab1->setText(QString(tr("设置地址%1失败")).arg(mCurBoxNum[0]));
        }
        ui->autoSet1Btn->setEnabled(true);
        QPalette pa;
        pa.setColor(QPalette::WindowText, Qt::red);
        ui->statuslab1->setPalette(pa);
        mCurBoxNum[0] = 0;
    }break;
    case 2:
    {
        setProcessBarColor(ui->progressBar_2 , "red");
        if(mCurBoxNum[1] == 0){
            ui->statuslab2->setText(QString(tr("始端箱未发出命令")));
        }else if(mCurBoxNum[1] >= 2 && mCurBoxNum[1] <= 20 ){
            ui->statuslab2->setText(QString(tr("设置地址%1失败")).arg(mCurBoxNum[1]));
        }
        ui->autoSet2Btn->setEnabled(true);
        QPalette pa;
        pa.setColor(QPalette::WindowText, Qt::red);
        ui->statuslab2->setPalette(pa);
        mCurBoxNum[1] = 0;
    }break;
    case 3:
    {
        setProcessBarColor(ui->progressBar_3 , "red");
        if(mCurBoxNum[2] == 0){
            ui->statuslab3->setText(QString(tr("始端箱未发出命令")));
        }else if(mCurBoxNum[2] >= 2 && mCurBoxNum[2] <= 20 ){
            ui->statuslab3->setText(QString(tr("设置地址%1失败")).arg(mCurBoxNum[2]));
        }
        ui->autoSet3Btn->setEnabled(true);
        QPalette pa;
        pa.setColor(QPalette::WindowText, Qt::red);
        ui->statuslab3->setPalette(pa);
        mCurBoxNum[2] = 0;
    }break;
    case 4:
    {
        setProcessBarColor(ui->progressBar_4 , "red");
        if(mCurBoxNum[3] == 0){
            ui->statuslab4->setText(QString(tr("始端箱未发出命令")));
        }else if(mCurBoxNum[3] >= 2 && mCurBoxNum[3] <= 20 ){
            ui->statuslab4->setText(QString(tr("设置地址%1失败")).arg(mCurBoxNum[3]));
        }
        ui->autoSet4Btn->setEnabled(true);
        QPalette pa;
        pa.setColor(QPalette::WindowText, Qt::red);
        ui->statuslab4->setPalette(pa);
        mCurBoxNum[3] = 0;
    }break;
    }
}
