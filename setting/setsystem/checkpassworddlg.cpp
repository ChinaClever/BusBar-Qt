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
    ui->lineEdit->clear();
}

void CheckPasswordDlg::on_pushButton_clicked()
{
    QString str = getPassword();
    QString linestr = ui->lineEdit->text();
    if(str == linestr)
        mResult = true;
    else
        mResult = false;

    emit dialogClosed(mResult);
    this->close();
}
