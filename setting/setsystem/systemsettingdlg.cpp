#include "systemsettingdlg.h"
#include "ui_systemsettingdlg.h"
#include <time.h>

SystemSettingDlg::SystemSettingDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemSettingDlg)
{
    ui->setupUi(this);

    initWid();

}

SystemSettingDlg::~SystemSettingDlg()
{
    delete ui;
}

void SystemSettingDlg::initWid()
{
    mLocaNetSettingWid = new LocalNetSettingDlg(this);
//    mNetWid = new NetSettingDlg(this);
    mOtherWid = new OtherSettingDlg(this);
//    mModeWid = new ModeDlg(this);
    mSerialPort = new Serialdata(this);

    mAutoSetAddress = new AutoSetAddress(this);

    QString str;
    if(gLanguage == 0) str = tr("网络设置");
    else str = tr("Network settings");
    ui->tabWidget->addTab(mLocaNetSettingWid,str);
//    str = tr("切换模式");
//    ui->tabWidget->addTab(mModeWid,str);
//    str = tr("网络设置");
//    ui->tabWidget->addTab(mNetWid,str);
    if(gLanguage == 0) str = tr("系统信息设置");
    else str = tr("System information settings");
    ui->tabWidget->addTab(mOtherWid,str);
    if(gLanguage == 0) str = tr("串口信息");
    else str = tr("Serial port information");
    ui->tabWidget->addTab(mSerialPort,str);
    if(gLanguage == 0) str = tr("自动分配地址");
    else str = tr("Automatically assign addresses");
    ui->tabWidget->addTab(mAutoSetAddress,str);
    ui->tabWidget->setStyleSheet("QTabBar::tab{height:50px; width: 200px;}");
    connect(this,SIGNAL(updateBusNameSig(int,QString&)),mAutoSetAddress , SLOT(updateBusNameSlot(int,QString&)));
}



