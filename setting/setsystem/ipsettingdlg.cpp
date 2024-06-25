#include "ipsettingdlg.h"
#include "ui_ipsettingdlg.h"

IpSettingDlg::IpSettingDlg(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::IpSettingDlg)
{
    ui->setupUi(this);
    this->m_index = index;
    initLanguage();
    initData(index);
}

IpSettingDlg::~IpSettingDlg()
{
    delete ui;
}

void IpSettingDlg::initLanguage()
{
    if(gLanguage == 0)
    {
        ui->groupBox->setTitle("Net1设置");
        ui->label->setText("IP地址:");
        ui->label_2->setText("子网掩码:");
        ui->label_3->setText("网关:");
        ui->label_4->setText("DNS:");
        ui->saveBtn->setText("保存");
        ui->cancelBtn->setText("退出");
    }
    else{
        ui->groupBox->setTitle("Net1 settings");
        ui->label->setText("IP Address:");
        ui->label_2->setText("Subnet mask:");
        ui->label_3->setText("Gateway:");
        ui->label_4->setText("DNS:");
        ui->saveBtn->setText("Save");
        ui->cancelBtn->setText("Quit");
    }
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
    if(gLanguage == 0) ui->groupBox->setTitle(tr("Net1网口设置"));
    else ui->groupBox->setTitle(tr("Net1 network port settings"));
    if( 2 == index ){
        str = "11-static-eth1.network";
        if(gLanguage == 0) ui->groupBox->setTitle(tr("Net2网口设置"));
        else ui->groupBox->setTitle(tr("Net2 network port settings"));
    }
    bool ret = sys_configNetFile_open(str);
    if(ret){
        QString str;
        str = sys_configNetFile_readStr("Address");
        QStringList temp = str.split("/");
        m_IP = temp.at(0);
        m_SubnetMask = prefixToSubnetMask(temp.at(1).toInt());
        ui->IPlineEdit->setText(m_IP);
        ui->NetMasklineEdit->setText(m_SubnetMask);
        m_GateWay = sys_configNetFile_readStr("Gateway");
        ui->GatewaylineEdit->setText(m_GateWay);
        m_DNS = sys_configNetFile_readStr("DNS");
        ui->DNSlineEdit->setText(m_DNS);
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
    QString str;
    if(gLanguage == 0) str= tr("检查无误，是否进行修改？");
    else str= tr("Check for accuracy, do you want to modify it？");
    bool ret = true;
    if(ip.isEmpty()){
        if(gLanguage == 0) str = tr("IP地址不能为空 ");
        else str = tr("IP address cannot be empty ");
        ret = false;
    }else{
        if(!ipCheck(ip)){
            if(gLanguage == 0) str = tr("IP地址不合法 ");
            else str = tr("IP address is illegal");

            ret = false;
        }
    }
    if(netmask.isEmpty()){
        if(gLanguage == 0) str += tr("子网掩码地址不能为空 ");
        else str += tr("The subnet mask address cannot be empty ");
        ret = false;
    }else{
        if(!ipSubnetMashValid(netmask)){
            if(gLanguage == 0) str += tr("子网掩码地址不合法 ");
            else str += tr("The subnet mask address is illegal ");
            ret = false;
        }
    }
    if(gateway.isEmpty()){
        if(gLanguage == 0) str += tr("网关地址不能为空 ");
        else str += tr("Gateway address cannot be empty ");
        ret = false;
    }else{
        if(!ipCheck(gateway)){
            if(gLanguage == 0) str += tr("网关地址不合法 ");
            else str += tr("Gateway address is illegal");
            ret = false;
        }
    }
    if(dns.isEmpty()){
        if(gLanguage == 0) str += tr("dns地址不能为空 ");
        else str += tr("DNS address cannot be empty ");
        ret = false;
    }else{
        if(!ipCheck(dns)){
            if(gLanguage == 0) str += tr("dns地址不合法 ");
            else str += tr("DNS address is illegal");
            ret = false;
        }
    }
    return MsgBox::question(NULL, str)&&ret;
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
            insertSystemLog("IP Address" , this->m_IP , ip);
            insertSystemLog("Subnet Mask" , this->m_SubnetMask , netmask);
            insertSystemLog("Gateway" , this->m_GateWay , gateway);
            insertSystemLog("DNS" , this->m_DNS , dns);
        }
        sys_configNetFile_close();
    }

}

void IpSettingDlg::insertSystemLog(const QString &change , const QString &origin , const QString &current)
{
    QString insertStr;
    if(origin != current){
        if(gLanguage == 0)insertStr = tr("把系统Net%1的%4从%2改成%3 !")
                            .arg(this->m_index).arg(origin).arg(current).arg(change);
        else insertStr = tr("Change the %4 of the system Net%1 from %2 to %3 !")
                            .arg(this->m_index).arg(origin).arg(current).arg(change);
        db_system_obj()->insertSystem(insertStr);
    }
}

void IpSettingDlg::on_cancelBtn_clicked()
{
    if( 1 == this->m_index ) emit sendCloseDlgSig1();
    else if( 2 == this->m_index ) emit sendCloseDlgSig2();
    this->close();
}

