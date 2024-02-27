#ifndef SYSTEMSETTINGDLG_H
#define SYSTEMSETTINGDLG_H

#include <QWidget>
#include <cstdlib>
#include <QDateTime>
#include <QDebug>
#include "common/common.h"
//#include "netsettingdlg.h"
#include "othersettingdlg.h"
//#include "modedlg.h"
#include "localnetsettingdlg.h"
#include "serialdata.h"
#include "autosetaddress.h"
namespace Ui {
class SystemSettingDlg;
}

class SystemSettingDlg : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingDlg(QWidget *parent = 0);
    ~SystemSettingDlg();
    AutoSetAddress *mAutoSetAddress;
    #if (SI_RTUWIFI == 1)
signals:
    void busChangedSig(int);
    #endif
    void initWid();

private:
    Ui::SystemSettingDlg *ui;
//    NetSettingDlg *mNetWid;
    OtherSettingDlg *mOtherWid;
//    IpSettingDlg *mIpWid;
//    ModeDlg *mModeWid;
    Serialdata *mSerialPort;
    LocalNetSettingDlg *mLocaNetSettingWid;
};

#endif // SYSTEMSETTINGDLG_H
