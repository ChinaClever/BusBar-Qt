#ifndef LOCALNETSETTINGDLG_H
#define LOCALNETSETTINGDLG_H

#include <QDialog>
#include "ipsettingdlg.h"

namespace Ui {
class LocalNetSettingDlg;
}

class LocalNetSettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LocalNetSettingDlg(QWidget *parent = nullptr);
    ~LocalNetSettingDlg();

private slots:
    void on_Net1Btn_clicked();
    void on_Net2Btn_clicked();

public slots:
    void getCloseDlgSlot1();
    void getCloseDlgSlot2();

private:
    Ui::LocalNetSettingDlg *ui;
    IpSettingDlg *m_net1Dlg;
    IpSettingDlg *m_net2Dlg;
    bool m_netDlgOpen1;
    bool m_netDlgOpen2;
};

#endif // LOCALNETSETTINGDLG_H
