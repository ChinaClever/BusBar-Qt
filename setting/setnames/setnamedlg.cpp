#include "setnamedlg.h"
#include "ui_setnamedlg.h"
#include "common/common.h"
#include "setshm.h"
#include "setthreshold/setnetcmd.h"

extern int net_data_packets(net_dev_data *pkt, uchar *buf);

SetNameDlg::SetNameDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetNameDlg)
{
    ui->setupUi(this);

    if(gLanguage == 0) {com_setBackColour(tr("设置界面"), this);
        ui->label->setText("名称修改");
        ui->label_2->setText("修改");
        ui->label_3->setText("名称");
        ui->saveBtn->setText("保存");
        ui->cancelBtn->setText("取消");
        ui->checkBox->setText("递增");
    }else {com_setBackColour(tr("Settings interface"), this);
        ui->label->setText("Name modification");
        ui->label_2->setText("Modify");
        ui->label_3->setText("Name");
        ui->saveBtn->setText("Save");
        ui->cancelBtn->setText("Cancel");
        ui->checkBox->setText("Increment");
    }
}

SetNameDlg::~SetNameDlg()
{
    delete ui;
}

void SetNameDlg::init(int bus, int box, int loop, const QString &name)
{
    mBusId = bus;
    mBox = box;
    mLoop = loop;
    ui->nameLab->setText(name);
    if(box == 1 && loop == 0){
        ui->checkBox->show();
    }else{
        ui->checkBox->hide();
    }
}

bool SetNameDlg::saveToDev()
{
    uchar array[64] = {0};
    net_dev_data pkt;
    pkt.num = mBusId; // 母线
    pkt.addr = mBox; // 插接箱
    pkt.fn[0] = 6;
    pkt.fn[1] = mLoop-1; // 相

    QString name = ui->nameEdit->text();
    pkt.len = name.size();
    pkt.data =(uchar*) name.toLocal8Bit().data();
    int len = net_data_packets(&pkt, array);

    return SetNetCmd::bulid()->sentNetData(mBusId, array, len);
}

bool SetNameDlg::save()
{
    int type=2, num = mBox;
    SetShm shm;

    DbNameItem item;
    item.bus = mBusId;
    if(mLoop) {
        type = 3;
        num = mBox*LINE_NUM + mLoop-1 ;
        saveToDev();//IP-BUSBAR设置loop名称
    }

    item.type = type;
    item.num = num;
    item.name = ui->nameEdit->text();
    item.increment = 0;
    if(ui->checkBox->isChecked()) item.increment = 1;
    const char *temp = item.name.right(1).toLatin1().data();
    if(((*temp)<'0'||(*temp)>'9') && ui->checkBox->isChecked() && 1 == item.num && 2 == item.type){
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("名称最后字符不是数字，请重新输入或者是不选择自递增!!"));
        else CriticalMsgBox box(NULL, tr("The last character of the name is not a number.\nPlease re-enter or do not select auto-increment!!"));
        return false;
    }
    shm.setName(item);
    return true;
}

void SetNameDlg::on_saveBtn_clicked()
{
    QString str = ui->nameEdit->text();
    if(!str.isEmpty()) {
        if(!(str.size() > NAME_LEN - 2)){
            if(save()){
                BeepThread::bulid()->beep();
                close();
            }
        } else {
            if(gLanguage == 0) CriticalMsgBox box(NULL, tr("名称不能超过30个字符!!"));
            else CriticalMsgBox box(NULL, tr("Name cannot exceed 30 characters!!"));
        }
    } else {
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("名称不能为空!!"));
        else CriticalMsgBox box(NULL, tr("Name cannot be empty!!"));
    }
}

void SetNameDlg::on_cancelBtn_clicked()
{
    BeepThread::bulid()->beep();
    close();
}
