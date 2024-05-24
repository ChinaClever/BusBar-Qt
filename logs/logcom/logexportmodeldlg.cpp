/*
 * logexportmodeldlg.cpp
 *
 *
 *  Created on: 2017年10月1日
 *      Author: Lzy
 */
#include "logexportmodeldlg.h"
#include "ui_logexportmodeldlg.h"
#include "logcalendardlg.h"
#include "common/common.h"
#include <QFileDialog>

sExcelExportStr *gExcelExportStr = new sExcelExportStr();


LogExportModelDlg::LogExportModelDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogExportModelDlg)
{
    ui->setupUi(this);
    initLanguage();

    ui->startDateEdit->setDate(QDate::currentDate());
    ui->endDateEdit->setDate(QDate::currentDate());

    // 暂时不实现日表、月报表功能
    ui->groupBox_2->setHidden(true);
    this->setMaximumWidth(400);
}

LogExportModelDlg::~LogExportModelDlg()
{
    delete ui;
}
void LogExportModelDlg::initLanguage()
{
    if(gLanguage == 0){
        ui->groupBox->setTitle("报表生成");
        ui->groupBox_2->setTitle("记录导出");
        ui->dayBtn->setText("日报表");
        ui->monthBtn->setText("月报表");
        ui->yearBtn->setText("年报表");
        ui->label->setText("开始时间");
        ui->label_2->setText("结束时间");
        ui->label_3->setText("文件名称");
        ui->label_4->setText("保存路径");
        ui->startDateBtn->setText("选择");
        ui->endDateBtn->setText("选择");
        ui->pushButton->setText("选择");
        ui->exportBtn->setText("导出");
        ui->quitBtn->setText("退出");
    }else{
        ui->groupBox->setTitle("Report generation");
        ui->groupBox_2->setTitle("Record export");
        ui->dayBtn->setText("Daily report");
        ui->monthBtn->setText("Monthly report");
        ui->yearBtn->setText("Annual report");
        ui->label->setText("Starting time");
        ui->label_2->setText("End time");
        ui->label_3->setText("File name");
        ui->label_4->setText("Save route");
        ui->startDateBtn->setText("Choose");
        ui->endDateBtn->setText("Choose");
        ui->pushButton->setText("Choose");
        ui->exportBtn->setText("Export");
        ui->quitBtn->setText("Quit");
    }
}
void LogExportModelDlg::on_startDateBtn_clicked()
{
    BeepThread::bulid()->beep();
    LogCalendarDlg dlg(this);
    int ret = dlg.exec();
    if(ret == QDialog::Accepted) {
        ui->startDateEdit->setDate(dlg.getDate());
    }
}

void LogExportModelDlg::on_endDateBtn_clicked()
{
    BeepThread::bulid()->beep();
    LogCalendarDlg dlg(this);
    int ret = dlg.exec();
    if(ret == QDialog::Accepted) {
        ui->endDateEdit->setDate(dlg.getDate());
    }
}

/**
 * @brief 检查输入
 */
bool LogExportModelDlg::checkInput()
{
    QString str = ui->pathEdit->text();
    if(str.isEmpty()) {
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("导出路径不能为空！"));
        else CriticalMsgBox box(NULL, tr("Export path cannot be empty！"));
        return false;
    }

    str = ui->fileEdit->text();
    if(str.isEmpty()) {
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("导出文件名不能为空！"));
        else CriticalMsgBox box(NULL, tr("The export file name cannot be empty！"));
        return false;
    }

    str = ui->pathEdit->text() + ui->fileEdit->text() +".csv";
    QFile file(str);
    if (file.exists()){
        if(gLanguage == 0) CriticalMsgBox box(NULL, str + tr("\n文件已存在！!"));
        else CriticalMsgBox box(NULL, str + tr("\nFile already exists！!"));
        return false;
    }

    str = ui->pathEdit->text() + ui->fileEdit->text() +".txt";
    QFile file1(str);
    if (file1.exists()){
        if(gLanguage == 0) CriticalMsgBox box(NULL, str + tr("\n文件已存在！!"));
        else CriticalMsgBox box(NULL, str + tr("\nFile already exists！!"));
        return false;
    }

    QDate startDate = ui->startDateEdit->date();
    QDate endDate = ui->endDateEdit->date();
    if(startDate > endDate) {
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("开始日期应早于结束日期，请重新输入日期！"));
        else CriticalMsgBox box(NULL, tr("Start date should be before end date,please re-enter the date！"));
        return false;
    }

    gExcelExportStr->path = ui->pathEdit->text();
    gExcelExportStr->file = ui->fileEdit->text();
    gExcelExportStr->start = ui->startDateEdit->date().toString("yyyy-MM-dd");
    gExcelExportStr->end = ui->endDateEdit->date().toString("yyyy-MM-dd");
    this->accept();

    return true;
}

void LogExportModelDlg::on_exportBtn_clicked()
{
    BeepThread::bulid()->beep();
    bool ret = checkInput();
    if(ret) {
        emit exportSig(0);
    }
}

void LogExportModelDlg::on_dayBtn_clicked()
{
    bool ret = checkInput();
    if(ret) {
        emit exportSig(1);
    }
}

void LogExportModelDlg::on_monthBtn_clicked()
{
    bool ret = checkInput();
    if(ret) {
        emit exportSig(2);
    }
}

void LogExportModelDlg::on_yearBtn_clicked()
{
    bool ret = checkInput();
    if(ret) {
        emit exportSig(3);
    }
}

void LogExportModelDlg::on_quitBtn_clicked()
{
    BeepThread::bulid()->beep();
    this->close();
}

void LogExportModelDlg::on_pushButton_clicked()
{
    BeepThread::bulid()->beep();QFileDialog dlg;
    if(gLanguage == 0) {
        QFileDialog dlg(NULL,tr("路径选择"));
        dlg.setFileMode(QFileDialog::DirectoryOnly);
        QString filepath = "/run/media/sda";
        QDir directory(filepath);
        if(!directory.exists()){
            filepath = "/run/media/sda1";
            QDir directory1(filepath);
            if(!directory1.exists()){
                filepath = "/run/media/sda2";
                QDir directory2(filepath);
                if(!directory2.exists()){
                    return;
                }
            }
        }
        dlg.setDirectory(filepath);
        if(dlg.exec() == QDialog::Accepted) {
            QStringList fileNames = dlg.selectedFiles();
            ui->pathEdit->setText(fileNames.at(0) + "/");
        }
    }else{
        QFileDialog dlg(NULL,tr("Path selection"));
        dlg.setFileMode(QFileDialog::DirectoryOnly);
        QString filepath = "/run/media/sda";
        QDir directory(filepath);
        if(!directory.exists()){
            filepath = "/run/media/sda1";
            QDir directory1(filepath);
            if(!directory1.exists()){
                filepath = "/run/media/sda2";
                QDir directory2(filepath);
                if(!directory2.exists()){
                    return;
                }
            }
        }
        dlg.setDirectory(filepath);
        if(dlg.exec() == QDialog::Accepted) {
            QStringList fileNames = dlg.selectedFiles();
            ui->pathEdit->setText(fileNames.at(0) + "/");
        }
    }
}
