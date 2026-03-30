#ifndef SETMAINWID_H
#define SETMAINWID_H

#include "setnames/setnameswid.h"
#include "setloop/settemmain.h"
#include "setsystem/systemsettingdlg.h"
#include "setsystem/checkpassworddlg.h"
#include "setloop/setpowmain.h"
#include "setloop/setbreakerwid.h"
#include "setloop/settripwid.h"

namespace Ui {
class SetMainWid;
}

class SetMainWid : public QWidget
{
    Q_OBJECT

public:
    explicit SetMainWid(QWidget *parent = 0);
    ~SetMainWid();
    SystemSettingDlg *mSystemDlg;

signals:
    void showAndHideBoxSig(int);

private:
    void initWidget();
    void setButtonColor(QPushButton *button);
    void initLanguage();
public slots:
    void busChangedSlot(int);

private slots:
    void initFunSLot();
    void on_nameBtn_clicked();
    void on_lineBtn_clicked();
    void on_loopBtn_clicked();
    void on_temBtn_clicked();
    void on_sysBtn_clicked();
#if ( SI_RTUWIFI == 1 )
signals:
    void busChangedSig(int);
#endif

    void on_powerBtn_clicked();
    void on_breakerBtn_clicked();

    void on_tripBtn_clicked();

private:
    Ui::SetMainWid *ui;

    SetTemMain *mSetTemWid;
    SetNamesWid *mSetNamesWid;
    SetLineWid *mSetLineWid;
    SetLoopWid *mSetLoopWid;
    SetBreakerWid *mSetBreakerWid;
    SetPowMain *mSetPowerMainWid;
    SetTripWid *mSetTripWid;

};

#endif // SETMAINWID_H
