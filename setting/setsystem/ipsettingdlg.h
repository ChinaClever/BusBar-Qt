#ifndef IPSETTINGDLG_H
#define IPSETTINGDLG_H

#include <QWidget>
#include "common/common.h"
#include "net/udp/sent/udpheartbeat.h"
#define IP1 "192.168.1.163"

namespace Ui {
class IpSettingDlg;
}

class IpSettingDlg : public QWidget
{
    Q_OBJECT

public:
    explicit IpSettingDlg(QWidget *parent = 0,int index = 1);
    ~IpSettingDlg();
    void initData(int index);

protected:
    bool ipCheck(const QString& isAddress);
    bool ipSubnetMashValid(const QString& subnetMask);
    QString prefixToSubnetMask(int prefix);
    int subnetMaskToPrefix(const QString& subnetMask);
    bool check(const QString& ip , const QString& netmask ,const QString& gateway ,const QString& dns );

private slots:
    void on_saveBtn_clicked();
    void on_cancelBtn_clicked();

signals:
    void sendCloseDlgSig1();
    void sendCloseDlgSig2();

private:
    Ui::IpSettingDlg *ui;
    int m_index;
};
#endif // IPSETTINGDLG_H
