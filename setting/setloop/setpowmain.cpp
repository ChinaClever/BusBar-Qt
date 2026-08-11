#include "setpowmain.h"
#include "ui_setpowmain.h"

SetPowMain::SetPowMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetPowMain)
{
    ui->setupUi(this);
    mLineTem = new SetLineTem(ui->widget_1 , false);
//    mLoopPowerTem = new SetLoopPowerWid(ui->widget_2);
    mLoopPowerTem = new SetLoopPowerWid(ui->tabWidget);
    mOutputPowerTem = new SetOutputPowerWid(ui->tabWidget);
    if(gLanguage == 0) ui->tabWidget->addTab(mLoopPowerTem, tr("回路功率"));
    else ui->tabWidget->addTab(mLoopPowerTem, tr("Loop power"));
    if(gLanguage == 0) ui->tabWidget->addTab(mOutputPowerTem, tr("输出位功率"));
    else ui->tabWidget->addTab(mOutputPowerTem, tr("Output power"));
    ui->tabWidget->setStyleSheet("QTabBar::tab{height:50px; width: 200px;}");
    ui->tabWidget->setTabShape(QTabWidget::Triangular);
}

SetPowMain::~SetPowMain()
{
    delete ui;
}

void SetPowMain::updateWid(int index)
{
    mLineTem->indexChanged(index);
    mLoopPowerTem->setBus(index);
    mOutputPowerTem->setBus(index);
}
