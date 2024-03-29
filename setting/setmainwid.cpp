#include "setmainwid.h"
#include "ui_setmainwid.h"
#include "setthreshold/setthread.h"

SetMainWid::SetMainWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetMainWid)
{
    ui->setupUi(this);
    this->setStyleSheet(BTN_FOCUS_BACK_COLOR);
    QTimer::singleShot(10,this,SLOT(initFunSLot())); //延时初始化
    SetThread::bulid(this)->start();
}

SetMainWid::~SetMainWid()
{
    delete ui;
}


void SetMainWid::initWidget()
{
    mSetLineWid = new SetLineWid(ui->stackedWidget); //主路
    ui->stackedWidget->addWidget(mSetLineWid);

    mSetLoopWid = new SetLoopWid(ui->stackedWidget); //支路电流
    ui->stackedWidget->addWidget(mSetLoopWid);

    mSetPowerMainWid = new SetPowMain(ui->stackedWidget); //支路功率
    ui->stackedWidget->addWidget(mSetPowerMainWid);

    mSetTemWid = new SetTemMain(ui->stackedWidget); //温度
    ui->stackedWidget->addWidget(mSetTemWid);

    mSystemDlg = new SystemSettingDlg(ui->stackedWidget); //系统
    ui->stackedWidget->addWidget(mSystemDlg);
#if ( SI_RTUWIFI == 1 )
    connect(this, SIGNAL(busChangedSig(int)), mSystemDlg, SIGNAL(busChangedSig(int)));
#endif

    mSetNamesWid = new SetNamesWid(ui->stackedWidget); //名称
    ui->stackedWidget->addWidget(mSetNamesWid);

    setButtonColor(ui->lineBtn);
    //    ui->stackedWidget->setCurrentWidget(mSetLineWid);
}


void SetMainWid::setButtonColor(QPushButton *button)
{
    ui->nameBtn->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";");
    ui->lineBtn->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";");
    ui->loopBtn->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";");
    ui->temBtn->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";");
    ui->sysBtn->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";");
    ui->powerBtn->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";");

    button->setStyleSheet("border:2px solid rgb(15,80,232);border-radius:15px;font: 8pt \"Ubuntu\";background-color: rgb(96,238,250);");
    BeepThread::bulid()->beep();
}

void SetMainWid::busChangedSlot(int index)
{
    mSetLineWid->updateWid(index);
    mSetLoopWid->setBus(index);
    mSetPowerMainWid->updateWid(index);
    mSetTemWid->updateWid(index);
    mSetNamesWid->indexChanged(index);
#if (SI_RTUWIFI == 1 )
    emit busChangedSig(index);
#endif
}

void SetMainWid::initFunSLot()
{
    initWidget();
}

void SetMainWid::on_nameBtn_clicked()
{
    setButtonColor(ui->nameBtn);
    ui->stackedWidget->setCurrentWidget(mSetNamesWid);
}

void SetMainWid::on_lineBtn_clicked()
{
    setButtonColor(ui->lineBtn);
    ui->stackedWidget->setCurrentWidget(mSetLineWid);
}

void SetMainWid::on_loopBtn_clicked()
{
    setButtonColor(ui->loopBtn);
    ui->stackedWidget->setCurrentWidget(mSetLoopWid);
}

void SetMainWid::on_temBtn_clicked()
{
    setButtonColor(ui->temBtn);
    ui->stackedWidget->setCurrentWidget(mSetTemWid);
}

void SetMainWid::on_sysBtn_clicked()
{
    setButtonColor(ui->sysBtn);
    ui->stackedWidget->setCurrentWidget(mSystemDlg);
}

void SetMainWid::on_powerBtn_clicked()
{
    setButtonColor(ui->powerBtn);
    ui->stackedWidget->setCurrentWidget(mSetPowerMainWid);
}
