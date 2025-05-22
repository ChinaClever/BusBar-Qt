#include "othersettingdlg.h"
#include "ui_othersettingdlg.h"
#include "common/common.h"
#include "datetime/timesettingdlg.h"
#include "serialport/serial_portset.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if ARM_LINUX  == 2
#define UPDATA_DIR "udisk"
#else
#define UPDATA_DIR "mmcblk0p1"
#endif

OtherSettingDlg::OtherSettingDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OtherSettingDlg)
{
    ui->setupUi(this);
    initLanguage();
    ui->timeSetBtn->setHidden(true);
    ui->ipEdit->setText(gSendIP);
    if(gUser)ui->useBox->setChecked(true);
    else ui->useBox->setChecked(false);
    ui->portEdit->setText(QString::number(gSendport));

    if(gStartAlarm) ui->alramBtn->setText(tr("关闭告警器"));
    else ui->alramBtn->setText(tr("启用告警器"));
//    ui->updateBtn->setHidden(true);
}

OtherSettingDlg::~OtherSettingDlg()
{
    delete ui;
}

void OtherSettingDlg::initLanguage()
{
    if(gLanguage == 0){
        ui->pwdSetBtn->setText("密码修改");
        ui->timeSetBtn->setText("时间修改");
        ui->resetBtn->setText("系统重启");
        ui->updateBtn->setText("软件升级");
        ui->languageBtn->setText("语言设置");
        ui->label->setText("推送数据IP地址");
        ui->label_2->setText("推送数据端口号");
        ui->useBox->setText("是否启用推送数据");
        ui->saveBtn->setText("保存");
    }else{
        ui->pwdSetBtn->setText("Password modifiction");
        ui->timeSetBtn->setText("Time modification");
        ui->resetBtn->setText("System restart");
        ui->updateBtn->setText("Software upgrading");
        ui->languageBtn->setText("Language settings");
        ui->label->setText("IP Address of \npushing data");
        ui->label_2->setText("Port number of \npushing data");
        ui->useBox->setText("Is it enabled pushing data");
        ui->saveBtn->setText("Save");
    }
}

static bool update_fun(const QString &str)
{
    bool ret = true;
    int ans = 0;
    QString cstr;

    cstr = QString("mount | grep %1").arg(str);
    ans = system(cstr.toLatin1());
    if(ans < 0) {
        qDebug() << "mount err";
    }
    QFileInfo fi(QString("/home/root/upgrade.sh"));
    QFileInfo fi1(QString("/run/media/%1/upgrade/busbar").arg(str));
    if(fi.exists() && fi1.exists() ) {

#if ARM_LINUX == 2
        cstr = QString("sh /mnt/%1/busbar/app_start/runMe.sh ").arg(str);
        system(cstr.toLatin1());
#else
        QString filepath = "/home/root/tmp";
        QDir directory(filepath);
        if(!directory.exists()){
            ans = system("mkdir /home/root/tmp");
            if(ans < 0) {
                qDebug() << "mkdir tmp err";
            }
        }

        cstr = QString("cp /run/media/%1/upgrade/busbar /home/root/tmp/busbar").arg(str);
        ans = system(cstr.toLatin1());
        if(ans < 0) {
            qDebug() << cstr+" err";
        }

        ans = system("sync");
        if(ans < 0) {
            qDebug() << "sync err";
        }

        cstr = QString("umount /run/media/%1").arg(str);
        ans = system(cstr.toLatin1());
        if(ans < 0) {
            qDebug() << "umount err";
        }

        ans = system("chmod 755 /home/root/upgrade.sh");
        if(ans < 0) {
            qDebug() << "chmod 755 /home/root/upgrade.sh err";
        }

        ans = system("/home/root/upgrade.sh &");
        if(ans < 0) {
            qDebug() << "/home/root/upgrade.sh &";
        }

//        cstr = QString("cp /mnt/%1/busbar/app /mnt/mtdblock3/app").arg(str);
//        ret = system(cstr.toLatin1());
//        if(ret < 0) {
//            qDebug() << str.toLatin1() << " err ";
//        }

//        sleep(2);
//        system("reboot");
#endif
    } else {
        ret = false;
    }

    return ret;
}

