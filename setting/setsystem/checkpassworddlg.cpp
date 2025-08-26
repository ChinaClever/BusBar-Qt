#include "checkpassworddlg.h"
#include "ui_checkpassworddlg.h"
//#include "frminput.h"

CheckPasswordDlg::CheckPasswordDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckPasswordDlg)
{
    ui->setupUi(this);
    if(gLanguage == 0) {com_setBackColour(tr("身份验证"), this);
        ui->lineEdit->setPlaceholderText("请输入系统密码确认权限");
    }else{
        com_setBackColour(tr("Authentication"), this);
        ui->lineEdit->setPlaceholderText("Please enter the system password to confirm permissions");
    }

    mNumberKeyboard = new MyNumberKeyboardWid(ui->widget_2);
    QHBoxLayout *layout = new QHBoxLayout(ui->widget_2);
    layout->addWidget(mNumberKeyboard);
    mNumberKeyboard->setCurrentLineEdit(ui->lineEdit);
    setWindowModality(Qt::WindowModal);
}

CheckPasswordDlg::~CheckPasswordDlg()
{
    delete ui;
}

void CheckPasswordDlg::clear()
{
    ui->lineEdit->setText(tr(""));
}

void CheckPasswordDlg::on_pushButton_clicked()
{
    QString str = getPassword();
    QString linestr = ui->lineEdit->text();
//    if( linestr == "reset" || linestr == "RESET" ){
//        QMessageBox msgBox(NULL);
//        if(gLanguage == 0){
//            com_setBackColour(tr("信息提示"),&msgBox);
//            msgBox.setText(tr("\n请确认是否重置密码？\n"));
//            msgBox.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
//            msgBox.setButtonText (QMessageBox::Ok,QString("确 定"));
//            msgBox.setButtonText (QMessageBox::Cancel,QString("取 消"));
//            msgBox.setDefaultButton(QMessageBox::Ok);
//        }else{
//            com_setBackColour(tr("Message notification"),&msgBox);
//            msgBox.setText(tr("\nPlease confirm whether to reset your password？\n"));
//            msgBox.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
//            msgBox.setButtonText (QMessageBox::Ok,QString("Confirm"));
//            msgBox.setButtonText (QMessageBox::Cancel,QString("Cancel"));
//            msgBox.setDefaultButton(QMessageBox::Ok);
//        }
//        int res = msgBox.exec();
//        if(res == QMessageBox::Ok){
//            resetPassword();
//            str = getPassword();
//            if(str.isEmpty()){
//                if(gLanguage == 0) QMessageBox::information(NULL,"information","重置密码成功！","确认");
//                else QMessageBox::information(NULL,"Password reset successful","！","Confirm");
//            }else{
//                if(gLanguage == 0) QMessageBox::information(NULL,"information","重置密码失败！","确认");
//                else QMessageBox::information(NULL,"Password reset failed","！","Confirm");
//            }
//        }
//    }else{
        if(str == linestr)
            mResult = true;
        else
            mResult = false;

        emit dialogClosed(mResult);
//    }
    this->close();
}
