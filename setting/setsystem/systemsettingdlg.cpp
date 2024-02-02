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
    // 串口信息mSerialPort = new
    QString str = tr("本地IP设置");
    ui->tabWidget->addTab(mLocaNetSettingWid,str);
//    str = tr("切换模式");
//    ui->tabWidget->addTab(mModeWid,str);
//    str = tr("网络设置");
//    ui->tabWidget->addTab(mNetWid,str);
    str = tr("其他参数设置");
    ui->tabWidget->addTab(mOtherWid,str);
    ui->tabWidget->setStyleSheet("QTabBar::tab{height:50px; width: 150px;}");
}



