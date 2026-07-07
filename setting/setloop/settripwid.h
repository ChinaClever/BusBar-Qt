#ifndef SETTRIPWID_H
#define SETTRIPWID_H

#include "common/comtablewid.h"
#include "setshm.h"
#include "setthreshold/setthread.h"
#include "setthreshold/setthresholddlg.h"

class SetTripWid : public ComTableWid
{
    Q_OBJECT
public:
    explicit SetTripWid(QWidget *parent = nullptr);

    void setBus(int bus) {mBus=bus;}

protected slots:
    void timeoutDone();
    void itemClicked(QTableWidgetItem* it);
    void handleButtonClick(int row , int col);
    void recvSetThreadTripSlot(QString mac, int val);

protected:
    void initWid();
    void checkBus(int index);
    int updateDev(sBoxData *dev, int row);
    void updateData();
    QString transformerMac(sBoxData *dev);

private:
    int mDc, mBus;
    sBusData *mPacket ;
    QTimer* timer;
};

#endif // SETTRIPWID_H
