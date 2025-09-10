#include "setcabnamedlg.h"
#include "ui_setcabnamedlg.h"
#include "common/common.h"
#include "setshm.h"

SetCabNameDlg::SetCabNameDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetCabNameDlg)
{
    ui->setupUi(this);

    if(gLanguage == 0) {com_setBackColour(tr("设置界面"), this);
        ui->label->setText("名称修改");
        ui->label_2->setText("修改");
        ui->label_3->setText("名称");
        ui->saveBtn->setText("保存");
        ui->cancelBtn->setText("取消");
    }else {com_setBackColour(tr("Settings interface"), this);
        ui->label->setText("Name modification");
        ui->label_2->setText("Modify");
        ui->label_3->setText("Name");
        ui->saveBtn->setText("Save");
        ui->cancelBtn->setText("Cancel");
    }

    initBox(ui->lineA_NoBox , 1);
    initBox(ui->lineB_NoBox , 1);
    initBox(ui->lineA_Tapoff_NoBox , 2);
    initBox(ui->lineB_Tapoff_NoBox , 2);
    initBox(ui->lineA_Tapoff_LineBox , 3);
    initBox(ui->lineB_Tapoff_LineBox , 3);
}

SetCabNameDlg::~SetCabNameDlg()
{
    delete ui;
}

void SetCabNameDlg::initBox(QComboBox * box , int type)
{
    QPixmap pix(1,40);
    pix.fill(Qt::transparent);
    QIcon icon(pix);
    box->setIconSize(QSize(1,40));
    int len = 4;
    if(type == 1) len = 4;
    else if(type == 2) len = 18;
    else len = 3;
    for(int i = 0 ; i < len ; i++){
        box->setItemIcon(i , icon);
    }
//    for(int i = 0 ; i < len ; i++){
//        if(type == 2) box->setItemText(i,tr("%1").arg(i+2));
//        else box->setItemText(i,tr("%1").arg(i+1));
//    }
}

void SetCabNameDlg::init(int bus, int box)
{
    mBusId = bus;
    mBox = box;
    QString str = QString("第 %1 行数据：\n机柜名称 %2\n 电力容量%3\nA路母线编号%4\nA路插接箱地址%5\nA路插接箱输出位%6\nB路母线编号%7\nB路插接箱地址%8\nB路插接箱输出位%9\n")
                      .arg(box + 1)
                      .arg(get_share_mem()->cabData[bus][box].cabName)
                      .arg(get_share_mem()->cabData[bus][box].capacity)
                      .arg(get_share_mem()->cabData[bus][box].lineA_No)
                      .arg(get_share_mem()->cabData[bus][box].lineA_Tapoff_No)
                      .arg(get_share_mem()->cabData[bus][box].lineA_Tapoff_Line)
                      .arg(get_share_mem()->cabData[bus][box].lineB_No)
                      .arg(get_share_mem()->cabData[bus][box].lineB_Tapoff_No)
                      .arg(get_share_mem()->cabData[bus][box].lineB_Tapoff_Line);
    ui->nameEdit->setText(get_share_mem()->cabData[bus][box].cabName);
    ui->capacityEdit->setText(QString::number(get_share_mem()->cabData[bus][box].capacity));
    ui->lineA_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineA_No-1);
    ui->lineA_Tapoff_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineA_Tapoff_No-2);
    ui->lineA_Tapoff_LineBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineA_Tapoff_Line-1);
    ui->lineB_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineB_No-1);
    ui->lineA_Tapoff_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineB_Tapoff_No-2);
    ui->lineB_Tapoff_LineBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineB_Tapoff_Line-1);
}

//bool SetCabNameDlg::saveToDev()
//{
//    uchar array[64] = {0};
//    net_dev_data pkt;
//    pkt.num = mBusId; // 母线
//    pkt.addr = mBox; // 插接箱
//    pkt.fn[0] = 6;
//    pkt.fn[1] = mLoop-1; // 相

//    QString name = ui->nameEdit->text();
//    pkt.len = name.size();
//    pkt.data =(uchar*) name.toLocal8Bit().data();
//    int len = net_data_packets(&pkt, array);

//    return SetNetCmd::bulid()->sentNetData(mBusId, array, len);
//}

bool SetCabNameDlg::save()
{
//    int type=2, num = mBox;
//    SetShm shm;

//    DbNameItem item;
//    item.bus = mBusId;
//    if(mLoop) {
//        type = 3;
//        num = mBox*LINE_NUM + mLoop-1 ;
//        //saveToDev();//IP-BUSBAR设置loop名称
//    }

//    item.type = type;
//    item.num = num;
//    item.name = ui->nameEdit->text();
//    const char *temp = item.name.right(1).toLatin1().data();
//    if(((*temp)<'0'||(*temp)>'9') && 2 == item.type){
//        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("名称最后字符不是数字，请重新输入或者是不选择自递增!!"));
//        else CriticalMsgBox box(NULL, tr("The last character of the name is not a number.\nPlease re-enter or do not select auto-increment!!"));
//        return false;
//    }
//    shm.setName(item);
    return true;
}

void SetCabNameDlg::on_saveBtn_clicked()
{
    QString str = ui->nameEdit->text();
    if(!str.isEmpty()) {
        if(!(str.size() > NAME_LEN - 2)){
            if(save()){
                //BeepThread::bulid()->beep();
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

void SetCabNameDlg::on_cancelBtn_clicked()
{
    //BeepThread::bulid()->beep();
    close();
}

void SetCabNameDlg::on_lineA_NoBox_currentIndexChanged(int index)
{
    ui->lineA_NoBox->setCurrentIndex(index);
}

