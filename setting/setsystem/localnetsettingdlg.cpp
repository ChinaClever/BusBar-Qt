#include "localnetsettingdlg.h"
#include "ui_localnetsettingdlg.h"

LocalNetSettingDlg::LocalNetSettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LocalNetSettingDlg)
{
    ui->setupUi(this);
    m_netDlgOpen1 = false;
    m_netDlgOpen2 = false;
    m_net1Dlg = new IpSettingDlg(NULL,1);
    connect(m_net1Dlg , SIGNAL(sendCloseDlgSig1()) , this , SLOT(getCloseDlgSlot1()));
    m_net2Dlg = new IpSettingDlg(NULL,2);
    connect(m_net2Dlg , SIGNAL(sendCloseDlgSig2()) , this , SLOT(getCloseDlgSlot2()));
}

LocalNetSettingDlg::~LocalNetSettingDlg()
{
    delete ui;
}

void LocalNetSettingDlg::on_Net1Btn_clicked()
{
    if(!m_netDlgOpen1){
        m_net1Dlg->initData(1);
        m_net1Dlg->setWindowModality(Qt::NonModal);
        m_net1Dlg->show();
        m_net1Dlg->move(0,0);
        m_netDlgOpen1 = true;
    }
}


void LocalNetSettingDlg::on_Net2Btn_clicked()
{
    if(!m_netDlgOpen2){
        m_net2Dlg->initData(2);
        m_net2Dlg->setWindowModality(Qt::NonModal);
        m_net2Dlg->show();
        m_net2Dlg->move(0,0);
        m_netDlgOpen2 = true;
    }
}

void LocalNetSettingDlg::getCloseDlgSlot1()
{
    m_netDlgOpen1 = false;
}

void LocalNetSettingDlg::getCloseDlgSlot2()
{
    m_netDlgOpen2 = false;
}

