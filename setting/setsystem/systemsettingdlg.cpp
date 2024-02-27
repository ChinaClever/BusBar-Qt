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

    QString str = tr("本地IP设置");
    ui->tabWidget->addTab(mLocaNetSettingWid,str);
//    str = tr("切换模式");
//    ui->tabWidget->addTab(mModeWid,str);
//    str = tr("网络设置");
//    ui->tabWidget->addTab(mNetWid,str);
    str = tr("其他参数设置");
    ui->tabWidget->addTab(mOtherWid,str);
    str = tr("串口信息");
    ui->tabWidget->addTab(mSerialPort,str);
    str = tr("自动分配地址");
    ui->tabWidget->addTab(mAutoSetAddress,str);
    ui->tabWidget->setStyleSheet("QTabBar::tab{height:50px; width: 150px;}");
}



