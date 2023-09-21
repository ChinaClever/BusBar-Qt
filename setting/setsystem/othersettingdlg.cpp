#include "othersettingdlg.h"
#include "ui_othersettingdlg.h"
#include "common.h"
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

    ui->timeSetBtn->setHidden(true);
//    ui->updateBtn->setHidden(true);
}

OtherSettingDlg::~OtherSettingDlg()
{
    delete ui;
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
    BeepThread::bulid()->beep();
    QuMsgBox box(this, tr("是否升级系统?"));
    if(box.Exec()) {
        bool ret = update_fun("sda");
        if(!ret) ret = update_fun("sda1");
        if(!ret) ret = update_fun("sda2");
        if(!ret)
            CriticalMsgBox box(this, tr("升级文件未找到！\n 请插入U盘，把升级文件放入busbar目录下!"));
    }
}

void OtherSettingDlg::on_resetBtn_clicked()
{
    BeepThread::bulid()->beep();
    bool ret = MsgBox::question(this, tr("是否重启系统?"));
    if(ret) {
        system("reboot");
    }
}

void OtherSettingDlg::on_timeSetBtn_clicked()
{
    BeepThread::bulid()->beep();
    TimeSettingDlg dlg(this);
    dlg.exec();
}

void OtherSettingDlg::on_pwdSetBtn_clicked()
{
    BeepThread::bulid()->beep();
    PassordSettingDlg *passwordDlg = new PassordSettingDlg(this);
    passwordDlg->show();
    passwordDlg->move(0,0);
}
