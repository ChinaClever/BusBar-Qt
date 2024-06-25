/*
 * logoperationexportdlg.cpp
 *
 *  Created on: 2017年10月11日
 *      Author: Lzy
 */
#include "logsystemexportdlg.h"
#include "ui_logsystemexportdlg.h"

LogSystemExportDlg::LogSystemExportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogSystemExportDlg)
{
    ui->setupUi(this);
    initLanguage();

    mDbThread = new LogSystemExportThread(this);
    connect(mDbThread,SIGNAL(readDbSig(QString)), this,SLOT(readDbSlot(QString)));
    connect(mDbThread,SIGNAL(exportSig(bool)), this,SLOT(exportSlot(bool)));

    mProgressWid = new ExcelExportWid(ui->widget);
    connect(mProgressWid,SIGNAL(overSig()), this,SLOT(overSlot()));
}

LogSystemExportDlg::~LogSystemExportDlg()
{
    delete ui;
}
void LogSystemExportDlg::initLanguage()
{
    if(gLanguage == 0){
        ui->label->setText("日志导出");
        ui->label_2->setText("导出状态");
        ui->label_4->setText("导出进度：");
        ui->pushButton->setText("退出");
    }else{
        ui->label->setText("Log export");
        ui->label_2->setText("Export Status:");
        ui->label_4->setText("Export Progress:");
        ui->pushButton->setText("Quit");
    }
}

void LogSystemExportDlg::set()
{
    ui->pushButton->setEnabled(false);
    mDbThread->initData();
}

void LogSystemExportDlg::readDbSlot(QString str)
{
    ui->statusLab->setText(str);
}

void LogSystemExportDlg::exportSlot(bool e)
{
    if(e)
        mProgressWid->set(gExcelExportStr->path, gExcelExportStr->file, mDbThread->mList , SystemLog);
    else
        overSlot();
}

void LogSystemExportDlg::overSlot()
{
    ui->pushButton->setEnabled(true);
}

void LogSystemExportDlg::on_pushButton_clicked()
{
    this->close();
}
