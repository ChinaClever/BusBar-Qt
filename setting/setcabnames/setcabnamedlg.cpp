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
        ui->label->setText("参数修改：");
        //ui->label_2->setText("容量修改：");
        ui->label_3->setText("名称修改：");
        ui->label_4->setText("主路母线编号：");
        ui->label_5->setText("主路插接箱地址：");
        ui->label_6->setText("主路插接箱输出位：");
        ui->label_7->setText("备路母线编号：");
        ui->label_8->setText("备路插接箱地址：");
        ui->label_9->setText("备路插接箱输出位：");
        ui->saveBtn->setText("保存");
        ui->cancelBtn->setText("取消");
    }else {com_setBackColour(tr("Settings interface"), this);
        ui->label->setText("Parameter modification:");
        //ui->label_2->setText("Capacity modification:");
        ui->label_3->setText("Name modification:");
        ui->label_4->setText("Busbar No. of mains:");
        ui->label_5->setText("Tap-off box No. of mains:");
        ui->label_6->setText("Tap-off box output position of mains:");
        ui->label_7->setText("Busbar No. of backup:");
        ui->label_8->setText("Tap-off box No. of backup:");
        ui->label_9->setText("Tap-off box output position of backup:");
        ui->saveBtn->setText("Save");
        ui->cancelBtn->setText("Cancel");
    }

    initBox(ui->lineA_NoBox , 1);
    initBox(ui->lineB_NoBox , 1);
    initBox(ui->lineA_Tapoff_NoBox , 2);
    initBox(ui->lineB_Tapoff_NoBox , 2);
    initBox(ui->lineA_Tapoff_LineBox , 3);
    initBox(ui->lineB_Tapoff_LineBox , 3);
    //ui->label_2->hide();
    //ui->capacitySpinBox->hide();
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

void SetCabNameDlg::init(int bus, int box , SetShm *setShm)
{
    this->mSetShm = setShm;
    mBusId = bus;
    mBox = box;
//    QString str = QString("第 %1 行数据：\n机柜名称 %2\n 电力容量%3\nA路母线编号%4\nA路插接箱地址%5\nA路插接箱输出位%6\nB路母线编号%7\nB路插接箱地址%8\nB路插接箱输出位%9\n")
//                      .arg(box + 1)
//                      .arg(get_share_mem()->cabData[bus][box].cabName)
//                      .arg(get_share_mem()->cabData[bus][box].capacity)
//                      .arg(get_share_mem()->cabData[bus][box].lineA_No)
//                      .arg(get_share_mem()->cabData[bus][box].lineA_Tapoff_No)
//                      .arg(get_share_mem()->cabData[bus][box].lineA_Tapoff_Line)
//                      .arg(get_share_mem()->cabData[bus][box].lineB_No)
//                      .arg(get_share_mem()->cabData[bus][box].lineB_Tapoff_No)
//                      .arg(get_share_mem()->cabData[bus][box].lineB_Tapoff_Line);

//    qDebug()<< " str "<<str;
    ui->nameEdit->setText(get_share_mem()->cabData[bus][box].cabName);
    //ui->capacitySpinBox->setValue(get_share_mem()->cabData[bus][box].capacity/COM_RATE_POW);
    ui->lineA_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineA_No-1);
    ui->lineA_Tapoff_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineA_Tapoff_No-2);
    ui->lineA_Tapoff_LineBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineA_Tapoff_Line-1);
    ui->lineB_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineB_No-1);
    ui->lineB_Tapoff_NoBox->setCurrentIndex(get_share_mem()->cabData[bus][box].lineB_Tapoff_No-2);
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
    QByteArray ba = ui->nameEdit->text().toLatin1();
    char *mm = ba.data();
    strcpy(get_share_mem()->cabData[this->mBusId][this->mBox].cabName,mm);
    //get_share_mem()->cabData[this->mBusId][this->mBox].capacity = ui->capacitySpinBox->value()*COM_RATE_POW;
    get_share_mem()->cabData[this->mBusId][this->mBox].lineA_No = ui->lineA_NoBox->currentIndex()+1;
    get_share_mem()->cabData[this->mBusId][this->mBox].lineA_Tapoff_No = ui->lineA_Tapoff_NoBox->currentIndex()+2;
    get_share_mem()->cabData[this->mBusId][this->mBox].lineA_Tapoff_Line = ui->lineA_Tapoff_LineBox->currentIndex()+1;

    get_share_mem()->cabData[this->mBusId][this->mBox].lineB_No = ui->lineB_NoBox->currentIndex()+1;
    get_share_mem()->cabData[this->mBusId][this->mBox].lineB_Tapoff_No = ui->lineB_Tapoff_NoBox->currentIndex()+2;
    get_share_mem()->cabData[this->mBusId][this->mBox].lineB_Tapoff_Line = ui->lineB_Tapoff_LineBox->currentIndex()+1;
    this->mSetShm->setCabinetParameters(this->mBusId , this->mBox);
    return true;
}

void SetCabNameDlg::on_saveBtn_clicked()
{
    QString str = ui->nameEdit->text();
    if(!str.isEmpty()) {
        if(!(str.size() > NAME_LEN - 2)){
            if(save()){
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
    close();
}

void SetCabNameDlg::on_lineA_NoBox_currentIndexChanged(int index)
{
    ui->lineA_NoBox->setCurrentIndex(index);
}

