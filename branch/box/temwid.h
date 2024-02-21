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
    void initFun(int bus, int box);

protected slots:
    void timeoutDone();

protected:
    void initWid();
    void checkBus(int index);
    void updateData();

private:
    int mBus;
    sBusData *mPacket ;
    QTimer* timer;
    sEnvData *mEnvData;
    sBoxData *mBox;
};

#endif // TEMWID_H
