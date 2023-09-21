#include "ipsettingdlg.h"
#include "ui_ipsettingdlg.h"

IpSettingDlg::IpSettingDlg(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::IpSettingDlg)
{
    ui->setupUi(this);
    this->m_index = index;
    initData(index);
}

IpSettingDlg::~IpSettingDlg()
{
    delete ui;
}

QString IpSettingDlg::prefixToSubnetMask(int prefix)
{
    quint32 subnetMask = 0xFFFFFFFF << (32 - prefix);
    QString subnetMaskString;

    for (int i = 0; i < 4; ++i) {
        subnetMaskString += QString::number((subnetMask >> (24 - i * 8)) & 0xFF);
        if (i < 3)
            subnetMaskString += ".";
    }

    return subnetMaskString;
}

int IpSettingDlg::subnetMaskToPrefix(const QString& subnetMask)
{
    QHostAddress address(subnetMask);
    quint32 ip = address.toIPv4Address();
    int prefix = 0;

    // 计算连续的1的数量
    while (ip & (1 << 31))
    {
        prefix++;
        ip <<= 1;
    }

    return prefix;
}

void IpSettingDlg::initData(int index)
{
    QString str = "10-static-eth0.network";
    ui->groupBox->setTitle(tr("Net1网口设置"));
    if( 2 == index ){
        str = "11-static-eth1.network";
        ui->groupBox->setTitle(tr("Net2网口设置"));
    }
    bool ret = sys_configNetFile_open(str);
    if(ret){
        QString str;
        str = sys_configNetFile_readStr("Address");
        QStringList temp = str.split("/");
        ui->IPlineEdit->setText(temp.at(0));
        ui->NetMasklineEdit->setText(prefixToSubnetMask(temp.at(1).toInt()));
        str = sys_configNetFile_readStr("Gateway");
        ui->GatewaylineEdit->setText(str);
        str = sys_configNetFile_readStr("DNS");
        ui->DNSlineEdit->setText(str);
    }
    sys_configNetFile_close();
}

/**
 * @brief 子网掩码验证
 * @return
 */
bool IpSettingDlg::ipSubnetMashValid(const QString& subnetMask)
{
    QHostAddress address(subnetMask);
    if(address.protocol() != QAbstractSocket::IPv4Protocol){
        return false;
    }
    quint32 subetMashValue = address.toIPv4Address();
    quint32 invertedMask = ~subetMashValue;
    quint32 normalizeMask = invertedMask + 1;

    return (normalizeMask &(normalizeMask-1)) == 0;
}

/**
 * @brief IP地址验证
 * @return
 */
bool IpSettingDlg::ipCheck(const QString& isAddress)
{
    QHostAddress address(isAddress);
    return address.protocol() != QAbstractSocket::UnknownNetworkLayerProtocol;
}

bool IpSettingDlg::check(const QString& ip , const QString& netmask ,const QString& gateway ,const QString& dns )
{
    QString str = tr("检查无误，是否进行修改？");
    bool ret = true;
    if(ip.isEmpty()){
        str = tr("ip地址不能为空 ");
        ret = false;
    }else{
        if(!ipCheck(ip)){
            str = tr("ip地址不合法 ");
            ret = false;
        }
    }
    if(netmask.isEmpty()){
        str += tr("子网掩码地址不能为空 ");
        ret = false;
    }else{
        if(!ipSubnetMashValid(netmask)){
            str += tr("子网掩码地址不合法 ");
            ret = false;
        }
    }
    if(gateway.isEmpty()){
        str += tr("网关地址不能为空 ");
        ret = false;
    }else{
        if(!ipCheck(gateway)){
            str += tr("网关地址不合法 ");
            ret = false;
        }
    }
    if(dns.isEmpty()){
        str += tr("dns地址不能为空 ");
        ret = false;
    }else{
        if(!ipCheck(dns)){
            str += tr("dns地址不合法 ");
            ret = false;
        }
    }
    return MsgBox::question(this, str)&&ret;
}

void IpSettingDlg::on_saveBtn_clicked()
{
    QString str = "10-static-eth0.network";
    if( 2 == this->m_index )str = "11-static-eth1.network";
    QString ip = ui->IPlineEdit->text();
    QString netmask = ui->NetMasklineEdit->text();
    QString gateway = ui->GatewaylineEdit->text();
    QString dns = ui->DNSlineEdit->text();
    if(check(ip , netmask , gateway , dns)){
        bool ret = sys_configNetFile_open(str);
        if(ret){
            QString str = ip+ "/" + QString::number(subnetMaskToPrefix(netmask));
            sys_configNetFile_write("Address" , str);
            sys_configNetFile_write("Gateway",gateway);
            sys_configNetFile_write("DNS" , dns);
        }
        sys_configNetFile_close();
    }

}

void IpSettingDlg::on_cancelBtn_clicked()
{
    if( 1 == this->m_index ) emit sendCloseDlgSig1();
    else if( 2 == this->m_index ) emit sendCloseDlgSig2();
    this->close();
}

