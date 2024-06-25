/*
 * logoperationexportdlg.cpp
 *
 *  Created on: 2017年10月11日
 *      Author: Lzy
 */
#include "logoperationexportdlg.h"
#include "ui_logoperationexportdlg.h"

LogOperationExportDlg::LogOperationExportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogOperationExportDlg)
{
    ui->setupUi(this);
    initLanguage();

    mDbThread = new LogOperationExportThread(this);
    connect(mDbThread,SIGNAL(readDbSig(QString)), this,SLOT(readDbSlot(QString)));
    connect(mDbThread,SIGNAL(exportSig(bool)), this,SLOT(exportSlot(bool)));

    mProgressWid = new ExcelExportWid(ui->widget);
    connect(mProgressWid,SIGNAL(overSig()), this,SLOT(overSlot()));
}

LogOperationExportDlg::~LogOperationExportDlg()
{
    delete ui;
}
void LogOperationExportDlg::initLanguage()
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

void LogOperationExportDlg::set(int id)
{
    ui->pushButton->setEnabled(false);
    mDbThread->initData(id);
}

void LogOperationExportDlg::readDbSlot(QString str)
{
    ui->statusLab->setText(str);
}

void LogOperationExportDlg::exportSlot(bool e)
{
    if(e)
        mProgressWid->set(gExcelExportStr->path, gExcelExportStr->file, mDbThread->mList , OperationLog);
    else
        overSlot();
}

void LogOperationExportDlg::overSlot()
{
    ui->pushButton->setEnabled(true);
}

void LogOperationExportDlg::on_pushButton_clicked()
{
    this->close();
}
