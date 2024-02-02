#ifndef TEMWID_H
#define TEMWID_H

#include "setshm.h"
#include "common/mydoublespinbox.h"
#include "common/comtablewid.h"

class TemWid : public ComTableWid
{
    Q_OBJECT
public:
    explicit TemWid(QWidget *parent = nullptr);
    void setBus(int bus) {mBus=bus;}

protected slots:
    void timeoutDone();

protected:
    void initWid();
    void checkBus(int index);
    int updateDev(sBoxData *dev, int row);
    void updateData();

private:
    int mBus;
    sBusData *mPacket ;
    QTimer* timer;
};

#endif // TEMWID_H
