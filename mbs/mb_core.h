#ifndef MB_CORE_H
#define MB_CORE_H

#include "mb_update.h"

class Mb_Core : public QThread
{
    Q_OBJECT
    explicit Mb_Core(QObject *parent = nullptr);
public:
    static Mb_Core *build(QObject *parent = nullptr);
    void disconnectRtu() {mRtu->disconnectModbus();}
    void disconnectTcp() {mTcp->disconnectModbus();}
    void setRtu(int parameter, const QVariant &value);
    sModbusSetting *cfg() {return mCfg;}
    static sModbusSetting modbusCfg;
    void setTcpAddress(int addr);
    void setAddress(int addr);
    void setPort(int port);

signals:
    void connectTcpSig();
    void connectRtuSig();

private slots:
    void run();
    void initFunSlot();
    void connectTcpSlot();
    void connectRtuSlot();

private:
    bool isRun=true;
    QTimer *mTimer;
    Mb_Update *mRtu;
    Mb_Update *mTcp;
    sModbusSetting *mCfg;
};

#endif // MB_CORE_H
