#ifndef SYSTEMSETTINGDLG_H
#define SYSTEMSETTINGDLG_H

#include <QWidget>
#include <cstdlib>
#include <QDateTime>
#include <QDebug>
#include "common.h"
//#include "netsettingdlg.h"
#include "othersettingdlg.h"
//#include "modedlg.h"
#include "localnetsettingdlg.h"

namespace Ui {
class SystemSettingDlg;
}

class SystemSettingDlg : public QWidget
{
    Q_OBJECT

public:
    explicit SystemSettingDlg(QWidget *parent = 0);
    ~SystemSettingDlg();
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
    LocalNetSettingDlg *mLocaNetSettingWid;
};

#endif // SYSTEMSETTINGDLG_H