void OtherSettingDlg::on_updateBtn_clicked()
{
    //BeepThread::bulid()->beep();
    if(gLanguage == 0) {
        QuMsgBox box(NULL, tr("是否升级系统?"));
        if(box.Exec()) {
            bool ret = update_fun("sda");
            if(!ret) ret = update_fun("sda1");
            if(!ret) ret = update_fun("sda2");
            if(!ret) ret = update_fun("mmcblk0p1");
            if(!ret)
                CriticalMsgBox box(NULL, tr("升级文件未找到！\n 请插入U盘，把升级文件放入upgrade目录下!"));
        }
    }else{
        QuMsgBox box(NULL, tr("Whether to upgrade the system?"));
        if(box.Exec()) {
            bool ret = update_fun("sda");
            if(!ret) ret = update_fun("sda1");
            if(!ret) ret = update_fun("sda2");
            if(!ret) ret = update_fun("mmcblk0p1");
            if(!ret)
                CriticalMsgBox box(NULL, tr("Upgrade file not found！\n Please insert a USB drive and place the upgrade files in the upgrade directory!"));
        }
    }
}

void OtherSettingDlg::on_resetBtn_clicked()
{
    //BeepThread::bulid()->beep();
    if(gLanguage == 0){
        QuMsgBox box(NULL, tr("是否重启系统?"));
        if(box.Exec()) {
            db_system_obj()->insertSystem(tr("系统重启 !"));
            db_system_obj_en()->insertSystem(tr("System restart !"));
            system("reboot");
        }
    }
    else{
        QuMsgBox box(NULL, tr("Do you want to restart the system?"));
        if(box.Exec()) {
            db_system_obj()->insertSystem(tr("系统重启 !"));
            db_system_obj_en()->insertSystem(tr("System restart !"));
            system("reboot");
        }
    }

}

void OtherSettingDlg::on_timeSetBtn_clicked()
{
    //BeepThread::bulid()->beep();
    TimeSettingDlg dlg(this);
    dlg.exec();
}

void OtherSettingDlg::on_pwdSetBtn_clicked()
{
    //BeepThread::bulid()->beep();
    PassordSettingDlg *passwordDlg = new PassordSettingDlg(this);
    passwordDlg->setWindowModality(Qt::WindowModal);
    passwordDlg->show();
    passwordDlg->move(0,0);
}

void OtherSettingDlg::on_languageBtn_clicked()
{
    //BeepThread::bulid()->beep();
    mlanguage = new Languagesetting(this);
    mlanguage->setWindowModality(Qt::WindowModal);
    mlanguage->show();
    mlanguage->move(368,222);
}


void OtherSettingDlg::on_saveBtn_clicked()
{
    gSendIP = ui->ipEdit->text();
    gSendport = ui->portEdit->text().toInt();
    gUser = ui->useBox->isChecked()?1:0;
    if(!gSendIP.isEmpty()&&!ui->portEdit->text().isEmpty()){

        sys_configFile_writeParam("SendIP",gSendIP);
        sys_configFile_writeParam("Sendport",QString::number(gSendport));
        sys_configFile_writeParam("Senduse",QString::number(gUser));
        //qDebug()<<"user"<<user;
        if(gLanguage == 0) { InfoMsgBox box(NULL, tr("保存成功！"));}
        else InfoMsgBox box(NULL, tr("Save successfully！"));
    }
}

void OtherSettingDlg::on_alramBtn_clicked()
{
    if(gStartAlarm){
        gStartAlarm = 0;
        ui->alramBtn->setText(tr("关闭告警器"));
        sys_configFile_writeParam("startalarm",QString::number(gStartAlarm));
    }else{
        gStartAlarm = 1;
        ui->alramBtn->setText(tr("启用告警器"));
        sys_configFile_writeParam("startalarm",QString::number(gStartAlarm));
    }
}

