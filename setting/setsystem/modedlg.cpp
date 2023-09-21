#include "modedlg.h"
#include "ui_ModeDlg.h"
#include <QDebug>

ModeDlg::ModeDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModeDlg)
{
    ui->setupUi(this);
    initMode();

}

ModeDlg::~ModeDlg()
{
    delete ui;
}

void ModeDlg::initMode()
{
    bool ret = sys_configFile_open();
    if(ret)
    {
        int index = sys_configFile_readInt("readmode");
        if( 0 == index ){
            index = 2;
            ui->ModeBox->setCurrentIndex(index-2);
        }else{
            ui->ModeBox->setCurrentIndex(index-2);
        }
    }
    sys_configFile_close();
}

void ModeDlg::on_changeModeBtn_clicked()
{
    int index = ui->ModeBox->currentIndex();
    bool ret = sys_configFile_open();
    if(ret)
    {
        ret = MsgBox::question(this, tr("是否进行通讯模式修改?"));
        if(ret) {
            sys_configFile_write("readmode" , QString::number(index + 2));
        }
    }
    sys_configFile_close();
}

