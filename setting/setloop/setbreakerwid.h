#ifndef SETBREAKERWID_H
#define SETBREAKERWID_H

#include "common/comtablewid.h"
#include "setline/setlinetem.h"
#include "setthreshold/setthread.h"

class SetBreakerWid : public ComTableWid
{
    Q_OBJECT
public:
    explicit SetBreakerWid(QWidget *parent = nullptr);

    void setBus(int bus) {mBus=bus;mPacket = &(get_share_mem()->data[mBus]);}
    bool checkBuzzerStatus(sBusData * packet,int row);

protected slots:
    void timeoutDone();
    void itemClicked(QTableWidgetItem* it);
    void onCheckBoxStateChanged(int state, int row, int col);

protected:
    void initWid();
    void checkBus(int index);
    int updateDev(sBoxData *dev, int row);
    void updateData();

private:
    int mDc, mBus;
    sBusData *mPacket ;
    QTimer* timer;
    sThresholdItem mItem;
};

#endif // SETBREAKERWID_H
